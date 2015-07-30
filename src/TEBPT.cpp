/*
 * TEBPT.cpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 15/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

// Include config.h first, for configuration of included files
#include "TEBPT_config.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>
#include <ctime>
#include <unistd.h>
#include <armadillo>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/types.h>
#include <sys/stat.h>

// Static tests (not needed, only for testing)
#include "test/StaticTests.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif


// Sub matrix size is defined in config.h file
// Region model is defined in config.h file

using namespace std;
using namespace tscbpt;
using namespace arma;

void printUsage(){
	cerr << "Usage:\n    TEBPT [options] pruning_factor(s) rows cols file1 [file2 ... fileN]" << endl;
	cerr << "\nOptions include:" << endl;
	cerr << "  --out outpath    Change the output path to outpath (default: '.')" << endl;
	cerr << "  --bpt file       Read the BPT merging sequence from the given file" << endl;
	cerr << "  --nl rows cols   Apply an initial multilook filter of size rows x cols" << endl;
	cerr << "  --bl rows cols   Apply an initial bilateral filter of size rows x cols" << endl;
	cerr << "  --cut start_row start_col height width   Process only a cut of the input data" << endl;
	cerr << "  --blf-sigma_s, --blf-sigma_p, --blf-sigma_t, --blf-iterations value" << endl;
	cerr << "                   Change the corresponding bilateral filtering parameter" << endl;
	cerr << "  --no-ts          Do not compute temporal stability measures" << endl;
	cerr << "  --no-write       Do not write pruned images data" << endl;
	cerr << "  --dist-all       Generate distance images between all pairs of acquisitions" << endl;
	cerr << endl;
}

double diffclock(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - clock2;
	double diffms = (diffticks * 1000) / CLOCKS_PER_SEC;
	return diffms;
}

struct printRegIdTo
{
	printRegIdTo(ofstream & astream) : stream(astream){}

	template<typename T>
	void operator()(const T np){
		BPT::NodeID in = static_cast<int>(np->getId());
		stream.write(reinterpret_cast<const char*> (&in), sizeof(in));
	}
private:
	ofstream & stream;
};

struct ModelAccessor : public unary_function< BPT::NodePointer, BPT::Node::RegionModel::covariance_type > {
	typedef BPT::NodePointer 							NodePointer;
	typedef BPT::Node::RegionModel::covariance_type		covariance_type;
	covariance_type& operator()(NodePointer a) const {
		return (a->getModel().getFullCovariance());
	}
};

template<typename T>
struct printValueTo
{
	printValueTo(ofstream & astream) : stream(astream){}

	void operator()(const T& value) {
		stream.write(reinterpret_cast<const char*> (&value), sizeof(T));
	}
private:
	ofstream & stream;
};


int main(int argc, char** argv) {
	#ifdef _OPENMP
	cout << "Potential parallel threads: " << omp_get_max_threads() << endl;
	#endif

	// Maximum number of allowed prunes per execution
	static const size_t MAX_PRUNES		= 100;

	double startPF=0, endPF=0, incPF = 1;
	clock_t start;
	size_t rows=0, cols=0;
	vector<string>	files;
	string bptFile;
	string outPath = ".";
	size_t nlr=3, nlc=3;
	size_t crop_sr = 1, crop_sc = 1, crop_height = 0, crop_with = 0;
	bool nl_filtering = true;
	bool bl_filtering = false;
	bool gen_dist_pairs = false;
	bool gen_ts = true;
	bool write_prune = true;
	bool swap_endianness = false;
	double blf_sigma_p = 0.5;
	double blf_sigma_s = 2;
	double blf_sigma_t = -1;
	size_t blf_iterations = 3;

	// Ensure the number of arguments is correct
	if(argc > 4){
		// Reader to read all the files into a vector
		typedef MultiFileWithSizeReader<complex<float> >		Matrix2DReader;
		// Wrapper to change basis according to SOperator (in _config.h)
		typedef SourceWrapper<Matrix2DReader, SOperator, std::vector<complex<float> > > SVector2DReader;
		// Source to work only with a crop of the complete data
		typedef SourceCutter<SVector2DReader>					SCutter;

		// Read input arguments
		int argi;
		for(argi = 1; argi < argc && strncmp(argv[argi],"--",2) == 0; argi++){
			if(strcmp(argv[argi],"--bpt")==0 && argi+1 < argc){
				bptFile = argv[++argi];
			}else if(strcmp(argv[argi],"--nl")==0 && argi+2 < argc) {
				nl_filtering = true;
				bl_filtering = false;
				nlr = atol(argv[++argi]);
				nlc = atol(argv[++argi]);
				assert(nlr > 0 && nlc > 0);
			}else if(strcmp(argv[argi],"--bl")==0 && argi+2 < argc) {
				nl_filtering = false;
				bl_filtering = true;
				nlr = atol(argv[++argi]);
				nlc = atol(argv[++argi]);
				assert(nlr > 0 && nlc > 0);
			} else if (strcmp(argv[argi], "--cut") == 0 && argi + 4 < argc) {
				crop_sr = atol(argv[++argi]);
				crop_sc = atol(argv[++argi]);
				crop_height = atol(argv[++argi]);
				crop_with = atol(argv[++argi]);
				assert(crop_height > 0 && crop_with > 0);
			} else if(strcmp(argv[argi],"--blf-sigma_s")==0 && argi+1 < argc){
				blf_sigma_s = atof(argv[++argi]);
			} else if(strcmp(argv[argi],"--blf-sigma_p")==0 && argi+1 < argc){
				blf_sigma_p = atof(argv[++argi]);
			} else if(strcmp(argv[argi],"--blf-sigma_t")==0 && argi+1 < argc){
				blf_sigma_t = atof(argv[++argi]);
			} else if(strcmp(argv[argi],"--blf-iterations")==0 && argi+1 < argc){
				blf_iterations = atol(argv[++argi]);
			} else if (strcmp(argv[argi], "--out") == 0 && argi+1 < argc) {
				outPath = (argv[++argi]);
			} else if (strcmp(argv[argi], "--dist-all") == 0) {
				gen_dist_pairs = true;
			} else if (strcmp(argv[argi], "--no-ts") == 0) {
				gen_ts = false;
			} else if (strcmp(argv[argi], "--no-write") == 0) {
				write_prune = false;
			} else if (strcmp(argv[argi], "--swap-endian") == 0) {
				swap_endianness = true;
			}else{
				cerr << "ERROR: Unknown parameter '" << argv[argi] << "'" << endl;
				printUsage();
				exit(-1);
			}
		}

		// Change working directory
		if(chdir(outPath.c_str())) cerr << "ERROR: Cannot change current working directory to " << outPath << endl;
		else cout << "Changed output directory to '" << outPath << "'" << endl;

		// Process the pruning factors interval
		std::vector<std::string> strs;
		boost::split(strs, argv[argi++], boost::is_any_of(":"));
		if(strs.size()==1)
			startPF = endPF = atof(strs.front().c_str());
		else if(strs.size()==3){
			startPF = atof(strs[0].c_str());
			incPF = atof(strs[1].c_str());
			endPF = atof(strs[2].c_str());
			assert(startPF <= endPF);
			assert(incPF > 0.0);
			assert((endPF-startPF)/incPF < MAX_PRUNES);
		}else{
			cerr << "Unable to understand prune factor value or range." << endl;
			cerr << "Use a fixed value ('-1.5') or a range ('-5:1:0')" << endl;
			return EXIT_FAILURE;
		}

		// Read rows and cols
		rows = static_cast<size_t>(atol(argv[argi++]));
		cols = static_cast<size_t>(atol(argv[argi++]));

		// Read all the remaining arguments as input files
		strs.clear();
		for(int i = argi; i< argc; ++i){
			strs.push_back(string(argv[i]));
		}

		// Construct the reader for all the files
		Matrix2DReader fileReader = (rows != 0 && cols != 0)?
			Matrix2DReader(&(strs[0]), strs.size(), rows, cols, swap_endianness) :
			Matrix2DReader(&(strs[0]), strs.size(), swap_endianness);
		// Apply the scattering vector basis change to the previous reader
		SVector2DReader	seriesReader(fileReader);

		// Apply the source cutter to a crop of the data...
		SCutter *cutter;
		if(crop_height > 0 && crop_with > 0){
			cutter = new SCutter(seriesReader, crop_sr, crop_sc, crop_height, crop_with);
			rows = crop_height;
			cols = crop_with;
		}else{	// ... or take as a crop the complete dataset if not specified
			cutter = new SCutter(seriesReader, 0, 0, seriesReader.getRows(), seriesReader.getCols());
			rows = seriesReader.getRows();
			cols = seriesReader.getCols();
		}

		cout << "Dataset size [pixels]: " << rows << " x " << cols << endl;

		// Define the dissimilarity measure employed --> in _config.h file
		Dissimilarity		diss = Dissimilarity();

		// Initialize the Weighted Region Adjacency Graph generator
		DenseWRAGGenerator<BPT::Node> wrag(
			make_pixel_iterator2D(cutter->begin()),
			make_pixel_iterator2D(cutter->end()),
			rows, cols);

		cout << "Model matrix size: " << wrag.getData()[0]->getModel().getMatrixSize() << " stored into matrix(ces) of size " << wrag.getData()[0]->getModel().getCovariance(0).getCols() << endl;

		// Apply the initial filtering, either Multilook (Boxcar)...
		// NOTE: To disable it a 1x1 multilook may be applied
		// ======================================== Multilook filter =============================================
		if(nl_filtering){
			cout << "\nPerforming Boxcar " << nlr << "x" << nlc << " spatial filtering... " << flush;
			start = clock();
			boxCarFilter2DFullInterp(wrag.getData().begin(), rows, cols, nlr, nlc);
			cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
		}
		// ====================================== End Multilook filter ===========================================


		// ... or Distance Based Bilateral filter, as defined in:
		// Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
		// Bilateral Distance Based Filtering for Polarimetric SAR Data.
		// Remote Sens. 2013, 5, 5620-5641.
		// ======================================== Bilateral filter =============================================
		if(bl_filtering){
			ImageData<double> 	k_img(rows, cols);

			if(blf_sigma_t < 0){
				cout << "Calculating automatically the sigma_t parameter (10x10 blocks)..." << endl;
				blf_sigma_t = compute_sigma_t(make_LinearAccessor(wrag.getData(), cols), rows, cols, ModelAccessor(), 10);
			}

			cout << "\nPerforming Bilateral " << nlr << "x" << nlc << " spatial filtering:\n  iterations:\t" << blf_iterations <<
					"\n  sigma_s:\t" << blf_sigma_s << "\n  sigma_p:\t" << blf_sigma_p << "\n  sigma_t:\t" << blf_sigma_t << endl;

			// Distance employed in the filter
//			BLFGeodesicDissExp<BPT::RegionModel::covariance_type> 		bfdiss(blf_sigma_t);
			BLFDiagonalWishartDiss<BPT::RegionModel::covariance_type> 	bfdiss(blf_sigma_t);

			{	// Scope to automatically delete the ProgressDisplay on exit
			ProgressDisplay show_progress(1);
			iterativeCrossBilateralDBF2Filter(
					make_LinearAccessor(wrag.getData(), cols),	// in
					make_LinearAccessor(wrag.getData(), cols),	// ref
					make_LinearAccessor(wrag.getData(), cols),	// out
					k_img,
					bfdiss, rows, cols, nlr, nlc, blf_sigma_s, blf_sigma_p, blf_iterations, ModelAccessor());
			}

			// Saving k parameter
			cout << "Saving the k parameter for the last iteration of bilateral filtering... " << flush;
			start = clock();
			ofstream k_stream(string("k.bin").c_str());
			if (k_stream.fail()) cerr << "ERROR: the 'k.bin' file cannot be opened for writing!" << endl;
			// Dissimilarity for temporal stability calculation
			for_each(k_img.begin(), k_img.end(), printValueTo<double>(k_stream));
			// Close file
			k_stream.close();
			cout << "Done. (Elapsed " << diffclock(clock(), start)	<< " milliseconds)" << endl;
		}
		// ====================================== End Bilateral filter ===========================================

		// Pointer to contain the root node
		BPT::WeakNodePointer root;

		// Construct the BPT
		if(bptFile.size() == 0){
			// If the merging sequence has not been provided
			// Generate WRAG and BPT
			cout << "\nGenerating WRAG... " << flush;
			start = clock();
			wrag.generateWRAG_2D_Connectivity8<Dissimilarity, BPT::Dissimilarity> (diss);
			cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
			cout << "  Number of Nodes existing: " << BPT::NodeStoragePolicy::balance << endl;
			cout << "  Number of Dissimilarities existing: " << BPT::DissimilarityStoragePolicy::balance << endl;

			cout << "\nGenerating the BPT representation..." << flush;
			start = clock();
			BPT::Constructor constructor(wrag.begin(), wrag.end());
			BPT::NodeSet consSet = constructor.getBinaryPartitionForest<
					Dissimilarity, ModelMerge > (1, diss);
			cout << "BPT created. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
			cout << "Number of Nodes existing: " << BPT::NodeStoragePolicy::balance << endl;
			cout << "Number of Dissimilarities existing: " << BPT::DissimilarityStoragePolicy::balance << endl;

			root = *(consSet.begin());
		}else{
			// If the merging sequence has been provided
			// ReGenerate BPT (much faster, no dissimilarity computation)
			ifstream msFile(bptFile.c_str());
			cout << "\nRegenerating the BPT representation..." << flush;
			start = clock();
			BPT::Reconstructor reconstructor(wrag.begin(), wrag.end());
			BPT::NodeSet consSet = reconstructor.getBinaryPartitionForest<ModelMerge > (msFile, 1);
			cout << "BPT created. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
			cout << "Number of Nodes existing: " << BPT::NodeStoragePolicy::balance << endl;
			cout << "Number of Dissimilarities existing: " << BPT::DissimilarityStoragePolicy::balance << endl;
			msFile.close();

			root = *(consSet.begin());
		}

		// Set to contain the pruned nodes
		BPT::NodeSet prunedSet;

		// Print the TotalSumOfSquares of the root node
//		cout << "Root Node TSS: \t" << root->getModel().getTotalSumOfSquares() << endl;

		// Start BPT pruning processes, for each prune factor
		for(double pruneFactor = startPF; pruneFactor <= endPF; pruneFactor += incPF){
			cout << "\nPruning BPT at " << pruneFactor << " dB ... " << flush;
			start = clock();

			// Prune the BPT
			// NOTE: PruneCriterion defined in _config.h
			BPT::prune(root, prunedSet, PruneCriterion(pruneFactor));

			cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
			cout << "  Number of pruned regions: " << prunedSet.size() << endl;
			cout << "  Average region size: " << static_cast<double>(rows) * cols / prunedSet.size() << endl;

			cout << "Generating image from pruned tree... " << flush;
			start = clock();

			// Generate a image source from the set of pruned nodes
			BPTDataSource<BPT::NodePointer, 2>		source(prunedSet, wrag.getDimensions().begin());
			cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;

			string dir = string("./Prune_") + to_string(pruneFactor);
			{	// Creating the directory path dir
				struct stat fstat;
				if (stat(dir.c_str(), &fstat) != 0){
					cout << "\n  Creating dir " << dir.c_str() << endl;
					assert(mkdir(dir.c_str(), S_IRWXU)==0);
				}
			}
			dir += OUTPUT_FOLDER;
			{ // Creating the directory path dir
				struct stat fstat;
				if (stat(dir.c_str(), &fstat) != 0){
					cout << "  Creating dir " << dir.c_str() << endl;
					assert(mkdir(dir.c_str(), S_IRWXU)==0);
				}
			}

			if(write_prune){
				cout << "Writing sequence data... " << flush;
				start = clock();

				// Save the whole matrix (Only use this with DynamicMatrixModel)
//				PolSARProFormatMatrixWriter<> writer;
				// Save only polarimetric submatrices (Use with VectorMatrixModel)
				PolSARProFormatVectorMatrixWriter<SUBMATRIX_SIZE> writer;

				writer.writeDataToDir(dir, source.begin(), source.end(), string("C"));
				writer.writeConfigFile(dir, rows, cols);
				cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;

				cout << "Generating region ID data... " << flush;
				start = clock();
				ofstream RIDFile ((dir + "/RegId.bin").c_str());
				if(RIDFile.fail()) cerr<<"ERROR: region ID file cannot be opened!"<<endl;
				for_each(source.begin(), source.end(), printRegIdTo(RIDFile));
				RIDFile.close();
				cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
			}

			// Only generate temporal stability data if NumCovariances() > 1
			if(gen_ts && root->getModel().getNumCovariances() > 1){
				// Temporal stability with full matrix geodesic measure
				cout << "Generating region temporal stability data (GEIGs_Full)... " << flush;
				start = clock();
				ofstream TSFileGEIG_full((dir + "/TStability_GEIGs_full.bin").c_str());
				if (TSFileGEIG_full.fail())
					cerr << "ERROR: temporal stability file cannot be opened!" << endl;

				// Dissimilarity for temporal stability calculation
				typedef GeodesicVectorChangeMeasureFull<BPT::NodePointer, double>			stab_func_GEIG_full;
				{ ProgressDisplay progress(1);
				BPTDataSource<BPT::NodePointer, 2, stab_func_GEIG_full>		TSsourceGEIG_full(prunedSet, wrag.getDimensions().begin());
				for_each(TSsourceGEIG_full.begin(), TSsourceGEIG_full.end(), printValueTo<double>(TSFileGEIG_full));
				// Close file
				TSFileGEIG_full.close(); }
				cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;

				// Temporal stability with diagonal geodesic measure
				cout << "Generating region temporal stability data (DGs_Full)... " << flush;
				start = clock();
				ofstream TSFileDG_full((dir + "/TStability_DGs_full.bin").c_str());
				if (TSFileDG_full.fail())
					cerr << "ERROR: temporal stability file cannot be opened!" << endl;

				// Dissimilarity for temporal stability calculation
				typedef DiagonalGeodesicVectorChangeMeasureFull<BPT::NodePointer, double>	stab_func_full;
				BPTDataSource < BPT::NodePointer, 2, stab_func_full > TSsourceDG_full(
						prunedSet, wrag.getDimensions().begin());
				for_each(TSsourceDG_full.begin(), TSsourceDG_full.end(),
						printValueTo<double> (TSFileDG_full));
				// Close file
				TSFileDG_full.close();
				cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;

				// Generate change images for each acquisition pair if enabled
				// NOTE: May be a large number of images
				if(gen_dist_pairs){
					cout << "Generating all change images pairs (GEIG)... " << endl;
					start = clock();
					for(size_t i = 0; i < wrag.getData()[0]->getModel().getNumCovariances(); ++i){
						for(size_t j = i+1; j < wrag.getData()[0]->getModel().getNumCovariances(); ++j){
							cout << "\tGenerating pair " << i << " -> " << j << " ..." << endl;
							ofstream DPFile((dir + string("/DP_GEIG_") + to_string(i) + string("_") + to_string(j) + ".bin").c_str());
							if (DPFile.fail())
								cerr << "ERROR: temporal stability file cannot be opened!" << endl;
							// Dissimilarity to calculate
							typedef GeodesicVectorPairChangeMeasure<BPT::NodePointer, double>	ch_meas;
							ch_meas measure = ch_meas(i,j);
							BPTDataSource < BPT::NodePointer, 2, ch_meas > DPSource(
									prunedSet, wrag.getDimensions().begin(), measure);
							for_each(DPSource.begin(), DPSource.end(), printValueTo<double> (DPFile));
							// Close file
							DPFile.close();
						}
					}
					cout << "Done. (Elapsed " << diffclock(clock(), start) << " milliseconds)" << endl;
				}
			}

			// Clear the pruned set
			prunedSet.clear();
		}
	} else {
		printUsage();
	}
}

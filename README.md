TSCBPT: Binary Partition Tree for Polarimetric SAR data.
========================================================

This repository contains part of the C++ software developed during my PhD thesis:

Alonso-Gonzalez, Alberto. [_Multidimensional and temporal SAR data representation and processing based on binary partition trees_](http://www.tdx.cat/handle/10803/146132), PhD Thesis, May 2014

The software contains the code that implements the Binary Partition Tree technique for processing Polarimetric SAR data, as described in [1].
Additionally, the command line tool is able to process several acquisitions, in order to perform the time series BPT-based processing described in [2].
Finally, the tool also implements the Distance Based Bilateral filering technique, as described in [3], in order to filter & regularize covariance matrices and preserve the spatial resolution.

[1] Alonso-Gonzalez, A.; Lopez-Martinez, C.; Salembier, P., **Filtering and Segmentation of Polarimetric SAR Data Based on Binary Partition Trees**, _Geoscience and Remote Sensing, IEEE Transactions on_, vol.50, no.2, pp.593, 605, Feb. 2012

[2] Alonso-Gonzalez, A.; Lopez-Martinez, C.; Salembier, P., **PolSAR Time Series Processing with Binary Partition Trees**, _Geoscience and Remote Sensing, IEEE Transactions on_, vol.52, no.6, pp.3553, 3567, June 2014

[3] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X., [**Bilateral Distance Based Filtering for Polarimetric SAR Data**](http://www.mdpi.com/2072-4292/5/11/5620), _Remote Sensing_, 2013, 5, 5620-5641.

- If you use this code in a scientific work, publication or public presentation, please cite the appropiate references above.

Compilation / Installation
--------------------------

- To get the complete software you can download the zip file from the project page or you can clone the project with `git` (requires the program to be installed):

```
 git clone https://github.com/aalgo/TSCBPT.git
```

- The software requires the [Armadillo C++ library](http://arma.sourceforge.net/) for linear algebra and matrix computations. It is available in most of the linux distributions. On Ubuntu/Debian, for instance, it may be installed with the apt package manager:
```bash
$ sudo apt-get install libarmadillo-dev
```
Note the `dev` suffix. It is required since the C++ headers libs need also to be installed!

- Additionally, a small subset of the [C++ Boost](http://www.boost.org/) libraries is employed. On Ubuntu/Debian it may be installed with:
```bash
$ sudo apt-get install libboost-dev
```

- To compile the code, execute the `make` command on the project folder (the `TSCBPT` folder).

- The executable files will be placed within the `bin` folder. For instance, to execute the `TEBPT` command line program:
```bash
$ bin/TEBPT
```

- **NOTE:** The software has only been tested under Linux with the GCC compiler.


TSCBPT C++ library
------------------

The repository contains a set of C++ header libraries to perform the Binary Partition Tree (BPT) processing mentioned in the previous references.
The libraries are located within the `include` folder.
They are "header only" libraries, based on generic template classes.

As an utility and as a usage example, a file called `TEBPT.cpp` is included within the `src` folder. It is employed to build the `TEBPT` command line tool to process time series PolSAR data according to [2].


### Usage of the TEBPT tool

The `TEBPT` tool is a command line tool that may be executed with the syntax:
```bash
$ bin/TEBPT [options] pruning_factors rows cols file1 [file2 ... fileN]
```
- With respect to the input files & format, it is important to nottice:
  * The input files are in binary format (complex float) with no header. Alternatively, files with a two 32bit integer header containing the rows and cols size information are allowed. In this case, specify 0 0 as rows and cols in the command line and the tool will read this data from the files.
  * NOTE: The binary endian of the input files can be switched with the `--swap-endian` option.
  * The files are assumed to be a sequence of (HH, HV, VH, VV) binary files. Then a number of files multiple of 4 is expected.
  * The files are converted to C3 covariance matrices and the results are stored in Prune_N folders (where N stands for the prune factor, in dB) in [PolSARPro](http://earth.eo.esa.int/polsarpro/) format. In fact they can be opened with this software for visualization and processing.

- The `rows` and `cols` parameters represent the size of the input files.

- The pruning factor may be a number (e.g. `-2`) or a sequence of numbers, specified as `start:inc:end` (e.g. `-3:0.5:0`). If a sequence is given, all the different prunes will be generated after BPT construction, which is much faster and efficient than with different executions of the tool.
- The different options include:
  * `--out outpath` Changes the output directory to given one. Otherwise the results are written in the currect folder.
  * `--cut start_row start_col height width`   Process only the specified crop of the input data.
  * `--bpt file` Read the BPT merging sequence from the given file. Once the BPT has been constructed for a dataset, the merging sequence file `BPT.msq` is generated. With this, the later BPT reconstruction may be performed much faster, as there is no need for the computation of the adjacency graph and the similarity measures.
  * `--nl rows cols` Apply a multillok as initial filtering of the given size rows by cols.
  * `--bl rows cols` Apply a distance based bilateral [3] as initial filtering of the given size rows by cols.
  * `--blf-sigma_s, --blf-sigma_p, --blf-sigma_t, --blf-iterations value` Change the parameters of the distance based bilateral, as described in [3].
  * `--no-ts` Do not compute temporal stability measures and images.
  * `--dist-all` Generate distance images between all pairs of acquisitions.
  * `--no-write` Do not write pruned images data.

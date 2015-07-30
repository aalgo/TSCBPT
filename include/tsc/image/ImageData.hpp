/*
 * ImageData.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 05/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef IMAGEDATA_HPP_
#define IMAGEDATA_HPP_

#include <vector>
#include "Pixel.hpp"

namespace tscbpt {

using namespace std;

template<typename PixelType>
class ImageData {
private:
	vector<vector<PixelType> > _data;

public:
	typedef PixelType		value_type;

	ImageData() {}

	ImageData(vector<vector<PixelType> >& data): _data(data){}

	ImageData(PixelType data[], size_t rows, size_t cols) {
		_data.resize(rows);
		for(size_t i = 0; i<rows; i++){
			_data[i].insert(*data[i*cols], *data[(i+1)*cols-1]);
		}
	}

	ImageData(size_t rows, size_t cols, const PixelType& value = PixelType()) {
		_data = vector<vector<PixelType> > (rows, vector<PixelType>(cols, value));
	}

	template<typename InputIterator>
	ImageData(InputIterator it, size_t rows, size_t cols){
		_data.resize(rows);
		for(size_t i = 0; i<rows; i++){
			_data[i].reserve(cols);
			for(size_t j = 0; j<cols; j++){
				_data[i].push_back(*it);
				++it;
			}
		}
	}

	template <typename InputIterator, class Transform>
	ImageData(InputIterator it, size_t rows, size_t cols, Transform transform) {
		_data.resize(rows);
		for (size_t i = 0; i < rows; i++) {
			_data[i].reserve(cols);
			for (size_t j = 0; j < cols; j++) {
				_data[i].push_back(transform(*it));
				++it;
			}
		}
	}

	size_t getRows() const{
		return _data.size();
	}

	size_t getCols() const{
		return _data.at(0).size();
	}

	PixelType& getPixelValue(int row, int col){
		return _data.at(row).at(col);
	}

	PixelType& operator()(int row, int col){
		return _data.at(row).at(col);
	}

	const PixelType& operator()(int row, int col) const {
		return _data.at(row).at(col);
	}

    vector<vector<PixelType> >& getData()
    {
        return _data;
    }

    void setData(vector<vector<PixelType> >& _data)
    {
        this->_data = _data;
    }

    vector<PixelType>& operator[] ( size_t n ){
    	return _data[n];
    }

    const vector<PixelType>& operator[](size_t n) const {
		return _data[n];
	}

    struct iterator{
    private:
    	size_t _px,_py;
    	ImageData<PixelType>& _father;
    	iterator(ImageData<PixelType>& father):	_px(0), _py(0), _father(father) {}
    	iterator(ImageData<PixelType>& father, size_t px, size_t py):
    		_px(px), _py(py), _father(father) {}
    public:

    	typedef PixelType		value_type;

    	PixelType& operator *(){
    		return _father[_py][_px];
    	}

    	size_t getRow() const{
    		return _py;
    	}

    	size_t getCol() const {
			return _px;
		}

    	iterator& operator ++(int n) {
    		_px++;
    		if (_px>=_father.getCols()){
    			if (_py>=_father.getRows()){
    				_px= _father.getCols();
    				_py= _father.getRows();
    			}else{
    				_px=0;
    				_py++;
    			}
    		}
    		return *this;
    	}

    	iterator& operator ++() {
    		_px++;
    		if (_px>=_father.getCols()){
    			_px=0;
    			_py++;
    			if (_py>=_father.getRows()){
    				_px= _father.getCols();
    				_py= _father.getRows();
    			}
    		}
    		return *this;
    	}

    	bool operator<(const iterator& b) const {
    		return this->_py < b._py ||
    		(this->_py == b._py && this->_px < b._px);
    	}
    	bool operator==(const iterator& b) const {
    		return this->_px == b._px && this->_py == b._py && this->_father = b._father;
    	}
    	bool operator!=(const iterator& b) const {
    		return this->_py != b._py ||
    		    		(this->_py == b._py && this->_px != b._px);
		}
    	friend class ImageData<PixelType>;
    };

    struct pixel_iterator{
    private:
    	ImageData<PixelType>& _father;
    	size_t _px,_py;
    	Pixel<PixelType,2> _data;
    	pixel_iterator(ImageData<PixelType>& father):	_father(father), _px(0), _py(0),
    		_data(father._data[0][0], 0, 0) {}
    	pixel_iterator(ImageData<PixelType>& father, size_t px, size_t py):
    		_father(father), _px(px), _py(py), _data(PixelType(), px, py) {}
    public:
    	Pixel<PixelType,2>& operator *(){
    		_data.setX(_px);
    		_data.setY(_py);
    		_data.setValue(_father[_py][_px]);
    		return _data;
    	}

//    	pixel_iterator& operator ++(int ) {
//    		_px++;
//    		if (_px>=_father.getCols()){
//    			if (_py>=_father.getRows()){
//    				_px= _father.getCols();
//    				_py= _father.getRows();
//    			}else{
//    				_px=0;
//    				_py++;
//    			}
//    		}
//    		return *this;
//    	}

    	pixel_iterator& operator ++() {
    		_px++;
    		if (_px>=_father.getCols()){
    			if (_py>=_father.getRows()){
    				_px= _father.getCols();
    				_py= _father.getRows();
    			}else{
    				_px=0;
    				_py++;
    			}
    		}
    		return *this;
    	}

    	bool operator<(const pixel_iterator& b) {
    		return this->_py < b._py ||
    		(this->_py == b._py && this->_px < b._px);
    	}
    	bool operator==(const pixel_iterator& b) {
    		return this->_px == b._px && this->_py == b._py && this->_father = b._father;
    	}
    	bool operator!=(const pixel_iterator& b) {
			return this->_py != b._py || (this->_py == b._py && this->_px != b._px);
		}
    	friend class ImageData<PixelType>;
    };

    typedef iterator			row_first_iterator;
    typedef iterator 			value_iterator;
    typedef iterator 			row_first_value_iterator;
    typedef pixel_iterator 		row_first_pixel_iterator;

    row_first_value_iterator begin() {
		return iterator(*this);
	}

	row_first_value_iterator end() {
		return iterator(*this, getCols(), getRows());
	}

    row_first_value_iterator begin_iterator(){
    	return iterator(*this);
    }

    row_first_value_iterator end_iterator(){
        return iterator(*this, getCols(), getRows());
    }

    row_first_pixel_iterator begin_pixel_iterator(){
    	return pixel_iterator(*this);
	}

    row_first_pixel_iterator end_pixel_iterator(){
    	return pixel_iterator(*this, getCols(), getRows());
    }
};

}

#endif /* IMAGEDATA_HPP_ */

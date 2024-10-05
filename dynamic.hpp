#pragma once
#include "math.hpp"
namespace Mathlab {
	template <Arithmetic _T = double> class DynamicMatrix {
		_T *_data;
		size_t _rows, _cols;
	public:
		constexpr DynamicMatrix(size_t x, size_t y) : _rows(x), _cols(y),
			_data(new _T[x * y]{0}) {}
		constexpr DynamicMatrix(const DynamicMatrix &other)
			: _rows(other.rows), _cols(other.cols) {
			size_t n = *other._dims;
			_dims = new size_t[_ndims]{n};
			for (size_t i = 1; i < _ndims; ++i) n *= _dims[i] = other._dims[i];
			_data = new _T[n]{*other._data};
			while (--n) _data[n] = other._data[n];
		}
		constexpr DynamicMatrix(const InitializerList<_T> &il) {
			il.begin();
		}
		constexpr ~DynamicMatrix() {
			delete[] _data;
		}
	};
}
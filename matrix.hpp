#pragma once
#ifndef _MATHLAB_MATRIX_
#define _MATHLAB_MATRIX_ 1
#include "vector.hpp"
#include <initializer_list>
namespace Mathlab {
	template <Arithmetic _T, size_t M, size_t N = M> class Matrix {
		static_assert(M > 0 && N > 0 && NumericType<_T>);
		Vector<_T, M> _data[N];
		typedef struct { size_t a, b; } _index_t;
	public:
		typedef _T ValueType;
		static constexpr size_t rows = M;
		static constexpr size_t columns = N;
		constexpr Matrix() noexcept : _data{0} {} //Zero Matrix
		template <Arithmetic _S> constexpr Matrix(const InitializerList<_S>& il) noexcept {
			size_t a = 0;
			for (_S s : il) _data[a / N][a % N] = s, ++a;
			while (a < M * N) _data[a / N][a % N] = 0, ++a;
		}
		template <Arithmetic _S> constexpr Matrix(const _S(&il)[M][N]) noexcept {
			size_t a = 0;
			for (_S s : il) _data[a / N][a % N] = s, ++a;
			while (a < M * N) _data[a / N][a % N] = 0, ++a;
		}
		template <Arithmetic _S = _T> constexpr Matrix(const Matrix<_S, M, N>& other) noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				_data[i][j] = other[i][j];
		}
		template <Arithmetic _S, size_t P, size_t Q>
		explicit constexpr Matrix(const Matrix<_S, P, Q>& other) noexcept
			requires (P <= M && Q <= N) {
			for (size_t i = 0; i < P; ++i) for (size_t j = 0; j < Q; ++j)
				_data[i][j] = other[i][j];
		}
		constexpr _T* begin() noexcept { return *_data; }
		constexpr _T* end() noexcept { return *_data + M * N; }
		constexpr const _T* begin() const noexcept { return *_data; }
		constexpr const _T* end() const noexcept { return *_data + M * N; }
		constexpr operator bool() const noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				if (_data[i][j]) return true;
			return false;
		}
		constexpr _T& at(size_t z) noexcept {
			_T t = 0;
			return z >= M * N ? t : _data[z / N][z % N];
		}
		constexpr Vector<_T, M>& operator[](size_t z) noexcept {
			return _data[z];
		}
		constexpr const Vector<_T, M>& operator[](size_t z) const noexcept {
			return _data[z];
		}
#ifdef __cpp_multidimensional_subscript
		constexpr _T& operator[](size_t a, size_t b) noexcept {
			return _data[a][b];
		}
		constexpr const _T& operator[](size_t a, size_t b) const noexcept {
			return _data[a][b];
		}
#endif
		template <Arithmetic _S>
		constexpr Matrix& operator+=(const Matrix<_S, M, N>& other) noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				_data[i][j] += other._data[i][j];
			return *this;
		}
		template <Arithmetic _S>
		constexpr Matrix& operator-=(const Matrix<_S, M, N>& other) noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				_data[i][j] -= other._data[i][j];
			return *this;
		}
		template <Arithmetic _S> constexpr Matrix& operator*=(const _S& s) noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				_data[i][j] *= s;
			return *this;
		}
		template <Arithmetic _S> constexpr Matrix& operator/=(const _S& s) noexcept {
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j)
				_data[i][j] /= s;
			return *this;
		}
		template <Arithmetic _S>
		constexpr bool operator==(const Matrix<_S, M, N>& rhs) const noexcept {
			for (size_t i = 0; i < M; i++) for (size_t j = 0; j < N; j++)
				if (_data[i][j] != rhs._data[i][j]) return false;
			return true;
		}
		constexpr Matrix<_T, M - 1, N - 1> subm(size_t x, size_t y) const noexcept {
			Matrix<_T, M - 1, N - 1> m{0};
			for (size_t i = 0; i < M - 1; ++i) for (size_t j = 0; j < N - 1; ++j)
				m[i][j] = _data[i + (i >= x)][j + (j >= y)];
			return m;
		}
		constexpr Matrix<_T, M - 1, N> rsubm(size_t x) const noexcept {
			Matrix<_T, M - 1, N> m{0};
			for (size_t i = 0; i < M - 1; ++i) for (size_t j = 0; j < N; ++j)
				m[i][j] = _data[i + (i >= x)][j];
			return m;
		}
		constexpr Matrix<_T, M, N - 1> csubm(size_t y) const noexcept {
			Matrix<_T, M, N - 1> m{0};
			for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N - 1; ++j)
				m[i][j] = _data[i][j + (j >= y)];
			return m;
		}
		constexpr Vector<_T, N> row(size_t x) const noexcept {
			Vector<_T, N> m{0};
			for (size_t j = 0; j < N; ++j) m[j] = _data[x][j];
			return m;
		}
		constexpr Vector<_T, M> column(size_t y) const noexcept {
			Vector<_T, M> m{0};
			for (size_t i = 0; i < M; ++i) m[i] = _data[i][y];
			return m;
		}
		constexpr Matrix& rswap(size_t dst, size_t src) noexcept {
			if (dst != src) for (size_t j = 0; j < N; ++j) swap(_data[dst][j], _data[src][j]);
			return *this;
		}
		constexpr Matrix& radd(size_t dst, size_t src, _T t = 0) noexcept {
			if (t) for (size_t j = 0; j < N; ++j) _data[dst][j] += _data[src][j] * t;
			return *this;
		}
		constexpr Matrix& rmul(size_t dst, _T t = 1) noexcept {
			if (t && t != 1) for (size_t j = 0; j < N; ++j) _data[dst][j] *= t;
			return *this;
		}
		constexpr Matrix& cswap(size_t dst, size_t src) noexcept {
			for (size_t i = 0; i < M; ++i) swap(_data[i][dst], _data[i][src]);
			return *this;
		}
		constexpr Matrix& cadd(size_t dst, size_t src, _T t = 0) noexcept {
			if (t) for (size_t i = 0; i < M; ++i) _data[i][dst] += _data[i][src] * t;
			return *this;
		}
		constexpr Matrix& cmul(size_t dst, _T t = 1) noexcept {
			for (size_t i = 0; i < M; ++i) _data[i][dst] *= t;
			return *this;
		}
		constexpr operator _T() noexcept requires (M == 1 && N == 1) {
			return _data[0][0];
		}
		template <Arithmetic _S> constexpr operator Vector<_S, M* N>()
			noexcept requires (M == 1 || N == 1) {
			Vector<_S, M* N> v = {_data[0][0]};
			for (size_t j = 1; j < M * N; ++j) v[j] = _data[0][j];
		}
	};
	template <class _T, class _S, size_t M, size_t N> inline constexpr auto
		operator+(const Matrix<_T, M, N>& lhs, const Matrix<_S, M, N>& rhs) noexcept {
		Matrix<CommonType<_T, _S>, M, N> m = lhs;
		return m += rhs;
	}
	template <class _T, class _S, size_t M, size_t N> inline constexpr auto
		operator-(const Matrix<_T, M, N>& lhs, const Matrix<_S, M, N>& rhs) noexcept {
		Matrix<CommonType<_T, _S>, M, N> m = lhs;
		return m -= rhs;
	}
	template <class _T, class _S, size_t M, size_t N> inline constexpr auto
		operator*(const Matrix<_T, M, N>& lhs, const _S& rhs) noexcept {
		Matrix<CommonType<_T, _S>, M, N> m = lhs;
		return m *= rhs;
	}
	template <class _T, class _S, size_t M, size_t N> inline constexpr auto
		operator*(const _T& lhs, const Matrix<_S, M, N>& rhs) noexcept {
		return rhs * lhs;
	}
	template <class _T, class _S, size_t M, size_t N> inline constexpr auto
		operator/(const Matrix<_T, M, N>& lhs, const _S& rhs) noexcept {
		Matrix<CommonType<_T, _S>, M, N> m = lhs;
		return m /= rhs;
	}
	template <class _T, size_t M, size_t N>
	inline constexpr Matrix<_T, M, N> operator+(const Matrix<_T, M, N>& m) noexcept {
		return m * 1;
	}
	template <class _T, size_t M, size_t N>
	inline constexpr Matrix<_T, M, N> operator-(const Matrix<_T, M, N>& m) noexcept {
		return m * -1;
	}
	template <class _T, class _S, size_t M, size_t N, size_t P>
	inline constexpr Matrix<Multiplies<_T, _S>, M, P>
		operator*(const Matrix<_T, M, N>& lhs, const Matrix<_S, N, P>& rhs) noexcept {
		Matrix<long double, M, P> m{0};
		for (size_t i = 0; i < M; ++i) for (size_t k = 0; k < P; ++k) for (size_t j = 0; j < N; ++j)
			m[{i, k}] += lhs[i][j] * rhs[{j, k}];
		return m;
	}
	template <class _T, class _S, size_t M, size_t N>
	inline constexpr Vector<Multiplies<_T, _S>, M>
		operator*(const Matrix<_T, M, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		Vector<long double, M> m{0};
		for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j) m[i] += lhs[i][j] * rhs[j];
		return m;
	}
	template <class _T, class _S, size_t M, size_t N>
	inline constexpr Vector<Multiplies<_T, _S>, N>
		operator*(const Vector<_T, M> lhs, const Matrix<_S, M, N>& rhs) noexcept {
		Vector<long double, N> m{0};
		for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j) m[i] += lhs[i] * rhs[i][j];
		return m;
	}
	//2 "Literal" Matrices
	template <class _T, size_t N>
	constexpr Matrix<_T, N, N> identityMatrix() {
		Matrix<_T, N, N> m{0};
		for (size_t i = 0; i < N; ++i) m[{i, i}] = 1;
		return m;
	}
	template <class _T, size_t N>
	constexpr Matrix<_T, N, N> exchangeMatrix() {
		Matrix<_T, N, N> m{0};
		for (size_t i = 0; i < N; ++i) m[{i, N - 1 - i}] = 1;
		return m;
	};
	template <class _T, size_t N>
	inline constexpr Matrix<_T, N, N> primaryMatrix1(size_t a, size_t b) noexcept {
		Matrix<_T, N, N> m = identityMatrix<_T, N>();
		m[{a, b}] = m[{b, a}] = 1;
		m[{a, a}] = m[{b, b}] = 0;
		return m;
	}
	template <class _T, size_t N>
	inline constexpr Matrix<_T, N, N> primaryMatrix2(size_t a, _T t) noexcept {
		Matrix<_T, N, N> m = identityMatrix<_T, N>();
		m[{a, a}] = t;
		return m;
	}
	template <class _T, size_t N>
	inline constexpr Matrix<_T, N, N> primaryMatrix3(size_t a, size_t b, _T t) noexcept {
		Matrix<_T, N, N> m = identityMatrix<_T, N>();
		m[{a, b}] = t;
		return m;
	}
	template <class _T, size_t N>
	inline constexpr Matrix<_T, N, N> redhefferMatrix() noexcept {
		Matrix<_T, N, N> m{1};
		for (size_t i = 1; i < N; ++i) for (size_t j = 0; j <= i; ++j)
			m[i][j] = !(j && (j + 1) % (i + 1));
		return m;
	}
	//3 Matrix operations
	template <class _T, size_t M, size_t N>
	inline constexpr Matrix<_T, N, M> transpose(const Matrix<_T, M, N>& m) noexcept {
		Matrix<_T, N, M> n{0};
		for (size_t i = 1; i <= M; ++i) for (size_t j = 1; j <= N; ++j) n[{j, i}] = m[i][j];
		return n;
	}
	template <class _T, size_t M, size_t N>
	inline constexpr Matrix<_T, N, M> transjugate(const Matrix<_T, M, N>& m) noexcept {
		Matrix<_T, N, M> n{0};
		for (size_t i = 1; i <= M; ++i) for (size_t j = 1; j <= N; ++j) n[{j, i}] = conj(m[i][j]);
		return n;
	}
	template <class _T, size_t N> constexpr _T det(const Matrix<_T, N, N>& m) noexcept {
		Matrix<_T, N, N> a = m;
		_T t = 1;
		for (size_t i = 0, n = 0; i < N; n = ++i) {
			if (!a[{i, i}]) while (++n <= N) {
				if (n == N) return 0;
				if (a[{n, i}]) {
					a.radd(i, n, 1); break;
				}
			}//top left corner of a is non-zero vvv
			t *= a[{i, i}];
			for (n += n == i; n < N; ++n) a.rsub(n, i, a[{n, i}] / a[{i, i}]);
		}
		return t;
	}
	template <class _T>
	inline constexpr _T det(const Matrix<_T, 1, 1>& m) noexcept {
		return m[{0, 0}];
	}
	template <class _T>
	inline constexpr _T det(const Matrix<_T, 2, 2>& m) noexcept {
		return m[{0, 0}] * m[{1, 1}] - m[{0, 1}] * m[{1, 0}];
	}
	template <class _T, size_t N>
	constexpr Matrix<_T, N, N> adj(const Matrix<_T, N, N>& m) noexcept {
		Matrix<_T, N, N> a = m;
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j)
			a[{j, i}] = det(m.subm(i, j));
		return a;
	}
	template <class _T>
	constexpr Matrix<_T, 1, 1> adj(const Matrix<_T, 1, 1>& m) noexcept {
		return {1};
	}
	template <class _T, size_t N>
	constexpr Matrix<_T, N, N> inv(const Matrix<_T, N, N>& m) noexcept {
		return adj(m) / det(m);
	}
	template <class _T, size_t N>
	constexpr _T track(const Matrix<_T, N, N>& m) noexcept {
		_T t = m[{0, 0}];
		for (size_t n = 1; n < N; ++n) t += m[{n, n}];
		return t;
	}
	template <class _T, size_t M, size_t N>
	constexpr size_t rank(const Matrix<_T, M, N>& m) noexcept {
		Matrix<long double, M, N> a = m;
		size_t t = 0;
		for (size_t i = 0, n = 0; i < N && i < M; n = ++i) {
			if (!a[{i, i}]) while (++n < N) {
				if (a[{n, i}]) {
					a.rswap(i, n); break;
				}
			}//top left corner of a is non-zero vvv
			if (n < N) ++t;
			for (n += n == i; n < M; ++n) a.radd(n, i, -a[{n, i}] / a[{i, i}]);
		}
		return t;
	}
	template <class _T, size_t N> constexpr Matrix<_T, N, N> pow(const Matrix<_T, N, N>& m, int n) {
		return n < 0 ? pow(inv(m), -n) : n ? m * pow(m, n - 1) : identityMatrix<_T, N>();
	}
	template <class _T, size_t N> constexpr Matrix<_T, N, N> exp(const Matrix<_T, N, N>& m) {
		Matrix<_T, N, N> result = identityMatrix<_T, N>() + m, n = m;
		_T t = 1;
		while (m + t * n != m) result += n *= m / (t += 1);
		return result;
	}
	template <class _T, class _S, size_t M, size_t N, size_t P, size_t Q>
	inline constexpr auto operator->*(const Matrix<_T, M, N>& m, const Matrix<_T, P, Q>& n) {
		Matrix<decltype(_T()* _S()), M* P, N* Q> result{0};
		_T t = 1;
		while (m + t * n != m) result += n *= m / (t += 1);
		return result;
	}
	//4 Matrix pattern properties
	template <class _T, size_t N>
	inline constexpr bool isSymmetric(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j)
			if (m[{j, i}] != m[i][j]) return 0;
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isSkewSymmetric(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j)
			if (m[{j, i}] != -m[i][j]) return 0;
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isHermitian(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j)
			if (m[{j, i}] != conj(m[i][j])) return 0;
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isSkewHermitian(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j)
			if (m[{j, i}] != -conj(m[i][j])) return 0;
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isCircular(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0;
			for (size_t k = 1; k < N; ++k) t += m[{i, k}] * conj(m[{k, j}]);
			if (i == j != t) return 0;
		}
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isIdempotent(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0;
			for (size_t k = 1; k < N; ++k) t += m[{i, k}] * m[{k, j}];
			if (m[i][j] != t) return 0;
		}
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isInvolutory(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0;
			for (size_t k = 1; k < N; ++k) t += m[{i, k}] * m[{k, j}];
			if (i == j != t) return 0;
		}
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr size_t isNilpotent(const Matrix<_T, N, N>& m) noexcept {
		Matrix<_T, N, N> n = m;
		for (size_t i = 1; i <= N; n *= m, ++i) { //from Hamilton-Cayley theorem
			if (!n) return i;
		}
		return 0;
	}
	template <class _T, size_t N>
	inline constexpr bool isNormal(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0, u = 0;
			for (size_t k = 1; k < N; ++k) {
				t += m[{i, k}] * conj(m[{j, k}]);
				u += conj(m[{k, i}])* m[{k, j}];
			}
			if (u != t) return 0;
		}
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isOrthogonal(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0;
			for (size_t k = 1; k < N; ++k) t += m[{i, k}] * m[{i, k}];
			if (i == j != t) return 0;
		}
		return 1;
	}
	template <class _T, size_t N>
	inline constexpr bool isUnitary(const Matrix<_T, N, N>& m) noexcept {
		for (size_t i = 1; i <= N; ++i) for (size_t j = 1; j <= N; ++j) {
			_T t = 0;
			for (size_t k = 1; k < N; ++k) t += m[{i, k}] * conj(m[{i, k}]);
			if (i == j != t) return 0;
		}
		return 1;
	}
	//5 Aliases
	template <class _T, size_t M> using ColumnMatrix = Matrix<_T, M, 1>;
	template <class _T, size_t N> using RowMatrix = Matrix<_T, 1, N>;
	template <class _T, size_t M> using ColumnVector = Matrix<_T, M, 1>;
	template <class _T, size_t N> using RowVector = Matrix<_T, 1, N>;
	template <class _T, size_t M> ColumnMatrix<_T, M> column(const Vector<_T, M>& v) {
		size_t i = 0;
		ColumnMatrix<_T, M> m{0};
		for (_T& t : m) t = v[i++];
	}
	template <class _T, size_t N> RowMatrix<_T, N> row(const Vector<_T, N>& v) {
		size_t i = 0;
		RowMatrix<_T, N> m{0};
		for (_T& t : m) t = v[i++];
	}
	//6 Linear Transformation
	template <class _T, size_t N> Matrix<_T, N + 1> transform(const Vector<_T, N>& v) {
		Matrix<_T, N + 1> res = identityMatrix<_T, N + 1>();
		res[N] = v;
	}
	template <class _T> Matrix<_T, 4> rotateQuaternion(const Vector<_T, 4>& v) {
		return 0; //{v[0] + v[1] - v[2] - v[3], 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	}
	template <class _T> Matrix<_T, 4> rotate(const Vector<_T, 3>& v, const _T& theta) {
		auto vs = 1 - cos(theta), s = sin(theta);
		// (v.k)xk, kx(vxk)
		return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	}
	template <class _T> Matrix<_T, 4> scale(const Vector<_T, 3>& v) {
		return {v[0], 0, 0, 0, 0, v[1], 0, 0, 0, 0, v[2], 0, 0, 0, 0, 1};
	}
}
#endif
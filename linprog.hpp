#pragma once
#include "matrix.hpp"
#ifndef _MATHLAB_LINPROG_
#define _MATHLAB_LINPROG_ 1
namespace Mathlab {
	// Maximize y=sum(c*x) assuming sum(a*x)==b and x>=0 using Simplex Method and Dual Simplex Method
	// n variables, m equalities
	// Return value: b column
	// O(m^2*n)
	template <RealArithmetic _T, RealArithmetic _U, RealArithmetic _V, size_t M, size_t N> constexpr int
		linearProgramIterate(Matrix<_T, M, N>& aa, Vector<_U, M>& bb, Vector<_V, N>& cc, size_t* xm) noexcept {
		typedef CommonType<_T, _U, _V, long double> _A;
		while (1) {
			_A s[N] = {0}, smax = 0, t = Limits<_A>::max(), bmin = 0;
			size_t ii = -1, jj = -1, i = 0, j = 0;
			for (j = 0; j < N; ++j) {
				for (s[j] = cc[j], i = 0; i < M; ++i) s[j] -= cc[xm[i]] * aa[{i, j}];
				if (smax < s[j]) smax = s[jj = j]; //Select the column of maximum s[j]
			}
			for (i = 0; i < M; ++i) if (bb[i] < bmin) bmin = bb[ii = i];
			if (bmin) { //Call dual simplex method with any bb[i]<0
				for (jj = -1, j = 0; j < N; ++j) { //Select the row of maximum s/a
					if (aa[{ii, j}] < 0) {
						_A u = s[j] / aa[{ii, j}];
						if (u < t) t = u, jj = j;
					}
				}
				if (jj == -1) return 1; //no root found
				else t = bb[ii] /= aa[{ii, jj}];
			} else if (smax) { //Call simplex method with all bb[i]>=0 but s[j]>0
				for (i = 0; i < M; ++i) if (aa[{i, jj}] > 0 && bb[i] / aa[{i, jj}] < t) t = bb[ii = i] / aa[{i, jj}];
				//Select the row of minimum theta
				bb[ii] = t;
			} else return 0;
			aa.rmul(ii, 1.0l / aa[{ii, xm[ii] = jj}]);
			for (i = 0; i < M; ++i) {
				if (i == ii) continue;
				bb[i] -= aa[{i, jj}] * t;
				aa.radd(i, ii, -aa[{i, jj}]);
			}
		}
	}
	// Maximize y=sum(c*x) assuming sum(a*x)<=b and x>=0 using Simplex Method and Dual Simplex Method
	// n variables, m inequalities
	// Return value: optimum x's as a Vector<X, N>, can culculate y=c*x
	// O(m^2*n)
	template <RealArithmetic _T, RealArithmetic _U, RealArithmetic _V, size_t M, size_t N> constexpr Vector<Promoted<_T, _U, _V>, N>
	linearProgram(const Matrix<_T, M, N>& a, const Vector<_U, M>& b, const Vector<_V, N>& c) noexcept {
		typedef CommonType<_T, _U, _V, long double> _A;
		size_t xm[M] = {0}, i = 0, j = 0;
		Matrix<_A, M, N + M> aa(a);
		Vector<_A, M> bb(b);
		Vector<_A, N + M> cc(c);
		for (i = 0; i < M; ++i) aa[{i, xm[i] = N + i}] = 1;
		if (linearProgramIterate(aa, bb, cc, xm)) return Vector<_A, N>(nan());
		Vector<_A, N> x = {0};
		for (i = 0; i < M; ++i) if (xm[i] < N) x[xm[i]] = bb[i];
		return x;
	}
	template <RealArithmetic _T, RealArithmetic _U, RealArithmetic _V, size_t M, size_t N> constexpr Vector<CommonType<_T, _U, _V>, N>
	integerProgram(const Matrix<_T, M, N>& a, const Vector<_U, M>& b, const Vector<_V, N>& c) noexcept {
		//Part 1: Find solution in N-real space
		typedef CommonType<_T, _U, _V, long double> _A;
		Vector<_A, N> x = linearProgram(a, b, c);
		size_t i = 0, j = 0, jj = -1, xm[M + N] = {0};
		for (; j < N; ++j) if (x[j] != trunc(x[j])) break;
		if (j == N) return x; else floor(x[jj = j]);
		Matrix<_A, M + N, M + N> aa(a);
		Vector<_A, M + N> bb(b), cc(c);
		for (i = 0; i < M; ++i) aa[{i, xm[i] = N + i}] = 1;
		for (j = 0; j < N; ++j) {//gather upper bound of each decision variable
			Vector<_A, N> v = {0};
			v[j] = 1; aa[{M + j, xm[M + j] = j}] = 1, bb[M + j] = linearProgram(a, b, v)[j];
		}
	}
}
#else
#pragma message(__FILE__ "has been included twice.")
#endif
#pragma once
#include "slice.hpp"
#include "math.hpp"
namespace Mathlab {
	// 1 Types
	template <class _T> struct _ValueType : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<_T[]> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<Slice<_T>> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<LinkedList<_T>> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<const _T> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<volatile _T> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<const volatile _T> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<_T&> : _TypeHolder<_T> {};
	template <class _T> struct _ValueType<_T&&> : _TypeHolder<_T> {};
	template <Range _R> using ValueType = typename _ValueType<_R>::type;
	template <Range... _R> using CommonValueType = CommonType<ValueType<_R>...>;
	template <Range... _R> using PromotedValueType = Promoted<ValueType<_R>...>;
	// 2 Functions
	template <Range _R> constexpr ValueType<_R> sum(const _R& r) {
		ValueType<_R> b = 0;
		for (auto a : r) b += a;
		return b;
	}
	template <Range _R> constexpr ValueType<_R> product(const _R& r) {
		ValueType<_R> b = 1;
		for (auto a : r) b *= a;
		return b;
	}
	template <Range _R> constexpr ValueType<_R> arithmeticMean(const _R& r) {
		ValueType<_R> b = 0, n = 0;
		for (auto a : r) b += a, n += 1;
		if (!n) return nan(errno = EPERM);
		if (isfinite(b)) return b / n;
		for (b = 0; auto a:r) b += a / n;
		return b;
	}
	template <Range _R> constexpr ValueType<_R> geometricMean(const _R& r) {
		long double b = 1, n = 0;
		for (auto a : r) b *= a, n += 1;
		if (!n) return nan(errno = EPERM);
		if (isfinite(b)) return pow(b, 1 / n);
		for (b = 1; auto a:r) b *= pow(a, 1 / n);
		return b;
	}
	template <Range _R> constexpr ValueType<_R> harmonicMean(const _R& r) {
		long double b = 0, n = 0;
		for (auto a : r) b += 1.0l / a, n += 1;
		if (!n) return nan(errno = EPERM);
		if (isfinite(b)) return n / b;
		for (b = 0; auto a:r) b += 1.0l / a / n;
		return (decltype(*r.begin()))(1 / b);
	}
	template <Range _R> constexpr ValueType<_R> maximum(_R r) {
		using _V = ValueType<_R>;
		_V b = Limits<_V>::has_infinity ? -Limits<_V>::infinity() : Limits<_V>::lowest();
		for (auto a : r) if (b < a) b = a;
		return b;
	}
	template <Range _R> constexpr ValueType<_R> minimum(_R r) {
		using _V = ValueType<_R>;
		_V b = Limits<_V>::has_infinity ? Limits<_V>::infinity() : Limits<_V>::max();
		for (auto a : r) if (a < b) b = a;
		return b;
	}
	template <Range _R, RealArithmetic _T> constexpr ValueType<_R> mean(_R r, _T y = 1) {
		long double b = 0, n = 0;
		if (isinf(y)) return y < 0 ? minimum(r) : maximum(r);
		else if (y == 0) return geometricMean(r);
		else if (y == 1) return arithmeticMean(r);
		else if (y == -1) return harmonicMean(r);
		for (auto a : r) b += pow(a, y), n += 1;
		if (isfinite(b)) return pow(b / n, 1 / y);
		for (b = 0; auto a : r) b += pow(a, y) / n;
		return pow(b, 1 / y);
	}
	template <Range _R> constexpr ValueType<_R> variance(const _R& r, bool sample = false) {
		long double b = 0, c = 0, n = 0;
		for (auto a : r) b += a * a, c += a, n += 1;
		if (n <= sample) return nan(errno = EPERM);
		if (isfinite(b - c * c)) return (b - c * c / n) / (n - sample);
		for (b = c = 0, n -= sample; auto a : r) b += a * a / n, c += a / n;
		return b - c * c;
	}
	template <Range _R> constexpr ValueType<_R> stddev(const _R& r, bool sample = false) {
		return sqrt(variance(r, sample));
	}
	template <Range _R, Range _S> constexpr CommonType<ValueType<_R>, ValueType<_S>>
	covariance(const _R& x, const _S& y, bool sample = false) {
		long double p = 0, q = 0, r = 0, n = 0;
		auto a = begin(x), b = end(x);
		auto c = begin(y), d = end(y);
		while (a != b && c != d) p += *a * *c, q += *a++, r += *c++, n += 1;
		if (n <= sample) return nan(errno = EPERM);
		if (isfinite(p - q * r)) return (p - q * r / n) / (n - sample);
		a = begin(x); c = begin(y);
		for (p = q = r = 0, n -= sample; a != b && c != d; ++a, ++c) {
			p += *a * *c / n;
			q += *a / n;
			r += *c / n;
		}
		return p - q * r;
	}
	template <Range _R, Range _S> constexpr CommonValueType<_R, _S>
	correlation(const _R& x, const _S& y) {
		return covariance(x, y) / stddev(x) / stddev(y);
	}
	template <class _T> struct RegulationResult {
		_T a, b, r;
	};
	template <class _T> String parseString(RegulationResult<_T> r) {
		return "(a="_s + parseString(r.a) + ", b="_s + parseString(r.b) +
			", r="_s + parseString(r.r) + ')';
	}
	template <Range _R, Range _S> constexpr RegulationResult<CommonValueType<_R, _S>>
	linearEstimate(const _R& x, const _S& y) {
		typedef CommonValueType<_R, _S> _RS;
		long double xa = 0, ya = 0, xx = 0, xy = 0, yy = 0;
		unsigned long long n = 0;
		auto xf = begin(x), xl = end(x);
		auto yf = begin(y), yl = end(y);
		while (yf != yl && xf != xl) {
			xy += *xf * *yf, xx += *xf * *xf, yy += *yf * *yf;
			xa += *xf++, ya += *yf++, n += 1;
		}
		xa /= n, ya /= n, xx /= n, yy /= n, xy /= n;
		xy -= xa * ya, xx -= xa * xa, yy -= ya * ya;
		_RS b = xy / xx;
		return RegulationResult{_RS(ya - b * xa), b, _RS(xy / sqrt(xx * yy))};
	}
}
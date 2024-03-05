#pragma once
#ifndef _MATHLAB_MATH_
#define _MATHLAB_MATH_ 1
#include "basics.hpp"
#if _OLD_CXX
#define Arithmetic class
#endif
#define M_E 2.71828182845904523536		  // e
#define M_LOG2E 1.44269504088896340736	  // log2(e)
#define M_LOG10E 0.434294481903251827651  // log10(e)
#define M_LN2 0.693147180559945309417	  // ln(2)
#define M_LN10 2.30258509299404568402	  // ln(10)
#define M_PI 3.14159265358979323846		  // pi
#define M_PI_2 1.57079632679489661923	  // pi/2
#define M_PI_4 0.785398163397448309616	  // pi/4
#define M_1_PI 0.318309886183790671538	  // 1/pi
#define M_2_PI 0.636619772367581343076	  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390 // 2/sqrt(pi)
#define M_SQRT2 1.41421356237309504880	  // sqrt(2)
#define M_SQRT1_2 0.707106781186547524401 // 1/sqrt(2)

namespace Mathlab {
	// 1 Numerics
	// 1.1 Floating-point Modulo
	template <Arithmetic _T, Arithmetic _U>
	constexpr Promoted<_T, _U> fmod(_T x, _U y) noexcept {
		if (y <= 0)
			return y ? fmod(x, -y) : nan(errno = EDOM);
		else if (x <= 0)
			return x ? fmod(-x, y) : 0;
		else if (!isfinite(x) || isnan(y))
			return nan(errno = EDOM);
		auto a = x >= 2 * y ? fmod(x, 2.0L * y) : x;
		return a >= y ? a - y : a <= -y ? a + y : a;
	}
	template <Arithmetic _T, Arithmetic _U>
	constexpr Promoted<_T, _U> remainder(_T x, _U y) noexcept {
		if (y <= 0)
			return y ? remainder(x, -y) : nan(errno = EDOM);
		else if (!isfinite(x))
			return nan(errno = EDOM);
		else if (y == infinity())
			return x;
		auto a = x > y || x < -y ? remainder(x, 2.0L * y) : x;
		return a > y * 0.5L ? a - y : a < -y * 0.5L ? a + y : a;
	}
	// 1.2 Integer rounding functions
	template <Arithmetic _T, Arithmetic _U = _T>
	inline constexpr auto trunc(_T x) noexcept {
		if constexpr (Integral<_T>)
			return x;
		else
			return x - fmod(x, 1);
	}
	template <Arithmetic _T, Arithmetic _U = _T>
	inline constexpr auto round(_T x) noexcept {
		if constexpr (Integral<_T>)
			return x;
		else
			return x - remainder(x, 1);
	}
	template <Arithmetic _T, Arithmetic _U = _T>
	inline constexpr auto ceil(_T x) noexcept {
		if constexpr (Integral<_T>)
			return x;
		else
			return trunc(x) + (x > 0 && fmod(x, 1));
	}
	template <Arithmetic _T, Arithmetic _U = _T>
	inline constexpr auto floor(_T x) noexcept {
		if constexpr (Integral<_T>)
			return x;
		else
			return trunc(x) - (x < 0 && fmod(x, 1));
	}
	// 1.3 Bit manipulation
	inline constexpr bool isPowOf2(uintmax_t i) { return i && !(i & i - 1); }
	template <Integral _I = uintmax_t> inline constexpr _I bitFloor(_I i) {
		if (i < 0)
			return limits(i).min();
		for (uintmax_t j = i; j >>= 1; i &= ~j)
			;
		return i;
	}
	template <Integral _I = uintmax_t> inline constexpr _I bitCiel(_I i) {
		return i <= 0 ? 0 : i == 1 ? 1 : bitFloor(i - 1) << 1;
	}
	template <Integral _I = uintmax_t> inline constexpr _I bitWidth(_I i) {
		if (i < 0)
			return i + 1 ? bitWidth(~i) : 1;
		else if (i == 0)
			return 0;
		_I j = sizeof(i) * 8, k = _I(-1) < 0 ? 2 : 1;
		while (j >>= 1)
			if (i >> j)
				i >>= j, k += j;
		return k;
	}
	template <Integral _I = uintmax_t> inline constexpr _I rotate(_I i, int r) {
		constexpr size_t N = 8 * sizeof(_I);
		if ((r %= N) < 0)
			r += N;
		return r ? i << r | _I((Unsigned<_I>)i >> N - r) : i;
	}
	// 2 Elementary functions
	// 2.1 Exponential functions
	template <Arithmetic _T> constexpr Promoted<_T> exp(_T x) noexcept {
		if (x > 1 || x < -1) {
			if (x >= LDBL_MAX_EXP)
				return infinity();
			else if (x <= LDBL_MIN_EXP)
				return 0;
			Promoted<_T> t = exp(x / 2.0L);
			return t * t;
		} else if (isnan(x))
			return nan(errno = EDOM);
		Promoted<_T> result = 1, y = 1, n = 0;
		while (y *= x / (n += 1))
			result += y;
		return result;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> exp2(_T x) noexcept {
		return exp(x * M_LN2);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> exp10(_T x) noexcept {
		return exp(x * M_LN10);
	}
	// 2.2 Logarithmic functions
	template <Arithmetic _T> constexpr Promoted<_T> log(_T x) noexcept {
		if (x < 0)
			return nan(errno = EDOM);
		else if (x == 0)
			return -infinity();
		else if (x < 0.5)
			return log(2.0l * x) - M_LN2;
		else if (x > 1)
			return -log(1.0l / x);
		Promoted<_T> result = 0, y = 1, n = 0;
		while (result - y - result)
			result -= (y *= 1 - x) / (n += 1);
		return result;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> log2(_T x) noexcept {
		return log(x) * M_LOG2E;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> log10(_T x) noexcept {
		return log(x) * M_LOG10E;
	}
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Promoted<_T, _U> logn(_T x, _U y) noexcept {
		return x < 0 || y <= 0 || y == 1 ? nan(errno = EDOM) : log(x) / log(y);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> sigmoid(_T x) noexcept {
		return 1.0l / (1.0l + exp(x));
	}
	// 2.3 Power functions
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Promoted<_T, _U> pow(_T x, _U y) noexcept {
		if (y == 0 || x == 1)
			return 1;
		else if (y < 0)
			return pow(1 / x, -y); // y>0 vvv
		else if (y == infinity())
			return abs(x) > 1 ? infinity() : x == -1;
		auto a = fmod(y, 2);
		if (signbit(x))
			return a == 1 ? -pow(-x, y) : a == 0 ? pow(-x, y) : nan(errno = EDOM);
		else
			return exp(y * log(x));
	}
	template <Arithmetic _T> constexpr Promoted<_T> sqrt(_T x) noexcept {
		if (signbit(x))
			return x ? nan(errno = EDOM) : x;
		else if (x >= 4)
			return sqrt(x / 4) * 2;
		else if (x < 1)
			return sqrt(x * 4) / 2;
		Promoted<_T> a = 1;
		while (Promoted<_T> b = (a + x / a) / 2)
			if (a != b)
				a = b;
			else
				return b;
	}
	template <Arithmetic _T> constexpr Promoted<_T> cbrt(_T x) noexcept {
		if (!x || !isfinite(x))
			return x;
		else if (x < 0)
			return -cbrt(-x);
		else if (x >= 8)
			return cbrt(x / 8) * 2;
		else if (x < 1)
			return cbrt(x * 8) / 2;
		Promoted<_T> a = 1;
		while (Promoted<_T> b = (2 * a + x / a / a) / 3)
			if (a != b)
				a = b;
			else
				return b;
	}
	template <Arithmetic... _T> constexpr Promoted<_T...> hypot(_T... t) noexcept {
		if ((isnan(t) || ...))
			return nan();
		if ((isinf(t) || ...))
			return infinity();
		auto l = ((1.0l * t * t) + ...);
		return isinf(l) ? 256 * hypot((t / 256)...)
			: isnormal(l) ? sqrt(l)
			: 256 * hypot((t * 256)...);
	}
	template <Arithmetic _T>
	inline constexpr Promoted<_T> hypot(_T t = 0) noexcept {
		return abs(t);
	}
	template <Arithmetic _T> inline constexpr double hypot() noexcept { return 0; }
	// 2.4 Trigonometric functions
	template <Arithmetic _T> constexpr Promoted<_T> sin(_T x) noexcept {
		if (x < 0)
			return -sin(-x);
		else if (x == 0)
			return x;
		else if (x >= M_PI)
			return -sin(x - M_PI);
		Promoted<_T> result = x, y = x, n = 2;
		while (y *= -x * x / (n * n + n))
			result += y, n += 2;
		return result;
	}
	template <Arithmetic _T> constexpr Promoted<_T> cos(_T x) noexcept {
		if (x < 0)
			return cos(-x);
		else if (x == 0)
			return 1;
		else if (x >= M_PI)
			return -cos(x - M_PI);
		Promoted<_T> result = 1, y = 1, n = 2;
		while (y *= -x * x / (n * n - n))
			result += y, n += 2;
		return result;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> tan(_T x) noexcept {
		return sin(x) / cos(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> cot(_T x) noexcept {
		return cos(x) / sin(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> sec(_T x) noexcept {
		return 1 / cos(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> csc(_T x) noexcept {
		return 1 / sin(x);
	}
	// 2.4.1 Inverse trigonometric functions
	template <Arithmetic _T> inline constexpr Promoted<_T> asin(_T x) noexcept {
		if (x < 0)
			return asin(-x);
		else if (x == 0)
			return x;
		else if (x > 1)
			return nan(errno = EDOM);
		else if (x == 1)
			return M_PI_2;
		Promoted<_T> result = x, y = x, n = 1, z = x * x;
		while (result + y - result)
			result += (y *= n / (n + 1) * z) / (n += 2);
		return result;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acos(_T x) noexcept {
		return M_PI_2 - asin(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> atan(_T x) noexcept {
		return asin(isinf(x) ? sign(x) : x / hypot(x, 1.0f));
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acot(_T x) noexcept {
		return M_PI_2 - atan(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> asec(_T x) noexcept {
		return acos(1.0L / x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acsc(_T x) noexcept {
		return asin(1.0L / x);
	}
	// Supplementaries
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Promoted<_T, _U> atan2(_T y, _U x) noexcept {
		return (signbit(x) ? signbit(y) ? -M_PI : M_PI : 0) + atan(y * 1.0l / x);
	}
	template <Arithmetic _T>
	inline constexpr Promoted<_T> arg(Complex<_T> z) noexcept {
		return atan2(z.imag, z.real);
	}
	// 2.5 Hyperbolic functions
	template <Arithmetic _T> inline constexpr Promoted<_T> sinh(_T x) noexcept {
		return (exp(x) - exp(-x)) / 2;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> cosh(_T x) noexcept {
		return (exp(x) + exp(-x)) / 2;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> tanh(_T x) noexcept {
		Promoted<_T> a = sinh<Promoted<_T>>(x), b = cosh<Promoted<_T>>(-x);
		return isinf(a) ? sign(a) : a / b;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> coth(_T x) noexcept {
		return 1 / tanh(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> sech(_T x) noexcept {
		return 1 / cosh(x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> csch(_T x) noexcept {
		return 1 / sinh(x);
	}
	// 2.5.1 Inverse hyperbolic functions
	template <Arithmetic _T> inline constexpr Promoted<_T> asinh(_T x) noexcept {
		return log(hypot(x, 1) + x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acosh(_T x) noexcept {
		return log(sqrt(x * x - 1) + x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> atanh(_T x) noexcept {
		return log((1 + x) / (1 - x)) / 2;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acoth(_T x) noexcept {
		return log((x + 1) / (x - 1)) / 2;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> asech(_T x) noexcept {
		return acosh(1.0L / x);
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> acsch(_T x) noexcept {
		return asinh(1.0L / x);
	}
	// 2.6 Linear interpolations
	template <Arithmetic _T, Arithmetic _U, Arithmetic _V>
	constexpr Plus<_T, _U> lerp(_T a, _U b, _V t) noexcept {
		return t == 0 || a == b ? a
			: t == 1 ? b
			: // avoid overflowing or underflowing
			1.0L * a * b <= 0 ? a * (1 - t) + b * t
			: t > 1 ? b + (t - 1) * (b - a)
			: a + t * (b - a);
	}
	template <class _T, Arithmetic _V>
	inline constexpr _T* lerp(_T* a, _T* b, _V t) noexcept {
		return a + ptrdiff_t((b - a) * t);
	}
	// 2.6.1 Midpoint
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Plus<_T, _U> midpoint(_T a, _U b) noexcept {
		return isinf(a + b) ? (a + b) * 0.5l : a * 0.5l + b * 0.5l;
	}
	template <class _T> inline constexpr _T* midpoint(_T* a, _T* b) noexcept {
		return a + (b - a) / 2;
	}
	// 2.7 Fibonacci sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21...
	inline constexpr uintmax_t fibonacci(uintmax_t n) {
		if (n < 2 || n == 5)
			return n;
		else if (n < 5)
			return n - 1;
		uintmax_t a = 3, b = 5, x = 6;
		while (x < n)
			++x & 1 ? (a += b) : (b += a);
		return a + b;
	}
	// 3 Factor operations
	template <Integral _T, Integral _U>
	inline constexpr auto gcd(_T x, _U y) -> decltype(x + y) {
		return x && y ? gcd(y, x % y) : abs(x + y);
	}
	template <Integral _T, Integral _U>
	inline constexpr auto lcm(_T x, _U y) -> decltype(x + y) {
		return x && y ? x / gcd(x, y) * y : 0;
	}
	template <Integral _T> inline constexpr Promoted<_T> fact(_T x) noexcept {
		return x < 0 ? nan(errno = EDOM) : x ? fact(x - 1) * x : 1;
	}
	// 3.1 Number of combinations and permutations
	template <Integral _T, Integral _U>
	constexpr CommonType<_T, _U> nPr(_T n, _U r) noexcept {
		return r ? nPr(n, r - 1) * (n - r + 1) : 1;
	}
	template <Integral _T, Integral _U>
	constexpr CommonType<_T, _U> nCr(_T n, _U r) noexcept {
		if (n < 0 || r < 0 || n < r)
			return 0;
		else if (r == 0 || r == n)
			return 1;
		else if (r > n / 2)
			r = n - r;
		uintmax_t* temp = new uintmax_t[r + 1]{1}, res = 0;
		for (_T t = 1; t <= n; ++t)
			for (_U u = t < r ? t : r; u; --u)
				temp[u] += temp[u - 1];
		res = temp[r];
		return delete[] temp, res;
	}
	// 4 Complex functions vvv
	template <Arithmetic _T>
	inline constexpr Complex<_T> exp(Complex<_T> z) noexcept {
		return exp(z.real) * Complex<_T>{cos(z.imag), sin(z.imag)};
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> exp2(Complex<_T> z) noexcept {
		return exp(z * M_LN2);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> exp10(Complex<_T> z) noexcept {
		return exp(z * M_LN10);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> log(Complex<_T> z) noexcept {
		return {log(abs(z)), arg(z)};
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> log2(Complex<_T> z) noexcept {
		return M_LOG2E * log(z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> log10(Complex<_T> z) noexcept {
		return M_LOG10E * log(z);
	}
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Complex<_T> logn(Complex<_T> z, _U y) noexcept {
		return log(z) / log(y);
	}
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Complex<_T> pow(Complex<_T> z, Complex<_U> y) noexcept {
		return exp<Promoted<_T>>(z * log<Promoted<_T>>(y));
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> sqrt(Complex<_T> z) noexcept {
		return sqrt(z.real) * Complex<_T>{cos(z.imag / 2), sin(z.imag / 2)};
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> cbrt(Complex<_T> z) noexcept {
		return cbrt(z.real) * Complex<_T>{cos(z.imag / 3), sin(z.imag / 3)};
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> sin(const Complex<_T>& z) noexcept {
		return {sin(z.real) * cosh(z.imag), cos(z.real) * sinh(z.imag)};
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> cos(const Complex<_T>& z) noexcept {
		return {cos(z.real) * cosh(z.imag), -sin(z.real) * sinh(z.imag)};
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> tan(const Complex<_T>& z) noexcept {
		return sin(z) / cos(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> cot(const Complex<_T>& z) noexcept {
		return cos(z) / sin(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> sec(const Complex<_T>& z) noexcept {
		return 1 / cos(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> csc(const Complex<_T>& z) noexcept {
		return 1 / sin(z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> asin(Complex<_T> z) noexcept {
		return 1il * log(sqrt(1 - z * z) - 1i * z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acos(Complex<_T> z) noexcept {
		return 1il * log(z - 1i * sqrt(1 - z - z));
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> atan(Complex<_T> z) noexcept {
		return 0.5il * (log(1i + z) - log(1i - z));
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acot(Complex<_T> z) noexcept {
		return 0.5il * (log(z - 1i) - log(z + 1i));
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> asec(Complex<_T> z) noexcept {
		return acos(1.0L / z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acsc(Complex<_T> z) noexcept {
		return asin(1.0L / z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> sinh(const Complex<_T>& z) noexcept {
		return {sinh(z.real) * cos(z.imag), cosh(z.real) * sin(z.imag)};
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> cosh(const Complex<_T>& z) noexcept {
		return {cosh(z.real) * cos(z.imag), sinh(z.real) * sin(z.imag)};
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> tanh(const Complex<_T>& z) noexcept {
		return sinh(z) / cosh(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> coth(const Complex<_T>& z) noexcept {
		return cosh(z) / sinh(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> sech(const Complex<_T>& z) noexcept {
		return 1 / cosh(z);
	}
	template <FloatingPoint _T>
	inline constexpr Complex<_T> csch(const Complex<_T>& z) noexcept {
		return 1 / sinh(z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> asinh(Complex<_T> z) noexcept {
		return log(sqrt(1 + z * z) + z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acosh(Complex<_T> z) noexcept {
		return log(sqrt(z * z - 1) + z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> atanh(Complex<_T> z) noexcept {
		return log((1 + z) / (1 - z)) * 0.5l;
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acoth(Complex<_T> z) noexcept {
		return log((z + 1) / (z - 1)) * 0.5l;
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> asech(Complex<_T> z) noexcept {
		return acosh(1.0L / z);
	}
	template <Arithmetic _T>
	inline constexpr Complex<_T> acsch(Complex<_T> z) noexcept {
		return asinh(1.0L / z);
	}
	// 7 Special functions
	// 7.1 Error functions
	template <Arithmetic _T> inline constexpr Promoted<_T> erf(_T x) noexcept {
		if (x < 0)
			return -erf(x);
		else if (x == 0)
			return x;
		else if (x == x + 1)
			return 1;
		else if (isnan(x))
			return nan(errno = EDOM);
		Promoted<_T> result = x, y = x, n = 0;
		while (result + y - result)
			result += (y *= -x * x / (n += 1)) / (2 * n - 1);
		return result * M_2_SQRTPI; // 2 divided by sqrt(M_PI)
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> erfc(_T x) noexcept {
		return 1 - erf(x);
	}
	// 7.2 Gamma functions
	template <Arithmetic _T> inline constexpr Promoted<_T> lgamma(_T x) noexcept {
		if (isnan(x) || x == 0)
			return nan(errno = EDOM);
		Promoted<_T> result = 0, n = 1;
		while (x < 0)
			result -= log(-x), x += 1;
		while (x > 2)
			result += log(x -= 1);
		if (x == 1 || x == 2)
			return 0;
		while (Promoted<_T> y = x * log((n + 1) / (n + x)))
			result += y, n += 1;
		return result;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> tgamma(_T x) noexcept {
		if (isnan(x) || x == 0)
			return nan(errno = EDOM);
		else if (x < 0)
			return tgamma(x + 1) / x;
		else if (x > limits<Promoted<_T>>::max_exponent)
			return infinity();
		else if (x == 1 || x == 2)
			return 1;
		else
			return exp(lgamma(x));
	}
	// 7.3 Beta function
	template <Arithmetic _T, Arithmetic _U>
	inline constexpr Promoted<_T, _U> beta(_T x, _U y) noexcept {
		if (isnan(x) || isnan(y))
			return nan(errno = EDOM);
		Promoted<_T> result = 1 / x + 1 / y, n = 0;
		while (Promoted<_T> z = (n += 1) / (n + x) * (n + x + y) / (n + y))
			if (z != 1)
				result *= z;
		return result;
	}
	// 7.4 Riemann zeta function
	template <Arithmetic _T>
	inline constexpr Promoted<_T> riemannZeta(_T x) noexcept {
		if (isnan(x))
			return nan(errno = EDOM);
		else if (x == 1)
			return infinity();
		else if (x == 0)
			return -0.5;
		else if (x > 1) {
			Promoted<_T> result = 1, n = 1;
			while (Promoted<_T> y = pow(n += 1, -x))
				result += y;
			return result;
		} else if (x > 0) {
			Promoted<_T> result = -1, n = 1, p = -1;
			while (Promoted<_T> y = ((p *= -1) * pow(n += 1, -x))) result += y;
			return result / (exp2(1 - x) - 1);
		} else
				return 2 * pow(2 * M_PI, x - 1) * sin(M_PI_2 * x) * tgamma(1 - x) *
			riemannZeta(1 - x);
	}
	// 8 Bessel functions
	// 8.1 Cylindrical
	template <Arithmetic _T>
	inline constexpr Promoted<_T> cyllindricalBesselI(_T x) {
		return 0;
	}
	template <Arithmetic _T>
	inline constexpr Promoted<_T> cyllindricalBesselJ(_T x) {
		return 0;
	}
	template <Arithmetic _T>
	inline constexpr Promoted<_T> cyllindricalBesselK(_T x) {
		return 0;
	}
	// 8.2 Spherical
	template <Arithmetic _T> inline constexpr Promoted<_T> sphericalBesselI(_T x) {
		return 0;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> sphericalBesselJ(_T x) {
		return 0;
	}
	template <Arithmetic _T> inline constexpr Promoted<_T> sphericalBesselK(_T x) {
		return 0;
	}
	// Alternative names
	auto ln(auto a) noexcept { return log(a); }
	auto lg(auto a) noexcept { return log10(a); }
	auto lb(auto a) noexcept { return log2(a); }
} // namespace Mathlab
#else
#pragma message(__FILE__ "has been included twice.")
#endif
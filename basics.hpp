// Mathlab (TM) files will fail without C++20 support.
#pragma once
#ifndef _MATHLAB_BASICS_
#define _MATHLAB_BASICS_
#include "concepts.hpp"
#include <compare>
#include <ctype.h>
#include <errno.h>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#if _HAS_CXX20 || __cplusplus >= 202000
#define _OLD_CXX 0
#define _CONSTEXPR20 constexpr
#else
#pragma message("Concepts are introduced in C++20")
#define _OLD_CXX 1
#define _CONSTEXPR20
#endif
#if __cplusplus >= 202300 || _HAS_CXX23
#define _CONSTEXPR23 constexpr
#else
#define _CONSTEXPR23
#define consteval (true)
#endif
// Keyword feature macros
#define _DECLTYPE_(...) ::std::decay_t<decltype((__VA_ARGS__))>
#define _NOEXCEPT_AS_(...) noexcept(noexcept(__VA_ARGS__))
#define _REQUIRING_(...) requires requires(__VA_ARGS__)
template <class _T>
_T && declval() {}
// Global Operator aliases
//(1) Compound assignment operators
template <class _T, class _S>
inline constexpr _T & operator+=(_T & t, const _S & s) noexcept {
	return t = t + s;
}
template <class _T, class _S>
inline constexpr _T & operator-=(_T & t, const _S & s) noexcept {
	return t = t - s;
}
template <class _T, class _S>
inline constexpr _T & operator*=(_T & t, const _S & s) noexcept {
	return t = t * s;
}
template <class _T, class _S>
inline constexpr _T & operator/=(_T & t, const _S & s) noexcept {
	return t = t / s;
}
template <class _T, class _S>
inline constexpr _T & operator%=(_T & t, const _S & s) noexcept {
	return t = t % s;
}
template <class _T, class _S>
inline constexpr _T & operator&=(_T & t, const _S & s) noexcept {
	return t = t & s;
}
template <class _T, class _S>
inline constexpr _T & operator|=(_T & t, const _S & s) noexcept {
	return t = t | s;
}
template <class _T, class _S>
inline constexpr _T & operator^=(_T & t, const _S & s) noexcept {
	return t = t ^ s;
}
template <class _T, class _S>
inline constexpr _T & operator<<=(_T & t, const _S & s) noexcept {
	return t = t << s;
}
template <class _T, class _S>
inline constexpr _T & operator>>=(_T & t, const _S & s) noexcept {
	return t = t >> s;
}
//(2) Comparison operaters deduced with operator== and operator<, deprecated in C++20
template <class _T, class _S>
inline constexpr auto operator!=(const _T & t, const _S & s) noexcept {
	return !(t == s);
}
template <class _T, class _S>
inline constexpr auto operator>(const _T & t, const _S & s) noexcept {
	return s < t;
}
template <class _T, class _S>
inline constexpr auto operator<=(const _T & t, const _S & s) noexcept {
	return t < s || t == s;
}
template <class _T, class _S>
inline constexpr auto operator>=(const _T & t, const _S & s) noexcept {
	return t > s || t == s;
}
//(3) Right-value in/decrement operators
template <class _T>
inline constexpr _T operator++(_T & t, int) {
	_T u = t;
	++t;
	return u;
}
template <class _T>
inline constexpr _T operator--(_T & t, int) {
	_T u = t;
	--t;
	return u;
}
//(4) Pointer-to-member
template <class _T, class _S>
inline constexpr auto operator->*(_T & t, const _S & s) noexcept {
	return t.operator->()->*s;
}
template <class _T, class _S>
inline constexpr auto operator->*(_T && t, const _S & s) noexcept {
	return t.operator->()->*s;
}

namespace Mathlab {
	// 1. Bitwise cast function
	template <class _T, class _S>
	inline _T bitwiseCast(const _S & s) {
		return *reinterpret_cast<_T *>(&s);
	}
	// 2. Identity function
	template <class _T>
	inline constexpr _T identity(_T t) noexcept { return t; }
	template <class _T>
	inline constexpr _T & identity(_T & t) noexcept { return t; }
	template <class _T>
	inline constexpr _T && identity(_T && t) noexcept {
		return t;
	}
	template <class _T>
	inline constexpr _T && move(const _T & t) noexcept {
		return static_cast<_T &&>(t);
	}
	template <class _T>
	inline constexpr _T decay(const _T & t) noexcept {
		return t;
	}
	// 3. Operator carriers
	template <class _T, class _S>
	inline constexpr auto plus(const _T & t, const _S & s) noexcept {
		return t + s;
	}
	template <class _T, class _S>
	inline constexpr auto minus(const _T & t, const _S & s) noexcept {
		return t - s;
	}
	template <class _T, class _S>
	inline constexpr auto multiplies(const _T & t, const _S & s) noexcept {
		return t * s;
	}
	template <class _T, class _S>
	inline constexpr auto divides(const _T & t, const _S & s) noexcept {
		return t / s;
	}
	template <class _T, class _S>
	inline constexpr auto modulus(const _T & t, const _S & s) noexcept {
		return t % s;
	}
	template <class _T>
	inline constexpr auto unaryPlus(const _T & t) noexcept {
		return +t;
	}
	template <class _T>
	inline constexpr auto unaryMinus(const _T & t) noexcept {
		return -t;
	}
	template <class _T>
	inline constexpr auto negate(const _T & t) noexcept {
		return -t;
	}
	template <class _T, class _S>
	inline constexpr auto equal(const _T & t, const _S & s) noexcept {
		return t == s;
	}
	template <class _T, class _S>
	inline constexpr auto notEqual(const _T & t, const _S & s) noexcept {
		return t != s;
	}
	template <class _T, class _S>
	inline constexpr auto less(const _T & t, const _S & s) noexcept {
		return t < s;
	}
	template <class _T, class _S>
	inline constexpr auto greater(const _T & t, const _S & s) noexcept {
		return t > s;
	}
	template <class _T, class _S>
	inline constexpr auto lessEqual(const _T & t, const _S & s) noexcept {
		return t <= s;
	}
	template <class _T, class _S>
	inline constexpr auto greaterEqual(const _T & t, const _S & s) noexcept {
		return t >= s;
	}
	template <class _T, class _S>
	inline constexpr auto logicalAnd(const _T & t, const _S & s) noexcept {
		return t && s;
	}
	template <class _T, class _S>
	inline constexpr auto logicalOr(const _T & t, const _S & s) noexcept {
		return t || s;
	}
	template <class _T, class _S>
	inline constexpr auto logicalXor(const _T & t, const _S & s) noexcept {
		return t && !s || s && !t;
	}
	template <class _T>
	inline constexpr auto logicalNot(const _T & t) noexcept {
		return !t;
	}
	template <class _T, class _S>
	inline constexpr auto bitAnd(const _T & t, const _S & s) noexcept {
		return t & s;
	}
	template <class _T, class _S>
	inline constexpr auto bitOr(const _T & t, const _S & s) noexcept {
		return t | s;
	}
	template <class _T, class _S>
	inline constexpr auto bitXor(const _T & t, const _S & s) noexcept {
		return t ^ s;
	}
	template <class _T>
	inline constexpr auto bitNot(const _T & t) noexcept {
		return ~t;
	}
	template <class _T, class _S>
	inline constexpr auto shiftLeft(const _T & t, const _S & s) noexcept {
		return t << s;
	}
	template <class _T, class _S>
	inline constexpr auto shiftRight(const _T & t, const _S & s) noexcept {
		return t >> s;
	}
	template <class _T, class _S>
	inline constexpr auto compare3Way(const _T & t, const _S & s) noexcept {
		return t <=> s;
	}
	template <class _T>
	inline constexpr signed char compareWith0(const _T & t) noexcept {
		return t < 0 ? -1 : t > 0 ? 1 : t == 0 ? 0 : -128;
	}
	// 4 Type helpers
	template <class... _T>
	using Plus = decltype((_T() + ...));
	template <class... _T>
	using Minus = decltype((_T() - ...));
	template <class... _T>
	using Multiplies = decltype((_T() * ...));
	template <class... _T>
	using Divides = decltype((_T() / ...));
	template <class... _T>
	using Modulus = decltype((_T() % ...));
	template <class... _T>
	using BitAnd = decltype((_T() & ...));
	template <class... _T>
	using BitOr = decltype((_T() | ...));
	template <class... _T>
	using BitXor = decltype((_T() ^ ...));
	template <class... _T>
	using ShiftLeft = decltype((_T() << ...));
	template <class... _T>
	using ShiftRight = decltype((_T() >> ...));
	template <class _T, class _S>
	using Comparison = decltype((_T() <=> _S()));
	// 5 Iterators
	template <class _T>
	inline constexpr auto begin(_T & t) noexcept {
		if constexpr (requires(_T t) { t.begin(); })
			return t.begin();
		else
			return nullptr;
	}
	template <class _T>
	inline constexpr auto end(_T & t) noexcept {
		if constexpr (requires(_T t) { t.end(); })
			return t.end();
		else
			return nullptr;
	}
	template <class _T, size_t N>
	inline constexpr _T * begin(_T(&t)[N]) noexcept {
		return t;
	}
	template <class _T, size_t N>
	inline constexpr _T * end(_T(&t)[N]) noexcept {
		return t + N;
	}
	template <class _T>
	inline constexpr size_t size(_T & t) noexcept {
		return end(t) - begin(t);
	}
	template <class _T>
	inline constexpr bool empty(_T & t) noexcept {
		return end(t) == begin(t);
	}
	template <class _T>
	inline constexpr size_t length(_T & t) noexcept {
		size_t n = 0;
		for (auto a = begin(t); a != end(t); ++a)
			++n;
		return n;
	}
	// 6 Swappers
	template <class _T, class _U>
	inline constexpr void swap(_T & t, _U & u) noexcept {
		if constexpr (requires(_T v, _U w) { v.swap(w); }) {
			t.swap(u);
		} else {
			_T x(u);
			_U y(t);
			t = x;
			u = y;
		}
	}
	template <class _T, class _U, size_t N>
	inline constexpr void swap(_T(&t)[N], _U(&u)[N]) noexcept {
		for (size_t i = 0; i < N; ++i)
			swap(t[i], u[i]);
	}
	template <class _T>
	using InitializerList = ::std::initializer_list<_T>;
#if !_OLD_CXX
	using namespace ::std::concepts;
	// 7 Ordering traits: 0 for equivalent, 1 for greater, -1 for less, -128 for
	// unordered
	using StrongOrder = ::std::strong_ordering;
	using WeakOrder = ::std::weak_ordering;
	using PartialOrder = ::std::partial_ordering;
	template <class _T>
	concept StrongOrdered = requires(_T t, _T u) {
		{ t <=> u } -> ConvertibleTo<StrongOrder>;
	};
	template <class _T>
	concept WeakOrdered = requires(_T t, _T u) {
		{ t <=> u } -> ConvertibleTo<WeakOrder>;
	};
	template <class _T>
	concept PartialOrdered = requires(_T t, _T u) {
		{ t <=> u } -> ConvertibleTo<PartialOrder>;
	};
#else
#define Arithmetic typename
#define FloatingPoint typename
#define Integral typename
#define RealArithmetic typename
#define Range typename
#define LegacyIterator typename
#endif
	// 8 Type traits
	template <class _T> struct Limits : ::std::numeric_limits<_T> {};

	template <class _T, bool B = true, class _F = void>
	struct _TypeHolder {
		typedef _T type;
	};
	template <class _T, class _F>
	struct _TypeHolder<_T, false, _F> {
		typedef _F type;
	};
	template <Arithmetic _T> struct _Signed : _TypeHolder<_T> {};
	template <> struct _Signed<char> : _TypeHolder<signed char> {};
	template <> struct _Signed<unsigned char> : _TypeHolder<signed char> {};
	template <> struct _Signed<char16_t> : _TypeHolder<int16_t> {};
	template <> struct _Signed<char32_t> : _TypeHolder<int32_t> {};
	template <> struct _Signed<unsigned short> : _TypeHolder<short> {};
	template <> struct _Signed<unsigned int> : _TypeHolder<int> {};
	template <> struct _Signed<unsigned long> : _TypeHolder<long> {};
	template <> struct _Signed<unsigned long long> : _TypeHolder<long long> {};
	template <> struct _Signed<wchar_t> : _Signed<wint_t> {};
	template <class _T> struct _Signed<const _T> : _Signed<_T> {};
	template <class _T> struct _Signed<volatile _T> : _Signed<_T> {};
	template <class _T> struct _Signed<const volatile _T> : _Signed<_T> {};
	template <class _T> struct _Signed<_T &> : _Signed<_T> {};
	template <class _T> struct _Signed<_T &&> : _Signed<_T> {};
	template <class _T> using Signed = typename _Signed<_T>::type;

	template <Arithmetic _T>
	struct _Unsigned : _TypeHolder<_T> {};
	template <> struct _Unsigned<char> : _TypeHolder<unsigned char> {};
	template <> struct _Unsigned<signed char> : _TypeHolder<unsigned char> {};
	template <> struct _Unsigned<char16_t> : _TypeHolder<uint16_t> {};
	template <> struct _Unsigned<char32_t> : _TypeHolder<uint32_t> {};
	template <> struct _Unsigned<short> : _TypeHolder<unsigned short> {};
	template <> struct _Unsigned<int> : _TypeHolder<unsigned int> {};
	template <> struct _Unsigned<long> : _TypeHolder<unsigned long> {};
	template <> struct _Unsigned<long long> : _TypeHolder<unsigned long long> {};
	template <> struct _Unsigned<wchar_t> : _Unsigned<wint_t> {};
	template <class _T> struct _Unsigned<const _T> : _Unsigned<_T> {};
	template <class _T> struct _Unsigned<volatile _T> : _Unsigned<_T> {};
	template <class _T> struct _Unsigned<const volatile _T> : _Unsigned<_T>{};
	template <class _T> struct _Unsigned<_T &> : _Unsigned<_T> {};
	template <class _T> struct _Unsigned<_T &&> : _Unsigned<_T> {};
	template <class _T> using Unsigned = typename _Unsigned<_T>::type;

	template <Arithmetic _T>
	struct _Promoted : _TypeHolder<float> {};
	template <>
	struct _Promoted<int> : _TypeHolder<double> {}; //int = 32b
	template <>
	struct _Promoted<unsigned int> : _TypeHolder<double> {};
	template <>
	struct _Promoted<double> : _TypeHolder<double> {};
	template <>
	struct _Promoted<long> : _TypeHolder<long double> {}; //long = 32b/64b
	template <>
	struct _Promoted<unsigned long> : _TypeHolder<long double> {};
	template <>
	struct _Promoted<long long> : _TypeHolder<long double> {}; //long long = 64b/128b
	template <>
	struct _Promoted<unsigned long long> : _TypeHolder<long double> {};
	template <>
	struct _Promoted<long double> : _TypeHolder<long double> {};
	template <class _T>
	struct _Promoted<const _T> : _Promoted<_T> {};
	template <class _T>
	struct _Promoted<volatile _T> : _Promoted<_T> {};
	template <class _T>
	struct _Promoted<const volatile _T> : _Promoted<_T> {};
	template <class _T>
	struct _Promoted<_T &> : _Promoted<_T> {};
	template <class _T>
	struct _Promoted<_T &&> : _Promoted<_T> {};

#if _OLD_CXX
	typedef char char8_t;
#else
	template <> struct _Signed<char8_t> : _TypeHolder<int8_t> {};
	template <> struct _Unsigned<char8_t> : _TypeHolder<uint8_t> {};
#endif
	template <class... _T>
	struct _Common : _TypeHolder<void> {};
	template <class _T>
	struct _Common<_T> : _TypeHolder<_T> {};
	template <class _T, class... _U>
	struct _Common<_T, _U...> : _TypeHolder<decltype(1 ? _T() : typename _Common<_U...>::type())> {};
	template <Arithmetic... _T>
	// Common value type
	using CommonType = typename _Common<int, _T...>::type;
	// Promoted value type
	template <Arithmetic... _T>
	using Promoted = CommonType<typename _Promoted<_T>::type...>;

	template <Arithmetic _T>
	constexpr Limits<_T> limits(_T t = 0) noexcept {}
	// 9 Error handling
	class Error {
		errno_t _err;
		char _msg[BUFSIZ];
	public:
		//Describe the error within BUFSIZ (e.g. 512) ASCII characters
		//Avoiding destructor effect
		constexpr Error(errno_t err = 0, const char * msg = 0) noexcept
			: _err(err), _msg{ 0 } {
			for (int i = 0; i < BUFSIZ; ++i) {
				if (!*msg) return;
				_msg[i] = *msg++;
			}
			_msg[BUFSIZ - 1] = 0;
		}
		//Using C library function "sprintf"
		template <class... _T>
		constexpr Error(errno_t err = 0, const char * msg = 0, _T... t) noexcept
			: _err(err), _msg{ 0 } {
			if constexpr (sizeof...(_T)) sprintf(_msg, msg, t...);
		}
		constexpr operator errno_t() { return _err; }
		int print(FILE * file = stderr) const noexcept {
			return fprintf(file, "Error #%d: %s\n", _err, _msg);
		}
		constexpr bool operator==(const Error & other) {
			return _err == other._err && !strcmp(_msg, other._msg);
		}
	};
	// 10 Basic numeric properties
	template <RealArithmetic _T>
	inline constexpr int sign(_T t) noexcept {
		return t < 0 ? -1 : t > 0 ? 1 : t;
	}
	template <RealArithmetic _T>
	inline constexpr bool signbit(_T t) noexcept {
		if (t < 0) return 1;
		else if (t > 0) return 0;
		else if constexpr (Integral<_T>) return 0;
		else return reinterpret_cast<int8_t *>(&t)[sizeof(t) - 1] < 0;
	}
	template <Arithmetic _T>
	inline constexpr Unsigned<_T> abs(_T t) noexcept {
		if (t == 0) return 0; //always "positive" zero
		else return signbit(t) ? -t : t;
	}
	template <Arithmetic _T>
	inline constexpr bool isfinite(_T t) noexcept {
		return !(t - t);
	}
	template <Arithmetic _T>
	inline constexpr bool isnan(_T t) noexcept {
		return t != t;
	}
	template <Arithmetic _T>
	inline constexpr bool isinf(_T t) noexcept {
		return t && 2 * t == t;
	}
	template <Arithmetic _T>
	inline constexpr bool isnormal(_T t) noexcept {
		return isfinite(t) && abs(t) >= std::numeric_limits<_T>::min();
	}
	// 10.1 Maximum and minimum
	template <class _T>
	inline constexpr _T max(const _T & t) noexcept {
		return t < t, t;
	}
	template <class _T>
	inline constexpr _T min(const _T & t) noexcept {
		return t < t, t;
	}
#if OLD_CXX
#define _MIN_MAX_REQ
#else
#define _MIN_MAX_REQ requires (LessThanComparable<_T, _U> && ...)
#endif
	template <class _T, class... _U>
	inline constexpr auto max(const _T & x, const _U &...y) noexcept _MIN_MAX_REQ {
		auto a = max(y...);
		return !isnan(a) && x < a || isnan(x) ? a : x;
	}
	template <class _T, class... _U>
	inline constexpr auto min(const _T & x, const _U &...y) noexcept _MIN_MAX_REQ {
		auto a = min(y...);
		return !isnan(a) && a < x || isnan(x) ? a : x;
	}
	// 10.2 Infinity and NaN
	template <FloatingPoint _T = float>
	inline constexpr _T infinity() noexcept {
		return Limits<_T>().infinity();
	}
	template <FloatingPoint _T = float>
	inline constexpr _T nan(bool signaling = 0) noexcept {
		return signaling ? Limits<_T>().signaling_NaN() : Limits<_T>().quiet_NaN();
	}
	// 10.3 Sum and Product
	template <Arithmetic... _T>
	inline constexpr auto sum(const _T &...x) noexcept {
		return (0 + ... + x);
	}
	template <Arithmetic... _T>
	inline constexpr auto product(const _T &...x) noexcept {
		return (1 * ... * x);
	}
	// 10.4 Comparison operators safe against lossy integer conversions
	template <class _T, class _S>
	inline constexpr bool cmpEqual(const _T & t, const _S & s) {
		return !(t < 0 && s > 0 || t > 0 && s < 0) && t == s;
	}
	template <class _T, class _S>
	inline constexpr bool cmpNotEqsal(const _T & t, const _S & s) {
		return t < 0 && s > 0 || t > 0 && s < 0 || t != s;
	}
	template <class _T, class _S>
	inline constexpr bool cmpLess(const _T & t, const _S & s) {
		return t < 0 && s >= 0 || !(t >= 0 && s < 0) && t < s;
	}
	template <class _T, class _S>
	inline constexpr bool cmpLessEqsal(const _T & t, const _S & s) {
		return t < 0 && s >= 0 || !(t >= 0 && s < 0) && t <= s;
	}
	template <class _T, class _S>
	inline constexpr bool cmpGreater(const _T & t, const _S & s) {
		return t >= 0 && s < 0 || !(t < 0 && s >= 0) && t > s;
	}
	template <class _T, class _S>
	inline constexpr bool cmpGreaterEqsal(const _T & t, const _S & s) {
		return t >= 0 && s < 0 || !(t < 0 && s >= 0) && t >= s;
	}
	// 11 Complex Arithmetic
	template <Arithmetic _T>
	struct Complex {
		_T real, imag;
		static_assert(_T(0) < _T(0.1));
		template <class _S>
		constexpr operator Complex<_S>() noexcept {
			return { (_S)real, (_S)imag };
		}
		constexpr _T & operator[](bool b) noexcept { return b ? imag : real; }
		constexpr operator _T() { return real; }
		constexpr operator bool() { return real || imag; }
	};
	template <class _T>
	struct Complex<Complex<_T>> {
		static_assert(_T(0), "No double complex number");
	};
	template <class _T>
	inline constexpr Complex<decltype(+_T())> operator+(Complex<_T> z) noexcept {
		return Complex{ +z.real, +z.imag };
	}
	template <class _T>
	inline constexpr Complex<decltype(-_T())> operator-(Complex<_T> z) noexcept {
		return Complex{ -z.real, -z.imag };
	}
	template <class _T>
	inline constexpr Complex<decltype(-_T())> operator~(Complex<_T> z) noexcept {
		return Complex{ +z.real, -z.imag };
	}
	template <class _T>
	inline constexpr _T operator*(Complex<_T> z) noexcept {
		return z.real * z.real + z.imag * z.imag;
	}
	// 11.1 Complex addition
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>> operator+(const Complex<_T> & lhs,
		const _S & rhs) noexcept {
		return { lhs.real + rhs, lhs.imag + _S(0) };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator+(const _T & lhs, const Complex<_S> & rhs) noexcept {
		return { lhs + rhs.real, _T(0) + rhs.imag };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator+(const Complex<_T> & lhs, const Complex<_S> & rhs) noexcept {
		return { lhs.real + rhs.real, lhs.imag + rhs.imag };
	}
	// 11.2 Complex subtraction
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>> operator-(const Complex<_T> & lhs,
		const _S & rhs) noexcept {
		return { lhs.real - rhs, lhs.imag - _S(0) };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator-(const _T & lhs, const Complex<_S> & rhs) noexcept {
		return { lhs - rhs.real, _T(0) - rhs.imag };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator-(const Complex<_T> & lhs, const Complex<_S> & rhs) noexcept {
		return { lhs.real - rhs.real, lhs.imag - rhs.imag };
	}
	// 11.3 Complex multiplication
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>> operator*(const Complex<_T> & lhs,
		const _S & rhs) noexcept {
		return { lhs.real * rhs, lhs.imag * rhs };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator*(const _T & lhs, const Complex<_S> & rhs) noexcept {
		return { lhs * rhs.real, lhs * rhs.imag };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator*(const Complex<_T> & lhs, const Complex<_S> & rhs) noexcept {
		Promoted<_T, _S> a[4] = {
			lhs.real * rhs.real, lhs.imag * rhs.imag,
			lhs.imag * rhs.real, lhs.real * rhs.imag
		};
		for (auto x : a)
			if (!isfinite(x)) {
				return isnan(x) ? x : (lhs * _T(0.5)) * (rhs * _S(0.5)) * 4;
			}
		return { a[0] - a[1], a[2] + a[3] };
	}
	// 11.4 Complex division
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>> operator/(const Complex<_T> & lhs,
		const _S & rhs) noexcept {
		return { lhs.real / rhs, lhs.imag / rhs };
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator/(const _T & lhs, const Complex<_S> & rhs) noexcept {
		if (!lhs) return 0;
		else if (isfinite(*rhs)) return lhs * (~rhs / *rhs);
		else return (lhs * 0.5) / (rhs * 0.5);
	}
	template <class _T, class _S>
	inline constexpr Complex<CommonType<_T, _S>>
		operator/(const Complex<_T> & lhs, const Complex<_S> & rhs) noexcept {
		Promoted<_T, _S> a[5] = {
			lhs.real * rhs.real, lhs.imag * rhs.imag,
			lhs.imag * rhs.real, lhs.real * rhs.imag, *rhs
		};
		for (auto x : a) {
			if (isnan(x))
				return nan();
			else if (isinf(x))
				return (lhs * 0.5) / (rhs * 0.5);
		}
		return Complex<Promoted<_T, _S>>{a[0] + a[1], a[2] - a[3]} / a[4];
	}
	// 11.5 Complex comparison
	template <class _T, class _S>
	inline constexpr bool operator==(const Complex<_T> & lhs, const _S & rhs) noexcept {
		return lhs.real == rhs && lhs.imag == _S(0);
	}
	template <class _T, class _S>
	inline constexpr bool operator==(const _T & lhs, const Complex<_S> & rhs) noexcept {
		return lhs == rhs.real && _T(0) == rhs.imag;
	}
	template <class _T, class _S>
	inline constexpr bool operator==(const Complex<_T> & lhs, const Complex<_S> & rhs) noexcept {
		return lhs.real == rhs.real && lhs.imag == rhs.imag;
	}
	// Less-than operator is only used for sorting algorithms, non-mathematical
	template <class _T, class _S>
	inline constexpr bool operator<(const Complex<_T> & lhs,
		const Complex<_S> & rhs) noexcept {
		return lhs.real < rhs.real || lhs.real == rhs.real && lhs.imag < rhs.imag;
	}
	template <Arithmetic _T>
	inline constexpr _T real(_T t) noexcept { return t; }
	template <Arithmetic _T>
	inline constexpr _T imag(_T t) noexcept { return 0; }
	template <Arithmetic _T>
	inline constexpr _T conj(_T x) noexcept { return x; }
	template <Arithmetic _T>
	inline constexpr _T norm(_T x) noexcept { return x * x; }
	template <class _T>
	inline constexpr _T real(Complex<_T> z) noexcept { return z.real; }
	template <class _T>
	inline constexpr _T imag(Complex<_T> z) noexcept { return z.imag; }
	template <class _T>
	inline constexpr Complex<_T> conj(Complex<_T> z) noexcept {
		return { z.real, -z.imag };
	}
	template <class _T>
	inline constexpr _T norm(Complex<_T> z) noexcept {
		return z.real * z.real + z.imag * z.imag;
	}
	template <class _T>
	constexpr Unsigned<_T> abs(Complex<_T> z) noexcept {
		if (!z.real || !z.imag)
			return abs(z.real + z.imag);
		else if (isnan(z.real) || isnan(z.imag))
			return nan();
		else if (isinf(z.real) || isinf(z.imag))
			return infinity();
		else if (*z > 4)
			return abs(z / 2) * 2;
		else if (*z < 1)
			return abs(z * 2) / 2;
		_T a = 1, b = 2, x = *z;
		while (a != b) {
			_T c = (a + b) / 2;
			if (c * c == x) return c;
			if (c * c < x) a = c;
			else b = c;
		}
	}
	template <class _T>
	struct _Promoted<Complex<_T>> : _TypeHolder<Complex<Promoted<_T>>> {};
	// Complex literals
	inline constexpr Complex<float> operator"" fi(long double y) noexcept {
		return { 0, (float)y };
	}
	inline constexpr Complex<float> operator"" fi(unsigned long long y) noexcept {
		return { 0, (float)y };
	}
	inline constexpr Complex<double> operator""i(long double y) noexcept {
		return { 0, (double)y };
	}
	inline constexpr Complex<double> operator""i(unsigned long long y) noexcept {
		return { 0, (double)y };
	}
	inline constexpr Complex<long double> operator"" li(long double y) noexcept {
		return { 0, y };
	}
	inline constexpr Complex<long double>
		operator"" li(unsigned long long y) noexcept {
		return { 0, (long double)y };
	}
	// 12 Character utilities
#if !_OLD_CXX
	template <class _T>
	concept CharacterType = SameAs<::std::remove_cv_t<_T>, char> ||
		SameAs<::std::remove_cv_t<_T>, wchar_t> ||
		SameAs<::std::remove_cv_t<_T>, char8_t> ||
		SameAs<::std::remove_cv_t<_T>, char16_t> ||
		SameAs<::std::remove_cv_t<_T>, char32_t>;
	template <CharacterType _T>
	constexpr _T * begin(_T * t) { return t; }
	template <CharacterType _T>
	constexpr _T * end(_T * t) {
		while (*t)
			++t;
		return t;
	}
#endif
} // namespace Mathlab
#else
#pragma message(__FILE__ "has been included twice.")
#endif
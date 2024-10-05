#pragma once
#ifndef _MATHLAB_STRING_
#define _MATHLAB_STRING_ 1
#include <uchar.h>
#include "slice.hpp"
#include "math.hpp"
namespace Mathlab {
	typedef Slice<char> String;
	typedef Slice<wchar_t> WideString;
	typedef Slice<char8_t> UTF8String;
	typedef Slice<char16_t> UTF16String;
	typedef Slice<char32_t> UTF32String;
	//1.1 String concaternation
	template <class _T, class _S>
	inline constexpr Slice<CommonType<_T, _S>> operator+(const Slice<_T>& lhs, const Slice<_S>& rhs) {
		Slice<CommonType<_T, _S>> s(lhs.begin(), lhs.end());
		s.append(rhs.begin(), rhs.end());
		return s;
	}
	template <class _T, ConvertibleTo<_T> _S = _T>
	inline constexpr Slice<_T> operator+(const Slice<_T>& lhs, const _S& rhs) {
		Slice<_T> s(1, lhs);
		s.append(rhs);
		return s;
	}
	template <class _T, ConvertibleTo<_T> _S = _T>
	inline constexpr Slice<_T> operator+(const _S& lhs, const Slice<_T> rhs) {
		Slice<_T> s(1, lhs);
		s.append(rhs);
		return s;
	}
	//2 String conversion
	template <class _T> inline constexpr String parseString(_T t) {
		return t.toString();
	}
	template <class... _T> inline constexpr String parseWithPrintf(const char* fmt, const _T&... t) {
		if (!fmt) return "";
		char* buf = new char[snprintf(NULL, 0, fmt, t...) + 1] {0};
		sprintf(buf, fmt, t...);
		String res(buf);
		delete[] buf;
		return res;
	}
	//2.1 Boolean
	inline constexpr String parseString(bool val) noexcept {
		return val ? "true" : "false";
	}
	//2.2 Integer
	template <Integral _I>
	constexpr String parseInteger(const _I& val, int base = 10, bool up = false, bool pos = false) noexcept {
		if (base > 36 || base < 2) return "*";
		size_t n = 0;
		char res[sizeof(_I) + 1 << 3] = "";
		String s = val < 0 ? "-" : pos ? "+" : "";
		char num[2][40] = {"0123456789abcdefghijklmnopqrstuvwxyz",
			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
		_I i = val;
		do {
			res[n++] = num[up][abs(i % base)];
		} while (i /= base);
		while (n--) s += res[n];
		return s;
	}
	inline constexpr String parseString(signed char val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(unsigned char val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(short val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(unsigned short val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(int val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(unsigned int val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(long val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(unsigned long val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(long long val) noexcept {
		return parseInteger(val);
	}
	inline constexpr String parseString(unsigned long long val) noexcept {
		return parseInteger(val);
	}
	//2.3 Floating-point number
	//2.3.1 Fixed point
	template <RealFloatingPoint _F>
	constexpr String parseFloat(const _F& val, int base = 10, bool up = false, bool pos = false) noexcept {
		if (base > 36 || base < 2) return "*";
		String s = signbit(val) ? "-" : pos ? "+" : "";
		char num[2][40] = {"0123456789abcdefghijklmnopqrstuvwxyz",
			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
			res[LDBL_MAX_EXP * 2 + LDBL_MANT_DIG] = 0;
		_F f = fmod(val, 1), i = val - f;
		size_t n = 0;
		do {
			res[n++] = num[up][abs(i % base)];
		} while (i /= base);
		while (n--) s += res[n];
		return s;
	}
	template<FloatingPoint _F>
	constexpr String parseFloat(_F val, char mode = 'g', char pos = 0) {
		char i = 'I', fmt[16] = "%", * ptr = fmt + 1;
		switch (pos) {
		case '-': case '+': case ' ': *ptr++ = pos;
		case 0:   break;
		default:  return "";
		}
		switch (mode) {
		case 'A': case 'E': case 'F': case 'G': *ptr++ = mode; break;
		case 'a': case 'e': case 'f': case 'g': *ptr++ = mode, i = 'i'; break;
		default: return "";
		}
		if (isnan(val)) return i == 'I' ? "NAN" : "nan";
		if constexpr (RealFloatingPoint<_F>) {
			return parseWithPrintf(fmt, val);
		} else {
			*ptr++ = '%', *ptr++ = '+', *ptr++ = mode;
			return parseWithPrintf(fmt, val.real, val.imag);
		}
	}
	constexpr String parseString(float val) {
		return parseWithPrintf("%g", val);
	}
	constexpr String parseString(double val) {
		return parseWithPrintf("%lg", val);
	}
	constexpr String parseString(long double val) {
		return parseWithPrintf("%Lg", val);
	}
	constexpr String parseString(Complex<float> val) {
		return parseWithPrintf("%g%+gi", val.real, val.imag);
	}
	constexpr String parseString(Complex<double> val) {
		return parseWithPrintf("%lg%+lgi", val.real, val.imag);
	}
	constexpr String parseString(Complex<long double> val) {
		return parseWithPrintf("%Lg%+Lgi", val.real, val.imag);
	}
	//2.4 Character
	constexpr String parseString(char c) noexcept {
		return {c};
	}
	constexpr String parseString(char8_t c) noexcept {
		return {c};
	}
	constexpr String parseString(char16_t c) noexcept {
		if (c <= 0x7F) return {c};
		else if (c <= 0x7FF) return {c >> 6 | 0xC0, c & 0x3F | 0x80};
		else return {c >> 12 | 0xE0, c >> 6 & 0x3F | 0x80, c & 0x3F | 0x80};
	}
	constexpr String parseString(char32_t c) noexcept {
		register unsigned int a = c & 0x3F | 0x80,
			b = c >> 6 & 0x3F | 0x80,
			d = c >> 12 & 0x3F | 0x80,
			e = c >> 18 & 0x3F | 0x80,
			f = c >> 24 & 0x3F | 0x80;
		if (c <= 0x7F) return {c};
		else if (c <= 0x7FF) return {b | 0xC0, a};
		else if (c <= 0xFFFF) return {d | 0xE0, b, a};
		else if (c <= 0x1FFFFF) return {e | 0xF0, d, b, a};
		else if (c <= 0x3FFFFFF) return {f | 0xF8, e, d, b, a};
		else if (c <= 0x7FFFFFFF) return {c >> 30 | 0xFCu, f, e, d, b, a};
		else return {-1};
	}
	constexpr String parseString(wchar_t c) noexcept {
		return parseString(char32_t(c));
	}
	//2.5 Byte/multibyte String
	inline constexpr String parseString(const String& s) noexcept {
		return s;
	}
	inline constexpr String parseString(const char* s) noexcept {
		return s;
	}
	inline constexpr String operator""_s(const char* s, size_t n) noexcept {
		return String(s, s+n);
	}
	//2.6 UTF Strings
#if !_OLD_CXX
	inline constexpr String parseString(const UTF8String& s) noexcept {
		return String(s);
	}
	inline constexpr UTF8String operator""_s(const char8_t* s, size_t n) noexcept {
		return UTF8String(s, s + n);
	}
#endif
	inline constexpr String parseString(const UTF16String& s) noexcept {
		String res = "";
		char32_t h = 0;
		for (char16_t wc : s) {
			char c[2 * MB_LEN_MAX] = {0};
			switch (wc & 0xFC00) {
			case 0xD800: //High surrogate
				h = wc;
				break;
			case 0xDC00: //Low surrogate
				res += h ? parseString(char32_t((h & 0x03FF << 10 | wc & 0x03FF) + 0x10000))
					: parseString(wc);
				h = 0;
				break;
			default: //Normal character
				if (h) res += parseString(h), h = 0;
				res += parseString(wc);
			}
		}
		if (h) res += parseString(h);
		return res;
	}
	inline constexpr UTF16String operator""_s(const char16_t* s, size_t n) noexcept {
		return UTF16String(s, s + n);
	}
	//UTF-32 String
	inline constexpr String parseString(const UTF32String& s) noexcept {
		String res = "";
		for (char32_t wc : s) res += parseString(wc);
		return res;
	}
	inline constexpr UTF32String operator""_s(const char32_t* s, size_t n) noexcept {
		return UTF32String(s, s + n);
	}
	//Wide string
	inline constexpr String parseString(const WideString& s) noexcept {
		switch (sizeof(wchar_t)) {
		case 2: return parseString(Slice<char16_t>(s));
		case 4: return parseString(Slice<char32_t>(s));
		default:return String(s);
		}
	}
	inline constexpr WideString operator""_s(const wchar_t* s, size_t n) noexcept {
		return WideString(s, s + n);
	}
	// Parse String from other slices
	template <typename _T> constexpr String parseString(const Slice<_T>& slice) {
		size_t n = slice.size();
		switch (n) {
		case 0: return "[]";
		case 1: return "[" + parseString(slice[0]) + ']';
		}
		String res = "[";
		for (size_t i = 0; i < n; ++i) {
			res += parseString(0);
			res += (i == n - 1 ? ']' : ',');
		}
		return res;
	}
	//3 Print String
	int printString(const String& s) noexcept {
		int n = 0;
		for (char c : s) {
			if (putchar(c) == EOF) return EOF;
			if (!c) break;
			++n;
		}
		return n;
	}
	int fprintString(FILE* f, const String& s) noexcept {
		int n = 0;
		for (char c : s) if (fputc(c, f) == EOF) return EOF; else ++n;
		return n;
	}
}
#endif
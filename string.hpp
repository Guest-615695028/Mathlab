#pragma once
#ifndef _MATHLAB_STRING_
#define _MATHLAB_STRING_ 1
#include <uchar.h>
#include "linearlist.hpp"
#include "math.hpp"
namespace Mathlab {
	typedef LinearList<char> String;
	typedef LinearList<wchar_t> WideString;
	typedef LinearList<char8_t> UTF8String;
	typedef LinearList<char16_t> UTF16String;
	typedef LinearList<char32_t> UTF32String;
	//1.1 String concaternation
	template <class _T>
	inline constexpr LinearList<_T> operator+(const LinearList<_T>& lhs, const LinearList<_T>& rhs) {
		LinearList<_T> s = lhs;
		s.append(rhs.begin(), rhs.end());
		return s;
	}
	template <class _T, ConvertibleTo<_T> _S>
	inline constexpr LinearList<_T> operator+(const LinearList<_T>& lhs, const _S& rhs) {
		LinearList<_T> s = lhs;
		s.append(rhs);
		return s;
	}
	template <class _T, ConvertibleTo<_T> _S>
	inline constexpr LinearList<_T> operator+(const _S& lhs, const LinearList<_T> rhs) {
		LinearList<_T> s = lhs;
		s.append(rhs);
		return s;
	}
	//2 String conversion
	template <class _T> inline constexpr String parseString(_T t) {
		return t.toString();
	}
	template <class... _T> inline constexpr String parseWithPrintf(const char* fmt, const _T&... t) {
		char* buf = new char[snprintf(NULL, 0, fmt, t...) + 1] {0};
		sprintf(buf, fmt, t...);
		String res(buf);
		delete buf;
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
	constexpr String parseFixed(const _F& val, bool up = false, bool pos = false, int prec = 6) noexcept {
		String s = signbit(val) ? "-" : pos ? "+" : "";
		char num[12] = {"0123456789"};
		if (isnan(val)) return s + (up ? "NAN" : "nan");
		else if (isinf(val)) return s + (up ? "INF" : "inf");
		_F f = fmod(abs(val), 1), i = abs(val) - f;
		String res = ".";
		do {
			int n(fmod(i, 10));
			res.insert(0, num[n]);
		} while ((i /= 10) >= 1);
		for (int i = 0; i < prec - 1; ++i) {
			unsigned int u(f *= 10);
			res += num[u];
			f -= u;
		}
		return s + res;
	}
	//2.3.2 Scientific notation
	template <RealFloatingPoint _F>
	constexpr String parseExp(const _F& val, bool up = false, bool pos = false, int prec = 6) noexcept {
		intmax_t e = 0;
		String s = signbit(val) ? "-" : pos ? "+" : "";
		if (isnan(val)) return s + (up ? "NAN" : "nan");
		else if (isinf(val)) return s + (up ? "INF" : "inf");
		else if (val == 0) return s + "0." + String(prec, 0);
		_F f = abs(val);
		while (f < 1) --e, f *= 10;
		while (f >= 10) ++e, f /= 10;
		String res = {(char)('0' + f), '.'}, ee = parseInteger(e);
		for (int i = 0; i < prec - 1; ++i) {
			unsigned int u(f *= 10);
			res += "0123456789"[u % 10];
			f -= u;
		}
		res += "0123456789"[u % 10];
		return s + res + (up ? 'E' : 'e') + ee;
	}
	constexpr String parseString(float val) {
		return parseWithPrintf("%g", val);
	}
	constexpr String parseString(double val) {
		return parseWithPrintf("%g", val);
	}
	constexpr String parseString(long double val) {
		return parseWithPrintf("%g", val);
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
	//2,6 UTF-8 String: Assuming this program is built for UTF-8 system
#if !_OLD_CXX
	inline constexpr String parseString(const UTF8String& s) noexcept {
		return s;
	}
#endif
	//UTF-16 String, with/out (L)ocal character set
	template <bool L = false>
	inline constexpr String parseString(const UTF16String& s) noexcept(!L) {
		String res = "";
		if constexpr (L) {
			mbstate_t a = {0};
			for (char16_t wc : s) {
				char c[2 * MB_LEN_MAX] = {0};
				int i = c16rtomb(c, wc, &a);
				if (i < 0) throw Error(EILSEQ, "Illegal character 0x%04x", wc);
				res += c;
			}
		} else {
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
						if (h) res += parseString(h);
						res += parseString(wc);
				}
			}
			if (h) res += parseString(h);
		}
		return res;
	}
	//UTF-32 String, with/out (L)ocal character set
	template <bool L = false>
	inline constexpr String parseString(const UTF32String& s) noexcept(!L) {
		String res = "";
		if constexpr (L) {
			mbstate_t a = {0};
			for (char32_t wc : s) {
				char c[2 * MB_LEN_MAX] = {0};
				int i = c32rtomb(c, wc, &a);
				if (i < 0) throw Error(EILSEQ, "Illegal character 0x%08x", wc);
				res += c;
			}
		} else {
			for (char32_t wc : s) {
				res += parseString(wc);
			}
		}
		return res;
	}
	//Wide string, with/out (L)ocal character set
	template <bool L = false>
	inline constexpr String parseString(const WideString& s) {
		if constexpr (L) {
			String res = "";
			wctomb(0, 0);
			for (wchar_t wc : s) {
				char c[2 * MB_LEN_MAX] = {0};
				int i = wctomb(c, wc);
				if (i < 0) throw Error(EILSEQ, "Illegal character 0x%0*x", 2 * sizeof(wc), wc);
				res += c;
			}
			return res;
		} else if constexpr (sizeof(wchar_t) == 2) {
			return parseString(LinearList<char16_t>(s));
		} else if constexpr (sizeof(wchar_t) == 4) {
			return parseString(LinearList<char32_t>(s));
		} else return String(s);
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
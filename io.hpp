#pragma once
#include <string.h>
#include "basics.hpp"
namespace Mathlab {
	class IOStream {
		FILE* _stream;
		unsigned char _format[8];
		/**
		* 0: Numeric formats
		*    0x80 Floating-point
		*    0x40 Format
		*    0x20 ^
		*    0x10 |
		*    0x08 Integral
		*    0x04 Base
		*    0x02 |
		*    0x01 v
		* 1: Precision
		* 2: boolalpha(0x10),
		*    showbase(0x08), showpoint(0x04), showpos(0x02), uppercase(0x01)
		*/
		public:
		enum Base { bin = 2, oct = 8, dec = 10, hex = 16 };
		enum Format { fixed = 1, scientific = 2, hexfloat = 3 };
		constexpr IOStream(FILE* f = 0) noexcept : _stream(f), _format{ 0 } {}
		IOStream(char* _FileName, char* _Mode) : _stream(fopen(_FileName, _Mode)), _format{ 0 } {
			setbuf(_stream, 0);
			if (errno) throw Error(this, strerror(errno));
		}
		IOStream(const IOStream&) = delete;
		~IOStream() {
			fflush(_stream), fclose(_stream);
			if (errno) throw Error(this, strerror(errno));
		}
		IOStream& skipws() noexcept {
			char c = 0;
			while (!isgraph(c = getc(_stream)));
			return ungetc(c, _stream), * this;
		}
		template <Invocable<char> _F> IOStream& skip(_F func) noexcept {
			char c = 0;
			while (func(c = getc(_stream)));
			return ungetc(c, _stream), * this;
		}
		//1 Status checkers
		bool good() noexcept {
			return !(feof(_stream) || ferror(_stream));
		}
		operator bool() noexcept {
			return !(feof(_stream) || ferror(_stream));
		}
		bool eof() noexcept {
			return feof(_stream);
		}
		bool fail() noexcept {
			return ferror(_stream);
		}
		int clear() noexcept {
			return errno = 0, clearerr(_stream), errno;
		}
		int getBase() {
			return _format[0] & 0x3f;
		}
		int getBase() {
			return _format[0] >> 6;
		}
		//2 Input methods
		char getChar() noexcept {
			return getc(_stream);
		}
		IOStream& operator>>(char& dest) noexcept {
			while (!isgraph(dest = getc(_stream)));
			return *this;
		}
		wchar_t getWideChar() noexcept {
			return getwc(_stream);
		}
		IOStream& operator>>(wchar_t& dest) noexcept {
			while (!iswgraph(dest = getwc(_stream)));
			return *this;
		}
		template <class _T = char> size_t getString(_T* dest, size_t num = BUFSIZ, const _T* delim = nullptr, bool invert = false) noexcept {
			size_t n = 0;
			auto strchr = [](const _T* str, const _T* c) {
				do {
					if (*str == c) return 1;
				} while (*++str);
				return 0;
			};
			if (delim) while (num--) {
				char c = getc(_stream);
				if (invert ^ !strchr(delim, c)) {
					*dest = 0, ungetc(c, _stream);
					break;
				} else *dest++ = c, ++n;
			} else fgets(dest, num, _stream);
			return n;
		}
		IOStream& operator>>(char* dest) {
			while (!isgraph(*dest = getc(_stream)));
			while (isgraph(*++dest = getc(_stream)));
			ungetc(*dest, _stream);
			return *this;
		}
		size_t getWideString(wchar_t* dest, size_t num = BUFSIZ, const wchar_t* delim = 0, bool invert = false) noexcept {
			size_t n = 0;
			if (delim) while (num--) {
				wchar_t c = getwc(_stream);
				if (invert ^ !wcschr(delim, c)) {
					*dest = 0, ungetwc(c, _stream);
					break;
				} else *dest++ = c, ++n;
			} else fgetws(dest, num, _stream);
			return n;
		}
		IOStream& operator>>(wchar_t* dest) {
			while (!iswgraph(*dest = getwc(_stream)));
			while (iswgraph(*++dest = getwc(_stream)));
			ungetwc(*dest, _stream);
			return *this;
		}
		template <Integral _T = int> _T getInt(unsigned int base = 0, unsigned int width = BUFSIZ) {
			char c[BUFSIZ] = "", * p = c;
			if (base > 36 || base == 1) throw Error(this, "Base %d invalid", % d);
			fread(c, BUFSIZ, 1, _stream);
			_T i = _T(-1) < _T(0) ? strtoll(c, &p, base) : strtoull(c, &p, base);
			memmove(c, p, BUFSIZ - (p - c));
			return i;
		}
		template<Integral _T> IOStream& operator>>(_T& i) {
			char c[BUFSIZ] = "", * p = c, * q = c;
			*this >> c;
			while (*++q);
			i = _T(-1) < _T(0) ? strtoll(c, &p, base) : strtoull(c, &p, base);
			while (--q >= p) ungetc(*q, _stream);
			return *this;
		}
		//Output methods
		IOStream& putChar(char _Ch) noexcept {
			return putc(_Ch, _stream), * this;
		}
		IOStream& operator<<(char _Ch) noexcept {
			return putc(_Ch, _stream), * this;
		}
		IOStream& putWideChar(wchar_t _Ch) noexcept {
			return putwc(_Ch, _stream), * this;
		}
		IOStream& operator<<(wchar_t _Ch) noexcept {
			return putwc(_Ch, _stream), * this;
		}
		IOStream& putChar(char* _Ch) noexcept {
			return fputs(_Ch, _stream), * this;
		}
		IOStream& operator<<(char* _Ch) noexcept {
			return fputs(_Ch, _stream), * this;
		}
		IOStream& putWideChar(wchar_t* _Ch) noexcept {
			return fputws(_Ch, _stream), * this;
		}
		IOStream& operator<<(wchar_t* _Ch) noexcept {
			return fputws(_Ch, _stream), * this;
		}
	} cin(stdin), cout(stdout);
	template <class OS, class _T> OS& operator<<(OS& os, Complex<_T> t) {
		return os << '(' << t.real << "}, {" << t.imag << ')';
	}
}
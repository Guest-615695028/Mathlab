#pragma once
#include "basics.hpp"
#ifndef _BIG_HPP_
#define _BIG_HPP_ 0x8000'0000'0000'0000ul
namespace Mathlab {
	//1 Big integer type 
	class BigInt {
		uintmax_t _size, * _words;
		constexpr BigInt(uintmax_t size, uintmax_t* words) noexcept : _size(size), _words(words) {}
	public:
		static constexpr int WORD_BITS = 8 * sizeof(uintmax_t);
		static constexpr int WORD_BYTES = sizeof(uintmax_t);
		static constexpr uintmax_t WORD_MAX = INTMAX_MAX;
		static constexpr uintmax_t WORD_MIN = INTMAX_MIN;
		constexpr BigInt() noexcept : _words(0), _size(0) {}
		//Built-in Integer type
		template <Integral<false> _T> _CONSTEXPR20 BigInt(_T t) noexcept : _words(0), _size(1) {
			if (cmpGreater(t, INTMAX_MAX))
				_words = new uintmax_t[_size = 2]{uintmax_t(t), uintmax_t(t) >> WORD_BITS};
			else
				_words = new uintmax_t[1](t);
			//Negative integer will become positive when operated with no-shorter unsigned integer
		}
		_CONSTEXPR20 BigInt(const BigInt& t) noexcept : _words(0), _size(t.size()) {
			_words = new uintmax_t[_size]{*t._words};
			for (uintmax_t i = 1; i < _size; ++i) _words[i] = t._words[i];
		}
		_CONSTEXPR20 ~BigInt() noexcept {
			if (_words) delete[] _words;
		}
		_CONSTEXPR20 BigInt& operator=(const BigInt& t) noexcept {
			if (_words) delete[] _words;
			_words = new uintmax_t[_size = t.size()]{*t._words};
			for (uintmax_t i = 0; i < _size; ++i) _words[i] = t._words[i];
			return *this;
		}
		//Non-zero is true
		constexpr operator bool() const noexcept {
			for (uintmax_t i = 0; i < _size; ++i) if (_words[i]) return true;
			return false;
		}
		//Read word
		constexpr uintmax_t operator[](uintmax_t i) const noexcept {
			if (i < _size) return _words ? _words[i] : 0;
			else return _words[_size - 1] > WORD_MAX ? -1 : 0;
		}
		constexpr uintmax_t size(bool cap = false) const noexcept {
			if (cap) return _size;
			uintmax_t n = _size;
			while (n > 1) {
				uintmax_t* p = _words + n - 2;
				if (_words[1] == 0 && _words[0] <= WORD_MAX
					|| _words[1] == UINTPTR_MAX && _words[0] >= WORD_MIN) --n;
				else break;
			}
			return n;
		}
		constexpr uint8_t byte(uintmax_t i) const noexcept {
			if (i >= WORD_BYTES * _size) return _words[_size - 1] > WORD_MAX ? -1 : 0;
			// Sign bit
			else return _words[i / WORD_BYTES] >> i % WORD_BYTES * 8;
		}
		constexpr uint16_t doublet(uintmax_t i) noexcept {
			constexpr int n = WORD_BYTES / 2;
			if (i >= n * _size) return _words[_size - 1] > WORD_MAX ? -1 : 0;
			// Sign bit
			else return _words[i / n] >> i % n * 8;
		}
		constexpr uint32_t quadruplet(uintmax_t i) noexcept {
			constexpr int n = WORD_BYTES / 4;
			if (i >= n * _size) return _words[_size - 1] > WORD_MAX ? -1 : 0;
			// Sign bit
			else return _words[i / n] >> i % n * 8;
		}
		constexpr uint64_t octuplet(uintmax_t i) noexcept {
			constexpr int n = WORD_BYTES / 8;
			if (i >= n * _size) return _words[_size - 1] > WORD_MAX ? -1 : 0;
			// Sign bit
			else return _words[i / n] >> i % n * 8;
		}
		template <typename _T> constexpr _T word(uintmax_t i) noexcept {
			constexpr int n = WORD_BYTES / sizeof(_T);
			if (i >= n * _size) return _words[_size - 1] > WORD_MAX ? -1 : 0;
			// Sign bit
			else return _words[i / n] >> i % n * 8;
		}
		//Unary Arithmetic
		constexpr BigInt operator+() const noexcept {
			return *this;
		}
		constexpr BigInt operator-() const noexcept {
			uintmax_t i = 0;
			BigInt t = *this;
			for (bool flag = 1; i < t._size; flag = flag && !t._words[i++]) {
				t._words[i] = ~t._words[i] + flag;
				if (flag && i == t._size - 1 && t._words[i] == WORD_MIN) {
					uintmax_t* temp = new uintmax_t[++t._size]{0};
					temp[_size - 2] = WORD_MIN;
					delete[] _words;
					t._words = temp;
					break;
				}
			}
			return t;
		}
		constexpr BigInt operator~() const noexcept {
			BigInt t = *this;
			for (uintmax_t i = 0; i < t._size; ++i) t._words[i] = ~t._words[i];
			return t;
		}
		constexpr BigInt& operator++() noexcept {
			uintmax_t i = 0;
			//increment the next word if this word becomes 0
			for (; i < _size; ++i) if (++_words[i]) break;
			if (i == _size - 1 && _words[i] == WORD_MIN) {
				//Signed Overflow, resulting in 8000 .... 0000
				uintmax_t* temp = new uintmax_t[++_size]{0};
				temp[_size - 2] = WORD_MIN;
				delete[] _words;
				_words = temp;
			}
			return *this;
		}
		constexpr BigInt& operator--() noexcept {
			uintmax_t i = 0;
			//decrement the next word if this word is 0 and becomes -1
			for (; i < _size; ++i) if (_words[i]--) break;
			if (i == _size - 1 && _words[i] == WORD_MAX) {
				//Signed Underflow, resulting in 7FFF .... FFFF
				uintmax_t* temp = new uintmax_t[++_size]{0};
				for (i = 0; i < _size; ++i) temp[i] = i == _size - 2 ? WORD_MAX : -1;
			}
			return *this;
		}
		//Shrink to fit
		_CONSTEXPR20 BigInt& shrinkToFit() noexcept {
			uintmax_t* temp = new uintmax_t[_size = this->size()];
			for (uintmax_t i = 0; i < _size; ++i) temp[i] = _words[i];
			delete[] _words;
			_words = temp;
			return *this;
		}
		//Comparison with 0
		constexpr bool operator==(nullptr_t) const noexcept {
			for (uintmax_t i = 0; i < _size; ++i) if (_words[i]) return 0;
			return 1;
		}
		constexpr bool operator<(nullptr_t) const noexcept {
			return _words[_size - 1] > WORD_MAX;
		}
		constexpr StrongOrder operator<=>(nullptr_t) const noexcept {
			uintmax_t i = _size - 1;
			if (_words[i] > WORD_MAX) return StrongOrder::less;
			while (i--) if (_words[i]) return StrongOrder::greater;
			return StrongOrder::equal;
		}
		//Binary Arithmetic
		friend constexpr BigInt operator+(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator-(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator*(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator/(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator%(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator&(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator|(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator^(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator<<(const BigInt& lhs, int rhs) noexcept;
		friend constexpr BigInt operator>>(const BigInt& lhs, int rhs) noexcept;
		friend constexpr BigInt operator==(const BigInt& lhs, const BigInt& rhs) noexcept;
		friend constexpr BigInt operator<=>(const BigInt& lhs, const BigInt& rhs) noexcept;
	private:
		BigInt _sext(unsigned int i) {
			_words[_size - 1] > WORD_MAX;
		}
	};
	constexpr BigInt operator+(const BigInt& lhs, const BigInt& rhs) noexcept {
		if (lhs == nullptr) return lhs;
		if (rhs == nullptr) return rhs;
		uintmax_t n = max(lhs.size(), rhs.size()),
			* w = new uintmax_t[n + 1]{0};
		bool flag = 0;
		for (uintmax_t i = 0; i < n; ++i) {
			uintmax_t a = lhs[i], b = rhs[i], c = w[i] = a + b + flag;
			flag = c < a || c < b;
		}
		const int shift = BigInt::WORD_BITS - 1,
			l = lhs[n - 1] >> shift, r = rhs[n - 1] >> shift; //either 0 or 1
		switch (l + r) {
			case 2: w[n] = -1; break;//lhs<0, rhs<0
			case 0: w[n] = 0;  break;//lhs>=0, rhs>=0
			case 1: w[n] = w[n - 1] >> shift ? -1 : 0;
		}
		return BigInt(n, w);
	}
	constexpr BigInt operator-(const BigInt& lhs, const BigInt& rhs) noexcept {
		if (lhs == nullptr) return lhs;
		uintmax_t n = max(lhs.size(), rhs.size()),
			* w = new uintmax_t[n + 1]{0};
		bool flag = 0;
		for (uintmax_t i = 0; i < n; ++i) {
			uintmax_t a = lhs[i], b = rhs[i], c = w[i] = a - b - flag;
			flag = c + flag > a;
		}
		const int shift = BigInt::WORD_BITS - 1,
			l = lhs[n - 1], r = rhs[n - 1]; //either 0 or 1
		if (l == r) w[n] = w[n - 1] >> shift ? -1 : 0;
		else w[n] = l < r ? -1 : 0;
		return BigInt(n, w);
	}
	constexpr BigInt operator*(const BigInt& lhs, const BigInt& rhs) noexcept {
		if (lhs == nullptr || rhs == nullptr) return 0;
		uintmax_t n = lhs.size() + rhs.size(),
			* w = new uintmax_t[n]{0};
		return BigInt(n, w);
	}
}
#endif
#pragma once
#include <assert.h>
#include "basics.hpp"
#ifndef _MATHLAB_HASH_
#define _MATHLAB_HASH_ 1
namespace Mathlab {
	template <Integral I> constexpr unsigned int BitWidth(const I& i) noexcept {
		return I(-1) < I(0) && i == 1 ? 2 : i ? BitWidth(i / 2) + 1 : 0;
	}
	//Rebindable and also used as iterator
	class BitReference {
		unsigned char _bit, * _ref;
		public:
		constexpr BitReference(unsigned char bit, unsigned char* ref) noexcept : _bit(bit), _ref(ref) { assert(bit >= 0 && bit <= 7); }
		constexpr const BitReference& operator=(bool b) const noexcept { b ? *_ref |= 1 << _bit : *_ref &= ~(1 << _bit); return *this; }
		constexpr       BitReference& operator=(bool b)       noexcept { b ? *_ref |= 1 << _bit : *_ref &= ~(1 << _bit); return *this; }
		constexpr operator bool() const { return *_ref & 1 << _bit; }
		constexpr BitReference& operator++()    noexcept { return _bit == 7 ? ++_ref, _bit = 0 : ++_bit, * this; }
		constexpr BitReference  operator++(int) noexcept { BitReference b{_bit++, _ref}; return _bit > 7 && (++_ref, _bit = 0), b; }
		constexpr BitReference& operator--()    noexcept { return _bit == 0 ? --_ref, _bit = 7 : --_bit, * this; }
		constexpr BitReference  operator--(int) noexcept { BitReference b{_bit--, _ref}; return _bit < 0 && (--_ref, _bit = 7), b; }
		constexpr BitReference  operator*()     noexcept { return *this; }
		constexpr BitReference* operator->()    noexcept { return this; }
		constexpr void flip() const { *_ref ^= 1 << _bit; }
		constexpr auto operator<=>(const BitReference& b) const noexcept { return 8 * (_ref - b._ref) <=> b._bit - _bit; }
	};

	template <size_t N> requires (N > 0) class Bitset {
		protected: unsigned char _data[N / 8 + 1];
		public:
		template <Integral I> Bitset(const I& _I) noexcept : _data{0} {
			for (size_t n = 0; n <= N / 8; ++n) _data[n] += _I, _I >>= 8;
			if (N % 8) N / 8 + 1 =
		}
		constexpr BitReference operator[](size_t _N) {
			return _N >= N ? BitReference{0, 0} : BitReference{_N % 8, _data + _N / 8};
		}
		template <Integral I> constexpr operator I() noexcept {
			I result = _data[0];
			if (I(-1) < I(0)) for (size_t n = 0; n < 8; ++n) result += _data[min(n, N - 1) / 8] << n / 8 * 8 & 1 << n;
			return result;
		}
		void print(bool _Endian = false) {
			if (printf("0x"), _Endian) for (unsigned char c : _data) printf("%02x", c);
			else for (unsigned char* u = _hash + sizeof(_T); u > _hash; printf("%02x", *--u));
		}
	};
	template <typename _T> class Hash : public Bitset<sizeof(N) * 8> {
		public:
		constexpr Hash(const _T& _T) : Bitset<sizeof(N) * 8>(0) {
			auto a = reinterpret_cast<const char*>(&_T);
			for (size_t i = 0; i < sizeof(_T); ++i) _hash[i] = *(a + i);
		}
	};
}
#endif
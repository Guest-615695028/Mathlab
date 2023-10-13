#pragma once
#include "basics.hpp"
#ifndef _BIGINT_HPP_
#define _BIGINT_HPP_ 0x8000'0000'0000'0000ul
namespace Mathlab {
	template <size_t _N, bool _Sgn = false> struct BigInt {
		alignas(_N) unsigned char bytes[_N];
		constexpr BigInt() : bytes{0} {}
		template <Integral _I> constexpr BigInt(const _I& i) : bytes{i & 255} {
			for (size_t n = 1, p = i < 0 ? 255 : 0; n < _N; ++n) bytes[n] = n < sizeof(_I) ? (i >>= 8) & 255 : p;
		}
		template <size_t _M, bool _S> constexpr BigInt(const BigInt<_M, _S>& i) : bytes{i.bytes[0]} {
			for (size_t n = 1, p = _S && i.bytes[_M - 1] > 128 ? 255 : 0; n < _N; ++n) bytes[n] = n < _M ? i.bytes[n] : p;
		}
		constexpr ~BigInt() { for (unsigned char& b : bytes) b = 0; }
		constexpr operator intmax_t() {
			intmax_t res = bytes[0];
			for (size_t n = 1; n < sizeof(intmax_t) && n < _N; ++n) res |= bytes[n] << n * 8;
			if (_Sgn) for (size_t n = _N; n < sizeof(intmax_t); ++n) res |= (intmax_t)255 << n * 8;
			return res;
		}
		constexpr operator uintmax_t() {
			uintmax_t res = bytes[0];
			for (size_t n = 1; n < sizeof(uintmax_t) && n < _N; ++n) res |= bytes[n] << n * 8;
			return res;
		}
	};
	template <size_t _M, size_t _N> constexpr BigInt<(_M < _N ? _N : _M)> operator+(BigInt<_M> x, BigInt<_N> y) {
		return BigInt < _M < _N ? _N : _M>(x) + BigInt < _M < _N ? _N : _M>(y);
	}
}
#endif
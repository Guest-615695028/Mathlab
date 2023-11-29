#pragma once
#ifndef _MATHLAB_POLY_
#define _MATHLAB_POLY_
#include "basics.hpp"
#include "iterator.hpp"
namespace Mathlab {
	template <Arithmetic _T> class Polynomial;
	namespace Mathlab {
		template <Arithmetic _T> class Polynomial {
			_T* _data;
			size_t _power;
		public:
			typedef _T ValueType;
			constexpr Polynomial(const _T& t = 0, const size_t p = 0) noexcept
				: _data(new _T[p + 1]{0}), _power(p) {
				_data[p] = t;
			}
			constexpr Polynomial(const Polynomial& r) noexcept
				: _data(new _T[r.power() + 1]{*r._data}), _power(r.power()) {
				for (size_t n = 1; n <= _power; ++n) {
					_data[n] = r._data[n];
				}
			}
			template <class _S> constexpr Polynomial(const Polynomial<_S>& r) noexcept
				: _data(new _T[r.power() + 1]{*r._data}), _power(r.power()) {
				for (size_t n = 1; n <= _power; ++n) {
					_data[n] = r._data[n];
				}
			}
			template <class _S> constexpr Polynomial(const InitializerList<_S>& r) noexcept
				: _data(new _T[r.size()]{*r.begin()}), _power(r.size() - 1) {
				for (size_t n = 1; n <= _power; ++n) {
					_data[n] = r.begin()[n];
				}
			}
			constexpr ~Polynomial() noexcept {
				delete[] _data;
				_power = 0;
			}
			constexpr Polynomial& operator=(const Polynomial& r) noexcept {
				delete[] _data;
				_data = new _T[(_power = r.power()) + 1]{*r._data};
				for (size_t n = 1; n <= _power; ++n) {
					_data[n] = r._data[n];
				}
				return *this;
			}
			template <class _S> constexpr Polynomial& operator=(const Polynomial<_S>& r) noexcept {
				delete[] _data;
				_data = new _T[(_power = r.power()) + 1]{*r._data};
				for (size_t n = 1; n <= _power; ++n) {
					_data[n] = r._data[n];
				}
				return *this;
			}
			// Iterators
			constexpr _T* begin() noexcept { return _data; }
			constexpr _T* end() noexcept { return _data + _power + 1; }
			constexpr ReverseIterator<_T*> rbegin() noexcept { return _data + _power; }
			constexpr ReverseIterator<_T*> rend() noexcept { return _data - 1; }
			constexpr const _T* begin() const noexcept { return _data; }
			constexpr const _T* end() const noexcept { return _data + _power + 1; }
			constexpr ReverseIterator<const _T*> rbegin() const noexcept { return _data + _power; }
			constexpr ReverseIterator<const _T*> rend() const noexcept { return _data - 1; }
			// Power
			constexpr size_t power(bool cap = false) const noexcept {
				if (cap) return _power;
				for (size_t n = _power; n; --n) if (_data[n]) return n;
				return 0;
			}
			// Coefficient
			constexpr _T& operator[](size_t n) noexcept {
				_T t = 0;
				return n <= _power ? _data[n] : t;
			}
			constexpr const _T& operator[](size_t n) const noexcept {
				return n <= _power ? _data[n] : 0;
			}
			// Evaluation
			template <Arithmetic _S> constexpr CommonType<_T, _S> operator()(const _S& x) const noexcept {
				size_t n = _power;
				CommonType<_T, _S> res = _data[n];
				while (n--) res = _data[n] + res * x;
				return res;
			}
			template <Arithmetic _S> constexpr CommonType<_T, _S> operator()(const Polynomial<_S>& x) const noexcept {
				size_t n = _power;
				Polynomial<CommonType<_T, _S>> res = _data[n];
				while (n--) res = _data[n] + res * x;
				return res;
			}
			// Reduce storage
			constexpr Polynomial& shrinkToFit() noexcept {
				size_t n = _power = power();
				_T* temp = new _T[++n]{*_data};
				while (--n) temp[n] = _data[n];
				delete[] _data;
				_data = temp;
				return *this;
			}
		};
		// Polynomial operations
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator+(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			size_t a = f.power(), b = g.power();
			Polynomial<Plus<_T, _S>> p(0, a < b ? b : a);
			for (size_t i = 0; i <= a || i <= b; ++i) p[i] = f[i] + g[i];
			return p.shrinkToFit();
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator+(const Polynomial<_T>& f, const _S& g) {
			return f + Polynomial<_S>(g);
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator+(const _T& f, const Polynomial<_S>& g) {
			return Polynomial<_T>(f) + g;
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Minus<_T, _S>> operator-(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			size_t a = f.power(), b = g.power();
			Polynomial<Plus<_T, _S>> p(0, a < b ? b : a);
			for (size_t i = 0; i <= a || i <= b; ++i) p[i] = f[i] - g[i];
			return p.shrinkToFit();
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator-(const Polynomial<_T>& f, const _S& g) {
			return f - Polynomial<_S>(g);
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator-(const _T& f, const Polynomial<_S>& g) {
			return Polynomial<_T>(f) - g;
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Multiplies<_T, _S>> operator*(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			size_t a = f.power(), b = g.power();
			Polynomial<Multiplies<_T, _S>> p(0, a + b);
			for (size_t i = 0; i <= a; ++i) for (size_t j = 0; j <= b; ++j) p[i + j] = f[i] * g[j];
			return p.shrinkToFit();
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator*(const Polynomial<_T>& f, const _S& g) {
			return f * Polynomial<_S>(g);
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator*(const _T& f, const Polynomial<_S>& g) {
			return Polynomial<_T>(f) * g;
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Divides<_T, _S>> operator/(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			size_t a = f.power(), b = g.power();
			if (a < b) return 0;
			Polynomial<Divides<_T, _S>> q(0, a - b);
			Polynomial<_T> r = f;
			for (size_t i = a; i >= b; --i) {
				auto& c = q[i - b] = r[i] / g[b];
				for (size_t j = r[i] = 0; j < b; ++j) r[i - b + j] -= c * g[j];
			}
			return q.shrinkToFit();
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Plus<_T, _S>> operator/(const Polynomial<_T>& f, const _S& g) {
			return f * Polynomial<_S>(1 / g);
		}
		template <class _T, class _S>
		inline constexpr Polynomial<Modulus<_T, _S>> operator%(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			size_t a = f.power(), b = g.power();
			if (a < b) return f;
			Polynomial<Modulus<_T, _S>> r = f;
			for (size_t i = a; i >= b; --i) {
				auto c = r[i] / g[b];
				for (size_t j = r[i] = 0; j < b; ++j) r[i - b + j] -= c * g[j];
			}
			return r;
		}
		template <class _T, class _S>
		inline constexpr bool operator==(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			int a = f.power(), b = g.power();
			if (a != b) return 0;
			for (int i = 0; i < a; ++i) if (f[a] != f[b]) return 0;
			return 1;
		}
		template <class _T, class _S>
		inline constexpr bool operator<(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			int a = f.power(), b = g.power();
			if (a != b) return a < b;
			do {
				if (f[a] != g[a]) return f[a] < g[a];
			} while (a--);
			return 0;
		}
#if !_OLD_CXX
		template <class _T, class _S>
		inline constexpr Comparison<_T, _S> operator<=>(const Polynomial<_T>& f, const Polynomial<_S>& g) {
			int a = f.power(), b = g.power();
			if (a != b) return a <=> b;
			do {
				if (f[a] != g[a]) return f[a] <=> g[a];
			} while (a--);
			return 1;
		}
#endif
	}
}
#endif
#pragma once
#ifndef _MATHLAB_VECTOR_
#define _MATHLAB_VECTOR_
#include <initializer_list>
#include "math.hpp"
namespace Mathlab {
	template <Arithmetic _T, size_t N> class Vector {
		static_assert(N > 0 && NumericType<_T>);
		_T _data[N];
	public:
		typedef _T ValueType;
		static constexpr size_t columns = N;
		constexpr Vector() noexcept = default;
		explicit constexpr Vector(const _T& t) noexcept : _data{ t } {
			for (_T& u : _data) u = t;
		}; //Zero Vector
		template <ConvertibleTo<_T> ..._S> constexpr Vector(const _S& ...s) noexcept
			: _data{ _T(s)... } {}
		template <Arithmetic _S> constexpr Vector(const _S(&il)[N]) noexcept : _data{ 0 } {
			size_t a = 0;
			for (_S s : il) _data[a++] = s;
		}
		template <Arithmetic _S> constexpr Vector(const Vector<_S, N>& other) noexcept : _data{ 0 } {
			for (size_t i = 0; i < N; ++i) _data[i] = other[i];
		}
		template <Arithmetic _S, size_t M, ConvertibleTo<_T>... _U>
		explicit constexpr Vector(const Vector<_S, M>& other, const _U&... u) noexcept : _data{ 0 } {
			static_assert(N > M && N >= M + sizeof...(_U));
			_T t[N - M] = { u... };
			for (size_t i = 0; i < N; ++i) _data[i] = i < M ? other[i] : t[i - M];
		}
		constexpr _T* begin() noexcept { return _data; }
		constexpr _T* end() noexcept { return _data + N; }
		constexpr const _T* begin() const noexcept { return _data; }
		constexpr const _T* end() const noexcept { return _data + N; }
		constexpr operator bool() noexcept {
			for (size_t i = 0; i < N; ++i) if (_data[i]) return true;
			return false;
		}
		constexpr operator _T() noexcept requires(N == 1) {
			return _data[0];
		}
		constexpr _T& operator[](size_t a) noexcept {
			return _data[a];
		}
		constexpr const _T& operator[](size_t a) const noexcept {
			return _data[a];
		}
		template <Arithmetic _S> constexpr Vector& operator+=(const Vector<_S, N>& other) noexcept {
			for (size_t i = 0; i < N; ++i) _data[i] += other._data[i];
			return *this;
		}
		template <Arithmetic _S> constexpr Vector& operator-=(const Vector<_S, N>& other) noexcept {
			for (size_t i = 0; i < N; ++i) _data[i] -= other._data[i];
			return *this;
		}
		template <Arithmetic _S> constexpr Vector& operator*=(const _S& s) noexcept {
			for (_T* t = begin(); t < end(); ++t) *t *= s;
			return *this;
		}
		template <Arithmetic _S> constexpr Vector& operator/=(const _S& s) noexcept {
			for (_T* t = begin(); t < end(); ++t) *t /= s;
			return *this;
		}
		template <Arithmetic _S> constexpr bool operator==(const Vector<_S, N>& rhs) const noexcept {
			for (size_t i = 0; i < N; i++) for (size_t j = 0; j < N; j++) if (_data[i] != rhs._data[i]) return false;
			return true;
		}
	};
	//1 Operators
	template <class _T, class _S, size_t N> inline constexpr auto operator+(const Vector<_T, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		auto a = lhs[0] + rhs[0];
		Vector<decltype(a), N> v = { a };
		for (size_t i = 1; i < N; ++i) v[i] = lhs[i] + rhs[i];
		return v;
	}
	template <class _T, class _S, size_t N> inline constexpr auto operator-(const Vector<_T, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		auto a = lhs[0] - rhs[0];
		Vector<decltype(a), N> v = { a };
		for (size_t i = 1; i < N; ++i) v[i] = lhs[i] - rhs[i];
		return v;
	}
	template <class _T, class _S, size_t N> inline constexpr auto operator*(const Vector<_T, N>& lhs, const _S& rhs) noexcept {
		auto a = lhs[0] * rhs;
		Vector<decltype(a), N> v = { a };
		for (size_t i = 1; i < N; ++i) v[i] = lhs[i] * rhs;
		return v;
	}
	template <class _T, class _S, size_t N> inline constexpr auto operator/(const Vector<_T, N>& lhs, const _S& rhs) noexcept {
		auto a = lhs[0] / rhs;
		Vector<decltype(a), N> v = { a };
		for (size_t i = 1; i < N; ++i) v[i] = lhs[i] / rhs;
		return v;
	}
	template <class _T, class _S, size_t N> inline constexpr auto operator*(const _T& lhs, const Vector<_S, N>& rhs) noexcept {
		return rhs * lhs;
	}
	template <class _T, class _S, size_t N>	inline constexpr auto operator*(const Vector<_T, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		auto a = lhs[0] * conj(rhs[0]);
		for (size_t i = 1; i < N; ++i) a += lhs[i] * conj(rhs[i]);
		return a;
	}
	template <class _T, size_t N> inline constexpr Vector<_T, N> operator+(const Vector<_T, N>& m) noexcept {
		return m * 1;
	}
	template <class _T, size_t N> inline constexpr Vector<_T, N> operator-(const Vector<_T, N>& m) noexcept {
		return m * -1;
	}
	//2 Vector operations
	template <class _T, size_t N> inline constexpr _T abs(const Vector<_T, N>& v) noexcept {
		long double r = 0;
		for (_T t : v) {
			if (isnan(t)) return limits(t).quiet_NaN();
			if (isinf(t)) return limits(t).infinity();
			r = Mathlab::hypot(r, t);
		}
		return r;
	}
	template <class _T, class _S, size_t N> inline constexpr CommonType<_T, _S> angle(const Vector<_T, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		return acos(lhs * rhs / abs(lhs) / abs(rhs));
	}
	template <class _T, class _S, size_t N> inline constexpr CommonType<_T, _S> operator|(const Vector<_T, N>& lhs, const Vector<_S, N>& rhs) noexcept {
		return acos(lhs * rhs / abs(lhs) / abs(rhs));
	}
	template <class _T, size_t N> inline constexpr Vector<_T, N> unitVector(const Vector<_T, N>& v) noexcept {
		return v / abs(v);
	}
	template <class _T, size_t N> inline constexpr Vector<_T, N> operator*(const Vector<_T, N>& v) noexcept {
		return v / abs(v);
	}
}
#endif
#pragma once
#include "math.hpp"
#include "function.hpp"
namespace Mathlab {
	//0 Index Sequences
	template <size_t... N> using Indices = ::std::index_sequence<N...>;
	template <size_t N> using OneTo = ::std::make_index_sequence<N>; //actually from 0 to N-1
	template <typename... _T> using IndicesFor = ::std::index_sequence_for<_T...>;
	//1 Differentiation
	template <Arithmetic _T, Arithmetic _U> constexpr Function<_T, _U> diff(const Function<_T, _U>& f, _U dx = 0x1p-16) _noexcept_as(f(_U())) {
		return [=](_U x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	template <Arithmetic _T, Arithmetic _U> constexpr Function<_T, _U> diff(_T(*f)(_U), _U dx = 0x1p-16) _noexcept_as(f(_U())) {
		return [=](_U x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	template <Arithmetic _T, Invocable<_T> _F> constexpr auto diff(_F f, _T dx = 0x1p-16) _noexcept_as(f(_T())) -> Function<decltype(f(_T())), _T> {
		return [=](_T x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	//1.1 Partial Differential
	template <Arithmetic _T, Arithmetic... _U, size_t... N>
	constexpr Function<_T, _U...> partial(const Function<_T, _U...>& f, size_t n, long double dx = 0x1p-16, Indices<N...> i = IndicesFor<_U...>()) _noexcept_as(f) {
		return [=](_U u...)->_T {
			if (n > sizeof...(_U)) return 0;
			CommonType<_U...> x[] = {u...};
			x[n] += dx / 2;
			_T t = f(x[N]...);
			x[n] -= dx;
			return t - f(x[N]...) / dx;
		};
	}
	template <Arithmetic _T, Arithmetic... _U, size_t... N>
	constexpr Function<_T, _U...> partial(_T(*f)(_U...), size_t n, long double dx = 0x1p-16, Indices<N...> i = IndicesFor<_U...>()) _noexcept_as(f) {
		return [=](_U u...)->_T {
			if (n > sizeof...(_U)) return 0;
			CommonType<_U...> x[] = {u...};
			x[n] += dx / 2;
			_T t = f(x[N]...);
			x[n] -= dx;
			return t - f(x[N]...) / dx;
		};
	}
	//2 Integration
	template <Arithmetic _T, Arithmetic _U> constexpr _T integrate(const Function<_T, _U>& f, _U a, _U b, _U dx = 0x1p-16) {
		if (isnan(b - a) || dx <= 0) return nan(EDOM);
		else if (a == b) return 0;
		else if (a > b) return -integrate(f, b, a, dx);
		else if (a + dx >= b) return (f(a) + f(b)) * 0.5f * (b - a);
		else return (f(a) + f(a + dx)) * 0.5f * dx + integrate(f, a + dx, b, dx);
	}
	//3 Solve Equation
	template <Arithmetic _T, Invocable<_T> _F> constexpr _T solve(const _F& f, _T a, _T dx = 0x1p-16) {
		long double t = f(a);
		if (!t) return a;
		Function<long double, _T> df = diff(Function<long double, _T>(f), dx);
		return solve(f, a - t / df(a), dx);
	}
}
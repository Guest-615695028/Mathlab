#pragma once
#include "math.hpp"
#include "function.hpp"
namespace Mathlab {
	//0 Index Sequences
	template <size_t... N> using Indices = ::std::index_sequence<N...>;
	template <size_t N> using OneTo = ::std::make_index_sequence<N>; //actually from 0 to N-1
	template <typename... _T> using IndicesFor = ::std::index_sequence_for<_T...>;
	//1 Differentiation
	template <Arithmetic _T, Arithmetic _U> constexpr Function<_T, _U> diff(const Function<_T, _U>& f, _U dx = 0x1p-16) _NOEXCEPT_AS_(f(_U())) {
		return [=](_U x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	template <Arithmetic _T, Arithmetic _U> constexpr Function<_T, _U> diff(_T(*f)(_U), _U dx = 0x1p-16) _NOEXCEPT_AS_(f(_U())) {
		return [=](_U x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	template <Arithmetic _T, Invocable<_T> _F> constexpr auto diff(_F f, _T dx = 0x1p-16) _NOEXCEPT_AS_(f(_T())) -> Function<decltype(f(_T())), _T> {
		return [=](_T x) {
			return (f(x + dx / 2) - f(x - dx / 2)) / dx;
		};
	}
	//1.1 Partial Differential
	template <Arithmetic _T, Arithmetic... _U, size_t... N>
	constexpr Function<_T, _U...> partial(const Function<_T, _U...>& f, size_t n, long double dx = 0x1p-16, Indices<N...> i = IndicesFor<_U...>()) _NOEXCEPT_AS_(f) {
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
	constexpr Function<_T, _U...> partial(_T(*f)(_U...), size_t n, long double dx = 0x1p-16, Indices<N...> i = IndicesFor<_U...>()) _NOEXCEPT_AS_(f) {
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
	template <Arithmetic _T, Invocable<_T> _F>
	constexpr auto integrate(const _F& f, _T a, _T b, _T dx = 0x1p-16) -> decltype(_F()(_T())) {
		if (isnan(b - a) || dx <= 0) return nan(EDOM);
		else if (a == b) return 0;
		else if (a > b) return -integrate(f, b, a, dx);
		else if (a + dx >= b) return (f(a) + f(b)) * 0.5f * (b - a);
		auto result = (f(a) + f(b)) * 0.5 * dx;
		for (_T x = a; x < b; x += dx) r += (f(x) + f(x + dx)) * dx;
	}
	//3 Solve Equation
	//3.1 Using Newton Method at x=a
	template <Arithmetic _T, Invocable<_T> _F>
	constexpr _T solveNewton(const _F& f, _T a, _T dx = 0x1p-16) {
		auto t = f(a);
		if (!t) return a;
		auto df = diff(Function<decltype(t), _T>(f), dx);
		return solve(f, a - t / df(a), dx);
	}
	//3.1 Using Binary Search between x=a and x=b
	template <Arithmetic _T, Invocable<_T> _F>
	constexpr _T solveBinary(const _F& f, _T a, _T b, _T dx = 0x1p-16) {
		auto c = f(a), d = f(b);
		if (c == 0) return a;
		else if (d == 0) return b;
		if (c < 0 && d>0) {
			auto e = f((a + b) / 2);
		}
	}
	//4 Solve Differential Equation
	template <Arithmetic _T, Invocable<_T, _T> _F>
	constexpr _T rungeKutta(const _F& f, _T a, _T b, _T dx = 0x1p-16) {
		auto c = f(a), d = f(b);
		if (c == 0) return a;
		else if (d == 0) return b;
		if (c < 0 && d>0) {
			auto e = f((a + b) / 2);
		}
	}
}
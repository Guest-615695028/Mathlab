#pragma once
#ifndef _MATHLAB_ITERATOR_
#define _MATHLAB_ITERATOR_
#include <initializer_list>
#include "basics.hpp"
namespace Mathlab {
	template <LegacyIterator _I, Integral _N> inline constexpr _I advance(_I i, _N n)
		_NOEXCEPT_AS_(++i) {
		if constexpr (LegacyRandomAccessIterator<_I>) return i + n;
		else if (n >= 0) while (n--) ++i;
		else if constexpr (LegacyBidirectionalIterator<_I>) while (n++) --i;
		else throw Error(&i, "Trying to decrement a forward-only iterator");
		return i;
	}
	template <LegacyIterator _I> inline constexpr ptrdiff_t distance(_I i, _I j)
		_NOEXCEPT_AS_(++i) {
		if constexpr (LegacyRandomAccessIterator<_I>) return j - i;
		size_t n = 0;
		while (i != j) ++n, ++i;
		return n;
	}
	//1.1 Reversed Iterator
	template <LegacyBidirectionalIterator _T> class ReverseIterator {
		_T _base;
	public:
		constexpr ReverseIterator() noexcept = default;
		constexpr ReverseIterator(_T t) noexcept : _base(t) {}
		constexpr ReverseIterator operator+() const noexcept { return *this; }
		constexpr ReverseIterator& operator++() noexcept { --_base; return *this; }
		constexpr ReverseIterator operator++(int) noexcept { return _base--; }
		constexpr ReverseIterator& operator+=(ptrdiff_t i) noexcept { _base -= i; return *this; }
		constexpr ReverseIterator operator+(ptrdiff_t i) const noexcept { return _base - i; }
		constexpr ReverseIterator& operator--() noexcept { ++_base; return *this; }
		constexpr ReverseIterator operator--(int) noexcept { return _base++; }
		constexpr ReverseIterator& operator-=(ptrdiff_t i) noexcept { _base += i; return *this; }
		constexpr ReverseIterator operator-(ptrdiff_t i) const noexcept { return _base + i; }
		constexpr auto operator-(ReverseIterator t) { return t._base - _base; }
		constexpr _T base() const noexcept { return _base; }
		constexpr operator _T () const noexcept { return _base; }
		constexpr _T operator->() const noexcept { return _base; }
		constexpr auto& operator*() const noexcept { return *_base; }
		constexpr auto& operator[](size_t i) const noexcept { return *(_base - i); }
		constexpr auto operator<=>(const ReverseIterator& r) noexcept { return r._base <=> _base; }
		constexpr bool operator==(const ReverseIterator& r) noexcept { return r._base == _base; }
		constexpr bool operator!=(const ReverseIterator& r) noexcept { return r._base != _base; }
		constexpr bool operator<(const ReverseIterator& r) noexcept { return r._base < _base; }
		constexpr bool operator<=(const ReverseIterator& r) noexcept { return r._base <= _base; }
		constexpr bool operator>(const ReverseIterator& r) noexcept { return r._base > _base; }
		constexpr bool operator>=(const ReverseIterator& r) noexcept { return r._base >= _base; }
	};
	template <class _T> inline constexpr ReverseIterator<_T>
	operator+(ptrdiff_t n, ReverseIterator<_T> i) { return i + n; }
}
#endif
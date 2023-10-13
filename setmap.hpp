#pragma once
#include "basics.hpp"

namespace Mathlab {
	template <class _T> class Multiset {
		protected:
		_T* _begin;
		size_t _size;
		public:
		typedef _T ValueType, * Iterator;
		constexpr Multiset() : _begin(0), _size(0) {}
		constexpr Multiset(const _T& t) : _begin(new _T(t)), _size(0) {}
		constexpr Multiset(_T&& t) : _begin(new _T(t)), _size(0) {}
		constexpr ~Multiset() { delete _begin };
	};
	template <class _T> class Set : public Multiset {
	};
	//Basic operations
	//Set intersection
	template <class K>
	Set<K> operator*(const Set<K>& lhs, const Set<K>& rhs) {
		Set<K> a;
		for (K& k : lhs) if(rhs.count(k)) a.insert(k);
		return lhs;
	}
	template <class K>
	Multiset<K> operator*(const Multiset<K>& lhs, const Multiset<K>& rhs) {
		Multiset<K> a;
		for (K& k : lhs) if (a.count(k) < rhs.count(k)) a.insert(k);
		return lhs;
	}
	//Set union
	template <class K>
	Set<K> operator+(const Set<K>& lhs, const Set<K>& rhs) {
		Set<K> a=lhs;
		for (const K& k : rhs) a.insert(k);
		return lhs;
	}
	template <class K>
	Multiset<K> operator+(const Multiset<K>& lhs, const Multiset<K>& rhs) {
		Multiset<K> a=lhs;
		for (const K& k : a) while (a.count(k)<rhs.count(k)) a.insert(k);
		return lhs;
	}
	//Set difference
	template <class K>
	Set<K> operator-(const Set<K>& lhs, const Set<K>& rhs) {
		Set<K> a=lhs;
		for (K& k : a) rhs.count(k) && a.erase(k);
		return a;
	}
	template <class K>
	Multiset<K> operator-(const Multiset<K>& lhs, const Multiset<K>& rhs) {
		Multiset<K> a=lhs;
		for (K& k : a) rhs.count(k) && a.erase(&k);
		return a;
	}
	//Set symmetric difference
	template <class K>
	Set<K> operator/(const Set<K>& lhs, const Set<K>& rhs) {
		return lhs+rhs-lhs*rhs;
	}
	template <class K>
	Multiset<K> operator/(const Multiset<K>& lhs, const Multiset<K>& rhs) {
		return lhs+rhs-lhs*rhs;
	}
	//Set comparisons
	template <class K>
	bool operator==(const Set<K>& lhs, const Set<K>& rhs) {
		for (K k : lhs + rhs) if (lhs.count(k) != rhs.count(k)) return 0;
		return 1;
	}
	template <class K>
	bool operator==(const Multiset<K>& lhs, const Multiset<K>& rhs) {
		for (K k : lhs + rhs) if (lhs.count(k) != rhs.count(k)) return 0;
		return 1;
	}
	//Map operations
#if __cplusplus>=202000L
	template <class Set, class Ostream> Ostream& operator<<(Ostream& o, Set arg) {
		std::ostringstream s;
		s.flags(o.flags());
		s.imbue(o.getloc());
		s.precision(o.precision());
		s << '{';
		for (auto& a : arg) s << raw(a) << ", ";
		return os << (s << "/}").str();
	}
#endif
}

#pragma once
#include <initializer_list>
#include "basics.hpp"
namespace Mathlab {
	//1 Iterator functions
	template <LegacyIterator _I, Integral _N> inline constexpr _I advance(_I i, _N n = 1) noexcept(requires(_I i) {
		{ ++i } noexcept;
		{ --i } noexcept;
	}) {
		if constexpr (LegacyRandomAccessIterator<_I>) return i + n;
		else if (n >= 0) while (n--) ++i;
		else if constexpr (LegacyBidirectionalIterator<_I>) while (n++) --i;
		else throw Error(&i, "Trying to decrement a forward-only iterator");
		return i;
	}
	template <LegacyIterator _I> inline constexpr ptrdiff_t distance(_I i, _I j) _noexcept_as(++i) {
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
		constexpr auto& operator*() const noexcept { return *(_base); }
		constexpr auto& operator[](size_t i) const noexcept { return (_base - i)[0]; }
		constexpr auto operator<=>(const ReverseIterator& r) noexcept { return r._base <=> _base; }
		constexpr bool operator==(const ReverseIterator& r) noexcept { return r._base == _base; }
	};
	template <class _T> inline constexpr ReverseIterator<_T> operator+(ptrdiff_t n, ReverseIterator<_T> i) { return i + n; }

	//2 Linear List by contiguous storage
	template <NumericType _T> class LinearList {
		public:
		typedef _T ValueType, * Iterator;
		typedef Mathlab::ReverseIterator<_T*> ReverseIterator;
		constexpr LinearList() noexcept : _begin(0), _size(0), _cap(0) {}
		constexpr LinearList(size_t n, const _T& t = _T()) noexcept : _begin(new _T[n]), _size(n), _cap(n) {
			for (size_t i = 0; i < _size; ++i) _begin[i] = t;
		}
		template <class _S> constexpr LinearList(const LinearList<_S>& r) noexcept : _begin(new _T[r._size]), _size(r._size), _cap(r._size) {
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
		}
		constexpr LinearList(LinearList&& r) noexcept : _begin(r._begin), _size(r._size), _cap(r._cap) {}
		template <LegacyIterator _I> constexpr LinearList(_I f, _I l) _noexcept_as(++f) : _begin(0), _size(distance(f, l)), _cap(0) {
			_begin = new _T[_cap = _size];
			size_t s = 0;
			while (f != l) _begin[s++] = *f++;
		}
		template <Range _R> constexpr LinearList(const _R& r) noexcept : LinearList(begin(r), end(r)) {}
		template <class _S> constexpr LinearList(const InitializerList<_S>& r) noexcept : _begin(0), _size(r.size()), _cap(0) {
			_begin = new _T[_cap = _size];
			size_t s = 0;
			for (_S a : r) _begin[s++] = a;
		}
		constexpr ~LinearList() noexcept { if (_begin) delete[] _begin; _size = _cap = 0; }
		constexpr LinearList& operator=(const LinearList& r) noexcept {
			_size = r._size;
			if (_cap < _size && _begin) delete[] _begin, _begin = 0; //Appealing for reallocation
			if (!_begin) _begin = new _T[_cap = _size];
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
			return *this;
		}
		//Iterators and capacity
		constexpr _T* begin()  noexcept { return _begin; }
		constexpr _T* end() noexcept { return _begin + _size; }
		constexpr ReverseIterator rbegin() const noexcept { return _begin + _size - 1; }
		constexpr ReverseIterator rend() const noexcept { return _begin - 1; }
		constexpr bool empty() const noexcept { return !_size; }
		constexpr operator bool() const noexcept { return _size; }
		constexpr size_t size() const noexcept { return _size; }
		//Element access, starting at 0
		constexpr _T& operator[](size_t n) { return n >= _size ? throw Error(this, "Out of range") : _begin[n]; }
		constexpr const _T& operator[](size_t n) const { return n >= _size ? throw Error(this, "Out of range") : _begin[n]; }
		//Insert
		constexpr size_t insert(size_t p, const _T& t) noexcept;
		template <LegacyInputIterator I> constexpr size_t insert(size_t p, const I& f, const I& l) noexcept;
		constexpr size_t append(const _T& t) noexcept;
		template <LegacyInputIterator I> constexpr size_t append(const I& f, const I& l) noexcept;
		constexpr LinearList& operator+=(const _T& t) noexcept;
		//Erase
		constexpr LinearList& operator-=(const _T& t) noexcept;
		constexpr size_t erase(const _T& t, bool all = true) noexcept;
		constexpr size_t eraseAt(size_t p) noexcept;
		template <Predicate<_T> _Fn> constexpr size_t eraseIf(const _Fn& f, bool all = true) noexcept;
		constexpr size_t clear() noexcept;
		constexpr void shrinkToFit() noexcept;
		//Emplace
		constexpr size_t emplace(size_t p, const _T& t) noexcept;
		//Find
		constexpr size_t find(const _T& t, bool = false) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t find(const _Fn& f, bool = false) const noexcept;
		constexpr size_t count(const _T& t) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t count(const _Fn& f) const noexcept;
		constexpr void swap(LinearList& r) noexcept { Mathlab::swap(_begin, r._begin), Mathlab::swap(_size, r._size); }
		protected:
		_T* _begin;
		size_t _size, _cap;
	};
	//2.1 Insertion
	//2.1.1 Insert one element t at position p
	template <NumericType _T> constexpr size_t LinearList<_T>::insert(size_t p, const _T& t) noexcept try {
		if (p > _size) return 0;
		else if (_cap == 0) return _begin = new _T[_size = _cap = 1]{t}, 1;
		else if (_cap <= _size) {//Expand allocation size
			_T* temp = new _T[_cap += _size];
			for (size_t i = 0; i < _size; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
		for (size_t n = _size++; n > p; --n) _begin[n] = _begin[n - 1];
		_begin[p] = t;
		return 1;
	} catch (...) {
		return 0;
	}
	//2.1.2 Insert a sequence [f,l) at position p
	template <NumericType _T> template <LegacyInputIterator I> constexpr size_t LinearList<_T>::insert(size_t p, const I& f, const I& l) noexcept {
		if (p > _size) return 0;
		size_t k = distance(f, l), n = _size += k, r = 0;
		try {
			if (_cap == 0) _begin = new _T[_size = _cap = k]{};
			else if (_cap < _size) {//Expand allocation size
				_T* temp = new _T[_cap += _size];
				for (size_t i = 0; i < _size - k; ++i) temp[i] = _begin[i];
				delete[] _begin, _begin = temp;
			}
			while (--n >= p + k) _begin[n] = _begin[n - k];
			for (I i = f; i != l; ++i) _begin[p++] = *i, ++r;
		} catch (...) {
		}
		return r;
	}
	//2.1.3 Append an element to the end
	template <NumericType _T> constexpr size_t LinearList<_T>::append(const _T& t) noexcept {
		if (_cap == 0) return _begin = new _T[_size = _cap = 1]{t}, 1;
		else if (_cap <= _size) {//Expand allocation size
			_T* temp = new _T[_cap += _size];
			for (size_t i = 0; i < _size; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
		return _T[++_size] = t, 1;
	}
	template <NumericType _T> constexpr LinearList<_T>& LinearList<_T>::operator+=(const _T& t) noexcept {
		append(t);
	}
	//2.1.4 Append a sequence [f,l) to the end
	template <NumericType _T> template <LegacyInputIterator I> constexpr size_t LinearList<_T>::append(const I& f, const I& l) noexcept {
		size_t n = 0;
		for (I i = f; i != l; ++i) n += append(*i);
		return n;
	}
	//2.2 Erasure
	//2.2.1 Erase the element at position p
	template <NumericType _T> constexpr size_t LinearList<_T>::eraseAt(size_t p) noexcept {
		if (p > _size) return 0;
		for (size_t n = p; n < _size; ++n) _begin[n] = _begin[n + 1];
		_begin = new(_begin) _T[_size -= 1];
		return 1;
	}
	//2.2.2 Erase all elements/the first element satisfying criterion f
	template <NumericType _T> template <Predicate<_T> _Fn> constexpr size_t LinearList<_T>::eraseIf(const _Fn& f, bool all) noexcept {
		size_t i = 0; //flag
		for (size_t n = 0; n < _size; ++n) i += (all || !i) && f(_begin[n]), _begin[n] = _begin[n + i];
		return _size -= i, i;
	}
	//2.2.3 Erase all elements/the first element equal to t
	template <NumericType _T> constexpr size_t LinearList<_T>::erase(const _T& t, bool all) noexcept {
		size_t i = 0; //flag
		for (size_t n = 0; n < _size; ++n) i += (all || !i) && _begin[n] == t, _begin[n] = _begin[n + i];
		return _size -= i, i;
	}
	//2.2.4 Erase the first element equal to t
	template <NumericType _T> constexpr LinearList<_T>& LinearList<_T>::operator-=(const _T& t) noexcept {
		bool i = false; //flag
		for (size_t n = 0; n < _size; ++n) i = i || _begin[n] == t, _begin[n] = _begin[n + i];
		return _size -= i, *this;
	}
	//2.2.5 Clear the list
	template <NumericType _T> constexpr size_t LinearList<_T>::clear() noexcept {
		if (_begin) delete[] _begin, _begin = 0;
		_size = _cap = 0;
	}
	//2.2.6 Remove unused space
	template <NumericType _T> constexpr void LinearList<_T>::shrinkToFit() noexcept {
		if (_size < _cap) {
			_T* temp = new _T[_cap = _size];
			for (size_t i = 0; i < _size; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
	}
	//2.3 Emplace
	template <NumericType _T> constexpr size_t LinearList<_T>::emplace(size_t p, const _T& t) noexcept {
		return p < size && (_begin[p] = t, 1);
	}
	//2.4 Find
	template <NumericType _T> constexpr size_t LinearList<_T>::find(const _T& t, bool b) const noexcept {
		if (b) for (size_t i = _size - 1; i >= 0; ++i) if (_begin[i] == t) return i;
		else for (size_t i = 0; i < _size; ++i) if (_begin[i] == t) return i;
		return -1;
	}
	template <NumericType _T> template <Predicate<_T> _Fn> constexpr size_t LinearList<_T>::find(const _Fn& f, bool b) const noexcept {
		for (size_t i = 0; i < _size; ++i) if (f(_begin[i])) return i;
		return -1;
	}
	//2.5 Count
	template <NumericType _T> constexpr size_t LinearList<_T>::count(const _T& t) const noexcept {
		size_t n = 0;
		for (size_t i = 0; i < _size; ++i) n += _begin[i] == t;
		return n;
	}
	template <NumericType _T> template <Predicate<_T> _Fn> constexpr size_t LinearList<_T>::count(const _Fn& f) const noexcept {
		size_t n = 0;
		for (size_t i = 0; i < _size; ++i) n += f(_begin[i]);
		return n;
	}
	//2.6 Equality and comparison
	template <class _T, class _S> constexpr bool operator==(const LinearList<_T>& t, const LinearList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a++ != *c++) return false;
		}
		return a == b && c == d;
	}
	template <class _T, class _S> constexpr bool operator<(const LinearList<_T>& t, const LinearList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a < *c) return true;
			else if (*a++ > *c++) return false;
		}
		return c < d;
	}
	#if !_OLD_CXX
	template <class _T, class _S> Comparison<_T, _S> constexpr operator<=>(const LinearList<_T>& t, const LinearList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			Comparison<_T, _S> x = *a++ <=> *c++;
			if (x != 0) return x;
		}
		return {0};
	}
	#endif
	//3 Linear List by linkage
	template <NumericType _T, bool B = false> class LinkedList {
		struct _True {
			_T value;
			_True* next, * prev;
		};
		struct _False {
			_T value;
			_False* next;
		};
		public:
		typedef _T ValueType;
		typedef typename _TypeHolder<_True, B, _False>::type NodeType;
		class Iterator;
		using ReverseIterator = typename _TypeHolder<ReverseIterator<typename _TypeHolder<Iterator, B, _T*>::type>, B, void>::type;
		constexpr LinkedList() noexcept : _head(0), _tail(0) {}
		constexpr LinkedList(size_t n = 0, const _T& t = _T()) noexcept : _head(n ? new NodeType{t, {0}} : 0), _tail(_head) {
			NodeType* p = _head;
			while (--n) {
				p->next = new NodeType{t, 0};
				if constexpr (B) p->next->prev = p;
				_tail = p = p->next;
			}
		}
		template <LegacyIterator _I> constexpr LinkedList(_I f, _I l) noexcept : _head(0), _tail(0) {
			if (f == l) return;
			NodeType* p = _tail = _head = new NodeType{*f++, 0};
			while (f != l) {
				p->next = new NodeType{*f++, 0};
				if constexpr (B) p->next->prev = p;
				_tail = p = p->next;
			}
		}
		template <Range _R> constexpr LinkedList(const _R& r) noexcept : LinkedList(begin(r), end(r)) {}
		template <class _S> constexpr LinkedList(const InitializerList<_S>& r) noexcept : LinkedList(r.begin(), r.end()) {}
		template <class _S> constexpr LinkedList(const LinkedList<_S>& r) noexcept : LinkedList(r.begin(), r.end()) {}
		constexpr LinkedList(LinkedList&& r) noexcept : _head(r._head), _tail(r._tail) {}
		constexpr ~LinkedList() noexcept { clear(); }
		template <Range _R> constexpr LinkedList& operator=(const _R& r) noexcept { return LinkedList(r).swap(*this), * this; }
		template <class _S> constexpr LinkedList& operator=(const InitializerList<_S>& r) noexcept { return LinkedList(r).swap(*this), * this; }
		template <class _S> constexpr LinkedList& operator=(const LinkedList<_S>& r) noexcept { return LinkedList(r).swap(*this), * this; }
		//Member access
		constexpr Iterator begin() const noexcept { return {_head, this}; }
		constexpr Iterator end() const noexcept { return {0, this}; }
		constexpr Iterator rbegin() const noexcept { return {_tail, this}; }
		constexpr Iterator rend() const noexcept { return {0, this}; }
		constexpr bool empty() const noexcept { return !_head; }
		constexpr operator bool() const noexcept { return _head; }
		constexpr size_t size() const noexcept {
			NodeType* p = _head; size_t n = 0;
			while (p) ++n, p = p->next;
			return n;
		}
		constexpr _T& operator[](size_t n) {
			NodeType* p = _head;
			while (n--) p = p ? p->next : throw Error(this, "Out of range");
			return p->value;
		}
		constexpr const _T& operator[](size_t n) const {
			NodeType* p = _head;
			while (n--) p = p ? p->next : throw Error(this, "Out of range");
			return p->value;
		}
		//Insert
		constexpr size_t insert(size_t p, const _T& t) noexcept;
		template <LegacyInputIterator I> constexpr size_t insert(size_t p, const I& f, const I& l) noexcept;
		constexpr size_t append(const _T& t) noexcept;
		template <LegacyIterator I> constexpr size_t append(const I& f, const I& l) noexcept;
		constexpr LinkedList& operator+=(const _T& t) noexcept;
		//Erase
		constexpr LinkedList& operator-=(const _T& t) noexcept;
		constexpr size_t erase(const _T& t, bool all = true) noexcept;
		constexpr size_t eraseAt(size_t p) noexcept;
		template <Predicate<_T> _Fn> constexpr size_t eraseIf(const _Fn& f, bool all = true) noexcept;
		constexpr size_t clear() noexcept;
		//Emplace
		constexpr size_t emplace(size_t p, const _T& t) noexcept;
		//Find
		constexpr size_t find(const _T& t, bool = false) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t find(const _Fn& f, bool = false) const noexcept;
		constexpr size_t count(const _T& t) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t count(const _Fn& f) const noexcept;
		//Swap
		constexpr void swap(LinkedList& r) noexcept { Mathlab::swap(_head, r._head), Mathlab::swap(_tail, r._tail); }
		protected: NodeType* _head, * _tail;
	};
	template <NumericType _T, bool B> class LinkedList<_T, B>::Iterator {
		NodeType* _base;
		LinkedList* _obj;
		public:
		constexpr Iterator(NodeType* n = 0, LinkedList* l = 0) noexcept : _base(n), _obj(l) {}
		constexpr Iterator& operator++() noexcept { return _base = _base ? _base->next : _obj->_head, *this; }
		constexpr _T& operator*() const noexcept { return _base->value; }
		constexpr _T* operator->() const noexcept { return &_base->value; }
		constexpr bool operator==(const Iterator& r) const noexcept { return _base == r._base; }
		constexpr Iterator& operator--() noexcept requires(B) { return _base = _base ? _base->prev : _obj->_tail, *this; }
	};
	//3.1 Insert
	//3.1.1 Insert one element t at position p
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::insert(size_t p, const _T& t) noexcept {
		NodeType* x = _head = p ? _head : new NodeType{t, _head};
		if (p) {
			while (--p) {
				if (x) x = x->next; else return 0;
			}
			x->next = new NodeType{t, x->next};
		}
		if constexpr (B) x->next->prev = x;
		if (!x->next->next) _tail = x->next;
		return 1;
	}
	//3.1.2 Insert a sequence [f,l) at position p
	template <NumericType _T, bool B> template <LegacyInputIterator I> constexpr size_t LinkedList<_T, B>::insert(size_t p, const I& f, const I& l) noexcept {
		if (f == l) return 0;
		NodeType* x = _head = p ? _head : new NodeType{*f++, _head};
		if (p) while (--p) {
			if (x) x = x->next; else return 0;
		} else x->next->prev = x;
		while (f != l) {
			x->next = new NodeType{*f++, x->next};
			if constexpr (B) x->next->prev = x;
			x = x->next;
		}
		if (!x->next) _tail = x;
		return 1;
	}
	//3.1.3 Append one element t to the end
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::append(const _T& t) noexcept {
		_tail = _tail->next = new NodeType(t, 0);
		return 1;
	}
	template <NumericType _T, bool B> constexpr LinkedList<_T, B>& LinkedList<_T, B>::operator+=(const _T& t) noexcept {
		_tail = _tail->next = new NodeType(t, 0);
		return *this;
	}
	//3.1.4 Append a sequence [f,l) to the end
	template <NumericType _T, bool B> template <LegacyIterator I> constexpr size_t LinkedList<_T, B>::append(const I& f, const I& l) noexcept {
		size_t n = 0;
		for (I i = f; i != l; ++i) ++n, _tail = _tail->next = new NodeType(*i, 0);
		return n;
	}
	//3.2 Erase
	//3.2.1 Erase all elements/the first element
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::eraseAt(size_t p) noexcept {
		NodeType* x = _head;
		if (!p) return _head && (delete x, _head = _head->next, 1);
		while (--p) if (!(x = x->next)) return 0;
		NodeType* y = x->next;
		if constexpr (B) y->next->prev = x;
		x->next = y->next, delete y;
		return 1;
	}
	//3.2.2 Erase all elements/the first element satisfying criteria f
	template <NumericType _T, bool B> template <Predicate<_T> _Fn> constexpr size_t LinkedList<_T, B>::eraseIf(const _Fn& f, bool all) noexcept {
		size_t n = 0;
		for (NodeType* x = _head; x; x = x->next) if (f(x->next->value)) {
			NodeType* y = x->next;
			if constexpr (B) y->next->prev = x;
			x->next = y->next, ++n, delete y;
			if (!all) return 1;
		}
		return n;
	}
	//3.2.3 Erase all elements/the first element equal to t
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::erase(const _T& t, bool all) noexcept {
		size_t n = 0;
		for (NodeType* x = _head; x; x = x->next) if (x->next->value == t) {
			NodeType* y = x->next;
			if constexpr (B) y->next->prev = x;
			++n, x->next = y->next, delete y;
			if (!all) return 1;
		}
		return n;
	}
	//3.2.3 Erase the first element equal to t
	template <NumericType _T, bool B> constexpr LinkedList<_T, B>& LinkedList<_T, B>::operator-=(const _T& t) noexcept {
		for (NodeType* x = _head; x; x = x->next) if (x->next->value == t) {
			NodeType* y = x->next;
			if constexpr (B) y->next->prev = x;
			x->next = y->next, delete y;
			break;
		}
		return *this;
	}
	//3.2.5 Clear the list
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::clear() noexcept {
		NodeType* q = _head;
		while (NodeType* p = q) q = p->next, delete p;
		_tail = _head = 0;
	}
	//3.3 Emplace
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::emplace(size_t p, const _T& t) noexcept {
		NodeType* x = _head;
		while (p--) x = x ? x->next : throw Error(this, "Out of range");
		return x->value;
	}
	//3.4 Find
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::find(const _T& t, bool b) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next, ++n) if (p->value == t) return n;
		return -1;
	}
	template <NumericType _T, bool B> template <Predicate<_T> _Fn> constexpr size_t LinkedList<_T, B>::find(const _Fn& f, bool b) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next, ++n) if (f(p->value)) return n;
		return -1;
	}
	//3.5 Count
	template <NumericType _T, bool B> constexpr size_t LinkedList<_T, B>::count(const _T& t) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next) if (p->value == t) ++n;
		return n;
	}
	template <NumericType _T, bool B> template <Predicate<_T> _Fn> constexpr size_t LinkedList<_T, B>::count(const _Fn& f) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next) if (f(p->value)) ++n;
		return n;
	}
	//3.6 Equality and comparison
	template <class _T, class _S> bool operator==(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a++ != *c++) return false;
		}
		return a == b && c == d;
	}
	template <class _T, class _S> bool operator<(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a < *c) return true;
			else if (*a++ != *c++) return false;
		}
		return c < d;
	}
	template <class _T, class _S> decltype(_T() <=> _S()) operator<=>(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			Comparison<_T, _S> x = *a++ <=> *c++;
			if (x != 0) return x;
		}
		return StrongOrder::equal;
	}
	//4 Linear list adaptors
	template <class _T> class Stack : LinkedList<_T> {
		public:
		constexpr Stack() : LinkedList<_T>() {}
		template <class... _S> constexpr Stack(const _S&... s) : LinkedList<_T>(s...) {}
		template <class _S> constexpr Stack(InitializerList<_S>& l) : LinkedList<_T>(l) {}
		constexpr Stack& push(const _T& t) noexcept {
			return LinkedList<_T>::insert(0, t), * this;
		}
		constexpr Stack& operator+=(const _T& t) noexcept {
			return LinkedList<_T>::insert(0, t), * this;
		}
		constexpr _T pop(bool b = true) {
			auto a = LinkedList<_T>::begin();
			_T t = a ? *a : throw Error(this, "To pop an empty stack");
			if (b) LinkedList<_T>::erase(0);
			return t;
		}
		constexpr _T operator--(int) {
			return pop();
		}
		constexpr operator bool() {
			return LinkedList<_T>::operator bool();
		}
		constexpr bool empty() {
			return LinkedList<_T>::empty();
		}
	};
	template <class _T> class Queue : LinkedList<_T> {
		public:
		constexpr Queue() : LinkedList<_T>() {}
		template <class... _S> constexpr Queue(const _S&... s) : LinkedList<_T>(s...) {}
		template <class _S> constexpr Queue(InitializerList<_S>& l) : LinkedList<_T>(l) {}
		constexpr Queue& push(const _T& t) noexcept {
			return LinkedList<_T>::append(t), * this;
		}
		constexpr Queue& operator+=(const _T& t) noexcept {
			return push(t);
		}
		constexpr _T pop(bool b = true) {
			auto a = LinkedList<_T>::begin();
			_T t = a ? *a : throw Error(this, "To pop an empty stack");
			if (b) LinkedList<_T>::erase(0);
			return t;
		}
		constexpr _T operator--(int) {
			return pop();
		}
		constexpr operator bool() {
			return LinkedList<_T>::operator bool();
		}
		constexpr bool empty() {
			return LinkedList<_T>::empty();
		}
	};
	//6 String
	class String : public LinearList<char> {
		public: 
		constexpr String() noexcept : LinearList<char>() {}
		constexpr String(size_t n, char c) noexcept : LinearList<char>(n, c) {
			for (size_t i = 0; i < _size; ++i) _begin[i] = c;
		}
		constexpr String(const String& r) noexcept {
			_begin = new char[r._size], _size = _cap = r._size;
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
		}
		constexpr String(String&& r) noexcept = default;
		template <LegacyIterator _I> constexpr String(_I f, _I l) : LinearList<char>(f, l) {}
		template <Range _R> constexpr String(const _R& r) noexcept : LinearList(begin(r), end(r)) {}
		constexpr String(const InitializerList<char>& r) noexcept : LinearList(r) {}
		template <CharacterType _T> constexpr String(const char* str) noexcept requires CharacterType<_T> : LinearList(str, str+strlen(str)) {}
		constexpr String& operator=(const String& r) noexcept {
			_size = r._size;
			if (_cap < _size && _begin) delete[] _begin, _begin = 0; //Appealing for reallocation
			if (!_begin) _begin = new char[_cap = _size];
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
			return *this;
		}
	};
	String toString(intmax_t i) noexcept {
		String result(1, 0);
		if (i < 0) i = -i;
		return result;
	}
}
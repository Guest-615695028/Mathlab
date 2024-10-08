#pragma once
#ifndef _MATHLAB_LINEARLIST_
#define _MATHLAB_LINEARLIST_
#include <initializer_list>
#include "iterator.hpp"

namespace Mathlab {
	//1 Linear List by contiguous storage
	template <NumericType _T> class Slice {
	public:
		typedef _T ValueType, * Iterator;
		typedef Mathlab::ReverseIterator<_T*> ReverseIterator;
		constexpr Slice() noexcept : _begin(0), _size(0), _cap(0) {}
		constexpr Slice(size_t n, const _T& t = _T()) noexcept
			: _begin(n ? new _T[n] : 0), _size(n), _cap(n) {
			for (size_t i = 0; i < _size; ++i) _begin[i] = t;
		}
		constexpr Slice(const Slice& r) noexcept
			: _begin(new _T[r._size]), _size(r._size), _cap(r._size) {
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
		}
		template <LegacyIterator _I> constexpr Slice(_I f, _I l) _NOEXCEPT_AS_(++f)
			: _begin(0), _size(distance(f, l)), _cap(0) {
			_begin = new _T[_cap = _size];
			size_t s = 0;
			while (f != l) _begin[s++] = *f++;
		}
		constexpr Slice(const _T* t) noexcept requires(CharacterType<_T>)
			: _begin(0), _size(0), _cap(0) {
			const _T* u = t;
			while (*u++) ++_size;
			_begin = new _T[_cap = _size];
			for (size_t i = 0; i < _cap; ++i) _begin[i] = t[i];
		}
		template <Range _R> constexpr Slice(const _R& r) noexcept {
			Slice(Mathlab::begin(r), Mathlab::end(r));
		}
		template <class _S = _T> constexpr Slice(const InitializerList<_S>& r) noexcept
			: _begin(0), _size(r.size()), _cap(0) {
			_begin = new _T[_cap = _size];
			size_t s = 0;
			for (_S a : r) _begin[s++] = a;
		}
		constexpr ~Slice() noexcept { if (_begin) delete[] _begin; _size = _cap = 0; }
		constexpr Slice& operator=(const Slice& r) noexcept {
			_size = r._size;
			if (_cap < _size && _begin) delete[] _begin, _begin = 0; //Appealing for reallocation
			if (!_begin) _begin = new _T[_cap = _size];
			for (size_t i = 0; i < _size; ++i) _begin[i] = r._begin[i];
			return *this;
		}
		//Iterators and capacity
		constexpr Iterator begin() noexcept { return _begin; }
		constexpr Iterator end() noexcept { return _begin + _size; }
		constexpr ReverseIterator rbegin() noexcept { return _begin + _size - 1; }
		constexpr ReverseIterator rend() noexcept { return _begin - 1; }
		constexpr const _T* begin() const noexcept { return _begin; }
		constexpr const _T* end() const noexcept { return _begin + _size; }
		constexpr ::Mathlab::ReverseIterator<const _T*> rbegin() const noexcept { return _begin + _size - 1; }
		constexpr ::Mathlab::ReverseIterator<const _T*> rend() const noexcept { return _begin - 1; }
		constexpr bool empty() const noexcept { return !_size; }
		constexpr operator bool() const noexcept { return _size; }
		constexpr const _T* data() const noexcept { return _begin; }
		constexpr size_t size() const noexcept { return _size; }
		constexpr size_t capacity() const noexcept { return _cap; }
		//Element access, starting at 0
		constexpr _T& operator[](size_t n) {
			return n >= _size ? throw Error(ERANGE, "Out of range") : _begin[n];
		}
		constexpr const _T& operator[](size_t n) const {
			const _T& t = _T();
			return n >= _size ? t : _begin[n];
		}
		//Insert
		constexpr size_t insert(size_t p, const _T& t) noexcept;
		template <LegacyInputIterator I> constexpr size_t
			insert(size_t p, const I& f, const I& l) noexcept;
		constexpr size_t append(const _T& t) noexcept;
		template <LegacyInputIterator I> constexpr size_t
			append(const I& f, const I& l) noexcept;
		constexpr Slice& operator+=(const _T& t) noexcept {
			this->append(t); return *this;
		}
		constexpr Slice& operator+=(const Slice& t) noexcept {
			this->append(t.begin(), t.end()); return*this;
		}
		//Delete
		constexpr size_t erase(const _T& t, bool all = true) noexcept;
		constexpr Slice& operator-=(const _T& t) noexcept {
			this->erase(t, false); return *this;
		}
		constexpr size_t eraseAt(size_t p) noexcept;
		template <Predicate<_T> _Fn> constexpr size_t
			eraseIf(const _Fn& f, bool all = true) noexcept;
		constexpr size_t clear() noexcept;
		constexpr Slice& shrinkToFit() noexcept;
		//Replace
		constexpr size_t replace(const _T& t, const _T& u, bool all = true) noexcept;
		constexpr size_t replaceAt(size_t p, const _T& t) noexcept;
		template <Predicate<_T> _Fn> constexpr size_t
			replaceIf(const _Fn& f, const _T& t, bool all = true) noexcept;
		//Find
		constexpr size_t find(const _T& t, bool = false) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t find(const _Fn& f, bool = false) const noexcept;
		constexpr size_t count(const _T& t) const noexcept;
		template <Predicate<_T> _Fn> constexpr size_t count(const _Fn& f) const noexcept;
		constexpr void swap(Slice& r) noexcept {
			Mathlab::swap(_begin, r._begin), Mathlab::swap(_size, r._size);
		}
	protected:
		_T* _begin;
		size_t _size, _cap;
	};
	//1.1 Insertion
	//1.1.1 Insert one element t at position p
	template <NumericType _T>
	constexpr size_t Slice<_T>::insert(size_t p, const _T& t) noexcept try {
		if (p > _size) return 0;
		else if (_cap == 0) return _begin = new _T[_size = _cap = 1]{t}, 1;
		else if (_cap <= _size) {//Expand allocation size
			_T* temp = new _T[_cap += _size++]{0};
			for (size_t i = 0; i < p; ++i) temp[i] = _begin[i];
			temp[p] = t;
			for (size_t i = p + 1; i < _size; ++i) temp[i] = _begin[i - 1];
			delete[] _begin, _begin = temp;
		} else {
			for (size_t n = _size++; n > p; --n) _begin[n] = _begin[n - 1];
			_begin[p] = t;
		}
		return 1;
	} catch (...) {
		return 0;
	}
	//1.1.2 Insert a sequence [f,l) at position p
	template <NumericType _T> template <LegacyInputIterator I>
	constexpr size_t Slice<_T>::insert(size_t p, const I& f, const I& l) noexcept {
		if (p > _size) return 0;
		size_t k = distance(f, l), n = _size += k, r = 0;
		if (_cap == 0) _begin = new _T[_size = _cap = k]{};
		else if (_cap < _size) {//Expand allocation size
			_T* temp = new _T[_cap += _size];
			for (size_t i = 0; i < _size - k; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
		while (--n >= p + k) _begin[n] = _begin[n - k];
		for (I i = f; i != l; ++i) _begin[p++] = *i, ++r;
		return r;
	}
	//1.1.3 Append an element to the end
	template <NumericType _T>
	constexpr size_t Slice<_T>::append(const _T& t) noexcept {
		if (_cap == 0) return _begin = new _T[_size = _cap = 1]{t}, 1;
		else if (_cap <= _size) {//Expand allocation size
			_T* temp = new _T[_cap += _size];
			for (size_t i = 0; i < _size; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
		return _begin[_size++] = t, 1;
	}
	//1.1.4 Append a sequence [f,l) to the end
	template <NumericType _T> template <LegacyInputIterator I>
	constexpr size_t Slice<_T>::append(const I& f, const I& l) noexcept {
		size_t n = 0;
		for (I i = f; i != l; ++i) n += this->append(*i);
		return n;
	}
	//1.2 Erasure
	//1.2.1 Erase the element at position p
	template <NumericType _T>
	constexpr size_t Slice<_T>::eraseAt(size_t p) noexcept {
		if (p > _size) return 0;
		for (size_t n = p; n < _size; ++n) _begin[n] = _begin[n + 1];
		_begin = new(_begin) _T[_size -= 1];
		return 1;
	}
	//1.2.2 Erase all elements/the first element satisfying criterion f
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t Slice<_T>::eraseIf(const _Fn& f, bool all) noexcept {
		size_t i = 0; //flag
		for (size_t n = 0; n < _size; ++n) {
			i += (all || !i) && f(_begin[n]);
			Mathlab::swap(_begin[n], _begin[n + i]);
		}
		return _size -= i, i;
	}
	//1.2.3 Erase all elements/the first element equal to t
	template <NumericType _T>
	constexpr size_t Slice<_T>::erase(const _T& t, bool all) noexcept {
		size_t i = 0; //flag
		for (size_t n = 0; n < _size; ++n) {
			i += (all || !i) && _begin[n] == t;
			Mathlab::swap(_begin[n], _begin[n + i]);
		}
		return _size -= i, i;
	}
	//1.2.5 Clear the list
	template <NumericType _T> constexpr size_t Slice<_T>::clear() noexcept {
		if (_begin) delete[] _begin, _begin = 0;
		_size = _cap = 0;
	}
	//1.2.6 Remove unused space
	template <NumericType _T> constexpr Slice<_T>& Slice<_T>::shrinkToFit() noexcept {
		if (_size < _cap) {
			_T* temp = new _T[_cap = _size];
			for (size_t i = 0; i < _size; ++i) temp[i] = _begin[i];
			delete[] _begin, _begin = temp;
		}
		return *this;
	}
	//1.3 Emplace
	template <NumericType _T>
	constexpr size_t Slice<_T>::replace(const _T& t, const _T& u, bool all) noexcept {
		size_t n = 0;
		for (size_t i = 0; i < _size; ++i) if (_begin[i] == t) {
			_begin[i] = u, ++n;
			if (!all) break;
		}
		return n;
	}
	template <NumericType _T>
	constexpr size_t Slice<_T>::replaceAt(size_t p, const _T& t) noexcept {
		return p < size && (_begin[p] = t, 1);
	}
	//1.4 Find
	template <NumericType _T>
	constexpr size_t Slice<_T>::find(const _T& t, bool b) const noexcept {
		if (b) for (size_t i = _size - 1; i >= 0; ++i) if (_begin[i] == t) return i;
		else for (size_t i = 0; i < _size; ++i) if (_begin[i] == t) return i;
		return -1;
	}
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t Slice<_T>::find(const _Fn& f, bool b) const noexcept {
		for (size_t i = 0; i < _size; ++i) if (f(_begin[i])) return i;
		return -1;
	}
	//1.5 Count
	template <NumericType _T>
	constexpr size_t Slice<_T>::count(const _T& t) const noexcept {
		size_t n = 0;
		for (size_t i = 0; i < _size; ++i) n += _begin[i] == t;
		return n;
	}
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t Slice<_T>::count(const _Fn& f) const noexcept {
		size_t n = 0;
		for (size_t i = 0; i < _size; ++i) n += f(_begin[i]);
		return n;
	}
	//1.6 Equality and comparison
	template <class _T, class _S>
	constexpr bool operator==(const Slice<_T>& t, const Slice<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a++ != *c++) return false;
		}
		return a == b && c == d;
	}
	template <class _T, class _S>
	constexpr bool operator<(const Slice<_T>& t, const Slice<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a < *c) return true;
			else if (*a++ > *c++) return false;
		}
		return c < d;
	}
#if !_OLD_CXX
	template <class _T, class _S>
	constexpr Comparison<_T, _S> operator<=>(const Slice<_T>& t, const Slice<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			Comparison<_T, _S> x = *a++ <=> *c++;
			if (x != 0) return x;
		}
		return {0};
	}
#endif
	//2 Linear List by linkage
	template <NumericType _T> class LinkedList {
	public:
		typedef _T ValueType;
		struct NodeType {
			_T value;
			NodeType* next;
		};
		class Iterator;
		constexpr LinkedList() noexcept : _head(0) {}
		constexpr LinkedList(size_t n, const _T& t = _T()) noexcept
			: _head(n ? new NodeType{t, {0}} : 0) {
			NodeType* p = _head;
			while (--n) {
				p = p->next = new NodeType{t, 0};
			}
		}
		template <LegacyIterator _I> constexpr LinkedList(_I f, _I l) noexcept {
			if (f == l) return;
			NodeType* p = _head = new NodeType{*f++, 0};
			while (f != l) {
				p = p->next = new NodeType{*f++, 0};
			}
		}
		template <Range _R> constexpr LinkedList(const _R& r) noexcept {
			auto f = Mathlab::begin(r), l = Mathlab::end(r);
			if (f == l) return;
			NodeType* p = _head = new NodeType{*f++, 0};
			while (f != l) {
				p = p->next = new NodeType{*f++, 0};
			}
		}
		template <class _S> constexpr LinkedList(const InitializerList<_S>& r) noexcept {
			auto f = Mathlab::begin(r), l = Mathlab::end(r);
			if (f == l) return;
			NodeType* p = _head = new NodeType{*f++, 0};
			while (f != l) {
				p = p->next = new NodeType{*f++, 0};
			}
		}
		constexpr LinkedList(const LinkedList& r) noexcept {
			auto f = Mathlab::begin(r), l = Mathlab::end(r);
			if (f == l) return;
			NodeType* p = _head = new NodeType{*f++, 0};
			while (f != l) {
				p = p->next = new NodeType{*f++, 0};
			}
		}
		constexpr ~LinkedList() noexcept { clear(); }
		template <Range _R> constexpr LinkedList& operator=(const _R& r) noexcept {
			return LinkedList(r).swap(*this), * this;
		}
		template <class _S> constexpr LinkedList& operator=(const InitializerList<_S>& r) noexcept {
			return LinkedList(r).swap(*this), * this;
		}
		template <class _S> constexpr LinkedList& operator=(const LinkedList<_S>& r) noexcept {
			return LinkedList(r).swap(*this), * this;
		}
		//Member access
		constexpr Iterator begin() const noexcept { return Iterator(_head, this); }
		constexpr Iterator end() const noexcept { return Iterator(0, this); }
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
		//Insert
		constexpr size_t insert(size_t p, const _T& t) noexcept;
		template <LegacyInputIterator I>
		constexpr size_t insert(size_t p, const I& f, const I& l) noexcept;
		constexpr size_t append(const _T& t) noexcept;
		template <LegacyIterator I>
		constexpr size_t append(const I& f, const I& l) noexcept;
		constexpr LinkedList& operator+=(const _T& t) noexcept;
		//Erase
		constexpr LinkedList& operator-=(const _T& t) noexcept;
		constexpr size_t erase(const _T& t, bool all = true) noexcept;
		constexpr size_t eraseAt(size_t p) noexcept;
		template <Predicate<_T> _Fn>
		constexpr size_t eraseIf(const _Fn& f, bool all = true) noexcept;
		constexpr size_t clear() noexcept;
		//Emplace
		constexpr size_t replace(size_t p, const _T& t) noexcept;
		//Find
		constexpr size_t find(const _T& t, bool = false) const noexcept;
		template <Predicate<_T> _Fn>
		constexpr size_t find(const _Fn& f, bool = false) const noexcept;
		constexpr size_t count(const _T& t) const noexcept;
		template <Predicate<_T> _Fn>
		constexpr size_t count(const _Fn& f) const noexcept;
		//Swap
		constexpr void swap(LinkedList& r) noexcept {
			Mathlab::swap(_head, r._head);
		}
	protected: NodeType* _head;
	};
	//2. Iterator for LinkedList<_T>
	template <NumericType _T> class LinkedList<_T>::Iterator {
		NodeType* _base;
		const LinkedList* _obj;
	public:
		constexpr Iterator(NodeType* n = 0, const LinkedList* l = 0) noexcept : _base(n), _obj(l) {}
		constexpr Iterator& operator++() noexcept {
			return _base = _base ? _base->next : _obj->_head, *this;
		}
		constexpr _T& operator*() const noexcept { return _base->value; }
		constexpr _T* operator->() const noexcept { return &_base->value; }
		constexpr bool operator==(const Iterator& r) const noexcept {
			return _obj == r._obj && _base == r._base;
		}
		constexpr bool operator==(nullptr_t) const noexcept { return !_obj || !_base; }
		constexpr operator bool() noexcept { return _obj && _base; }
	};
	//2.1 Insert
	//2.1.1 Insert one element t at position p
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::insert(size_t p, const _T& t) noexcept {
		NodeType* x = _head = p ? _head : new NodeType{t, _head};
		if (p) {
			while (--p) {
				if (x) x = x->next; else return 0;
			}
			x->next = new NodeType{t, x->next};
		} else _head = new NodeType{t, _head};
		return 1;
	}
	//2.1.2 Insert a sequence [f,l) at position p
	template <NumericType _T> template <LegacyInputIterator I>
	constexpr size_t LinkedList<_T>::insert(size_t p, const I& f, const I& l) noexcept {
		size_t n = 0;
		NodeType* x = _head;
		if (p) {
			while (--p) {
				if (x) x = x->next; else return 0;
			}
			while (f != l) {
				x = x->next = new NodeType{*f++, x->next};
			}
		} else {
			while (f != l) {
				_head = new NodeType{*f++, _head};
			}
		}
		return n;
	}
	//2.1.3 Append one element t to the end
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::append(const _T& t) noexcept {
		NodeType* x = _head;
		while (x->next) x = x->next;
		x->next = new NodeType(t, 0);
		return 1;
	}
	template <NumericType _T>
	constexpr LinkedList<_T>& LinkedList<_T>::operator+=(const _T& t) noexcept {
		this->append(t);
		return *this;
	}
	//2.1.4 Append a sequence [f,l) to the end
	template <NumericType _T> template <LegacyIterator I>
	constexpr size_t LinkedList<_T>::append(const I& f, const I& l) noexcept {
		size_t n = 0;
		NodeType* x = _head;
		while (x->next) x = x->next;
		for (I i = f; i != l; ++i) ++n, x = x->next = new NodeType(*i, 0);
		return n;
	}
	//2.2 Erase
	//2.2.1 Erase all elements/the first element
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::eraseAt(size_t p) noexcept {
		NodeType* x = _head;
		if (!p) {
			if (x) {
				_head = x->next, delete x;
				return 1;
			} else return 0;
		}
		while (--p) if (!(x = x->next)) return 0;
		NodeType* y = x->next;
		x->next = y->next, delete y;
		return 1;
	}
	//2.2.2 Erase all elements/the first element satisfying criteria f
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t LinkedList<_T>::eraseIf(const _Fn& f, bool all) noexcept {
		size_t n = 0;
		for (NodeType* x = _head; x; x = x->next) if (f(x->next->value)) {
			NodeType* y = x->next;
			x->next = y->next, ++n, delete y;
			if (!all) return 1;
		}
		return n;
	}
	//2.2.3 Erase all elements/the first element equal to t
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::erase(const _T& t, bool all) noexcept {
		size_t n = 0;
		NodeType* x = _head;
		if (x->value == t) {
			_head = x->next, ++n, delete x;
			if (!all) return 1;
		}
		for (; x; x = x->next) {
			if (!x->next) break;
			if (x->next->value == t) {
				NodeType* y = x->next;
				++n, x->next = y->next, delete y;
				if (!all) return 1;
			}
		}
		return n;
	}
	//2.2.3 Erase the first element equal to t
	template <NumericType _T>
	constexpr LinkedList<_T>& LinkedList<_T>::operator-=(const _T& t) noexcept {
		this->erase(t, false);
		return *this;
	}
	//2.2.5 Clear the list
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::clear() noexcept {
		size_t n = 0;
		NodeType* q = _head;
		while (NodeType* p = q) q = p->next, delete p, ++n;
		_head = 0;
		return n;
	}
	//2.3 Emplace
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::replace(size_t p, const _T& t) noexcept {
		NodeType* x = _head;
		while (p--) x = x ? x->next : throw Error(this, "Out of range");
		return x->value;
	}
	//2.4 Find
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::find(const _T& t, bool b) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next, ++n) if (p->value == t) return n;
		return -1;
	}
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t LinkedList<_T>::find(const _Fn& f, bool b) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next, ++n) if (f(p->value)) return n;
		return -1;
	}
	//2.5 Count
	template <NumericType _T>
	constexpr size_t LinkedList<_T>::count(const _T& t) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next) if (p->value == t) ++n;
		return n;
	}
	template <NumericType _T> template <Predicate<_T> _Fn>
	constexpr size_t LinkedList<_T>::count(const _Fn& f) const noexcept {
		size_t n = 0;
		for (NodeType* p = _head; p; p = p->next) if (f(p->value)) ++n;
		return n;
	}
	//2.6 Equality and comparison
	template <class _T, class _S>
	constexpr bool operator==(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a++ != *c++) return false;
		}
		return a == b && c == d;
	}
	template <class _T, class _S>
	constexpr bool operator<(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a < b && c < d) {
			if (*a < *c) return true;
			else if (*a++ != *c++) return false;
		}
		return a != b;
	}
#if !_OLD_CXX
	template <class _T, class _S>
	constexpr Comparison<_T, _S> operator<=>(const LinkedList<_T>& t, const LinkedList<_S>& s) {
		_T* a = t.begin(), b = t.end();
		_S* c = s.begin(), d = s.end();
		while (a != b && c != d) {
			Comparison<_T, _S> x = *a++ <=> *c++;
			if (x != 0) return x;
		}
		if (a != b) return StrongOrder::less;
		else if (c != d) return StrongOrder::greater;
		else return StrongOrder::equivalent;
	}
#endif
	//3 Linear list adaptors
	template <NumericType _T> class Stack : LinkedList<_T> {
	public:
		typedef LinkedList<_T> Container;
		constexpr Stack() : Container() {}
		template <class... _S> constexpr Stack(const _S&... s) : Container(s...) {}
		template <class _S> constexpr Stack(InitializerList<_S>& l) : Container(l) {}
		using Container::operator=;
		constexpr Stack& push(const _T& t) noexcept {
			return Container::insert(0, t), * this;
		}
		constexpr Stack& operator+=(const _T& t) noexcept {
			return Container::insert(0, t), * this;
		}
		constexpr _T pop(bool b = true) {
			auto a = Container::begin();
			_T t = a ? *a : throw Error(EPERM, "To pop an empty stack");
			if (b) Container::erase(0);
			return t;
		}
		constexpr _T operator--(int) {
			return pop();
		}
		using Container::size;
		using Container::empty;
		operator bool() {
			return Container::begin();
		}
	};
	template <NumericType _T> class Queue : LinkedList<_T> {
	public:
		typedef LinkedList<_T> Container;
		constexpr Queue() : Container() {}
		template <class... _S> constexpr Queue(const _S&... s) : Container(s...) {}
		template <class _S> constexpr Queue(InitializerList<_S>& l) : Container(l) {}
		using Container::operator=;
		constexpr Queue& push(const _T& t) noexcept {
			return Container::append(t), * this;
		}
		constexpr Queue& operator+=(const _T& t) noexcept {
			return push(t);
		}
		constexpr _T pop(bool b = true) {
			auto a = Container::begin();
			_T t = a ? *a : throw Error(this, "To pop an empty stack");
			if (b) Container::eraseAt(0);
			return t;
		}
		constexpr _T operator--(int) noexcept {
			return pop();
		}
		using Container::size;
		using Container::empty;
		using Container::operator bool;
	};
}
#endif
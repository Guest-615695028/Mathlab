#pragma once
#include "linearlist.hpp"
namespace Mathlab {
	template <class _T> class BinaryTree {
		struct _N {
			_T value;
			NodeType pred, * left, * right;
		} *_head;
		public:
		typedef _T ValueType;
		typedef _N* NodeType;
		class LayerwiseIterator;
		class PreOrderIterator;
		class MidOrderIterator;
		class PostOrderIterator;
		//Ctor and Dtor
		constexpr BinaryTree() = default;
		template <class _S> constexpr BinaryTree(const InitializerList<_S>& r) noexcept : _head(0) {
			if (!r.size()) return;
			_T* a = r.begin(), b = r.end();
			if (!a || a >= b) return;
			Queue<NodeType> q{ _head = new _N{*a++} };
			while (a < b) {
				NodeType n = q.pop();
				q += n->left = new _N{ *a++, n };
				if (a >= b) return;
				q += n->right = new _N{ *a++, n };
			}
		}
		template <LegacyIterator _I> constexpr BinaryTree(_I f, _I l) noexcept : _head(0) {
			if (!f || f == l) return;
			Queue<NodeType> q{ _head = new _N{*f++} };
			while (f != l) {
				NodeType n = q.pop();
				q += n->left = new _N{ f++, n };
				if (f == l) break;
				q += n->right = new _N{ f++, n };
			}
		}
		template <Range _R> constexpr BinaryTree(const _R& r) noexcept : BinaryTree(begin(r), end(r)) {}
		template <class _S> constexpr BinaryTree(const BinaryTree<_S>& r) noexcept : _head(0) {
			_copy(_head, r->_head);
		}
		constexpr ~BinaryTree() noexcept { _copy(_head, 0); }
		template <class _S> constexpr BinaryTree& operator=(const BinaryTree<_S>& r) noexcept {
			_copy(_head, r->_head);
			return *this;
		}
		//Element Access
		constexpr _T& operator[](size_t n) noexcept { return _node(n + 1)->value; }
		constexpr LayerwiseIterator begin() noexcept { return { 1, this }; }
		constexpr LayerwiseIterator end() noexcept { return { 0, this }; }
		constexpr operator bool() noexcept { return _head; }
		constexpr bool empty() noexcept { return !_head; }
		constexpr NodeType leftmost(bool b = false) noexcept {
			NodeType n = _head;
			while (1) {
				if (n->left) n = n->left;
				else if (b && n->right) n = n->right;
				else return n;
			}
		}
		constexpr NodeType rightmost(bool b = false) noexcept {
			NodeType n = _head;
			while (1) {
				if (n->right) n = n->right;
				else if (b && n->left) n = n->left;
				else return n;
			}
		}
		//Modification
		constexpr void insert(size_t pos, const _T& val, bool dir = false) {
			if (NodeType p = _node(n + 1)) {
				auto l = &NodeType::left, r = &NodeType::right;
				p->pred->*(n % 2 ? r : l) = new _N{ val, p->pred, p, 0 };
			} else if (NodeType p = _node(n / 2)) {
				(n % 2 ? p->left : p->right) = new _N{ val, p };
			} else throw Error(this, "Parent node does not exist");
		}
		constexpr bool erase(size_t pos) noexcept {
			NodeType p = _node(pos);
			return p ? _copy(p, 0) : 0;
		}
		private:
		template <class _S = _T> static constexpr size_t _copy(NodeType& l, const typename BinaryTree<_S>::NodeType r) noexcept {
			if (r) {
				if (l) l->value = r->value;
				else l = new _N{ r->value };
				size_t res = _copy(l->left, r->left);
				res += _copy(l->right, r->right);
				l->left->pred = l->right->pred = l;
				return ++res;
			} else if (l) {
				size_t res = _copy(l->left, nullptr);
				res += _copy(l->right, nullptr);
				delete l, l = 0;
				return ++res;
			} else return 0;
		}
		static constexpr size_t _copy(NodeType& l, nullptr_t) noexcept {
			if (!l) return 0;
			size_t res = _copy(l->left, nullptr);
			res += _copy(l->right, nullptr);
			delete l, l = 0;
			return ++res;
		}
		constexpr NodeType _node(size_t n) noexcept {
			switch (n) {
				case 0: return 0;
				case 1: return _head;
				default: if (NodeType p = _node(n / 2)) return n % 2 ? p->left : p->right;
					   else return 0;
			}
		}
	};
	template <class _T> class BinaryTree<_T>::LayerwiseIterator {
		size_t _n;
		BinaryTree* _obj;
		public:
		constexpr LayerwiseIterator(size_t n = 0, BinaryTree* obj = 0) : _n(n), _obj(obj) {};
		constexpr _T& operator*() {
			return _obj->_node(_n)->value;
		}
		constexpr _T* operator->() {
			return &_obj->_node(_n)->value;
		}
		constexpr LayerwiseIterator& operator++() {
			size_t s = 2 * ++_n;
			do if (_obj->_node(_n)) return *this; while (++_n < s);
			return _n = 0, *this;
		}
		constexpr LayerwiseIterator& operator--() {
			do if (_obj->_node(--_n)) return *this; while (_n);
			return *this;
		}
		constexpr bool operator==(const LayerwiseIterator& rhs) { return _n == rhs._n && _obj == rhs._obj; }
		constexpr PartialOrder operator<=>(const LayerwiseIterator& rhs) { return _obj == rhs._obj ? _n <=> rhs._n : PartialOrder::unordered; }
	};
	template <class _T> class BinaryTree<_T>::PreOrderIterator {
		NodeType _n;
		BinaryTree* _obj;
		public:
		constexpr PreOrderIterator(NodeType n = 0, BinaryTree* obj = 0) : _n(n), _obj(obj) {};
		constexpr _T& operator*() {
			return _n->value;
		}
		constexpr _T* operator->() {
			return &_n->value;
		}
		constexpr PreOrderIterator& operator++() {
			if (!_n) return _n = _obj->_head, *this;
			if (_n->left) _n = _n->left;
			else if (_n->right) _n = _n->right;
			else if (_n->pred) {
				while (_n == _n->pred->right || !_n->pred->right) if (!(n = n->pred)) return *this;
				n = _n->pred->right;
			} else _n = 0;
			return *this;
		}
		constexpr PreOrderIterator& operator--() {
			if (!_n) return _n = _obj->rightmost(1), *this;
			_N p = _n->pred;
			if (!p || !p->left || _n == p->left) return _n = p, *this;
			_n = p->left;
			while (1) {
				if (_n->right) _n = _n->right;
				else if (_n->left) _n = _n->left;
				else return *this;
			}
			return *this;
		}
		constexpr bool operator==(const PreOrderIterator& rhs) { return _n == rhs._n && _obj == rhs._obj; }
	};
	template <class _T> class BinaryTree<_T>::MidOrderIterator {
		NodeType _n;
		BinaryTree* _obj;
		public:
		constexpr MidOrderIterator(NodeType n = 0, BinaryTree* obj = 0) : _n(n), _obj(obj) {};
		constexpr _T& operator*() {
			return _n->value;
		}
		constexpr _T* operator->() {
			return &_n->value;
		}
		constexpr MidOrderIterator& operator++() {
			if (!_n) return _n = _obj->leftmost(0), *this;
			else if (NodeType p = _n->right) {
				do _n = p; while (p = _n->left);
			} else if (NodeType p = _n->pred) {
				while (n == p->right) _n = p;
				_n = p; //_n is either left child of parent or head
			}
			return *this;
		}
		constexpr MidOrderIterator& operator--() {
			if (!_n) return _n = _obj->rightmost(0), *this;
			else if (NodeType p = _n->left) {
				do _n = p; while (p = _n->right);
			} else if (NodeType p = _n->pred) {
				while (n == p->left) _n = p;
				_n = p; //_n is either right child of parent or head
			}
			return *this;
		}
		constexpr bool operator==(const MidOrderIterator& rhs) { return _n == rhs._n && _obj == rhs._obj; }
	};
	template <class _T> class BinaryTree<_T>::PostOrderIterator {
		NodeType _n;
		BinaryTree* _obj;
		public:
		constexpr PostOrderIterator(NodeType n = 0, BinaryTree* obj = 0) : _n(n), _obj(obj) {};
		constexpr _T& operator*() {
			return _n->value;
		}
		constexpr _T* operator->() {
			return &_n->value;
		}
		constexpr PostOrderIterator& operator++() {
			if (!_n) return _n = _obj->leftmost(1), *this;
			_N p = _n->pred;
			if (!p || !p->right || _n == p->right) return _n = p, *this;
			_n = p->right;
			while (1) {
				if (_n->left) _n = _n->left;
				else if (_n->right) _n = _n->right;
				else return *this;
			}
			return *this;
		}
		constexpr PostOrderIterator& operator--() {
			if (!_n) return _n = _obj->_head, *this;
			if (_n->right) _n = _n->right;
			else if (_n->left) _n = _n->left;
			else if (_n->pred) {
				while (_n == _n->pred->left || !_n->pred->left) if (!(n = n->pred)) return *this;
				n = _n->pred->left;
			} else _n = 0;
			return *this;
		}
		constexpr bool operator==(const PostOrderIterator& rhs) { return _n == rhs._n && _obj == rhs._obj; }
	};
}
#pragma once
#ifndef FUNCTIONAL_HPP
#define FUNCTIONAL_HPP 1

#include "basics.hpp"
namespace Mathlab {
	template <typename _R, typename... _Args> class Function {
		struct vtable_t {
			_R(*invoke)(uintptr_t&, _Args...);
			void (*copy)(uintptr_t&, const uintptr_t&) noexcept;
			void (*move)(uintptr_t&, uintptr_t&&) noexcept;
			void (*swap)(uintptr_t&, uintptr_t&) noexcept;
			void (*destroy)(uintptr_t&) noexcept;
			::std::type_info const& (*type)() noexcept;
			bool reqAlloc;
		};
		template<typename _T, bool B = true>
		struct vtable_funcs {
			static constexpr _R invoke(uintptr_t& storage, _Args... args) _noexcept_as(_declval(_T)(args...)) {
				return (B ? *reinterpret_cast<_T*>(storage) : reinterpret_cast<_T&>(storage))(args...);
			}
			static constexpr void copy(uintptr_t& dest, const uintptr_t& src) noexcept {
				if constexpr (B) dest = reinterpret_cast<uintptr_t>(new _T(*reinterpret_cast<_T*>(src)));
				else dest = src;
			}
			static constexpr void move(uintptr_t& dest, uintptr_t&& src) noexcept {
				dest = src, src = 0;
			}
			static constexpr void swap(uintptr_t& lhs, uintptr_t& rhs) noexcept {
				uintptr_t u = lhs;
				lhs = rhs, rhs = u;
			}
			static constexpr void destroy(uintptr_t& storage) noexcept {
				if constexpr (B) delete reinterpret_cast<_T*>(storage);
				else reinterpret_cast<_T*>(&storage)->~_T();
			}
			static constexpr ::std::type_info const& type() noexcept {
				return typeid(_T);
			}
			static constexpr bool reqAlloc = B;
		};
		template<typename _T> static constexpr vtable_t* vtable() {
			vtable_funcs<_T, (alignof(_T) > alignof(uintptr_t) || sizeof(_T) > sizeof(uintptr_t) || !std::is_trivially_copyable_v<_T>) > v{};
			static vtable_t vtable{v.invoke, v.copy, v.move, v.swap, v.destroy, v.type, v.reqAlloc};
			return &vtable;
		}
		mutable uintptr_t _storage = 0;
		vtable_t* _vtable;

		public:
		typedef _R ResultType, (*PointerType)(_Args...);
		constexpr Function(std::nullptr_t n = nullptr) noexcept : _storage(0), _vtable(n) {}
		template <class _S, class... _T> constexpr Function(const Function<_S, _T...>& other) noexcept : _storage(0), _vtable(other._vtable) {
			if (_vtable) _vtable->copy(_storage, other._storage);
		}
		template <class _S, class... _T> constexpr Function(Function<_S, _T...>&& other) noexcept : _storage(other._storage), _vtable(other._vtable) {}
		constexpr Function(PointerType f) noexcept : _vtable(vtable<PointerType>()), _storage(reinterpret_cast<uintptr_t>(f)) {}
		//Other than PointerType
		template<FunctionObject _Fn> constexpr Function(_Fn&& f) noexcept : _storage(0), _vtable(vtable<_Fn>()) {
			if (_vtable->reqAlloc) _storage = new _Fn(f);
			else new(&_storage) _Fn(f);
		}
		constexpr Function& operator=(Function const& other) noexcept {
			if (_vtable && _storage != other._storage) _vtable->destroy(_storage);
			if (_vtable = other._vtable) _vtable->copy(_storage, other._storage);
			return *this;
		}
		constexpr Function& operator=(Function&& other) noexcept {
			if (_vtable && _storage != other._storage) _vtable->destroy(_storage);
			if (_vtable = other._vtable) _vtable->move(_storage, other._storage), other._vtable = nullptr;
		}
		template<FunctionObject _Fn> constexpr Function& operator=(_Fn&& f) {
			Function(f).swap(*this);
			return *this;
		}
		constexpr ~Function() noexcept {
			if (_vtable) _vtable->destroy(_storage);
		}
		constexpr void swap(Function& other) noexcept {
			if (_vtable == other._vtable) {
				if (_vtable) _vtable->swap(_storage, other._storage);
				return;
			}
			Function tmp(other);
			if (other._vtable = _vtable) other._vtable->move(other._storage, _storage);
			if (_vtable = tmp._vtable) _vtable->move(_storage, tmp._storage);
		}
		constexpr _R operator()(_Args... args) const {
			return _vtable ? _vtable->invoke(_storage, args...) : throw Error(this);
		}
		constexpr ::std::type_info const& target_type() const noexcept {
			return _vtable ? _vtable->type() : typeid(void);
		}
		template<typename _T = void> constexpr _T* target() noexcept {
			return typeid(_T) == _vtable->type() || typeid(_T) == typeid(void) ? reinterpret_cast<_T*>(_storage) : nullptr;
		}
		template<typename _T = void> constexpr const _T* target() const noexcept {
			return typeid(_T) == _vtable->type() || typeid(_T) == typeid(void) ? reinterpret_cast<const _T*>(_storage) : nullptr;
		}
		constexpr operator void* () const noexcept {
			return _vtable;
		}
		constexpr size_t hash() const noexcept {
			return _storage;
		}
		constexpr operator bool() const noexcept {
			return _vtable;
		}
		constexpr bool operator==(nullptr_t) const noexcept {
			return _vtable == nullptr;
		}
		constexpr bool operator==(const Function& other) const noexcept {
			return _vtable == other._vtable && (_storage == other._storage || _storage == other._storage);
		}
	};
	template <class... _Args> using Predicate = Function<bool, _Args...>;
	template <class... _Args> using Operation = Function<void, _Args...>;
	template <class _R, class... _Args> constexpr Function<_R, _Args...> makeFunction(_R(*f)(_Args...)) {
		return f;
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Plus<_T, _S>, _Args...> operator+(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) + g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Plus<_T, _S>, _Args...> operator+(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) + g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Plus<_T, _S>, _Args...> operator+(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) + g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Plus<_T, _S>, _Args...> operator+(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t + g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Plus<_T, _S>, _Args...> operator+(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) + s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Minus<_T, _S>, _Args...> operator-(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) - g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Minus<_T, _S>, _Args...> operator-(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) - g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Minus<_T, _S>, _Args...> operator-(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) - g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Minus<_T, _S>, _Args...> operator-(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t - g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Minus<_T, _S>, _Args...> operator-(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) - s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Multiplies<_T, _S>, _Args...> operator*(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) * g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Multiplies<_T, _S>, _Args...> operator*(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) * g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Multiplies<_T, _S>, _Args...> operator*(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) * g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Multiplies<_T, _S>, _Args...> operator*(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t * g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Multiplies<_T, _S>, _Args...> operator*(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) * s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Divides<_T, _S>, _Args...> operator/(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) / g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Divides<_T, _S>, _Args...> operator/(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) / g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Divides<_T, _S>, _Args...> operator/(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) / g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Divides<_T, _S>, _Args...> operator/(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t / g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Divides<_T, _S>, _Args...> operator/(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) / s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Modulus<_T, _S>, _Args...> operator%(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) % g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Modulus<_T, _S>, _Args...> operator%(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) % g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Modulus<_T, _S>, _Args...> operator%(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) % g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Modulus<_T, _S>, _Args...> operator%(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t % g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<Modulus<_T, _S>, _Args...> operator%(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) % s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitAnd<_T, _S>, _Args...> operator&(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) & g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitAnd<_T, _S>, _Args...> operator&(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) & g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitAnd<_T, _S>, _Args...> operator&(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) & g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitAnd<_T, _S>, _Args...> operator&(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t & g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitAnd<_T, _S>, _Args...> operator&(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) & s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitOr<_T, _S>, _Args...> operator|(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) | g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitOr<_T, _S>, _Args...> operator|(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) | g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitOr<_T, _S>, _Args...> operator|(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) | g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitOr<_T, _S>, _Args...> operator|(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t | g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitOr<_T, _S>, _Args...> operator|(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) | s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitXor<_T, _S>, _Args...> operator^(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) ^ g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitXor<_T, _S>, _Args...> operator^(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) ^ g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitXor<_T, _S>, _Args...> operator^(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) ^ g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitXor<_T, _S>, _Args...> operator^(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t ^ g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<BitXor<_T, _S>, _Args...> operator^(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) ^ s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftLeft<_T, _S>, _Args...> operator<<(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) << g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftLeft<_T, _S>, _Args...> operator<<(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) << g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftLeft<_T, _S>, _Args...> operator<<(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) << g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftLeft<_T, _S>, _Args...> operator<<(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t << g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftLeft<_T, _S>, _Args...> operator<<(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) << s; };
	}

	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftRight<_T, _S>, _Args...> operator>>(const Function<_T, _Args...>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) >> g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftRight<_T, _S>, _Args...> operator>>(_T(*f)(_Args...), const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(args...) >> g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftRight<_T, _S>, _Args...> operator>>(const Function<_T, _Args...>& f, _S(*g)(_Args...)) {
		return [=](_Args... args) {return f(args...) >> g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftRight<_T, _S>, _Args...> operator>>(const _T& t, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return t >> g(args...); };
	}
	template <Arithmetic _T, Arithmetic _S, class... _Args> constexpr Function<ShiftRight<_T, _S>, _Args...> operator>>(const Function<_T, _Args...>& f, const _S& s) {
		return [=](_Args... args) {return f(args...) >> s; };
	}
	template <class _T, class _S, class... _Args> constexpr Function<_T, _Args...> operator->*(const Function<_T, _S>& f, const Function<_S, _Args...>& g) {
		return [=](_Args... args) {return f(g(args...)); };
	}
}
#endif
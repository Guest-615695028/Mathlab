#pragma once
#include <stddef.h>
#include <type_traits>
#include <utility>
#ifndef _CONCEPTS_HPP_
#define _CONCEPTS_HPP_
namespace std {
	namespace concepts {
		#if _HAS_CXX20 || __cplusplus >= 202000L
		#define _OLD_CXX 0
		template <class _T>              concept Destructible = is_destructible_v<_T> || requires(_T * t) { t->~_T(); };
		template <class _T, class... _U> concept Constructible = is_constructible_v<_T, _U...> || requires(_U... u) { _T(u...); };
		template <class _T>              concept DefaultConstructible = is_default_constructible_v<_T> || requires{ _T(); };
		template <class _T>              concept CopyConstructible = is_copy_constructible_v<_T> || Constructible<_T, const _T&>;
		template <class _T>              concept MoveConstructible = is_move_constructible_v<_T> || Constructible<_T, _T&&>;
		template <class _T, class _U>    concept Assignable = is_assignable_v<_T, _U> || requires(_T t, _U u) { t = u; };
		template <class _T>              concept CopyAssignable = is_copy_assignable_v<_T> || Assignable<_T, const _T&>;
		template <class _T>              concept MoveAssignable = is_move_assignable_v<_T> || Assignable<_T, _T&&>;
		template <class _T, class _U>    concept SwappableWith = is_swappable_with_v<_T, _U> || requires(_T t, _U u) { swap(t, u); };
		template <class _T>              concept Swappable = is_swappable_v<_T> || SwappableWith<_T, _T>;

		template <class _T>              concept NothrowDestructible = is_nothrow_destructible_v<_T> || requires(_T * t) { { t->~_T() } noexcept; };
		template <class _T, class... _U> concept NothrowConstructible = is_nothrow_constructible_v<_T, _U...> || requires(_U... u) { { _T(u...) } noexcept; };
		template <class _T>              concept NothrowDefaultConstructible = is_nothrow_default_constructible_v<_T> || requires{ { _T() } noexcept; };
		template <class _T>              concept NothrowCopyConstructible = is_nothrow_copy_constructible_v<_T> || NothrowConstructible<_T, const _T&>;
		template <class _T>              concept NothrowMoveConstructible = is_nothrow_move_constructible_v<_T> || NothrowConstructible<_T, _T&&>;
		template <class _T, class _U>    concept NothrowAssignable = is_nothrow_assignable_v<_T, _U> || requires(_T t, _U u) { { t = u } noexcept; };
		template <class _T>              concept NothrowCopyAssignable = is_nothrow_copy_assignable_v<_T> || NothrowAssignable<_T, const _T&>;
		template <class _T>              concept NothrowMoveAssignable = is_nothrow_move_assignable_v<_T> || NothrowAssignable<_T, _T&&>;
		template <class _T, class _U>    concept NothrowSwappableWith = is_nothrow_swappable_with_v<_T, _U> || requires(_T t, _U u) { { swap(t, u) } noexcept; };
		template <class _T>              concept NothrowSwappable = is_nothrow_swappable_v<_T> || NothrowSwappableWith<_T, _T>;

		template <class _T>              concept TriviallyDestructible = is_trivially_destructible_v<_T>;
		template <class _T, class... _U> concept TriviallyConstructible = is_trivially_constructible_v<_T, _U...>;
		template <class _T>              concept TriviallyDefaultConstructible = is_trivially_default_constructible_v<_T>;
		template <class _T>              concept TriviallyCopyConstructible = is_trivially_copy_constructible_v<_T>;
		template <class _T>              concept TriviallyMoveConstructible = is_trivially_move_constructible_v<_T>;
		template <class _T, class _U>    concept TriviallyAssignable = is_trivially_assignable_v<_T, _U>;
		template <class _T>              concept TriviallyCopyAssignable = is_trivially_copy_assignable_v<_T>;
		template <class _T>              concept TriviallyMoveAssignable = is_trivially_move_assignable_v<_T>;
		template <class _T>              concept TriviallyCopyable = is_trivially_copyable_v<_T>;
		template <class _T>              concept TrivialType = is_trivial_v<_T>;

		template <class _T> concept StandardLayoutType = is_standard_layout_v<_T>;
		template <class _T> concept PODType = is_trivial_v<_T> && is_standard_layout_v<_T>;
		template <class _T> concept Scalar = is_scalar_v<_T>;
		template <class _T> concept Object = is_object_v<_T>;
		template <class _T> concept Empty = is_empty_v<_T>;
		template <class _T> concept Polymorphic = is_polymorphic_v<_T>;
		template <class _T> concept Abstract = is_abstract_v<_T>;
		template <class _T> concept Final = is_final_v<_T>;
		template <class _T> concept Aggregate = is_aggregate_v<_T>;

		template <class _T, class _U>    concept SameAs = is_same_v<_T, _U>;
		template <class _T, class _U>    concept LayoutCompatibleWith = is_same_v<remove_cvref<_T>, remove_cvref<_U>>;
		template <class _T, class _U>    concept ConvertibleTo = (is_convertible_v<_T, _U>) || is_void_v<_U>;
		template <class _T, class _U>    concept BaseOf = is_base_of_v<_T, _U>;
		template <class _T, class _U>    concept InterconvertibleWith = ConvertibleTo<_T, _U>&& ConvertibleTo<_U, _T>;
		template <class _T, class... _U> concept EitherOf = (is_same_v<_T, _U> || ...);
		//Library-wide concepts
		template <class _T, class _U = _T> concept ValueSwappable = requires (_T t, _U u) {
			swap(*t, *u);
		};
		template <class _T, class _U = _T> concept EqualityComparable = requires (_T t, _U u) {
			{ t == u } -> ConvertibleTo<bool>; { t != u } -> ConvertibleTo<bool>;
			{ u == t } -> ConvertibleTo<bool>; { u != t } -> ConvertibleTo<bool>;
		};
		template <class _T, class _U = _T> concept LessThanComparable = EqualityComparable<_T, _U>&& requires (_T t, _U u) {
			{ t < u } -> ConvertibleTo<bool>; { t > u } -> ConvertibleTo<bool>; { t <= u } -> ConvertibleTo<bool>; { t >= u } -> ConvertibleTo<bool>;
			{ u < t } -> ConvertibleTo<bool>; { u > t } -> ConvertibleTo<bool>; { u <= t } -> ConvertibleTo<bool>; { u >= t } -> ConvertibleTo<bool>;
		};
		template <class _T, class _U = _T, class O = partial_ordering> concept ThreeWayComparable = requires (_T t, _U u) {
			{ t <=> u } -> ConvertibleTo<O>;
		};
		template <class _T> concept Movable = Object<_T> && Destructible<_T> && MoveConstructible<_T> && MoveAssignable<_T> && Swappable<_T>;
		template <class _T> concept Copyable = Movable<_T> && CopyConstructible<_T> && CopyAssignable<_T>;
		template <class _T> concept Semiregular = Copyable<_T> && DefaultConstructible<_T>;
		template <class _T> concept Regular = Semiregular<_T> && EqualityComparable<_T>;

		template <class _T> concept NullablePointer = Regular<_T> && EqualityComparable<_T, nullptr_t>&& Constructible<_T, nullptr_t>&& Assignable<_T, nullptr_t>;
		template <class _T> concept Allocator = Regular<_T> && requires (_T t, size_t n) {
			{ t.allocate(n) } -> ConvertibleTo<typename _T::value_type*>;
			t.deallocate(n);
		};
		template <class _T> concept Pointer = is_pointer_v<_T>;
		template <class _T> concept PointerToMember = is_member_pointer_v<_T>;
		template <class _T> concept PointerToMemberObject = is_member_object_pointer_v<_T>;
		template <class _T> concept PointerToMemberFunction = is_member_function_pointer_v<_T>;
		template <class _T> concept LvalueReference = is_lvalue_reference_v<_T>;
		template <class _T> concept RvalueReference = is_rvalue_reference_v<_T>;
		template <class _T> concept Enum = is_enum_v<_T>;
		template <class _T> concept Class = is_class_v<_T> || is_union_v<_T>;
		template <class _T> concept Struct = is_class_v<_T>;
		template <class _T> concept Union = is_union_v<_T>;
		//Function objects
		template <class _Fn, class... _Args>           concept Invocable = is_invocable_v<_Fn, _Args...>;
		template <class _Fn, class _R, class... _Args> concept InvocableAs = is_invocable_r_v<_R, _Fn, _Args...>;
		template <class _Fn, class... _Args>           concept NothrowInvocable = is_nothrow_invocable_v<_Fn, _Args...>;
		template <class _Fn, class _R, class... _Args> concept NothrowInvocableAs = is_nothrow_invocable_r_v<_R, _Fn, _Args...>;
		template <class _T>                            concept FunctionObject = requires (_T t) { t.operator(); } || is_function_v<remove_pointer_t<_T>>;
		template <class _T, class _U>                  concept Predicate = requires (_T t, _U a) { { t(a) } -> ConvertibleTo<bool>; };
		template <class _T, class _U, class _V>        concept BinaryPredicate = requires (_T t, _U a, _V b) { { t(a, b) } -> ConvertibleTo<bool>; };
		//Numerics
		template <class _T, class _U, bool Modulo = false, bool Order = false> concept ArithmeticallyOperableWith =
			(Order ? LessThanComparable<_T, _U> : EqualityComparable<_T, _U>) && requires(_T t, _U u) {
				{ _T(t + u) } noexcept; { _T(t - u) } noexcept; { _T(t * u) } noexcept; { _T(t / u) } noexcept;
				{ _U(t + u) } noexcept; { _U(t - u) } noexcept; { _U(t * u) } noexcept; { _U(t / u) } noexcept;
				{ _T(u + t) } noexcept; { _T(u - t) } noexcept; { _T(u * t) } noexcept; { _T(u / t) } noexcept;
				{ _U(u + t) } noexcept; { _U(u - t) } noexcept; { _U(u * t) } noexcept; { _U(u / t) } noexcept;
					requires !Modulo || requires{
						{ _T(t % u) } noexcept; { _U(t % u) } noexcept; { _T(u % t) } noexcept; { _U(u % t) } noexcept;
				};
		};
		template <class _T, class _U> concept BitwiseOperableWith = requires (_T t, _U u) {
			{ _T(t & u) } noexcept; { _T(t | u) } noexcept; { _T(t ^ u) } noexcept; { _T(t << u) } noexcept; { _T(t >> u) } noexcept;
			{ _T(t & u) } noexcept; { _T(t | u) } noexcept; { _T(t ^ u) } noexcept; { _T(t << u) } noexcept; { _T(t >> u) } noexcept;
			{ _T(t & u) } noexcept; { _T(t | u) } noexcept; { _T(t ^ u) } noexcept; { _T(t << u) } noexcept; { _T(t >> u) } noexcept;
			{ _T(t & u) } noexcept; { _T(t | u) } noexcept; { _T(t ^ u) } noexcept; { _T(t << u) } noexcept; { _T(t >> u) } noexcept;
		};
		template <class _T, bool B = true> concept Arithmetic = is_arithmetic_v<_T> || B && Regular<_T> && !is_enum_v<_T> &&
			ArithmeticallyOperableWith <_T, _T>&&
			ArithmeticallyOperableWith <_T, int>&&
			ArithmeticallyOperableWith <_T, unsigned int>&&
			ArithmeticallyOperableWith <_T, long>&&
			ArithmeticallyOperableWith <_T, unsigned long>&&
			ArithmeticallyOperableWith <_T, long long>&&
			ArithmeticallyOperableWith <_T, unsigned long long>&&
			ArithmeticallyOperableWith <_T, float>&&
			ArithmeticallyOperableWith <_T, double>&&
			ArithmeticallyOperableWith <_T, long double>&&
			requires(_T t) { { _T(+t) } noexcept; { _T(-t) } noexcept; { bool(t) } noexcept; };
		template <class _T, bool B = true> concept RealArithmetic = Arithmetic<_T, B>&& LessThanComparable<_T>&&
			LessThanComparable <_T, int>&&
			LessThanComparable <_T, unsigned int>&&
			LessThanComparable <_T, long>&&
			LessThanComparable <_T, unsigned long>&&
			LessThanComparable <_T, long long>&&
			LessThanComparable <_T, unsigned long long>&&
			LessThanComparable <_T, float>&&
			LessThanComparable <_T, double>&&
			LessThanComparable <_T, long double>;
		template <class _T, bool B = true> concept Integral = is_integral_v<_T> || B && Regular<_T> && RealArithmetic<_T, B> &&
			ArithmeticallyOperableWith <_T, _T, true, true>&&
			ArithmeticallyOperableWith <_T, int, true, true>&&
			ArithmeticallyOperableWith <_T, unsigned int, true, true>&&
			ArithmeticallyOperableWith <_T, long, true, true>&&
			ArithmeticallyOperableWith <_T, unsigned long, true, true>&&
			ArithmeticallyOperableWith <_T, long long, true, true>&&
			ArithmeticallyOperableWith <_T, unsigned long long, true, true>&&
			BitwiseOperableWith <_T, _T>&&
			BitwiseOperableWith <_T, int>&&
			BitwiseOperableWith <_T, unsigned int>&&
			BitwiseOperableWith <_T, long>&&
			BitwiseOperableWith <_T, unsigned long>&&
			BitwiseOperableWith <_T, long long>&&
			BitwiseOperableWith <_T, unsigned long long>&& Arithmetic<_T>;
		template <class _T, bool B = true> concept SignedIntegral = Integral<_T, B> && (is_signed_v<_T> || B && _T(-1) < _T(0));
		template <class _T, bool B = true> concept UnsignedIntegral = Integral<_T, B> && (is_unsigned_v<_T> || B && _T(-1) > _T(0));
		template <class _T, bool B = true> concept SignedArithmetic = RealArithmetic<_T, B> && (is_signed_v<_T> || B && _T(-1) < _T(0));
		template <class _T, bool B = true> concept UnsignedArithmetic = RealArithmetic<_T, B> && (is_unsigned_v<_T> || B && _T(-1) > _T(0));
		template <class _T, bool B = true> concept FloatingPoint = is_floating_point_v<_T> || B && Arithmetic<_T> && !Integral<_T> && (_T(0.25) != _T(0));
		template <class _T, bool B = true> concept RealFloatingPoint = FloatingPoint<_T, B>&& LessThanComparable<_T>;
		//Iterators
		template <class _T> concept LegacyIterator = Copyable<_T> && requires(_T t) {
			{ *t } -> LvalueReference;
			{ ++t } -> LayoutCompatibleWith<_T&>;
			{ t++ } -> LayoutCompatibleWith<_T>;
		};
		template <class _T> concept LegacyInputIterator = LegacyIterator<_T> && EqualityComparable<_T> && requires(_T t) {
			{ *t++ };
		};
		template <class _T> concept LegacyOutputIterator = LegacyIterator<_T> && requires(_T t) { *t = *t; };
		template <class _T, bool M = false> concept LegacyForwardIterator = LegacyInputIterator<_T> && Regular<_T> && (!M || LegacyOutputIterator<_T>);
		template <class _T, bool M = false> concept LegacyBidirectionalIterator = LegacyForwardIterator<_T, M>&& requires(_T t) {
			{ --t } -> LayoutCompatibleWith<_T&>;
			{ t-- } -> LayoutCompatibleWith<_T>;
		};
		template <class _T, bool M = false> concept LegacyRandomAccessIterator = LegacyBidirectionalIterator<_T, M>&& LessThanComparable<_T>&& requires(_T t, ptrdiff_t n) {
			{ t += n } -> LayoutCompatibleWith<_T&>;
			{ t + n } -> LayoutCompatibleWith<_T>;
			{ t -= n } -> LayoutCompatibleWith<_T&>;
			{ t - n } -> LayoutCompatibleWith<_T>;
			{ t - t } -> ConvertibleTo<ptrdiff_t>;
			{ t[n] } -> SameAs<decltype(*t)>;
		} && (!M || LegacyOutputIterator<_T>);
		template <class _T, bool M = false> concept LegacyContiguousIterator = LegacyRandomAccessIterator<_T, M>&& requires(_T t) {
			{ &*t } -> ConvertibleTo<_T>;
		};
		//Containers
		template <class _T> concept Container = Regular<_T> && requires(_T t) {
			{ t.begin() } -> LegacyForwardIterator;
			{ t.end() } -> LegacyForwardIterator;
			//{t.cbegin()} -> LegacyForwardIterator;
			//{t.cend()} -> LegacyForwardIterator;
			//t.swap(t); as swap(t, t)
			{ *t.begin() } -> Copyable;
		};
		template <class _T> concept ReversibleContainer = Container<_T> && requires(_T t) {
			{ t.rbegin() } -> LegacyBidirectionalIterator;
			{ t.rend() } -> LegacyBidirectionalIterator;
			//{t.crbegin()} -> LegacyBidirectionalIterator;
			//{t.crend()} -> LegacyBidirectionalIterator;
		};
		template <class _T> concept AllocatorAwareContainer = Container<_T> && requires(_T t) {
			{ t.get_allocator() } -> Allocator;
		};
		template <class _T> concept SequenceContainer = Container<_T> && requires(remove_cv_t<_T>&a, size_t n) {
			{ a[n] } -> Semiregular;
		};
		template <class _T> concept AssociativeContainer = Container<_T> && LessThanComparable<_T> && Constructible<_T, size_t>;
		template <class _T> concept UnorderedAssociativeContainer = Container<_T> && EqualityComparable<_T> && Constructible<_T, size_t>;
		//Other
		template <class _T> concept BitMaskType = Regular<_T> && requires (_T t, _T u) {
			{ _T(t & u) } noexcept;
			{ _T(t | u) } noexcept;
			{ _T(t ^ u) } noexcept;
			{ _T(t << 1) } noexcept;
			{ _T(t >> 1) } noexcept;
			{ _T(~t) } noexcept;
		};
		template <class _T> concept NumericType = !is_const_v<_T> && !is_volatile_v<_T> && Regular<_T>;
		template <class _T> concept BooleanTestable = ConvertibleTo<_T, bool>;
		template <class _T> concept Range = is_array_v<_T> || requires(_T t) {
			begin(t);
			end(t);
		};
		#else
		#pragma message("Concepts are introduced in C++20")
		#define _OLD_CXX 1
		#endif
	}
}
#else
#pragma message(__FILE__ " has been included twice.")
#endif
//===----------------------------------------------------------------------===//
//
// This file defines the isa<X>(), cast<X>(), dynCast<X>(), castOrNull<X>(),
// and dynCastOrNull<X>() templates.
//
// This file is based on llvm/include/Support/Casting.h from the LLVM project.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <type_traits>

namespace gintonic
{

/// \brief If T is a pointer to X, return a pointer to const X. If it is not,
/// return const T.
template <typename T, typename Enable = void> struct add_const_past_pointer
{
    typedef const T type;
};

template <typename T>
struct add_const_past_pointer<
    T, typename std::enable_if<std::is_pointer<T>::value>::type>
{
    typedef const typename std::remove_pointer<T>::type* type;
};

/// \brief If T is a pointer, just return it. If it is not, return T&.
template <typename T, typename Enable = void>
struct add_lvalue_reference_if_not_pointer
{
    typedef T& type;
};

template <typename T>
struct add_lvalue_reference_if_not_pointer<
    T, typename std::enable_if<std::is_pointer<T>::value>::type>
{
    typedef T type;
};

//===----------------------------------------------------------------------===//
//                          isa<x> Support Templates
//===----------------------------------------------------------------------===//

// Define a template that can be specialized by smart pointers to reflect the
// fact that they are automatically dereferenced, and are not involved with the
// template selection process...  the default implementation is a noop.
//
template <typename From> struct SimplifyType
{
    typedef From SimpleType; // The real type this represents...

    // An accessor to get the real value...
    static SimpleType& getSimplifiedValue(From& Val) { return Val; }
};

template <typename From> struct SimplifyType<const From>
{
    typedef typename SimplifyType<From>::SimpleType NonConstSimpleType;
    typedef
        typename add_const_past_pointer<NonConstSimpleType>::type SimpleType;
    typedef
        typename add_lvalue_reference_if_not_pointer<SimpleType>::type RetType;
    static RetType getSimplifiedValue(const From& Val)
    {
        return SimplifyType<From>::getSimplifiedValue(const_cast<From&>(Val));
    }
};

// The core of the implementation of isa<X> is here; To and From should be
// the names of classes.  This template can be specialized to customize the
// implementation of isa<> without rewriting it from scratch.
template <typename To, typename From, typename Enabler = void> struct isaImpl
{
    static inline bool doIt(const From& Val) { return To::classOf(&Val); }
};

/// \brief Always allow upcasts, and perform no dynamic check for them.
template <typename To, typename From>
struct isaImpl<To, From,
               typename std::enable_if<std::is_base_of<To, From>::value>::type>
{
    static inline bool doIt(const From&) { return true; }
};

template <typename To, typename From> struct isaImplCL
{
    static inline bool doIt(const From& Val)
    {
        return isaImpl<To, From>::doIt(Val);
    }
};

template <typename To, typename From> struct isaImplCL<To, const From>
{
    static inline bool doIt(const From& Val)
    {
        return isaImpl<To, From>::doIt(Val);
    }
};

template <typename To, typename From> struct isaImplCL<To, From*>
{
    static inline bool doIt(const From* Val)
    {
        assert(Val && "isa<> used on a null pointer");
        return isaImpl<To, From>::doIt(*Val);
    }
};

template <typename To, typename From> struct isaImplCL<To, From* const>
{
    static inline bool doIt(const From* Val)
    {
        assert(Val && "isa<> used on a null pointer");
        return isaImpl<To, From>::doIt(*Val);
    }
};

template <typename To, typename From> struct isaImplCL<To, const From*>
{
    static inline bool doIt(const From* Val)
    {
        assert(Val && "isa<> used on a null pointer");
        return isaImpl<To, From>::doIt(*Val);
    }
};

template <typename To, typename From> struct isaImplCL<To, const From* const>
{
    static inline bool doIt(const From* Val)
    {
        assert(Val && "isa<> used on a null pointer");
        return isaImpl<To, From>::doIt(*Val);
    }
};

template <typename To, typename From, typename SimpleFrom> struct isaImplWrap
{
    // When From != SimplifiedType, we can simplify the type some more by using
    // the SimplifyType template.
    static bool doIt(const From& Val)
    {
        return isaImplWrap<To, SimpleFrom,
                           typename SimplifyType<SimpleFrom>::SimpleType>::
            doIt(SimplifyType<const From>::getSimplifiedValue(Val));
    }
};

template <typename To, typename FromTy> struct isaImplWrap<To, FromTy, FromTy>
{
    // When From == SimpleType, we are as simple as we are going to get.
    static bool doIt(const FromTy& Val)
    {
        return isaImplCL<To, FromTy>::doIt(Val);
    }
};

// isa<X> - Return true if the parameter to the template is an instance of the
// template type argument.  Used like this:
//
//  if (isa<Type>(myVal)) { ... }
//
template <class X, class Y> inline bool isa(const Y& Val)
{
    return isaImplWrap<X, const Y,
                       typename SimplifyType<const Y>::SimpleType>::doIt(Val);
}

//===----------------------------------------------------------------------===//
//                          cast<x> Support Templates
//===----------------------------------------------------------------------===//

template <class To, class From> struct CastRetty;

// Calculate what type the 'cast' function should return, based on a requested
// type of To and a source type of From.
template <class To, class From> struct CastRettyImpl
{
    typedef To& RetType; // Normal case, return Ty&
};
template <class To, class From> struct CastRettyImpl<To, const From>
{
    typedef const To& RetType; // Normal case, return Ty&
};

template <class To, class From> struct CastRettyImpl<To, From*>
{
    typedef To* RetType; // Pointer arg case, return Ty*
};

template <class To, class From> struct CastRettyImpl<To, const From*>
{
    typedef const To* RetType; // Constant pointer arg case, return const Ty*
};

template <class To, class From> struct CastRettyImpl<To, const From* const>
{
    typedef const To* RetType; // Constant pointer arg case, return const Ty*
};

template <class To, class From, class SimpleFrom> struct CastRetty_wrap
{
    // When the simplified type and the from type are not the same, use the type
    // simplifier to reduce the type, then reuse CastRettyImpl to get the
    // resultant type.
    typedef typename CastRetty<To, SimpleFrom>::RetType RetType;
};

template <class To, class FromTy> struct CastRetty_wrap<To, FromTy, FromTy>
{
    // When the simplified type is equal to the from type, use it directly.
    typedef typename CastRettyImpl<To, FromTy>::RetType RetType;
};

template <class To, class From> struct CastRetty
{
    typedef typename CastRetty_wrap<
        To, From, typename SimplifyType<From>::SimpleType>::RetType RetType;
};

// Ensure the non-simple values are converted using the SimplifyType template
// that may be specialized by smart pointers...
//
template <class To, class From, class SimpleFrom> struct CastConvertVal
{
    // This is not a simple type, use the template to simplify it...
    static typename CastRetty<To, From>::RetType doIt(From& Val)
    {
        return CastConvertVal<To, SimpleFrom,
                              typename SimplifyType<SimpleFrom>::SimpleType>::
            doIt(SimplifyType<From>::getSimplifiedValue(Val));
    }
};

template <class To, class FromTy> struct CastConvertVal<To, FromTy, FromTy>
{
    // This _is_ a simple type, just cast it.
    static typename CastRetty<To, FromTy>::RetType doIt(const FromTy& Val)
    {
        typename CastRetty<To, FromTy>::RetType Res2 =
            (typename CastRetty<To, FromTy>::RetType) const_cast<FromTy&>(Val);
        return Res2;
    }
};

template <class X> struct is_simple_type
{
    static const bool value =
        std::is_same<X, typename SimplifyType<X>::SimpleType>::value;
};

// cast<X> - Return the argument parameter cast to the specified type.  This
// casting operator asserts that the type is correct, so it does not return null
// on failure.  It does not allow a null argument (use castOrNull for that).
// It is typically used like this:
//
//  cast<Instruction>(myVal)->getParent()
//
template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, const Y>::RetType>::type
cast(const Y& Val)
{
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return CastConvertVal<
        X, const Y, typename SimplifyType<const Y>::SimpleType>::doIt(Val);
}

template <class X, class Y>
inline typename CastRetty<X, Y>::RetType cast(Y& Val)
{
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return CastConvertVal<X, Y, typename SimplifyType<Y>::SimpleType>::doIt(
        Val);
}

template <class X, class Y>
inline typename CastRetty<X, Y*>::RetType cast(Y* Val)
{
    assert(isa<X>(Val) && "cast<Ty>() argument of incompatible type!");
    return CastConvertVal<X, Y*, typename SimplifyType<Y*>::SimpleType>::doIt(
        Val);
}

// castOrNull<X> - Functionally identical to cast, except that a null value is
// accepted.
//
template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, const Y>::RetType>::type
castOrNull(const Y& Val)
{
    if (!Val) return nullptr;
    assert(isa<X>(Val) && "castOrNull<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, Y>::RetType>::type
castOrNull(Y& Val)
{
    if (!Val) return nullptr;
    assert(isa<X>(Val) && "castOrNull<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

template <class X, class Y>
inline typename CastRetty<X, Y*>::RetType castOrNull(Y* Val)
{
    if (!Val) return nullptr;
    assert(isa<X>(Val) && "castOrNull<Ty>() argument of incompatible type!");
    return cast<X>(Val);
}

// dynCast<X> - Return the argument parameter cast to the specified type.  This
// casting operator returns null if the argument is of the wrong type, so it can
// be used to test for a type as well as cast if successful.  This should be
// used in the context of an if statement like this:
//
//  if (const Instruction *I = dynCast<Instruction>(myVal)) { ... }
//

template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, const Y>::RetType>::type
dynCast(const Y& Val)
{
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename CastRetty<X, Y>::RetType dynCast(Y& Val)
{
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename CastRetty<X, Y*>::RetType dynCast(Y* Val)
{
    return isa<X>(Val) ? cast<X>(Val) : nullptr;
}

// dynCastOrNull<X> - Functionally identical to dynCast, except that a null
// value is accepted.
//
template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, const Y>::RetType>::type
dynCastOrNull(const Y& Val)
{
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename std::enable_if<!is_simple_type<Y>::value,
                               typename CastRetty<X, Y>::RetType>::type
dynCastOrNull(Y& Val)
{
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

template <class X, class Y>
inline typename CastRetty<X, Y*>::RetType dynCastOrNull(Y* Val)
{
    return (Val && isa<X>(Val)) ? cast<X>(Val) : nullptr;
}

} // End gintonic namespace

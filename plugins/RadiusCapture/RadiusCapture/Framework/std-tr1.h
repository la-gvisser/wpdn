// ============================================================================
//	std-tr1.h
// ============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

// Copyright (c) Microsoft Corporation. All rights reserved.
// Copied from Visual Studio 2010 header file: memory
// Only the shared_ptr template is defined.
// Modified: all instances of _STD removed.

#pragma once

#ifdef _HAS_TR1

#define NSTR1	std::tr1

#else

#define NSTR1	tr1

namespace tr1 {

#ifndef _XSTD2
#define _XSTD2
#endif /* _XSTD2 */

#include <exception>
#include <typeinfo>

extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedIncrement(volatile long *);
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedDecrement(volatile long *);
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedCompareExchange(volatile long *, long, long);

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedCompareExchange)

#define _MT_INCR(mtx, x)	_InterlockedIncrement(&x)
#define _MT_DECR(mtx, x)	_InterlockedDecrement(&x)
#define _MT_CMPX(x, y, z)	_InterlockedCompareExchange(&x, y, z)

#define _CONVERTIBLE_TO_TRUE	(&_Bool_struct::_Member)

struct _Bool_struct {	// define member just for its address
	int _Member;
};

typedef int _Bool_struct::* _Bool_type;


// CLASS _Ref_count_base
class _Ref_count_base
{	// common code for reference counting
private:
	virtual void _Destroy() = 0;
	virtual void _Delete_this() = 0;

	const volatile void *_Ptr;
	long _Uses;
	long _Weaks;

protected:
	_Ref_count_base(const volatile void *_Px)
		: _Ptr(_Px), _Uses(0), _Weaks(1)
	{	// construct
	}

public:
	virtual ~_Ref_count_base()
	{	// ensure that derived classes can be destroyed properly
	}

	bool _Incref_nz()
	{	// increment use count if not zero, return true if successful
		for (; ; )
		{	// loop until state is known
			long _Count = (volatile long&)_Uses;
			if (_Count == 0)
				return (false);
			if (_MT_CMPX(_Uses, _Count + 1, _Count) == _Count)
				return (true);
		}
	}

	void _Incref()
	{	// increment use count
		_MT_INCR(_Mtx, _Uses);
	}

	void _Incwref()
	{	// increment weak reference count
		_MT_INCR(_Mtx, _Weaks);
	}

	void _Decref()
	{	// decrement use count
		if (_MT_DECR(_Mtx, _Uses) == 0)
		{	// destroy managed resource, decrement weak reference count
			_Destroy();
			_Decwref();
		}
	}

	void _Decwref()
	{	// decrement weak reference count
		if (_MT_DECR(_Mtx, _Weaks) == 0)
			_Delete_this();
	}

	long _Use_count() const
	{	// return use count
		return (_Uses);
	}

	bool _Expired() const
	{   // return true if _Uses == 0
		return (_Uses == 0);
	}

	virtual void *_Get_deleter(const _XSTD2 type_info&) const
	{	// return address of deleter object
		return (0);
	}

	const volatile void *_Getptr() const
	{	// return stored pointer
		return (_Ptr);
	}
};

// TEMPLATE CLASS _Ref_count, _Ref_count_d
template<class _Ty>
class _Ref_count
	: public _Ref_count_base
{	// handle reference counting for object without deleter
public:
	_Ref_count(_Ty *_Px)
		: _Ref_count_base(_Px)
	{	// construct
	}

private:
	virtual void _Destroy()
	{	// destroy managed resource
		delete (_Ty *)_Getptr();
	}

	virtual void _Delete_this()
	{	// destroy self
		delete this;
	}
};

template<class _Ty,
class _Dx,
class _Alloc = std::allocator<int> >
class _Ref_count_d
	: public _Ref_count_base
{	// handle reference counting for object with deleter
public:
	typedef _Ref_count_d<_Ty, _Dx, _Alloc> _Myty;
	typedef typename _Alloc::template rebind<_Myty>::other _Myalty;

	_Ref_count_d(_Ty *_Px, _Dx _Dt, _Myalty _Al = _Myalty())
		: _Ref_count_base(_Px), _Dtor(_Dt), _Myal(_Al)
	{	// construct
	}

	void *_Get_deleter(const _XSTD2 type_info& _Type) const
	{	// return address of deleter object
		return ((void*)(_Type == typeid(_Dx) ? &_Dtor : 0));
	}

private:
	virtual void _Destroy()
	{	// destroy managed resource
		_Dtor((_Ty *)_Getptr());
	}

	virtual void _Delete_this()
	{	// destroy self
		_Myalty _Al = _Myal;
		_Al.destroy(this);
		_Al.deallocate(this, 1);
	}

	_Myalty _Myal;
	_Dx _Dtor;
};

// DECLARATIONS
template<class _Ty>
class weak_ptr;
template<class _Ty>
class shared_ptr;
template<class _Ty>
class enable_shared_from_this;
struct _Static_tag {};
struct _Const_tag {};
struct _Dynamic_tag {};

template<class _Ty1,
class _Ty2>
	void _Do_enable(_Ty1*, enable_shared_from_this<_Ty2>*, _Ref_count_base*);

template<class _Ty>
inline void _Enable_shared(_Ty *_Ptr, _Ref_count_base *_Refptr,
						   typename _Ty::_EStype* = 0)
{	// reset internal weak pointer
	if (_Ptr)
		_Do_enable(_Ptr,
		(enable_shared_from_this<typename _Ty::_EStype>*)_Ptr, _Refptr);
}

inline void _Enable_shared(const volatile void *, const volatile void *)
{	// not derived from enable_shared_from_this; do nothing
}

// TEMPLATE CLASS _Ptr_base
template<class _Ty>
class _Ptr_base
{	// base class for shared_ptr and weak_ptr
public:
	typedef _Ty _Elem;
	typedef _Elem element_type;

	_Ptr_base()
		: _Ptr(0), _Rep(0)
	{	// construct
	}

	long use_count() const
	{	// return use count
		return (_Rep ? _Rep->_Use_count() : 0);
	}

	void _Swap(_Ptr_base& _Other)
	{	// swap pointers
		swap(_Rep, _Other._Rep);
		swap(_Ptr, _Other._Ptr);
	}

	template<class _Ty2>
	bool _Cmp(const _Ptr_base<_Ty2>& _Right) const
	{	// compare addresses of manager objects
		return (_Rep < _Right._Rep);
	}

	void *_Get_deleter(const _XSTD2 type_info& _Type) const
	{	// return pointer to deleter object if its type is _Type
		return (_Rep ? _Rep->_Get_deleter(_Type) : 0);
	}

	_Ty *_Get() const
	{	// return pointer to resource
		return (_Ptr);
	}

	bool _Expired() const
	{
		return (!_Rep || _Rep->_Expired());
	}

	void _Decref()
	{	// decrement reference count
		if (_Rep != 0)
			_Rep->_Decref();
	}

	void _Reset()
	{	// release resource
		_Reset(0, 0);
	}

	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other)
	{	// release resource and take ownership of _Other._Ptr
		_Reset(_Other._Ptr, _Other._Rep);
	}

	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, bool _Throw)
	{	// release resource and take ownership from weak_ptr _Other._Ptr
		_Reset(_Other._Ptr, _Other._Rep, _Throw);
	}

	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, const _Static_tag&)
	{	// release resource and take ownership of _Other._Ptr
		_Reset(static_cast<_Elem*>(_Other._Ptr), _Other._Rep);
	}

	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, const _Const_tag&)
	{	// release resource and take ownership of _Other._Ptr
		_Reset(const_cast<_Elem*>(_Other._Ptr), _Other._Rep);
	}
	template<class _Ty2>
	void _Reset(const _Ptr_base<_Ty2>& _Other, const _Dynamic_tag&)
	{	// release resource and take ownership of _Other._Ptr
		_Elem *_Ptr = dynamic_cast<_Elem*>(_Other._Ptr);
		if (_Ptr)
			_Reset(_Ptr, _Other._Rep);
		else
			_Reset();
	}

	template<class _Ty2>
	void _Reset(auto_ptr<_Ty2>& _Other)
	{	// release resource and take _Other.get()
		_Ty2 *_Px = _Other.get();
		_Reset(_Px, new _Ref_count<_Elem>(_Px));
		_Other.release();
	}

	void _Reset(_Ty *_Other_ptr, _Ref_count_base *_Other_rep)
	{	// release resource and take _Other_ptr through _Other_rep
		if (_Other_rep)
			_Other_rep->_Incref();
		if (_Rep != 0)
			_Rep->_Decref();
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
	}

	void _Reset(_Ty *_Other_ptr, _Ref_count_base *_Other_rep, bool _Throw)
	{	// take _Other_ptr through _Other_rep from weak_ptr if not expired
		// otherwise, leave in default state if !_Throw,
		// otherwise throw exception
		if (_Other_rep && _Other_rep->_Incref_nz())
		{   // take pointers
			if (_Rep != 0)
				_Rep->_Decref();
			_Rep = _Other_rep;
			_Ptr = _Other_ptr;
		}
		else if (_Throw)
			_Xweak();
	}

	void _Decwref()
	{	// decrement weak reference count
		if (_Rep != 0)
			_Rep->_Decwref();
	}

	void _Resetw()
	{	// release weak reference to resource
		_Resetw((_Elem*)0, 0);
	}

	template<class _Ty2>
	void _Resetw(const _Ptr_base<_Ty2>& _Other)
	{	// release weak reference to resource and take _Other._Ptr
		_Resetw(_Other._Ptr, _Other._Rep);
	}

	template<class _Ty2>
	void _Resetw(const _Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)
	{	// point to _Other_ptr through _Other_rep
		_Resetw(const_cast<_Ty2*>(_Other_ptr), _Other_rep);
	}

	template<class _Ty2>
	void _Resetw(_Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)
	{	// point to _Other_ptr through _Other_rep
		if (_Other_rep)
			_Other_rep->_Incwref();
		if (_Rep != 0)
			_Rep->_Decwref();
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
	}

private:
	_Ty *_Ptr;
	_Ref_count_base *_Rep;
	template<class _Ty0>
	friend class _Ptr_base;
};

// HELPER TEMPLATE CLASS _Ref
template<class _Ty>
struct _Ref
{	// ref for _Ty
	typedef _Ty& _Type;
};

template<> struct
_Ref<void>
{	// ref for void
	typedef void _Type;
};

template<>
struct _Ref<const void>
{	// ref for const void
	typedef void _Type;
};

template<>
struct _Ref<volatile void>
{	// ref for volatile void
	typedef void _Type;
};

template<>
struct _Ref<const volatile void>
{	// ref for const volatile void
	typedef void _Type;
};

// TEMPLATE CLASS shared_ptr
template<class _Ty>
class shared_ptr
	: public _Ptr_base<_Ty>
{	// class for reference counted resource management
public:
	shared_ptr()
	{	// construct empty shared_ptr object
		this->_Reset();
	}

	template<class _Ux>
	explicit shared_ptr(_Ux *_Px)
	{	// construct shared_ptr object that owns *_Px
		_Resetp(_Px);
	}

	template<class _Ux,
	class _Dx>
		shared_ptr(_Ux *_Px, _Dx _Dt)
	{	// construct with *_Px, deleter
		_Resetp(_Px, _Dt);
	}

	template<class _Ux,
	class _Dx,
	class _Alloc>
		shared_ptr(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
	{	// construct with *_Px, deleter, allocator
		_Resetp(_Px, _Dt, _Ax);
	}

	shared_ptr(const shared_ptr& _Other)
	{	// construct shared_ptr object that owns same resource as _Other
		this->_Reset(_Other);
	}

	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Other)
	{	// construct shared_ptr object that owns same resource as _Other
		this->_Reset(_Other);
	}

	template<class _Ty2>
	explicit shared_ptr(const weak_ptr<_Ty2>& _Other,
		bool _Throw = true)
	{	// construct shared_ptr object that owns resource *_Other
		this->_Reset(_Other, _Throw);
	}

	template<class _Ty2>
	explicit shared_ptr(auto_ptr<_Ty2>& _Other)
	{	// construct shared_ptr object that owns *_Other.get()
		this->_Reset(_Other);
	}

	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Other, const _Static_tag& _Tag)
	{	// construct shared_ptr object for static_pointer_cast
		this->_Reset(_Other, _Tag);
	}

	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Other, const _Const_tag& _Tag)
	{	// construct shared_ptr object for const_pointer_cast
		this->_Reset(_Other, _Tag);
	}

	template<class _Ty2>
	shared_ptr(const shared_ptr<_Ty2>& _Other, const _Dynamic_tag& _Tag)
	{	// construct shared_ptr object for dynamic_pointer_cast
		this->_Reset(_Other, _Tag);
	}

	~shared_ptr()
	{	// release resource
		this->_Decref();
	}

	shared_ptr& operator=(const shared_ptr& _Right)
	{	// assign shared ownership of resource owned by _Right
		this->_Reset(_Right);
		return (*this);
	}

	template<class _Ty2>
	shared_ptr& operator=(const shared_ptr<_Ty2>& _Right)
	{	// assign shared ownership of resource owned by _Right
		this->_Reset(_Right);
		return (*this);
	}

	template<class _Ty2>
	shared_ptr& operator=(auto_ptr<_Ty2>& _Right)
	{	// assign ownership of resource pointed to by _Right
		this->_Reset(_Right);
		return (*this);
	}

	void reset()
	{	// release resource and convert to empty shared_ptr object
		this->_Reset();
	}

	template<class _Ux>
	void reset(_Ux *_Px)
	{	// release, take ownership of _Px
		_Resetp(_Px);
	}

	template<class _Ux,
	class _Dx>
		void reset(_Ux *_Px, _Dx _Dt)
	{	// release, take ownership of _Px, with deleter _Dt
		_Resetp(_Px, _Dt);
	}

	template<class _Ux,
	class _Dx,
	class _Alloc>
		void reset(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
	{	// release, take ownership of _Px, with deleter _Dt, allocator _Ax
		_Resetp(_Px, _Dt, _Ax);
	}

	void swap(shared_ptr& _Other)
	{	// swap pointers
		this->_Swap(_Other);
	}

	_Ty *get() const
	{	// return pointer to resource
		return (this->_Get());
	}

	typename _Ref<_Ty>::_Type operator*() const
	{	// return reference to resource
		return (*this->_Get());
	}

	_Ty *operator->() const
	{	// return pointer to resource
		return (this->_Get());
	}

	bool unique() const
	{	// return true if no other shared_ptr object owns this resource
		return (this->use_count() == 1);
	}

	operator _Bool_type() const
	{	// test if shared_ptr object owns no resource
		return (this->_Get() != 0 ? _CONVERTIBLE_TO_TRUE : 0);
	}

private:
	template<class _Ux>
	void _Resetp(_Ux *_Px)
	{	// release, take ownership of _Px
		_TRY_BEGIN	// allocate control block and reset
			_Resetp0(_Px, new _Ref_count<_Ux>(_Px));
		_CATCH_ALL	// allocation failed, delete resource
			delete _Px;
		_RERAISE;
		_CATCH_END
	}

	template<class _Ux,
	class _Dx>
		void _Resetp(_Ux *_Px, _Dx _Dt)
	{	// release, take ownership of _Px, deleter _Dt
		typedef _Ref_count_d<_Ux, _Dx> _Refd;

		_TRY_BEGIN	// allocate control block and reset
			_Resetp0(_Px, new _Refd(_Px, _Dt));
		_CATCH_ALL	// allocation failed, delete resource
			_Dt(_Px);
		_RERAISE;
		_CATCH_END
	}

	template<class _Ux,
	class _Dx,
	class _Alloc>
		void _Resetp(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
	{	// release, take ownership of _Px, deleter _Dt, allocator _Ax
		typedef _Ref_count_d<_Ux, _Dx, _Alloc> _Refd;
		typename _Alloc::template rebind<_Refd>::other _Al = _Ax;

		_TRY_BEGIN	// allocate control block and reset
			_Refd *_Ptr = _Al.allocate(1);
		new (_Ptr) _Refd(_Px, _Dt, _Al);
		_Resetp0(_Px, _Ptr);
		_CATCH_ALL	// allocation failed, delete resource
			_Dt(_Px);
		_RERAISE;
		_CATCH_END
	}

	template<class _Ux>
	void _Resetp0(_Ux *_Px, _Ref_count_base *_Rx)
	{	// release resource and take ownership of _Px
		this->_Reset(_Px, _Rx);
		_Enable_shared(_Px, _Rx);
	}
};

template<class _Ty1,
class _Ty2>
	bool operator==(const shared_ptr<_Ty1>& _S1,
	const shared_ptr<_Ty2>& _S2)
{	// test if shared_ptr objects hold pointers that compare equal
	return (_S1.get() == _S2.get());
}

template<class _Ty1,
class _Ty2>
	bool operator!=(const shared_ptr<_Ty1>& _S1,
	const shared_ptr<_Ty2>& _S2)
{	// test if shared_ptr objects hold pointers that compare unequal
	return (!(_S1 == _S2));
}

template<class _Ty1,
class _Ty2>
	bool operator< (const shared_ptr<_Ty1>& _S1,
	const shared_ptr<_Ty2>& _S2)
{	// return true if _S1 precedes _S2 (order defined by control block)
	return (_S1._Cmp(_S2));
}

template<class _Elem,
class _Traits,
class _Ty>
	basic_ostream<_Elem, _Traits>&
	operator<<(basic_ostream<_Elem, _Traits>& _Out,
	const shared_ptr<_Ty>& _Px)
{	// write contained pointer to stream
	return (_Out << _Px.get());
}

template<class _Ty>
void swap(shared_ptr<_Ty>& _S1, shared_ptr<_Ty>& _S2)
{	// swap contents of _S1 and _S2
	_S1.swap(_S2);
}

template<class _Ty1,
class _Ty2>
	shared_ptr<_Ty1> static_pointer_cast(const shared_ptr<_Ty2>& _Other)
{	// return shared_ptr object holding static_cast<_Ty1 *)(_Other.get())
	return (shared_ptr<_Ty1>(_Other, _Static_tag()));
}

template<class _Ty1,
class _Ty2>
	shared_ptr<_Ty1> const_pointer_cast(const shared_ptr<_Ty2>& _Other)
{	// return shared_ptr object holding const_cast<_Ty1 *)(_Other.get())
	return (shared_ptr<_Ty1>(_Other, _Const_tag()));
}

template<class _Ty1,
class _Ty2>
	shared_ptr<_Ty1> dynamic_pointer_cast(const shared_ptr<_Ty2>& _Other)
{	// return shared_ptr object holding dynamic_cast<_Ty1*)(_Other.get())
	return (shared_ptr<_Ty1>(_Other, _Dynamic_tag()));
}

template<class _Dx,
class _Ty>
	_Dx *get_deleter(const shared_ptr<_Ty>& _Sx)
{	// return pointer to shared_ptr's deleter object if its type is _Ty
	return ((_Dx *)_Sx._Get_deleter(typeid(_Dx)));
}

} // namespace tr1

#endif	// _HAS_TR1

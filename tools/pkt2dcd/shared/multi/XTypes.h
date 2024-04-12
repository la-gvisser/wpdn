// ============================================================================
//	XTypes.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#ifndef XTYPES_H
#define XTYPES_H

#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include "AGTypes.h"

//
// return values
//

#define X_OK	0
#define X_FAIL	-1
#define X_FALSE	1
#define X_SUCCEEDED(x)	((x) >=0)
#define X_FAILED(x)		((x) < 0)


//
// Type macros that provide getters and setters:
//
// X_DECLARE_OBJ(y, x)     - declare a member called m_p[x] of type y
// X_DECLARE_TYPE(y, x, z) - declare a member called m_p[x] of type 
//                            y with a type id of z
// X_DECLARE_PTR(y, x)      - declare a pointer called m_p[x] of type y
// X_DECLARE_CONST_PTR(y, x)- declare a const pointer called m_p[x] of type y
// X_DECLARE_BOOL(x)        - declare a bool called m_b[x]
//                            the getter is called ::Is[x]
// X_DECLARE_INT(x)	        - declare an int called m_n[x]
// X_DECLARE_UINT8(x)       - declare a UInt8 called m_n[x]
// X_DECLARE_UINT16(x)      - declare a UInt16 called m_n[x]
// X_DECLARE_UINT32(x)      - declare a UInt32 called m_n[x]
// X_DECLARE_UINT64(x)      - declare a UInt64 called m_n[x]
// X_DECLARE_SINT8(x)       - declare a SInt8 called m_n[x]
// X_DECLARE_SINT16(x)      - declare a SInt16 called m_n[x]
// X_DECLARE_SINT32(x)      - declare a SInt32 called m_n[x]
// X_DECLARE_SINT64(x)      - declare a SInt64 called m_n[x]
//
// X_DECLARE_STR(y x)       - declare a ptr that calls strdup/free
//
// Macros to create an Init() method to initialize members:
//
// X_BEGIN_INIT_MAP(y) 
// X_END_MAP(y) 
//
// Macros to initialize members in a map:
//
// X_INIT_PTR( y );
// X_INIT_BOOL( y );
// X_INIT_NUMBER( y );
//
// The X_DECLARE_* macros take the following parameters:
//
// x is the raw name of the member data.  It is prefixed
// with "m_" and the appropriate identifier like: "p"
// for pointer, "n" for number, and "b" for bool
//
// y is a type name for ptr and obj macros.  
//
// Each macro provide a Set and Get method.  These 
// methods are suffixed with the name of the member
//
// Example: 
//
//  class CBar :
//  	virtual public XObject
//  {
//    public:
//  	CBar() { Init(); }
//
//  	X_DECLARE_PTR( TCHAR, Char );
//  	X_DECLARE_BOOL( Collapsed );
//  	X_DECLARE_INT( Count );
//  	X_DECLARE_UINT16( DataOffset );
//  	X_DECLARE_UINT16( DataSize );
//	
//  	X_BEGIN_INIT_MAP(CDecodeTreeItem)
//		  X_INIT_PTR( Char );
//	      X_INIT_BOOL( Collapsed );
//		  X_INIT_NUMBER( Count );
//		  X_INIT_NUMBER( DataOffset );
//		  X_INIT_NUMBER( DataSize );
//	    X_END_MAP
//  };
//

#define X_DECLARE_OBJ(y, x) \
protected: \
	y m_ ## x; \
public: \
	inline void Set ## x( y in ## x ) { m_ ## x = in ## x; } \
	inline y& Get ## x() { return m_ ## x; } \
	inline y* Get ## x ## Ptr() { return &m_ ## x; } 

#define X_DECLARE_CONST_OBJ(y, x) \
protected: \
	y m_ ## x; \
public: \
	inline void Set ## x( const y& in ## x ) { m_ ## x = in ## x; } \
	inline const y& Get ## x() { return m_ ## x; } 

#define X_DECLARE_TYPE(y, x, z) \
protected: \
	y m_ ## z ## x; \
public: \
	inline void Set ## x( y in ## x ) { m_ ## z ## x = in ## x; } \
	inline y& Get ## x() { return m_ ## z ## x; } \
	inline y* Get ## x ## Ptr() { return &m_ ## z ## x; } 

#define X_DECLARE_CONST_TYPE(y, x, z) \
protected: \
	y m_ ## z ## x; \
public: \
	inline void Set ## x( const y& in ## x ) { m_ ## z ## x = in ## x; } \
	inline const y& Get ## x() { return m_ ## z ## x; } 

#define X_DECLARE_PTR(y, x) \
protected: \
	y* m_p ## x; \
public: \
	inline void Set ## x( y* in ## x ) { m_p ## x = in ## x; } \
	inline y* Get ## x() { return m_p ## x; } 

#define X_DECLARE_CONST_PTR(y, x) \
protected: \
	const y* m_p ## x; \
public: \
	inline void Set ## x( const y* in ## x ) { m_p ## x = in ## x; } \
	inline const y* Get ## x() { return m_p ## x; } 

#define X_DECLARE_BOOL(x) \
protected: \
	BOOL m_b ## x; \
public: \
	inline void Set ## x( const BOOL in ## x ) { m_b ## x = in ## x; } \
	inline const BOOL Is ## x() { return m_b ## x; } 

#define X_DECLARE_STR(y, x) \
protected: \
	y* m_p ## x; \
public: \
	inline void Set ## x( y* in ## x ) { if (m_p ## x) free(m_p ## x); m_p ## x = strdup( in ## x ); } \
	inline y* Get ## x() { return m_p ## x; } 


#define X_DECLARE_INT(x)	 X_DECLARE_TYPE( int,    x, n )
#define X_DECLARE_UINT8(x)   X_DECLARE_TYPE( UInt8,  x, n )
#define X_DECLARE_UINT16(x)  X_DECLARE_TYPE( UInt16, x, n )
#define X_DECLARE_UINT32(x)  X_DECLARE_TYPE( UInt32, x, n )
#define X_DECLARE_UINT64(x)  X_DECLARE_TYPE( UInt64, x, n )
#define X_DECLARE_SINT8(x)   X_DECLARE_TYPE( SInt8,  x, n )
#define X_DECLARE_SINT16(x)  X_DECLARE_TYPE( SInt16, x, n )
#define X_DECLARE_SINT32(x)  X_DECLARE_TYPE( SInt32, x, n ) 
#define X_DECLARE_SINT64(x)  X_DECLARE_TYPE( SInt64, x, n )


//  counter 

#define X_DECLARE_COUNTER(x) \
	X_DECLARE_TYPE( int,    x, n ) \
	inline void Incr ## x(int i=1) { m_n ## x += i; } \
	inline void Decr ## x(int i=1) { m_n ## x -= i; } 
	

// macros to initialize members declared using above macros

#define X_BEGIN_INIT_MAP(x) \
public: \
X_RESULT Init() { \
	int ret = X_OK; {

#define X_BEGIN_INIT_MAP2(x,y) \
X_RESULT Init() { \
	X_RESULT ret = y::Init(); \
	if (ret==X_OK) {

#define X_INIT_BOOL(x) m_b ## x = false;
#define X_INIT_NUMBER(x) m_n ## x = 0;
#define X_INIT_PTR(x) m_p ## x = NULL;

#define X_END_MAP \
	} \
	return ret; \
} \

//
// Allocations and deallocation macros
//

#define X_SAFE_RELEASE(x) \
if (x) \
{ \
	x->Release(); \
	x = NULL; \
}

#define X_SAFE_DELETE(x) \
if (x) \
{ \
	delete x; \
	x = NULL; \
}


#define X_SAFE_FREE(x) \
if (x) \
{ \
	free(x); \
	x = NULL; \
}


//-----------------------------------------------------------
// define X_GUID and X_DEFINE_GUID.  These are modeled after
// the struct _GUID and DEFINE_GUID() in COM.  It should be
// possible to convert the whole framework to COM.

#if defined(WIN32) || defined(_LINUX)
typedef struct _X_GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} X_GUID;

#define X_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const X_GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

//#define X_GUID UInt32
//#define X_DEFINE_GUID(N,O,P,Q,R,S,T,U,V,X,Y,Z) const X_GUID N = O;
#else
//zz20010625 fix: reorder #define order so that X_GUID is already defined before the X_DEFINE_GUID macro,
// then remove the trailing semicolon from the statement because otherwise you get a lot
// of "illegal empty statement" warnings (because human beings put semicolons after macro
// invocations, even though they don't need to).
#define X_GUID UInt32
#define X_DEFINE_GUID(N,O,P,Q,R,S,T,U,V,X,Y,Z) \
const X_GUID N = O
#endif

// X_GUID comparison macros

#define X_InlineIsEqualGUID(rguid1, rguid2)  \
        (((unsigned long *) &rguid1)[0] == ((unsigned long *) &rguid2)[0] &&   \
        ((unsigned long *) &rguid1)[1] == ((unsigned long *) &rguid2)[1] &&    \
        ((unsigned long *) &rguid1)[2] == ((unsigned long *) &rguid2)[2] &&    \
        ((unsigned long *) &rguid1)[3] == ((unsigned long *) &rguid2)[3])

#define X_IsEqualGUID(rguid1, rguid2) (!memcmp(&rguid1, &rguid2, sizeof(X_GUID)))


// object map macros
#define X_BEGIN_OBJ_MAP(Y) \
namespace Y \
{ \
	X_RESULT XCreateInstance( X_GUID guid, XObject** ppObject ) \
	{ 
		

#define X_OBJ_ENTRY(X) \
	if (X_InlineIsEqualGUID( X::GetClassID(), guid )) \
	{\
		X* pNew = NULL; \
		X::Create( &pNew ); \
		*ppObject = pNew; \
		return X_OK; \
	}

#define X_MAP_ENTRY(X) \
	if (X::CreateInstance( guid, ppObject ) == X_OK ) \
		return X_OK;

#define X_END_OBJ_MAP return X_FAIL; }  }

		
//--------------------------
//	X_BEGIN_ID_MAP(X)
//--------------------------

// Any class can use the following macros to enable
// interface based programming.  Using these macros
// allows a consumer to query an object for a specific 
// interface that may or not be in the base class 
// hierarchy.  

#define X_BEGIN_ID_MAP(Y) public: \
typedef Y				_Class; \
typedef XSmartPtr< Y >	SmartPtr; \
static	const X_GUID&	GetClassID() { return Y ## ID; } \
virtual const X_GUID&	GetObjID() { return Y ## ID; } \
virtual const char*		GetClassName() { return #Y; } \
virtual BOOL			HasInterface(const X_GUID& guid) \
{ void* pVoid=NULL; return (QueryInterface(guid,&pVoid) == X_OK) ? TRUE : FALSE; } \
virtual int				QueryInterface(const X_GUID& guid, void** pResult) const \
{\
	int err = X_FAIL;


// Interface Macros that are used to build an interface map
// They are always placed between the X_BEGIN_ID_MAP()
// and X_END_ID_MAP


// X_ID_ENTRY(Y)
//
// Y = a classid like XObjectID
//
// This macro is always used by a class on itself

#define X_ID_ENTRY(Y)\
	if (guid.Data1 == Y.Data1)\
	{\
		*pResult = const_cast<_Class*>(this);\
		return X_OK; \
	}

// X_CLASS_ID_ENTRY(Y,Z)
//
// Y = a classname like X_Object
// Z = a classid like X_ObjectID
//
// This macro is used by subclasses that delegate
// up to their baseclasses if guid==X

#define X_CLASS_ID_ENTRY(Y,Z)\
	if (guid.Data1 == Y.Data1)\
	{\
		err = Z::QueryInterface( guid, pResult);\
		return err;\
	}

// X_CLASS_ENTRY(Y)
//
// Y = a classname like X_Object
//
// This macro is used by subclasses that delegate
// up to their baseclasses unconditionally. 

#define X_CLASS_ENTRY(Y)\
	err = Y::QueryInterface( guid, pResult);\
	if (err == X_OK)\
	{\
		return X_OK;\
	}

// X_OBJECT_ENTRY(Y)
//
// Y = an aggregated object
//
// This allows for aggregation (ie tear-offs)
// Feel the power!  Beware of QI loops!

#define X_OBJECT_ENTRY(Y)\
	if (Y) {\
		err = Y->QueryInterface( guid, pResult);\
		if (err == X_OK)\
		{\
			return X_OK;\
		}\
	}

// X_OBJECT_ID_ENTRY(Y,Z)
//
// Y = an aggregated object
// Z = a classname like X_Object
//
// This allows for aggregation (ie tear-offs)
// of multiple objects of the same type. It
// matches on X instead of the Y->qi

#define X_OBJECT_ID_ENTRY(Y,Z)\
	if (guid.Data1 == Y.Data1)\
	{\
		*pResult = Z;\
		return X_OK; \
	}

// X_OBJECT_LAST_ENTRY(Y)
//
// X = an aggregated object
//
// This should only be used as the last entry
// It will always return.  Protects against 
// objects that point to each other

#define X_OBJECT_LAST_ENTRY(Y)\
	if (Y)\
	{\
		return Y->QueryInterface( guid, pResult);\
	}

// X_END_ID_MAP
//
// Always end the map with this 

#define X_END_ID_MAP \
	if (err == X_FAIL)\
	{\
		*pResult = NULL;\
	}\
	return err;\
}

// function macros
typedef SInt32 X_RESULT;
#define X_METHOD(x)				X_RESULT x
#define X_TRY					try
#define X_THROW(x)				throw x;
#define X_CATCH(x)				catch(X_RESULT x) {}
#define X_STDMETHOD(x)			virtual X_RESULT x
#define X_STDMETHODIMP			X_RESULT


// x variant type stuff

enum XVariantType
{
	kpvoid,
	kppvoid,
	kx_result,
	kx_guid,
	kpxobject,
	kppxobject
};

//class XObject;

typedef struct XVARIANT
{	
	XVariantType nType;
	union
	{
		void*				pvoid;
		void**				ppvoid;
		X_RESULT			x_result;
		X_GUID				x_guid;
		//XObject*			pxobject;
		//XObject**			ppxobject;
	};
} XVARIANT;


class XVariant : 
	//public XObject,
	public XVARIANT
{
public:
	XVariantType GetType() 
	{
		return nType;
	}
};



#endif

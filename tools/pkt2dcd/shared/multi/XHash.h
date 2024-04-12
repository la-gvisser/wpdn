// ============================================================================
//	XHash.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002-2004. All rights reserved.

#ifndef XHASH_H
#define XHASH_H

#include "XObject.h"
#include "XList.h"


//--------------------------------------------------

// {0F53C6D2-D5DC-11d3-8863-0050DA087DA3}
X_DEFINE_GUID(XKeyID, 
0xf53c6d2, 0xd5dc, 0x11d3, 0x88, 0x63, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

template<class X>
class DECLSPEC XKey : virtual public XObject
{
  public:

	XKey(X iKey=-1) : m_iKey( iKey ) { }
	virtual ~XKey() {}

	X_BEGIN_ID_MAP(XKey)
		X_ID_ENTRY(XKeyID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
	
	virtual X GetKey() { return m_iKey; }
	virtual int SetKey(X iKey) { m_iKey = iKey; return X_OK; }
	virtual int SetKey(XKey<X>* pKey) { m_iKey = pKey->GetKey(); return X_OK; }

  protected:

    X   m_iKey;
};

typedef XKey<int> XKeyInt;
typedef XKey<UInt8> XKey8;
typedef XKey<UInt16> XKey16;
typedef XKey<UInt32> XKey32;


//------------------------------------------

// {ECD63E4D-5C9F-4061-888D-861F2A0C7D57}
X_DEFINE_GUID(XHashID, 
0xecd63e4d, 0x5c9f, 0x4061, 0x88, 0x8d, 0x86, 0x1f, 0x2a, 0xc, 0x7d, 0x57);

template<class T>
class DECLSPEC XHash : 
	virtual public XMutex,
	virtual public XOwner,
	virtual public XSwitch
{
  public:

	XHash();
	virtual ~XHash();

	X_BEGIN_ID_MAP(XHash)
		X_ID_ENTRY(XHashID)
		X_CLASS_ENTRY(XMutex)
		X_CLASS_ENTRY(XOwner)
		X_CLASS_ENTRY(XSwitch)
	X_END_ID_MAP
	
	X_DECLARE_CREATE( XHash );

	virtual int			Clear();
	virtual T*			GetItem(XKey32* pKey);
	virtual T*			GetItem(UInt32 iKey);
	virtual XLink<T>*	GetLink(UInt32 iKey);
	virtual int			PutItem(XKey32* pKey, T* pT);
	virtual int			PutItem(UInt32 iKey, T* pT);
	virtual UInt32		GetCount();
	virtual UInt32		GetMaxQueues();
	virtual UInt32		GetHashSize();
	virtual void		SetHashSize(UInt32 nSize);
	virtual UInt32		GetTotal();
	virtual int			SetChain(bool bChain);

#if 0
	virtual int			Write(FILE* fd);
#endif

  protected:

	virtual XLink<T>*	CreateLink(T* pT);

	XLink<T>**		m_ppTable;
	UInt32			m_iMaxQueues;	// this is the real max, meaning that m_ppTable[m_iMaxQueues] is OK
	UInt32			m_iTotal;
	bool			m_isClearing;
	XBitField64		m_flags;	

};

#define X_DEFINE_HASH_CLASS(T,X) typedef XHash< X > T;

//----------------------------------------

// {C5BDB2E3-3A45-4d43-B905-B09AA96B3951}
X_DEFINE_GUID(XLinkedHashID, 
0xc5bdb2e3, 0x3a45, 0x4d43, 0xb9, 0x5, 0xb0, 0x9a, 0xa9, 0x6b, 0x39, 0x51);

template<class T>
class DECLSPEC XLinkedHash : virtual public XHash<T>
{
  public:

	virtual ~XLinkedHash();

	X_BEGIN_ID_MAP(XLinkedHash)
		X_ID_ENTRY(XLinkedHashID)
		//X_CLASS_ENTRY( (XHash< T, X >) )
	X_END_ID_MAP
		
	X_DECLARE_CREATE( XLinkedHash );
	
	virtual int		Clear();
	virtual int		PutItem(UInt32 iKey, T* pT);
	virtual int		PutItem(T* pT);
	virtual T*		GetItem(UInt32 iKey);
	virtual T*		GetItem(T& pClone);
	virtual T*		Find(UInt32 iKey, T* pT);
	virtual int		Remove( UInt32 iKey, T* pT);
	virtual int		Remove( T* pT);

#if 0
	virtual int		Write(FILE* fd);
	virtual int		Read(FILE *fd);
#endif
};

#define X_DEFINE_LINKEDHASH_CLASS(T,X) typedef XLinkedHash< X > T;

//---------------------------------------

// {F52E394C-0DCC-44c1-BBB0-CBDBA1A6140F}
X_DEFINE_GUID(XLinkedHashIteratorID, 
0xf52e394c, 0xdcc, 0x44c1, 0xbb, 0xb0, 0xcb, 0xdb, 0xa1, 0xa6, 0x14, 0xf);

template <class Y, class T>
class DECLSPEC XLinkedHashIterator : public XObject
{
  public:

	XLinkedHashIterator(Y* pHash);

	X_BEGIN_ID_MAP(XLinkedHashIterator)
		X_ID_ENTRY(XLinkedHashIteratorID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
			
	virtual T* GetNext();
	virtual T* GetFirst();

  protected:

	XLink<T>*	m_pCurrent;
	Y*			m_pHash;
	UInt32		m_iCurrent;
};


//----------------------------------------


// {F8F7E424-6FFE-456f-8DAE-1202777AA6B5}
X_DEFINE_GUID(XMapLinkID, 
0xf8f7e424, 0x6ffe, 0x456f, 0x8d, 0xae, 0x12, 0x2, 0x77, 0x7a, 0xa6, 0xb5);

template<class Y, class T>
class DECLSPEC XMapLink : virtual public XLink<T>
{
  public:
	  
	  XMapLink(T* t=NULL) : XLink<T>(t) {}
	//virtual ~XMapLink();

	X_BEGIN_ID_MAP(XMapLink)
		X_ID_ENTRY(XMapLinkID)
		X_CLASS_ENTRY(XLink<T>)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
	
	X_DECLARE_OBJ( Y, Key );
};

//----------------------------------------

// {DCEDAE0D-AA72-4b1f-8982-9DC203DF46F8}
X_DEFINE_GUID(XMapID, 
0xdcedae0d, 0xaa72, 0x4b1f, 0x89, 0x82, 0x9d, 0xc2, 0x3, 0xdf, 0x46, 0xf8);

template<class Y, class T>
class DECLSPEC XMap : virtual public XLinkedHash<T>
{
  public:

	X_BEGIN_ID_MAP(XMap)
		X_ID_ENTRY(XMapID)
		//X_CLASS_ENTRY( (XHash< T, X >) )
	X_END_ID_MAP
		
	X_DECLARE_CREATE( XMap );
	
	virtual int		SetAt(  const Y& key, T* pT);
	virtual T*		Lookup(  const Y&	key );
	virtual int		RemoveKey(  const Y& key );

protected:
	virtual XLink<T>*	CreateLink(T* pT);

};

#define X_DEFINE_MAP_CLASS(X,Y,T) typedef XMap< Y, T > X;

//----------------------------------------

#ifdef _LINUX
#include "XGUIDUtil.h"

// {557DC308-8363-4bee-BC3E-54FE885BBF07}
X_DEFINE_GUID(XGUIDMapID, 
0x557dc308, 0x8363, 0x4bee, 0xbc, 0x3e, 0x54, 0xfe, 0x88, 0x5b, 0xbf, 0x7);

template<class T>
class DECLSPEC XGUIDMap : virtual public XObject
{
  public:

	//virtual ~XGUIDMap();

	X_BEGIN_ID_MAP(XGUIDMap)
		X_ID_ENTRY(XGUIDMapID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
		
	X_DECLARE_CREATE( XGUIDMap );
	
	virtual int			SetAt( const X_GUID& key, T* pT);
	virtual T*			Lookup(  const X_GUID&	key );
	virtual UInt32		GetCount() { return m_Map.GetCount(); }
	virtual int			RemoveKey(  const X_GUID& key );
	virtual int			RemoveAll( );

protected:
	XMap<XString,T> m_Map;

};

#define X_DEFINE_GUIDMAP_CLASS(T,X) typedef XGUIDMap< X > T;
#endif

#include "XHash.cpp"

#endif

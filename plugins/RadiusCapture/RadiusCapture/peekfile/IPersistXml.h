

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Jul 02 19:09:27 2003
 */
/* Compiler settings for IPersistXml.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IPersistXml_h__
#define __IPersistXml_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPersistXml_FWD_DEFINED__
#define __IPersistXml_FWD_DEFINED__
typedef interface IPersistXml IPersistXml;
#endif 	/* __IPersistXml_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IPersistXml_INTERFACE_DEFINED__
#define __IPersistXml_INTERFACE_DEFINED__

/* interface IPersistXml */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_IPersistXml;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("83FE0429-CBE9-47a2-98E5-4B858464AF9C")
    IPersistXml : public IPersist
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ IUnknown *pXmlDoc,
            IUnknown *pXmlNode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ IUnknown *pXmlDoc,
            /* [in] */ IUnknown *pXmlNode,
            /* [in] */ BOOL fClearDirty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistXmlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistXml * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistXml * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistXml * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistXml * This,
            /* [out] */ CLSID *pClassID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            IPersistXml * This,
            /* [in] */ IUnknown *pXmlDoc,
            IUnknown *pXmlNode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IPersistXml * This,
            /* [in] */ IUnknown *pXmlDoc,
            /* [in] */ IUnknown *pXmlNode,
            /* [in] */ BOOL fClearDirty);
        
        END_INTERFACE
    } IPersistXmlVtbl;

    interface IPersistXml
    {
        CONST_VTBL struct IPersistXmlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistXml_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistXml_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistXml_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistXml_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistXml_Load(This,pXmlDoc,pXmlNode)	\
    (This)->lpVtbl -> Load(This,pXmlDoc,pXmlNode)

#define IPersistXml_Save(This,pXmlDoc,pXmlNode,fClearDirty)	\
    (This)->lpVtbl -> Save(This,pXmlDoc,pXmlNode,fClearDirty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPersistXml_Load_Proxy( 
    IPersistXml * This,
    /* [in] */ IUnknown *pXmlDoc,
    IUnknown *pXmlNode);


void __RPC_STUB IPersistXml_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPersistXml_Save_Proxy( 
    IPersistXml * This,
    /* [in] */ IUnknown *pXmlDoc,
    /* [in] */ IUnknown *pXmlNode,
    /* [in] */ BOOL fClearDirty);


void __RPC_STUB IPersistXml_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistXml_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



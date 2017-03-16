#pragma once

#include <vector>

#include "Macroses.h"

#if defined _M_X64
#define Api_File "PDFXEditCore.x64.dll"
#else
#define Api_File "PDFXEditCore.x86.dll"
#endif

#pragma warning( disable : 4192 )
#pragma warning( disable : 4278 )

#import Api_File rename_namespace("PXV"), raw_interfaces_only, exclude("LONG_PTR", "ULONG_PTR", "UINT_PTR")

#pragma warning( default : 4278 )
#pragma warning( default : 4192 )

#include "CabHelper.h"

typedef CComPtr<PXV::IPXV_Inst> t_pInst;

//typedef CComPtr<PXV::ICabNode> t_pCabNode;

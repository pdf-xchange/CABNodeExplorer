// CABNodeExplorerRightView.cpp : implementation of the CCABNodeExplorerRightView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CABNodeExplorerRightView.h"
#include "CabInst.h"
#include "MainFrm.h"

void CCABNodeExplorerRightView::FullCab(const CString& strPath)
{
	_cab_node_t parentNode = m_pWorker->m_rootNode[strPath];

	if (parentNode.GetType() == PXV::dt_Array)
	{
		CString strName;
		int length = parentNode.size();
		for (int i = 0; i < length; i++)
		{
			strName.Format(L"%d", i);
			_cab_node_t pNode(parentNode.at(strName.GetBuffer()));
			strName.Format(L"[%d]", i);
			insetCabItem(i, strName, pNode);
		}
		return;
	}
	auto names = parentNode.Dictionary_GetKeys();
	int nIndex = 0;
	for (auto strName : names)
	{
		_cab_node_t pNode(parentNode.at(strName.GetBuffer()));
		insetCabItem(nIndex, strName, pNode);
		nIndex++;
	}
}

void CCABNodeExplorerRightView::ShowCab(const CString& strPath)
{
	DeleteAllItems();
	FullCab(strPath);
}

void CCABNodeExplorerRightView::ShowChildCab(const CString& strPath)
{
	LVITEM pItem = { sizeof(LVITEM) };
	pItem.state = LVIF_TEXT;
	GetSelectedItem(&pItem);
	CString strPath1(strPath);
	strPath1.Append(L".");
	strPath1.Append(pItem.pszText);
	DeleteAllItems();
	FullCab(strPath);
}

void CCABNodeExplorerRightView::insetCabItem(const int nIndex, const CString strName, _cab_node_t& pNode)
{
	InsertItem(nIndex, strName);
	PXV::CabDataTypeID idType = pNode.GetType();

	CString strType(_GetNameType(idType).c_str());
	CString strValue;
	switch (idType)
	{
	case PXV::dt_Undefined:
		strValue = "Undefined";
		break;
	case PXV::dt_Bool:
		strValue = (bool)pNode ? "true" : "false";
		break;
	case PXV::dt_Int:
		strValue.Format(L"%d", (int)pNode);
		break;
	case PXV::dt_Int64:
		strValue.Format(L"%I64d", (__int64)pNode);
		break;
	case PXV::dt_Double:
		strValue.Format(L"%f", (double)pNode);
		break;
	case PXV::dt_String:
		strValue.Format(L"\"%s\"", (LPCWSTR)pNode);
		break;
	case PXV::dt_Stream:
		strValue = "Stream";
		break;
	case PXV::dt_IUnknown:
	{
		_unknown_t unk;
		pNode.GetValue(unk);
		strValue.Format(L"0x%Hd", (void*)unk);
	}
	break;
	case PXV::dt_Container:
		strValue = "Container";
		break;
	case PXV::dt_Array:
		strValue.Format(L"[%d]", pNode.size());
		break;
	case PXV::dt_Dictionary:
		strValue.Format(L"Items count: %d", pNode.size());
		break;
	default:
		break;
	}
	SetItemText(nIndex, 1, strType);
	SetItemText(nIndex, 2, strValue);
}

CCABNodeExplorerRightView::CCABNodeExplorerRightView()
{
}

CCABNodeExplorerRightView::~CCABNodeExplorerRightView()
{
}

BOOL CCABNodeExplorerRightView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (pMsg->hwnd == this->m_hWnd)
		{
			switch (nVirtKey)
			{
			case VK_RETURN:
			{
				int nRow = GetNextItem(-1, LVNI_SELECTED);
				CString str;
				GetItemText(nRow, 0, str);
				m_pMain->LoadPathChild(str);
			}
				return TRUE;
				break;
			case VK_BACK:
				m_pMain->SendMessageW(WM_COMMAND, MAKEWPARAM(ID_EXPLORERBACK, BN_CLICKED));
				return TRUE;
				break;
			default:
				break;
			}
			
		}
	}
	return FALSE;
}

LRESULT CCABNodeExplorerRightView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	bHandled = FALSE;
	if (IsThemingSupported())
		SetWindowTheme(m_hWnd, L"Explorer", NULL);

	// Put your intitialization code here
	
	return lRes;
}

LRESULT CCABNodeExplorerRightView::OnLVItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	CString str;
	if (pnmh->hwndFrom == this->m_hWnd)
	{
		POINT pt;
		::GetCursorPos((LPPOINT)&pt);
		this->ScreenToClient(&pt);

		LVHITTESTINFO lvhti;
		lvhti.pt = pt;
		this->HitTest(&lvhti);
		LVITEM lvi;
		if (lvhti.flags & LVHT_ONITEM)
		{
			// 		this->ClientToScreen(&pt);
			// 		lvi.mask = LVIF_PARAM;
			// 		lvi.iItem = lvhti.iItem;
			// 		lvi.iSubItem = 0;
			// 		if (!this->GetItem(&lvi))
			// 			return 0;

			GetItemText(lvhti.iItem, 0, str);
		}
	}
	m_pMain->SetSelect(str);
}

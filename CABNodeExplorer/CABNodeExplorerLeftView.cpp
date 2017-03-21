// CABNodeExplorerLeftView.cpp : implementation of the CCABNodeExplorerLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CABNodeExplorerLeftView.h"
#include "CabInst.h"
#include "MainFrm.h"
#include <functional>

CCABNodeExplorerLeftView::CCABNodeExplorerLeftView()
{
}

CCABNodeExplorerLeftView::~CCABNodeExplorerLeftView()
{
}

BOOL CCABNodeExplorerLeftView::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

LRESULT CCABNodeExplorerLeftView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//int* a = new int();
	//*a = 22;
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	bHandled = FALSE;
	if (IsThemingSupported())
		SetWindowTheme(m_hWnd, L"Explorer", NULL);
	return lRes;
}

LRESULT CCABNodeExplorerLeftView::OnItemExpanding(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMTREEVIEW* pnmtv = (NMTREEVIEW*)pnmh;
	if ((pnmtv->action & TVE_EXPAND) != TVE_EXPAND)
	{
		return FALSE;
	}

	if (pnmtv->action & TVE_COLLAPSE)
		return TRUE;    // don't allow it
	else
	{
		TVITEM& pItem = (pnmtv)->itemNew;
		if (pItem.lParam == 0)
			return FALSE;
		CTreeItem hRoot(pItem.hItem, this);
		if (hRoot.GetChild().IsNull())
		//if (!hRoot.HasChildren())
		{
			_cab_node_t pNode((PXV::ICabNode*)pItem.lParam, true);
			FillItem(hRoot, pNode);
		}
		return FALSE;   // allow it
	}
}

LRESULT CCABNodeExplorerLeftView::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW lpTV = (LPNMTREEVIEW)pnmh;
	LPTVITEM pItem = &(lpTV->itemNew);
	CTreeItem treeItem(pItem->hItem, this);
	if (pItem->lParam == 0)
		return 0;

	_cab_node_t pNode((PXV::ICabNode*)pItem->lParam, true);
	//pNode.p
	
	std::function<void(const CTreeItem& item, CString& strPath)> _path = [&](const CTreeItem& item, CString& strPath) 
	{
		CTreeItem itemParent = item.GetParent();
		if (!itemParent.IsNull())
			_path(item.GetParent(), strPath);
		CString str;
		item.GetText(str);
		strPath.Append(L".");
		strPath.Append(str);
	};
		
	CString strPath;
	_path(treeItem, strPath);
	strPath.Delete(0, 2);
	m_pMain->LoadPath(strPath);
	return 0;
}

LRESULT CCABNodeExplorerLeftView::OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pnmh);
	TVITEM* pItem = &(pDispInfo)->item;
	if (pItem->lParam == 0)
		return 0;

	_cab_node_t pNode((PXV::ICabNode*)pItem->lParam, true);
// 	if (pItem->mask & TVIF_TEXT)
// 	{
// 		lstrcpyn(pItem->pszText, m_pWorker->TreeItemText(pNode), pItem->cchTextMax);
// 	}
	if (pItem->mask & TVIF_IMAGE)
	{
		pItem->iImage = m_pWorker->TreeItemImage(pNode);
	}
	if (pItem->mask & TVIF_SELECTEDIMAGE)
	{
		pItem->iSelectedImage = m_pWorker->TreeItemSelectedImage(pNode);
	}
	if (pItem->mask & TVIF_CHILDREN)
	{
		const PXV::CabDataTypeID arrType[] = { PXV::dt_Array,  PXV::dt_Dictionary };
		pItem->cChildren = _IsTypeItem(pNode, arrType, _countof(arrType)) ? 1 : 0;
	}
	return S_OK;
}

void CCABNodeExplorerLeftView::FillRoot()
{
	CTreeItem hRoot = GetRootItem();
	CTreeItem hItem = InsertItem(L"", hRoot, TVI_LAST);
	FillItem(hItem, m_pWorker->m_rootNode);
	hItem.Expand();
}

void CCABNodeExplorerLeftView::FillItem(CTreeItem& parentItem, _cab_node_t& parentNode)
{
	auto names = parentNode.Dictionary_GetKeys();
	for (auto strName : names)
	{
		_cab_node_t pNode(parentNode.at(strName.GetBuffer()));

		PXV::CabDataTypeID idType = pNode.GetType();
		if (idType == PXV::dt_Dictionary || idType == PXV::dt_Array)
		{
			CString str(strName);
			//if (idType == PXV::dt_Array)
			//	str.Format(L"%s[%d]", (LPCWSTR)strName, pNode.size());

			TVINSERTSTRUCT tvs = { 0 };
			tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
			tvs.item.pszText = str.GetBuffer();
			tvs.item.iImage = I_IMAGECALLBACK;
			tvs.item.iSelectedImage = I_IMAGECALLBACK;
			tvs.item.cChildren = I_CHILDRENCALLBACK;

			tvs.item.lParam = (LPARAM)pNode.Detach();
			tvs.hParent = parentItem;
			tvs.hInsertAfter = TVI_LAST;
			InsertItem(&tvs);
		}
	}
}

void CCABNodeExplorerLeftView::ShowCab(const CString& strPath)
{
	std::function<void(CTreeItem& item, const CString& strPath)> _path = [&](CTreeItem& item, const CString& strPath)
	{
		CString strFind = strPath.SpanExcluding(L".");
		bool bLast = strPath == strFind;
		CTreeItem hItem = item;
		while (hItem != NULL)
		{
			//SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);

			CString strName;
			hItem.GetText(strName);
			if (strName == strFind)
			{
				if (bLast)
				{
					//hItem.SetState(TVIS_SELECTED, TVIS_SELECTED);
					//hItem.Select(TVGN_CARET | TVGN_FIRSTVISIBLE | TVGN_DROPHILITE);
					hItem.Select(TVGN_CARET );
					//hItem.Select(TVGN_FIRSTVISIBLE);
				}
				else if (hItem.HasChildren())
				{
					if (hItem.GetChild() == NULL)
						hItem.Expand();
					_path(hItem.GetChild(), strPath.Mid(strFind.GetLength() + 1));
				}
			}
			hItem = hItem.GetNextSibling();
		}
	};
	CTreeItem hItem = GetRootItem();
	_path(hItem.GetChild(), strPath);
}


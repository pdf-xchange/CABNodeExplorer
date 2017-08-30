#include "stdafx.h"
#include "resource.h"
#include "ToolBarFind.h"
#include "CabInst.h"
#include "MainFrm.h"


BOOL CToolBarFind::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN && m_cb.IsChild(pMsg->hwnd))
		{
			SendMessageW(WM_COMMAND, MAKEWPARAM(ID_BTN_FIND, BN_CLICKED));
			return TRUE;
		}
	}
	return FALSE;
}

CToolBarFind::CToolBarFind()
{
}


CToolBarFind::~CToolBarFind()
{
}

LRESULT CToolBarFind::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// let the combo box initialize itself
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

	SetDroppedWidth(m_cxMinDropWidth);
	if (lRet != -1)
	{
		// adjust the drop-down width
		m_cb = GetComboCtrl();
		m_cb.SetDroppedWidth(m_cxMinDropWidth);

		CEdit edt = GetEditCtrl();
		//m_edit.Attach(edt);

		m_tb = CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(m_hWnd, IDT_FIND, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST);
		LPCTSTR lpszStringsFind = _T("Find\0");
		LPCTSTR lpszStringsNext = _T("Next\0");

		int nIndexFind = m_tb.CommandToIndex(ID_BTN_FIND);
		int nIndexNext = m_tb.CommandToIndex(ID_BTN_FIND_NEXT);
		int nStringIDNext = m_tb.AddStrings(lpszStringsNext);
		int nStringIDFind = m_tb.AddStrings(lpszStringsFind);
		TBBUTTON tbFind;
		TBBUTTON tbNext;
		m_tb.GetButton(nIndexFind, &tbFind);
		m_tb.GetButton(nIndexNext, &tbNext);
		tbFind.iString = nStringIDFind;
		tbNext.iString = nStringIDNext;
		tbFind.fsStyle |= TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
		tbNext.fsStyle |= TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
		m_tb.DeleteButton(nIndexFind);
		m_tb.DeleteButton(nIndexNext);
		m_tb.InsertButton(nIndexFind, &tbFind);
		m_tb.InsertButton(nIndexNext, &tbNext);


		RECT btnRect;
		RECT rect;
		m_tb.GetItemRect(nIndexFind, &btnRect);
		m_tb.GetItemRect(0, &rect);
		LONG nWidthOfOneButton = btnRect.right - btnRect.left;
		m_sizeTB.cx = rect.right + nWidthOfOneButton;
		m_sizeTB.cy = rect.bottom;
	}
	return lRet;
}

LRESULT CToolBarFind::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_edit.Detach();
	return 0;
}

LRESULT CToolBarFind::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	return S_OK;
}

LRESULT CToolBarFind::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_tb.m_hWnd == NULL)
	{
		bHandled = FALSE;
		return 1;
	}
	// copy the WINDOWPOS struct and adjust for the buttons
	WINDOWPOS wp = *(LPWINDOWPOS)lParam;
	wp.cx -= m_sizeTB.cx + m_cxGap;
	LRESULT lRet = DefWindowProc(uMsg, wParam, (LPARAM)&wp);

	// paint below the  buttons
	RECT rcBut = { wp.cx, 0, wp.cx + m_sizeTB.cx + m_cxGap, wp.cy };
	InvalidateRect(&rcBut);

	// center the buttons relative to the combo box
	RECT rcCombo;
	m_cb.GetWindowRect(&rcCombo);
	int y = (rcCombo.bottom - rcCombo.top - m_sizeTB.cy) / 2;
	// position the buttons on the right
	m_tb.SetWindowPos(NULL, wp.cx + m_cxGap, y, m_sizeTB.cx, m_sizeTB.cy, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	return lRet;
}

LRESULT CToolBarFind::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_edit.SetFocus();
	return 0;
}

void CToolBarFind::UpdateHistory()
{
	CString str;
	GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	if (!str.IsEmpty())
	{
		if (FindStringExact(-1, str) != CB_ERR)
			return;

		while (GetCount() > 10)
		{
			DeleteString(GetCount() - 1);
		}
		InsertItem(0, str, 0, 0, 0);
	}
}

bool CToolBarFind::FindCurrNameInCab()
{
	CString str;
	GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	if (!str.IsEmpty())
	{
		CString sPath;
		CString sKeyName;
		if (FindCurrNameInCab(sPath, sKeyName, m_pWorker->m_rootNode))
		{
			m_vAlreadyFound.push_back(sPath);
			m_pMain->m_wndView1.ShowCab(sPath);
			_cab_node_t pNode(m_pWorker->m_rootNode.at(sPath.GetBuffer()));
			PXV::CabDataTypeID idType = pNode.GetType();
			int nSelectItemAtRight = -1;
			if (idType != PXV::dt_Dictionary && idType != PXV::dt_Array)
			{
				for (int i = sPath.GetLength() - 1; i >= 0; i--)
					if (sPath[i] == L'.') 
					{
						sPath.Delete(i, sPath.GetLength() - i);
						break;
					}
				
				_cab_node_t pParentNode(m_pWorker->m_rootNode.at(sPath.GetBuffer()));
				auto vKeys = pParentNode.Dictionary_GetKeys();
				auto it = find(vKeys.begin(), vKeys.end(), sKeyName);
				if (it != vKeys.end())
					nSelectItemAtRight = (int)std::distance(vKeys.begin(), it);
			}
			m_pMain->m_wndToolBarPath.AddItem(sPath, 0, 0, 0);
			m_pMain->m_wndView1.ShowCab(sPath);

			m_pMain->m_wndView2.DeleteAllItems();
			m_pMain->m_wndView2.FullCab(sPath);
			m_pMain->m_wndView2.SetFocus();
			if (idType != PXV::dt_Dictionary && idType != PXV::dt_Array)
			{
				m_pMain->m_wndView2.SelectItem(nSelectItemAtRight);
			}
		}
		else
		{
			if (m_vAlreadyFound.empty())
				MessageBox(L"Haven't found any coincidence.", L"Info");
			else 
			{
				MessageBox(L"It was last coincidence.", L"Info");
			}
		}
	}
	return false;
}

bool CToolBarFind::FindCurrNameInCab(CString & sParent, CString & sFoundKey, _cab_node_t& parentNode)
{
	CString str;
	GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	CString sBefore = sParent;
	if (!str.IsEmpty())
	{
		auto names = parentNode.Dictionary_GetKeys();
		for (auto strName : names)
		{
			CString sNameBefore = strName;
			_cab_node_t pNode(parentNode.at(strName.GetBuffer()));
			PXV::CabDataTypeID idType = pNode.GetType();
			if (!sParent.IsEmpty())
				sParent.Append(L".");
			sParent.Append(strName);
			str.MakeLower();
			strName.MakeLower();
			int nLast = strName.GetLength();
			int nResult = strName.Find(str, 0);
			CString sVal = GetStringOfValue(pNode);
			if ((strName.Find(str) != -1) || (sVal.Find(str) != -1))
			{
				if (std::find(m_vAlreadyFound.begin(), m_vAlreadyFound.end(), sParent) == m_vAlreadyFound.end())
				{
					sFoundKey = sNameBefore;
					return true;
				}		
			}		
			if (idType == PXV::dt_Dictionary)
			{
				if (FindCurrNameInCab(sParent, sFoundKey, pNode))
					return true;
			}
			sParent = sBefore;
		}
	}
	sParent = sBefore;
	return false;
}


CString CToolBarFind::GetStringOfValue( _cab_node_t& pNode)
{
	PXV::CabDataTypeID idType = pNode.GetType();
	CString strValue;
	if (idType != PXV::dt_Dictionary && idType != PXV::dt_Array)
	{
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
			break;
		}
		case PXV::dt_Container:
			strValue = "Container";
			break;
		default:
			break;
		}
	}
	return strValue;
}


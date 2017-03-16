#include "stdafx.h"
#include "resource.h"
#include "ToolBarCombo.h"


BOOL CToolBarCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN && m_cb.IsChild(pMsg->hwnd))
		{
			SendMessageW(WM_COMMAND, MAKEWPARAM(ID_EXPLORERGO, BN_CLICKED));
			return TRUE;
		}
	}
	return FALSE;
}

CToolBarCombo::CToolBarCombo()
{
}


CToolBarCombo::~CToolBarCombo()
{
}

LRESULT CToolBarCombo::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
		m_edit.Attach(edt);

		// create a toolbar for the GO button
		m_tb = CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(m_hWnd, IDT_ADDRESS, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST);
		LPCTSTR lpszStrings = _T("Go\0");
		m_tb.AddStrings(lpszStrings);

		RECT rect;
		m_tb.GetItemRect(0, &rect);
		m_sizeTB.cx = rect.right;
		m_sizeTB.cy = rect.bottom;
	}
	return lRet;
}

LRESULT CToolBarCombo::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_edit.Detach();
	return 0;
}

LRESULT CToolBarCombo::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
// 	CDCHandle dc = (HDC)wParam;
// 	CWindow wndParent = GetParent();
// 
// 	// Forward this to the parent window, rebar bands are transparent
// 	POINT pt = { 0, 0 };
// 	MapWindowPoints(wndParent, &pt, 1);
// 	dc.OffsetWindowOrg(pt.x, pt.y, &pt);
// 	LRESULT lRet = wndParent.SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
// 	dc.SetWindowOrg(pt.x, pt.y);
// 
// 	bHandled = (lRet != 0);
// 	return lRet;
	bHandled = TRUE;
	return S_OK;
}

LRESULT CToolBarCombo::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_tb.m_hWnd == NULL)
	{
		bHandled = FALSE;
		return 1;
	}

	// copy the WINDOWPOS struct and adjust for the GO button
	WINDOWPOS wp = *(LPWINDOWPOS)lParam;
	wp.cx -= m_sizeTB.cx + m_cxGap;
	LRESULT lRet = DefWindowProc(uMsg, wParam, (LPARAM)&wp);

	// paint below the GO button
	RECT rcGo = { wp.cx, 0, wp.cx + m_sizeTB.cx + m_cxGap, wp.cy };
	InvalidateRect(&rcGo);

	// center the GO button relative to the combo box
	RECT rcCombo;
	m_cb.GetWindowRect(&rcCombo);
	int y = (rcCombo.bottom - rcCombo.top - m_sizeTB.cy) / 2;
	//		if(y < 0)
	//			y = 0;

	// position the GO button on the right
	m_tb.SetWindowPos(NULL, wp.cx + m_cxGap, y, m_sizeTB.cx, m_sizeTB.cy, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	return lRet;
}

LRESULT CToolBarCombo::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//m_cb.SetCurSel(-1);
	m_edit.SetFocus();
	return 0;
}

LRESULT CToolBarCombo::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_RETURN)
	{
		SendMessageW(WM_COMMAND, MAKEWPARAM(ID_EXPLORERGO, BN_CLICKED));
	}
	return FALSE;
}


void CToolBarCombo::UpdateHistory()
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

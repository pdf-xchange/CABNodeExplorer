// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"
#include <algorithm>

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::SetSplitterPos(int iPos)
{
	if (iPos < -1)
	{
		RECT rc;
		m_wndSplitter.GetClientRect(&rc);
		iPos = (rc.right - rc.left) / (-iPos);
	}
	m_wndSplitter.SetSplitterPos(iPos);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;
		
	if (m_wndView1.PreTranslateMessage(pMsg))
		return TRUE;
		
	if (m_wndView2.PreTranslateMessage(pMsg))
		return TRUE;

	if (m_wndToolBarPath.PreTranslateMessage(pMsg))
		return TRUE;
	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	UIEnable(ID_FILE_SAVE, FALSE);
	UIEnable(ID_FILE_SAVE_AS, FALSE);
	UIEnable(ID_EDIT_CUT, FALSE);
	UIEnable(ID_EDIT_COPY, m_worker.CanSelect());
	UIEnable(ID_EDIT_PASTE, FALSE);
	UIEnable(ID_FILE_PRINT, FALSE);
	//UIEnable(ID_FILE_SAVE, FALSE);
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	m_CmdBar.SetCommandBarExtendedStyle(0, CBR_EX_SHAREMENU);
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_wndToolBarPath.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBS_DROPDOWN | CBS_AUTOHSCROLL);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
	AddSimpleReBarBand(m_wndToolBarPath, NULL, TRUE);
	

	CreateSimpleStatusBar();
	// Creating splitter and views
	m_hWndClient = m_wndSplitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_wndView1.m_pMain = this;
	m_wndView1.m_pWorker = &m_worker;
	m_wndView1.Create(m_wndSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		TVS_TRACKSELECT | TVS_DISABLEDRAGDROP | TVS_NONEVENHEIGHT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_FULLROWSELECT, WS_EX_CLIENTEDGE);
	m_wndView1.SetExtendedStyle(TVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
	//m_wndView1.SetExtendedStyle(TVS_EX_FADEINOUTEXPANDOS, TVS_EX_FADEINOUTEXPANDOS);
	//m_wndTabLogView.SetExtendedListViewStyle(LVS_EX_AUTOSIZECOLUMNS, LVS_EX_AUTOSIZECOLUMNS);
	m_wndView1.FillRoot();

	m_wndView2.m_pMain = this;
	m_wndView2.m_pWorker = &m_worker;
	m_wndView2.Create(m_wndSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_wndView2.SetExtendedListViewStyle(LVS_EX_REGIONAL, LVS_EX_REGIONAL);
	m_wndView2.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	m_wndView2.SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
	m_wndView2.SetExtendedListViewStyle(LVS_EX_INFOTIP, LVS_EX_INFOTIP);
	//	m_wndView2.SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP, LVS_EX_HEADERDRAGDROP);	
	//	m_wndView2.SetExtendedListViewStyle(LVS_EX_FLATSB, LVS_EX_FLATSB);	
	m_wndView2.SetView(LV_VIEW_DETAILS);
	m_wndView2.SetUnicodeFormat();
	m_wndView2.InsertColumn(0, L"Name", 0, 300);
	m_wndView2.InsertColumn(1, L"Type", 0, 300);
	m_wndView2.InsertColumn(2, L"Value", 0, 300);


	m_wndSplitter.SetSplitterPanes(m_wndView1, m_wndView2);
	PostMessage(MSG_POSTCREATE);
	// End creating splitter and views

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	m_wndView1.SetFocus();
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMainFrame::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetSplitterPos(-3);
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnAppGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	if (hWndCtl == m_wndView2)
	{

	}
	else
	{
		CString strPath;
		m_wndToolBarPath.GetWindowText(strPath);
		m_wndToolBarPath.AddItem(strPath, 0, 0, 0);
		m_wndView1.ShowCab(strPath);
		//m_wndView2.ShowCab(strPath);
		m_wndView2.SetFocus();
	}
	return 0;
}

LRESULT CMainFrame::OnAppBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	if (hWndCtl == m_wndToolBarPath)
	{
		bHandled = FALSE;
		return 0;
	}
	CString strPath;
	m_wndToolBarPath.GetWindowText(strPath);
	int pos = strPath.ReverseFind(L'.');
	strPath.ReleaseBuffer(pos);
	m_wndView1.ShowCab(strPath);
	return 0;
}

LRESULT CMainFrame::OnAppNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
	const HWND listWnd[] = { m_wndView2, m_wndView1/*, m_wndToolBarPath*/};
	HWND hFocus = GetFocus();
	HWND hFind = m_wndView2;
	int nLen = _countof(listWnd);
	for (int i = 0;  i < nLen; i++)
	{
		if (listWnd[i] == hFocus)
		{
			hFind = listWnd[(i + 1) % nLen];
			break;
		}
	}
	::SetFocus(hFind);
	return 0;
}

LRESULT CMainFrame::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ATLASSERT(::IsWindow(m_hWnd));
	BOOL bOK = OpenClipboard();
	if (bOK != FALSE)
	{
		bOK = EmptyClipboard();
		if (bOK != FALSE)
		{
			CStringW str = m_worker.GetSelect();
			CGlobalHeap glb;
			{
				CStringA strASCII(str);
				const size_t nSize = (strASCII.GetLength() + 1) * sizeof(char);
				HGLOBAL hGlob = glb.Allocate(nSize);
				strcpy_s((LPSTR)hGlob, nSize, strASCII.GetBuffer());
				bOK = SetClipboardData(CF_TEXT, hGlob) != NULL;
				if (bOK == FALSE)
					glb.Free(hGlob);
			}
			{
				const size_t nSize = (str.GetLength() + 1) * sizeof(wchar_t);
				HGLOBAL hGlob = glb.Allocate(nSize);
				wcscpy_s((LPWSTR)hGlob, str.GetLength() + 1, str.GetBuffer());
				bOK = SetClipboardData(CF_UNICODETEXT, hGlob) != NULL;
				if (bOK == FALSE)
					glb.Free(hGlob);
			}
		}
		CloseClipboard();
	}
	return (bOK != FALSE);
}

void CMainFrame::LoadPath(const CString& strPath)
{
	m_wndToolBarPath.SetWindowText(strPath);
	m_wndView2.ShowCab(strPath);
	m_worker.SetSelect(strPath);
}

void CMainFrame::LoadPathChild(const CString& strPath)
{
	CString str;
	m_wndToolBarPath.GetWindowText(str);
	str.Append(L".");
	str.Append(strPath);
	m_wndToolBarPath.SetWindowText(str);
	m_wndView1.ShowCab(str);
	m_wndView2.ShowCab(str);

}

void CMainFrame::SetSelect(LPCWSTR sName)
{
	CString str;
	m_wndToolBarPath.GetWindowText(str);
	str.Append(L".");
	str.Append(sName);
	m_worker.SetSelect(str);
}


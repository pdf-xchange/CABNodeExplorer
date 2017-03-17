// CABNodeExplorerRightView.h : interface of the CCABNodeExplorerRightView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CCabInst;
class CMainFrame;

class CCABNodeExplorerRightView : public CWindowImpl<CCABNodeExplorerRightView, CListViewCtrl>, public CTheme
{
public:
	
	DECLARE_WND_SUPERCLASS(_T("CCABNodeExplorerRightView"), CListViewCtrl::GetWndClassName())

private:

public:
	CCabInst* m_pWorker = nullptr;
	CMainFrame* m_pMain = nullptr;

	void FullCab(const CString& strPath);
	void ShowCab(const CString& strPath);
	void ShowChildCab(const CString& strPath);
	void insetCabItem(const int nIndex, const CString strName, _cab_node_t& pNode);
public:
	CCABNodeExplorerRightView();
	~CCABNodeExplorerRightView();
	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CCABNodeExplorerRightView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnLVItemClick)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK, OnLVItemClick)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLVItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
};

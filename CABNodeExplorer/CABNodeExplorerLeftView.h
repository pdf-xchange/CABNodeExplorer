// CABNodeExplorerLeftView.h : interface of the CCABNodeExplorerLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CCabInst;
class CMainFrame;

class CCABNodeExplorerLeftView : public CWindowImpl<CCABNodeExplorerLeftView, CTreeViewCtrlEx>, public CTheme
{
public:
	typedef CWindowImpl<CCABNodeExplorerLeftView, CTreeViewCtrl> _baseclass;
	DECLARE_WND_SUPERCLASS(_T("CCABNodeExplorerLeftView"), CTreeViewCtrl::GetWndClassName())
	
private:
	CImageList	m_TreeImages;
public:
	CCabInst* m_pWorker = nullptr;
	CMainFrame* m_pMain = nullptr;

	void FillRoot();
	void FillItem(CTreeItem& parentItem, _cab_node_t& pNode);
	void ShowCab(const CString& strPath);
public:

	CCABNodeExplorerLeftView();
	~CCABNodeExplorerLeftView();
	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CCABNodeExplorerLeftView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnItemExpanding)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_GETDISPINFO, OnGetDispInfo)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnItemExpanding(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
};

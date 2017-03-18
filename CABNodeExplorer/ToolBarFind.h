#pragma once

class CCabInst;
class CMainFrame;

class CToolBarFind : public CWindowImpl<CToolBarFind, CComboBoxEx>
	//public CEditCommands<CToolBarFind>
{
public:
	typedef CWindowImpl<CToolBarFind, CComboBoxEx> baseClass;


	class CEditFind : // a CEdit handling VK_RETURN and Edit commands
		public CWindowImpl<CEditFind, CEdit>, public CEditCommands<CEditFind>
	{
	public:
		DECLARE_WND_SUPERCLASS(L"PDFExplorer40.CToolBarFind.CEditFind", CEdit::GetWndClassName())

		BEGIN_MSG_MAP(CEditFind)
			//MESSAGE_RANGE_HANDLER(WM_KEYFIRST, WM_KEYLAST, OnKey)
			ALT_MSG_MAP(1)
			if ((this->m_hWnd == ::GetFocus()))
			{
				CHAIN_MSG_MAP_ALT(CEditCommands<CEditFind>, 1)
			}
		END_MSG_MAP()

		//LRESULT OnKey(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	};

	CToolBarFind();
	~CToolBarFind();
	enum
	{
		m_cxGap = 4,
		m_cxMinDropWidth = 800
	};

	CComboBox m_cb;
	CEditFind m_edit;
	CToolBarCtrl m_tb;
	SIZE m_sizeTB;

	CCabInst* m_pWorker = nullptr;
	CMainFrame* m_pMain = nullptr;


	std::vector<CString> m_vAlreadyFound;

	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_SUPERCLASS(_T("PDFExplorer_ComboBox"), CComboBoxEx::GetWndClassName())


	BEGIN_MSG_MAP(CToolBarFind)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		ALT_MSG_MAP(1)
		CHAIN_MSG_MAP_ALT_MEMBER(m_edit, 1)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	void UpdateHistory();
	bool FindCurrNameInCab();

private:
	bool FindCurrNameInCab(CString & sParent, _cab_node_t& parentNode);
};


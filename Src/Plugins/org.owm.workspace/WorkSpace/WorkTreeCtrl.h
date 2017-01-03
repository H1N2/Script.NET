#if !defined(AFX_WORKTREECTRL_H__D8058500_582D_11D4_BEF6_000021605F53__INCLUDED_)
#define AFX_WORKTREECTRL_H__D8058500_582D_11D4_BEF6_000021605F53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkTreeCtrl window

// ���ڵ����ݽṹ
struct HWORKTREEDATA
{
	BYTE		byType;		// �ڵ�����
	BYTE		byState;	// �ڵ�״̬
	int			nID;		// �ڵ�ID
	CMenu*		pMenu;		// �ڵ�˵�
	CObject*	pObject;	// �ڵ����
	HTREEITEM	hItem;		// �ڵ�Item
	CString		strInfo;	// �ڵ���Ϣ
	CString		strDClick;	// �ڵ�˫���ű�
};

class CWorkTreeCtrl : public CXTTreeCtrl
{
// Construction
public:
	CWorkTreeCtrl();

// Attributes
public:
	CImageList		m_ImageList;	// �ڵ�ͼ������
	CPtrArray		m_aMenus;		// �˵�����
	CMenuItemArray	m_aMenuItems;	// �˵�������
	CPtrArray		m_aDatas;		// �ڵ���������
	HTREEITEM		m_hCurTreeItem;	// ��ǰ���������ڵ�
	int				m_nImageCount;	// �ڵ�ͼ����
	int				m_nTreeType;	// ������
	int				m_nWsNo;		// �����б��
	int				m_nTreeID;		// ��ID;
	int				m_nMenuItemID;	// �˵���ID
	CString			m_strBmpFile;	// ͼƬ�ļ���
	CString			m_strDClickScript;// ȱʡ˫������ʱ��ִ�еĽű�
	CString			m_strCurDClickScript;// ��ǰ�ڵ�˫���ű����ݴ����

// Operations
public:
	BOOL		IsWsInit();				// �����б��Ƿ��ʼ��
	void		SetWsInit(BOOL bInit);	// ���������б��ʼ����־

	void		InitTree();			// ��ʼ����
	void		SetTreeImageList(CString strBmpFile);

	// ���˵�
	int			AddTreeMenu(CMenu* pMenu);	// ��Ӳ˵�
	CMenu*		GetTreeMenu(int nIndex);	// ȡ��ĳ���˵�

	// �˵������
	int			AddMenuItem(BYTE byType, UINT nID, CString strItem, CString strCommand, TYPEOF_Proc* Proc, TYPEOF_UpdateProc* UpdateProc=NULL);
	BOOL		GetMenuItem(BYTE byType, UINT nID, CString& strItem, CString& strCommand, TYPEOF_Proc** pProc, TYPEOF_UpdateProc** pUpdateProc);

	HTREEITEM	AddTreeItem(HTREEITEM hParent, CString strText, 
				int iImage = 0, HTREEITEM hInsAfter = TVI_LAST);
	BOOL		DelTreeItem(HTREEITEM hItem, BOOL bChildOnly);
	BOOL		DelTreeItemData(HTREEITEM hItem);

	// �ڵ����ݲ���
	void		SetItemType(HTREEITEM hItem, BYTE byType);	// ���ýڵ�����
	BYTE		GetItemType(HTREEITEM hItem);	// ȡ�ýڵ�����
	void		SetItemState1(HTREEITEM hItem, BYTE byState);	// ���ýڵ�״̬
	void		TraversalSetItemState(HTREEITEM hItem, BYTE byState, BOOL bFreshAll = TRUE);	// �������ýڵ�״̬
	void		UpTraversalSetItemState(HTREEITEM hItem, BYTE byState);	// ���ϱ������ýڵ�״̬
	BYTE		GetItemState1(HTREEITEM hItem);	// ȡ�ýڵ�״̬
	void		SetItemMenu(HTREEITEM hItem, CMenu* pMenu);	// ���ýڵ�˵�
	CMenu*		GetItemMenu(HTREEITEM hItem);	// ȡ�ýڵ�˵�
	void		SetItemObject(HTREEITEM hItem, CObject* pObject);	// ���ýڵ����
	CObject*	GetItemObject(HTREEITEM hItem);	// ȡ�ýڵ����
	void		SetItemID(HTREEITEM hItem, int nID);	// ���ýڵ�ID
	int			GetItemID(HTREEITEM hItem);		// ȡ�ýڵ�ID
	void		SetItemImage1(HTREEITEM hItem, int nImage);	// ���ýڵ�ͼ��
	int			GetItemImage1(HTREEITEM hItem);		// ȡ�ýڵ�ͼ��
	void		SetItemInfo(HTREEITEM hItem, CString strInfo);	// ���ýڵ���Ϣ
	CString		GetItemInfo(HTREEITEM hItem);	// ��ȡ�ڵ���Ϣ
	void		SetItemDClick(HTREEITEM hItem, CString strDClick);	// ���ýڵ�˫���ű�
	CString		GetItemDClick(HTREEITEM hItem);	// ȡ�ýڵ�˫���ű�

	// �����б����
	int			GetWsID();						// ��ȡ�����б�ID

	// ��ѯ����
	HTREEITEM	GetItemFromID(int nID);
	HTREEITEM	GetItemFromInfo(CString strInfo);

	// �����������
	BOOL		EnableTestInit();		// ��ʼ������
	BOOL		EnableTestStart();
	BOOL		EnableTestStop();
	BOOL		EnableTestPause();
	BOOL		EnableTestContinue();
	BOOL		EnableTestDisable();
	BOOL		EnableTestFresh();
	BOOL		EnableTestProperty();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkTreeCtrl)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTestInit();
	afx_msg void OnTestStart();
	afx_msg void OnTestStop();
	afx_msg void OnTestPause();
	afx_msg void OnTestContinue();
	afx_msg void OnTestDisable();
	afx_msg void OnTestFresh();
	afx_msg void OnTestProperty();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRsmProperty();
	afx_msg void OnUpdateRsmProperty(CCmdUI* pCmdUI);
	afx_msg void OnRsmRefresh();
	afx_msg void OnUpdateRsmRefresh(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnWorkTreeCmd(UINT nID);
	afx_msg void OnUpdateWorkTreeCmd(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKTREECTRL_H__D8058500_582D_11D4_BEF6_000021605F53__INCLUDED_)

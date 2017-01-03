#ifndef __WORKSPACEVIEW_H__
#define __WORKSPACEVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkSpaceView.h : header file
//

#include "WorkTreeCtrl.h"
#include "WorkTaskPanel.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceView view
class CWorkSpaceDoc;
class CWorkSpaceView : public CView
{
	DECLARE_DYNCREATE(CWorkSpaceView)

// Attributes
public:
	CWorkTreeCtrl		m_TreeCtrl;		// ���ؼ�
	CWorkTaskPanel		m_TaskPanel;	// ����Panel
	TYPEOF_ViewChange*	m_pViewChange;	// ҳ��ı�ص�����
	int					m_nWsNo;		// �����б��
	BOOL				m_bTreeInited;	// ���ؼ��Ƿ��Ѿ���ʼ��
	int					m_nPageType;	// ҳ������

	CWorkSpaceView();
	virtual ~CWorkSpaceView();

	CWorkSpaceDoc* GetDocument();

// Operations
public:
	// WorkTree
	BOOL SetTreeType(int nTreeType);
	void InitTreeCtrl();
	void InitTaskPanel();
	void SetTreeImageList(CString strBmpFile);
	void InstallVCCallBack(TYPEOF_ViewChange* pViewChange);
	int  AddTreeMenu(CMenu* pMenu);
	int  AddMenuItem(HWORKMENUITEM* pItem);
	HTREEITEM AddTreeItem(HWORKTREEITEM* pItem);
	BOOL DelTreeItem(HWORKTREEITEM* pItem);
	void SetDClickScript(CString strScript);

	// TaskPanel
	CXTPTaskPanelGroupItem* AddTaskItem(HWORKTASKITEM* pItem);
	void DelTaskItem(HWORKTASKITEM* pItem);
	int GetTaskItemIDByItem(CXTPTaskPanelItem* pTaskItem);
	int GetTaskItemIDByCaption(CString strCaption, int nType);
	CXTPTaskPanelGroupItem* GetTaskItemByID(int nID);
	void CreateTaskMenu(CMenu* pMenu);
	void SetTaskPanelTheme(XTPTaskPanelPaintTheme taskPanelTheme);
	int SetTaskPanelIcon(CString strIcon);

	// �ڵ����ݲ���
	void		SetItemType(HWORKTREEITEM* pItem);	// ���ýڵ�����
	BYTE		GetItemType(HWORKTREEITEM* pItem);	// ȡ�ýڵ�����
	void		SetItemState1(HWORKTREEITEM* pItem);	// ���ýڵ�״̬
	BYTE		GetItemState1(HWORKTREEITEM* pItem);	// ȡ�ýڵ�״̬
	void		SetItemMenu(HWORKTREEITEM* pItem);	// ���ýڵ�˵�
	CMenu*		GetItemMenu(HWORKTREEITEM* pItem);	// ȡ�ýڵ�˵�
	void		SetItemObject(HWORKTREEITEM* pItem);// ���ýڵ����
	CObject*	GetItemObject(HWORKTREEITEM* pItem);// ȡ�ýڵ����
	void		SetItemID(HWORKTREEITEM* pItem);	// ���ýڵ�ID
	int			GetItemID(HWORKTREEITEM* pItem);	// ȡ�ýڵ�ID
	void		SetItemName(HWORKTREEITEM* pItem);	// ���ýڵ���
	CString		GetItemName(HWORKTREEITEM* pItem);	// ȡ�ýڵ���
	void		SetItemImage(HWORKTREEITEM* pItem);	// ���ýڵ�ͼ��
	int			GetItemImage(HWORKTREEITEM* pItem);	// ȡ�ýڵ�ͼ��
	void		ExpandItem(HWORKTREEITEM* pItem);	// ��չ�ڵ�
	void		CollapseItem(HWORKTREEITEM* pItem);	// �����ڵ�
	void		EnsureItem(HWORKTREEITEM* pItem);	// ת��ĳ����Ŀ
	BOOL		SetItemInfo(HWORKTREEITEM* pItem);	// �������ڵ���Ϣ
	BOOL		GetItemInfo(HWORKTREEITEM* pItem);	// ��ȡ���ڵ���Ϣ
	HTREEITEM	SearchItem(HWORKTREEITEM* pItem);	// ���ҽڵ�
	HTREEITEM	GetChildItem(HWORKTREEITEM* pItem);	// ��ȡ��һ���ӽڵ�
	HTREEITEM	GetNextItem(HWORKTREEITEM* pItem);	// ��ȡ��һ���ڵ�
	BOOL		SortChildren(HWORKTREEITEM* pItem);	// ���ӽڵ�����

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceView)
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WorkSpaceView.cpp
inline CWorkSpaceDoc* CWorkSpaceView::GetDocument()
   { return (CWorkSpaceDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__WORKSPACEVIEW_H__)

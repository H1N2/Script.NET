#if !defined(AFX_TREEOUTCTRL_H__896BF40D_111A_4C27_8BED_6FFB9CB83D7C__INCLUDED_)
#define AFX_TREEOUTCTRL_H__896BF40D_111A_4C27_8BED_6FFB9CB83D7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeOutCtrl.h : header file
//
#include "ReportCtrl\ReportCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeOutCtrl window

class CTreeOutCtrl : public CReportCtrl
{
// Construction
public:
	CTreeOutCtrl();

// Attributes
public:
	CImageList m_ilTree;
	CString m_strSeparator;	// ��Ŀ�ָ��
	CString m_strScriptClose;	// ���ڹر�ʱִ�еĽű�
	CStringArray m_asItemScript; // ��Ŀ�ű�����
	CStringArray m_asExMenu;	// ��չ�˵�
	CStringArray m_asExMenuScript;	// ��չ�˵��ű�
	HTREEITEM m_hLastItem;	// ���һ����ӵ����ڵ���

// Operations
public:
	void SetParentBar(CWnd* pWnd) { m_pParentBar = pWnd; }

	void Init();
	void DoCloseScript();
	int  ParseStyle(CString strStyle);
	void ParseLines(CString strSet, CStringArray& aLines);
	void ParseALine(CString strLine);
	void SetCtrlFormat(CString strSet);
	void Output(CString strOutput);
	void vsWrite(LPCTSTR lpszFormat, va_list argList);
	CString GetInfo(CString strParam);
	void AddTreeItems(CTTreeItemArray*& pItems);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeOutCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeOutCtrl();

	// Generated message map functions
protected:
	CWnd*		m_pParentBar;	// ��¼��Bar����
	//{{AFX_MSG(CTreeOutCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOeditClear();
	afx_msg void OnOutctrlDelrow();
	afx_msg void OnOutctrlScript();
	afx_msg void OnUpdateOutctrlScript(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOeditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutctrlDelrow(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEOUTCTRL_H__896BF40D_111A_4C27_8BED_6FFB9CB83D7C__INCLUDED_)

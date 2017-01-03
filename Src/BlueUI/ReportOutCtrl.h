#if !defined(AFX_TREEOUTCTRL_H__91998913_22CF_479B_92F3_04C24269D1BE__INCLUDED_)
#define AFX_TREEOUTCTRL_H__91998913_22CF_479B_92F3_04C24269D1BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeOutCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportOutCtrl window

class CReportOutCtrl : public CXTPReportControl
{
// Construction
public:
	CReportOutCtrl();

// Attributes
public:
	CString m_strSeparator;	// ��Ŀ�ָ��
	CString m_strScriptClose;	// ���ڹر�ʱִ�еĽű�
	CStringArray m_asItemScript; // ��Ŀ�ű�����
	CStringArray m_asExMenu;	// ��չ�˵�
	CStringArray m_asExMenuScript;	// ��չ�˵��ű�
	CArray<COLORREF, COLORREF&>	m_aColorTable;	// ��ɫ��
	BOOL		m_bSetColor;
	COLORREF	m_clText;
	COLORREF	m_clBkGnd;
	CXTPReportRecord* m_pLastRecord;	// ���һ����ӵļ�¼

	CImageList m_ilIcons;		// ͼ��

// Operations
public:

	void AppendColor(COLORREF color);
	void SetColor(int nIndex, COLORREF color);
	COLORREF GetColor(int nIndex);

	void Clear();
	void DoCloseScript();
	XTPReportGridStyle  ParseStyle(CString strStyle);
	void ParseLines(CString strSet, CStringArray& aLines);
	void ParseALine(CString strLine);
	void SetCtrlFormat(CString strSet);
	void Output(CString strOutput);
	void vsWrite(LPCTSTR lpszFormat, va_list argList);
	CString GetInfo(CString strParam);
	CXTPReportRecord* GetItemRecord(CString strItem);
	void AddTreeItems(CTTreeItemArray*& pItems);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportOutCtrl)
	public:
	//}}AFX_VIRTUAL
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Implementation
public:
	virtual ~CReportOutCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReportOutCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
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

#endif // !defined(AFX_TREEOUTCTRL_H__91998913_22CF_479B_92F3_04C24269D1BE__INCLUDED_)

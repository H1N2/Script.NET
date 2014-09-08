#if !defined(AFX_NEWPRJPAGE_H__B99FAEF7_24EC_40DF_88E5_3E099C2F5D55__INCLUDED_)
#define AFX_NEWPRJPAGE_H__B99FAEF7_24EC_40DF_88E5_3E099C2F5D55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewPrjPage.h : header file
//

#include "kcsidebannerwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CNewPrjPage dialog

class CNewPrjPage : public CDialog
{
	//DECLARE_DYNCREATE(CNewPrjPage)

// Construction
public:
	CNewPrjPage(CWnd* pParent = NULL);
	~CNewPrjPage();

// Dialog Data
	//{{AFX_DATA(CNewPrjPage)
	enum { IDD = IDD_NEW_PROJECT };
	CListBox	m_listPrjType;
	CString	m_strPrjName;
	CString	m_strPrjDesc;
	CString	m_strAuthor;
	CString	m_strPrjPath;
	//}}AFX_DATA

	CKCSideBannerWnd  m_banner;

	int				m_nPrjType;		// ��������
	CString			m_strPrjType;	// ��������
	CString			m_strPrjVci;	// ���������
	CString			m_strPrjExt;	// ���������
	CString			m_strTemplate;	// ģ���ļ���Ŀ¼
	CString			m_strInterp;	// ������
	CString			m_strScript;	// ִ�еĽű�
	CUIntArray		m_auIcon;		// ͼ���б�
	CStringArray	m_asIcon;		// ͼ���б�(ͼ���ļ���)
	CStringArray	m_asTemplate;	// ģ���б�
	CStringArray	m_asType;		// ���������б�
	CStringArray	m_asPrjVci;		// ����VCI������б�
	CStringArray	m_asExt;		// �����ļ���׺�б�
	CStringArray	m_asInterp;		// �ű��������б�
	CStringArray	m_asScript;		// �ű��б�(ѡ��һ�ֹ���֮��ִ�еĽű�)

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewPrjPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewPrjPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS);
	afx_msg void OnButtonPrjpath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPRJPAGE_H__B99FAEF7_24EC_40DF_88E5_3E099C2F5D55__INCLUDED_)

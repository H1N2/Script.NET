#if !defined(AFX_NEWFILEPAGE_H__4E7E8A47_A92A_44A8_8685_326582C85FDF__INCLUDED_)
#define AFX_NEWFILEPAGE_H__4E7E8A47_A92A_44A8_8685_326582C85FDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewFilePage.h : header file
//

#include "kcsidebannerwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CNewFilePage dialog

class CNewFilePage : public CDialog
{
	//DECLARE_DYNCREATE(CNewFilePage)

// Construction
public:
	CNewFilePage(CWnd* pParent = NULL);
	~CNewFilePage();

// Dialog Data
	//{{AFX_DATA(CNewFilePage)
	enum { IDD = IDD_NEW_FILE };
	CListBox	m_listFileType;
	BOOL	m_bAddToProject;
	CString	m_strFileName;
	CString	m_strFilePath;
	CString	m_strAuthor;
	CString	m_strFileDesc;
	//}}AFX_DATA

	CKCSideBannerWnd  m_banner;

	int				m_nFileType;	// �ļ�����
	CString			m_strFileType;	// �ļ�����
	CString			m_strTemplate;	// ģ���ļ���Ŀ¼
	CString			m_strInterp;	// ������
	CString			m_strScript;	// ִ�еĽű�
	CUIntArray		m_auIcon;		// ͼ���б�
	CStringArray	m_asIcon;		// ͼ���б�(ͼ���ļ���)
	CStringArray	m_asTemplate;	// ģ���б�
	CStringArray	m_asType;		// �ļ������б�
	CStringArray	m_asInterp;		// �ű��������б�
	CStringArray	m_asScript;		// �ű��б�(ѡ��һ�ֹ���֮��ִ�еĽű�)
	CStringArray	m_asExt;		// ��չ���б�

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewFilePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewFilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS);
	afx_msg void OnButtonFilepath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWFILEPAGE_H__4E7E8A47_A92A_44A8_8685_326582C85FDF__INCLUDED_)

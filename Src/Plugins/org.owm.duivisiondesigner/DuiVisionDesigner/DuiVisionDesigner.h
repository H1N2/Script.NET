// DuiVisionDesigner.h : main header file for the DuiVisionDesigner DLL
//

#ifndef __DuiVisionDesigner_H__
#define __DuiVisionDesigner_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDuiVisionDesignerApp
// See DuiVisionDesigner.cpp for the implementation of this class
//

class CDuiVisionDesignerView;
class CDuiVisionDesignerApp : public CWinApp
{
public:
	CDuiVisionDesignerApp();

	CString GetPlatRootPath();
	CString GetModulePath();

	// ���������
	CString StripMnemonics(CString str);
	void CreateToolboxPane();
	void ActiveToolboxPane();
	CXTPTaskPanelGroup* CreateToolboxGroup(UINT nID, LPCTSTR lpszCaption);
	CXTPTaskPanelGroup* FindToolboxGroup(UINT nID);
	void SetToolboxDefaultItem();
	BOOL SetToolboxIcons(LPCTSTR lpszBmpFile, CUIntArray* paIDs);
	BOOL SetToolboxIcon(CString strIcon, int nId);

	CString	GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = "");
	int		GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0);
	CString WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	int		WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDuiVisionDesignerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDuiVisionDesignerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	OWM_LINKAGE	m_ol;		// OWM�ӿڵĿ���
	BOOL		m_bDebug;	// ���Կ���
	CMultiDocTemplate*	m_pDocTemplate;	// �ĵ�ģ��
	int			m_nLanguage;// ��������
	IPlatUI*	m_pIPlatUI;	// ƽ̨�ӿ�ָ��
	HINSTANCE	m_hDuiPluginHandle;		// �����������̬��ľ��

	CXTPTaskPanel*			m_pToolboxPane;	// ������Pane
	CXTPTaskPanelGroup*		m_pCurToolboxGroup;	// ��ǰ������
	CXTPTaskPanelGroupItem*	m_pToolboxItemPointer;	// �������Ĭ����
};

class CDuiVisionDesignerView;
CDuiVisionDesignerView* GetFirstDuiVisionDesignerView();

extern class CDuiVisionDesignerApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__DuiVisionDesigner_H__)

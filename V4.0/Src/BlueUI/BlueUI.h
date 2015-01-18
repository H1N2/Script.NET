// BlueUI.h : main header file for the BLUEUI application
//

#if !defined(AFX_BLUEUI_H__A5581E03_3755_43AC_87E9_39300BF16DFF__INCLUDED_)
#define AFX_BLUEUI_H__A5581E03_3755_43AC_87E9_39300BF16DFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "PlugIn.h"
#include "Tools.h"
/////////////////////////////////////////////////////////////////////////////
// CBlueUIApp:
// See BlueUI.cpp for the implementation of this class
//

class CBlueUIApp : public CWinApp
{
public:
	CBlueUIApp();

	BOOL	InterprocessCommunication();	// ���̼�ͨ��

	CString GetPlatRootPath();
	void	SaveAndSetCurPath(CString strURL);
	void	RestoreCurPath();
	IPlatUI* GetIPlatUI();
	CString GetConfText(CString strPath);
	BOOL	LoadPlugIns();
	CPlugIn* FindPlugIn(CString strVciId);
	int		AutoLoadPlugIns(CString strPlugInPath);
	BOOL	ShellCommand(CString strCommand, CStringArray& asFile);
	BOOL	ShellOpenFile(CString strFileName);
	CDocument* OpenDocumentFileWithOwm(LPCTSTR lpszFileName, LPCTSTR lpszOwmId);
	CDocument* OpenHtmlWindow(CString strUrl, BOOL bActivate = TRUE, BOOL bReplaceBlank = FALSE);
	void	ProcessPath(CString& strPath, CString strPlugInId = "");
	CString	ExecMenuScript(CString strMenuName);
	BOOL	CommLoginDialog(void* p);	// ͨ�õ�¼�Ի���
	void	ShowPopupControl(TPopupControlInfo* pInfo);	// ��ʾPopup����
	BOOL	RunPopupControlLink(CString strLink);
	BOOL	GetPopupControlState();
	void	ClosePopupControl();
	void	ShowAboutBox(CString strPluginId);
	
	// TaskDialog����
	int		DoTaskDialog(void* p);		// ͨ������Ի���
	int		DoXmlTaskDialog(void* p);	// ͨ������Ի���(ͨ��XML����)
	int		RefreshTaskDialog(void* p);	// ˢ��ͨ������Ի���
	void	ClearTaskDialogPage();		// ɾ��������ҳ��
	int		AddTaskDialogPage(void* p);	// �����ҳ��
	int		DoTaskDialogSheets();		// ִ����

	// OWM����
	CPlugIn* GetOWM(UINT nID);
	CPlugIn* GetOWM(CString strOwmId);
	int		 GetOWMID(CString strOwmId);
	CPlugIn* GetDynamicOWM(UINT nID);
	CPlugIn* GetDynamicOWM(CString strOwmId);
	BOOL	 CheckActiveOWM(CString strOwmId, CString strDocumentFile);
	CDocument* GetOwmDocumentFromFrame(CWnd* pFrame, CPlugIn*& pPlugIn);

	// VCI����
	void*	CreateVciObject(CString strVciId, CString strInstName = "");
	BOOL	ReleaseObject(LPVOID pVciObj, BOOL bForceDelete = FALSE);
	BOOL	ReleaseObjectByInstanceName(CString strInstName);
	void*	GetObjectByInstanceName(CString strInstName);
	void*	GetObjectControlPtrByInstancePtr(LPVOID pVciObj);
	CString GetPlugInIdByInstancePtr(LPVOID pVciObj);

	// ��ʱ���������
	CTimerTask* AddTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds=1, BOOL bStart=TRUE);
	CTimerTask* AddTimerTask(CString strTaskName, LPVOID lpFunction, int nSeconds=1, BOOL bStart=TRUE);
	CTimerTask* FindTimerTask(CString strTaskName);
	BOOL	DeleteTimerTask(CTimerTask* pTimerTask);
	BOOL	DeleteTimerTask(CString strTaskName);
	BOOL	RunTimerTask();

	// ���߲���
	int		AutoLoadTools(CString strToolPath);
	void	CreateLoadToolsThread();

	// ��չģ�������
	void	PlatComCmd(WPARAM wParam, LPARAM lParam);
	int		CommandPlat(int nCmd, WPARAM wParam, LPARAM lParam);

	// ������֧��
	void	ReloadDefaultMenu();
	void	SetRTLLayout(BOOL bRTLLayout);
	BOOL	SetLocale(LCID Locale, const char* szLocale, BOOL bIsRTL, BOOL bIsTextFile = FALSE);

	// ���̲���
	BOOL	IsProjectOpen();
	void	SaveRecentProject(CString strProjectFile);
	void	RemoveRecentProject(CString strProjectFile);
	void	ExecProjectScript(CString strMenuAction);
	BOOL	OpenProjectFile(CPlugIn* pPlugIn, CString strProjectFile);

public:
	UINT			m_curLanguage;			// ��ǰ����
	BOOL			m_bIsRTL;

	CString			m_strProcessId;			// ��ǰ���̱�ʶ

	CPlatUI			m_PlatUI;				// ƽ̨UI�ӿڶ���
	CPlugInArray	m_aPlugIns;				// ����б�
	CToolArray		m_aTools;				// �����б�
	CXmlParser		m_xmlPlat;				// ƽ̨�����ļ�
	CString			m_strOldPath;			// ���浱ǰ·����Ϣ
	CString			m_strDefaultEditor;		// ȱʡ�ı༭��ģ����
	CList<CTimerTask*, CTimerTask*> m_lsTimerTask; // ��ʱ���������
	CMutex			m_MutexTimerTask;		// ��ʱ��������Ļ������

	CStringArray	m_asProject;			// �򿪵Ĺ����б�
	IProjectManager* m_pIProjectManager;	// ���̹������ӿ�ָ��
	IProject*		m_pIProject;			// ���̲���ӿ�ָ��
	IMessageQueue*	m_pIMessageQueue;		// ��Ϣ���в���ӿ�ָ��

	CXTPPopupControl	m_wndPopupControl;	// popup window object
	TPopupControlInfo	m_infoPopupControl;	// popup control info

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlueUIApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBlueUIApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileMruFile1(CCmdUI* pCmdUI);
	afx_msg void OnOption();
	afx_msg void OnUpdateOption(CCmdUI* pCmdUI);
	afx_msg void OnVciManager();
	afx_msg void OnUpdateVciManager(CCmdUI* pCmdUI);
	afx_msg void OnProjectNewfile();
	afx_msg void OnUpdateProjectNewfile(CCmdUI* pCmdUI);
	afx_msg void OnHelpTopics();
	afx_msg void OnUpdateHelpTopics(CCmdUI* pCmdUI);
	afx_msg void OnHelpRegisterate();
	afx_msg void OnUpdateHelpRegisterate(CCmdUI* pCmdUI);
	afx_msg void OnHelpUpgrade();
	afx_msg void OnUpdateHelpUpgrade(CCmdUI* pCmdUI);
	afx_msg void OnHelpHomepage();
	afx_msg void OnUpdateHelpHomepage(CCmdUI* pCmdUI);
	afx_msg void OnHelpBugreport();
	afx_msg void OnUpdateHelpBugreport(CCmdUI* pCmdUI);
	afx_msg void OnOpenWith();
	afx_msg void OnUpdateOpenWith(CCmdUI* pCmdUI);
	afx_msg void OnToolsExtend();
	afx_msg void OnUpdateToolsExtend(CCmdUI* pCmdUI);
	afx_msg void OnHelpDownload();
	afx_msg void OnUpdateHelpDownload(CCmdUI* pCmdUI);
	afx_msg void OnProjectNew();
	afx_msg void OnUpdateProjectNew(CCmdUI* pCmdUI);
	afx_msg void OnProjectOpen();
	afx_msg void OnUpdateProjectOpen(CCmdUI* pCmdUI);
	afx_msg void OnProjectSave();
	afx_msg void OnUpdateProjectSave(CCmdUI* pCmdUI);
	afx_msg void OnProjectClose();
	afx_msg void OnUpdateProjectClose(CCmdUI* pCmdUI);
	afx_msg void OnProjectBuild();
	afx_msg void OnUpdateProjectBuild(CCmdUI* pCmdUI);
	afx_msg void OnProjectRebuild();
	afx_msg void OnUpdateProjectRebuild(CCmdUI* pCmdUI);
	afx_msg void OnProjectClean();
	afx_msg void OnUpdateProjectClean(CCmdUI* pCmdUI);
	afx_msg void OnProjectInfo();
	afx_msg void OnUpdateProjectInfo(CCmdUI* pCmdUI);
	afx_msg void OnProjectProperty();
	afx_msg void OnUpdateProjectProperty(CCmdUI* pCmdUI);
	afx_msg void OnProjectRun();
	afx_msg void OnUpdateProjectRun(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadsession();
	afx_msg void OnUpdateFileLoadsession(CCmdUI* pCmdUI);
	afx_msg void OnFileSavesession();
	afx_msg void OnUpdateFileSavesession(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPagesetup();
	afx_msg void OnUpdateFilePrintPagesetup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern class CBlueUIApp theApp;
/////////////////////////////////////////////////////////////////////////////

inline BOOL PathFileExists(CString str)
{
	CFileStatus fStatus;
	
	if (! CFile::GetStatus(str, fStatus))
		return FALSE;

	return TRUE;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLUEUI_H__A5581E03_3755_43AC_87E9_39300BF16DFF__INCLUDED_)

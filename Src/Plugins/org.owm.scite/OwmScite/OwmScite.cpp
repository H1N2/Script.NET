// OwmScite.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "OwmScite.h"
#include "ChildFrame.h"
#include "OwmSciteView.h"
#include "OwmSciteDoc.h"
#include "TpsGeneral.h"
#include "TpsNewDocument.h"
#include "TpsStyle.h"
#include "TpsPrintPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// COwmSciteApp

BEGIN_MESSAGE_MAP(COwmSciteApp, CWinApp)
	//{{AFX_MSG_MAP(COwmSciteApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COwmSciteApp construction

COwmSciteApp::COwmSciteApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDebug	= FALSE;
	m_pDocTemplate = NULL;
	m_pIPlatUI = NULL;
	m_bScriptDebuging = FALSE;

	m_hScintillaDll = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ƽ̨����(�ַ���)
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	struct INIUNIT
	{
		CString strEntry;
		CString strRet;
		int nRet;
	};
	INIUNIT IniUnit;
	IniUnit.strEntry = lpszEntry;
	IniUnit.strRet = lpszDefault;
	m_ol.CommandPlat(OC_GETINISTRING, (WPARAM)lpszSection, (LPARAM)(&IniUnit));
	return IniUnit.strRet;
}

/////////////////////////////////////////////////////////////////////////////
// ��ƽ̨����(����)
/////////////////////////////////////////////////////////////////////////////
int COwmSciteApp::GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	struct INIUNIT
	{
		CString strEntry;
		CString strRet;
		int nRet;
	};
	INIUNIT IniUnit;
	IniUnit.strEntry = lpszEntry;
	IniUnit.nRet = nDefault;
	return m_ol.CommandPlat(OC_GETINIINT, (WPARAM)lpszSection, (LPARAM)(&IniUnit));
}

/////////////////////////////////////////////////////////////////////////////
// дƽ̨����(�ַ���)
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	struct INIUNIT
	{
		CString strEntry;
		CString strRet;
		int nRet;
	};
	INIUNIT IniUnit;
	IniUnit.strEntry = lpszEntry;
	IniUnit.strRet = lpszValue;
	m_ol.CommandPlat(OC_SETINISTRING, (WPARAM)lpszSection, (LPARAM)(&IniUnit));
	return IniUnit.strRet;
}

/////////////////////////////////////////////////////////////////////////////
// дƽ̨����(����)
/////////////////////////////////////////////////////////////////////////////
int COwmSciteApp::WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	struct INIUNIT
	{
		CString strEntry;
		CString strRet;
		int nRet;
	};
	INIUNIT IniUnit;
	IniUnit.strEntry = lpszEntry;
	IniUnit.nRet = nValue;
	return m_ol.CommandPlat(OC_SETINIINT, (WPARAM)lpszSection, (LPARAM)(&IniUnit));
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨����·��
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::GetPlatRootPath()
{
	//��ȡƽ̨·��
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	//::GetModuleFileName(m_hInstance, szFullPath, MAX_PATH);
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);
	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	szPath = szPath.Left(szPath.GetLength() - 1);
	int nPos = szPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		szPath = szPath.Left(nPos+1);
	}

	return szPath;
}

/////////////////////////////////////////////////////////////////////////////
// ȡƽ̨LibĿ¼
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::GetPlatLibPath()
{
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);

	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	szPath = szPath.Left(szPath.GetLength() - 1);
	int nPos = szPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		szPath = szPath.Left(nPos+1);
	}

	szPath += _T("Lib\\");

	return szPath;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡģ������·��
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::GetModulePath()
{
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(AfxGetStaticModuleState()->m_hCurrentInstanceHandle, szFullPath, MAX_PATH);
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);
	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	szPath = szPath.Left(szPath.GetLength() - 1);

	return szPath;
}

/////////////////////////////////////////////////////////////////////////////
// ִ��ƽ̨�ļ�����ű�,���Դ�����
/////////////////////////////////////////////////////////////////////////////
BOOL COwmSciteApp::ExecFileMngScript(CString strParam)
{
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);

	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	szPath = szPath.Left(szPath.GetLength() - 1);
	int nPos = szPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		szPath = szPath.Left(nPos+1);
	}
	szPath += _T("Lib\\plat\\filemng.tcl");

	// ����������
	IInterp* pInterp = NULL;
	if(m_pIPlatUI)
	{
		pInterp = (IInterp*)(m_pIPlatUI->CreateVciObject("org.interp.tcl", ""));
	}
	if(pInterp == NULL)
	{
		return FALSE;
	}

	// ִ�нű�
	CString strCmdLine;
	strCmdLine.Format("\"%s\" %s", szPath, strParam);
	// �����ļ���Ҫ����б���滻
	strCmdLine.Replace('\\', '/');
	pInterp->RunScriptFile(strCmdLine);

	// �ͷŽ�����
	m_pIPlatUI->ReleaseVciObject(pInterp, FALSE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡִ�е�ǰ�ļ��Ľ�����ID
/////////////////////////////////////////////////////////////////////////////
CString COwmSciteApp::GetExecuteInterp(CString strDocId)
{
	if(!m_pIPlatUI)
	{
		return "";
	}

	CStringArray arExecuteModule;
	int nCount = m_pIPlatUI->GetVciDocExecuteModule("org.owm.edit", strDocId, "interp", arExecuteModule);

	if(nCount > 0)
	{
		return arExecuteModule[0];
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ˢ�����д���
/////////////////////////////////////////////////////////////////////////////
void COwmSciteApp::RefreshAllSciteView()
{
	if(!m_ol.pDocTemplate)
		return;

	POSITION posdoc = m_ol.pDocTemplate->GetFirstDocPosition();
	while(posdoc)
	{
		CDocument* pDoc = m_ol.pDocTemplate->GetNextDoc(posdoc);
		if(pDoc)
		{
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)
			{
				COwmSciteView* pView = (COwmSciteView*)(pDoc->GetNextView(posview));
				pView->m_SciEdit.defineDocType(pView->getLangType());
				pView->m_SciEdit.performGlobalStyles();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COwmSciteApp object

COwmSciteApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ��Դ�л�����
// get_dll_resource   �л���DLL����Դ
// reset_dll_resource �ָ���ǰ����Դ
/////////////////////////////////////////////////////////////////////////////
static int resource_counter = 0;		// ��Դ����
static HINSTANCE save_hInstance = NULL;	// ��Դ����

void get_dll_resource(void)
{
   /* this function changes the resource handle to that of the DLL */
   if (resource_counter == 0)
   {
	   AFX_MODULE_STATE* pModule= AfxGetStaticModuleState();
	   save_hInstance = AfxGetResourceHandle();
	   AfxSetResourceHandle(pModule->m_hCurrentResourceHandle);
   }

   resource_counter++;
}

void reset_dll_resource(void)
{
   /* this function restores the resource handle set by	'get_dll_resource()' */
   if (resource_counter > 0)
      resource_counter--;

   if (resource_counter == 0)
      AfxSetResourceHandle(save_hInstance);
}
/////////////////////////////////////////////////////////////////////////////

// �����߳��е���������
void SetLocateInThread()
{
	int nCurrentLanguage = AfxGetApp()->GetProfileInt(REG_VERSION_SUBKEY, REG_REG_LANGUAGE, ::GetSystemDefaultLangID());
	if(LANGUAGE_PAGE_CHINESE == nCurrentLanguage)
	{
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT));
	}else
	{
		::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	reset_dll_resource();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ����OWM
/////////////////////////////////////////////////////////////////////////////
int AboutOWM(void)
{
	get_dll_resource();
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


// ����OWM����
int RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc);
int UnregisterClientMenu(COWMMenuArray& aOWMMenus);
int RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex);
int UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart);
int RegisterOption(COWMOptionArray& aOWMOption);
int SetDebugMode(BOOL bDebug);
int CommandOWM(int nCmd, WPARAM wParam, LPARAM lParam);
int InitOWM();
int DoneOWM();

/////////////////////////////////////////////////////////////////////////////
// ��װOWMģ��
/////////////////////////////////////////////////////////////////////////////
BOOL InstallOWM(OWM_LINKAGE& ol)
{
	// ��ʼ��OWM�ṹ
	ol.nOWMType				= OWM_DOC | OWM_MENU | OWM_TOOLBAR | OWM_OPTION | OWM_DYNLOAD;
	ol.nOWMID				= 100;
	ol.pDocClass			= RUNTIME_CLASS(COwmSciteDoc);
	ol.pFrameClass			= RUNTIME_CLASS(CChildFrame);
	ol.pViewClass			= RUNTIME_CLASS(COwmSciteView);
	ol.pCtrlBarClass		= NULL;
	ol.SetOWM				= NULL;
	ol.InitOWM				= InitOWM;
	ol.DoneOWM				= DoneOWM;
	ol.RegisterTclCmd		= NULL;
	ol.UnregisterTclCmd		= NULL;
	ol.RegisterTclExKeyword	= NULL;
	ol.RegisterClientMenu	= RegisterClientMenu;
	ol.UnregisterClientMenu	= UnregisterClientMenu;
	ol.RegisterClientToolBar= RegisterClientToolBar;
	ol.UnregisterClientToolBar= UnregisterClientToolBar;
	ol.RegisterOption		= RegisterOption;
	ol.SetDebugMode			= SetDebugMode;
	ol.RunHelp				= NULL;
	ol.CommandOWM			= CommandOWM;
	ol.GetDllResource		= get_dll_resource;
	ol.ResetDllResource		= reset_dll_resource;

	// ��OWM�ṹ���������ر���
	memcpy(&(theApp.m_ol), &ol, sizeof(OWM_LINKAGE));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWMģ��
/////////////////////////////////////////////////////////////////////////////
void TerminateOWM(void)
{
	if(theApp.m_pDocTemplate)
	{
		delete theApp.m_pDocTemplate;
		theApp.m_pDocTemplate = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWM�汾
/////////////////////////////////////////////////////////////////////////////
BOOL OWMVersion(OWM_VERSION& ov)
{
	ov.svOWMName	= "OwmScite";
	ov.svDescription= "OwmScite OWM";
	ov.wVersionHi	= 1;
	ov.wVersionLo	= 0;
	ov.wMVersionHi	= 1;
	ov.wMVersionLo	= 0;
	ov.nLanguage	= 0;
	ov.bDebugVer	= FALSE;
#ifdef _DEBUG
	ov.bDebugVer	= TRUE;
#endif //_DEBUG

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���õ���ģʽ
/////////////////////////////////////////////////////////////////////////////
int SetDebugMode(BOOL bDebug)
{
	theApp.m_bDebug	= bDebug;

	return bDebug;
}

/////////////////////////////////////////////////////////////////////////////
// ȡ��һ��COwmSciteView
/////////////////////////////////////////////////////////////////////////////
COwmSciteView* GetFirstOwmSciteView()
{
	if(!theApp.m_ol.pDocTemplate)
		return NULL;

	POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
	if(posdoc)
	{
		CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
		POSITION posview = pDoc->GetFirstViewPosition();
		if(posview)
		{
			return (COwmSciteView*)(pDoc->GetNextView(posview));
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// �����ļ�·����������ͼ����ָ��
/////////////////////////////////////////////////////////////////////////////
COwmSciteView* GetSciteEditView(CString strFilePath, BOOL bActive)
{
	if(!theApp.m_ol.pDocTemplate)
		return NULL;

	strFilePath.Replace("/", "\\");
	strFilePath.Replace("\\.\\", "\\");	// �����õ�·��ȥ��

	POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
	while(posdoc)
	{
		CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
		CString strPathName = pDoc->GetPathName();
		strPathName.MakeUpper();
		strFilePath.MakeUpper();
		if(strFilePath == strPathName)
		{
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)
			{
				COwmSciteView* pView = (COwmSciteView*)(pDoc->GetNextView(posview));
				if(bActive)
				{
					theApp.m_ol.pDocTemplate->OpenDocumentFile(pDoc->GetPathName());
				}
				return pView;
			}
			return NULL;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ����ScintillaEditView�ؼ�ָ�룬������ͼ����ָ��
/////////////////////////////////////////////////////////////////////////////
COwmSciteView* GetSciteEditViewWithEditor(ScintillaEditView* pEditor)
{
	if(!theApp.m_ol.pDocTemplate)
		return NULL;

	POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
	while(posdoc)
	{
		CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
		if(pDoc)
		{
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)
			{
				COwmSciteView* pView = (COwmSciteView*)(pDoc->GetNextView(posview));
				if((&pView->m_SciEdit) == pEditor)
				{
					return pView;
				}
			}
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��ǰ�������ͼ
/////////////////////////////////////////////////////////////////////////////
COwmSciteView* GetActiveSciteEditView()
{
	if(!theApp.m_ol.pDocTemplate)
		return NULL;

	// ��ȡ��ǰ�����Frame
	CChildFrame* pFrame = (CChildFrame*)((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	if(pFrame)
	{
		CView* pActiveView = pFrame->GetActiveView();

		POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
		while(posdoc)
		{
			CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)
			{
				CView* pView = pDoc->GetNextView(posview);
				if(pView->GetParentFrame()->GetActiveView() == pActiveView)
				{
					return (COwmSciteView*)pView;
				}
			}
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// OWM�������
/////////////////////////////////////////////////////////////////////////////
int OWMCmdHelp(int nCmd, CStringArray* aHelpStr)
{
	switch(nCmd)
	{
	case OC_CMDALL:
		aHelpStr->Add("Help of OWM[OwmScite]:");
		aHelpStr->Add("ID      : 100");
		aHelpStr->Add("Auhtor  : blueant");
		aHelpStr->Add("Version : 1.0");
		aHelpStr->Add("Descript: OwmScite OWM");
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

// �������ñ�
//static struct FontItem g_FontItems[] =
//{
//	FontItem(FON_BK,					// ����
//			"���Ե�Ԫ����",				// ��ʾ����
//			"FTList.TestUnit",			// ע�������
//			"����", 16,					// ����
//			RGB(0,0,0), CLR_DEFAULT,	// ���ֺͱ���ɫ
//			FALSE, FALSE, FALSE, FALSE),// ���
//};

/////////////////////////////////////////////////////////////////////////////
// ��ע����ȡ���µ��������ñ���Ϣ
/////////////////////////////////////////////////////////////////////////////
void GetFontFromReg(FontItem* pFontItem)
{
	CString strSubKey;
	strSubKey.Format("%s\\%s", REG_FORMAT_SUBKEY, pFontItem->strSubKey);
	pFontItem->strFont		= AfxGetApp()->GetProfileString(strSubKey, REG_FMFONT, pFontItem->strFont);	// ������
	pFontItem->nSize		= AfxGetApp()->GetProfileInt(strSubKey, REG_FMSIZE, pFontItem->nSize);	// �����С
	pFontItem->colorFg		= AfxGetApp()->GetProfileInt(strSubKey, REG_FMCOLORFG, pFontItem->colorFg);	// ǰ��ɫ
	pFontItem->colorBk		= AfxGetApp()->GetProfileInt(strSubKey, REG_FMCOLORBK, pFontItem->colorBk);	// ����ɫ
	pFontItem->bBold		= AfxGetApp()->GetProfileInt(strSubKey, REG_FMBOLD, pFontItem->bBold);	// �Ƿ����
	pFontItem->bItalic		= AfxGetApp()->GetProfileInt(strSubKey, REG_FMITALIC, pFontItem->bItalic);	// �Ƿ�б��
	pFontItem->bUnderLine	= AfxGetApp()->GetProfileInt(strSubKey, REG_FMUNDERLINE, pFontItem->bUnderLine);	// �Ƿ��»���
	pFontItem->bStrikeOut	= AfxGetApp()->GetProfileInt(strSubKey, REG_FMSTRIKEOUT, pFontItem->bStrikeOut);	// �Ƿ�ɾ����
}

/////////////////////////////////////////////////////////////////////////////
// OWM�������
/////////////////////////////////////////////////////////////////////////////
int CommandOWM(int nCmd, WPARAM wParam, LPARAM lParam)
{
	switch(nCmd)
	{
	case OC_SETOWMLINK:	// ����OWM����(ϵͳ����,����Ķ�)
		{
			if(wParam == NULL)
				return FALSE;

			// ��OWM�ṹ���������ر���
			memcpy(&(theApp.m_ol), (OWM_LINKAGE*)wParam, sizeof(OWM_LINKAGE));

			return TRUE;
		}

	case OC_CMDHELP:	// �������
		{
			return OWMCmdHelp(wParam, (CStringArray*)lParam);
		}

	case OC_CLOSEVIEW:	// �ر�OWM����
		{
			COwmSciteView* pView = GetFirstOwmSciteView();
			if(!pView)
				return FALSE;
			CWnd* pFrame = pView->GetParent();
			if(!pFrame)
				return FALSE;
			pFrame->SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}

	case OC_OPENFILE:	// ���ĵ�
		{
			LPCTSTR lpszPathName = NULL;
			if(wParam != NULL)
				lpszPathName = (LPCTSTR)wParam;
			COwmSciteView* pView = GetSciteEditView(lpszPathName);
			if(pView)
			{
				// �Ѿ�����,�ͼ����ļ�,������ָ��
				pView->GetParentFrame()->ActivateFrame();
				return (int)(pView->GetDocument());
			}
			return (int)(theApp.m_ol.pDocTemplate->OpenDocumentFile(lpszPathName));
		}

	case OC_OPENFILEFIRST:	// �ڵ�һ��λ�ô��ĵ�
		{
			return FALSE;
		}

	case OC_SETREADONLY:	// �����ĵ���ֻ������
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->m_SciEdit.execute(SCI_SETREADONLY, lParam);
			return TRUE;
		}

	case OC_GETOPENFILE:	// ��ȡ�򿪵��ļ���
		{
			if((int)wParam == -1)	// ��ȡ���ļ���
			{
				if(!theApp.m_ol.pDocTemplate)
					return 0;
				int nCount = 0;
				POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
				while(posdoc)
				{
					theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
					nCount++;
				}
				return nCount;
			}else
			if((int)wParam == -2)	// ��ȡ��ǰ�򿪵��ļ���
			{
				if(!theApp.m_ol.pDocTemplate)
					return 0;
				// ��ȡ��ǰ�����Frame
				CChildFrame* pFrame = (CChildFrame*)((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
				if(pFrame)
				{
					CView* pActiveView = pFrame->GetActiveView();

					POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
					while(posdoc)
					{
						CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
						POSITION posview = pDoc->GetFirstViewPosition();
						if(posview)
						{
							CView* pView = pDoc->GetNextView(posview);
							if(pView->GetParentFrame()->GetActiveView() == pActiveView)
							{
								CString strPathName = pDoc->GetPathName();
								return (int)(LPCTSTR)strPathName;
							}
						}
					}
				}
				return (int)(LPCTSTR)"";
			}else
			if((int)wParam >= 0)	// ָ����ŵ��ļ�
			{
				if(!theApp.m_ol.pDocTemplate)
					return 0;
				int nCount = 0;
				POSITION posdoc = theApp.m_ol.pDocTemplate->GetFirstDocPosition();
				while(posdoc)
				{
					CDocument* pDoc = theApp.m_ol.pDocTemplate->GetNextDoc(posdoc);
					if((int)wParam == nCount)
					{
						POSITION posview = pDoc->GetFirstViewPosition();
						if(posview)
						{
							CString strPathName = pDoc->GetPathName();
							return (int)(LPCTSTR)strPathName;
						}else
						{
							return (int)(LPCTSTR)"";
						}
					}
					nCount++;
				}
				return (int)(LPCTSTR)"";
			}

			return FALSE;
		}

	case OC_SETEXECUTION:	// ���ô�����
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			return FALSE;
		}

	case OC_SETCURENTLINE:	// ���õ�ǰ��
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			int nLineNo = lParam-1;
			pView->m_SciEdit.gotoLine(nLineNo);
			pView->linemarkAdd(MARK_CURRENTLINE, nLineNo, true);
			return TRUE;
		}

	case OC_SETCURENTEXECLINE:	// ���õ�ǰִ����
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			int nLineNo = lParam-1;
			pView->m_SciEdit.gotoLine(nLineNo);
			pView->linemarkAdd(MARK_RUNCURRENT, nLineNo, true);
			return TRUE;
		}

	case OC_SEARCHLINE:	// ����ָ�����ݵ��в�����Ϊ��ǰ��
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->SearchLine((LPCTSTR)lParam, MARK_CURRENTLINE);
			return TRUE;
		}

	case OC_GETBREAKLIST:	// ��ȡ�ϵ��б�
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->linemarkGetAll(MARK_BREAKPOINT, (CUIntArray&)lParam);
			return TRUE;
		}

	case OC_SETSLAVEDEBUG:	// ���ôӵ���ģʽ(����ģʽ,���������Գ���)
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->SetSlaveDebugMode(lParam);
			return TRUE;
		}

	case OC_GET_LINECOUNT:	// ��ȡ������
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			return pView->m_SciEdit.execute(SCI_GETLINECOUNT);
		}

	case OC_GET_LINE:	// ��ȡ��
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			int nLineNo = lParam-1;
			static CString g_strLine_GetLine;			
			int nLineLength = pView->m_SciEdit.getLineLength(nLineNo);
			TCHAR* pszLine = new TCHAR[nLineLength+3];	// ����+3,��Ϊ�н������п���ռ������λ��
			memset(pszLine, 0, nLineLength+3);
			pView->m_SciEdit.getLine(nLineNo, pszLine, nLineLength);
			g_strLine_GetLine = pszLine;
			delete [] pszLine;

			return (int)(LPCTSTR)g_strLine_GetLine;
		}

	case OC_SET_LINE:	// ������
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			CString strLine = (LPCTSTR)lParam;
			int nPos = strLine.Find(">");
			if(nPos == -1)
			{
				return FALSE;
			}
			CString strLineNo = strLine.Left(nPos);
			strLine.Delete(0, nPos+1);
			if(strLineNo.Find("<") == 0)
			{
				strLineNo.Delete(0, 1);
			}
			int nLine = atoi(strLineNo);
			if(nLine > pView->m_SciEdit.execute(SCI_GETLINECOUNT))
			{
				return FALSE;
			}
			//pView->SetLine(nLine-1, strLine);
			return TRUE;
		}

	case OC_GET_SELECTLINE:	// ��ȡ��ǰѡ����з�Χ
		{/*
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			int nStartLine, nEndLine;

			CPoint ptStart;
			CPoint ptEnd;
			pView->public_GetSelection(ptStart, ptEnd);
			nStartLine = ptStart.y+1;
			nEndLine = ptEnd.y+1;
			if((ptEnd.x == 0) && (ptEnd.y > ptStart.y))
			{
				nEndLine--;
			}
			int nCount = nEndLine-nStartLine+1;

			int* lpEndLine = (int*)lParam;
			*lpEndLine = nStartLine;
			return nCount;*/
		}

	case OC_SET_BREAKLIST:	// ���öϵ��б�
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->linemarkClearAll(MARK_BREAKPOINT);
			CUIntArray* puIBreak = (CUIntArray*)lParam;
			if(puIBreak)
			{
				for(int i=0; i<puIBreak->GetSize(); i++)
				{
					pView->linemarkAdd(MARK_BREAKPOINT, puIBreak->GetAt(i));
				}
			}
			return TRUE;
		}

	case OC_CLEAR_BREAK:	// ��նϵ�
		{
			COwmSciteView* pView = GetSciteEditView((LPCTSTR)wParam);
			if(pView == NULL)
				return FALSE;
			pView->linemarkClearAll(MARK_BREAKPOINT);
			return TRUE;
		}

	case OC_EDIT_FIND:	// ����
		{
			COwmSciteView* pView = GetActiveSciteEditView();
			if(pView)
			{
				SearchOption* pOption = NULL;
				theApp.m_pIPlatUI->SendOwmCmd("org.owm.grep", OC_GREP_GETOPTION, 0, 0, &pOption);
				FindOption opt;
				if(pOption)
				{
					opt._isMatchCase = pOption->m_bMatchCase;
					opt._isWholeWord = pOption->m_bMatchWholeWords;
					opt._searchType = pOption->m_bRegularExpressions ? FindRegex : FindNormal;
					opt._whichDirection = DIR_DOWN;
					opt._isIncremental = false;
					opt._isWrapAround = true;
					opt._isInSelection = (pView->m_SciEdit.execute(SCI_GETSELECTIONSTART) != pView->m_SciEdit.execute(SCI_GETSELECTIONEND));
				}

				generic_string str2Search = pView->getTextFromGrep(OC_GREP_GETTEXTFIND);
				int stringSizeFind = str2Search.length();
				TCHAR *pTextFind = new TCHAR[stringSizeFind + 1];
				lstrcpy(pTextFind, str2Search.c_str());

				int nbTotal = 0;
				const bool isEntireDoc = true;
				CString strPathName = pView->GetDocument()->GetPathName();
				nbTotal += pView->processAll(ProcessFindAll, pTextFind, NULL, isEntireDoc, strPathName, pOption ? &opt : NULL);
				delete [] pTextFind;
			}
			return TRUE;
		}

	case OC_EDIT_REPLACE:	// �滻
		{
			COwmSciteView* pView = GetActiveSciteEditView();
			if(pView)
			{
				SearchOption* pOption = NULL;
				theApp.m_pIPlatUI->SendOwmCmd("org.owm.grep", OC_GREP_GETOPTION, 0, 0, &pOption);
				FindOption opt;
				if(pOption)
				{
					opt._isMatchCase = pOption->m_bMatchCase;
					opt._isWholeWord = pOption->m_bMatchWholeWords;
					opt._searchType = pOption->m_bRegularExpressions ? FindRegex : FindNormal;
					opt._whichDirection = DIR_DOWN;
					opt._isIncremental = false;
					opt._isWrapAround = true;
					opt._isInSelection = (pView->m_SciEdit.execute(SCI_GETSELECTIONSTART) != pView->m_SciEdit.execute(SCI_GETSELECTIONEND));
				}

				generic_string str2Search = pView->getTextFromGrep(OC_GREP_GETTEXTFIND);
				int stringSizeFind = str2Search.length();
				TCHAR *pTextFind = new TCHAR[stringSizeFind + 1];
				lstrcpy(pTextFind, str2Search.c_str());

				generic_string str2Replace = pView->getTextFromGrep(OC_GREP_GETTEXTREPLACE);
				int stringSizeReplace = str2Replace.length();
				TCHAR *pTextReplace = new TCHAR[stringSizeReplace + 1];
				lstrcpy(pTextReplace, str2Replace.c_str());

				int nbTotal = 0;
				const bool isEntireDoc = true;
				CString strPathName = pView->GetDocument()->GetPathName();
				nbTotal += pView->processAll(ProcessReplaceAll, pTextFind, pTextReplace, isEntireDoc, strPathName, pOption ? &opt : NULL);
				delete [] pTextFind;
				delete [] pTextReplace;
			}
			return TRUE;
		}

	case OC_WORD_HELP:	// ���ʰ���
		{
			COwmSciteView* pView = GetActiveSciteEditView();
			if(pView)
			{
				pView->SearchWordHelp((LPCTSTR)wParam);
			}
			return TRUE;
		}

	default:
		break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ƽ̨��ѯOWM��װ��Ϣ
/////////////////////////////////////////////////////////////////////////////
COWMArray* GetOwmInfo(int nId)
{
	if(!theApp.m_ol.CommandPlat)
		return NULL;

	return (COWMArray*)theApp.m_ol.CommandPlat(OC_QUERYOWMID, nId, 0);
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM��չ�˵�
/////////////////////////////////////////////////////////////////////////////
int RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc)
{
/*
	// ��ʼ��OWM�˵�,����ӵ����˵�
	get_dll_resource();
	CMenu* pmenu = new CMenu();
	pmenu->LoadMenu(IDR_OWM);
	CMenu* psubmenu = (CMenu*)(pmenu->GetSubMenu(0));
	reset_dll_resource();

	// ���˵��Ƿ��Ѿ�����,���������ɾ��
	int count = pMenu->GetMenuItemCount();
	OWM_CHECKMENUISEXIST(count-1, _T("OwmScite"))

	// �����˵���������չ�˵�
	pMenu->InsertMenu(count-1, MF_POPUP|MF_STRING|MF_ENABLED, 
				(UINT)psubmenu->GetSafeHmenu(), _T("OwmScite"));

	// �ڴ������չ�˵�
	OWM_REGISTERMENU(ID_OWM_ABOUT, AboutOWM, NULL, USER_NONE)

	// ��DOC�м�����չ�˵�ָ��,�ĵ��ر�ʱ���
	if(pDoc)
	{
		((COwmSciteDoc*)pDoc)->m_pMenu = pmenu;
	}
*/
	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM��չ�˵�����
/////////////////////////////////////////////////////////////////////////////
int UnregisterClientMenu(COWMMenuArray& aOWMMenus)
{
	// �ڴ�ע����չ�˵�����
	//OWM_UNREGISTERMENU(AboutOWM)

	return aOWMMenus.GetSize();
}


/////////////////////////////////////////////////////////////////////////////
// ��ʾ/����OWM����
/////////////////////////////////////////////////////////////////////////////
int OpenOwmView()
{
	if(GetFirstOwmSciteView())
		theApp.m_ol.CommandPlat(OC_CLOSEVIEW, theApp.m_ol.nOWMID, 0);
	else
		theApp.m_ol.CommandPlat(OC_OPENVIEW, theApp.m_ol.nOWMID, 0);
	return 0;
}

void UpdateOpenOwmView(CCmdUI* pCmdUI)
{
	int nCheck = GetFirstOwmSciteView() ? 1 : 0;
	pCmdUI->SetCheck(nCheck);
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM ToolBar
/////////////////////////////////////////////////////////////////////////////
int RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex)
{
	CToolBar* pToolBar = (CToolBar*)pBar;

	// ����ToolBar��Դ
	CToolBarCtrl* pCtrl = &(pToolBar->GetToolBarCtrl());
	CImageList* pImage = pCtrl->GetImageList();
	int count = pImage->GetImageCount();
	CBitmap bitmap;
	get_dll_resource();
	bitmap.LoadBitmap(IDR_OWM);
	pImage->Add(&bitmap, RGB(0, 128, 128));
	reset_dll_resource();

	// �ڴ����ToolBar
	OWM_REGISTERTOOLBAR(count, OpenOwmView, UpdateOpenOwmView, USER_NONE)

	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM ToolBar
/////////////////////////////////////////////////////////////////////////////
int UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{
	CToolBar* pToolBar = (CToolBar*)pBar;

	// �ڴ�ע��ToolBar
	OWM_UNREGISTERTOOLBAR(OpenOwmView)

	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM����ҳ
/////////////////////////////////////////////////////////////////////////////
int RegisterOption(COWMOptionArray& aOWMOption)
{
	// �ڴ��������ҳ
	DLL_LOADSTRING(strTpsEdit, IDS_PROP_STR_GENERAL);	// �༭��
	DLL_LOADSTRING(strTpsAboutEdit, IDS_PROP_STR_ABOUT_GENERAL);	// �༭����������...
	DLL_LOADSTRING(strTpsNewDoc, IDS_PROP_STR_NEWDOC);	// �½��ĵ�
	DLL_LOADSTRING(strTpsAboutNewDoc, IDS_PROP_STR_ABOUT_NEWDOC);	// �½��ĵ�����...
	DLL_LOADSTRING(strTpsStyle, IDS_PROP_STR_STYLE);	// ���Ը�ʽ
	DLL_LOADSTRING(strTpsAboutStyle, IDS_PROP_STR_ABOUT_STYLE);	// ���Ը�ʽ����...

	OWM_REGISTEROPTION("", strTpsEdit, strTpsAboutEdit, IDD_TPS_GENERAL, CTpsGeneral, 0)
	OWM_REGISTEROPTION(strTpsEdit, strTpsNewDoc, strTpsAboutNewDoc, IDD_TPS_NEWDOC, CTpsNewDocument, 0)
	OWM_REGISTEROPTION(strTpsEdit, strTpsStyle, strTpsAboutStyle, IDD_TPS_STYLE, CTpsStyle, 0)

	return aOWMOption.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ��OWM
/////////////////////////////////////////////////////////////////////////////
int InitOWM()
{
	// ע:�����������Ĵ���
/*	CString strSciLexerFile = theApp.GetModulePath();
	strSciLexerFile += "\\SciLexer.DLL";
	theApp.m_hScintillaDll = ::LoadLibrary(strSciLexerFile);
*/
	NppParameters *pNppParameters = NppParameters::getInstance();
	pNppParameters->load();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM
/////////////////////////////////////////////////////////////////////////////
int DoneOWM()
{
	// ע:�����������Ĵ���
	// �������ò���
	NppParameters *pNppParam = NppParameters::getInstance();
	(NppParameters::getInstance())->writeScintillaParams(pNppParam->getSVP(SCIV_PRIMARY), SCIV_PRIMARY);
	(NppParameters::getInstance())->writeScintillaParams(pNppParam->getSVP(SCIV_SECOND), SCIV_SECOND);
	(NppParameters::getInstance())->writeGUIParams();
	// �ͷ����ò���������ڴ�
	(NppParameters::getInstance())->destroyInstance();

	// �ͷ�UTF8-���ֽ�ת������̬�����з�����ڴ�
	WcharMbcsConvertor::destroyInstance();

	if(theApp.m_hScintillaDll)
	{
		//::FreeLibrary(theApp.m_hScintillaDll);
	}

	/*
	HINSTANCE hScintillaDll = gGetScintillaLibHandle();
	if(hScintillaDll)
	{
		::FreeLibrary(hScintillaDll);
	}*/

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// OWM VCI Interface

#include "vcicomm.h"
#include "vciowm.h"

#define INF_IVciOwm \
{\
	"IVciOwm",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"OwmScite",\
	"OwmScite OWM",\
	"blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IVciOwm = INF_IVciOwm;

class CVciOwm : public CVisualComponent
{
public:
	CVciOwm() {};
	~CVciOwm() {};

//////////////////////////////////////////////////////////////////////////
// �ṩ���ӿڵĹ��ܺ���
//////////////////////////////////////////////////////////////////////////
public:
	virtual int __stdcall SetDebugMode(BOOL bDebug);
	virtual int __stdcall SetLanguage(int nLanguage);
	virtual int __stdcall Init(LPCSTR lpcsParams);
	virtual int __stdcall Done(LPCSTR lpcsParams);
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut);

private:

//////////////////////////////////////////////////////////////////////////
// Ҫ����Ĺ��ܽӿ�
//////////////////////////////////////////////////////////////////////////
protected:
	// �ӿڶ���
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

	BEGIN_INTERFACE_PART(VciOwm, IVciOwm)
		OWM_INTERFACE_DEFINE
	END_INTERFACE_PART(VciOwm)
	EXPORT_INTERFACE_PART(VciOwm)
};

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(VciOwm)
CONTROL_INTERFACE_IMPLEMENT(VciOwm)

// ���õ���ģʽ
int __stdcall CVciOwm::SetDebugMode(BOOL bDebug)
{
	theApp.m_bDebug	= bDebug;
	return CVisualComponent::SetDebugMode(bDebug);
}

// ��������
int __stdcall CVciOwm::SetLanguage(int nLanguage)
{
	theApp.m_nLanguage = nLanguage;
	return CVisualComponent::SetLanguage(nLanguage);
}

// ��ʼ�����
int __stdcall CVciOwm::Init(LPCSTR lpcsParams)
{
	return InitOWM();
}

// �������
int __stdcall CVciOwm::Done(LPCSTR lpcsParams)
{
	return DoneOWM();
}

// ��Ϣ����
int __stdcall CVciOwm::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	CVciMessage* pMsg = (CVciMessage*)pIn;
	if(pMsg->GetCmdCode() == VMC_SENDOWMCMD)
	{
		CString* pStrFilePath = (CString*)(pMsg->GetHandle());
		COwmSciteView* pSciteView = GetSciteEditView(*pStrFilePath, FALSE);
		if(pSciteView)
		{
			int nOwmCmdCode, wParam, lParam;
			if(pMsg->GetOwmCmd(nOwmCmdCode, wParam, lParam))
			{
				return pSciteView->m_SciEdit.execute(nOwmCmdCode, wParam, lParam);
			}
		}
	}

	return 0;
}

// ��ѯ�����Ƿ�֧��
extern "C" __declspec(dllexport)
int gQueryCommandIsSupport(LPCSTR lpcsCmd)
{
	//if(strcmp(lpcsCmd, "HELLO") == 0)
	//{
	//}
	return 0;
}

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CVciOwm::XVciOwm::GetExtraInfo(LPVOID lpRefData)
{
	CVciOwm *pObj = GET_INTERFACE_OBJECT(VciOwm);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

// ��ȡģ����
int __stdcall CVciOwm::XVciOwm::GetOwmName(CString& strOwmName)
{
	strOwmName = INTERFACE_INFO_IVciOwm.csName;
	return TRUE;
}

// ��ȡģ������
int __stdcall CVciOwm::XVciOwm::GetOwmDesc(CString& strOwmDesc)
{
	strOwmDesc = INTERFACE_INFO_IVciOwm.csDescription;
	return TRUE;
}

// ��ȡģ��汾
int __stdcall CVciOwm::XVciOwm::GetOwmVersion(int& nVersion)
{
	nVersion = 0x100;
	return TRUE;
}

// ��ȡģ������
int __stdcall CVciOwm::XVciOwm::GetOwmType(UINT& nType)
{
	nType = theApp.m_ol.nOWMType;
	return TRUE;
}

// ��ȡģ��ID
int __stdcall CVciOwm::XVciOwm::GetOwmID(UINT& nID)
{
	nID = theApp.m_ol.nOWMID;
	return TRUE;
}

// ����ƽ̨UI�ӿ�ָ��
int __stdcall CVciOwm::XVciOwm::SetIPlatUI(LPVOID lpIPlatUI)
{
	theApp.m_pIPlatUI = (IPlatUI*)lpIPlatUI;
	return TRUE;
}

// �����ĵ�ģ��ָ��
int __stdcall CVciOwm::XVciOwm::SetDocTemplate(CMultiDocTemplate* pDocTemplate)
{
	theApp.m_pDocTemplate = pDocTemplate;
	return TRUE;
}

// ��ȡ�ĵ�ģ��ָ��
int __stdcall CVciOwm::XVciOwm::GetDocTemplate(CMultiDocTemplate** ppDocTemplate)
{
	*ppDocTemplate = theApp.m_pDocTemplate;
	return TRUE;
}

// ��ȡָ������ʱ��ָ��
int __stdcall CVciOwm::XVciOwm::GetRuntimeClass(int nType, CRuntimeClass** ppRuntimeClass)
{
	switch(nType)
	{
	case 1: *ppRuntimeClass = theApp.m_ol.pDocClass;break;
	case 2: *ppRuntimeClass = theApp.m_ol.pFrameClass;break;
	case 3: *ppRuntimeClass = theApp.m_ol.pViewClass;break;
	case 4: *ppRuntimeClass = theApp.m_ol.pCtrlBarClass;break;
	}
	return TRUE;
}

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall CVciOwm::XVciOwm::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	CWnd* pWnd = NULL;
	if(nID == VIEW_ID_BOOKMARK)	// ��ǩ����
	{
		if(theApp.m_pIPlatUI)
		{
			theApp.m_pIPlatUI->AddOutput("##GRID**10");
			COwmSciteView* pView = GetActiveSciteEditView();
			if(pView)
			{
				pView->ViewBookmarksWindow();
			}
		}
	}else
	if(nID == VIEW_ID_BREAKPOINT)	// �ϵ㴰��
	{
		if(theApp.m_pIPlatUI)
		{
			theApp.m_pIPlatUI->AddOutput("##GRID**11");
			COwmSciteView* pView = GetActiveSciteEditView();
			if(pView)
			{
				pView->ViewBreaksWindow();
			}
		}
	}/*else
	if(nID == VIEW_ID_DYNHELP)	// ��̬��������
	{
		if(theApp.m_pIPlatUI)
		{
			//theApp.m_pIPlatUI->CreateDockingPane("org.owm.htmlview", "��̬����", VIEW_ID_DYNHELP, xtpPaneDockBottom, PANE_ATTACH_VIEW, NULL, 0);
		}
	}*/
	
	return pWnd;
}

// OWM�����
int __stdcall CVciOwm::XVciOwm::Command(int nCmd, WPARAM wParam, LPARAM lParam)
{
	return ::CommandOWM(nCmd, wParam, lParam);
}

// ע��˵�
int __stdcall CVciOwm::XVciOwm::RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc)
{
	return ::RegisterClientMenu(pMenu, aOWMMenus, nOWMIdStart, pDoc);
}

// ж�ز˵�
int __stdcall CVciOwm::XVciOwm::UnregisterClientMenu(COWMMenuArray& aOWMMenus)
{	
	return ::UnregisterClientMenu(aOWMMenus);
}

// ע��ToolBar
int __stdcall CVciOwm::XVciOwm::RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex)
{	
	return ::RegisterClientToolBar(pBar, aOWMMenus, nOWMIdStart, nIndex);
}

// ж��ToolBar
int __stdcall CVciOwm::XVciOwm::UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{	
	return ::UnregisterClientToolBar(pBar, aOWMMenus, nOWMIdStart);
}

// ע��OWM����ҳ
int __stdcall CVciOwm::XVciOwm::RegisterOption(COWMOptionArray& aOWMOption)
{	
	return ::RegisterOption(aOWMOption);
}

// ע���ӡҳ����������ҳ
int __stdcall CVciOwm::XVciOwm::RegisterPrintPageOption(COWMOptionArray& aOWMOption)
{	
	DLL_LOADSTRING(strTpsPrintPage, IDS_PROP_STR_PRINTPAGE);	// ��ӡҳ������
	DLL_LOADSTRING(strTpsAboutPrintPage, IDS_PROP_STR_ABOUT_PRINTPAGE);	// ���ô�ӡҳ��...
	OWM_REGISTEROPTION("", strTpsPrintPage, strTpsAboutPrintPage, IDD_TPS_PRINTPAGE, CTpsPrintPage, 0);

	return aOWMOption.GetSize();
}

CString GetValueFromSessionLine(CString strLine, CString strName, BOOL bToEnd = FALSE)
{
	int nPos = strLine.Find(strName+"=");
	if(nPos != -1)
	{
		strLine.Delete(0, nPos+strName.GetLength()+1);
		int nPosEnd = strLine.Find(",");
		if((nPosEnd != -1) && (!bToEnd))
		{
			strLine = strLine.Left(nPosEnd);
		}
		strLine.TrimLeft();
		strLine.TrimRight();
		return strLine;
	}
	return "";
}

// �����ĵ�Session
int __stdcall CVciOwm::XVciOwm::LoadDocumentSession(CDocument* pDoc, CStringArray& asSessionLine)
{
	COwmSciteView* pView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	if(pos != NULL)
	{
		pView = (COwmSciteView*)(pDoc->GetNextView(pos));
	}
	if(pView == NULL)
	{
		return FALSE;
	}

	CUIntArray auBreaks;
	CUIntArray auBookmarks;

	for(int i=0; i<asSessionLine.GetSize(); i++)
	{
		CString strSessionLine = asSessionLine[i];
		if(strSessionLine.Find("[[=SCI_POS=]]") == 0)
		{
			// �ָ���ǰλ��
			Position _pos;
			_pos._firstVisibleLine = atoi(GetValueFromSessionLine(strSessionLine, "firstVisibleLine"));
			_pos._xOffset = atoi(GetValueFromSessionLine(strSessionLine, "xOffset"));
			_pos._scrollWidth = atoi(GetValueFromSessionLine(strSessionLine, "scrollWidth"));
			_pos._startPos = atoi(GetValueFromSessionLine(strSessionLine, "startPos"));
			_pos._endPos = atoi(GetValueFromSessionLine(strSessionLine, "endPos"));
			_pos._selMode = atoi(GetValueFromSessionLine(strSessionLine, "selMode"));
			pView->_position = _pos;
			pView->restoreCurrentPos();
		}else
		if(strSessionLine.Find("[[=SCI_BREAK=]]") == 0)
		{
			int nLineNo = atoi(GetValueFromSessionLine(strSessionLine, "lineno"));
			CString strLineContent = GetValueFromSessionLine(strSessionLine, "line", TRUE);
			CString strLine;
			int nLineLength = pView->m_SciEdit.getLineLength(nLineNo-1);
			TCHAR* pszLine = new TCHAR[nLineLength+3];	// ����+3,��Ϊ�н������п���ռ������λ��
			memset(pszLine, 0, nLineLength+3);
			pView->m_SciEdit.getLine(nLineNo-1, pszLine, nLineLength);
			strLine = pszLine;
			delete [] pszLine;
			// У���кŶ�Ӧ�����Ƿ���ȷ
			if(strLine.Find(strLineContent) != -1)
			{
				auBreaks.Add(nLineNo);
			}
		}else
		if(strSessionLine.Find("[[=SCI_BOOKMARK=]]") == 0)
		{
			int nLineNo = atoi(GetValueFromSessionLine(strSessionLine, "lineno"));
			CString strLineContent = GetValueFromSessionLine(strSessionLine, "line", TRUE);
			CString strLine;
			int nLineLength = pView->m_SciEdit.getLineLength(nLineNo-1);
			TCHAR* pszLine = new TCHAR[nLineLength+3];	// ����+3,��Ϊ�н������п���ռ������λ��
			memset(pszLine, 0, nLineLength+3);
			pView->m_SciEdit.getLine(nLineNo-1, pszLine, nLineLength);
			strLine = pszLine;
			delete [] pszLine;
			// У���кŶ�Ӧ�����Ƿ���ȷ
			if(strLine.Find(strLineContent) != -1)
			{
				auBookmarks.Add(nLineNo);
			}
		}
	}

	// ���ضϵ��б�
	for(int i=0; i<auBreaks.GetSize(); i++)
	{
		pView->linemarkAdd(MARK_BREAKPOINT, auBreaks[i]-1);
	}

	// ������ǩ�б�
	for(int i=0; i<auBookmarks.GetSize(); i++)
	{
		pView->linemarkAdd(MARK_BOOKMARK, auBookmarks[i]-1);
	}

	return TRUE;
}

// �����ĵ�Session
int __stdcall CVciOwm::XVciOwm::SaveDocumentSession(CDocument* pDoc, CStringArray& asSessionLine)
{
	COwmSciteView* pView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	if(pos != NULL)
	{
		pView = (COwmSciteView*)(pDoc->GetNextView(pos));
	}
	if(pView == NULL)
	{
		return FALSE;
	}

	// ���浱ǰλ��
	pView->saveCurrentPos();
	Position _pos = pView->_position;
	CString strTmp;
	strTmp.Format("[[=SCI_POS=]]firstVisibleLine=%d,xOffset=%d,scrollWidth=%d,startPos=%d,endPos=%d,selMode=%d,",
			_pos._firstVisibleLine, _pos._xOffset, _pos._scrollWidth,
			_pos._startPos, _pos._endPos, _pos._selMode);
	asSessionLine.Add(strTmp);

	// ����ϵ��б�
	CUIntArray auBreaks;
	pView->linemarkGetAll(MARK_BREAKPOINT, auBreaks);
	for(int i=0; i<auBreaks.GetSize(); i++)
	{
		CString strLine;
		int nLineLength = pView->m_SciEdit.getLineLength(auBreaks[i]-1);
		TCHAR* pszLine = new TCHAR[nLineLength+3];	// ����+3,��Ϊ�н������п���ռ������λ��
		memset(pszLine, 0, nLineLength+3);
		pView->m_SciEdit.getLine(auBreaks[i]-1, pszLine, nLineLength);
		strLine = pszLine;
		delete [] pszLine;
		strLine.TrimRight();

		strTmp.Format("[[=SCI_BREAK=]]lineno=%d,line=%s", auBreaks[i], strLine);
		asSessionLine.Add(strTmp);
	}

	// ������ǩ�б�
	CUIntArray auBookmarks;
	pView->linemarkGetAll(MARK_BOOKMARK, auBookmarks);
	for(int i=0; i<auBookmarks.GetSize(); i++)
	{
		CString strLine;
		int nLineLength = pView->m_SciEdit.getLineLength(auBookmarks[i]-1);
		TCHAR* pszLine = new TCHAR[nLineLength+3];	// ����+3,��Ϊ�н������п���ռ������λ��
		memset(pszLine, 0, nLineLength+3);
		pView->m_SciEdit.getLine(auBookmarks[i]-1, pszLine, nLineLength);
		strLine = pszLine;
		delete [] pszLine;
		strLine.TrimRight();

		strTmp.Format("[[=SCI_BOOKMARK=]]lineno=%d,line=%s", auBookmarks[i], strLine);
		asSessionLine.Add(strTmp);
	}

	return TRUE;
}

// �����ĵ�
int __stdcall CVciOwm::XVciOwm::BackupDocument(CDocument* pDoc, LPCTSTR lpszBackupFile, int nBackupType)
{
	return TRUE;
}

// ��ȡ��������
int __stdcall CVciOwm::XVciOwm::GetImportTypeList(CStringArray& asImportType)
{
	return 0;
}

// ��ȡ��������
int __stdcall CVciOwm::XVciOwm::GetExportTypeList(CStringArray& asExportType)
{
	DLL_LOADSTRING(strExportHtmlFile, IDS_EXPORT_HTML_FILE);
	DLL_LOADSTRING(strExportRtfFile, IDS_EXPORT_RTF_FILE);
	asExportType.Add(strExportHtmlFile);
	asExportType.Add(strExportRtfFile);
	return asExportType.GetSize();
}

// �����ĵ�
int __stdcall CVciOwm::XVciOwm::ImportDocument(CDocument* pDoc, LPCTSTR lpszImportSource, int nImportType)
{
	return TRUE;
}

// �����ĵ�
int __stdcall CVciOwm::XVciOwm::ExportDocument(CDocument* pDoc, LPCTSTR lpszExportDest, int nExportType)
{
	COwmSciteView* pView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	if(pos != NULL)
	{
		pView = (COwmSciteView*)(pDoc->GetNextView(pos));
	}
	if(pView == NULL)
	{
		return FALSE;
	}

	if((nExportType < 0) || (nExportType > 1))
	{
		return FALSE;
	}

	switch(nExportType)
	{
	case 0:pView->exportHtmlFile(lpszExportDest);break;
	case 1:pView->exportRtfFile(lpszExportDest);break;
	}

	return TRUE;
}

// ����(pDocΪ�ձ�ʾ�������ĵ�)
int __stdcall CVciOwm::XVciOwm::Zoom(float fZoomFactor, CDocument* pDoc)
{
	return TRUE;
}

// ��ȡ��ǰ��������(pDocΪ�ձ�ʾ�������ĵ�)
int __stdcall CVciOwm::XVciOwm::GetZoomFactor(float& fZoomFactor, CDocument* pDoc)
{
	return TRUE;
}

// ��ȡ�����б�
int __stdcall CVciOwm::XVciOwm::GetZoomFactorRange(CStringArray& asZoomFactor, float& fZoomMin, float& fZoomMax)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
const TInterfaceInfo INTERFACE_LIST[] =
{
	INF_IVciOwm,
};

//////////////////////////////////////////////////////////////////////////
// OWMVCI����ĺ���
//////////////////////////////////////////////////////////////////////////
//
// ��ѯ�� VCI ����Ľӿ��б����ؽӿ���Ŀ
//
extern "C" __declspec(dllexport)
int GetInterfaceList(const TInterfaceInfo **pInfo)
{
	*pInfo = INTERFACE_LIST;
	return (sizeof(INTERFACE_LIST)/sizeof(INTERFACE_LIST[0]));
}

//
// �������󲢷��ض���Ľӿ�
//
extern "C" __declspec(dllexport)
LPVOID CreateObject(LPCSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData)
{
	TRACE("CreateObject(\"%s\")-OWM-OwmScite\n", lpcsInterfaceName);

	CVisualComponent *pObject;
	if(strcmp(lpcsInterfaceName, IID_IVciOwm) == 0)
	{
		pObject = new CVciOwm;
		OWM_LINKAGE ol;
		InstallOWM(ol);
	}
	else
	{
		TRACE("Can't find Object: %s, CreateObject failed!\n", lpcsInterfaceName);
		return NULL;
	}

	// Ϊ�����װ������Ϣ����ص�����
	if(lpInitData)
	{
		TInitData_DebugOut *pInitData_DebugOut = (TInitData_DebugOut *)lpInitData;
		pObject->InstallHandler(DEBUG_OUT_EVENT, pInitData_DebugOut->lpfnDebugOut, pInitData_DebugOut->nDeviceID);
	}

	*lppVciControl = pObject->VciControlQueryInterface();

	return pObject->ExternalQueryInterface();
}

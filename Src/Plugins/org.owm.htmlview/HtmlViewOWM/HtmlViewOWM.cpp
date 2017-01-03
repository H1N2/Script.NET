// HtmlViewOWM.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "HtmlViewOWM.h"
#include "ChildFrame.h"
#include "StylerView.h"
#include "StylerDoc.h"
#include "DynamicHelpBar.h"

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
// CHtmlViewOWMApp

BEGIN_MESSAGE_MAP(CHtmlViewOWMApp, CWinApp)
	//{{AFX_MSG_MAP(CHtmlViewOWMApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMApp construction

CHtmlViewOWMApp::CHtmlViewOWMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDebug	= FALSE;
	m_pDocTemplate = NULL;
	m_pIPlatUI = NULL;
	m_strDefaultPage = "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ƽ̨����(�ַ���)
/////////////////////////////////////////////////////////////////////////////
CString CHtmlViewOWMApp::GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
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
int CHtmlViewOWMApp::GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
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
CString CHtmlViewOWMApp::WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
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
int CHtmlViewOWMApp::WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
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
CString CHtmlViewOWMApp::GetPlatRootPath()
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
// ��ȡģ������·��
/////////////////////////////////////////////////////////////////////////////
CString CHtmlViewOWMApp::GetModulePath()
{
	//��ȡƽ̨·��
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
// ��·���ַ������д���
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMApp::ProcessPath(CString& strPath)
{
	CString strTemp;
	if(strPath.Find(_T("PLAT://")) == 0)
	{
		strTemp = strPath.Right(strPath.GetLength()-7);
		strTemp.Replace("/", "\\");
		if(strTemp.Find(_T("\\")) == 0)
		{	// GetPlatRootPath�Ѿ�������\,��˿���ȥ��
			strTemp = strTemp.Right(strTemp.GetLength()-1);
		}
		strPath = GetPlatRootPath() + strTemp;
	}else
	if(strPath.Find(_T("OWM://")) == 0)
	{
		strTemp = strPath.Right(strPath.GetLength()-6);
		strTemp.Replace("/", "\\");
		strPath = GetModulePath() + strTemp;
	}else
	{
		strPath.Replace("/", "\\");
		if(strPath.Find(_T("\\")) < 0)
		{	// ���û��·��,��ʾ��ģ��Ŀ¼
			strTemp = strPath;
			strPath = GetModulePath() + strTemp;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHtmlViewOWMApp object

CHtmlViewOWMApp theApp;

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
	ol.nOWMType				= OWM_DOC | OWM_MENU | OWM_TOOLBAR | OWM_OPTION | OWM_ONEFRAME | OWM_AUTOCREATE | OWM_DYNLOAD;
	ol.nOWMID				= 1000;
	ol.pDocClass			= RUNTIME_CLASS(CStylerDoc);//CHtmlViewOWMDoc);
	ol.pFrameClass			= RUNTIME_CLASS(CChildFrame);
	ol.pViewClass			= RUNTIME_CLASS(CStylerView);//CHtmlViewOWMView);
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
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWM�汾
/////////////////////////////////////////////////////////////////////////////
BOOL OWMVersion(OWM_VERSION& ov)
{
	ov.svOWMName	= "HtmlView";
	ov.svDescription= "HtmlViewOWM OWM";
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
// ȡ��һ��CStylerView
/////////////////////////////////////////////////////////////////////////////
CStylerView* GetFirstHtmlViewOWMView()
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
			return (CStylerView*)(pDoc->GetNextView(posview));
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
		aHelpStr->Add("Help of OWM[HtmlView]:");
		aHelpStr->Add("ID      : 1000");
		aHelpStr->Add("Auhtor  : Blueant");
		aHelpStr->Add("Version : 1.0");
		aHelpStr->Add("Descript: HtmlViewOWM OWM");
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

//#define OC_VIEWHTMLPAGE		100
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
			CStylerView* pView = GetFirstHtmlViewOWMView();
			if(!pView)
				return FALSE;
			CWnd* pFrame = pView->GetParent();
			if(!pFrame)
				return FALSE;
			pFrame->SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}

	case OC_VIEWHTMLPAGE:	// ��ʾҳ��
		{
			CStylerView* pView = GetFirstHtmlViewOWMView();
			if(lParam != NULL)
			{
				pView = (CStylerView*)lParam;
			}
			if(!pView)
			{
				theApp.m_pDocTemplate->OpenDocumentFile(NULL);
				pView = GetFirstHtmlViewOWMView();
				if(pView && (theApp.m_ol.nOWMType & OWM_MAXWINDOW))
				{
					// ���������
					pView->GetParentFrame()->ShowWindow(SW_SHOWMAXIMIZED);
				}
				if(!pView)
				{
					return FALSE;
				}
			}				
			pView->GetParentFrame()->ActivateFrame(); // ������
			//CString str = (LPCTSTR)wParam;
			//AfxMessageBox(str);
			pView->Navigate2((LPCTSTR)wParam);
			return TRUE;
		}

	case OC_GETHTMLURL:	// ��ȡHtmlҳ���URL
		{
			CStylerView* pView = GetFirstHtmlViewOWMView();
			if(lParam != NULL)
			{
				pView = (CStylerView*)lParam;
			}
			if(pView)
			{
				return (int)(LPCTSTR)(pView->GetCurrentURL());
			}
			return NULL;
		}

	case OC_REFRESH_DOCKINGHTML:	// ˢ��Docking Htmlҳ��
		{
			int nDockingPaneID = wParam;
			CString strURL = "tcl:<<BlueMessenger>><file>PLAT://\\html\\imstart.tcl";
			if(theApp.m_pIPlatUI)
			{
				CString strDefaultValue;
				theApp.m_pIPlatUI->GetVciParameter("org.owm.htmlview", "IMStartScript", strURL, strDefaultValue);
			}
			if(lParam != NULL)
			{
				strURL = (LPCTSTR)(LPARAM)lParam;
			}
			if(theApp.m_pIPlatUI)
			{
				CExplorerBar* pWnd = (CExplorerBar*)(theApp.m_pIPlatUI->GetDockingPaneWnd(nDockingPaneID));
				if(pWnd && pWnd->m_pIECtrl && pWnd->m_pIECtrl->GetSafeHwnd())
				{
					pWnd->m_pIECtrl->Navigate2(strURL);
				}
			}
		}

//	ע:�����Ҫ����ĳЩ�������ɫ,���Դ����������ע��
//	case OC_GETFONTARRAY:	// ��ȡ�������ñ�
//		{
//			int* pnItemCount = (int*)wParam;
//			*pnItemCount = 1;	// ���������
//			for(int i=0; i<(*pnItemCount); i++)
//			{
//				GetFontFromReg(g_FontItems+i);
//			}
//
//			if(lParam == 1)	// 1��ʾ��ԭΪȱʡֵ
//			{
//				g_FontItems[0].colorBk = CLR_DEFAULT;
//			}
//
//			return (int)g_FontItems;
//		}

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
// ��ʾ��ʼҳ
/////////////////////////////////////////////////////////////////////////////
void OnViewStartpage() 
{
	if(theApp.m_pDocTemplate)
	{
		CStylerView* pView = GetFirstHtmlViewOWMView();			
		if(pView == NULL)
		{
			theApp.m_pDocTemplate->OpenDocumentFile(NULL);
			pView = GetFirstHtmlViewOWMView();
			if(pView && (theApp.m_ol.nOWMType & OWM_MAXWINDOW))
			{
				// ���������
				pView->GetParentFrame()->ShowWindow(SW_SHOWMAXIMIZED);
			}
		}else
		{
			pView->GetParentFrame()->ActivateFrame(); // ������
		}
		if(pView)
		{
			pView->Navigate2(theApp.m_strDefaultPage);
		}
	}
}

void OnUpdateViewStartpage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}
/*
/////////////////////////////////////////////////////////////////////////////
// ��ʾ��ʱ��ϢDockingbar
/////////////////////////////////////////////////////////////////////////////
void OnViewIMsg() 
{
	if(theApp.m_pIPlatUI)
	{
		theApp.m_pIPlatUI->CreateDockingPane("org.owm.htmlview", 10011);
	}
}

void OnUpdateIMsg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}
*/
/////////////////////////////////////////////////////////////////////////////
// ע��OWM��չ�˵�
/////////////////////////////////////////////////////////////////////////////
int RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc)
{
	DLL_LOADSTRING(strViewMenu, IDS_VIEW_MENU);
	PLATUI_ADDMENUITEM(strViewMenu, IDR_OWM_VIEW, 0, OnViewStartpage, OnUpdateViewStartpage);
	//PLATUI_ADDMENUITEM(strViewMenu, IDR_OWM_VIEW, 1, OnViewIMsg, OnUpdateIMsg);
/*
	CMenu menu;
	get_dll_resource();
	menu.LoadMenu(IDR_OWM_VIEW);
	reset_dll_resource();
	CString strMenuItem;
	menu.GetSubMenu(0)->GetMenuString(0, strMenuItem, MF_BYPOSITION);
	CXTPCommandBar* pMenuBar = (CXTPCommandBar*)pMenu;
	CXTPControls* pControls = pMenuBar->GetControls();
	CXTPControl* pControl = pControls->GetFirst();
	CXTPCommandBar* pMenuBarSub = NULL;
	CXTPControl* pControlExtSub = NULL;
	while(pControl)
	{
		if(pControl->GetCaption() == "�鿴(&V)")
		{
			pMenuBarSub = pControl->GetCommandBar();
			CXTPControls* pControlsSub = pMenuBarSub->GetControls();
			pControl = pControlsSub->GetFirst();
			while(pControl)
			{
				if(pControl->GetCaption() == strMenuItem)
				{
					pControlExtSub = pControl;
					break;
				}
				pControlsSub->GetNext(pControl);
			}			
			break;
		}
		pControls->GetNext(pControl);
	}
	if(pMenuBarSub && !pControlExtSub)
	{
		CXTPControls* pControlsSub = pMenuBarSub->GetControls();
		CMenu* pmenu = menu.GetSubMenu(0);
		if(pmenu)
		{
			pControlsSub->AddMenuItem(pmenu, 0);
		}
	}
*/
/*	// ��ʼ��OWM�˵�,����ӵ����˵�
	get_dll_resource();
	CMenu* pmenu = new CMenu();
	pmenu->LoadMenu(IDR_OWM);
	CMenu* psubmenu = (CMenu*)(pmenu->GetSubMenu(0));
	reset_dll_resource();

	// ���˵��Ƿ��Ѿ�����,���������ɾ��
	int count = pMenu->GetMenuItemCount();
	OWM_CHECKMENUISEXIST(count-1, _T("HtmlViewOWM"))

	// �����˵���������չ�˵�
	pMenu->InsertMenu(count-1, MF_POPUP|MF_STRING|MF_ENABLED, 
				(UINT)psubmenu->GetSafeHmenu(), _T("HtmlViewOWM"));

	// �ڴ������չ�˵�
	OWM_REGISTERMENU(ID_OWM_ABOUT, AboutOWM, NULL, USER_NONE)

	// ��DOC�м�����չ�˵�ָ��,�ĵ��ر�ʱ���
	if(pDoc)
	{
		((CHtmlViewOWMDoc*)pDoc)->m_pMenu = pmenu;
	}
*/
	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM��չ�˵�����
/////////////////////////////////////////////////////////////////////////////
int UnregisterClientMenu(COWMMenuArray& aOWMMenus)
{
	//OWM_REMOVEMENUITEM("�鿴(&V)", IDR_OWM_VIEW, 1);
	// �ڴ�ע����չ�˵�����
	//OWM_UNREGISTERMENU(AboutOWM)

	return aOWMMenus.GetSize();
}


/////////////////////////////////////////////////////////////////////////////
// ��ʾ/����OWM����
/////////////////////////////////////////////////////////////////////////////
int OpenOwmView()
{
	if(GetFirstHtmlViewOWMView())
		theApp.m_ol.CommandPlat(OC_CLOSEVIEW, theApp.m_ol.nOWMID, 0);
	else
		theApp.m_ol.CommandPlat(OC_OPENVIEW, theApp.m_ol.nOWMID, 0);
	return 0;
}

void UpdateOpenOwmView(CCmdUI* pCmdUI)
{
	int nCheck = GetFirstHtmlViewOWMView() ? 1 : 0;
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
	//OWM_REGISTEROPTION("", "HtmlViewOWM", "HtmlViewOWM OWM", IDD_OWM, CDialog, 0)

	return aOWMOption.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ��OWM
/////////////////////////////////////////////////////////////////////////////
int InitOWM()
{
	// ע:�����������Ĵ���
	CStylerView::Load();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM
/////////////////////////////////////////////////////////////////////////////
int DoneOWM()
{
	// ע:�����������Ĵ���
	CStylerView::Save();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//

#include "vcicomm.h"
#include "vciowm.h"

#define INF_IVciOwm \
{\
	"IVciOwm",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"org.owm.htmview",\
	"HtmlViewOWM OWM",\
	"Blueant",\
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
	CString strStartPage = lpcsParams;
	if((strStartPage == "") && theApp.m_pIPlatUI)
	{
		theApp.m_pIPlatUI->GetVciParameter("", "StartPage", strStartPage, strStartPage);
	}
	theApp.m_strDefaultPage = strStartPage;
	return InitOWM();
}

// �������
int __stdcall CVciOwm::Done(LPCSTR lpcsParams)
{
	/* �Ѿ��ҵ��˳�ʱ��XMPP�쳣��ԭ��,����Ϊû��ע����ConnectionListener�ļ���,ע����û������
	// ɾ��BlueMessenger XMPP����,����ƽ̨�˳����쳣
	if(theApp.m_pIPlatUI)
	{
		CString strIMVciObjectName = "BlueMessengerVci";
		CString strDefaultValue;
		theApp.m_pIPlatUI->GetVciParameter("org.owm.htmlview", "IMVciObjectName", strIMVciObjectName, strDefaultValue);

		theApp.m_pIPlatUI->ReleaseObjectByInstanceName(strIMVciObjectName);
	}*/

	// ɾ��Html Bar�����б��е���
	int count = theApp.m_HtmlBarList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = theApp.m_HtmlBarList.FindIndex(i);
		CExplorerBar* pWnd = (CExplorerBar*)theApp.m_HtmlBarList.GetAt(pos);
		delete pWnd;
	}

	return DoneOWM();
}

// ��Ϣ����
int __stdcall CVciOwm::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
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
	//theApp.m_ol.pDocTemplate = pDocTemplate;
	theApp.m_pDocTemplate = pDocTemplate;
	return TRUE;
}

// ��ȡ�ĵ�ģ��ָ��
int __stdcall CVciOwm::XVciOwm::GetDocTemplate(CMultiDocTemplate** ppDocTemplate)
{
	//*ppDocTemplate = theApp.m_ol.pDocTemplate;
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

#define VIEW_ID_DYNHELP				10014	// ��̬��������

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall CVciOwm::XVciOwm::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	CString strScript = lpszParam;
	/*
	CString strIMStartScript = "tcl:<<BlueMessenger>><file>PLAT://\\html\\imstart.tcl";
	if(theApp.m_pIPlatUI)
	{
		CString strDefaultValue;
		theApp.m_pIPlatUI->GetVciParameter("org.owm.htmlview", "IMStartScript", strIMStartScript, strDefaultValue);
	}
	*/

	if((VIEW_ID_DYNHELP == nID) && theApp.m_pIPlatUI)
	{
		CString strDefaultValue;
		theApp.m_pIPlatUI->GetVciParameter("org.owm.htmlview", "DynHelpPage", strScript, strDefaultValue);
		strScript.MakeUpper();
		theApp.ProcessPath(strScript);

		CDynamicHelpBar* pWnd = new CDynamicHelpBar();
		pWnd->Create(_T("STATIC"), _T(""), WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
					CRect(0, 0, 0, 0), pParentWnd, 0);
		pWnd->OnViewHtml(strScript);
		pWnd->m_nId = nID;
		theApp.m_HtmlBarList.AddTail(pWnd);
		return pWnd;
	}

	CExplorerBar* pWnd = new CExplorerBar();
	pWnd->Create(_T("STATIC"), _T(""), WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
				CRect(0, 0, 0, 0), pParentWnd, 0);
	pWnd->OnViewHtml(strScript);
	pWnd->m_nId = nID;
	theApp.m_HtmlBarList.AddTail(pWnd);
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
	return aOWMOption.GetSize();
}

// ��Session���л�ȡָ�����ֵı���ֵ(��ʽΪname=value,)
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
	return TRUE;
}

// �����ĵ�Session
int __stdcall CVciOwm::XVciOwm::SaveDocumentSession(CDocument* pDoc, CStringArray& asSessionLine)
{
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
	return 0;
}

// �����ĵ�
int __stdcall CVciOwm::XVciOwm::ImportDocument(CDocument* pDoc, LPCTSTR lpszImportSource, int nImportType)
{
	return TRUE;
}

// �����ĵ�
int __stdcall CVciOwm::XVciOwm::ExportDocument(CDocument* pDoc, LPCTSTR lpszExportDest, int nExportType)
{
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
	TRACE("CreateObject(\"%s\")\n", lpcsInterfaceName);

	CVisualComponent *pObject;
	if(strcmp(lpcsInterfaceName, IID_IVciOwm) == 0)
	{
		pObject = new CVciOwm;
		OWM_LINKAGE ol;
		InstallOWM(ol);
		//get_dll_resource();
		//theApp.m_ol.pDocTemplate = new CMultiDocTemplate(
		//	NULL,
		//	ol.pDocClass,
		//	ol.pFrameClass,
		//	ol.pViewClass);
		//reset_dll_resource();
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


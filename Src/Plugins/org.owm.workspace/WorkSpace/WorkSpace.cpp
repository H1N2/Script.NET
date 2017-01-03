// WorkSpace.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WorkSpace.h"
#include "ChildFrame.h"
#include "WorkSpaceView.h"
#include "WorkSpaceDoc.h"
//#include "WorkSpaceBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LOADRCSTRING(str,strID) \
	get_dll_resource();\
	str.LoadString(strID);\
	reset_dll_resource();

void get_dll_resource(void);
void reset_dll_resource(void);

static CString g_strOWMDescription(_T(""));

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
// CWorkSpaceApp

BEGIN_MESSAGE_MAP(CWorkSpaceApp, CWinApp)
	//{{AFX_MSG_MAP(CWorkSpaceApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceApp construction

CWorkSpaceApp::CWorkSpaceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDebug	= FALSE;
	m_pDocTemplate = NULL;
	m_pIPlatUI = NULL;
	m_nLastTreePaneId = 0;
	m_nLastTreeType = PGI_FILE;
	m_strLastBmpFile = "";
}
/*
/////////////////////////////////////////////////////////////////////////////
// ��INI
/////////////////////////////////////////////////////////////////////////////
CString CWorkSpaceApp::GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
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
// ��INI
/////////////////////////////////////////////////////////////////////////////
int CWorkSpaceApp::GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
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
// дINI
/////////////////////////////////////////////////////////////////////////////
CString CWorkSpaceApp::WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
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
// дINI
/////////////////////////////////////////////////////////////////////////////
int CWorkSpaceApp::WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
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
*/
/////////////////////////////////////////////////////////////////////////////
// The one and only CWorkSpaceApp object

CWorkSpaceApp theApp;

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
	ol.nOWMType				= OWM_DOC | /*OWM_MENU | OWM_TOOLBAR |*/ OWM_OPTION | OWM_TABCTRLBAR | OWM_AUTOCREATE | OWM_ONEFRAME | OWM_BROADCAST | OWM_TCL;
	ol.nOWMID				= 110;
	ol.pDocClass			= RUNTIME_CLASS(CWorkSpaceDoc);
	ol.pFrameClass			= RUNTIME_CLASS(CChildFrame);
	ol.pViewClass			= RUNTIME_CLASS(CWorkSpaceView);
	ol.pCtrlBarClass		= NULL;
	ol.SetOWM				= NULL;
	ol.InitOWM				= InitOWM;
	ol.DoneOWM				= DoneOWM;
	ol.RegisterTclCmd		= NULL;//Tcl_RegisterCmd;;
	ol.UnregisterTclCmd		= NULL;//Tcl_UnregisterCmd;
	ol.RegisterTclExKeyword	= NULL;//Tcl_RegisterExKeyword;
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
	ov.svOWMName	= "WorkSpace";

	LOADRCSTRING(g_strOWMDescription,IDS_OWM_WS)
	ov.svDescription = g_strOWMDescription.GetBuffer(g_strOWMDescription.GetLength());
       
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
// ȡ��һ��CWorkSpaceView
/////////////////////////////////////////////////////////////////////////////
CWorkSpaceView* GetFirstWorkSpaceView()
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
			return (CWorkSpaceView*)(pDoc->GetNextView(posview));
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// OWM�������
/////////////////////////////////////////////////////////////////////////////
int OWMCmdHelp(int nCmd, CStringArray* aHelpStr)
{
	CString strTemp;
	switch(nCmd)
	{
	case OC_CMDALL:
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ƽ̨��ѯOWM��װ��Ϣ
/////////////////////////////////////////////////////////////////////////////
static OWM_INFO OwmInfo;
OWM_INFO* GetOwmInfo(int nId)
{
	if(!theApp.m_ol.CommandPlat)
		return NULL;

	COWMArray* pOwmArr = (COWMArray*)(theApp.m_ol.CommandPlat(OC_QUERYOWMID, nId, 0));
	if(pOwmArr)
	{
		OwmInfo.ov	= ((*pOwmArr)[0]).ov;
		OwmInfo.ol	= ((*pOwmArr)[0]).ol;
		OwmInfo.ins	= ((*pOwmArr)[0]).ins;
		OwmInfo.pBar= ((*pOwmArr)[0]).pBar;
		delete pOwmArr;
		return &OwmInfo;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ����ID��ȡĳ��������
/////////////////////////////////////////////////////////////////////////////
CWorkSpaceView* GetWorkSpaceView(int nId)
{
	if(theApp.m_pIPlatUI == NULL)
	{
		return NULL;
	}

	CWnd* pWnd = theApp.m_pIPlatUI->GetDockingPaneWnd(nId);
	if(!pWnd)
	{
		// �������δ���������ȴ���
		theApp.m_pIPlatUI->ActiveDockingPane(nId);
		theApp.m_nLastTreePaneId = nId;
		theApp.m_strLastBmpFile = "";
		// ��Ϊ��ʱ���ڻ�������,�����Ȼ���ؿ�
		return NULL;
	}

	return (CWorkSpaceView*)(pWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST));
}

// �����ڵ���ʼID
static int g_nTreePaneId = 19000;

/////////////////////////////////////////////////////////////////////////////
// OWM�������
/////////////////////////////////////////////////////////////////////////////
int CommandOWM(int nCmd, WPARAM wParam, LPARAM lParam)
{
	switch(nCmd)
	{
	case OC_SETOWMLINK:	// ����OWM����
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
			CWorkSpaceView* pView = GetFirstWorkSpaceView();
			if(!pView)
				return FALSE;
			CWnd* pFrame = pView->GetParent();
			if(!pFrame)
				return FALSE;
			pFrame->SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}

	case OC_GOTOITEM:	// ת��ָ������Ŀ
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM item;
				item.dwMask	= TIM_ID;
				item.nID	= lParam;
				pView->EnsureItem(&item);
				return TRUE;
			}
			return FALSE;
		}

	// ������
	case OC_WS_ADDTREE:	// ���������
		{
			if(theApp.m_pIPlatUI)
			{
				theApp.m_nLastTreeType = lParam;
				int nID = g_nTreePaneId++;
				theApp.m_pIPlatUI->CreateDockingPane("org.owm.workspace",
					(LPCTSTR)wParam, nID, xtpPaneDockLeft, PANE_ATTACH_VIEW, NULL, 0);
				return nID;
			}
			return 0;
		}

	case OC_WS_ADDTREEEX:	// ���������(��չ��ʽ,ʹ�ýṹ������Ϣ)
		{
			if(theApp.m_pIPlatUI)
			{
				STRU_ADDTREEEX* pStruTree = (STRU_ADDTREEEX*)wParam;
				theApp.m_nLastTreeType = pStruTree->nTreeType;
				int nID = 0;
				if(pStruTree->nPaneId == 0)
				{
					nID = g_nTreePaneId++;
				}else
				{
					nID = pStruTree->nPaneId;
				}

				theApp.m_strLastBmpFile = pStruTree->strBmpFile;

				HICON hIcon = NULL;
				if(pStruTree->strIconFile != "")
				{
					WORD wIndex = 0;
					hIcon = ::ExtractAssociatedIcon(AfxGetApp()->m_hInstance,
								pStruTree->strIconFile.GetBuffer(0), &wIndex);
					pStruTree->strIconFile.ReleaseBuffer();
				}else
				{
					//get_dll_resource();
					//hIcon = AfxGetApp()->LoadIcon(IDI_ICON_WORKSPACE);
					//reset_dll_resource();
				}

				theApp.m_pIPlatUI->CreateDockingPane("org.owm.workspace",
					pStruTree->strTreeName, nID, xtpPaneDockLeft, PANE_ATTACH_VIEW,
					hIcon, pStruTree->nAttachPaneId);
				return nID;
			}
			return 0;
		}

	case OC_WS_CLOSETREE:	// �ر�������
		{
			if(theApp.m_pIPlatUI)
			{
				return theApp.m_pIPlatUI->CloseDockingPane(wParam);
			}
			return 0;
		}

	case OC_WS_GETTREEINDEX:	// �������ֵõ���ID
		{
			if(theApp.m_pIPlatUI)
			{
				return theApp.m_pIPlatUI->GetDockingPaneIdByName((LPCTSTR)wParam);
			}
			return -1;
		}

	case OC_WS_SETACTIVETREE:	// ���û��
		{
			if(theApp.m_pIPlatUI)
			{
				return theApp.m_pIPlatUI->ActiveDockingPane(wParam);
			}
			return 0;
		}

	case OC_WS_ACTIVECB:	// ��װ���ڼ���Ļص�����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->InstallVCCallBack((TYPEOF_ViewChange*)lParam);
				return wParam;
			}
			return -1;
		}

	case OC_WS_TREEEXIST:	// �ж�ָ���������Ƿ��Ѿ�����
		{
			if(theApp.m_pIPlatUI)
			{
				CWnd* pWnd = theApp.m_pIPlatUI->GetDockingPaneWnd(wParam);
				if(pWnd)
				{
					return TRUE;
				}
			}
			return FALSE;
		}

	case OC_WS_SETTREETYPE:	// ���ô�������
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->SetTreeType(lParam);
				return TRUE;
			}
			return FALSE;
		}

	case OC_WS_SETTREEIMAGE:	// ��������ͼ����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->SetTreeImageList((LPCTSTR)lParam);
				return TRUE;
			}
			return FALSE;
		}

	case OC_WS_ADDMENU:	// ��Ӳ˵�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				CMenu* pMenu = (CMenu*)lParam;
				pView->AddTreeMenu(pMenu);
				return wParam;
			}
			return -1;
		}

	case OC_WS_ADDMENUITEM:	// ��Ӳ˵���
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKMENUITEM* pItem = (HWORKMENUITEM*)lParam;
				pView->AddMenuItem(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_ADDTREEITEM:	// ������ڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				HTREEITEM hItem = pView->AddTreeItem(pItem);
				return (int)hItem;
			}
			return NULL;
		}

	case OC_WS_DELTREEITEM:	// ɾ���ڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				BOOL bOK = pView->DelTreeItem(pItem);
				return bOK;
			}
			return FALSE;
		}

	case OC_WS_SETITEMTYPE:	// ���ýڵ�����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemType(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMTYPE:	// ȡ�ýڵ�����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				BYTE byType = pView->GetItemType(pItem);
				return byType;
			}
			return -1;
		}

	case OC_WS_SETITEMSTATE:	// ���ýڵ�״̬
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				BYTE byStateOld = pView->GetItemState1(pItem);
				if(byStateOld != WTS_DISABLE)
				{
					pView->SetItemState1(pItem);
				}
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMSTATE:	// ȡ�ýڵ�״̬
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				BYTE byState = pView->GetItemState1(pItem);
				return byState;
			}
			return -1;
		}

	case OC_WS_SETITEMMENU:	// ���ýڵ�˵�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemMenu(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMMENU:	// ȡ�ýڵ�˵�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				CMenu* pMenu = pView->GetItemMenu(pItem);
				return (int)pMenu;
			}
			return NULL;
		}

	case OC_WS_SETITEMOBJECT:	// ���ýڵ����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemObject(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMOBJECT:	// ȡ�ýڵ����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				CObject* pObject = pView->GetItemObject(pItem);
				return (int)pObject;
			}
			return NULL;
		}

	case OC_WS_SETITEMID:	// ���ýڵ�ID
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemID(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMID:	// ȡ�ýڵ�ID
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				int nID = pView->GetItemID(pItem);
				return (int)nID;
			}
			return NULL;
		}

	case OC_WS_SETITEMNAME:	// ���ýڵ���
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemName(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMNAME:	// ȡ�ýڵ���
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				CString strName = pView->GetItemName(pItem);
				return (int)(LPCTSTR)strName;
			}
			return NULL;
		}

	case OC_WS_SETITEMIMAGE:	// ���ýڵ�ͼ��
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemImage(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_GETITEMIMAGE:	// ȡ�ýڵ�ͼ��
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				int nImage = pView->GetItemImage(pItem);
				return nImage;
			}
			return NULL;
		}

	case OC_WS_EXPANDITEM:	// չ���ڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->ExpandItem(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_COLLAPSEITEM:	// �����ڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->CollapseItem(pItem);
				return wParam;
			}
			return -1;
		}

	case OC_WS_SETDCLICKSCRIPT:	// ������ȱʡ�Ľڵ�˫���ű�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->SetDClickScript((LPCTSTR)lParam);
				return 1;
			}
			return 0;
		}

	case OC_WS_SETITEMINFO:	// ���ýڵ���Ϣ
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->SetItemInfo(pItem);
				return 1;
			}
			return 0;
		}

	case OC_WS_GETITEMINFO:	// ��ȡ�ڵ���Ϣ
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				pView->GetItemInfo(pItem);
				return 1;
			}
			return 0;
		}

	case OC_WS_SEARCHITEM:	// ���ҽڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				return (int)(pView->SearchItem(pItem));
			}
			return NULL;
		}

	case OC_WS_GETCHILDITEM:	// ��ȡ��һ���ӽڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				return (int)(pView->GetChildItem(pItem));
			}
			return 0;
		}

	case OC_WS_GETNEXTITEM:	// ��ȡ��һ���ڵ�
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				return (int)(pView->GetNextItem(pItem));
			}
			return 0;
		}

	case OC_WS_SORTCHILDREN:	// ���ӽڵ�����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTREEITEM* pItem = (HWORKTREEITEM*)lParam;
				return pView->SortChildren(pItem);
			}
			return 0;
		}

	case OC_WS_SETTASKTHEME:	// ����TaskPanel��Theme
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->SetTaskPanelTheme((XTPTaskPanelPaintTheme)lParam);
				return TRUE;
			}
			return FALSE;
		}

	case OC_WS_ADDTASKITEM:	// ���һ��TaskItem
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTASKITEM* pItem = (HWORKTASKITEM*)lParam;
				CXTPTaskPanelGroupItem* pTaskItem = pView->AddTaskItem(pItem);
				int nID = pView->GetTaskItemIDByItem(pTaskItem);
				return nID;
			}
			return 0;
		}

	case OC_WS_DELTASKITEM:	// ɾ��ָ��TaskItems
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				HWORKTASKITEM* pItem = (HWORKTASKITEM*)lParam;
				pView->DelTaskItem(pItem);
				return TRUE;
			}
			return FALSE;
		}

	case OC_WS_SETTASKICON:	// ����TaskPanel��ͼ���ͼ����
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				int nImageIndex = pView->SetTaskPanelIcon((LPCTSTR)lParam);
				return nImageIndex;
			}
			return 0;
		}

	case OC_WS_CREATTASKMENU:	// ��ָ���˵�����TaskPanel
		{
			CWorkSpaceView* pView = GetWorkSpaceView(wParam);
			if(pView)
			{
				pView->CreateTaskMenu((CMenu*)lParam);
				return TRUE;
			}
			return FALSE;
		}

	case OC_WS_SETPAGEMENU:	// ����ҳ��˵�
		{
			return FALSE;
		}

	default:
		break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾ������
/////////////////////////////////////////////////////////////////////////////
void OnViewNavigater() 
{
	if(theApp.m_pIPlatUI)
	{
		theApp.m_pIPlatUI->CreateDockingPane("org.owm.workspace", TREE_ID_NAVIGATER);
		theApp.m_nLastTreeType = PGI_FILE;
		theApp.m_nLastTreePaneId = TREE_ID_NAVIGATER;
		theApp.m_strLastBmpFile = "";
	}
}

void OnUpdateViewNavigater(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾ�ļ���
/////////////////////////////////////////////////////////////////////////////
void OnViewFileTree() 
{
	if(theApp.m_pIPlatUI)
	{
		theApp.m_pIPlatUI->CreateDockingPane("org.owm.workspace", TREE_ID_FILE);
		theApp.m_nLastTreeType = PGI_FILE;
		theApp.m_nLastTreePaneId = TREE_ID_FILE;
		theApp.m_strLastBmpFile = "";
	}
}

void OnUpdateViewFileTree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM��չ�˵�
/////////////////////////////////////////////////////////////////////////////
int RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc)
{
	DLL_LOADSTRING(strViewMenu, IDS_VIEW_MENU);
	//PLATUI_ADDMENUITEM(strViewMenu, IDR_OWM_VIEW, 0, OnViewNavigater, OnUpdateViewNavigater);
	PLATUI_ADDMENUITEM(strViewMenu, IDR_OWM_VIEW, 1, OnViewFileTree, OnUpdateViewFileTree);
	/*
	// ��ʼ��OWM�˵�,����ӵ����˵�
	get_dll_resource();
	CMenu* pmenu = new CMenu();
	pmenu->LoadMenu(IDR_OWM);
	CMenu* psubmenu = (CMenu*)(pmenu->GetSubMenu(0));
	reset_dll_resource();

	// ���˵��Ƿ��Ѿ�����,���������ɾ��
	int count = pMenu->GetMenuItemCount();
	OWM_CHECKMENUISEXIST(count, _T("WorkSpace"))

	// �����˵���������չ�˵�
	pMenu->InsertMenu(count, MF_POPUP|MF_STRING|MF_ENABLED, 
				(UINT)psubmenu->GetSafeHmenu(), _T("WorkSpace"));

	// �ڴ������չ�˵�
	OWM_REGISTERMENU(ID_OWM_ABOUT, AboutOWM, NULL, USER_NONE)

	// ��DOC�м�����չ�˵�ָ��,�ĵ��ر�ʱ���
	if(pDoc)
	{
		((CWorkSpaceDoc*)pDoc)->m_pMenu = pmenu;
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
// ��ʾ/����OWM ControlBar
/////////////////////////////////////////////////////////////////////////////
int OpenOwmBar()
{
	OWM_INFO* pOWM = GetOwmInfo(theApp.m_ol.nOWMID);
	if(pOWM && pOWM->pBar)
	{
		if(pOWM->pBar->IsWindowVisible())
			theApp.m_ol.CommandPlat(OC_CLOSEBAR, theApp.m_ol.nOWMID, 1);
		else
			theApp.m_ol.CommandPlat(OC_OPENBAR, theApp.m_ol.nOWMID, 1);
	}
	return 0;
}

void UpdateOpenOwmBar(CCmdUI* pCmdUI)
{
	int nCheck = 0;
	OWM_INFO* pOWM = GetOwmInfo(theApp.m_ol.nOWMID);
	if(pOWM && pOWM->pBar)
	{
		if(pOWM->pBar->IsWindowVisible())
			nCheck = 1;
	}
	pCmdUI->SetCheck(nCheck);
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM ToolBar
/////////////////////////////////////////////////////////////////////////////
int RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex)
{
	/*
	CToolBar* pToolBar = (CToolBar*)pBar;

	CToolBarCtrl* pCtrl = &(pToolBar->GetToolBarCtrl());
	CImageList* pImage = pCtrl->GetImageList();
	int count = pImage->GetImageCount();
	CBitmap bitmap;
	get_dll_resource();
	bitmap.LoadBitmap(IDR_OWM);
	pImage->Add(&bitmap, RGB(0, 128, 128));
	reset_dll_resource();

	// �ڴ����ToolBar
	OWM_REGISTERTOOLBAR(count, OpenOwmBar, UpdateOpenOwmBar, USER_NONE)
*/
	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM ToolBar
/////////////////////////////////////////////////////////////////////////////
int UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{
	/*
	CToolBar* pToolBar = (CToolBar*)pBar;

	// �ڴ�ע��ToolBar
	OWM_UNREGISTERTOOLBAR(OpenOwmBar)
*/
	return aOWMMenus.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ע��OWM����ҳ
/////////////////////////////////////////////////////////////////////////////
int RegisterOption(COWMOptionArray& aOWMOption)
{
	// �ڴ��������ҳ
	//OWM_REGISTEROPTION("", "WorkSpace", "WorkSpace OWM.", IDD_OWM, CDialog, 0)

	return aOWMOption.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ��OWM
/////////////////////////////////////////////////////////////////////////////
int InitOWM()
{
//	theApp.m_pIPlatUI->ActiveDockingPane(TREE_ID_FILE);
//	theApp.m_nLastTreePaneId = TREE_ID_FILE;
//	theApp.m_pIPlatUI->ActiveDockingPane(TREE_ID_NAVIGATER);
//	theApp.m_nLastTreePaneId = TREE_ID_NAVIGATER;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж��OWM
/////////////////////////////////////////////////////////////////////////////
int DoneOWM()
{

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
	"owm.blueui.workspace",\
	"OWM WorkSpace",\
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
	return 0;
}

// ��ѯ�����Ƿ�֧��
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

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall CVciOwm::XVciOwm::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	get_dll_resource();
	CWnd* pWnd = (CWnd*)(RUNTIME_CLASS(CWorkSpaceView));
	theApp.m_nLastTreePaneId = nID;
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

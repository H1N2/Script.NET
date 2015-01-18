//-----------------------------------------------------------------------------------------------------------
// Script.NET��Ȩ����
// 
// ������ĸ��˰汾Ϊ��ѡ���Դ����������������غ�ʹ�ã�������������ҵĿ�ġ�
// 
// ������Ĳ��ִ���ʹ����һЩ��Դ�������ο���ؿ�Դ����İ�Ȩ��Ϣ��
// ������Ĵ�����Ҫ������һЩ��ҵ������˲��ֲ������ڴ�����У������ȡ�����ҵ������룬�����дӹٷ�������ȡ��
// ��������ֲ��в�������Ϊ�����ռ��������ã��˲��ְ�ȨΪԭ�������С�
//-----------------------------------------------------------------------------------------------------------

// BlueUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
//#include "vld.h"
#include "BlueUI.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BlueUIDoc.h"
#include "BlueUIView.h"
#include "mdump.h"
#include "cgfiltyp.h"
#include "Splash.h"
#include "PictureEx.h"
#include "FlashWin.h"
#include "TreePropertySheet.h"
#include "TpsFont.h"
#include "TpsAssociate.h"
#include "TpsLanguage.h"
#include "TpsUIStyle.h"
#include "TpsWorkspace.h"
#include "TpsVci.h"
#include "TpsInterpDirect.h"

#include <locale.h>
#include "winnls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlueUIApp

BEGIN_MESSAGE_MAP(CBlueUIApp, CWinApp)
	//{{AFX_MSG_MAP(CBlueUIApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
	ON_COMMAND(ID_OPTION, OnOption)
	ON_UPDATE_COMMAND_UI(ID_OPTION, OnUpdateOption)
	ON_COMMAND(ID_VCI_MANAGER, OnVciManager)
	ON_UPDATE_COMMAND_UI(ID_VCI_MANAGER, OnUpdateVciManager)
	ON_COMMAND(ID_PROJECT_NEWFILE, OnProjectNewfile)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_NEWFILE, OnUpdateProjectNewfile)
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_UPDATE_COMMAND_UI(ID_HELP_TOPICS, OnUpdateHelpTopics)
	ON_COMMAND(ID_HELP_REGISTERATE, OnHelpRegisterate)
	ON_UPDATE_COMMAND_UI(ID_HELP_REGISTERATE, OnUpdateHelpRegisterate)
	ON_COMMAND(ID_HELP_UPGRADE, OnHelpUpgrade)
	ON_UPDATE_COMMAND_UI(ID_HELP_UPGRADE, OnUpdateHelpUpgrade)
	ON_COMMAND(ID_HELP_HOMEPAGE, OnHelpHomepage)
	ON_UPDATE_COMMAND_UI(ID_HELP_HOMEPAGE, OnUpdateHelpHomepage)
	ON_COMMAND(ID_HELP_BUGREPORT, OnHelpBugreport)
	ON_UPDATE_COMMAND_UI(ID_HELP_BUGREPORT, OnUpdateHelpBugreport)
	ON_COMMAND(ID_OPEN_WITH, OnOpenWith)
	ON_UPDATE_COMMAND_UI(ID_OPEN_WITH, OnUpdateOpenWith)
	ON_COMMAND(ID_TOOLS_EXTEND, OnToolsExtend)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXTEND, OnUpdateToolsExtend)
	ON_COMMAND(ID_HELP_DOWNLOAD, OnHelpDownload)
	ON_UPDATE_COMMAND_UI(ID_HELP_DOWNLOAD, OnUpdateHelpDownload)
	ON_COMMAND(ID_PROJECT_NEW, OnProjectNew)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_NEW, OnUpdateProjectNew)
	ON_COMMAND(ID_PROJECT_OPEN, OnProjectOpen)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_OPEN, OnUpdateProjectOpen)
	ON_COMMAND(ID_PROJECT_SAVE, OnProjectSave)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVE, OnUpdateProjectSave)
	ON_COMMAND(ID_PROJECT_CLOSE, OnProjectClose)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_CLOSE, OnUpdateProjectClose)
	ON_COMMAND(ID_PROJECT_BUILD, OnProjectBuild)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_BUILD, OnUpdateProjectBuild)
	ON_COMMAND(ID_PROJECT_REBUILD, OnProjectRebuild)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_REBUILD, OnUpdateProjectRebuild)
	ON_COMMAND(ID_PROJECT_CLEAN, OnProjectClean)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_CLEAN, OnUpdateProjectClean)
	ON_COMMAND(ID_PROJECT_INFO, OnProjectInfo)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_INFO, OnUpdateProjectInfo)
	ON_COMMAND(ID_PROJECT_PROPERTY, OnProjectProperty)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_PROPERTY, OnUpdateProjectProperty)
	ON_COMMAND(ID_PROJECT_RUN, OnProjectRun)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_RUN, OnUpdateProjectRun)
	ON_COMMAND(ID_FILE_LOADSESSION, OnFileLoadsession)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADSESSION, OnUpdateFileLoadsession)
	ON_COMMAND(ID_FILE_SAVESESSION, OnFileSavesession)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVESESSION, OnUpdateFileSavesession)
	ON_COMMAND(ID_FILE_PRINT_PAGESETUP, OnFilePrintPagesetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PAGESETUP, OnUpdateFilePrintPagesetup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlueUIApp construction

CBlueUIApp::CBlueUIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pIProjectManager = NULL;
	m_pIProject = NULL;
	m_pIMessageQueue = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBlueUIApp object

CBlueUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ���̼�ͨ��,�����������ַ���
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::InterprocessCommunication()
{
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = 1024;

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-9025-4A0CD86F6C9F");

	// ��Win7��ParseCommandLine������ȷ���������в���,��˸�Ϊ�Լ���������
	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	//if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)

	int nShellCommand = CCommandLineInfo::FileNew;
	if(__argc > 1)
	{
		nShellCommand = CCommandLineInfo::FileOpen;
	}
	CString strFileNames = _T("");
	for(int i=1; i<__argc; i++)
	{
		if(i != 1)
		{
			strFileNames += _T(" ");
		}
		strFileNames += _T("\"");
		strFileNames += __targv[i];
		strFileNames += _T("\"");
	}

	{
		//AfxMessageBox(cmdInfo.m_strFileName);

		HANDLE hFileMapping = NULL;
		LPVOID pViewOfFile = NULL;

		// Create file mapping which can contain dwMemoryFileSize characters
		hFileMapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,           // system paging file
			NULL,                           // security attributes
			PAGE_READWRITE,                 // protection
			0,                              // high-order DWORD of size
			dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
			sMemoryFileName);               // name

		DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
				// this instance is not first (other instance created file mapping)

		if(! hFileMapping)
		{
			TRACE(_T("Creating of file mapping failed.\n"));
			return FALSE;
		}

        pViewOfFile = MapViewOfFile(
            hFileMapping,               // handle to file-mapping object
            FILE_MAP_ALL_ACCESS,        // desired access
            0,
            0,
            0);                         // map all file

        if(!pViewOfFile)
        {
            TRACE(_T("MapViewOfFile failed.\n"));
			CloseHandle(hFileMapping);
			return FALSE;
        }

		lstrcpy( (LPTSTR) pViewOfFile, strFileNames);//cmdInfo.m_strFileName);

		DWORD result;
		LRESULT ok = SendMessageTimeout(HWND_BROADCAST,
									   WM_CHECK_ITS_ME,
									   //cmdInfo.m_nShellCommand,
									   nShellCommand,		// wParam:�����в�������
									   //(LPARAM)(LPCTSTR)(cmdInfo.m_strFileName),
									   __argc-1,				// �ļ�����
									   SMTO_BLOCK |
									   SMTO_ABORTIFHUNG,
									   200,
									   &result);

		UnmapViewOfFile(pViewOfFile);
		CloseHandle(hFileMapping);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBlueUIApp initialization

BOOL CBlueUIApp::InitInstance()
{
	CString strConfigFile = GetPlatRootPath();
	strConfigFile += "conf\\main_cn.xml";
	m_xmlPlat.Open(strConfigFile);

	CString strLicensePluginPath = "";

	CString strAppMutex = m_xmlPlat.GetNodeText("application\\appMutex");
	::CreateMutex(NULL, FALSE, strAppMutex);//_T("##SCRIPT.NETV4##"));
	if(ERROR_ALREADY_EXISTS == GetLastError() || ERROR_ACCESS_DENIED == GetLastError())
	{
		InterprocessCommunication();

		return FALSE; // Here we quit this application
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// ��ʼ��COM��
	::CoInitialize(NULL);

	AfxInitRichEdit();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	CString strRegCompany = m_xmlPlat.GetNodeText("application\\RegCompany");
	SetRegistryKey(strRegCompany);//_T("Blueant"));
	CString strRegProfile = m_xmlPlat.GetNodeText("application\\RegProfile");
	m_pszProfileName = _tcsdup(strRegProfile);//"BlueUI");

	m_xmlPlat.Close();

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// ��ȡ��ǰ���Բ���������
	int nCurrentLanguage = GetProfileInt(REG_VERSION_SUBKEY, REG_REG_LANGUAGE, ::GetSystemDefaultLangID());
	nCurrentLanguage = LANGUAGE_PAGE_CHINESE;
	if(LANGUAGE_PAGE_CHINESE == nCurrentLanguage)
	{
		SetLocale(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
				"chinese-simplified.2052", FALSE);
	}else
	{
		SetLocale(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				"English.1252", FALSE);
	}

	strConfigFile = GetPlatRootPath();
	if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
	{
		if(strLicensePluginPath == "")
		{
			strConfigFile += "conf\\main_cn.xml";
		}else
		{
			strConfigFile = strLicensePluginPath + "\\main_cn.xml";
		}
	}else
	{
		if(strLicensePluginPath == "")
		{
			strConfigFile += "conf\\main.xml";
		}else
		{
			strConfigFile = strLicensePluginPath + "\\main.xml";
		}
	}
	m_xmlPlat.Open(strConfigFile);

	// ��ʼ��MiniDump
	CString strPlatVersion = "";
	IPlatUI* pIPlatUI = GetIPlatUI();
	if(pIPlatUI)
	{
		strPlatVersion = pIPlatUI->GetPlatVersion();
	}
	theCrashDumper.Enable(m_xmlPlat.GetNodeText("application\\appName") + "_" + strPlatVersion, true);

	// ��ȡ���̱�ʶ
	m_strProcessId = m_xmlPlat.GetNodeText("application\\Process");

	// ��������,��Ϊ����һ����0������
	//int a = 10;
	//float b = a / 0;

	// Splash
	CString strSplashEnable = m_xmlPlat.GetNodeAttribute("application\\Splash", "enable");
	if(strSplashEnable == "true")
	{
		CSplashWnd::EnableSplashScreen();
		CSplashWnd::ShowSplashScreen();
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_BLUEUITYPE,
		RUNTIME_CLASS(CBlueUIDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CBlueUIView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// ���ô��ڱ���
	CString strAppName = m_xmlPlat.GetNodeText("application\\appName");
	pMainFrame->SetTitle(strAppName);

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// ��ȡȱʡ�ı༭��ģ����
	m_strDefaultEditor = m_xmlPlat.GetNodeText("Editor\\Default");

#ifndef _DEBUG
	/////////////////////////////////////////////////////////////
	// Regist Shell File Type
	DOMNode* pNode = m_xmlPlat.GetNode("shell\\File");
	while (pNode != NULL)
	{
		CString strFileExtension = m_xmlPlat.GetNodeAttribute(pNode, "Extension");
		CString strDocClassName = m_xmlPlat.GetNodeAttribute(pNode, "Class");
		if(strFileExtension != "")
		{

			CGCFileTypeAccess TheFTA;
			TheFTA.SetApplicationName(strAppName);

			TCHAR	szProgPath[MAX_PATH * 2];
			::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath)/sizeof(TCHAR));

			CString csTempText;

			// ע��Script.NET�����ļ���׺
			TheFTA.SetExtension(strFileExtension);//"snp");

			// just pass file path in quotes on command line
			csTempText  = szProgPath;
			csTempText += " \"%1\"";
			TheFTA.SetShellOpenCommand(csTempText);
			TheFTA.SetDocumentShellOpenCommand(csTempText);

			TheFTA.SetDocumentClassName(strDocClassName);//"snp.Document");

			// use first icon in program
			csTempText  = szProgPath;
			csTempText += ",0";
			TheFTA.SetDocumentDefaultIcon(csTempText);
			
			TheFTA.RegSetAllInfo();
		}

		pNode = m_xmlPlat.GetNextNode(pNode, "File");
	}
	/////////////////////////////////////////////////////////////
#endif

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CString strShellFileName = "";
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
		strShellFileName = cmdInfo.m_strFileName;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindowEx(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// ���ð�װ·��
	WriteProfileString(REG_PATH_SUBKEY, REG_PATH_INSTALL, GetPlatRootPath());

	// ���ز��
	LoadPlugIns();

	// ������Ϣ�������ʵ��
	m_pIMessageQueue = (IMessageQueue*)(CreateVciObject(VCIID_MESSAGEQUEUE));
	if(m_pIMessageQueue)
	{
		m_pIMessageQueue->SetAppProcessId(m_strProcessId);
	}

	// �Ƿ��Զ�������ʼҳ
	BOOL bAutoOpenStartPage = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_AUTO_OPEN_STARTPAGE, TRUE);
	if(bAutoOpenStartPage)
	{
		CString strStartPage;
		GetIPlatUI()->GetVciParameter("", "StartPage", strStartPage, strStartPage);
		OpenHtmlWindow(strStartPage);
	}

	// ������˸����
	int nTime = atoi(m_xmlPlat.GetNodeAttribute("application\\Splash", "time"));
	if(nTime == 0)
		CSplashWnd::HideSplashScreen();

	int nRegister = GetProfileInt(REG_VERSION_SUBKEY, REG_REGISTER, 0);
	if(nRegister == 0)
	{
		ExecMenuScript("Help\\Register");
	}else
	{
		// ���ر���������ļ�
		BOOL bAutoReloadDocument = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_AUTO_RELOAD_DOCUMENT, TRUE);
		if(bAutoReloadDocument)
		{
			CString strFileWorkspace = GetPlatRootPath();
			strFileWorkspace += "conf\\workspace.session";
			pMainFrame->LoadSessionFile(strFileWorkspace);
		}
	}

	// ������������Ҫ�򿪵��ļ�
	if(strShellFileName != "")
	{
		ShellOpenFile(strShellFileName);
	}else
	if(__argc > 1)	// Win7�¿��ܲ�����ȷ���������в���,��Ϊ�Լ�����
	{
		CStringArray asFile;
		CString strCommand = __targv[1];
		if(strCommand.Find(_T("CMD_")) != 0)
		{
			strCommand = _T("");
			asFile.Add(__targv[1]);
		}
		for(int i=2; i<__argc; i++)
		{
			asFile.Add(__targv[i]);
		}
		if(strCommand != _T(""))
		{
			// ���������в���
			ShellCommand(strCommand, asFile);
		}else
		{
			// û�������в��������ʾҪ���ļ�
			for(int i=0; i<asFile.GetSize(); i++)
			{
				ShellOpenFile(asFile[i]);
			}
		}
	}

	// �жϲ���������ʱ���еĶ�ʽ����
	CString strTimerXmlFile = GetPlatRootPath() + "\\conf\\starttimer.xml";
	if(GetFileAttributes(strTimerXmlFile) != 0xFFFFFFFF)
	{
		CXmlParser xmlTimer;
		xmlTimer.Open(strTimerXmlFile);

		DOMNode* pNode = xmlTimer.GetNode("Task");
		while (pNode != NULL)
		{
			CString strTaskName = xmlTimer.GetNodeAttribute(pNode, "Name");
			CString strInterpId = xmlTimer.GetNodeAttribute(pNode, "Interp");
			CString strInterpName = xmlTimer.GetNodeAttribute(pNode, "InterpName");
			CString strCmdType = xmlTimer.GetNodeAttribute(pNode, "CmdType");
			int nSeconds = atoi(xmlTimer.GetNodeAttribute(pNode, "Seconds"));
			int nCount = atoi(xmlTimer.GetNodeAttribute(pNode, "Count"));
			CString strStartTime = xmlTimer.GetNodeAttribute(pNode, "StartTime");
			CString strEndTime = xmlTimer.GetNodeAttribute(pNode, "strEndTime");
			CString strTrigger = xmlTimer.GetNodeAttribute(pNode, "Trigger");

			CString strCommand = xmlTimer.GetNodeText(pNode);
			strCommand.TrimLeft();
			strCommand.TrimRight();
			if(strCmdType == "file")
			{
				strCommand = "<file>" + strCommand;
			}

			CTimerTask* pTask = AddTimerTask(strTaskName, strInterpId, strInterpName, strCommand, nSeconds, FALSE);
			if(pTask)
			{
				pTask->SetExecCount(nCount);
				//pTask->SetStartTime();
				//pTask->SetEndTime();
				if(strTrigger != "")
				{
					pTask->SetTrigger(strTrigger);
				}
				pTask->StartTask();
			}

			pNode = xmlTimer.GetNextNode(pNode, "Task");
		}
	}

	// ���ع�����Ϣ(�̷߳�ʽ���ػᵼ��ĳЩ���ߴ�������ʱ���쳣,��ʱ�Ȳ����̷߳�ʽ)
	AutoLoadTools("Tools");
	//CreateLoadToolsThread();

	return TRUE;
}

int CBlueUIApp::ExitInstance() 
{
	// ɾ�����ж�ʱ����
	POSITION pos = m_lsTimerTask.GetHeadPosition();
	while(pos != NULL)
	{
		CTimerTask* pItem = m_lsTimerTask.GetAt(pos);

		delete pItem;

		m_lsTimerTask.GetNext(pos);
	}

	::CoUninitialize();

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// ��������
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::SetLocale(LCID Locale, const char* szLocale, BOOL bIsRTL, BOOL bIsTextFile)
{
	if (bIsTextFile)
	{
		CString strPath;
		VERIFY(::GetModuleFileName(AfxGetApp()->m_hInstance, strPath.GetBufferSetLength(_MAX_PATH), _MAX_PATH));
		strPath.ReleaseBuffer();

		int nIndex  = strPath.ReverseFind(_T('\\'));
		if (nIndex > 0) strPath = strPath.Left(nIndex + 1); else strPath.Empty();

		// ��Դ�ļ�·������:res_English.1252.xml
		strPath = strPath + _T("res_") + szLocale + _T(".xml");

		if (!XTPResourceManager()->SetResourceManager(new CXTPResourceManagerXML(strPath)))
			return FALSE;

	} else
	{
		if (!XTPResourceManager()->SetResourceManager(new CXTPResourceManager()))
			return FALSE;
	}

	::SetThreadLocale(MAKELCID(Locale, SORT_DEFAULT));
	::setlocale(LC_ALL, szLocale);

	m_curLanguage = Locale;
	SetRTLLayout(bIsRTL);

	// ���¼���ȱʡ�Ĳ˵�
	ReloadDefaultMenu();

	return TRUE;
}

void CBlueUIApp::ReloadDefaultMenu()
{
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pMainFrame)
	{
		pMainFrame->ResetCommandBars();
	}
}

void SetRTLLayout(CWnd* pWnd, BOOL bRTLLayout)
{
	pWnd->ModifyStyleEx(bRTLLayout? 0: WS_EX_LAYOUTRTL, !bRTLLayout? 0: WS_EX_LAYOUTRTL);

	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (CWnd* pChild = pWnd->GetWindow(GW_CHILD); pChild != NULL;
		pChild = pChild->GetWindow(GW_HWNDNEXT))
	{

		// send to child windows after parent
		SetRTLLayout(pChild, bRTLLayout);
	}
}


void CBlueUIApp::SetRTLLayout(BOOL bRTLLayout)
{
	if ((BYTE)::GetVersion() < 4)
		return;

	if (m_bIsRTL == bRTLLayout)
		return;

	m_bIsRTL = bRTLLayout;


	if (m_pMainWnd)
	{
		::SetRTLLayout(m_pMainWnd, bRTLLayout);

		// then update the state of all floating windows owned by the parent
		HWND hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (hWnd != NULL)
		{
			if (m_pMainWnd->m_hWnd == ::GetParent(hWnd))
				::SetRTLLayout(CWnd::FromHandle(hWnd), bRTLLayout);

			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}
	}

	XTPImageManager()->DrawReverted(bRTLLayout);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CPictureEx	m_Picture;
	//CString		m_strKeyCode;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//m_strKeyCode = _T("B2F975A1-B72D-4281-B7BC-330DD414CBDE");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_Picture);
	//DDX_Text(pDX, IDC_EDIT_KEYCODE, m_strKeyCode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strSplashPicture = theApp.GetPlatRootPath();
	strSplashPicture += "conf\\splash.bmp";
	if (m_Picture.Load(strSplashPicture))
		m_Picture.Draw();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// App command to run the dialog
void CBlueUIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CBlueUIApp message handlers

BOOL CBlueUIApp::PreTranslateMessage(MSG* pMsg) 
{
	CSplashWnd::PreTranslateAppMessage(pMsg);
	return CWinApp::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// ȡƽ̨��Ŀ¼
/////////////////////////////////////////////////////////////////////////////
CString CBlueUIApp::GetPlatRootPath()
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

	return szPath;
}

/////////////////////////////////////////////////////////////////////////////
// ���沢���õ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::SaveAndSetCurPath(CString strURL)
{
	// ��¼��ǰ·��,�����õ�ǰ·��
	char szOldPath[256];
	memset(szOldPath, 0, 256);
	DWORD dwPathLen = GetCurrentDirectory(255, szOldPath);
	m_strOldPath = szOldPath;
	CString strPath = strURL;
	strPath.Replace("/", "\\");
	int nPos = strPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		strPath = strPath.Left(nPos);
	}
	SetCurrentDirectory(strPath);
}

/////////////////////////////////////////////////////////////////////////////
// �ָ�����ĵ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::RestoreCurPath()
{
	// ִ�н���,�ָ���ǰ·��������
	if(m_strOldPath.GetLength() > 0)
	{
		SetCurrentDirectory(m_strOldPath);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�����ļ�xml��ĳ����
/////////////////////////////////////////////////////////////////////////////
CString CBlueUIApp::GetConfText(CString strPath)
{
	return m_xmlPlat.GetNodeText(strPath);
}

/////////////////////////////////////////////////////////////////////////////
// ȡƽ̨UI�ӿ�
/////////////////////////////////////////////////////////////////////////////
IPlatUI* CBlueUIApp::GetIPlatUI()
{
	return (IPlatUI*)(m_PlatUI.ExternalQueryInterface());
}

/////////////////////////////////////////////////////////////////////////////
// ���ز��
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::LoadPlugIns()
{
	// �Զ����ؽ��������
	AutoLoadPlugIns("Interps");

	//�򿪲��XML�ļ�
	CString strXmlFile = theApp.GetPlatRootPath();
	strXmlFile += "plugins\\plugins.xml";

	BOOL bAutoLoadFolder = TRUE;
	if(GetFileAttributes(strXmlFile) != 0xFFFFFFFF)
	{
		// ������plugins.xml�ļ����������ز��
		CString strLicensePlugin = m_xmlPlat.GetNodeText("application\\LicensePlugin");

		CXmlParser parser;
		parser.Open(strXmlFile);
		DOMNode* pNode = parser.GetRootNode();
		pNode = parser.GetChildNode(pNode);
		bAutoLoadFolder = (parser.GetNodeAttribute(pNode, "AutoLoad") == "true");	// �Զ�����Ŀ¼��ʶ
		pNode = parser.GetChildNode(pNode);
		while (pNode != NULL)
		{
			BOOL bEnable = (parser.GetNodeAttribute(pNode, "enable") == "true");
			if(bEnable)
			{
				CString strName = parser.GetNodeText(pNode);
				if(strName.CompareNoCase(strLicensePlugin) == 0)
				{
					// ƽ̨Ĭ��License����ڳ�������ʱ�Ѿ�����,����Ҫ�ٴμ���
					pNode = parser.GetNextNode(pNode);
					continue;
				}
				//CString strAttr = parser.GetNodeAttribute(pNode, "name");
				CString strPath = theApp.GetPlatRootPath();
				strPath += "plugins\\";
				strPath += strName;
				CPlugIn plugin(strPath);
				if(plugin.LoadInfo() && plugin.LoadPlugIn())
				{
					m_aPlugIns.Add(plugin);

					// ���VCI����������Զ�����ʵ��������,�򴴽�ʵ��
					if(plugin.m_bAutoCreate)
					{
						m_aPlugIns[m_aPlugIns.GetSize()-1].CreateObject(plugin.m_strAutoCreateInstanceName);
					}

					// ����������Ҫ�Զ�������DockingPane
					m_aPlugIns[m_aPlugIns.GetSize()-1].LoadDockingPane(-1);
				}
			}

			pNode = parser.GetNextNode(pNode);
		}
		parser.Close();
	}

	if(bAutoLoadFolder)
	{
		// ���û���ҵ�plugins.xml,��plugins.xml��˵���˻�Ҫ�Զ�����Ŀ¼,���Զ�����Ŀ¼����������в��
		AutoLoadPlugIns("Plugins");
	}

	// �Զ����ع��̲��(�Ѿ��ϲ���PluginsĿ¼)
	//AutoLoadPlugIns("PrjPlugins");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ID����PlugIn,���δ���ؾ���PluginsĿ¼�²��Ҳ��,����ҵ��ͼ��ز��
/////////////////////////////////////////////////////////////////////////////
CPlugIn* CBlueUIApp::FindPlugIn(CString strVciId)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		//OUTPUT_ID(1, "--%s\r\n", m_aPlugIns[i].m_strId);
		if(m_aPlugIns[i].m_strId == strVciId)
			return &(m_aPlugIns[i]);
	}

	CFileFind finder;

	CString strWildcard = theApp.GetPlatRootPath();
	strWildcard += _T("plugins\\*.*") ;

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if not a directory, skip
		if (!finder.IsDirectory())
			continue;

		if (finder.GetFileName() != strVciId)
			continue;

		// if it's a directory, and id is same, then load it
		CString strPath = finder.GetFilePath();
		CPlugIn plugin(strPath);
		if(plugin.LoadInfo() && plugin.LoadPlugIn())
		{
			m_aPlugIns.Add(plugin);

			// ����������Ҫ�Զ�������DockingPane
			m_aPlugIns[m_aPlugIns.GetSize()-1].LoadDockingPane(-1);

			finder.Close();
			return &(m_aPlugIns[m_aPlugIns.GetSize()-1]);
		}

		finder.Close();
		return NULL;
	}

	finder.Close();

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ָ����PluginsĿ¼�²������в��,����ҵ��ͼ��ز��
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::AutoLoadPlugIns(CString strPlugInPath)
{
	int nCount = 0;

	//CString strLicensePlugin = m_xmlPlat.GetNodeText("application\\LicensePlugin");
	CString strPlugInBasePath = theApp.GetPlatRootPath();
	strPlugInBasePath += strPlugInPath;

	CFileFind finder;

	CString strWildcard = theApp.GetPlatRootPath();
	strWildcard += strPlugInPath;
	strWildcard += _T("\\*.*") ;

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if not a directory, skip
		if (!finder.IsDirectory())
			continue;

		// if it's a directory, and found plusin.xml, then load it
		CString strPath = finder.GetFilePath();
		if( (GetFileAttributes(strPath+"\\plugin.xml") == 0xFFFFFFFF) &&
			(GetFileAttributes(strPath+"\\plugin_cn.xml") == 0xFFFFFFFF))
			continue;

		/*if(strPath.CompareNoCase(strPlugInBasePath + "\\" + strLicensePlugin) == 0)
		{
			// ƽ̨Ĭ��License����ڳ�������ʱ�Ѿ�����,����Ҫ�ٴμ���
			continue;
		}*/
		// �ж�����Ѿ����ع��Ͳ����ټ���
		BOOL bExist = FALSE;
		for(int i=0; i<m_aPlugIns.GetSize(); i++)
		{
			if(strPath.CompareNoCase(m_aPlugIns[i].m_strPath) == 0)
			{
				bExist = TRUE;
				break;
			}
		}
		if(bExist)
		{
			continue;
		}

		CPlugIn plugin(strPath);
		if(plugin.LoadInfo() && plugin.LoadPlugIn())
		{
			m_aPlugIns.Add(plugin);

			// ���VCI����������Զ�����ʵ��������,�򴴽�ʵ��
			if(plugin.m_bAutoCreate)
			{
				m_aPlugIns[m_aPlugIns.GetSize()-1].CreateObject(plugin.m_strAutoCreateInstanceName);
			}

			// ����������Ҫ�Զ�������DockingPane
			m_aPlugIns[m_aPlugIns.GetSize()-1].LoadDockingPane(-1);

			nCount++;
		}
	}

	finder.Close();

	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWM����ָ��
/////////////////////////////////////////////////////////////////////////////
CPlugIn* CBlueUIApp::GetOWM(UINT nID)
{
	if(nID == 0)
	{
		return NULL;
	}

	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].IsVciOwm() && (m_aPlugIns[i].m_nOwmId == nID))
			return &(m_aPlugIns[i]);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWM����ָ��(������ID)
/////////////////////////////////////////////////////////////////////////////
CPlugIn* CBlueUIApp::GetOWM(CString strOwmId)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].IsVciOwm() && (m_aPlugIns[i].m_strId == strOwmId))
			return &(m_aPlugIns[i]);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWM����id(������ID)
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::GetOWMID(CString strOwmId)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].m_strId == strOwmId)
			return m_aPlugIns[i].m_nOwmId;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��̬OWMָ��,�����OWM��ǰδפ��,�������ж�̬����
/////////////////////////////////////////////////////////////////////////////
CPlugIn* CBlueUIApp::GetDynamicOWM(UINT nID)
{
	CPlugIn* pOWM = GetOWM(nID);
	if(pOWM && !pOWM->HasLoad())
	{
		// �����δ����,�����
		pOWM->LoadPlugIn(TRUE);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��̬OWMָ��,�����OWM��ǰδפ��,�������ж�̬����(������ID)
/////////////////////////////////////////////////////////////////////////////
CPlugIn* CBlueUIApp::GetDynamicOWM(CString strOwmId)
{
	CPlugIn* pOWM = GetOWM(strOwmId);
	if(pOWM && !pOWM->HasLoad())
	{
		// �����δ����,�����
		pOWM->LoadPlugIn(TRUE);
	}else
	if(!pOWM)
	{
		// �����Ƿ�Ϊδע���OWM
		pOWM = FindPlugIn(strOwmId);
		if(pOWM && !pOWM->HasLoad())
		{
			// �����δ����,�����
			pOWM->LoadPlugIn(TRUE);
		}
	}

	return pOWM;
}

/////////////////////////////////////////////////////////////////////////////
// �ж�ָ��OWM�����Ƿ�Ϊ��ǰ�����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::CheckActiveOWM(CString strOwmId, CString strDocumentFile)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].m_strId == strOwmId)
		{
			// ��ȡ����Ŀ��
			CFrameWnd* pActiveFrame = ((CMainFrame*)AfxGetMainWnd())->GetActiveFrame();
			CMultiDocTemplate* pOwmDocTemplate = m_aPlugIns[i].m_pDocTemplate;
			if(pOwmDocTemplate == NULL)
			{
				continue;
			}
			CView* pOwmView = NULL;
			POSITION posdoc = pOwmDocTemplate->GetFirstDocPosition();
			while(posdoc)
			{
				CDocument* pDoc = pOwmDocTemplate->GetNextDoc(posdoc);
				POSITION posview = pDoc->GetFirstViewPosition();
				if(posview)	// ����Ӵ���
				{
					pOwmView = pDoc->GetNextView(posview);	// ȡ����ָ��
					if( (pActiveFrame == pOwmView->GetParentFrame()) &&
						((strDocumentFile == "") || (pDoc->GetPathName() == strDocumentFile)) )
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ����VCI����
/////////////////////////////////////////////////////////////////////////////
void* CBlueUIApp::CreateVciObject(CString strVciId, CString strInstName)
{
	//OUTPUT_ID(1, "##%s\r\n", strVciId);
	// �������
	CPlugIn* pPlugIn = FindPlugIn(strVciId);
	if(pPlugIn)
	{
		return pPlugIn->CreateObject(strInstName);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�VCI����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::ReleaseObject(LPVOID pVciObj, BOOL bForceDelete)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].ReleaseObject(pVciObj, bForceDelete))
			return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�VCI����(ָ��ʵ����)
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::ReleaseObjectByInstanceName(CString strInstName)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].ReleaseObject(strInstName))
			return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ���������(����ʵ����)
/////////////////////////////////////////////////////////////////////////////
void* CBlueUIApp::GetObjectByInstanceName(CString strInstName)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		void* pIObj = m_aPlugIns[i].GetObjectByInstanceName(strInstName);
		if(pIObj != NULL)
		{
			return pIObj;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����������ƽӿ�(�������ʵ���ӿ�ָ��)
/////////////////////////////////////////////////////////////////////////////
void* CBlueUIApp::GetObjectControlPtrByInstancePtr(LPVOID pVciObj)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		void* pIControlObj = m_aPlugIns[i].GetObjectControlPtrByInstancePtr(pVciObj);
		if(pIControlObj != NULL)
		{
			return pIControlObj;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����ID(�������ʵ���ӿ�ָ��)
/////////////////////////////////////////////////////////////////////////////
CString CBlueUIApp::GetPlugInIdByInstancePtr(LPVOID pVciObj)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].IsInstancePtr(pVciObj))
		{
			return m_aPlugIns[i].m_strId;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӷ�ʱ����
/////////////////////////////////////////////////////////////////////////////
CTimerTask* CBlueUIApp::AddTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds, BOOL bStart)
{
	CTimerTask* pTask = FindTimerTask(strTaskName);
	if(pTask != NULL)
	{
		return pTask;
	}

	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	pTask = new CTimerTask(strTaskName, strInterpId, strInterpName, strCommand);
	pTask->SetTaskSeconds(nSeconds);
	if(bStart)
	{
		pTask->StartTask();
	}

	m_lsTimerTask.AddTail(pTask);

	TRACE("Add Script Timer Task: %s\n", strTaskName);

	return pTask;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӷ�ʱ����
/////////////////////////////////////////////////////////////////////////////
CTimerTask* CBlueUIApp::AddTimerTask(CString strTaskName, LPVOID lpFunction, int nSeconds, BOOL bStart)
{
	CTimerTask* pTask = FindTimerTask(strTaskName);
	if(pTask != NULL)
	{
		return pTask;
	}

	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	pTask = new CTimerTask(strTaskName, lpFunction);
	pTask->SetTaskSeconds(nSeconds);
	if(bStart)
	{
		pTask->StartTask();
	}

	m_lsTimerTask.AddTail(pTask);

	TRACE("Add timer task function %s success\n", strTaskName);

	return pTask;
}

/////////////////////////////////////////////////////////////////////////////
// ���Ҷ�ʱ����
/////////////////////////////////////////////////////////////////////////////
CTimerTask* CBlueUIApp::FindTimerTask(CString strTaskName)
{
	if(strTaskName == "")
	{
		return NULL;
	}

	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	POSITION pos = m_lsTimerTask.GetHeadPosition();
	while(pos != NULL)
	{
		CTimerTask* pItem = m_lsTimerTask.GetAt(pos);

		if(pItem->m_strTaskName == strTaskName)
		{
			return pItem;
		}

		m_lsTimerTask.GetNext(pos);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����ʱ����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::DeleteTimerTask(CTimerTask* pTimerTask)
{
	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	POSITION pos = m_lsTimerTask.GetHeadPosition();
	while(pos != NULL)
	{
		CTimerTask* pItem = m_lsTimerTask.GetAt(pos);

		if(pItem == pTimerTask)
		{
			delete pItem;
			m_lsTimerTask.RemoveAt(pos);
			return TRUE;
		}

		m_lsTimerTask.GetNext(pos);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����ʱ����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::DeleteTimerTask(CString strTaskName)
{
	if(strTaskName == "")
	{
		return FALSE;
	}

	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	POSITION pos = m_lsTimerTask.GetHeadPosition();
	while(pos != NULL)
	{
		CTimerTask* pItem = m_lsTimerTask.GetAt(pos);

		if(pItem->m_strTaskName == strTaskName)
		{
			delete pItem;
			m_lsTimerTask.RemoveAt(pos);
			return TRUE;
		}

		m_lsTimerTask.GetNext(pos);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ж�ʱ����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::RunTimerTask()
{
	CSingleLock SingleLock(&m_MutexTimerTask);
	SingleLock.Lock();

	POSITION pos = m_lsTimerTask.GetHeadPosition();
	while(pos != NULL)
	{
		CTimerTask* pItem = m_lsTimerTask.GetAt(pos);

		POSITION posDelete = NULL;

		if(pItem)
		{
			int nStatus = pItem->CheckStatus();
			if((nStatus != TIMER_STATE_END) && (nStatus != TIMER_STATE_STOP))
			{
				//TRACE("Run Timer Task: %s\n", pItem->m_strTaskName);
				pItem->RunTask();
			}

			if(pItem->GetStatus() == TIMER_STATE_END)
			{
				TRACE("Delete Timer Task: %s\n", pItem->m_strTaskName);
				delete pItem;
				posDelete = pos;
			}
		}

		m_lsTimerTask.GetNext(pos);

		if(posDelete != NULL)
		{
			m_lsTimerTask.RemoveAt(posDelete);
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ָ����ToolsĿ¼�²������й���,����ҵ��ͼ�����Ϣ
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::AutoLoadTools(CString strToolPath)
{
	int nCount = 0;

	CFileFind finder;

	CString strWildcard = theApp.GetPlatRootPath();
	strWildcard += strToolPath;
	strWildcard += _T("\\*.*") ;

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if not a directory, skip
		if (!finder.IsDirectory())
			continue;

		// if it's a directory, and found plusin.xml, then load it
		CString strPath = finder.GetFilePath();
		if( (GetFileAttributes(strPath+"\\tool.xml") == 0xFFFFFFFF) &&
			(GetFileAttributes(strPath+"\\tool_cn.xml") == 0xFFFFFFFF))
			continue;

		CTool tool(strPath);
		if(tool.LoadInfo())
		{
			m_aTools.Add(tool);

			// ����������Ҫ�Զ�������DockingPane
			m_aTools[m_aTools.GetSize()-1].LoadDockingPane(-1);

			nCount++;
		}
	}

	finder.Close();

	// ��ʼ�����й���
	for(int i=0; i<m_aTools.GetSize(); i++)
	{
		m_aTools[i].Init();
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////
// ����������Ϣ�����߳�
//////////////////////////////////////////////////////////////////
ULONG _stdcall _CreateLoadToolsThread(LPVOID pParam)
{
	CBlueUIApp* pApp = (CBlueUIApp*)pParam;
	pApp->AutoLoadTools("Tools");
	AfxEndThread(10);
	return 10;
}

/////////////////////////////////////////////////////////////////////////////
// �̷߳�ʽ�������й�����Ϣ
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::CreateLoadToolsThread()
{
	HANDLE	hThread;
	DWORD dwThreadID;
	hThread = ::CreateThread(NULL
							  ,0
							  ,_CreateLoadToolsThread
							  ,(LPVOID)(this)
							  ,0
							  ,&dwThreadID
							  );
}

/////////////////////////////////////////////////////////////////////////////
// ƽ̨COM�ӿ���Ϣ������
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::PlatComCmd(WPARAM wParam, LPARAM lParam)
{
	//int nWs = wParam;
	//CString strOwmId = (LPCTSTR)wParam;
	STRU_COMCMD* pComCmd = (STRU_COMCMD*)lParam;
	if(pComCmd == NULL)
	{
		return;
	}
	CString strOwmId = pComCmd->strOwmId;
	int ncCmd = pComCmd->nCmd;
	WPARAM wcParam = pComCmd->wParam;
	LPARAM lcParam = pComCmd->lParam;
	//if((0 == nWs) || (-1 == nWs))
	if(("" == strOwmId) || ("0" == strOwmId) || ("-1" == strOwmId))
	{
		// �Ƿ���ƽ̨������ĳ��OWM��
		int nRet = CommandPlat(ncCmd, wcParam, lcParam);
		if(pComCmd->lpResult != NULL)	// ��䷵�ؽ��
		{
			int* pnRet = (int*)(pComCmd->lpResult);
			*pnRet = nRet;
		}
	}else
	{
		CPlugIn* pOWM = GetDynamicOWM(strOwmId);
		if(pOWM != NULL)
		{
			IVciOwm* pIVciOwm = pOWM->GetIVciOwm();
			if(pIVciOwm)
			{
				int nRet = pIVciOwm->Command(ncCmd, wcParam, lcParam);
				if(pComCmd->lpResult != NULL)	// ��䷵�ؽ��
				{
					int* pnRet = (int*)(pComCmd->lpResult);
					*pnRet = nRet;
				}
			}
		}
	}

	if(pComCmd->nDataLen > 0)	// ɾ��Я��������
	{
		delete ((BYTE*)(pComCmd->lParam));
		pComCmd->lParam = NULL;
	}

	if(pComCmd->bPost)	// �����PostMessage��ɾ����Ϣ�ṹ
	{
		delete pComCmd;
		pComCmd = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���������в���
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::ShellCommand(CString strCommand, CStringArray& asFile)
{
	int nPos = strCommand.Find(_T("CMD_"));
	if(nPos != 0)
	{
		return FALSE;
	}

	strCommand.Delete(0, nPos+4);
	int nCmdID = atoi(strCommand);

	int i;
	CString strParams = _T("?files=");
	for(i=0; i<asFile.GetSize(); i++)
	{
		strParams += asFile[i];
		strParams += _T(",");
	}

	// ���Ҳ˵�ID��Ӧ�Ķ���,�ڲ����Ribbon��PopupMenu�в���
	/*for(i=0; i<m_aPlugIns.GetSize(); i++)
	{
		for(int j=0; j<m_aPlugIns[i].m_arRibbonTab.GetSize(); j++)
		{
			for(int k=0; k<m_aPlugIns[i].m_arRibbonTab[j].m_arVciRibbonGroup.GetSize(); k++)
			{
				for(int m=0; m<m_aPlugIns[i].m_arRibbonTab[j].m_arVciRibbonGroup[k].m_arVciRibbonControl.GetSize(); m++)
				{
					if(m_aPlugIns[i].m_arRibbonTab[j].m_arVciRibbonGroup[k].m_arVciRibbonControl[m].m_nId == nCmdID)
					{
						if(m_aPlugIns[i].m_arRibbonTab[j].m_arVciRibbonGroup[k].m_arVciRibbonControl[m].m_strAction == _T("htmlview"))
						{
							ShowUIPage(m_aPlugIns[i].m_arRibbonTab[j].m_arVciRibbonGroup[k].m_arVciRibbonControl[m].m_strScript + strParams);
							return TRUE;
						}
					}
				}
			}
		}
	}

	for(i=0; i<m_aPlugIns.GetSize(); i++)
	{
		for(int j=0; j<m_aPlugIns[i].m_arPopupMenu.GetSize(); j++)
		{
			if(m_aPlugIns[i].m_arPopupMenu[j].m_nId == nCmdID)
			{
				if(m_aPlugIns[i].m_arPopupMenu[j].m_strAction == _T("htmlview"))
				{
					ShowUIPage(m_aPlugIns[i].m_arPopupMenu[j].m_strScript + strParams);
					return TRUE;
				}
			}
		}
	}*/

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ���������еĴ��ļ�����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::ShellOpenFile(CString strFileName)
{
	CDocument* pDoc = OpenDocumentFile(strFileName);
	return (NULL != pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// ���ĵ�(����)
/////////////////////////////////////////////////////////////////////////////
CDocument* CBlueUIApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	if(lpszFileName == NULL)
	{
		CPlugIn* pOwmEdit = GetDynamicOWM(m_strDefaultEditor);//"OwmEdit";
		if(pOwmEdit && pOwmEdit->m_pDocTemplate)
		{
			return pOwmEdit->m_pDocTemplate->OpenDocumentFile(NULL);
		}
	}

	// ��Recent Fileʱ����ô˺���
	if( (lpszFileName == "") ||
		(GetFileAttributes(lpszFileName) == 0xFFFFFFFF))
	{
		return NULL;
	}

	// ����OWM������չ��,���ĸ�OWM���Դ򿪴��ĵ�
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(!m_aPlugIns[i].IsVciOwm())
			continue;

		CDocument* pDoc = m_aPlugIns[i].OpenDocument(lpszFileName);
		if(pDoc)
		{
			return pDoc;
		}
	}
/*
	// �����Ƿ񹤳��ļ�
	CString strFileName = lpszFileName;
	strFileName.MakeLower();
	int nPos = strFileName.ReverseFind('.');
	if(nPos != -1)
	{
		CString strFileExt = strFileName;
		strFileExt.Delete(0, nPos+1);
		for(int i=0; i<m_aPlugIns.GetSize(); i++)
		{
			if(m_aPlugIns[i].m_strType != "project")
				continue;

			for(int j=0; j<m_aPlugIns[i].m_arDocExtend.GetSize(); j++)
			{
				for(int k=0; k<m_aPlugIns[i].m_arDocExtend[j].m_arDocExt.GetSize(); k++)
				{
					if(strFileExt == m_aPlugIns[i].m_arDocExtend[j].m_arDocExt[k])
					{
						if(OpenProjectFile(&(m_aPlugIns[i]), lpszFileName))
						{
							return NULL;
						}
					}
				}
			}
		}
	}
*/
	// ���û���ҵ����Դ��ĵ���ģ��,����ȱʡ�༭����
	CPlugIn* pOwmEdit = GetDynamicOWM(m_strDefaultEditor);
	if(pOwmEdit && pOwmEdit->m_pDocTemplate)
	{
		return pOwmEdit->m_pDocTemplate->OpenDocumentFile(lpszFileName);
	}

	// ��Script.NET����
	//OpenProject(lpszFileName);

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ���ĵ�(ָ��������OWMģ��)
/////////////////////////////////////////////////////////////////////////////
CDocument* CBlueUIApp::OpenDocumentFileWithOwm(LPCTSTR lpszFileName, LPCTSTR lpszOwmId) 
{
	CString strOwmId = lpszOwmId;
	if(strOwmId == "")
	{
		if(strcmp(lpszFileName, "<null>") == 0)
		{
			return OpenDocumentFile(NULL);
		}else
		{
			return OpenDocumentFile(lpszFileName);
		}
	}

	if( (lpszFileName == NULL) ||
		(strcmp(lpszFileName, "<null>") == 0) )
	{
		CPlugIn* pOwm = GetDynamicOWM(strOwmId);
		if(pOwm && pOwm->m_pDocTemplate)
		{
			return pOwm->m_pDocTemplate->OpenDocumentFile(NULL);
		}
	}

	if( (lpszFileName == "") ||
		(GetFileAttributes(lpszFileName) == 0xFFFFFFFF))
	{
		return NULL;
	}

	CString strFilePath = lpszFileName;
	strFilePath.Replace("/", "\\");
	strFilePath.Replace("\\.\\", "\\");	// �����õ�·��ȥ��

	CPlugIn* pOwm = GetDynamicOWM(strOwmId);
	if(pOwm && pOwm->m_pDocTemplate)
	{
		// ��ֹ���Ѿ��򿪵��ĵ�,�������һ���Ƿ��Ѿ�����,����Ǿ�ֱ�Ӽ���
		POSITION posdoc = pOwm->m_pDocTemplate->GetFirstDocPosition();
		while(posdoc)
		{
			CDocument* pDoc = pOwm->m_pDocTemplate->GetNextDoc(posdoc);
			CString strPathName = pDoc->GetPathName();
			strPathName.MakeUpper();
			strFilePath.MakeUpper();
			if(strFilePath == strPathName)
			{
				POSITION posview = pDoc->GetFirstViewPosition();
				if(posview)
				{
					CView* pView = pDoc->GetNextView(posview);
					pView->GetParentFrame()->ActivateFrame(); // ������
				}
				return pDoc;
			}
		}

		return pOwm->m_pDocTemplate->OpenDocumentFile(lpszFileName);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��Html����
/////////////////////////////////////////////////////////////////////////////
CDocument* CBlueUIApp::OpenHtmlWindow(CString strUrl, BOOL bActivate, BOOL bReplaceBlank)
{
	CPlugIn* pOwm = GetDynamicOWM(VCIID_HTMLVIEW);
	if((pOwm == NULL) || (pOwm->m_pDocTemplate == NULL))
	{
		return NULL;
	}

	if (bReplaceBlank)
	{
		CChildFrame* pFrame = (CChildFrame*)((CMainFrame*)AfxGetMainWnd())->MDIGetActive();

		if(pFrame)
		{
			CView* pActiveView = pFrame->GetActiveView();

			POSITION posdoc = pOwm->m_pDocTemplate->GetFirstDocPosition();
			while(posdoc)
			{
				CDocument* pDoc = pOwm->m_pDocTemplate->GetNextDoc(posdoc);
				POSITION posview = pDoc->GetFirstViewPosition();
				if(posview)
				{
					CView* pView = pDoc->GetNextView(posview);
					if(pView->GetParentFrame()->GetActiveView() == pActiveView)
					{
						COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strUrl, (int)pActiveView);
						return pDoc;
					}
				}
			}
		}
		return NULL;
	}
	
	//COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strUrl, 0);
	//return NULL;
	pOwm->m_pDocTemplate->OpenDocumentFile(NULL);
	CChildFrame* pFrame = (CChildFrame*)((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
	if(pFrame)
	{
		CView* pActiveView = pFrame->GetActiveView();
		if(pActiveView)
		{
			COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strUrl, (int)pActiveView);
			return NULL;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// �½��ļ�
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnFileNew() 
{
	OpenDocumentFile(NULL);
}

void CBlueUIApp::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	
}

/////////////////////////////////////////////////////////////////////////////
// ���ļ�
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnFileOpen() 
{
	CString strFilter;
	CString strTemp;

	CString strDefaultExt = GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_DEFFILEEXT, "");
	strDefaultExt.MakeLower();
	BOOL bFoundDefaultExt = FALSE;

	//strFilter = "Script.NET project file (*.snp)|*.snp|";
	// ����OWM������չ��,���ĸ�OWM���Դ򿪴��ĵ�
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(!m_aPlugIns[i].IsVciOwm())
			continue;

		for(int j=0; j<m_aPlugIns[i].m_arDocExtend.GetSize(); j++)
		{
			BOOL bDefaultExt = FALSE;
			CString strTemp1;
			for(int k=0; k<m_aPlugIns[i].m_arDocExtend[j].m_arDocExt.GetSize(); k++)
			{
				strTemp1 += "*.";
				strTemp1 += m_aPlugIns[i].m_arDocExtend[j].m_arDocExt[k];
				if(k != (m_aPlugIns[i].m_arDocExtend[j].m_arDocExt.GetSize()-1))
				{
					strTemp1 += ";";
				}

				CString strDocExt = m_aPlugIns[i].m_arDocExtend[j].m_arDocExt[k];
				strDocExt.MakeLower();
				if(strDefaultExt == strDocExt)
				{
					// ���ϴα���ĺ�׺��
					bDefaultExt = TRUE;
				}
			}

			strTemp.Format("%s (%s)|%s|", m_aPlugIns[i].m_arDocExtend[j].m_strDocName, strTemp1, strTemp1);
			if(bDefaultExt && !bFoundDefaultExt)
			{
				strFilter = strTemp + strFilter;
				bFoundDefaultExt = TRUE;
			}else
			{				
				strFilter += strTemp;
			}
		}
	}
	strFilter += "All files(*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,	strFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString strExt = dlg.GetFileExt();
		WriteProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_DEFFILEEXT, strExt);
		OpenDocumentFile(dlg.GetPathName());
	}
}

void CBlueUIApp::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	
}

/////////////////////////////////////////////////////////////////////////////
// ʹ����򿪵��ļ������Ӳ˵���
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnUpdateFileMruFile1(CCmdUI* pCmdUI) 
{
	if(pCmdUI->m_nIndex == 0)
	{
		OnUpdateRecentFileMenu(pCmdUI);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ϵͳ����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnOption() 
{
	// ��ʾFlash����
	FlashWin FlashDlg;
	FlashDlg.Create(IDD_DIALOGFLASH);
	FlashDlg.ShowWindow(SW_SHOW);
	FlashDlg.CenterWindow();
	FlashDlg.Invalidate(TRUE);

	CTreePropertySheet tps(150);

	// ��License����л�ȡƽ̨���ڶԻ���
	CDialog* pLicenseTpsPlatform = NULL;
	CDialog* pLicenseTpsOwm = NULL;
	CDialog* pLicenseTpsInterp = NULL;
	CString strLicensePlugin = m_xmlPlat.GetNodeText("application\\LicensePlugin");
	CPlugIn* pLicensePlugIn = FindPlugIn(strLicensePlugin);
	if(pLicensePlugIn)
	{
		if(!(pLicensePlugIn->HasLoad()))
		{
			pLicensePlugIn->LoadPlugIn(TRUE);
		}
		CVciPropertyPageArray aLicensePropertyPage;
		pLicensePlugIn->RegisterPropertyPage(aLicensePropertyPage);
		pLicensePlugIn->RegisterPropertyPage(aLicensePropertyPage);
		pLicensePlugIn->RegisterPropertyPage(aLicensePropertyPage);
		if(aLicensePropertyPage.GetSize() > 2)
		{
			pLicenseTpsPlatform = (CDialog*)(aLicensePropertyPage[0].pPage);
			pLicenseTpsOwm = (CDialog*)(aLicensePropertyPage[1].pPage);
			pLicenseTpsInterp = (CDialog*)(aLicensePropertyPage[2].pPage);
		}
	}

	// ƽ̨����
	PLAT_LOADSTRING(strtpsPlatform, IDS_TPS_PLATFORM); // ƽ̨
	PLAT_LOADSTRING(strtpsAboutPlatform, IDS_TPS_ABOUT_PLATFORM); // ����ƽ̨ ...
	if(pLicenseTpsPlatform)
	{
		tps.AddPage(strtpsPlatform,tps_item_branch,-1,strtpsAboutPlatform,pLicenseTpsPlatform,OPT_PLATFORM,TRUE);
	}else
	{
		tps.AddPage(strtpsPlatform,tps_item_branch,IDD_TPS_PLATFORM,strtpsAboutPlatform,NULL,OPT_PLATFORM);
	}

	if(m_xmlPlat.GetNodeAttribute("options\\Language", "enable") != "false")
	{
		CTpsLanguage* pTpsLanguage = new CTpsLanguage();
		PLAT_LOADSTRING(strtpsLanguage, IDS_TPS_LANGUAGE); // ��������
		PLAT_LOADSTRING(strtpsAboutLanguage, IDS_TPS_ABOUT_LANGUAGE); // ����ƽ̨����...
		tps.AddPage(strtpsLanguage,tps_item_node,IDD_TPS_LANGUAGE,strtpsAboutLanguage,pTpsLanguage,OPT_NODE);
	}

	if(m_xmlPlat.GetNodeAttribute("options\\UIStyle", "enable") != "false")
	{
		CTpsUIStyle* pTpsUIStyle = new CTpsUIStyle();
		PLAT_LOADSTRING(strtpsUIStyle, IDS_TPS_UISTYLE); // ������
		PLAT_LOADSTRING(strtpsAboutUIStyle, IDS_TPS_ABOUT_UISTYLE); // ���ý�����...
		tps.AddPage(strtpsUIStyle,tps_item_node,IDD_TPS_UISTYLE,strtpsAboutUIStyle,pTpsUIStyle,OPT_NODE);
	}

	if(m_xmlPlat.GetNodeAttribute("options\\Font", "enable") != "false")
	{
		CTpsFont* pTpsFont = new CTpsFont();
		PLAT_LOADSTRING(strtpsFont, IDS_TPS_FONT); // ��������
		PLAT_LOADSTRING(strtpsAboutFont, IDS_TPS_ABOUT_FONT); // ����ƽ̨�������ɫ...
		tps.AddPage(strtpsFont,tps_item_node,IDD_TPS_FONT,strtpsAboutFont,pTpsFont,OPT_NODE);
	}

	if(m_xmlPlat.GetNodeAttribute("options\\Associate", "enable") != "false")
	{
		CTpsAssociate* pTpsAssociate = new CTpsAssociate();
		PLAT_LOADSTRING(strtpsAssociate, IDS_TPS_ASSOCIATE); // �ļ�����
		PLAT_LOADSTRING(strtpsAboutAssociate, IDS_TPS_ABOUT_ASSOCIATE); // �����ļ�����...
		tps.AddPage(strtpsAssociate,tps_item_node,IDD_TPS_ASSOCIATE,strtpsAboutAssociate,pTpsAssociate,OPT_NODE);
	}

	if(m_xmlPlat.GetNodeAttribute("options\\Workspace", "enable") != "false")
	{
		CTpsWorkspace* pTpsWorkspace = new CTpsWorkspace();
		PLAT_LOADSTRING(strtpsWorkspace, IDS_TPS_WORKSPACE); // ����������
		PLAT_LOADSTRING(strtpsAboutWorkspace, IDS_TPS_ABOUT_WORKSPACE); // ���ù����ռ�...
		tps.AddPage(strtpsWorkspace,tps_item_node,IDD_TPS_WORKSPACE,strtpsAboutWorkspace,pTpsWorkspace,OPT_NODE);
	}

	tps.SetLastPageType(tps_item_endbranch);

	// OWMģ������
	PLAT_LOADSTRING(strtpsOwm, IDS_TPS_OWM); // ��չģ��
	PLAT_LOADSTRING(strtpsAboutOwm, IDS_TPS_ABOUT_OWM); // ������չģ�� ...
	if(pLicenseTpsOwm)
	{
		tps.AddPage(strtpsOwm,tps_item_branch,-1,strtpsAboutOwm,pLicenseTpsOwm,OPT_OWM,TRUE);
	}else
	{
		tps.AddPage(strtpsOwm,tps_item_branch,IDD_TPS_PLATFORM,strtpsAboutOwm,NULL,OPT_OWM);
	}

	CDWordArray aOWMDynUnTsrs;	// ���ڶ�̬ж��״̬��OWM�б�
	// ע��OWM����ҳ
	COWMOptionArray aOWMOptions;	// OWM����ҳ�б�
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(!m_aPlugIns[i].IsVciOwm())
		{
			continue;
		}

		// ��ģ���Ƿ�������ʾ����ҳ
		if((m_aPlugIns[i].m_nType & OWM_OPTION) == 0)
		{
			continue;
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			// ������ڶ�̬ж��״̬����ʱ����,�������ٻָ�
			m_aPlugIns[i].LoadPlugIn(TRUE);
			aOWMDynUnTsrs.Add(i);
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			continue;
		}

		int oldcount = aOWMOptions.GetSize();
		m_aPlugIns[i].RegisterOption(aOWMOptions);
		int count = aOWMOptions.GetSize() - oldcount;
		if(count > 0)
		{
			for(int j=0; j<count; j++)
			{
				int index = oldcount+j;
				CString strNode = aOWMOptions[index].strNode;
				CString strDesc = aOWMOptions[index].strDesc;
				CDialog* pDialog = (CDialog*)(aOWMOptions[index].pOptionPage);
				int nType;
				if(count == 1)
					nType = tps_item_node;
				else
				{
					nType = tps_item_node;
					if(j == 0)
						nType = tps_item_branch;
					if(j+1 == count)
						nType = tps_item_endbranch;
				}
				tps.AddPage(strNode,nType,-1,strDesc,pDialog,OPT_NODE,TRUE);
			}
		}
	}

	tps.SetLastPageType(tps_item_endbranch);

	// ����������
	PLAT_LOADSTRING(strtpsInterp, IDS_TPS_INTERP); // ������
	PLAT_LOADSTRING(strtpsAboutInterp, IDS_TPS_ABOUT_INTERP); // ���ڽ����� ...
	if(pLicenseTpsInterp)
	{
		tps.AddPage(strtpsInterp,tps_item_branch,-1,strtpsAboutInterp,pLicenseTpsInterp,OPT_INTERP,TRUE);
	}else
	{
		tps.AddPage(strtpsInterp,tps_item_branch,IDD_TPS_PLATFORM,strtpsAboutInterp,NULL,OPT_INTERP);
	}

	// �ض����������
	CTpsInterpDirect* pTpsInterpDirect = new CTpsInterpDirect();
	PLAT_LOADSTRING(strtpsInterpDirect, IDS_TPS_INTERP_DIRECT); // ����ض���
	PLAT_LOADSTRING(strtpsAboutInterpDirect, IDS_TPS_ABOUT_INTERP_DIRECT); // ��������ض������...
	tps.AddPage(strtpsInterpDirect,tps_item_node,IDD_TPS_INTERP_DIRECT,strtpsAboutInterpDirect,pTpsInterpDirect,OPT_NODE);

	CDWordArray aInterpDynUnTsrs;	// ���ڶ�̬ж��״̬�Ľ������б�
	// ע�����������ҳ
	CVciPropertyPageArray aInterpPropertyPage;	// ����������ҳ�б�
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(!m_aPlugIns[i].IsVciInterp())
		{
			continue;
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			// ������ڶ�̬ж��״̬����ʱ����,�������ٻָ�
			m_aPlugIns[i].LoadPlugIn(TRUE);
			aInterpDynUnTsrs.Add(i);
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			continue;
		}

		int oldcount = aInterpPropertyPage.GetSize();
		m_aPlugIns[i].RegisterPropertyPage(aInterpPropertyPage);
		int count = aInterpPropertyPage.GetSize() - oldcount;
		if(count > 0)
		{
			for(int j=0; j<count; j++)
			{
				int index = oldcount+j;
				CString strNode = aInterpPropertyPage[index].strNode;
				CString strDesc = aInterpPropertyPage[index].strDesc;
				CDialog* pDialog = (CDialog*)(aInterpPropertyPage[index].pPage);
				int nType;
				if(count == 1)
					nType = tps_item_node;
				else
				{
					nType = tps_item_node;
					if(j == 0)
						nType = tps_item_branch;
					if(j+1 == count)
						nType = tps_item_endbranch;
				}
				tps.AddPage(strNode,nType,-1,strDesc,pDialog,OPT_NODE,TRUE);
			}
		}
	}

	tps.SetLastPageType(tps_item_endbranch);

	// �������
	CTpsVci* pTpsVci = new CTpsVci();
	PLAT_LOADSTRING(strtpsVci, IDS_TPS_VCI); // VCI���
	PLAT_LOADSTRING(strtpsAboutVci, IDS_TPS_ABOUT_VCI); // ����VCI��� ...
	tps.AddPage(strtpsVci,tps_item_branch,IDD_TPS_VCI,strtpsAboutVci,pTpsVci,OPT_VCI);

	CDWordArray aVciDynUnTsrs;	// ���ڶ�̬ж��״̬�Ľ������б�
	// ע��VCI�������ҳ
	CVciPropertyPageArray aVciPropertyPage;	// VCI�������ҳ�б�
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(m_aPlugIns[i].IsVciOwm() || m_aPlugIns[i].IsVciInterp() || m_aPlugIns[i].IsVciLicense())
		{
			continue;
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			// ������ڶ�̬ж��״̬����ʱ����,�������ٻָ�
			m_aPlugIns[i].LoadPlugIn(TRUE);
			aVciDynUnTsrs.Add(i);
		}

		if(!m_aPlugIns[i].HasLoad())
		{
			continue;
		}

		int oldcount = aVciPropertyPage.GetSize();
		m_aPlugIns[i].RegisterPropertyPage(aVciPropertyPage);
		int count = aVciPropertyPage.GetSize() - oldcount;
		if(count > 0)
		{
			for(int j=0; j<count; j++)
			{
				int index = oldcount+j;
				if((j == 0) && (aVciPropertyPage[index].strParent == "[]"))
				{
					break;
				}
				CString strNode = aVciPropertyPage[index].strNode;
				CString strDesc = aVciPropertyPage[index].strDesc;
				CDialog* pDialog = (CDialog*)(aVciPropertyPage[index].pPage);
				int nType;
				if(count == 1)
					nType = tps_item_node;
				else
				{
					nType = tps_item_node;
					if(j == 0)
						nType = tps_item_branch;
					if(j+1 == count)
						nType = tps_item_endbranch;
				}
				tps.AddPage(strNode,nType,-1,strDesc,pDialog,OPT_NODE,TRUE);
			}
		}
	}

	tps.SetLastPageType(tps_item_endbranch);

	for(int i=0; i<aVciPropertyPage.GetSize(); i++)
	{
		CString strParent = aVciPropertyPage[i].strParent;
		if(strParent.Find("[]") != 0)
		{
			continue;
		}
		strParent.Delete(0, 2);
		CString strNode = aVciPropertyPage[i].strNode;
		CString strDesc = aVciPropertyPage[i].strDesc;
		CDialog* pDialog = (CDialog*)(aVciPropertyPage[i].pPage);
		int nType;
		nType = tps_item_node;
		if(strParent == "")
		{
			nType = tps_item_branch;
		}
		tps.AddPage(strNode,nType,-1,strDesc,pDialog,OPT_NODE,TRUE);
	}

	tps.SetLastPageType(tps_item_endbranch);


	tps.SetFancy(FALSE);
	//tps.SetLikeNetscape(FALSE);

	// ����Flash����
	FlashDlg.ShowWindow(SW_HIDE);

	// ִ���������öԻ���(ע:��ԭ���м��������ȥ����,��Ϊ�˴������м��������Ѿ�������)
	tps.DoModal();

	// �ͷ���Դ,��ֹ������Ĳ���������������������
	tps.RemoveResource();

	// �ָ���̬ж�ص�OWMģ�鵽��̬ж��״̬
	for(int i=0; i< aOWMDynUnTsrs.GetSize(); i++)
	{
		m_aPlugIns[aOWMDynUnTsrs[i]].CloseVci();
	}
}

void CBlueUIApp::OnUpdateOption(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ���������
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnVciManager() 
{
	ExecMenuScript("Tools\\PluginManager");
}

void CBlueUIApp::OnUpdateVciManager(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��·���ַ������д���
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::ProcessPath(CString& strPath, CString strPlugInId)
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
	if((strPath.Find(_T("PLUGIN://")) != -1) && (strPlugInId != ""))
	{
		int nPos  = strPath.Find(_T("PLUGIN://"));
		strTemp = strPath.Right(strPath.GetLength()-9-nPos);
		strTemp.Replace("/", "\\");
		if(strTemp.Find(_T("\\")) == 0)
		{	// GetPlatRootPath�Ѿ�������\,��˿���ȥ��
			strTemp = strTemp.Right(strTemp.GetLength()-1);
		}
		strPath = strPath.Left(nPos) + GetPlatRootPath() + "Plugins\\" + strPlugInId + "\\" + strTemp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��xml�����ļ��в��Ҳ�ִ�нű�(��Ӧĳ���˵�����)
/////////////////////////////////////////////////////////////////////////////
CString CBlueUIApp::ExecMenuScript(CString strMenuName)
{
	CString strXmlNodeName = _T("menus\\");
	strXmlNodeName += strMenuName;
	CString strInterp = m_xmlPlat.GetNodeText(strXmlNodeName + "\\Interp");
	CString strInterpInstance = m_xmlPlat.GetNodeAttribute(strXmlNodeName + "\\Interp" , "Name");
	CString strScriptFile = m_xmlPlat.GetNodeText(strXmlNodeName + "\\script");
	if(strInterp == VCIID_HTMLVIEW)
	{
		COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strScriptFile, 0);
		return "";
	}else
	if(strInterp == "shellfile")
	{
		ProcessPath(strScriptFile);
		ShellExecute(NULL,"open",strScriptFile,NULL,NULL,SW_SHOWNORMAL);
		return "";
	}

	CString strResult = "";
	ProcessPath(strScriptFile);
	IInterp* pInterp = (IInterp*)CreateVciObject(strInterp, strInterpInstance);
	if(pInterp)
	{
		if(!pInterp->RunScriptFile(strScriptFile))
		{
			AfxMessageBox(pInterp->GetResult());
		}
		strResult = pInterp->GetResult();
		ReleaseObject(pInterp);
	}

	return strResult;
}

/////////////////////////////////////////////////////////////////////////////
// �½��ļ�
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnProjectNewfile() 
{
	//ExecMenuScript("files\\newfile");
	if(m_pIProjectManager == NULL)
	{
		m_pIProjectManager = (IProjectManager*)(CreateVciObject(VCIID_PROJECTMANAGER, "###projectmanager"));
	}

	if(m_pIProjectManager != NULL)
	{
		m_pIProjectManager->NewFileWizard();
	}
}

void CBlueUIApp::OnUpdateProjectNewfile(CCmdUI* pCmdUI) 
{
	
}

/////////////////////////////////////////////////////////////////////////////
// ��������
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpTopics() 
{
	ExecMenuScript("Help\\Topic");
}

void CBlueUIApp::OnUpdateHelpTopics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ע��
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpRegisterate() 
{
	ExecMenuScript("Help\\Register");
}

void CBlueUIApp::OnUpdateHelpRegisterate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpUpgrade() 
{
	ExecMenuScript("Help\\Update");
}

void CBlueUIApp::OnUpdateHelpUpgrade(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpDownload() 
{
	ExecMenuScript("Help\\Download");
}

void CBlueUIApp::OnUpdateHelpDownload(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��ҳ
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpHomepage() 
{
	ExecMenuScript("Help\\Homepage");
}

void CBlueUIApp::OnUpdateHelpHomepage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnHelpBugreport() 
{
	ExecMenuScript("Help\\BugReport");
}

void CBlueUIApp::OnUpdateHelpBugreport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnToolsExtend() 
{
	ExecMenuScript("Tools\\Extends");
}

void CBlueUIApp::OnUpdateToolsExtend(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ѡ��OWMģ����ĵ�
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnOpenWith() 
{
	CChildFrame* pFrame = (CChildFrame*)((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
	if(pFrame)
	{
		CView* pActiveView = pFrame->GetActiveView();
		if(pActiveView)
		{
			CDocument* pDoc = pActiveView->GetDocument();
			if(pDoc)
			{
				CString strFilePath = pDoc->GetPathName();
				IInterp* pInterp = (IInterp*)(CreateVciObject("org.interp.tcl"));
				if(pInterp)
				{
					CString strTclScript = theApp.GetPlatRootPath();
					strTclScript += "lib/plat/openfilewith.tcl?";
					strTclScript += strFilePath;
					//strTclScript += "\"";
					pInterp->RunScriptFile(strTclScript);
					//CString strRes = pInterp->GetResult();
					ReleaseObject(pInterp);
				}
			}
		}
	}
}

void CBlueUIApp::OnUpdateOpenWith(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾPopup����
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::ShowPopupControl(TPopupControlInfo* pInfo)
{
	if (m_wndPopupControl.GetSafeHwnd())
		return;

	m_infoPopupControl = *pInfo;

	XTPPopupPaintTheme theme = xtpPopupThemeOffice2007;
	if(pInfo->strTheme == "msn")
	{
		theme = xtpPopupThemeMSN;
	}else
	if(pInfo->strTheme == "office2003")
	{
		theme = xtpPopupThemeOffice2003;
	}

	m_wndPopupControl.RemoveAllItems();

	CSize szPopup;
	CSize szTextItem;

	int nWidth = 220;
	if(pInfo->nWidth != 0)
	{
		nWidth = pInfo->nWidth;
	}

	int nHight = (pInfo->strText.GetLength() / ((nWidth-66) / 5) +1) * 16;
	if(pInfo->asButtonText.GetSize() > 0)
	{
		// ������ť�е�λ��
		nHight += 30;
	}

	int nAddHight = pInfo->nAddHight;
	CString strIcon = pInfo->strIcon;

	HICON hIcon = NULL;
	if(strIcon.Find(":") != -1)
	{
		WORD wIndex = 0;
		hIcon = ::ExtractAssociatedIcon(theApp.m_hInstance,
				strIcon.GetBuffer(0), &wIndex);
		strIcon.ReleaseBuffer();
	}

	if(theme == xtpPopupThemeOffice2003 || theme == xtpPopupThemeOffice2007)
	{
		// add text items.
		CXTPPopupItem* pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(46, 16, nWidth-20, 41), pInfo->strCaption));
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetBold(TRUE);
		pItemText->SetHyperLink(FALSE);

		pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(46, 30, nWidth-20, 46), pInfo->strTitle));
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetBold(TRUE);
		pItemText->SetHyperLink(FALSE);
		if(pInfo->strLinkTitle != "")
		{
			pItemText->SetID(ID_POPUP_GOTO_SITE1);
			pItemText->SetHyperLink(TRUE);
		}

		if(pInfo->nItemType == POPUP_ITEM_TEXT)
		{
			pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
				new CXTPPopupItem(CRect(46, 50, nWidth-20, nHight+60+nAddHight), pInfo->strText));
		}else
		{
			pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
				new CXTPPopupItem(CRect(46, 50, nWidth-20, nHight+60+nAddHight)));
		}
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetTextAlignment(DT_LEFT|DT_WORDBREAK);
		pItemText->SetHyperLink(FALSE);
		if(pInfo->strLinkText != "")
		{
			pItemText->SetID(ID_POPUP_GOTO_SITE2);
			pItemText->SetHyperLink(TRUE);
		}
		if(pInfo->nItemType == POPUP_ITEM_RTF)
		{
			pItemText->SetRTFText(pInfo->strText);
		}else
		if(pInfo->nItemType == POPUP_ITEM_MARKUP)
		{
			pItemText->SetMarkupText(pInfo->strText);
		}
		pItemText->FitToContent();

		szTextItem = CSize(pItemText->GetRect().Size());

		if((pInfo->nItemType == POPUP_ITEM_MARKUP) && (pInfo->nWidth == 0))
		{
			nWidth = szTextItem.cx + 70;
		}

		// left icon.
		CXTPPopupItem* pItemIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(5, 12, 17, 24)));
		if (strIcon == "info")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "warn")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_WARN), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "error")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_ERROR), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "plat")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, 0));
		}else
		{
			pItemIcon->SetIcon(hIcon);
		}
		pItemIcon->SetButton(FALSE);

		// close icon.
		CXTPPopupItem* pItemCloseIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(nWidth-22, 14, nWidth-6, 30)));
		//pItemCloseIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
		//	MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, 16, 16, 0));
		pItemCloseIcon->SetIcons(IDB_CLOSEMSN, 0, xtpPopupItemIconNormal|xtpPopupItemIconSelected|xtpPopupItemIconPressed);
		pItemCloseIcon->SetID(ID_POPUP_CLOSE);

		// top icon.//bug:��ӵ����޷��ͷ�,����ƶ�����������λ��
		CXTPPopupItem* pItemTopIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(-100, -100, -100, -100)));
	}else
	if(theme == xtpPopupThemeMSN)
	{
		CXTPPopupItem* pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(25, 6, nWidth-20, 19), pInfo->strCaption));
		pItemText->SetTextAlignment(DT_LEFT);
		pItemText->SetBold(TRUE);
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetHyperLink(FALSE);

		pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(46, 32, nWidth-20, 50), pInfo->strTitle));
		pItemText->SetTextAlignment(DT_LEFT);
		//pItemText->CalculateHeight();
		//pItemText->CalculateWidth();
		pItemText->SetBold(TRUE);
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetHyperLink(FALSE);
		if(pInfo->strLinkTitle != "")
		{
			pItemText->SetID(ID_POPUP_GOTO_SITE1);
			pItemText->SetHyperLink(TRUE);
		}

		if(pInfo->nItemType == POPUP_ITEM_TEXT)
		{
			pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
				new CXTPPopupItem(CRect(46, 50, nWidth-20, nHight+60+nAddHight), pInfo->strText));
		}else
		{
			pItemText = (CXTPPopupItem*)m_wndPopupControl.AddItem(
				new CXTPPopupItem(CRect(46, 50, nWidth-20, nHight+60+nAddHight)));
		}
		pItemText->SetTextAlignment(DT_LEFT|DT_WORDBREAK);
		pItemText->SetTextColor(RGB(0, 61, 128));
		pItemText->SetHyperLink(FALSE);
		if(pInfo->strLinkText != "")
		{
			pItemText->SetID(ID_POPUP_GOTO_SITE2);
			pItemText->SetHyperLink(TRUE);
		}
		if(pInfo->nItemType == POPUP_ITEM_RTF)
		{
			pItemText->SetRTFText(pInfo->strText);
		}else
		if(pInfo->nItemType == POPUP_ITEM_MARKUP)
		{
			pItemText->SetMarkupText(pInfo->strText);
		}
		pItemText->FitToContent();

		szTextItem = CSize(pItemText->GetRect().Size());

		if((pInfo->nItemType == POPUP_ITEM_MARKUP) && (pInfo->nWidth == 0))
		{
			nWidth = szTextItem.cx + 70;
		}

		// left icon
		CXTPPopupItem* pItemIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(7, 30, 19, 44)));
		if (strIcon == "info")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "warn")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_WARN), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "error")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDI_ICON_ERROR), IMAGE_ICON, 32, 32, 0));
		}else
		if (strIcon == "plat")
		{
			pItemIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, 0));
		}
		else
		{
			pItemIcon->SetIcon(hIcon);
		}
		pItemIcon->SetButton(FALSE);

		// close icon
		CXTPPopupItem* pItemCloseIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(nWidth-19, 6, nWidth-6, 19)));
		pItemCloseIcon->SetIcons(IDB_CLOSEMSN, 0, xtpPopupItemIconNormal|xtpPopupItemIconSelected|xtpPopupItemIconPressed);
		pItemCloseIcon->SetID(ID_POPUP_CLOSE);

		// top icon.
		CXTPPopupItem* pItemTopIcon = (CXTPPopupItem*)m_wndPopupControl.AddItem(
			new CXTPPopupItem(CRect(7, 6, 20, 19)));
		pItemTopIcon->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0));
		pItemTopIcon->SetButton(FALSE);
	}

	// ��ť
	if(pInfo->asButtonText.GetSize() > 0)
	{
		int nButtonWidth = 70;
		//int nButtonH = nHight + 30 + nAddHight;
		int nButtonH = szTextItem.cy + 65;
		int nButtonL = (nWidth - (pInfo->asButtonText.GetSize() * nButtonWidth)) / 2 + 3;

		for(int i=0; i<pInfo->asButtonText.GetSize(); i++)
		{
			CXTPPopupItem* pItemButton = (CXTPPopupItem*)m_wndPopupControl.AddItem(
				new CXTPPopupItem(CRect(nButtonL, nButtonH, nButtonL+nButtonWidth-2, nButtonH+18), pInfo->asButtonText[i]));
			pItemButton->SetTextAlignment(DT_CENTER|DT_VCENTER|DT_WORDBREAK);
			pItemButton->SetTextColor(RGB(0xA0, 0, 0));
			pItemButton->SetID(ID_POPUP_BUTTON_CLICK + i);
			pItemButton->SetHyperLink(FALSE);
			pItemButton->SetButton(TRUE);

			nButtonL += nButtonWidth;
		}
	}

	m_wndPopupControl.SetTheme(theme);

	m_wndPopupControl.GetPaintManager()->RefreshMetrics();

	DWORD dwShowDelay = 3000;
	DWORD dwAnimateDelay = 500;
	BYTE nTransparency = 230;

	if(theme == xtpPopupThemeOffice2003 || theme == xtpPopupThemeOffice2003)
	{
		dwShowDelay = 3000;
		dwAnimateDelay = 500;
		nTransparency = 230;
	}else
	if(theme == xtpPopupThemeMSN)
	{
		dwShowDelay = 3000;
		dwAnimateDelay = 350;
		nTransparency = 245;
	}

	if(pInfo->nShowDelay != 0)
	{
		dwShowDelay = pInfo->nShowDelay;
	}
	if(pInfo->nTransparency != 0)
	{
		nTransparency = pInfo->nTransparency;
	}

	if(pInfo->nHight != 0)
	{
		szPopup = CSize(nWidth, pInfo->nHight+nAddHight);
	}else
	{
		//szPopup = CSize(nWidth, nHight+70+nAddHight);
		szPopup = CSize(nWidth, szTextItem.cy + ((pInfo->asButtonText.GetSize() > 0) ? 100 : 75));
	}

	///////////////////////////////////////////////////////////////////////////
	// ���ر���ͼƬ
	CXTPImageManagerIcon* pImage = NULL;
	if(pInfo->strBackgroundImage != "")
	{
		BOOL bAlphaBitmap = FALSE;
		HBITMAP hBitmap = CXTPImageManagerIcon::LoadBitmapFromFile(pInfo->strBackgroundImage, &bAlphaBitmap);

		if (hBitmap)
		{
			CBitmap bmp;
			bmp.Attach(hBitmap);
			UINT nCommands[] = {100};
			m_wndPopupControl.GetImageManager()->SetIcons(bmp, nCommands, 1, 0, xtpImageNormal, bAlphaBitmap);
			m_wndPopupControl.SetBackgroundBitmap(nCommands[0]);

			BITMAP bmpInfo;
			::GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);
			szPopup = CSize(bmpInfo.bmHeight,bmpInfo.bmHeight);
		}
		pImage = m_wndPopupControl.GetImageManager()->GetImage(0, 0);
		//CMDTARGET_ADDREF(pImage);
	}else
	{
		m_wndPopupControl.SetBackgroundBitmap(0);
	}

	m_wndPopupControl.SetPopupAnimation((XTPPopupAnimation)(pInfo->nAnimation));
	m_wndPopupControl.SetShowDelay(dwShowDelay);
	m_wndPopupControl.SetAnimateDelay(dwAnimateDelay);
	m_wndPopupControl.SetTransparency(nTransparency);
	m_wndPopupControl.SetPopupSize(szPopup);
	m_wndPopupControl.AllowMove(TRUE);
	m_wndPopupControl.Show(AfxGetMainWnd());

	//CMDTARGET_RELEASE(pImage);
}

/////////////////////////////////////////////////////////////////////////////
// �������ڵ������¼�����
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::RunPopupControlLink(CString strLink)
{
	if(strLink.Find("link:") == 0)
	{
		strLink.Delete(0, 5);
		::ShellExecute(NULL, _T("open"), strLink, NULL, NULL, SW_SHOW);
		return TRUE;
	}else
	if(strLink.Find("html:") == 0)
	{
		strLink.Delete(0, 5);
		OpenHtmlWindow(strLink, TRUE, FALSE);
		return TRUE;
	}

	IInterp* pInterp = (IInterp*)(m_infoPopupControl.pInterp);
	if(pInterp == NULL)
	{
		return FALSE;
	}

	BOOL bCreateInterp = FALSE;
	if(strLink.Find("org.interp.") == 0)
	{
		int nPos = strLink.Find(":");
		CString strInterpId = strLink.Left(nPos);
		strLink.Delete(0, nPos+1);
		pInterp = (IInterp*)(CreateVciObject(strInterpId, ""));
		bCreateInterp = TRUE;
	}

	BOOL bRet = TRUE;
	int nPos = strLink.Find("<file>");
	if(nPos == 0)
	{
		strLink.Delete(0, 6);
		ProcessPath(strLink);

		// ���沢���õ�ǰ·��
		SaveAndSetCurPath(strLink);

		bRet = pInterp->RunScriptFile(strLink);

		// �ָ���ǰ·��
		RestoreCurPath();

	}else
	{
		bRet =  pInterp->RunScriptCommand(strLink);
	}

	if(bCreateInterp)
	{
		ReleaseObject(pInterp);
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��������״̬
/////////////////////////////////////////////////////////////////////////////
BOOL CBlueUIApp::GetPopupControlState()
{
	return (xtpPopupStateClosed != m_wndPopupControl.GetPopupState());
}

/////////////////////////////////////////////////////////////////////////////
// �رյ�������
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::ClosePopupControl()
{
	m_wndPopupControl.Close();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾAbout�Ի���
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::ShowAboutBox(CString strPluginId)
{
	for(int i=0; i<m_aPlugIns.GetSize(); i++)
	{
		if(strPluginId == m_aPlugIns[i].m_strId)
		{
			m_aPlugIns[i].ShowAboutBox();
			return;
		}
	}

	for(int i=0; i<m_aTools.GetSize(); i++)
	{
		if(strPluginId == m_aTools[i].m_strId)
		{
			m_aTools[i].ShowAboutBox();
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����ChildFrameָ���ȡ��Ӧ��OWM���������ĵ�ָ��
/////////////////////////////////////////////////////////////////////////////
CDocument* CBlueUIApp::GetOwmDocumentFromFrame(CWnd* pFrame, CPlugIn*& pPlugIn)
{
	for(int i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		if(theApp.m_aPlugIns[i].IsVciOwm() && theApp.m_aPlugIns[i].m_pDocTemplate)
		{
			POSITION posdoc = theApp.m_aPlugIns[i].m_pDocTemplate->GetFirstDocPosition();
			while(posdoc)
			{
				CDocument* pDoc = theApp.m_aPlugIns[i].m_pDocTemplate->GetNextDoc(posdoc);
				if(pDoc)
				{
					CString strPathName = pDoc->GetPathName();
					POSITION posview = pDoc->GetFirstViewPosition();
					if(posview)
					{
						CView* pView = pDoc->GetNextView(posview);
						if(pView && (pView->GetParent() == pFrame))
						{
							pPlugIn = &(theApp.m_aPlugIns[i]);
							return pDoc;
						}
					}
				}
			}
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ���ػỰ
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnFileLoadsession() 
{
	CString strFilter = "Session files(*.session)|*.session|All files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,	strFilter);
	if (dlg.DoModal() == IDOK)
	{
		CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, m_pMainWnd);
		pMainFrame->LoadSessionFile(dlg.GetPathName());
	}
}

void CBlueUIApp::OnUpdateFileLoadsession(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����Ự
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnFileSavesession() 
{
	CString strFilter = "Session files(*.session)|*.session|All files(*.*)|*.*||";
	CFileDialog dlg(FALSE, "session", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, m_pMainWnd);
		pMainFrame->SaveSessionFile(dlg.GetPathName());
	}
}

void CBlueUIApp::OnUpdateFileSavesession(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��ӡҳ������
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::OnFilePrintPagesetup() 
{
	CVciPropertyPageArray aPropertyPage;

	CTreePropertySheet tps(150);
	PLAT_LOADSTRING(strTitle, IDS_TITLE_TREEPROPSHEET_PRINTPAGESETUP); // ��ӡҳ������
	tps.m_strWinTitle = strTitle;

	// �ҵ���ǰ��ͼ��Ӧ�Ĳ������
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CPlugIn* pPlugIn = NULL;
	GetOwmDocumentFromFrame(pMainFrame->MDIGetActive(), pPlugIn);
	if(pPlugIn == NULL)
	{
		return;
	}
	IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
	if(pIVciOwm == NULL)
	{
		return;
	}

	// ��ȡ�Ի����б�
	COWMOptionArray aOWMOptions;
	pIVciOwm->RegisterPrintPageOption(aOWMOptions);
	if(aOWMOptions.GetSize() == 0)
	{
		return;
	}

	for(int i=0; i<aOWMOptions.GetSize(); i++)
	{
		CString strNode = aOWMOptions[i].strNode;
		CString strDesc = aOWMOptions[i].strDesc;
		CDialog* pDialog = (CDialog*)(aOWMOptions[i].pOptionPage);
		tps.AddPage(strNode,tps_item_node,-1,strDesc,pDialog,OPT_NODE,TRUE);
	}

	tps.SetLastPageType(tps_item_endbranch);

	tps.SetFancy(FALSE);

	// ִ���������öԻ���
	tps.DoModal();

	// �ͷ���Դ,��ֹ������Ĳ���������������������
	tps.RemoveResource();
}

void CBlueUIApp::OnUpdateFilePrintPagesetup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

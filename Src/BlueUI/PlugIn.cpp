////////////////////////////////////////////////////////////////////////////
//	File:		PlugIn.cpp
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	PlugIn implement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlueUI.h"
#include "PlugIn.h"
#include "MainFrm.h"
#include "resource.h"
#include "TaskDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ��ѯ�����Ƿ�֧��
int gQueryCommandIsSupport(LPCSTR lpcsCmd)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// �ĵ���չ����Ϣ��
//////////////////////////////////////////////////////////////////////////
COwmDocExtend::COwmDocExtend()
{
	m_strDocName = "";
	m_strDocId = "";
	m_strOwmId = "";
	m_strIcon = "";
	m_bEnableLink = TRUE;
}

COwmDocExtend::~COwmDocExtend()
{
}


//////////////////////////////////////////////////////////////////////////
// ͸��ͼ��Ϣ��
//////////////////////////////////////////////////////////////////////////
COwmPerspective::COwmPerspective()
{
	m_strName = "";
	m_strId = "";
	m_strOwmId = "";
	m_strViewName = "";
}

COwmPerspective::~COwmPerspective()
{
}

//////////////////////////////////////////////////////////////////////////
// VCI��չ������
//////////////////////////////////////////////////////////////////////////
CVciParameter::CVciParameter()
{
	m_strName = "";
	m_strValue = "";
	m_strDefaultValue = "";
}

CVciParameter::~CVciParameter()
{
}

//////////////////////////////////////////////////////////////////////////
// VCI��չ�˵���
//////////////////////////////////////////////////////////////////////////
CVciMenu::CVciMenu()
{
	m_strMainMenu	= "";
	m_strSubMenu	= "";
	m_strAction		= "";
}

CVciMenu::~CVciMenu()
{
}

//////////////////////////////////////////////////////////////////////////
// �����Ϣ��
//////////////////////////////////////////////////////////////////////////
CPlugIn::CPlugIn()
{
	m_nLoadState = PLUGIN_UNLOAD;
	m_strPath = "";
	m_nOwmId = 0;
	m_strId = "";
	m_hVciHandle = NULL;
	m_pVciObj = NULL;
	m_pIVciControl = NULL;
	m_nRefCount = 0;
	m_nType = 0;
	m_bAutoCreate = FALSE;
	m_strAutoCreateInstanceName = "";
	m_strClass = "";
	m_strClassVersion = "";
	m_strProcess = "";
	m_strFile = "";
	m_strInterfaceFile = "";
	m_pDocTemplate = NULL;
	m_pControlBar = NULL;
	m_strInitParam = "";
	m_strDoneParam = "";
	m_strAboutPage = "";
}

CPlugIn::CPlugIn(CString strPath)
{
	m_nLoadState = PLUGIN_UNLOAD;
	m_strPath = strPath;
	m_nOwmId = 0;
	m_strId = "";
	m_hVciHandle = NULL;
	m_pVciObj = NULL;
	m_pIVciControl = NULL;
	m_nRefCount = 0;
	m_nType = 0;
	m_bAutoCreate = FALSE;
	m_strAutoCreateInstanceName = "";
	m_strClass = "";
	m_strClassVersion = "";
	m_strProcess = "";
	m_strFile = "";
	m_strInterfaceFile = "";
	m_pDocTemplate = NULL;
	m_pControlBar = NULL;
	m_strInitParam = "";
	m_strDoneParam = "";
	m_strAboutPage = "";
}

CPlugIn::~CPlugIn()
{
}

/////////////////////////////////////////////////////////////////////////////
// ���ز����Ϣ
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::LoadInfo()
{
	// �Ѿ����ع���Ϣ,�Ͳ����ظ�������
	if(GetLoadState() >= PLUGIN_LOAD_INFO)
	{
		return TRUE;
	}

	CString strXmlFile = m_strPath;
	if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
	{
		strXmlFile += "\\plugin_cn.xml";
		// ������������ĵ������ļ�,����ȱʡ��
		if(GetFileAttributes(strXmlFile) == 0xFFFFFFFF)
		{
			strXmlFile = m_strPath + "\\plugin.xml";
		}
	}else
	{
		strXmlFile += "\\plugin.xml";
	}
	CXmlParser parser;
	if(parser.Open(strXmlFile))
	{
		OUTPUT(COLOR_ERROR, "Load %s fail!\r\n", strXmlFile);
		return FALSE;
	}

	DOMNode* pNode = parser.GetRootNode();
	m_strId = parser.GetNodeAttribute(pNode, "id");
	m_nOwmId = atoi(parser.GetNodeAttribute(pNode, "owmid"));
	m_strName = parser.GetNodeAttribute(pNode, "name");
	m_strVersion = parser.GetNodeAttribute(pNode, "version");
	m_strProvider = parser.GetNodeAttribute(pNode, "provider-name");
	m_strType = parser.GetNodeAttribute(pNode, "type");
	m_strClass = parser.GetNodeAttribute(pNode, "class");
	m_strClassVersion = parser.GetNodeAttribute(pNode, "class-version");
	m_strProcess = parser.GetNodeAttribute(pNode, "process");

	// ��ȡ������Ϣ
	DOMNode* pSubNode = parser.GetChildNode(pNode, "runtime");
	m_strInitParam = parser.GetNodeText(parser.GetChildNode(pSubNode, "init"));
	m_strDoneParam = parser.GetNodeText(parser.GetChildNode(pSubNode, "done"));
	m_strInterfaceFile = parser.GetNodeText(parser.GetChildNode(pSubNode, "interface-define"));
	m_strInterfaceFile = m_strPath + "\\" + m_strInterfaceFile;
	m_strInterfaceFile.Replace("/", "\\");
	#ifdef _DEBUG
	pSubNode = parser.GetChildNode(pSubNode, "library-debug");
	#else
	pSubNode = parser.GetChildNode(pSubNode, "library");
	#endif
	CString strLibrary = parser.GetNodeAttribute(pSubNode, "name");
	m_strFile = m_strPath + "\\" + strLibrary;
	m_strFile.Replace("/", "\\");	

	// ��ȡOWM������Ϣ
	m_nType = 0;
	DOMNode* pNodeProperty = parser.GetChildNode(pNode, "property");
	if(IsVciOwm())
	{
		// OWM������
		pSubNode = parser.GetChildNode(pNodeProperty, "owm-type");
		if(parser.GetNodeAttribute(pSubNode, "doc") == "true")
		{
			m_nType |= OWM_DOC;
		}
		if(parser.GetNodeAttribute(pSubNode, "dockbar") == "true")
		{
			m_nType |= OWM_CONTROLBAR;
		}
		if(parser.GetNodeAttribute(pSubNode, "menu") == "true")
		{
			m_nType |= OWM_MENU;
		}
		if(parser.GetNodeAttribute(pSubNode, "toolbar") == "true")
		{
			m_nType |= OWM_TOOLBAR;
		}
		if(parser.GetNodeAttribute(pSubNode, "option") == "true")
		{
			m_nType |= OWM_OPTION;
		}
		if(parser.GetNodeAttribute(pSubNode, "oneframe") == "true")
		{
			m_nType |= OWM_ONEFRAME;
		}
		if(parser.GetNodeAttribute(pSubNode, "autocreate") == "true")
		{
			m_nType |= OWM_AUTOCREATE;
		}
		if(parser.GetNodeAttribute(pSubNode, "dynload") == "true")
		{
			m_nType |= OWM_DYNLOAD;
		}
		if(parser.GetNodeAttribute(pSubNode, "broadcast") == "true")
		{
			m_nType |= OWM_BROADCAST;
		}
		if(parser.GetNodeAttribute(pSubNode, "maxwindow") == "true")
		{
			m_nType |= OWM_MAXWINDOW;
		}
	}else
	{
		// VCI����
		pSubNode = parser.GetChildNode(pNodeProperty, "autocreate");
		m_bAutoCreate = (parser.GetNodeText(pSubNode) == "true");
		m_strAutoCreateInstanceName = parser.GetNodeAttribute(pSubNode, "instancename");
	}

	// ��ȡ����ҳ���λ��
	pSubNode = parser.GetChildNode(pNode, "url");
	pSubNode = parser.GetChildNode(pSubNode, "about");
	m_strAboutPage = parser.GetNodeAttribute(pSubNode, "url");

	// ��ȡ�ĵ���չ����Ϣ
	pSubNode = parser.GetNode("doc-extend\\doc");
	while (pSubNode != NULL)
	{
		COwmDocExtend docExtend;

		docExtend.m_strDocName = parser.GetNodeAttribute(pSubNode, "name");
		docExtend.m_strDocId = parser.GetNodeAttribute(pSubNode, "id");
		docExtend.m_strOwmId = m_strId;
		docExtend.m_strIcon = parser.GetNodeAttribute(pSubNode, "icon");
		docExtend.m_bEnableLink = (parser.GetNodeAttribute(pSubNode, "enablelink") != "false");

		// ��׺
		DOMNode* pNodeExts = parser.GetChildNode(pSubNode, "exts");
		if(pNodeExts)
		{
			DOMNode* pNodeExt = parser.GetChildNode(pNodeExts, "ext");
			while (pNodeExt != NULL)
			{
				docExtend.m_arDocExt.Add(parser.GetNodeText(pNodeExt));
				pNodeExt = parser.GetNextNode(pNodeExt, "ext");
			}
		}

		// ִ����
		DOMNode* pNodeExecutes = parser.GetChildNode(pSubNode, "executes");
		if(pNodeExecutes)
		{
			DOMNode* pNodeExecute = parser.GetChildNode(pNodeExecutes);
			while (pNodeExecute != NULL)
			{
				docExtend.m_arExecuteModuleType.Add(parser.GetNodeName(pNodeExecute));
				docExtend.m_arExecuteModule.Add(parser.GetNodeText(pNodeExecute));
				pNodeExecute = parser.GetNextNode(pNodeExecute);
			}
			/*
			DOMNode* pNodeExecuteOwm = parser.GetChildNode(pNodeExecutes, "owm");
			while (pNodeExecuteOwm != NULL)
			{
				docExtend.m_arExecuteModule.Add(parser.GetNodeText(pNodeExecuteOwm));
				pNodeExecuteOwm = parser.GetNextNode(pNodeExecuteOwm, "owm");
			}
			*/
		}

		// �ĵ�ͼ��
		DOMNode* pNodeIcons = parser.GetChildNode(pSubNode, "icons");
		if(pNodeIcons)
		{
			DOMNode* pNodeIcon = parser.GetChildNode(pNodeIcons);
			while (pNodeIcon != NULL)
			{
				docExtend.m_arDocIconType.Add(parser.GetNodeName(pNodeIcon));
				docExtend.m_arDocIconFile.Add(parser.GetNodeText(pNodeIcon));
				pNodeIcon = parser.GetNextNode(pNodeIcon);
			}
		}

		m_arDocExtend.Add(docExtend);

		pSubNode = parser.GetNextNode(pSubNode, "doc");
	}

	// ��ȡDockingPane��Ϣ
	pSubNode = parser.GetNode("dockingpanes\\pane");
	while (pSubNode != NULL)
	{
		COwmDockingPane paneInfo;
		paneInfo.m_bFileDefined = TRUE;	// �����ļ��ж����DockingPane

		paneInfo.m_strName = parser.GetNodeAttribute(pSubNode, "name");
		paneInfo.m_nId = atoi(parser.GetNodeAttribute(pSubNode, "id"));
		CString strDirection = parser.GetNodeAttribute(pSubNode, "direction");
		if(strDirection == "left")
		{
			paneInfo.m_nDirection = xtpPaneDockLeft;
		}else
		if(strDirection == "right")
		{
			paneInfo.m_nDirection = xtpPaneDockRight;
		}else
		if(strDirection == "top")
		{
			paneInfo.m_nDirection = xtpPaneDockTop;
		}else
		if(strDirection == "bottom")
		{
			paneInfo.m_nDirection = xtpPaneDockBottom;
		}else
		{
			paneInfo.m_nDirection = xtpPaneDockLeft;
		}
		paneInfo.m_strIcon = parser.GetNodeAttribute(pSubNode, "icon");
		CString strAttachType = parser.GetNodeAttribute(pSubNode, "type");
		if(strAttachType == "window")
		{
			paneInfo.m_nAttachType = PANE_ATTACH_WND;
		}else
		if(strAttachType == "view")
		{
			paneInfo.m_nAttachType = PANE_ATTACH_VIEW;
		}else
		{
			paneInfo.m_nAttachType = PANE_ATTACH_WND;
		}
		paneInfo.m_bAutoCreate = (parser.GetNodeAttribute(pSubNode, "autocreate") == "true");
		paneInfo.m_bProjectCreate = (parser.GetNodeAttribute(pSubNode, "projectcreate") == "true");
		paneInfo.m_bSave = (parser.GetNodeAttribute(pSubNode, "save") == "true");
		paneInfo.m_strOwmId = parser.GetNodeAttribute(pSubNode, "owm");

		m_arDockingPane.Add(paneInfo);

		pSubNode = parser.GetNextNode(pSubNode, "pane");
	}	

	// ��ȡ͸��ͼ��Ϣ

	// ��ȡ��չ������Ϣ
	pSubNode = parser.GetNode("parameters\\parameter");
	while (pSubNode != NULL)
	{
		CVciParameter parameterInfo;

		parameterInfo.m_strName = parser.GetNodeAttribute(pSubNode, "name");
		parameterInfo.m_strValue = parser.GetNodeAttribute(pSubNode, "value");
		parameterInfo.m_strDefaultValue = parser.GetNodeAttribute(pSubNode, "defaultName");

		m_arParameter.Add(parameterInfo);

		pSubNode = parser.GetNextNode(pSubNode, "parameter");
	}

	// ��ȡ��չ�˵���Ϣ
	pSubNode = parser.GetNode("menus\\menu");
	while (pSubNode != NULL)
	{
		CVciMenu menuInfo;

		menuInfo.m_strMainMenu = parser.GetNodeAttribute(pSubNode, "mainmenu");
		menuInfo.m_strSubMenu = parser.GetNodeAttribute(pSubNode, "submenu");
		menuInfo.m_strAction = parser.GetNodeText(pSubNode);

		m_arMenu.Add(menuInfo);

		if(m_strType == "owm")
		{
			// OWM����ڴ˴��ͼ��ز˵�
			AddOwmSubMenu(menuInfo.m_strMainMenu, menuInfo.m_strSubMenu, menuInfo.m_strAction);
		}

		pSubNode = parser.GetNextNode(pSubNode, "menu");
	}

	m_nLoadState = PLUGIN_LOAD_INFO;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾAbout�Ի���
/////////////////////////////////////////////////////////////////////////////
void CPlugIn::ShowAboutBox()
{
	TTaskDialogInfo taskDialogInfo;
	taskDialogInfo.strWindowTitle = m_strName;
	taskDialogInfo.strMainInstruction = m_strName;
	CString strContent;
	strContent = "Version : " + m_strVersion + "\n";
	strContent += "Author : ";
	strContent += m_strProvider;
	strContent += "\n";

	CString strXmlFile = m_strPath;
	if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
	{
		strXmlFile += "\\plugin_cn.xml";
		// ������������ĵ������ļ�,����ȱʡ��
		if(GetFileAttributes(strXmlFile) == 0xFFFFFFFF)
		{
			strXmlFile = m_strPath + "\\plugin.xml";
		}
	}else
	{
		strXmlFile += "\\plugin.xml";
	}
	CXmlParser parser;
	if(parser.Open(strXmlFile) == 0)
	{
		strContent += "License : ";
		CString strLicense = parser.GetNodeText("license");
		strLicense.TrimLeft();
		strLicense.TrimRight();
		strContent += strLicense;
		strContent += "\n";
		CString strDesc = parser.GetNodeText("description");
		strDesc.TrimLeft();
		strDesc.TrimRight();
		strContent += strDesc;
		strContent += "\n";
		CString strCopyright = parser.GetNodeText("copyright");
		strCopyright.TrimLeft();
		strCopyright.TrimRight();
		strContent += strCopyright;
	}
	taskDialogInfo.strContent = strContent;
	taskDialogInfo.strMainIcon = "Information";
	theApp.DoTaskDialog(&taskDialogInfo);

}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ����ı���ʱ��
/////////////////////////////////////////////////////////////////////////////
CTime CPlugIn::GetPlugInBuildTime()
{
	typedef int (*FNGetInterfaceList)(TInterfaceInfo **pInfo);
	// ��ȡ����ָ��
	FNGetInterfaceList fnGetInterfaceList = (FNGetInterfaceList)GetProcAddress(m_hVciHandle, "GetInterfaceList");
	if(fnGetInterfaceList)
	{
		TInterfaceInfo *pInfo = NULL;
		int nInterfaceCount = fnGetInterfaceList(&pInfo);
		if(pInfo != NULL)
		{
			CString strBuildDate = pInfo->csDate;
			COleDateTime dtTime;
			dtTime.ParseDateTime(strBuildDate);

			SYSTEMTIME st;
			dtTime.GetAsSystemTime(st);
			CTime tTime(st);
			return tTime;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�����GUID
/////////////////////////////////////////////////////////////////////////////
CString CPlugIn::GetPlugInGUID()
{
	typedef int (*FNGetInterfaceList)(TInterfaceInfo **pInfo);
	// ��ȡ����ָ��
	FNGetInterfaceList fnGetInterfaceList = (FNGetInterfaceList)GetProcAddress(m_hVciHandle, "GetInterfaceList");
	if(fnGetInterfaceList)
	{
		TInterfaceInfo *pInfo = NULL;
		int nInterfaceCount = fnGetInterfaceList(&pInfo);
		if(pInfo != NULL)
		{
			return pInfo->csGuid;
		}
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////
// OWM��չ�˵��������
/////////////////////////////////////////////////////////////////////////////
void OnOwmExtMenuProc(int nID) 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CExtMenuInfo* pMenuInfo = pMainFrame->GetExtMenuInfo(nID);
	if(pMenuInfo)
	{
		CPlugIn* pPlugIn = theApp.GetDynamicOWM(pMenuInfo->m_strVciId);
		if(pPlugIn == NULL)
		{
			return;
		}

		if(!(pPlugIn->IsVciOwm()))
		{
			return;
		}

		IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
		if(pIVciOwm == NULL)
		{
			// ���OWM�����δ����,���ȼ���
			pPlugIn->LoadOwmPlugIn();
		}

		if(pMenuInfo->m_strAction == "showview")
		{
			// ��ʾOWM����
			pPlugIn->ShowOwmView(TRUE, TRUE);
		}else
		if(pMenuInfo->m_strAction.Find("showpane:") == 0)
		{
			// ��ʾOWM Docking Pane
			CString strPaneId = pMenuInfo->m_strAction;
			strPaneId.Delete(0, 9);
			int nPaneId = atoi(strPaneId);
			pPlugIn->LoadDockingPane(nPaneId);
		}else
		{
		}
	}
}

static int g_nIdOwmExMenu = 4000;	// OWM����չ�˵���IDM_OWMCMD+4000��ʼ,���96��
/////////////////////////////////////////////////////////////////////////////
// ������չ�˵�
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::AddOwmSubMenu(CString strMainMenu, CString strSubMenu, CString strAction)
{
	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
	CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
	if(!pCommandBars)
	{
		return FALSE;		
	}
	CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
	if(!pMenuBar)
	{
		return FALSE;
	}

	CXTPControls* pControls = pMenuBar->GetControls();
	CXTPControl* pControl = pControls->GetFirst();
	CXTPCommandBar* pMenuBarSub = NULL;
	CXTPControl* pControlExtSub = NULL;
	while(pControl)
	{
		if(pControl->GetCaption() == strMainMenu)
		{
			pMenuBarSub = pControl->GetCommandBar();
			CXTPControls* pControlsSub = pMenuBarSub->GetControls();
			pControl = pControlsSub->GetFirst();
			while(pControl)
			{
				if(pControl->GetCaption() == strSubMenu)
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
		CExtMenuInfo extMenuInfo;
		extMenuInfo.m_strVciId = m_strId;
		extMenuInfo.m_strMainMenu = strMainMenu;
		extMenuInfo.m_strSubMenu = strSubMenu;
		extMenuInfo.pProc = (TYPEOF_ClientMenu*)OnOwmExtMenuProc;
		extMenuInfo.pUpdateProc = NULL;
		extMenuInfo.nProcType = MENU_PROC_TYPE_PARAM_ID;
		
		int nExMenuId = IDM_OWMCMD+g_nIdOwmExMenu;
		extMenuInfo.m_strAction = strAction;
		extMenuInfo.nId = nExMenuId-IDM_OWMCMD;
		pMainFrame->m_arExtMenuInfo.Add(extMenuInfo);

		CXTPControls* pControlsSub = pMenuBarSub->GetControls();
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();
		pMenu->AppendMenu(MF_STRING, nExMenuId, strSubMenu);
		pControlsSub->AddMenuItem(pMenu, 0);
		delete pMenu;

		g_nIdOwmExMenu++;
	}

	return TRUE;
}

typedef LPVOID (*TYPEOF_CreateObject)(LPCSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData);

/////////////////////////////////////////////////////////////////////////////
// ����VCI���
/////////////////////////////////////////////////////////////////////////////
void* CPlugIn::OpenVciLib(CString strInterface)
{
	// ����Ѿ�����,��ֱ�ӷ���
	if(HasLoad())
	{
		return m_hVciHandle;
	}

	if(GetFileAttributes(m_strFile) == 0xFFFFFFFF)
	{
		OUTPUT(COLOR_ERROR, "File %s not exist!\r\n", m_strFile);
		return NULL;
	}

	if(m_hVciHandle == NULL)
	{
		// ���沢���õ�ǰ·��
		theApp.SaveAndSetCurPath(m_strFile);

		// ����DLLģ��
		m_hVciHandle = LoadLibrary(m_strFile);

		// �ָ���ǰ·��
		theApp.RestoreCurPath();
	}

	if(m_hVciHandle == NULL)
	{
		DWORD dwError = ::GetLastError();
		OUTPUT(COLOR_ERROR, "Load %s fail, error code=%u!\r\n", m_strFile, dwError);
	}

	TRACE("INFO:Load VCI %s success!\n", m_strId);

	m_nLoadState = PLUGIN_LOAD_LIB;

	return m_hVciHandle;
}

/////////////////////////////////////////////////////////////////////////////
// �ر�һ��VCI���
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CloseVci()
{
	if(m_strType == "owm")
	{
		CloseOwm();
		m_aVciObj.RemoveAll();
		m_pVciObj = NULL;
	}else
	{
		// ִ��Done����
		for(int i=0; i<m_aVciObj.GetSize(); i++)
		{
			if(m_aVciObj[i].m_pIVciControl != NULL)
			{
				((IVciControl*)(m_aVciObj[i].m_pIVciControl))->Done(m_strDoneParam);
			}
			if(m_aVciObj[i].m_pVciObj != NULL)
			{
				((IUnknown *)(m_aVciObj[i].m_pVciObj))->Release();
			}
		}
		m_aVciObj.RemoveAll();
		m_pVciObj = NULL;
	}

	if(m_hVciHandle == NULL)
	{
		return FALSE;
	}

	// �ͷ�DLL
	if(m_strType != "interp")	// ����ֵ�����,Tcl���������ö�λ�����ͷ�DLL�����ر�ʱ�쳣
	{
		FreeLibrary(m_hVciHandle);
		m_hVciHandle = NULL;
	}

	m_nLoadState = PLUGIN_LOAD_INFO;

	return TRUE;
}

// VCI�ĵ�����Ϣ����ص�����
int callback_Vci_DebugOut(LPCTSTR lpszComponentId, LPVOID pInstance, int nLevel, int nType, int nMode, LPCSTR lpData, int nLen)
{
	//OUTPUT_ID_DIRECT(0, lpData);
	BOOL bEnableDebugOut = theApp.GetProfileInt(REG_VCI_SUBKEY, REG_VCI_ENABLE_DEBUGOUT, FALSE);
	if(bEnableDebugOut)
	{
		OUTPUT(nType, lpData);
	}

	// ����ƽ̨����־����
	theApp.PLATDEBUG(lpszComponentId, nLevel, lpData, nType, nMode);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ����VCI�������
/////////////////////////////////////////////////////////////////////////////
void* CPlugIn::CreateObject(CString strInstName)
{
	// �����̱�ʶ,�ж��Ƿ�����ڱ����̼���
	if((m_strProcess != "") && (theApp.m_strProcessId != "") && (m_strProcess != theApp.m_strProcessId))
	{
		return NULL;
	}

	// ���δ���ض�̬��,���ȼ���
	if(!HasLoad())
	{
		OpenVciLib(m_strClass);
	}

	// ��̬�����ʧ��,���˳�
	if(m_hVciHandle == NULL)
	{
		return NULL;
	}

	if(strInstName != "")
	{
		for(int i=0; i<m_aVciObj.GetSize(); i++)
		{
			if(m_aVciObj[i].m_strInstName == strInstName)
			{
				m_aVciObj[i].m_nRefCount++;
				if(m_aVciObj[i].m_pVciObj == m_pVciObj)
				{
					m_nRefCount++;
				}
				return m_aVciObj[i].m_pVciObj;
			}
		}
	}

	#ifdef _DEBUG
	DWORD dwRevisionType = REVISION_TYPE_DEBUG;
	#else
	DWORD dwRevisionType = REVISION_TYPE_RELEASE;
	#endif;

	LPVOID	pIVciControl = NULL;

	// ��ȡ����ָ��
	TYPEOF_CreateObject fnCreateObject = (TYPEOF_CreateObject)GetProcAddress(m_hVciHandle, "CreateObject");
	LPVOID lpVciObj = fnCreateObject(m_strClass, &pIVciControl, NULL);

	if(lpVciObj == NULL)
	{
		return NULL;
	}

	// ����ƽ̨�ӿ�
	if(pIVciControl)
	{
		((IVciControl*)pIVciControl)->setIPlatUI(theApp.GetIPlatUI());
	}

	// ��������
	if(pIVciControl)
	{
		((IVciControl*)pIVciControl)->SetLanguage(
			(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage) ? LANGUAGE_CHINESE : LANGUAGE_ENGLISH);
	}

	// ��װ���Իص�����
	if(pIVciControl)
	{
		TInitData_DebugOut InitData_DebugOut;
		InitData_DebugOut.lpfnDebugOut = callback_Vci_DebugOut;
		InitData_DebugOut.strComponentId = m_strId;
		InitData_DebugOut.lpVciInstance = NULL;
		InitData_DebugOut.nDeviceID = 0;

		((IVciControl*)pIVciControl)->InstallHandler(DEBUG_OUT_EVENT, callback_Vci_DebugOut, (DWORD)(&InitData_DebugOut));
	}

	if(m_strType == "interp")
	{
		IInterp* pIInterp = (IInterp*)lpVciObj;
		// ����ƽ̨�ӿ�
		pIInterp->SetIPlatUI(theApp.GetIPlatUI());
		// ���ý�������ΪVCIʵ����
		pIInterp->SetInterpName(strInstName);
	}

	CVciObject obj;
	obj.m_pVciObj		= lpVciObj;
	obj.m_pIVciControl	= pIVciControl;
	obj.m_strInstName	= strInstName;
	obj.m_nRefCount		= 1;
	m_aVciObj.Add(obj);

	// ��ʼ��ȫ�ֱ���
	if(m_pVciObj == NULL)
	{
		m_pVciObj		= lpVciObj;
		m_pIVciControl	= pIVciControl;
		m_nRefCount		= 1;
	}

	// ִ������ĳ�ʼ������(OWM�����OWM�ļ��غ�����ִ�г�ʼ��)
	if(pIVciControl && !IsVciOwm())
	{
		((IVciControl*)pIVciControl)->Init(m_strInitParam);
	}

	//CTime tBuild = GetPluginBuildTime();
	//TRACE("PlugIn %s build date is %s.\n", m_strId, tBuild.Format("%Y-%m-%d   %H:%M:%S"));

	return lpVciObj;
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�VCI�������
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::ReleaseObject(LPVOID pVciObj, BOOL bForceDelete)
{
	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		if(m_aVciObj[i].m_pVciObj == pVciObj)
		{
			m_aVciObj[i].m_nRefCount--;			
			if(m_aVciObj[i].m_pVciObj == m_pVciObj)
			{
				m_nRefCount--;
			}
			if((m_aVciObj[i].m_nRefCount > 0) && (!bForceDelete))
			{
				return TRUE;
			}

			// ִ��Done����
			if(m_aVciObj[i].m_pIVciControl != NULL)
			{
				((IVciControl*)(m_aVciObj[i].m_pIVciControl))->Done(m_strDoneParam);
			}

			if(m_aVciObj[i].m_pVciObj)
			{
				try{
				((IUnknown *)(m_aVciObj[i].m_pVciObj))->Release();
				}catch(...){
					OUTPUT(COLOR_ERROR, "Catch Exception: CPlugIn::ReleaseObject(0x%08X), Release object fail!\r\n", m_pVciObj);
					OUTPUT(COLOR_ERROR, "Object Instance Name: %s\r\n", m_aVciObj[i].m_strInstName);
					OUTPUT(COLOR_ERROR, "Object Refrence Count: %d\r\n", m_aVciObj[i].m_nRefCount);
				}
			}

			m_aVciObj[i].m_nRefCount = 0;
			
			if(m_aVciObj[i].m_pVciObj == m_pVciObj)
			{
				m_pVciObj		= NULL;
				m_pIVciControl	= NULL;
				m_nRefCount		= 0;
			}

			m_aVciObj.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�VCI�������(ָ��ʵ����)
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::ReleaseObject(CString strInstName)
{
	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		if(m_aVciObj[i].m_strInstName == strInstName)
		{
			// ִ��Done����
			if(m_aVciObj[i].m_pIVciControl != NULL)
			{
				((IVciControl*)(m_aVciObj[i].m_pIVciControl))->Done(m_strDoneParam);
			}

			if(m_aVciObj[i].m_pVciObj)
			{
				((IUnknown *)(m_aVciObj[i].m_pVciObj))->Release();
			}

			m_aVciObj[i].m_nRefCount = 0;
			
			if(m_aVciObj[i].m_pVciObj == m_pVciObj)
			{
				m_pVciObj		= NULL;
				m_pIVciControl	= NULL;
				m_nRefCount		= 0;
			}

			m_aVciObj.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ���������(����ʵ����)
/////////////////////////////////////////////////////////////////////////////
void* CPlugIn::GetObjectByInstanceName(CString strInstName)
{
	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		if(m_aVciObj[i].m_strInstName == strInstName)
		{
			return (void*)(m_aVciObj[i].m_pVciObj);
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����������ƽӿ�(�������ʵ���ӿ�ָ��)
/////////////////////////////////////////////////////////////////////////////
void* CPlugIn::GetObjectControlPtrByInstancePtr(LPVOID	pVciObj)
{
	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		if(m_aVciObj[i].m_pVciObj == pVciObj)
		{
			return (void*)(m_aVciObj[i].m_pIVciControl);
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// �ж�һ��ʵ���ӿ�ָ���Ƿ�����������
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::IsInstancePtr(LPVOID	pVciObj)
{
	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		if(m_aVciObj[i].m_pVciObj == pVciObj)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ѯ���Ƿ���Թر�,���������Ϣ
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CanClose()
{
	if(m_strType == "owm")
	{
		return CanCloseOwm();
	}else
	if(m_strType == "interp")
	{
		return CanCloseInterp();
	}
	return TRUE;
}

// ���ز��
BOOL CPlugIn::LoadPlugIn(BOOL bForce)
{
	if(m_strType == "owm")
	{
		// ����Ƕ�̬���������Ҳ�ǿ�Ƽ�����ֱ�ӷ���
		if(!bForce && (m_nType & OWM_DYNLOAD) && !(m_nType & OWM_AUTOCREATE))
		{
			return TRUE;
		}
		return LoadOwmPlugIn();
	}else
	{
		// Ϊ�˼ӿ������ٶ�,VCI���ֻ��Createʱ�����
		if(bForce)
		{
			return LoadVciPlugIn();
		}
		return TRUE;
	}

	return FALSE;
}

// ����VCI���Ͳ��
BOOL CPlugIn::LoadVciPlugIn()
{
	return (OpenVciLib(m_strClass) != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ö�ٲ�����
/////////////////////////////////////////////////////////////////////////////
int CPlugIn::EnumParameters(CStringArray& asName)
{
	for(int i=0; i<m_arParameter.GetSize(); i++)
	{
		asName.Add(m_arParameter[i].m_strName);
	}
	return asName.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ����
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::GetParameter(CString strName, CString& strValue, CString& strDefaultValue)
{
	for(int i=0; i<m_arParameter.GetSize(); i++)
	{
		if(strName == m_arParameter[i].m_strName)
		{
			strValue = m_arParameter[i].m_strValue;
			strDefaultValue = m_arParameter[i].m_strDefaultValue;
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ò���
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::SetParameter(CString strName, CString strValue)
{
	for(int i=0; i<m_arParameter.GetSize(); i++)
	{
		if(strName == m_arParameter[i].m_strName)
		{
			m_arParameter[i].m_strValue = strValue;
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ƽ̨�������
/////////////////////////////////////////////////////////////////////////////
int gCommandPlat(int nCmd, WPARAM wParam, LPARAM lParam)
{
	return theApp.CommandPlat(nCmd, wParam, lParam);
}

// ����OWM���Ͳ��
BOOL CPlugIn::LoadOwmPlugIn()
{
	// �����̱�ʶ,�ж��Ƿ�����ڱ����̼���
	if((m_strProcess != "") && (theApp.m_strProcessId != "") && (m_strProcess != theApp.m_strProcessId))
	{
		return NULL;
	}

	OpenVciLib(IID_IVciOwm);
	m_pVciObj = CreateObject();
	if(m_pVciObj == NULL)
	{
		return FALSE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;

	OWM_INFO owminfo;
	memset(&owminfo, 0, sizeof(OWM_INFO));
	owminfo.bEnable = TRUE;

	pIVciOwm->SetIPlatUI(theApp.GetIPlatUI());

	// �����ĵ�ģ��
	if(m_nType & OWM_DOC)
	{
		CRuntimeClass* pDocClass = NULL;
		CRuntimeClass* pFrameClass = NULL;
		CRuntimeClass* pViewClass = NULL;
		pIVciOwm->GetRuntimeClass(1, &pDocClass);
		pIVciOwm->GetRuntimeClass(2, &pFrameClass);
		pIVciOwm->GetRuntimeClass(3, &pViewClass);
		m_pDocTemplate =  new CMultiDocTemplate(
			IDR_BLUEUITYPE,
			pDocClass,
			pFrameClass,
			pViewClass);
		pIVciOwm->SetDocTemplate(m_pDocTemplate);
		// ������ǰ�����ݽṹ
		owminfo.ol.pDocClass = pDocClass;
		owminfo.ol.pFrameClass = pFrameClass;
		owminfo.ol.pViewClass = pViewClass;
		owminfo.ol.pDocTemplate = m_pDocTemplate;
	}

	owminfo.ol.nOWMType		= m_nType;
	owminfo.ol.CommandPlat	= gCommandPlat;
//	owminfo.ol.TraceFmt		= PF_TRACE_FMT;
//	owminfo.ol.CreateOutput	= PF_CREATE_OUTPUT;
//	owminfo.ol.CloseOutput	= PF_CLOSE_OUTPUT;
//	owminfo.ol.OutputSet	= PF_TRACE_SET_ID;
//	owminfo.ol.Output		= PF_TRACE_EX_ID;

	// ����OWM����
	pIVciOwm->Command(OC_SETOWMLINK, (WPARAM)(&owminfo.ol), NULL);

	// ���ز˵�
	// ע��OWM�˵�,��ӵ������˵���
	// ע:��Xtreme����Ҫͨ��MenuBar��GetMenu��������ȡ��
	// �˵�����Դ,��CJLib��ֱ��ͨ��MainFrame��GetMenu�Ϳ�����
	//ol.RegisterClientMenu(m_pMainWnd->GetMenu(), m_aOWMMenus, IDM_OWMCMD, pDoc);
	
	// ������չ�˵�
	if(AfxGetMainWnd()->GetSafeHwnd() != NULL)
	{
		COWMMenuArray aOWMMenus;
		CXTPCommandBars* pCommandBars = ((CMainFrame*)AfxGetMainWnd())->GetCommandBars();
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
		pIVciOwm->RegisterClientMenu((CMenu*)pMenuBar,
						aOWMMenus, 40000, NULL);
	}

	// ����ToolBar
	if(m_nType & OWM_TOOLBAR)
	{/*
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CXTToolBar* pToolBar = &(pMainFrame->m_wndToolBar);
		int nStart = pToolBar->CommandToIndex(ID_VIEW_OUTPUT);

		pIVciOwm->RegisterClientToolBar(pToolBar, m_aOWMMenus, IDM_OWMCMD, nStart);
		pMainFrame->ShowControlBar(pToolBar, TRUE, FALSE);*/
	}

	// ִ��OWM�ĳ�ʼ������
	if(m_pIVciControl == NULL)
	{
		return FALSE;
	}
	((IVciControl*)m_pIVciControl)->Init(m_strInitParam);

	// ������һ��OWM����
	if((m_nType & OWM_DOC) && (m_nType & OWM_AUTOCREATE))
	{
		if((m_nType & OWM_CONTROLBAR) || (m_nType & OWM_TABCTRLBAR))
			//&& owminfo.ol.pCtrlBarClass)
			NewOwmControlBar();
		else
			NewOwmFrame(m_strInitParam);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����DockingPane
// nID = -1��ʾ���������Զ����ص�
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::LoadDockingPane(int nID, BOOL bProjectLoad)
{
	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetMainWnd());
	if(nID == -1)
	{
		// 2007-09-09�޸�,����Ҫ���ȼ���OWM,�ڴ���Ҫ��ʾʱ����Զ�����
		// �˶δ���ȡ��--��������δ���ؾ��ȼ���--
		//if((m_arDockingPane.GetSize() > 0) && (m_pVciObj == NULL))
		//{
		//	LoadOwmPlugIn();
		//}

		for(int i=0; i<m_arDockingPane.GetSize(); i++)
		{
			CXTPDockingPane* pPane = pMainFrame->GetPaneManager()->FindPane(m_arDockingPane[i].m_nId);
			// �����Ҫ�Զ����������߹��̴�ʱ���������ҹ������ڴ򿪣�������Pane�Ѿ�����,��ִ�д�������
			if(m_arDockingPane[i].m_bAutoCreate || (m_arDockingPane[i].m_bProjectCreate && bProjectLoad) || pPane)
			{
				CString strIconPath = m_arDockingPane[i].m_strIcon;
				if(strIconPath.Find(":") == -1)
				{
					strIconPath = m_strPath+"\\"+m_arDockingPane[i].m_strIcon;
				}
				WORD wIndex = 0;
				HICON hIcon = ::ExtractAssociatedIcon(theApp.m_hInstance,
					strIconPath.GetBuffer(0), &wIndex);
				strIconPath.ReleaseBuffer();
				if(m_arDockingPane[i].m_bAutoCreate || (m_arDockingPane[i].m_bProjectCreate && bProjectLoad))
				{
					pMainFrame->CreateDockingBar(m_arDockingPane[i].m_nId,
						m_arDockingPane[i].m_nDirection, hIcon);
				}else
				if(hIcon)
				{
					pMainFrame->GetPaneManager()->SetIcon(m_arDockingPane[i].m_nId,
						CXTPImageManagerIconHandle(hIcon));
				}
			}
		}
	}else
	{
		for(int i=0; i<m_arDockingPane.GetSize(); i++)
		{
			if(m_arDockingPane[i].m_nId == nID)
			{
				if(m_pVciObj == NULL)
				{
					LoadOwmPlugIn();
				}

				CString strIconPath = m_arDockingPane[i].m_strIcon;
				if(strIconPath.Find(":") == -1)
				{
					strIconPath = m_strPath+"\\"+m_arDockingPane[i].m_strIcon;
				}
				WORD wIndex = 0;
				HICON hIcon = ::ExtractAssociatedIcon(theApp.m_hInstance,
					strIconPath.GetBuffer(0), &wIndex);
				strIconPath.ReleaseBuffer();
				pMainFrame->CreateDockingBar(m_arDockingPane[i].m_nId,
					m_arDockingPane[i].m_nDirection, hIcon);
				return TRUE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �½�OWM���󴰿�(for Xtreme)
/////////////////////////////////////////////////////////////////////////////
void CPlugIn::NewOwmFrame(LPCTSTR lpszPath)
{
	if(m_pVciObj == NULL)
	{
		return;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	if((m_nType & OWM_DOC) && pDocTemplate)
	{
		// ���ֻ����һ��ʵ����������Ӧ����
		if((m_nType & OWM_ONEFRAME) &&
			(pDocTemplate->GetFirstDocPosition()))
			return;

		// ��һ����OWM�ĵ�
		//if(!pDocTemplate->OpenDocumentFile(lpszPath))
		if(!pDocTemplate->OpenDocumentFile(NULL))
		{
			return;
		}

		// ����OWM��չ�˵�
		if(m_nType & OWM_MENU)
		{
			// ȡ���ĵ�ָ��,��������չ�˵�ָ���¼���ĵ���,�ĵ��ر�ʱ���
			POSITION pos = pDocTemplate->GetFirstDocPosition();
			CDocument* pDoc = pDocTemplate->GetNextDoc(pos);
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)	// ����Ӵ���
			{
				CView* pView = pDoc->GetNextView(posview);	// ȡ����ָ��
				//pView->GetParentFrame()->ActivateFrame(); // ������
				if(m_nType & OWM_MAXWINDOW)
				{
					// ���������
					pView->GetParentFrame()->ShowWindow(SW_SHOWMAXIMIZED);
				}
			}
/*
			// ע��OWM�˵�,��ӵ������˵���
			// ע:��Xtreme����Ҫͨ��MenuBar��GetMenu��������ȡ��
			// �˵�����Դ,��CJLib��ֱ��ͨ��MainFrame��GetMenu�Ϳ�����
			//ol.RegisterClientMenu(m_pMainWnd->GetMenu(), m_aOWMMenus, IDM_OWMCMD, pDoc);
			CXTMenuBar* pMenuBar = ((CMainFrame*)AfxGetMainWnd())->GetMenuBar();
			pIVciOwm->RegisterClientMenu(pMenuBar->GetMenu(),
							m_aOWMMenus, IDM_OWMCMD, pDoc);

			// ˢ�²˵���
			// ע:��CJLib����DrawMenuBar�Ϳ�����,��Xtreme�б�����
			// DrawMenuBarX��ˢ��Xtreme�е�MenuBar��Menu��Դ�Ĺ���
			((CMainFrame*)AfxGetMainWnd())->DrawMenuBarX();

			// ע:����OWM���Xtreme toolkit�ĸĶ�
			// Xtreme��CJLibrary������Ƚϴ�,Cool�˵���Ϊͳһ����,
			// ͨ��CXTCoolMenu��������,������Լ������ͼ����Դ,Ȼ��
			// ��װ��һ��������,��װ�Ժ�,�û���������Բ˵������⴦��
			// �Ϳ���ʹ��Cool�˵���.����ǰ��CJLibʹ��CoolMenu�ķ�����
			// ÿ���˵���������ͼ����Դ.
			// ���,��OWM��Ҫ��ʵ����չ�˵�ʹ���Լ���ͼ����Դ,������
			// һЩ���⴦��,��ΪOWM�����������ʹ��ͬһ��CXTCoolMenu,
			// �����Ǻ����������Դ��ͻ�����,����Ҳ����ͨ����Դ�л�
			// �����,��ôΨһ�İ취���Ǳ�֤OWM����Ҫ��ӵ�������Ĳ�
			// �����������еĲ˵�����ԴID���غ�,��˹涨OWM����Ҫ��չ
			// ����ԴID������10000��20000��Χ��,����10000-11999Ϊϵͳ
			// OWM����,�û��Լ�д�ı����12000��ʼ.
			// Ϊ�˱�֤OWM�ӿڵļ�����,��˶Խӿڶ��岻���κθĶ�,��
			// OWM��RegisterClientMenu������Ҫ��ע���Լ���ͼ����Դ,
			// ����ͨ����ȡMainFrameָ��,Ȼ��ͨ��MainFrame�ҵ�CXTCoolMenu
			// ��ָ��������.
*/
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ر�OWM���󴰿�
/////////////////////////////////////////////////////////////////////////////
void CPlugIn::CloseOwmFrame()
{
	if(m_pVciObj == NULL)
	{
		return;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	if(pDocTemplate && (m_nType & OWM_DOC))
	{
		// �ر�����OWM�ĵ��ʹ���
		pDocTemplate->CloseAllDocuments(FALSE);

		// ����Ƕ�̬����OWM,��ж�ص���ģ��,�������ṹ
		if(m_nType & OWM_DYNLOAD)
		{/*
			for(int i=0; i<m_aOWMs.GetSize(); i++)
			{
				if((m_aOWMs[i].ol.nOWMID == ol.nOWMID) && m_aOWMs[i].bEnableDynLoad)
				{	// ֻ��ģ������������OWM_DYNLOAD,�����ݿ���Ҳ����������̬����,��̬���ز�������Ч
					TRACE("DynUnLoad a OWM[%d]: %s\n", m_aOWMs[i].ol.nOWMID, m_aOWMs[i].szName);
					UnLoadOWM(i, TRUE);
					break;
				}
			}*/
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �½�OWM����ControlBar(for Xtreme)
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::NewOwmControlBar()
{
	if(m_pVciObj == NULL)
	{
		return FALSE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	CRuntimeClass* pCtrlBarClass = NULL;
	pIVciOwm->GetRuntimeClass(4, &pCtrlBarClass);

	if(!pCtrlBarClass)
		return FALSE;

	if(m_pControlBar)
		return FALSE;

	// �˴��Ѿ��޸�Ϊ֧��Xtreme
	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetMainWnd());

	m_pControlBar = (CControlBar*)(pCtrlBarClass->CreateObject());

//	pMainFrame->EnableDockingEx(CBRS_ALIGN_ANY, CBRS_XT_FLAT);
//	pMainFrame->DockControlBar( pBar );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �ر�OWM����ControlBar
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CloseOwmControlBar()
{
	if(m_pVciObj == NULL)
	{
		return FALSE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	CRuntimeClass* pCtrlBarClass = NULL;
	pIVciOwm->GetRuntimeClass(4, &pCtrlBarClass);

	if(!pCtrlBarClass)
		return FALSE;

	if(!m_pControlBar)
		return FALSE;

	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetMainWnd());
	pMainFrame->ShowControlBar(m_pControlBar, FALSE, FALSE);
	delete m_pControlBar;
	m_pControlBar = NULL;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾ/����OWM����ControlBar
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::ShowOwmControlBar(BOOL bShow)
{
	if(m_pVciObj == NULL)
	{
		return FALSE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	CRuntimeClass* pCtrlBarClass = NULL;
	pIVciOwm->GetRuntimeClass(4, &pCtrlBarClass);

	if(!pCtrlBarClass)
		return FALSE;

	if(!m_pControlBar && !NewOwmControlBar())
		return FALSE;

	CMainFrame* pMainFrame = (CMainFrame*)(AfxGetMainWnd());
	pMainFrame->ShowControlBar(m_pControlBar, bShow, FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾOWM(View��ʽ)
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::ShowOwmView(BOOL bShow, BOOL bActive)
{
	if(m_pVciObj == NULL)
	{
		return FALSE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	if(pDocTemplate == NULL)
	{
		return FALSE;
	}

	POSITION posdoc = pDocTemplate->GetFirstDocPosition();
	if(!bShow)
	{
		if(posdoc)
		{
			CloseOwmFrame();
		}
		return TRUE;
	}

	if(!posdoc)
	{
		NewOwmFrame();
	}
	if(bActive)
	{
		POSITION posdoc = pDocTemplate->GetFirstDocPosition();
		if(posdoc)
		{
			CDocument* pDoc = pDocTemplate->GetNextDoc(posdoc);
			POSITION posview = pDoc->GetFirstViewPosition();
			if(posview)	// ����Ӵ���
			{
				CView* pOwmView = pDoc->GetNextView(posview);	// ȡ����ָ��
				pOwmView->GetParentFrame()->ActivateFrame(); // ������
				return TRUE;
			}
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ѯ���Ƿ���Թر�OWM,���������Ϣ
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CanCloseOwm()
{
	if(m_pVciObj == NULL)
	{
		return TRUE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	CMultiDocTemplate* pDocTemplate = NULL;
	pIVciOwm->GetDocTemplate(&pDocTemplate);

	if(pDocTemplate == NULL)
	{
		return TRUE;
	}

	if(!pDocTemplate->SaveAllModified())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �ر�һ��VCI���
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CloseOwm()
{
	if(m_pVciObj == NULL)
	{
		return TRUE;
	}

	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	//CMultiDocTemplate* pDocTemplate = NULL;
	//pIVciOwm->GetDocTemplate(&pDocTemplate);

	// ִ��Done����
	if(m_pIVciControl == NULL)
	{
		return FALSE;
	}
	((IVciControl*)m_pIVciControl)->Done(m_strDoneParam);

	// �ر�ControlBar
	if((m_nType & OWM_CONTROLBAR) || (m_nType & OWM_TABCTRLBAR))
		CloseOwmControlBar();

	// ж��ToolBar
	if(m_nType & OWM_TOOLBAR)
	{
		CMainFrame* pMainFrame = (CMainFrame*)(AfxGetMainWnd());
		//CXTToolBar* pToolBar = &(pMainFrame->m_wndToolBar);
		//pIVciOwm->UnregisterClientToolBar(pToolBar, m_aOWMMenus, IDM_OWMCMD);
		//pMainFrame->ShowControlBar(pToolBar, TRUE, FALSE);
	}

	// ����TCL��չ����(û�б�Ҫ�ڴ˴�ע��,��ΪTCLֻ�ڴ���������ʱ��Ч)
	if(m_nType & OWM_TCL)
	{
		// ע��TCL��չ����
		//if(owminfo.ol.UnregisterTclCmd)
		//	owminfo.ol.UnregisterTclCmd();
	}

	// ж����չ�˵�����
	if(m_nType & OWM_MENU)
	{
		COWMMenuArray aOWMMenus;
		aOWMMenus.Add(OWMMENUITEM());
		CXTPCommandBars* pCommandBars = ((CMainFrame*)AfxGetMainWnd())->GetCommandBars();
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
		aOWMMenus[0].pProc = (TYPEOF_ClientMenu*)pMenuBar;
		pIVciOwm->UnregisterClientMenu(aOWMMenus);
	}

	// �ͷ�OWM�ĵ�ģ��
	if((m_nType & OWM_DOC) && m_pDocTemplate)
	{
		// �ر�����OWM�ĵ��ʹ���
		POSITION pos = m_pDocTemplate->GetFirstDocPosition();
		if(pos != 0)
		{
			m_pDocTemplate->CloseAllDocuments(FALSE);
		}
		// ɾ��OWM�ĵ�ģ��
		delete m_pDocTemplate;
		m_pDocTemplate = NULL;
	}

	pIVciOwm->Release();

	m_pIVciControl = NULL;
	m_pVciObj = NULL;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ĵ�,������ܴ��򷵻�FALSE
/////////////////////////////////////////////////////////////////////////////
CDocument* CPlugIn::OpenDocument(LPCTSTR lpszFileName)
{
	CString strFileName = lpszFileName;
	strFileName.MakeLower();
	int nPos = strFileName.ReverseFind('.');
	if(nPos == -1)
	{
		return NULL;
	}

	CString strFileExt = strFileName;
	strFileExt.Delete(0, nPos+1);

	for(int i=0; i<m_arDocExtend.GetSize(); i++)
	{
		for(int j=0; j<m_arDocExtend[i].m_arDocExt.GetSize(); j++)
		{
			if(strFileExt == m_arDocExtend[i].m_arDocExt[j])
			{
				// ����Ƕ�̬���ص�OWM,����δ���ع�,���ȼ���
				if((m_pDocTemplate == NULL) && (m_nType & OWM_DYNLOAD))
				{
					LoadOwmPlugIn();
				}

				// ����ĵ�ģ��Ϊ��,������ĵ�ģ�岻��ʹ��,������������
				if(m_pDocTemplate == NULL)
				{
					continue;
				}

				// ���ж��Ƿ��Ѿ�����
				strFileName.Replace("/", "\\");

				CView* pView = NULL;
				POSITION posdoc = m_pDocTemplate->GetFirstDocPosition();
				while(posdoc != NULL)
				{
					CDocument* pDoc = m_pDocTemplate->GetNextDoc(posdoc);
					CString strFilePathExist = pDoc->GetPathName();
					strFilePathExist.MakeLower();
					strFilePathExist.Replace("/", "\\");
					if(strFilePathExist == strFileName)
					{
						// ����Ѿ����򼤻�
						POSITION posview = pDoc->GetFirstViewPosition();
						if(posview)	// ����Ӵ���
						{
							CView* pView = pDoc->GetNextView(posview);	// ȡ����ָ��
							pView->GetParentFrame()->ActivateFrame(); // ������
						}
						return pDoc;
					}
				}

				return m_pDocTemplate->OpenDocumentFile(lpszFileName);
			}
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡOWMע�������ҳָ������
/////////////////////////////////////////////////////////////////////////////
int CPlugIn::RegisterOption(COWMOptionArray& aOWMOption)
{
	IVciOwm* pIVciOwm = (IVciOwm*)m_pVciObj;
	return pIVciOwm->RegisterOption(aOWMOption);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡע�������ҳָ������
/////////////////////////////////////////////////////////////////////////////
int CPlugIn::RegisterPropertyPage(CVciPropertyPageArray& aPropertyPage)
{
	// ��ȡ����ָ��
	TYPEOF_RegisterPropertyPage fnRegisterPropertyPage = (TYPEOF_RegisterPropertyPage)GetProcAddress(m_hVciHandle, "gRegisterPropertyPage");
	if(fnRegisterPropertyPage)
	{
		return fnRegisterPropertyPage(aPropertyPage);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// �������̽ӿڶ���
/////////////////////////////////////////////////////////////////////////////
IProject* CPlugIn::CreateProjectObject(CString strInstName)
{
	IProject* pIProject = (IProject*)(CreateObject(strInstName));
	if(pIProject)
	{
		// ������չ�˵�
		COWMMenuArray aOWMMenus;
		CXTPCommandBars* pCommandBars = ((CMainFrame*)AfxGetMainWnd())->GetCommandBars();
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
		pIProject->RegisterMenu((CMenu*)pMenuBar, aOWMMenus, 40000);
	}

	return pIProject;
}

/////////////////////////////////////////////////////////////////////////////
// �ͷŹ��̽ӿڶ���
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::ReleaseProjectObject(LPVOID pVciObj, BOOL bForceDelete)
{
	if(m_strType != "project")
	{
		return FALSE;
	}

	IProject* pIProject = (IProject*)pVciObj;
	if(pIProject)
	{
		COWMMenuArray aOWMMenus;
		aOWMMenus.Add(OWMMENUITEM());
		CXTPCommandBars* pCommandBars = ((CMainFrame*)AfxGetMainWnd())->GetCommandBars();
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
		aOWMMenus[0].pProc = (TYPEOF_ClientMenu*)pMenuBar;
		pIProject->UnregisterMenu(aOWMMenus);
	}

	return ReleaseObject(pVciObj, bForceDelete);
}

/////////////////////////////////////////////////////////////////////////////
// ѯ���Ƿ���Թر�Interp
/////////////////////////////////////////////////////////////////////////////
BOOL CPlugIn::CanCloseInterp()
{
	if(m_pVciObj == NULL)
	{
		return TRUE;
	}

	//IInterp* pIInterp = (IInterp*)m_pVciObj;
	//return !(pIInterp->IsInterpRunning());

	for(int i=0; i<m_aVciObj.GetSize(); i++)
	{
		IInterp* pIInterp = (IInterp*)(m_aVciObj[i].m_pVciObj);
		if(pIInterp->IsInterpRunning())
		{
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ������Ϣ
/////////////////////////////////////////////////////////////////////////////
int CPlugIn::SendMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	if(m_pIVciControl == NULL)
	{
		// ������ʵ��δ����,�򴴽�ʵ������
		if(m_strType == "owm")
		{
			// OWM�����Ҫ�Ƚ��м��غͳ�ʼ��
			if(!LoadOwmPlugIn())
			{
				return -1;
			}
		}else
		{
			if(CreateObject() == NULL)
			{
				return -1;
			}
		}
	}

	if(pIn->GetSync() == VMT_SYNC)
	{
		// ͬ����Ϣ,ֱ�ӵ��ò�����
		return ((IVciControl*)m_pIVciControl)->ProcessMessage(pIn, ppOut);
	}else
	if(pIn->GetSync() == VMT_ASYNC)
	{
		// �첽��Ϣ,������Ϣ����
		if(theApp.m_pIMessageQueue)
		{
			theApp.m_pIMessageQueue->PostMessage(pIn, m_strId);
		}
	}

	return 0;
}

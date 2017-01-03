#include "stdafx.h"
#include "CVciOwm.h"
#include "DuiVisionDesigner.h"

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨·��
/////////////////////////////////////////////////////////////////////////////
CString GetPlatRootPath()
{
	//��ȡƽ̨·��
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


//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CVciOwm::CVciOwm()
{
}

CVciOwm::~CVciOwm()
{
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CVciOwm::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int CVciOwm::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
int CVciOwm::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	return 0;
}

// ���õ���ģʽ
int CVciOwm::SetDebugMode(BOOL bDebug)
{
	theApp.m_bDebug	= bDebug;
	return CVisualComponent::SetDebugMode(bDebug);
}

// ��������
int CVciOwm::SetLanguage(int nLanguage)
{
	theApp.m_nLanguage = nLanguage;
	return CVisualComponent::SetLanguage(nLanguage);
}

// ��ʼ�����
int CVciOwm::Init(LPCSTR lpcsParams)
{
	return InitOWM();
}

// �������
int CVciOwm::Done(LPCSTR lpcsParams)
{
	return DoneOWM();
}

//{{VCI_IMPLEMENT_BEGIN
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(VciOwm)
CONTROL_INTERFACE_IMPLEMENT(VciOwm)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CVciOwm::XVciOwm::GetExtraInfo(LPVOID lpRefData)
{
	CVciOwm *pObj = GET_INTERFACE_OBJECT(VciOwm);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

//{{VCI_INTERFACE_IMPLEMENT_BEGIN
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

// Theme ע����
#define REG_CONFIG_SUBKEY		_T("Settings")
#define REG_CONFIG_SKIN_THEME	_T("SkinTheme")			// ����

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall CVciOwm::XVciOwm::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	CVciOwm *pObj = GET_INTERFACE_OBJECT(VciOwm);

	XTPTaskPanelPaintTheme paneTheme = xtpTaskPanelThemeShortcutBarOffice2003;
	
	int nTheme = AfxGetApp()->GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_THEME, xtpThemeRibbon);
	switch(nTheme)
	{
	case xtpThemeOffice2000: paneTheme = xtpTaskPanelThemeOffice2000; break;
	case xtpThemeOfficeXP: paneTheme = xtpTaskPanelThemeListViewOfficeXP; break;
	case xtpThemeOffice2003: paneTheme = xtpTaskPanelThemeListViewOffice2003; break;
	case xtpThemeNativeWinXP: paneTheme = xtpTaskPanelThemeNativeWinXP; break;
	case xtpThemeWhidbey: paneTheme = xtpTaskPanelThemeToolboxWhidbey; break;
	case xtpThemeOffice2007: paneTheme = xtpTaskPanelThemeShortcutBarOffice2003; break;
	case xtpThemeRibbon: paneTheme = xtpTaskPanelThemeShortcutBarOffice2007; break;
	}

	CWnd* pWnd = (CWnd*)(new CXTPTaskPanel());
	theApp.m_pToolboxPane = (CXTPTaskPanel*)pWnd;
	theApp.m_pToolboxPane->Create(WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), pParentWnd, nID);//DOCKPANE_ID_TOOLBOX);
	theApp.m_pToolboxPane->SetOwner(pParentWnd);
	theApp.m_pToolboxPane->SetBehaviour(xtpTaskPanelBehaviourToolbox);
	theApp.m_pToolboxPane->SetTheme(paneTheme);
	theApp.m_pToolboxPane->SetSelectItemOnFocus(TRUE);
	theApp.m_pToolboxPane->SetGroupIconSize(CSize(16, 16));
	theApp.m_pToolboxPane->Reposition();
	theApp.m_pToolboxPane->AllowDrag(FALSE);
	theApp.m_pToolboxPane->EnableAnimation(TRUE);

	// ����������
	get_dll_resource();

	// ����toolbox�����ļ�
	CString strToolboxPath = theApp.GetModulePath() + "\\toolbox";
	if(LANGUAGE_CHINESE == pObj->GetLanguage())
	{
		strToolboxPath += "\\cn";
	}else
	{
		strToolboxPath += "\\en";
	}

	theApp.m_pToolboxPane->GetGroups()->Clear(FALSE);

	CString strToolboxFile = strToolboxPath + "\\duivision_base_control_cn.xml";
	pObj->LoadToolbox(strToolboxFile);

	reset_dll_resource();

	return pWnd;
}

// ����Toolbox�����ļ�
BOOL CVciOwm::LoadToolbox(LPCTSTR lpszToolboxFile)
{
	if(GetFileAttributes(lpszToolboxFile) == 0xFFFFFFFF)
	{
		return FALSE;
	}

	CXmlParser parser;
	if(parser.Open(lpszToolboxFile))
	{
		DEBUG_OUTF(LOG_LEVEL_ERROR, "Load toolbox file %s fail!", lpszToolboxFile);
		return FALSE;
	}

	CString strImagePath = theApp.GetModulePath();

	BOOL bRet = TRUE;
	DOMNode* pNode = parser.GetRootNode();

	// ����ͼƬ��Դ
	DOMNode* pImageNode = parser.GetChildNode(pNode, "image");
	while (pImageNode != NULL)
	{
		CString strIconFile = strImagePath + "\\" + parser.GetNodeAttribute(pImageNode, "file");
		int nImageId = atoi(parser.GetNodeAttribute(pImageNode, "id"));

		// ���ص���ͼƬ
		if(!theApp.SetToolboxIcon(strIconFile, nImageId))
		{
			bRet = FALSE;
		}

		pImageNode = parser.GetNextNode(pImageNode, "image");
	}

	DOMNode* pImageListNode = parser.GetChildNode(pNode, "imagelist");
	while (pImageListNode != NULL)
	{
		CString strBmpFile = strImagePath + "\\" + parser.GetNodeAttribute(pImageListNode, "file");
		CString strIDs = parser.GetNodeText(pImageListNode);
		CUIntArray auID;
		int nPos = strIDs.Find(",");
		while(nPos != -1)
		{
			auID.Add(atoi(strIDs.Left(nPos)));
			strIDs.Delete(0, nPos+1);
			nPos = strIDs.Find(",");
		}
		auID.Add(atoi(strIDs));

		// ����ͼƬ�б�
		if(!theApp.SetToolboxIcons(strBmpFile, &auID))
		{
			bRet = FALSE;
		}

		pImageListNode = parser.GetNextNode(pImageListNode, "imagelist");
	}

	// ���ؿؼ���Ϳؼ�
	DOMNode* pGroupNode = parser.GetChildNode(pNode, "group");
	while (pGroupNode != NULL)
	{
		CString strGroupName = parser.GetNodeAttribute(pGroupNode, "name");
		int nGroupId = atoi(parser.GetNodeAttribute(pGroupNode, "id"));
		BOOL bExpand = (parser.GetNodeAttribute(pGroupNode, "expand") == "true");
		CXTPTaskPanelGroup* pFolderClass = theApp.CreateToolboxGroup(nGroupId, strGroupName);

		// ���ؿؼ�
		DOMNode* pItemNode = parser.GetChildNode(pGroupNode, "control");
		while (pItemNode != NULL)
		{
			CString strControlName = parser.GetNodeAttribute(pItemNode, "name");
			int nControlImageId = atoi(parser.GetNodeAttribute(pItemNode, "image-id"));
			CXTPTaskPanelGroupItem* pItem = pFolderClass->AddLinkItem(nControlImageId, nControlImageId);
			pItem->SetCaption(theApp.StripMnemonics(strControlName));

			pItemNode = parser.GetNextNode(pItemNode, "control");
		}

		pFolderClass->SetExpanded(bExpand);

		pGroupNode = parser.GetNextNode(pGroupNode, "group");
	}

	parser.Close();

	return TRUE;
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
//}}VCI_INTERFACE_IMPLEMENT_END

// ��ѯ�����Ƿ�֧��
extern "C" __declspec(dllexport)
int gQueryCommandIsSupport(LPCSTR lpcsCmd)
{
	//if(strcmp(lpcsCmd, "HELLO") == 0)
	//{
	//}
	return 0;
}

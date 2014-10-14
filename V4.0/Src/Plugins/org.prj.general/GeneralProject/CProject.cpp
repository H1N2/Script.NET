#include "stdafx.h"
#include "CProject.h"

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

/////////////////////////////////////////////////////////////////////////////
// ��ȡ���·��
/////////////////////////////////////////////////////////////////////////////
CString GetPlugInRootPath()
{
	//��ȡƽ̨·��
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(GetDllInstance(), szFullPath, MAX_PATH);
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);
	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);

	return szPath;
}

static CProject* g_Project = NULL;

IPlatUI* gGetIPlatUI()
{
	if(g_Project)
	{
		IPlatUI* pIPlatUI = (IPlatUI*)(g_Project->getIPlatUI());
		return pIPlatUI;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CProject::CProject()
{
	g_Project = this;
}

CProject::~CProject()
{
	g_Project = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CProject::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int CProject::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
int CProject::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	return 0;
}


//{{VCI_IMPLEMENT_BEGIN
// �½�����
int __stdcall CProject::NewProject(LPCTSTR lpszProjectName, LPCTSTR lpszProjectPath, LPCTSTR lpszProjectDesc, LPCTSTR lpszAuthor)
{
	// TODO: Add your code here

	return trpOk;
}

// �򿪹���
int __stdcall CProject::OpenProject(LPCTSTR lpszProjectFile)
{
	// TODO: Add your code here

	return trpOk;
}

// ���湤��
int __stdcall CProject::SaveProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ִ�й���
int __stdcall CProject::RunProject()
{
	// TODO: Add your code here

	return trpOk;
}

// �رչ���
int __stdcall CProject::CloseProject(BOOL bForce)
{
	// TODO: Add your code here

	return trpOk;
}

// ���빤��
int __stdcall CProject::BuildProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ���±��빤��
int __stdcall CProject::RebuildProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ������̱�����Ϣ
int __stdcall CProject::CleanProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ��ʾ������Ϣ
int __stdcall CProject::ProjectInformation()
{
	// TODO: Add your code here

	return trpOk;
}

// ˢ�����Դ��ڵĹ�����Ϣ
int __stdcall CProject::RefreshProjectProperty()
{
	// TODO: Add your code here

	return trpOk;
}

// ������������
int __stdcall CProject::ProjectPropertyPage(CVciPropertyPageArray& aPropertyPage)
{
	// TODO: Add your code here

	return aPropertyPage.GetSize();
}

// ��ȡ�򿪵Ĺ����б�
int __stdcall CProject::GetOpenProjectList(CStringArray& asProjectFile)
{
	// TODO: Add your code here

	return 1;
}

// ��ȡ��ǰ����Ĺ���
int __stdcall CProject::GetActiveProject(CString& strProject)
{
	// TODO: Add your code here

	return trpOk;
}

// ���ü���Ĺ���
int __stdcall CProject::SetActiveProject(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return trpOk;
}

// ��ȡ������Ϣ
int __stdcall CProject::GetProjectInfo(LPCTSTR lpszProject, CString& strProjectDesc, CString& strProjectFile, CString& strProjectPath)
{
	// TODO: Add your code here

	return trpOk;
}

// ��ȡ����״̬
int __stdcall CProject::GetProjectState(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return PROJECT_IDLE;
}

// ע��˵�
int __stdcall CProject::RegisterMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{
	// TODO: Add your code here

	return 0;
}

// ж�ز˵�
int __stdcall CProject::UnregisterMenu(COWMMenuArray& aOWMMenus)
{
	// TODO: Add your code here

	return 0;
}

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall CProject::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	
	return NULL;
}

// ��ʼ�����붨���
int __stdcall CProject::InitCodeDefineLibrary(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return 0;
}

// ���ش��붨���
int __stdcall CProject::LoadCodeDefineLibrary(LPCTSTR lpszProject)
{
	// TODO: Add your code here
	// ����ⲻ����,���Ƚ��п�ĳ�ʼ��

	return 0;
}

// ���Ҵ��붨���б�(����ֵ��ʾ����)
int __stdcall CProject::GetCodeDefine(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)
{
	// TODO: Add your code here

	return 0;
}

// ���Ҵ��������б�(����ֵ��ʾ����)
int __stdcall CProject::GetCodeReference(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)
{
	// TODO: Add your code here

	return 0;
}
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(Project)
CONTROL_INTERFACE_IMPLEMENT(Project)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CProject::XProject::GetExtraInfo(LPVOID lpRefData)
{
	CProject *pObj = GET_INTERFACE_OBJECT(Project);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

//{{VCI_INTERFACE_IMPLEMENT_BEGIN
	PROJECT_INTERFACE_IMPLEMENT
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

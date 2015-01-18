////////////////////////////////////////////////////////////////////////////
//	File:		IInterp.h
//	Version:	2.0.0.0
//	Created:	2003-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	interface of project
////////////////////////////////////////////////////////////////////////////
#if !defined(__VCI_INTERFACE_IProject_H_INCLUDED__)
#define __VCI_INTERFACE_IProject_H_INCLUDED__


#define IID_IProject "IProject"

// ���ؽ��
enum{
	trpOk,					// ����
	trpFail,				// �򿪹���ʧ��
	trpCancel,				// ����ȡ��
	trpWorking,				// ������������
};

// ����״̬
enum{
	PROJECT_CLOSE,			// �ر�
	PROJECT_IDLE,			// ����
	PROJECT_WORKING,		// ����
};

//
// ����λ��
//
class CCodePosition : public CObject
{
public:
	CCodePosition()
	{
		m_strCodeFile = _T("");
		m_nCodeLine = -1;
	}
	~CCodePosition() {};

	CCodePosition(const CCodePosition& other)
	{
		m_strCodeFile	= other.m_strCodeFile;
		m_nCodeLine		= other.m_nCodeLine;
	}

	CCodePosition& operator = (const CCodePosition& other)
	{
		m_strCodeFile	= other.m_strCodeFile;
		m_nCodeLine		= other.m_nCodeLine;
		return *this;
	}

public:
	CString	m_strCodeFile;			// �ļ�
	int		m_nCodeLine;			// ������
};

typedef CArray<CCodePosition, CCodePosition&> CCodePositionArray;


interface IProject : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
	// �½�����
	virtual int __stdcall NewProject(LPCTSTR lpszProjectName, LPCTSTR lpszProjectPath, LPCTSTR lpszProjectDesc, LPCTSTR lpszAuthor) = 0;
	// �򿪹���
	virtual int __stdcall OpenProject(LPCTSTR lpszProjectFile) = 0;
	// ���湤��
	virtual int __stdcall SaveProject() = 0;
	// ִ�й���
	virtual int __stdcall RunProject() = 0;
	// �رչ���
	virtual int __stdcall CloseProject(BOOL bForce) = 0;
	// ���빤��
	virtual int __stdcall BuildProject() = 0;
	// ���±��빤��
	virtual int __stdcall RebuildProject() = 0;
	// ������̱�����Ϣ
	virtual int __stdcall CleanProject() = 0;
	// ��ʾ������Ϣ
	virtual int __stdcall ProjectInformation() = 0;
	// ˢ�����Դ��ڵĹ�����Ϣ
	virtual int __stdcall RefreshProjectProperty() = 0;
	// ������������
	virtual int __stdcall ProjectPropertyPage(CVciPropertyPageArray& aPropertyPage) = 0;
	// ��ȡ�򿪵Ĺ����б�
	virtual int __stdcall GetOpenProjectList(CStringArray& asProjectFile) = 0;
	// ��ȡ��ǰ����Ĺ���
	virtual int __stdcall GetActiveProject(CString& strProject) = 0;
	// ���ü���Ĺ���
	virtual int __stdcall SetActiveProject(LPCTSTR lpszProject) = 0;
	// ��ȡ������Ϣ
	virtual int __stdcall GetProjectInfo(LPCTSTR lpszProject, CString& strProjectDesc, CString& strProjectFile, CString& strProjectPath) = 0;
	// ��ȡ����״̬
	virtual int __stdcall GetProjectState(LPCTSTR lpszProject) = 0;
	// ע��˵�
	virtual int __stdcall RegisterMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart) = 0;
	// ж�ز˵�
	virtual int __stdcall UnregisterMenu(COWMMenuArray& aOWMMenus) = 0;
	// ��ʾDockingPane����ʱ���õĺ���
	virtual CWnd* __stdcall OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam) = 0;
	// ��ʼ�����붨���
	virtual int __stdcall InitCodeDefineLibrary(LPCTSTR lpszProject) = 0;
	// ���ش��붨���
	virtual int __stdcall LoadCodeDefineLibrary(LPCTSTR lpszProject) = 0;
	// ���Ҵ��붨���б�
	virtual int __stdcall GetCodeDefine(LPCTSTR lpszCode, CCodePositionArray& asCodePosition) = 0;
	// ���Ҵ��������б�
	virtual int __stdcall GetCodeReference(LPCTSTR lpszCode, CCodePositionArray& asCodePosition) = 0;
	//}}VCI_INTERFACE_END
};


// Project�ӿڶ��帨����
#define PROJECT_INTERFACE_DEFINE \
	virtual int __stdcall NewProject(LPCTSTR lpszProjectName, LPCTSTR lpszProjectPath, LPCTSTR lpszProjectDesc, LPCTSTR lpszAuthor);	\
	virtual int __stdcall OpenProject(LPCTSTR lpszProjectFile);	\
	virtual int __stdcall SaveProject();	\
	virtual int __stdcall RunProject();	\
	virtual int __stdcall CloseProject(BOOL bForce);	\
	virtual int __stdcall BuildProject();	\
	virtual int __stdcall RebuildProject();	\
	virtual int __stdcall CleanProject();	\
	virtual int __stdcall ProjectInformation();	\
	virtual int __stdcall RefreshProjectProperty();	\
	virtual int __stdcall ProjectPropertyPage(CVciPropertyPageArray& aPropertyPage);	\
	virtual int __stdcall GetOpenProjectList(CStringArray& asProjectFile);	\
	virtual int __stdcall GetActiveProject(CString& strProject);	\
	virtual int __stdcall SetActiveProject(LPCTSTR lpszProject);	\
	virtual int __stdcall GetProjectInfo(LPCTSTR lpszProject, CString& strProjectDesc, CString& strProjectFile, CString& strProjectPath);	\
	virtual int __stdcall GetProjectState(LPCTSTR lpszProject);	\
	virtual int __stdcall RegisterMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart);	\
	virtual int __stdcall UnregisterMenu(COWMMenuArray& aOWMMenus);	\
	virtual CWnd* __stdcall OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam);	\
	virtual int __stdcall InitCodeDefineLibrary(LPCTSTR lpszProject);	\
	virtual int __stdcall LoadCodeDefineLibrary(LPCTSTR lpszProject);	\
	virtual int __stdcall GetCodeDefine(LPCTSTR lpszCode, CCodePositionArray& asCodePosition);	\
	virtual int __stdcall GetCodeReference(LPCTSTR lpszCode, CCodePositionArray& asCodePosition);


// Project�ӿ�ʵ�ָ�����
#define PROJECT_INTERFACE_IMPLEMENT \
int __stdcall CProject::XProject::NewProject(LPCTSTR lpszProjectName, LPCTSTR lpszProjectPath, LPCTSTR lpszProjectDesc, LPCTSTR lpszAuthor)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->NewProject(lpszProjectName, lpszProjectPath, lpszProjectDesc, lpszAuthor);	\
}	\
int __stdcall CProject::XProject::OpenProject(LPCTSTR lpszProjectFile)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->OpenProject(lpszProjectFile);	\
}	\
int __stdcall CProject::XProject::SaveProject()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->SaveProject();	\
}	\
int __stdcall CProject::XProject::RunProject()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->RunProject();	\
}	\
int __stdcall CProject::XProject::CloseProject(BOOL bForce)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->CloseProject(bForce);	\
}	\
int __stdcall CProject::XProject::BuildProject()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->BuildProject();	\
}	\
int __stdcall CProject::XProject::RebuildProject()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->RebuildProject();	\
}	\
int __stdcall CProject::XProject::CleanProject()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->CleanProject();	\
}	\
int __stdcall CProject::XProject::ProjectInformation()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->ProjectInformation();	\
}	\
int __stdcall CProject::XProject::RefreshProjectProperty()	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->RefreshProjectProperty();	\
}	\
int __stdcall CProject::XProject::ProjectPropertyPage(CVciPropertyPageArray& aPropertyPage)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->ProjectPropertyPage(aPropertyPage);	\
}	\
int __stdcall CProject::XProject::GetOpenProjectList(CStringArray& asProjectFile)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetOpenProjectList(asProjectFile);	\
}	\
int __stdcall CProject::XProject::GetActiveProject(CString& strProject)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetActiveProject(strProject);	\
}	\
int __stdcall CProject::XProject::SetActiveProject(LPCTSTR lpszProject)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->SetActiveProject(lpszProject);	\
}	\
int __stdcall CProject::XProject::GetProjectInfo(LPCTSTR lpszProject, CString& strProjectDesc, CString& strProjectFile, CString& strProjectPath)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetProjectInfo(lpszProject, strProjectDesc, strProjectFile, strProjectPath);	\
}	\
int __stdcall CProject::XProject::GetProjectState(LPCTSTR lpszProject)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetProjectState(lpszProject);	\
}	\
int __stdcall CProject::XProject::RegisterMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->RegisterMenu(pMenu, aOWMMenus, nOWMIdStart);	\
}	\
int __stdcall CProject::XProject::UnregisterMenu(COWMMenuArray& aOWMMenus)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->UnregisterMenu(aOWMMenus);	\
}	\
CWnd* __stdcall CProject::XProject::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->OnShowDockingPane(nID, pParentWnd, lpszParam);	\
}	\
int __stdcall CProject::XProject::InitCodeDefineLibrary(LPCTSTR lpszProject)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->InitCodeDefineLibrary(lpszProject);	\
}	\
int __stdcall CProject::XProject::LoadCodeDefineLibrary(LPCTSTR lpszProject)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->LoadCodeDefineLibrary(lpszProject);	\
}	\
int __stdcall CProject::XProject::GetCodeDefine(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetCodeDefine(lpszCode, asCodePosition);	\
}	\
int __stdcall CProject::XProject::GetCodeReference(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)	\
{	\
	CProject *pObj = GET_INTERFACE_OBJECT(Project);	\
	return pObj->GetCodeReference(lpszCode, asCodePosition);	\
}


// ���һ����չ�˵��������˵���ĳ���˵�������(����ƽ̨�ӿ�)
#define	PLATUI_ADDPRJMENUITEM(mainmenu, menuid, menuindex, proc, updateproc)	\
	{	\
		get_dll_resource();	\
		CMenu* pmenu = new CMenu();	\
		pmenu->LoadMenu(menuid);	\
		reset_dll_resource();	\
		IPlatUI* pIPlatUI = gGetIPlatUI();	\
		pIPlatUI->AddSubMenu(mainmenu, menuindex, pmenu, proc, updateproc);	\
		delete pmenu;	\
	}	\


#endif // !defined(__VCI_INTERFACE_IProject_H_INCLUDED__)

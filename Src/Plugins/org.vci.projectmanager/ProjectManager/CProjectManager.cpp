#include "stdafx.h"
#include "CProjectManager.h"
#include "NewPrjPage.h"
#include "NewFilePage.h"

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
// ��·���ַ������д���
/////////////////////////////////////////////////////////////////////////////
void ProcessPath(CString& strPath, CString strBasePath)
{
	CString strTemp;
	if(strPath.Find(_T("PLAT://")) == 0)
	{
		strTemp = strPath.Right(strPath.GetLength()-7);
		//strTemp.Replace("/", "\\");
		if(strTemp.Find(_T("\\")) == 0)
		{	// GetPlatRootPath�Ѿ�������\,��˿���ȥ��
			strTemp = strTemp.Right(strTemp.GetLength()-1);
		}
		strPath = GetPlatRootPath() + strTemp;
	}else
	{
		strPath.Replace("/", "\\");
		if(strPath.Find(_T(":")) < 0)
		{	// ���û��·��,��ʾ��ƽ̨Ŀ¼
			strTemp = strPath;
			if(strBasePath == "")
			{
				strPath = GetPlatRootPath() + "Bin\\" + strTemp;
			}else
			{
				// ���strBasePath�ǿգ����ô���Ϊ����·��
				strPath = strBasePath + "\\" + strTemp;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CProjectManager::CProjectManager()
{
}

CProjectManager::~CProjectManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CProjectManager::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int CProjectManager::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
int CProjectManager::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	return 0;
}


//{{VCI_IMPLEMENT_BEGIN
/////////////////////////////////////////////////////////////////////////////
// �����½�������
/////////////////////////////////////////////////////////////////////////////
int CProjectManager::NewProjectWizard(CString& strPrjVci, CString& strPrjFile)
{
	IPlatUI* pIPlatUI = (IPlatUI*)getIPlatUI();
	if(pIPlatUI == NULL)
	{
		return FALSE;
	}

	// ���ȹرյ�ǰ����
	IProject* pOldIProject = (IProject*)pIPlatUI->GetObjectByInstanceName("###project###");
	if(pOldIProject != NULL)
	{
		CString strActiveProject;
		if(pOldIProject->GetActiveProject(strActiveProject) == trpOk)
		{
			DLL_LOADSTRING(strConfirm, IDS_CONFIRM_CLOSEPROJECT)
			if(AfxMessageBox(strConfirm, MB_YESNO | MB_ICONQUESTION) != IDYES)
			{
				return FALSE;
			}
		}
		pIPlatUI->ReleaseObjectByInstanceName("###project###");
	}

	// �½����̶Ի���
	get_dll_resource();
	CNewPrjPage newPrjDlg;
	if(newPrjDlg.DoModal() != IDOK)
	{
		return FALSE;
	}

	// ���������·��
	CString _strPrjVci = newPrjDlg.m_strPrjVci;
	CString strPrjVciPath = GetPlatRootPath();
	strPrjVciPath += "\\plugins\\";
	strPrjVciPath += _strPrjVci;

	// ��������Ŀ¼
	CString strProjectPath = newPrjDlg.m_strPrjPath;
	CString strProjectName = newPrjDlg.m_strPrjName;
	CreateDirectory(strProjectPath + "\\" + strProjectName, 0);

	CString strTemplate = newPrjDlg.m_strTemplate;

	// ��������˽ű��������͹��̽ű�,���ȵ��ýű�������ִ���½����̽ű�
	IInterp* pInterp = NULL;
	CString strInterp = newPrjDlg.m_strInterp;
	CString strScript = newPrjDlg.m_strScript;
	if((strInterp.GetLength() != 0) && (strScript.GetLength() != 0))
	{
		if(strInterp == "org.interp.tcl")
		{
			pInterp = (IInterp*)(pIPlatUI->CreateVciObject(strInterp, ""));
			if(pInterp)
			{
				int nRet = pInterp->RunScriptFile(strScript);
				if(!nRet)
				{
					AfxMessageBox(pInterp->GetResult());
				}
				if(nRet)
				{
					nRet = pInterp->RunScriptCommand("TProject project");
				}
				if(!nRet)
				{
					AfxMessageBox(pInterp->GetResult());
				}
				CString strCommand;
				strProjectPath.Replace("\\", "/");
				strProjectPath.Replace("\\", "/");
				strTemplate.Replace("\\", "/");
				strCommand.Format("set project_file [project New \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"]",
					strProjectName, strProjectPath + "/" + strProjectName, newPrjDlg.m_strPrjType,
					newPrjDlg.m_strPrjDesc, newPrjDlg.m_strAuthor,
					strTemplate);
				if(nRet)
				{
					nRet = pInterp->RunScriptCommand(strCommand);
				}
				if(!nRet)
				{
					AfxMessageBox(pInterp->GetResult());
				}
			}
		}
	}

	// ���ù��̶���Ĵ����ӿ�
	IProject* pIProject = (IProject*)(pIPlatUI->CreateVciObject(_strPrjVci, "###project###"));
	if(pIProject)
	{
		pIProject->NewProject(strProjectName, strProjectPath + "\\" + strProjectName,
							newPrjDlg.m_strPrjDesc, newPrjDlg.m_strAuthor);
	}

	// ��������˽ű��������͹��̽ű�,����ýű��Ĺ��̴���֮��ִ�еĺ���
	if((strInterp.GetLength() != 0) && (strScript.GetLength() != 0) && (pInterp != NULL))
	{
		if(strInterp == "org.interp.tcl") 
		{
			CString strCommand;
			strCommand.Format("project AfterNew \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
				strProjectName, strProjectPath + "/" + strProjectName, newPrjDlg.m_strPrjType,
				newPrjDlg.m_strPrjDesc, newPrjDlg.m_strAuthor,
				strTemplate);
			if(!pInterp->RunScriptCommand(strCommand))
			{
				AfxMessageBox(pInterp->GetResult());
			}
		}
	}

	strPrjVci = _strPrjVci;
	strPrjFile = strProjectPath + "\\" + strProjectName + "\\" + strProjectName + "." + newPrjDlg.m_strPrjExt;

	// ���ô򿪹��̵Ľӿ�
	/*if(pIProject)
	{
		pIProject->OpenProject(strProjectPath + "\\" + strProjectName + "\\" + strProjectName + "." + newPrjDlg.m_strPrjExt);
	}*/

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����½��ļ���
/////////////////////////////////////////////////////////////////////////////
int CProjectManager::NewFileWizard()
{
	get_dll_resource();
	CNewFilePage newFileDlg;
	if(newFileDlg.DoModal() != IDOK)
	{
		return 0;
	}

	// ����д򿪵Ĺ��̣�����ù��̵��½��ļ��ӿ�

	// ����ֱ�Ӱ���ģ�����½��ļ�

	return 0;
}
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(ProjectManager)
CONTROL_INTERFACE_IMPLEMENT(ProjectManager)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CProjectManager::XProjectManager::GetExtraInfo(LPVOID lpRefData)
{
	CProjectManager *pObj = GET_INTERFACE_OBJECT(ProjectManager);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

//{{VCI_INTERFACE_IMPLEMENT_BEGIN
// �����½�������
int __stdcall CProjectManager::XProjectManager::NewProjectWizard(CString& strPrjVci, CString& strPrjFile)
{
	CProjectManager *pObj = GET_INTERFACE_OBJECT(ProjectManager);
	return pObj->NewProjectWizard(strPrjVci, strPrjFile);
}

// �����½��ļ���
int __stdcall CProjectManager::XProjectManager::NewFileWizard()
{
	CProjectManager *pObj = GET_INTERFACE_OBJECT(ProjectManager);
	return pObj->NewFileWizard();
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

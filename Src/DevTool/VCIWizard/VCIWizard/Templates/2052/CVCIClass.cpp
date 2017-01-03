#include "stdafx.h"
#include "C[!output INTERFACE_NAME].h"
[!if VCITYPE_OWM]
#include "[!output PROJECT_NAME].h"
[!endif]

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

[!if VCITYPE_INTERP]
/////////////////////////////////////////////////////////////////////////////
// ��ȡ������·��
/////////////////////////////////////////////////////////////////////////////
CString GetInterpRootPath()
{
	//��ȡƽ̨·��
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(GetDllInstance(), szFullPath, MAX_PATH);
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
[!endif]
[!if VCITYPE_INTERP]
//////////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////////
static int g_InterpID = 0x100;	// ������ID������
[!endif]
[!if VCITYPE_PROJECT]
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
[!endif]

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
C[!output INTERFACE_NAME]::C[!output INTERFACE_NAME]()
{
[!if VCITYPE_INTERP]
	m_nInterpID		= g_InterpID++;
	if(g_InterpID > 0x01000000)
		g_InterpID  = 0x100;
	pIPlatUI		= NULL;
	m_dwRunThreadId	= NULL;
	m_strInterpName	= _T("");
	m_strScriptFile	= _T("");
	m_nRunMode		= SCRIPT_RUN_NORMAL;
	m_nRunState		= SCRIPT_STATE_IDLE;
	m_nRunLevel		= SCRIPT_LEVEL_NONE;
	m_nEvalResult	= 0;
	m_bStopSignal	= FALSE;
	m_bBreakSignal	= FALSE;
	m_nBreakMode	= SCRIPT_BREAK_NORMAL;
	m_nBreakLineNo	= 0;
	m_strBreakName	= _T("");
	m_nBreakLevel		= 0;
	m_nStackLevel	= 0;
	m_bProfileEnable= FALSE;
	m_nTDLineCount	= 0;
	m_ProfileMap.InitHashTable(128);	// ��ʼ�������ʷ�����ϣ��
	m_bHide			= FALSE;
	m_lpfnExit		= NULL;
	m_hPipeProcess	= NULL;
	m_bPipeProcessTerminated = FALSE;
[!endif]
[!if VCITYPE_PROJECT]
	g_Project = this;
[!endif]
}

C[!output INTERFACE_NAME]::~C[!output INTERFACE_NAME]()
{
[!if VCITYPE_INTERP]
	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();

	try {
	}catch(...)
	{
	}
[!endif]
[!if VCITYPE_PROJECT]
	g_Project = NULL;
[!endif]
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int C[!output INTERFACE_NAME]::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

[!if VCITYPE_INTERP]
	if(nEventID == CALLBACK_EVENT_INTERP_EXIT)
	{
		if(lpHandler == NULL)
		{
			return -1;
		}

		// ��װ�˳��ص�����
		m_lpfnExit = (INTERP_EXIT_FUNC*)lpHandler;
		TRACE("InstallHandler: Event=CALLBACK_EVENT_INTERP_EXIT, FunctionEntry=0x%08X, DeviceID=0x%04X\n",
			lpHandler, dwRefData);
	}else
	if(nEventID == CALLBACK_EVENT_INTERP_DEBUG)
	{
		if(lpHandler == NULL)
		{
			return -1;
		}

		// ��װ�˳��ص�����
		m_lpfnDebug = (INTERP_DEBUG_FUNC*)lpHandler;
		TRACE("InstallHandler: Event=CALLBACK_EVENT_INTERP_DEBUG, FunctionEntry=0x%08X, DeviceID=0x%04X\n",
			lpHandler, dwRefData);
	}
[!endif]
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int C[!output INTERFACE_NAME]::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{
[!if VCITYPE_INTERP]
	if(nEventID == CALLBACK_EVENT_INTERP_EXIT)
	{
		m_lpfnExit = NULL;
	}else
	if(nEventID == CALLBACK_EVENT_INTERP_DEBUG)
	{
		m_lpfnDebug = NULL;
	}
[!endif]

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
int C[!output INTERFACE_NAME]::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	return 0;
}

[!if VCITYPE_OWM]
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
[!endif]

//{{VCI_IMPLEMENT_BEGIN
[!if VCITYPE_INTERP]
int __stdcall C[!output INTERFACE_NAME]::SetIPlatUI(LPVOID lpIPlatUI)
{
	pIPlatUI = (IPlatUI*)lpIPlatUI;

	// �������Ա���
	CString strCurrentLanguage = _T("english");
	if(VCI_LANGUAGE_CHINESE == GetLanguage())
	{
		strCurrentLanguage = _T("chinese");
	}

	// TODO: Add your code here

	return 0;
}

// ���ÿ�·��
int __stdcall C[!output INTERFACE_NAME]::SetLibPath(LPCTSTR lpszPath)
{
	// TODO: Add your code here

	return 0;
}

// �������Ƿ�������
int __stdcall C[!output INTERFACE_NAME]::IsInterpRunning()
{
	// TODO: Add your code here

	return FALSE;
}

// ���ýű��ļ���
int __stdcall C[!output INTERFACE_NAME]::SetScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	m_strScriptFile = lpszPathName;

	return TRUE;
}

// ��ȡ�ű��ļ���
CString __stdcall C[!output INTERFACE_NAME]::GetScriptFile()
{
	// TODO: Add your code here

	return m_strScriptFile;
}

// ���н��̲��ҽ��Զ�������ܵ�
BOOL C[!output INTERFACE_NAME]::RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId, BOOL bActiveOutput)
{
	m_hPipeProcess = NULL;

	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead,&hWrite,&sa,0))
	{
		AfxMessageBox("�������йܵ�ʧ�ܣ�");
		DWORD dwError = GetLastError();
		return FALSE;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	//si.hStdInput = hRead;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	// ���������
	CString strCmd;
	strCmd.Format("%s %s", strExePath, strParam);
	char cscmd[512];
	strcpy(cscmd, strCmd);

	m_bPipeProcessTerminated = FALSE;
	if (!CreateProcess(NULL, cscmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		DWORD dwError = GetLastError();
		AfxMessageBox("�����ܵ�����ʧ�ܣ�");
		return FALSE;
	}
	CloseHandle(hWrite);

	m_hPipeProcess = pi.hProcess;

	if(pIPlatUI && bActiveOutput)
	{
		pIPlatUI->ActiveOutput(nOutputId);
		pIPlatUI->OutputClear(nOutputId);
		pIPlatUI->OutputSet(nOutputId, "COLOR=0,0,255");	// Set output color
	}

	char buffer[4096] = {0};
	DWORD bytesRead;
	while (true)
	{
		memset(buffer, 0, 4096);
		if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
			break;
		if(pIPlatUI)
		{
			pIPlatUI->Output(nOutputId, buffer);
		}
		Sleep(200);
	}

	m_hPipeProcess = NULL;

	// ����ܵ������Ǳ���ֹ��,����ʧ��
	if(m_bPipeProcessTerminated)
	{
		return FALSE;
	}

	return TRUE;
}

// ��ֹ�ܵ�����
BOOL C[!output INTERFACE_NAME]::TerminatePipeProcess()
{
	if(m_hPipeProcess != NULL)
	{
		m_bPipeProcessTerminated = TRUE;
		if(!::TerminateProcess(m_hPipeProcess, 0))
		{
			return FALSE;
		}
		m_hPipeProcess = NULL;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ִ���ⲿ����
/////////////////////////////////////////////////////////////////////////////
BOOL C[!output INTERFACE_NAME]::ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd)
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
 
	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.wShowWindow=SW_HIDE ;
	siStartInfo.dwFlags=STARTF_USESHOWWINDOW;

	// ���������
	CString strCmd;
	strCmd.Format("%s %s", strExePath, strParam);
	char cscmd[255];
	strcpy(cscmd, strCmd);
	
	// �����������
	BOOL bRet=CreateProcess(NULL,
      cscmd,		  // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION
	
	if(bWaitEnd)
	{
		// �ȴ����̽���(��ʱʱ��20��)
		WaitForSingleObject(piProcInfo.hProcess, 20000);
	}
	return bRet;
}

// ���нű��ļ�
int __stdcall C[!output INTERFACE_NAME]::RunScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	if(lpszPathName != NULL)
	{
		if(!SetScriptFile(lpszPathName))
			return FALSE;
	}

	m_dwRunThreadId = ::GetCurrentThreadId();

	m_nRunState = SCRIPT_STATE_RUN;

	try {
	////////////////////////////////////////////
	// ִ�нű��ļ�
	//if(TCL_OK != Tcl_EvalFile(pInterp, script))
	//{
	//	m_nRunState = SCRIPT_STATE_END;
	//	m_nEvalResult = TCL_ERROR;
	//	return FALSE;
	//}
	////////////////////////////////////////////
	}catch(...)
	{
	}

	m_nRunState = SCRIPT_STATE_END;

	m_dwRunThreadId = NULL;
	//m_nEvalResult	= TCL_OK;
	return TRUE;
}

// ִ�нű�����
int __stdcall C[!output INTERFACE_NAME]::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	// TODO: Add your code here

	//m_nEvalResult = TCL_OK;
	return TRUE;
}

// ֹͣ�ű�����
int __stdcall C[!output INTERFACE_NAME]::StopScript()
{
	// TODO: Add your code here
	m_bStopSignal	= TRUE;	// ������ֹ��־

	// ��ֹ�ܵ�����
	TerminatePipeProcess();

	// ����Ϣ������Ϣѭ��
	// ע��m_dwRunThreadId�Ǳ���Ľű�����ʱ���߳�ID������������߳���
	// ���ô˺���,�ͻ�ͨ���߳���Ϣȥ���������߳��е�td����
	if(m_dwRunThreadId)
	{
		::PostThreadMessage(m_dwRunThreadId, WM_USER, 0, 0);
	}

	return TRUE;
}

// ��ȡ������ָ��
LPVOID __stdcall C[!output INTERFACE_NAME]::GetInterp()
{
	// TODO: Add your code here

	return NULL;
}

// ��ȡ������ID
int __stdcall C[!output INTERFACE_NAME]::GetInterpID()
{
	// TODO: Add your code here

	return m_nInterpID;
}

// ���ý�������
void __stdcall C[!output INTERFACE_NAME]::SetInterpName(LPCTSTR lpszInterptName)
{
	// TODO: Add your code here
	m_strInterpName = lpszInterptName;
}

// ��ȡ��������
CString __stdcall C[!output INTERFACE_NAME]::GetInterpName()
{
	// TODO: Add your code here

	return m_strInterpName;
}

// ��ȡ�����к�
int __stdcall C[!output INTERFACE_NAME]::GetErrorLine()
{
	// TODO: Add your code here
	return 0;
}

// ��ȡ�����Ϣ
CString __stdcall C[!output INTERFACE_NAME]::GetResult()
{
	// TODO: Add your code here
	return "";
}

// ���ñ���ֵ
int __stdcall C[!output INTERFACE_NAME]::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ����ֵ
CString __stdcall C[!output INTERFACE_NAME]::GetVar(LPCTSTR lpszVarName)
{
	// TODO: Add your code here

	return "";
}

// ע����չ����
int __stdcall C[!output INTERFACE_NAME]::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	// TODO: Add your code here

	return 0;
}

// ������չ����
int __stdcall C[!output INTERFACE_NAME]::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��չ����
CString __stdcall C[!output INTERFACE_NAME]::GetInterpParam(LPCTSTR lpszParamName)
{
	// TODO: Add your code here

	return "";
}

// ��������ģʽ
int __stdcall C[!output INTERFACE_NAME]::SetRunMode(int nRunMode)
{
	m_nRunMode = nRunMode;
	return true;
}

// ��ȡ����ģʽ
int __stdcall C[!output INTERFACE_NAME]::GetRunMode()
{
	return m_nRunMode;
}

// ��������״̬
int __stdcall C[!output INTERFACE_NAME]::SetRunState(int nState)
{
	m_nRunState = nState;
	return true;
}

// ��ȡ����״̬
int __stdcall C[!output INTERFACE_NAME]::GetRunState()
{
	return m_nRunState;
}

// ��ȡ��ǰ��ջ����
int __stdcall C[!output INTERFACE_NAME]::GetCurStackLevel()
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ�ؼ����б�
int __stdcall C[!output INTERFACE_NAME]::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	// TODO: Add your code here
	CString strType = lpszType;
	if(strType == "platform")	// ƽ̨��չ�ؼ���
	{
		//asKeyWord.Add("LangString");
	}else
	if(strType == "platform_keyword_tooltip")	// ƽ̨��չ�ؼ���ʹ��˵��
	{
		//asKeyWord.Add("DESCRIPTION:\r\n Set a language depend string\r\nSYNOPSIS:\r\n LangString strid language string");
	}

	return asKeyWord.GetSize();
}

// ���õ����¼�
int __stdcall C[!output INTERFACE_NAME]::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	CString strParam = lpszParam;

	if(nEvent == IDB_STOP)
	{
		//sprintf(szTclCmd, "td -stop");
	}else
	if(nEvent == IDB_NEXT)
	{
		//sprintf(szTclCmd, "td -next");
	}else
	if(nEvent == IDB_GOTO)
	{
		if(strParam.GetLength() == 0)
		{
			// �жϵ�
			//sprintf(szTclCmd, "td -goto *%d", nParam);
		}else
		{
			// �����ϵ�
			//sprintf(szTclCmd, "td -goto #%s", lpszParam);
		}
	}else
	if(nEvent == IDB_STEPINTO)
	{
		//sprintf(szTclCmd, "td -step into");
	}else
	if(nEvent == IDB_SETPOUT)
	{
		//sprintf(szTclCmd, "td -step return");
	}else
	if(nEvent == IDB_STEP)
	{
		//sprintf(szTclCmd, "td -step");
	}else
	if(nEvent == IDB_END)
	{
		return TRUE;
	}

	// ִ�жϵ����

	return TRUE;
}

// ���öϵ��б�
int __stdcall C[!output INTERFACE_NAME]::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	// TODO: Add your code here
	m_auBreakList.RemoveAll();
	for(int i=0; i<auBreaks.GetSize(); i++)
	{
		m_auBreakList.Add(auBreaks[i]);
	}

	return 0;
}

// ��ȡ�ϵ��б�
int __stdcall C[!output INTERFACE_NAME]::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	// TODO: Add your code here
	auBreaks.RemoveAll();
	for(int i=0; i<m_auBreakList.GetSize(); i++)
	{
		auBreaks.Add(m_auBreakList[i]);
	}

	return 0;
}

// ��ȡ�Ƿ��������ʷ���
int __stdcall C[!output INTERFACE_NAME]::GetEnableProfileAnalyse()
{
	// TODO: Add your code here
	return m_bProfileEnable;
}

// ��ȡ��Ч��������
int __stdcall C[!output INTERFACE_NAME]::GetAvailableLineCount()
{
	// TODO: Add your code here
	return m_nTDLineCount;
}

// ��ȡ�������и��Ǳ�
LPVOID __stdcall C[!output INTERFACE_NAME]::GetProfileMap()
{
	// TODO: Add your code here
	return (void*)(&m_ProfileMap);
}

// �ж�ָ�������Ƿ�֧��
int __stdcall C[!output INTERFACE_NAME]::MethodIsSupport(LPCTSTR lpszMethod)
{
	// TODO: Add your code here
	CString strMethod = lpszMethod;
	if( (strMethod == "run") ||		// ֧������
		(strMethod == "debug") ||	// ֧�ֵ���
		(strMethod == "break") ||	// ֧�ֶϵ�
		(strMethod == "compile") ||	// ֧�ֱ���
		(strMethod == "build") )	// ֧�ֹ�����ִ�г���
	{
		return TRUE;
	}

	return FALSE;
}

// ��ʼ������ģʽ
int __stdcall C[!output INTERFACE_NAME]::InitialDebugMode(CString& strResult)
{
	// TODO: Add your code here
	// ��ȡƽ̨�������ű�������

	// ��ʼ���������Դ���

	return TRUE;
}

// ���µ��Դ�����Ϣ
int __stdcall C[!output INTERFACE_NAME]::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	// TODO: Add your code here
	CString strWindow = lpszWindow;

	BOOL bWindowVar = FALSE;
	BOOL bWindowObject = FALSE;
	BOOL bWindowProc = FALSE;
	BOOL bWindowStack = FALSE;
	if(strWindow == "var")
	{
		bWindowVar = TRUE;
	}else
	if(strWindow == "object")
	{
		bWindowObject = TRUE;
	}else
	if(strWindow == "proc")
	{
		bWindowProc = TRUE;
	}else
	if(strWindow == "stack")
	{
		bWindowStack = TRUE;
	}else
	if(strWindow == "all")
	{
		bWindowVar = TRUE;
		bWindowObject = TRUE;
		//bWindowProc = TRUE;
		bWindowStack = TRUE;
	}

	if(bWindowVar)
	{
		//RunCommand(_T("global _plat_Tcl_WatchVariables;eval $_plat_Tcl_WatchVariables"));
		if(bActive)
		{
			//RunCommand(_T("_plat_Tcl_ActiveDebugWatch var"));
		}
	}
	if(bWindowObject)
	{
	}
	if(bWindowProc)
	{
	}
	if(bWindowStack)
	{
	}

	return TRUE;
}

// ���ýű�����
int __stdcall C[!output INTERFACE_NAME]::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	// TODO: Add your code here
	CString strCmd = lpszMethod;
	for(int i=0; i<asParam.GetSize(); i++)
	{
		if((asParam[i].Find('"') != 0) && (asParam[i].ReverseFind('"') != (asParam[i].GetLength()-1)))
		{
			strCmd += " \"";
			strCmd += asParam[i];
			strCmd += "\"";
		}else
		{
			strCmd += " ";
			strCmd += asParam[i];
		}
	}

	return RunScriptCommand(strCmd);
}

// ����
int __stdcall C[!output INTERFACE_NAME]::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	// TODO: Add your code here
	CString strSrcFile = lpszSrcFile;
	if(strSrcFile == "")
	{
		strSrcFile = m_strScriptFile;
	}

	if(strSrcFile == "")
	{
		return FALSE;
	}

	return 0;
}

// ����
int __stdcall C[!output INTERFACE_NAME]::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	// TODO: Add your code here
	CString strSrcFile = lpszSrcFile;
	if(strSrcFile == "")
	{
		strSrcFile = m_strScriptFile;
	}

	if(strSrcFile == "")
	{
		return FALSE;
	}

	return 0;
}
[!endif]
[!if VCITYPE_LICENSE]
// Ӧ�ó�����ڶԻ���
int __stdcall C[!output INTERFACE_NAME]::ApplicationAbout()
{
	// TODO: Add your code here
	//get_dll_resource();
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();

	return 0;
}

CString C[!output INTERFACE_NAME]::FindPlugInPath(CString strVciId)
{
	CFileFind finder;

	CString strWildcard = GetPlatRootPath();
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
		finder.Close();
		return strPath;
	}

	finder.Close();

	return "";
}

// ����Ĺ��ڶԻ���
int __stdcall C[!output INTERFACE_NAME]::ComponentAbout(LPCTSTR lpszVciId)
{
	// TODO: Add your code here
	CString strPlugInPath = FindPlugInPath(lpszVciId);
	if(strPlugInPath == "")
	{
		return FALSE;
	}

	CString strXmlFile = strPlugInPath;
	if(VCI_LANGUAGE_CHINESE == GetLanguage())
	{
		strXmlFile += "\\plugin_cn.xml";
		// ������������ĵ������ļ�,����ȱʡ��
		if(GetFileAttributes(strXmlFile) == 0xFFFFFFFF)
		{
			strXmlFile = strPlugInPath + "\\plugin.xml";
		}
	}else
	{
		strXmlFile += "\\plugin.xml";
	}
	CXmlParser parser;
	if(parser.Open(strXmlFile))
	{
		TRACE("Load %s fail!\n", strXmlFile);
		return FALSE;
	}

	DOMNode* pNode = parser.GetRootNode();
	CString strName = parser.GetNodeAttribute(pNode, "name");
	CString strVersion = parser.GetNodeAttribute(pNode, "version");
	CString strProvider = parser.GetNodeAttribute(pNode, "provider-name");

	//get_dll_resource();
	//CComponentAboutDlg aboutDlg;
	//aboutDlg.m_strName = strName;
	//aboutDlg.m_strVersion = strVersion;
	//aboutDlg.DoModal();

	return FALSE;
}

// У��License���
int __stdcall C[!output INTERFACE_NAME]::VerifyLicenseComponent(int nCmd, LPCTSTR lpszParamIn, CString& strParamOut)
{
	// TODO: Add your code here

	return 0;
}

// �����LicenseУ��
int __stdcall C[!output INTERFACE_NAME]::VerifyPluginLicense(LPCTSTR lpszVciId, int nAction)
{
	// TODO: Add your code here

	return TRUE;
}

// У���ƷLicense
int __stdcall C[!output INTERFACE_NAME]::VerifyProductLicense()
{
	// TODO: Add your code here

	return 0;
}

// У�鹦��License
int __stdcall C[!output INTERFACE_NAME]::VerifyFunctionLicense(int nFunction, int nValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��Ʒ���к�
int __stdcall C[!output INTERFACE_NAME]::GetProductNo(CString& strProductNo)
{
	// TODO: Add your code here

	return License::trLicenseFail;
}

// ��ȡ���������ַ���
int __stdcall C[!output INTERFACE_NAME]::GetUpgradeRequestString(CString& strRequest)
{
	// TODO: Add your code here

	return License::trLicenseFail;
}

// ����License�ļ�
int __stdcall C[!output INTERFACE_NAME]::UpgradeLicenseFile(LPCTSTR lpszUpgradeFile)
{
	// TODO: Add your code here

	return License::trLicenseFail;
}

// ��ȡӲ��Ψһ���к�
int __stdcall C[!output INTERFACE_NAME]::GetESN(CString& strESN)
{
	// TODO: Add your code here

	return 0;
}

// ����License ID
int __stdcall C[!output INTERFACE_NAME]::GenerateLicenseID(LPCTSTR lpszESN, LPCTSTR lpszMaskCode, CString& strLicenseID)
{
	// TODO: Add your code here

	return 0;
}

// ����License ID
int __stdcall C[!output INTERFACE_NAME]::ImportLicenseID(LPCTSTR lpszLicenseID)
{
	// TODO: Add your code here

	return 0;
}

// У��License ID
int __stdcall C[!output INTERFACE_NAME]::VerifyLicenseID(LPCTSTR lpszFunction)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡLicense ID
int __stdcall C[!output INTERFACE_NAME]::GetLicenseID(CString& strLicenseID)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ�����License��Ϣ
int __stdcall C[!output INTERFACE_NAME]::GetPluginLicenseInfo(LPCTSTR lpszVciId, int& nLicenseType, CTime& tUpdateLimit, int& nUserCount, CTime& tTrialTime)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��Ȩ�û���Ϣ
int __stdcall C[!output INTERFACE_NAME]::GetCustomerInfo(CString& strCustomerInfo)
{
	// TODO: Add your code here
	strCustomerInfo = _T("");

	return License::trOk;
}

// ��ȡ������Ϣ
int __stdcall C[!output INTERFACE_NAME]::GetOrderInfo(CString& strOrderInfo)
{
	// TODO: Add your code here
	strOrderInfo = _T("");

	return License::trOk;
}
[!endif]
[!if VCITYPE_PROJECT]
// �½�����
int __stdcall C[!output INTERFACE_NAME]::NewProject(LPCTSTR lpszProjectName, LPCTSTR lpszProjectPath, LPCTSTR lpszProjectDesc, LPCTSTR lpszAuthor)
{
	// TODO: Add your code here

	return trpOk;
}

// �򿪹���
int __stdcall C[!output INTERFACE_NAME]::OpenProject(LPCTSTR lpszProjectFile)
{
	// TODO: Add your code here

	return trpOk;
}

// ���湤��
int __stdcall C[!output INTERFACE_NAME]::SaveProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ִ�й���
int __stdcall C[!output INTERFACE_NAME]::RunProject()
{
	// TODO: Add your code here

	return trpOk;
}

// �رչ���
int __stdcall C[!output INTERFACE_NAME]::CloseProject(BOOL bForce)
{
	// TODO: Add your code here

	return trpOk;
}

// ���빤��
int __stdcall C[!output INTERFACE_NAME]::BuildProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ���±��빤��
int __stdcall C[!output INTERFACE_NAME]::RebuildProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ������̱�����Ϣ
int __stdcall C[!output INTERFACE_NAME]::CleanProject()
{
	// TODO: Add your code here

	return trpOk;
}

// ��ʾ������Ϣ
int __stdcall C[!output INTERFACE_NAME]::ProjectInformation()
{
	// TODO: Add your code here

	return trpOk;
}

// ˢ�����Դ��ڵĹ�����Ϣ
int __stdcall C[!output INTERFACE_NAME]::RefreshProjectProperty()
{
	// TODO: Add your code here

	return trpOk;
}

// ������������
int __stdcall C[!output INTERFACE_NAME]::ProjectPropertyPage(CVciPropertyPageArray& aPropertyPage)
{
	// TODO: Add your code here

	return aPropertyPage.GetSize();
}

// ��ȡ�򿪵Ĺ����б�
int __stdcall C[!output INTERFACE_NAME]::GetOpenProjectList(CStringArray& asProjectFile)
{
	// TODO: Add your code here

	return 1;
}

// ��ȡ��ǰ����Ĺ���
int __stdcall C[!output INTERFACE_NAME]::GetActiveProject(CString& strProject)
{
	// TODO: Add your code here

	return trpOk;
}

// ���ü���Ĺ���
int __stdcall C[!output INTERFACE_NAME]::SetActiveProject(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return trpOk;
}

// ��ȡ������Ϣ
int __stdcall C[!output INTERFACE_NAME]::GetProjectInfo(LPCTSTR lpszProject, CString& strProjectDesc, CString& strProjectFile, CString& strProjectPath)
{
	// TODO: Add your code here

	return trpOk;
}

// ��ȡ����״̬
int __stdcall C[!output INTERFACE_NAME]::GetProjectState(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return PROJECT_IDLE;
}

// ע��˵�
int __stdcall C[!output INTERFACE_NAME]::RegisterMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{
	// TODO: Add your code here

	return 0;
}

// ж�ز˵�
int __stdcall C[!output INTERFACE_NAME]::UnregisterMenu(COWMMenuArray& aOWMMenus)
{
	// TODO: Add your code here

	return 0;
}

// ��ʾDockingPane����ʱ���õĺ���
CWnd* __stdcall C[!output INTERFACE_NAME]::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	
	return NULL;
}

// ��ʼ�����붨���
int __stdcall C[!output INTERFACE_NAME]::InitCodeDefineLibrary(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return 0;
}

// ���ش��붨���
int __stdcall C[!output INTERFACE_NAME]::LoadCodeDefineLibrary(LPCTSTR lpszProject)
{
	// TODO: Add your code here

	return 0;
}

// ���Ҵ��붨���б�(����ֵ��ʾ����)
int __stdcall C[!output INTERFACE_NAME]::GetCodeDefine(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)
{
	// TODO: Add your code here

	return 0;
}

// ���Ҵ��������б�(����ֵ��ʾ����)
int __stdcall C[!output INTERFACE_NAME]::GetCodeReference(LPCTSTR lpszCode, CCodePositionArray& asCodePosition)
{
	// TODO: Add your code here

	return 0;
}
[!endif]
[!if VCITYPE_POLICY]
// ���Գ�ʼ��,����ʱ����
int __stdcall C[!output INTERFACE_NAME]::init(LPCSTR lpszParameter)
{
	return 0;
}

// ������ֹʱ����
int __stdcall C[!output INTERFACE_NAME]::done(LPCSTR lpszParameter)
{
	return 0;
}

// ���ԺϷ���У��
int __stdcall C[!output INTERFACE_NAME]::IsValid(LPCSTR lpszConfig)
{
	//lpszConfigΪ����ID���˴���Ҫ����ID���������ID�Ա�У��Ϸ���
	
	return POLICY_VALIDITY;
}

// ����ִ��
int __stdcall C[!output INTERFACE_NAME]::Exec(LPCSTR lpszConfig)
{
	return 0;
}

// �����޸�
int __stdcall C[!output INTERFACE_NAME]::Repair(LPCSTR lpszConfig)
{
	return 0;
}

// ���²���ִ�в���
int __stdcall C[!output INTERFACE_NAME]::UpdateExecParam(LPCSTR lpszConfig)
{
	//�ýӿڱ�����ʱ˵�����������仯����Ҫ����
	
	return 0;
}

// ���Ա���
int __stdcall C[!output INTERFACE_NAME]::Backup(LPCSTR lpszConfig)
{
	return 0;
}

// ���Իָ�
int __stdcall C[!output INTERFACE_NAME]::Restore(LPCSTR lpszConfig)
{
	return 0;
}
[!endif]
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT([!output INTERFACE_NAME])
CONTROL_INTERFACE_IMPLEMENT([!output INTERFACE_NAME])

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetExtraInfo(LPVOID lpRefData)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

//{{VCI_INTERFACE_IMPLEMENT_BEGIN
[!if VCITYPE_INTERP]
// ����ƽ̨UI�ӿ�ָ��
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetIPlatUI(LPVOID lpIPlatUI)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetIPlatUI(lpIPlatUI);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetLibPath(LPCTSTR lpszPath)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetLibPath(lpszPath);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::IsInterpRunning()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->IsInterpRunning();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetScriptFile(LPCTSTR lpszPathName)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetScriptFile(lpszPathName);
}

CString __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetScriptFile()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetScriptFile();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RunScriptFile(LPCTSTR lpszPathName)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->RunScriptFile(lpszPathName);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->RunScriptCommand(lpszTclCmd);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::StopScript()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->StopScript();
}

LPVOID __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetInterp()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetInterp();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetInterpID()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetInterpID();
}

void __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetInterpName(LPCTSTR lpszInterptName)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	pObj->SetInterpName(lpszInterptName);
}

CString __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetInterpName()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetInterpName();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetErrorLine()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetErrorLine();
}

CString __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetResult()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetResult();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetVar(lpszVarName, lpszVarValue);
}

CString __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetVar(LPCTSTR lpszVarName)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetVar(lpszVarName);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->RegisterCommand(pInterp, lpszCmd, lpFunc);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetInterpParam(lpszParamName, lpszParamValue);
}

CString __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetInterpParam(LPCTSTR lpszParamName)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetInterpParam(lpszParamName);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetRunMode(int nRunMode)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetRunMode(nRunMode);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetRunMode()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetRunMode();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetRunState(int nRunState)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetRunState(nRunState);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetRunState()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetRunState();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetCurStackLevel()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetCurStackLevel();
}

// ��ȡ�ؼ����б�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetKeyWordList(lpszType, asKeyWord);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetDebugEvent(nEvent, nParam, lpszParam);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->SetBreakList(lpszFile, auBreaks);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetBreakList(lpszFile, auBreaks);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetEnableProfileAnalyse()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetEnableProfileAnalyse();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetAvailableLineCount()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetAvailableLineCount();
}

LPVOID __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetProfileMap()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetProfileMap();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::MethodIsSupport(LPCTSTR lpszMethod)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->MethodIsSupport(lpszMethod);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::InitialDebugMode(CString& strResult)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->InitialDebugMode(strResult);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->RefreshDebugWindow(lpszWindow, bActive, strResult);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->CallMethod(lpszMethod, asParam);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Compile(lpszSrcFile, strDestFile, lpszParams);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Build(lpszSrcFile, strDestFile, lpszParams);
}
[!endif]
[!if VCITYPE_LICENSE]
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::ApplicationAbout()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->ApplicationAbout();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::ComponentAbout(LPCTSTR lpszVciId)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->ComponentAbout(lpszVciId);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::VerifyLicenseComponent(int nCmd, LPCTSTR lpszParamIn, CString& strParamOut)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->VerifyLicenseComponent(nCmd, lpszParamIn, strParamOut);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::VerifyPluginLicense(LPCTSTR lpszVciId, int nAction)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->VerifyPluginLicense(lpszVciId, nAction);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::VerifyProductLicense()
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->VerifyProductLicense();
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::VerifyFunctionLicense(int nFunction, int nValue)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->VerifyFunctionLicense(nFunction, nValue);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetProductNo(CString& strProductNo)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetProductNo(strProductNo);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetUpgradeRequestString(CString& strRequest)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetUpgradeRequestString(strRequest);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::UpgradeLicenseFile(LPCTSTR lpszUpgradeFile)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->UpgradeLicenseFile(lpszUpgradeFile);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetESN(CString& strESN)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetESN(strESN);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GenerateLicenseID(LPCTSTR lpszESN, LPCTSTR lpszMaskCode, CString& strLicenseID)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GenerateLicenseID(lpszESN, lpszMaskCode, strLicenseID);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::ImportLicenseID(LPCTSTR lpszLicenseID)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->ImportLicenseID(lpszLicenseID);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::VerifyLicenseID(LPCTSTR lpszFunction)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->VerifyLicenseID(lpszFunction);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetLicenseID(CString& strLicenseID)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetLicenseID(strLicenseID);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetPluginLicenseInfo(LPCTSTR lpszVciId, int& nLicenseType, CTime& tUpdateLimit, int& nUserCount, CTime& tTrialTime)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetPluginLicenseInfo(lpszVciId, nLicenseType, tUpdateLimit, nUserCount, tTrialTime);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetCustomerInfo(CString& strCustomerInfo)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetCustomerInfo(strCustomerInfo);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOrderInfo(CString& strOrderInfo)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->GetOrderInfo(strOrderInfo);
}
[!endif]
[!if VCITYPE_PROJECT]
	PROJECT_INTERFACE_IMPLEMENT
[!endif]
[!if VCITYPE_POLICY]
void __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetIAgent(LPVOID pIAgent)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	pObj->m_pIAgent = static_cast<IAgent*>(pIAgent);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::init(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->init(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::done(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->done(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::IsValid(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->IsValid(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Exec(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Exec(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Repair(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Repair(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::UpdateExecParam(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->UpdateExecParam(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Backup(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Backup(lpszParameter);
}

int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Restore(LPCSTR lpszParameter)
{
	C[!output INTERFACE_NAME] *pObj = GET_INTERFACE_OBJECT([!output INTERFACE_NAME]);
	return pObj->Restore(lpszParameter);
}
[!endif]
[!if VCITYPE_OWM]
// ��ȡģ����
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOwmName(CString& strOwmName)
{
	strOwmName = INTERFACE_INFO_IVciOwm.csName;
	return TRUE;
}

// ��ȡģ������
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOwmDesc(CString& strOwmDesc)
{
	strOwmDesc = INTERFACE_INFO_IVciOwm.csDescription;
	return TRUE;
}

// ��ȡģ��汾
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOwmVersion(int& nVersion)
{
	nVersion = 0x100;
	return TRUE;
}

// ��ȡģ������
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOwmType(UINT& nType)
{
	nType = theApp.m_ol.nOWMType;
	return TRUE;
}

// ��ȡģ��ID
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetOwmID(UINT& nID)
{
	nID = theApp.m_ol.nOWMID;
	return TRUE;
}

// ����ƽ̨UI�ӿ�ָ��
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetIPlatUI(LPVOID lpIPlatUI)
{
	theApp.m_pIPlatUI = (IPlatUI*)lpIPlatUI;
	return TRUE;
}

// �����ĵ�ģ��ָ��
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SetDocTemplate(CMultiDocTemplate* pDocTemplate)
{
	theApp.m_pDocTemplate = pDocTemplate;
	return TRUE;
}

// ��ȡ�ĵ�ģ��ָ��
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetDocTemplate(CMultiDocTemplate** ppDocTemplate)
{
	*ppDocTemplate = theApp.m_pDocTemplate;
	return TRUE;
}

// ��ȡָ������ʱ��ָ��
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetRuntimeClass(int nType, CRuntimeClass** ppRuntimeClass)
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
CWnd* __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam)
{
	CWnd* pWnd = NULL;
	/*if(nID == VIEW_ID_[!output PROJECT_NAME])
	{
		get_dll_resource();
		pWnd = (CWnd*)(RUNTIME_CLASS(C[!output PROJECT_NAME]View));
		theApp.m_p[!output PROJECT_NAME]View = pWnd;
	}*/
	
	return pWnd;
}

// OWM�����
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Command(int nCmd, WPARAM wParam, LPARAM lParam)
{
	return ::CommandOWM(nCmd, wParam, lParam);
}

// ע��˵�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc)
{
	return ::RegisterClientMenu(pMenu, aOWMMenus, nOWMIdStart, pDoc);
}

// ж�ز˵�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::UnregisterClientMenu(COWMMenuArray& aOWMMenus)
{	
	return ::UnregisterClientMenu(aOWMMenus);
}

// ע��ToolBar
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex)
{	
	return ::RegisterClientToolBar(pBar, aOWMMenus, nOWMIdStart, nIndex);
}

// ж��ToolBar
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart)
{	
	return ::UnregisterClientToolBar(pBar, aOWMMenus, nOWMIdStart);
}

// ע��OWM����ҳ
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RegisterOption(COWMOptionArray& aOWMOption)
{	
	return ::RegisterOption(aOWMOption);
}

// ע���ӡҳ����������ҳ
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::RegisterPrintPageOption(COWMOptionArray& aOWMOption)
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
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::LoadDocumentSession(CDocument* pDoc, CStringArray& asSessionLine)
{
	return TRUE;
}

// �����ĵ�Session
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::SaveDocumentSession(CDocument* pDoc, CStringArray& asSessionLine)
{
	return TRUE;
}

// �����ĵ�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::BackupDocument(CDocument* pDoc, LPCTSTR lpszBackupFile, int nBackupType)
{
	return TRUE;
}

// ��ȡ��������
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetImportTypeList(CStringArray& asImportType)
{
	return 0;
}

// ��ȡ��������
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetExportTypeList(CStringArray& asExportType)
{
	return 0;
}

// �����ĵ�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::ImportDocument(CDocument* pDoc, LPCTSTR lpszImportSource, int nImportType)
{
	return TRUE;
}

// �����ĵ�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::ExportDocument(CDocument* pDoc, LPCTSTR lpszExportDest, int nExportType)
{
	return TRUE;
}

// ����(pDocΪ�ձ�ʾ�������ĵ�)
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::Zoom(float fZoomFactor, CDocument* pDoc)
{
	return TRUE;
}

// ��ȡ��ǰ��������(pDocΪ�ձ�ʾ�������ĵ�)
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetZoomFactor(float& fZoomFactor, CDocument* pDoc)
{
	return TRUE;
}

// ��ȡ�����б�
int __stdcall C[!output INTERFACE_NAME]::X[!output INTERFACE_NAME]::GetZoomFactorRange(CStringArray& asZoomFactor, float& fZoomMin, float& fZoomMax)
{
	return 0;
}
[!endif]
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

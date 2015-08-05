#include "stdafx.h"
#include "CLuaInterp.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////
// OwmEdit��չ����
#define OC_OPENFILE				102	// ��һ���ĵ�(to owm)
#define OC_OPENFILEFIRST		103	// �ڵ�һ��λ�ô��ĵ�(to owm)
#define OC_SETREADONLY			104	// �����ĵ���ֻ������
#define OC_GETOPENFILE			105	// ��ȡ�򿪵��ļ���

#define OC_SETEXECUTION			110	// ���ô�����
#define OC_SETCURENTLINE		111	// ���õ�ǰ��
#define OC_SETCURENTEXECLINE	112	// ���õ�ǰִ����
#define OC_SEARCHLINE			113	// ����ָ�����ݵ��в�����Ϊ��ǰ��
#define OC_GETBREAKLIST			115	// ��ȡ�ϵ��б�
#define OC_SETSLAVEDEBUG		116	// ���ôӵ���ģʽ(����ģʽ,���������Գ���)

#define OC_GET_LINECOUNT		117	// ��ȡ������
#define OC_GET_LINE				118	// ��ȡ��
#define OC_SET_LINE				119	// ������
#define OC_GET_SELECTLINE		120	// ��ȡ��ǰѡ����з�Χ
#define OC_SET_SELECTLINE		121	// ����ѡ�����
#define OC_GET_CURSORPOS		122	// ��ȡ���λ��
#define OC_SET_CURSORPOS		123	// ���ù��λ��
#define OC_SET_BREAKLIST		124	// ���öϵ��б�
#define OC_CLEAR_BREAK			125	// ��նϵ�

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

CString LuaMakeNumber(lua_State *pLState, double dnum)
{
	CString strRet;
    long num = (long) dnum;
    if ((double) num == dnum)
	{
        strRet.Format("%ld (0x%lx)", num, num);
	}else
	{
		strRet.Format("%g", dnum);
	}

    return strRet;
}

CString LuaMakeBoolean(lua_State *pLState, int num)
{
    return num != 0 ? "true" : "false";
}

CString LuaGetTableInfo(lua_State *pLState, int nIndex)
{
	CString strRet;
    //int         nTag   = wxlua_ttag(pLState, nIndex);
	#ifdef USE_LUA_51
	int         nItems = luaL_getn(pLState, nIndex);
	#endif
	#ifdef USE_LUA_52
    int         nItems = lua_objlen(pLState, nIndex);
	#endif
    const void *pItem  = lua_topointer(pLState, nIndex);

    strRet.Format("0x%p", pItem);

    if (nItems > 0)
        strRet.Format("0x%p (approx %d items)", pItem, nItems);

    //if (nTag != WXLUA_NOTAG)
    //    strRet.Format("0x%p (tag %d)"), pItem, nTag);
	strRet = "table:" + strRet;
	return strRet;
}

// ��ȡָ��������ֵ(ת��Ϊ�ַ���)
CString GetValueString(lua_State *pLState, int nIndex)
{
	CString strValue = "";
	int ntype = lua_type(pLState, nIndex);
	switch(ntype)
	{
	case LUA_TNIL:
		strValue = "nil";
		break;
	case LUA_TBOOLEAN:
		strValue = LuaMakeBoolean(pLState, lua_toboolean(pLState, nIndex));
		break;
	case LUA_TNUMBER:
		strValue = LuaMakeNumber(pLState, lua_tonumber(pLState, nIndex));
		break;
	case LUA_TSTRING:
		strValue.Format("%s", lua_tostring(pLState, nIndex));
		break;
	case LUA_TTABLE:
		strValue = LuaGetTableInfo(pLState, nIndex);
		break;
	case LUA_TFUNCTION:
		if (lua_iscfunction(pLState, nIndex))
		{
			strValue.Format("%p", lua_tocfunction(pLState, nIndex));
		}else
		{
			strValue.Format("%p", lua_topointer(pLState, nIndex));
		}
		break;
	case LUA_TTHREAD:
		strValue.Format("%p", lua_topointer(pLState, nIndex));
		break;
	default:
		break;
	}
	return strValue;
}

// ���ص�print����
static int print_glue(lua_State *L)
{
	CLuaInterp* pInterp = GetLuaInterp(L);
	if(pInterp == NULL)
	{
		return 0;
	}

	IPlatUI* pIPlatUI = pInterp->pIPlatUI;
	if(pIPlatUI == NULL)
	{
		return 0;
	}

	BOOL bDirectOutput = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT, TRUE);
	int nOT = 2;
	if(bDirectOutput)
	{
		nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
	}

	// ������ű���׼���
	//BOOL bDirectResult = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_RESULT, TRUE);

	if(bDirectOutput)
	{
		pIPlatUI->OutputSet(nOT, "COLOR=0,0,255");	// ���������ɫ
	}

	// Get the number of parameters
	int n = lua_gettop(L);
	for(int i = 1; i <= n; i ++)
	{
		//const char *s = lua_tostring(L, i);
		CString s = GetValueString(L, i);
		if(bDirectOutput)// && (s != NULL))
		{
			pIPlatUI->Output(nOT, s);
			pIPlatUI->Output(nOT, "\t");
		}
	}

	// ���ӻ���
	if((n > 0) && bDirectOutput)
	{
		pIPlatUI->Output(nOT, "\r\n");
	}

	// No results
	return 0;
}

// ���������ַ����ĺ���
static int put_result_glue(lua_State *L)
{
	CLuaInterp* pInterp = GetLuaInterp(L);
	if(pInterp == NULL)
	{
		return 0;
	}

	// Get the number of parameters
	int n = lua_gettop(L);
	for(int i = 1; i <= n; i ++)
	{
		//const char *s = lua_tostring(L, i);
		CString s = GetValueString(L, i);
		pInterp->m_strResult += s;
	}

	// No results
	return 0;
}

static int lua_traceback (lua_State *L)
{
	#ifdef USE_LUA_51
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	#endif
	#ifdef USE_LUA_52
	lua_getglobal(L, "debug");	// Lua 5.2û��global
	#endif

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

/*
// ���صļ�����չ��ĺ���
typedef int (*LuaRegister)(lua_State*, HWND hWnd);
static int lua_loadlib(lua_State *L)
{
	CString strLibPath = lua_tostring(L, 1);
	strLibPath.Replace("/", "\\");
	if(strLibPath.Find("\\") == -1)
	{
		strLibPath = GetInterpRootPath() + "bin\\" + strLibPath;
	}

	HMODULE hMod = LoadLibrary(strLibPath);
	if ( hMod )
	{
		LuaRegister lua_reg = (LuaRegister)GetProcAddress(hMod, "LuaRegister");
		if ( lua_reg )
		{
			lua_reg(L, AfxGetMainWnd()->GetSafeHwnd());
			CLuaInterp* pInterp = GetLuaInterp(L);
			if(pInterp)
			{
				pInterp->m_hLoaded[pInterp->m_nLoaded++] = hMod;
			}
			lua_pushboolean(L, TRUE);
			return 1;
		}
		else
		{
			luaL_argerror(L, 1, "Dll has no LuaRegister entry");
			lua_pushboolean(L, FALSE);
			return 1;
		}	
	}
	else
	{
		luaL_argerror(L, 1, (LPCTSTR)"Load library failed!");
		lua_pushboolean(L, FALSE);
		return 1;
	}
}
*/
//////////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////////
static CPtrList	g_InterpList;	// �����������б�
static int g_InterpID = 0x100;	// ������ID������

// ��ȡLua�������б�ָ��
CPtrList* GetInterpList()
{
	return &g_InterpList;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ�����ָ���ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CLuaInterp* GetLuaInterp(LPVOID lpInterp)
{
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CLuaInterp* pInterp = (CLuaInterp*)g_InterpList.GetAt(pos);
		if(lpInterp == pInterp->GetInterp())
		{
			return pInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// �жϽ����������Ƿ����
//////////////////////////////////////////////////////////////////////
BOOL CheckLuaInterpExist(CLuaInterp* pLuaInterp)
{
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CLuaInterp* pInterp = (CLuaInterp*)g_InterpList.GetAt(pos);
		if(pLuaInterp == pInterp)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CLuaInterp::CLuaInterp()
{
	m_nInterpID		= g_InterpID++;
	if(g_InterpID > 0x01000000)
		g_InterpID  = 0x100;
	pIPlatUI		= NULL;
	m_dwRunThreadId	= NULL;
	m_strInterpName	= _T("");
	m_strScriptFile	= _T("");

	// init lua base library
	#ifdef USE_LUA_51
	pLState			= lua_open();
	#endif
	#ifdef USE_LUA_52
    pLState			= luaL_newstate();
	#endif

	// ����lua�Ŀ�·��(�˺�����blueant��lua��׼��̬����������,���lua�汾����,��Ҫ������ֲ����)
	luaL_setlibpath(pLState, GetInterpRootPath() + "bin");

	luaL_openlibs(pLState);
	/*
	luaopen_base(pLState);
	//luaopen_package(pLState);
	luaopen_table(pLState);
	//luaopen_io(pLState);
	luaopen_os(pLState);
	luaopen_string(pLState);
	luaopen_math(pLState);
	luaopen_debug(pLState);
	//luaopen_loadlib(pLState);
	*/

	lua_register(pLState, "print",  print_glue);
	lua_register(pLState, "put_result",  put_result_glue);
	//lua_register(pLState, "loadlib", lua_loadlib);

	LuaExCmd_Register(pLState);

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
	g_InterpList.AddTail(this);
	m_nLoaded		= 0;

	m_lpfnDebug		= NULL;
}

CLuaInterp::~CLuaInterp()
{
	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();

	try {
		lua_close(pLState);
	}catch(...)
	{
	}

	// ɾ�������������б��е���
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CLuaInterp* pInterp = (CLuaInterp*)g_InterpList.GetAt(pos);
		if(pInterp == this) 
		{
			g_InterpList.RemoveAt(pos);
			return;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CLuaInterp::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

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
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int CLuaInterp::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{
	if(nEventID == CALLBACK_EVENT_INTERP_EXIT)
	{
		m_lpfnExit = NULL;
	}else
	if(nEventID == CALLBACK_EVENT_INTERP_DEBUG)
	{
		m_lpfnDebug = NULL;
	}

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ��Lua��ƽ̨����
/////////////////////////////////////////////////////////////////////////////
int CLuaInterp::LuaSetPlatformVariable()
{
	IPlatUI* pIPlatUI = ((IPlatUI*)getIPlatUI());

	// ����ȫ�ֱ���platform_language,Lua���ñ����ķ������Ȱ�ֵѹջ,Ȼ���������
	lua_pushstring(pLState, (VCI_LANGUAGE_CHINESE == GetLanguage()) ? "chinese" : "english");
	lua_setglobal(pLState, "platform_language");

	// ����platform_path
	lua_pushstring(pLState, GetPlatRootPath());
	lua_setglobal(pLState, "platform_path");

	// ����platform_version
	CString strPlatformVersion = "";
	if(pIPlatUI)
	{
		strPlatformVersion = pIPlatUI->GetPlatVersion();
	}
	lua_pushstring(pLState, strPlatformVersion);
	lua_setglobal(pLState, "platform_version");

	// ����platform_interface
	lua_pushinteger(pLState, (int)pIPlatUI);
	lua_setglobal(pLState, "platform_interface");

	// ����platform_type
	#ifdef _DEBUG
	lua_pushstring(pLState, "debug");
	#else
	lua_pushstring(pLState, "release");
	#endif
	lua_setglobal(pLState, "platform_type");

	// ����interp_path
	lua_pushstring(pLState, GetInterpRootPath());
	lua_setglobal(pLState, "interp_path");

	// ����interpName
	lua_pushstring(pLState, m_strInterpName);
	lua_setglobal(pLState, "interpName");

	return 0;
}

//{{VCI_IMPLEMENT_BEGIN
int __stdcall CLuaInterp::SetIPlatUI(LPVOID lpIPlatUI)
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
int __stdcall CLuaInterp::SetLibPath(LPCTSTR lpszPath)
{
	// TODO: Add your code here

	return 0;
}

// �������Ƿ�������
int __stdcall CLuaInterp::IsInterpRunning()
{
	// TODO: Add your code here
	// �ж����н������Ƿ��Ѿ���ֹ
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CLuaInterp* pInterp = (CLuaInterp*)g_InterpList.GetAt(pos);
		int nRunState = pInterp->GetRunState();
		if( (SCRIPT_STATE_RUN == nRunState) ||
			(SCRIPT_STATE_BREAK == nRunState) ||
			(SCRIPT_STATE_STEP == nRunState) )
			return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�����ļ���������,�����ж��ļ��Ƿ��Ѿ�����
/////////////////////////////////////////////////////////////////////////////
int CLuaInterp::FindDebugFile(CString strFileName)
{
	for(int i=0; i<m_asDebugScript.GetSize(); i++)
	{
		if(strFileName == m_asDebugScript[i])
		{
			return i;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// �ڵ����ļ��б��������ļ�
/////////////////////////////////////////////////////////////////////////////
int CLuaInterp::AddDebugFile(CString strFileName)
{
	if(FindDebugFile(strFileName) == -1)
	{
		return m_asDebugScript.Add(strFileName);
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// ��յ����ļ��б�
/////////////////////////////////////////////////////////////////////////////
BOOL CLuaInterp::ClearDebugFile()
{
	m_asDebugScript.RemoveAll();
	return TRUE;
}

// ���ýű��ļ���
int __stdcall CLuaInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	m_strScriptFile = lpszPathName;

	return TRUE;
}

// ��ȡ�ű��ļ���
CString __stdcall CLuaInterp::GetScriptFile()
{
	// TODO: Add your code here

	return m_strScriptFile;
}

// ���н��̲��ҽ��Զ�������ܵ�
BOOL CLuaInterp::RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId, BOOL bActiveOutput)
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
BOOL CLuaInterp::TerminatePipeProcess()
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
BOOL CLuaInterp::ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd)
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

// ���Իص�
void CLuaInterp::DebugBreak(const char *szFile, int nStackLevel, int nLine)
{
	// �����ջ��������Ҫ����
	SetStackLevel(nStackLevel);

	//�����Ƿ���Ҫ�����ļ�
	CString file1 = szFile;
	file1.Replace('\\', '/');
	CString file2 = m_strScriptFile;
	file2.Replace('\\', '/');
	if(file1 != file2)
	{
		if(FindDebugFile(szFile) < 0)
		{
			//����ļ�
			if(AddDebugFile(szFile) >= 0)
			{
				pIPlatUI->OpenDocumentFile(szFile, "");
			}
			//��ӵ����ļ�ʧ�ܣ������жϵ�
			else
			{
				//TODO
				return;
			}
		}
		// ��OwmEdit������������ģʽΪ�ӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)szFile, 1, NULL);
	}
	else
	{
		// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)szFile, 0, NULL);
	}

	//���õ�ǰ��
	if(m_lpfnDebug)
	{
		m_lpfnDebug(IDB_USER_SETCURRENTLINE, szFile, nLine);
	}

	// ���õ�ǰ״̬Ϊ�ж�״̬
	SetRunState(SCRIPT_STATE_BREAK);
	//���������¼�
	::ResetEvent(m_hDebugEvent);
	
	//�ȴ���������
	::WaitForSingleObject(m_hDebugEvent, INFINITE);

	// ���õ�ǰ״̬Ϊ����״̬
	SetRunState(SCRIPT_STATE_RUN);
	// ��Edit���͵�ǰ���������
	if(m_lpfnDebug)
	{
		m_lpfnDebug(IDB_USER_SETCURRENTLINE, szFile, 0);
	}
}

// �лص�
void CLuaInterp::LineHook(const char *szFile, int nLine)
{
	//�жϵ�ǰģʽ
	if(m_nCurCommand == IDB_STOP)	// ��ֹ��������
	{
		StopScript();
	}else
	if(m_nCurCommand == IDB_STEPINTO) //�������뺯����ÿ����ͣ
	{
		DebugBreak(szFile, GetCurStackLevel(), nLine);
		return;
	}else
	{
		if(m_nCurCommand == IDB_STEP) //�����������뺯������ͬһ֡����һ������ֹͣ
		{
			int nStackLevel = GetCurStackLevel();
			if(GetStackLevel() >= nStackLevel)
			{
				DebugBreak(szFile, nStackLevel, nLine);
				return;
			}
		}else
		if(m_nCurCommand == IDB_GOTO) //ִ�е�ָ����
		{
			if(nLine == GetBreakLineNo())
			{
				DebugBreak(szFile, GetCurStackLevel(), nLine);
				return;
			}
		}
		
		//�ж��Ƿ������õĶϵ�λ�ã��ǵĻ��ϵ㣬��ʱֻ֧�ֵ��ļ��ϵ�
		CString file1 = szFile;
		file1.Replace('\\', '/');
		CString file2 = m_strScriptFile;
		file2.Replace('\\', '/');
		if(file1 == file2)
		{
			for(int i=0; i<m_auBreakList.GetSize(); i++)
			{
				if(nLine == m_auBreakList.GetAt(i))
				{
					DebugBreak(szFile, GetCurStackLevel(), nLine);
					return;
				}
			}
		}

		if(m_nCurCommand == IDB_SETPOUT) //�˳����������ص���һ��
		{
			int nStackLevel = GetCurStackLevel();
			if(nStackLevel < GetStackLevel())
			{
				DebugBreak(szFile, nStackLevel, nLine);
				return;
			}
		}
	}
}

// �лص�
static void lua_line_hook (lua_State *L, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);

	if ( ar->source[0] == '@' )
	{
		CLuaInterp* pInterp = GetLuaInterp(L);
		if(pInterp != NULL)
		{
			pInterp->LineHook(ar->source+1, ar->currentline);
		}
	}
}

/*
// �����ص�
static void lua_func_hook (lua_State *L, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);
	
	if ( ar->source[0] == '@' )
 	{
		CLuaInterp* pInterp = GetLuaInterp(L);
		if(pInterp != NULL)
		{
			pInterp->LineHook(ar->source+1, ar->currentline);
		}
 	}
}*/

// ���Իص�����
static void lua_hook (lua_State *L, lua_Debug *ar)
{
 	switch(ar->event)
 	{
		#ifdef USE_LUA_51
 		case LUA_HOOKTAILRET:
		#endif
		#ifdef USE_LUA_52
		case LUA_HOOKTAILCALL:
		#endif
 		case LUA_HOOKRET:
 						//lua_func_hook(L,ar);
 						break;
 		case LUA_HOOKCALL://lua_func_hook(L,ar);
 						break;
 		case LUA_HOOKLINE:lua_line_hook(L,ar);
 						break;
 	}
}

int CLuaInterp::LuaReport(int status)
{
	if ((status!=0) && !lua_isnil(pLState, -1))
	{
		const char *msg = lua_tostring(pLState, -1);
		if (msg == NULL)
		{
			msg = "(error object is not a string)\n";
		}
		/* ������Ϣֱ��ͨ���������,�����ڴ˴���ʾ
		// output
		if(pIPlatUI)
		{
			BOOL bDirectOutput = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT, TRUE);
			int nOT = 1;
			if(bDirectOutput)
			{
				pIPlatUI->OutputSet(nOT, "COLOR=255,0,0");	// ���������ɫ
				pIPlatUI->Output(nOT, msg);
				pIPlatUI->Output(nOT, "\r\n");
			}
		}
		*/

		lua_pop(pLState, 1);

		//status = 1;
		m_strResult = msg;
	}

	return status;
}

int CLuaInterp::LuaDocall(int narg, int clear)
{
	//����ǵ���ģʽ��������ģʽ������ô���������¼�
	if(m_nRunMode != SCRIPT_RUN_NORMAL)
	{
		static int eventNum = 0;
		CString eventName;
		eventName.Format("LuaDebug%d", eventNum);
		m_hDebugEvent = ::CreateEvent(NULL, FALSE, FALSE, eventName);
		eventNum++;
		if(!m_hDebugEvent)
		{
			::AfxMessageBox("Create Debug Event Failed, Can not debug");
			lua_pushstring(pLState, "Create Debug Event Failed, Can not debug!\n");
			lua_error(pLState);
			m_nRunState = SCRIPT_STATE_END;
			return FALSE;
		}

		// ����hook
		lua_sethook(pLState, lua_hook, LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);
	}

	m_strResult = "";
	int status;
	int base = lua_gettop(pLState) - narg;  /* function index */
	lua_pushcfunction(pLState, lua_traceback);  /* push traceback function */
	lua_insert(pLState, base);  /* put it under chunk and args */
	//signal(SIGINT, laction);
	status = lua_pcall(pLState, narg, (clear ? 0 : LUA_MULTRET), base);
	//signal(SIGINT, SIG_DFL);
	lua_remove(pLState, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0)
	{
		lua_gc(pLState, LUA_GCCOLLECT, 0);
	}

	lua_sethook(pLState, NULL, 0, 0);  /* turn off hooks */

	status = LuaReport(status);

	//�����debug�ļ�
	for(int i=0; i<m_asDebugScript.GetSize(); i++)
	{
		CString strFileName = m_asDebugScript.GetAt(i);
		// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG, (WPARAM)(LPTSTR)(LPCTSTR)strFileName, -1, NULL);
		//������ļ��ĵ��Լ�ͷ
		if(m_lpfnDebug)
		{
			m_lpfnDebug(IDB_USER_SETCURRENTLINE, strFileName, 0);
		}
	}
	m_asDebugScript.RemoveAll();

	if((m_nRunMode != SCRIPT_RUN_NORMAL)&&(m_lpfnDebug != NULL))
	{
		//����������ļ��ĵ��Լ�ͷ
		m_lpfnDebug(IDB_USER_SETCURRENTLINE, m_strScriptFile, 0);

		// ��������¼�
		if(m_hDebugEvent)
		{
			::CloseHandle(m_hDebugEvent);
			m_hDebugEvent = NULL;
		}
	}

	return status;
}

// ���нű��ļ�
int __stdcall CLuaInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	m_nEvalResult = 0;
	if(lpszPathName != NULL)
	{
		if(!SetScriptFile(lpszPathName))
			return FALSE;
	}

	m_dwRunThreadId = ::GetCurrentThreadId();

	m_nRunState = SCRIPT_STATE_RUN;

	try {
	////////////////////////////////////////////

	// ����ƽ̨����
	LuaSetPlatformVariable();

	// ִ�нű��ļ�
	m_strResult = "";
	//m_nEvalResult = luaL_loadfile(pLState, m_strScriptFile) ||
	//				LuaDocall(0, 1);
	m_nEvalResult = luaL_loadfile(pLState, m_strScriptFile);
	if(m_nEvalResult == 0)
	{
		// ����﷨����û�д����ִ��
		m_nEvalResult = LuaDocall(0, 1);
	}else
	{
		//����﷨�����д����ֱ�ӱ���
		LuaReport(m_nEvalResult);
	}
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
int __stdcall CLuaInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	// TODO: Add your code here
	if(m_nRunMode != SCRIPT_RUN_NORMAL)
	{
		// �����ڵ���ģʽ��,������ִ������
		return FALSE;
	}

	try {
	////////////////////////////////////////////

	// ����ƽ̨����
	LuaSetPlatformVariable();

	m_strResult = "";
	//m_nEvalResult = luaL_loadbuffer(pLState, lpszTclCmd, strlen(lpszTclCmd), "command") ||
	//				LuaDocall(0, 1);
	m_nEvalResult = luaL_loadbuffer(pLState, lpszTclCmd, strlen(lpszTclCmd), "command");
	if(m_nEvalResult == 0)
	{
		// ����﷨����û�д����ִ��
		m_nEvalResult = LuaDocall(0, 1);
	}else
	{
		//����﷨�����д����ֱ�ӱ���
		LuaReport(m_nEvalResult);
	}

	////////////////////////////////////////////
	}catch(...)
	{
	}

	return TRUE;
}

// ֹͣ�ű�����
int __stdcall CLuaInterp::StopScript()
{
	// TODO: Add your code here
	m_bStopSignal	= TRUE;	// ������ֹ��־

	luaL_error(pLState, "User stop eval script!");

	// ��ֹ�ܵ�����
	TerminatePipeProcess();

	// ����Ϣ������Ϣѭ��
	// ע��m_dwRunThreadId�Ǳ���Ľű�����ʱ���߳�ID������������߳���
	// ���ô˺���,�ͻ�ͨ���߳���Ϣȥ���������߳��е�td����
	if(m_dwRunThreadId)
	{
		::PostThreadMessage(m_dwRunThreadId, WM_USER, 0, 0);
	}

	// ���ֹͣ����,���ͷ����м��ؿ�
	if ( m_nLoaded > 0 )
	{
		for ( int i=m_nLoaded-1; i>=0; --i )
			FreeLibrary(m_hLoaded[i]);

		m_nLoaded = 0;
	}

	return TRUE;
}

// ��ȡ������ָ��
LPVOID __stdcall CLuaInterp::GetInterp()
{
	// TODO: Add your code here

	return pLState;
}

// ��ȡ������ID
int __stdcall CLuaInterp::GetInterpID()
{
	// TODO: Add your code here

	return m_nInterpID;
}

// ���ý�������
void __stdcall CLuaInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	// TODO: Add your code here
	m_strInterpName = lpszInterptName;
}

// ��ȡ��������
CString __stdcall CLuaInterp::GetInterpName()
{
	// TODO: Add your code here

	return m_strInterpName;
}

// ��ȡ�����к�
int __stdcall CLuaInterp::GetErrorLine()
{
	// TODO: Add your code here
	if(m_nEvalResult != 0)	// ������Ϣ�ʹ����н���
	{
		GetResult();
	}
	return m_nEvalResult;
}

// ��ȡ�����Ϣ
CString __stdcall CLuaInterp::GetResult()
{
	// TODO: Add your code here
	CString strResult = m_strResult;
	if(m_nEvalResult != 0)	// ������Ϣ����
	{
		if(strResult.Find("[string \"command\"]:") == 0)
		{
			// ����RunScriptCommand�Ĵ�����Ϣ,��ʽΪ [string "command"]:lineno:errorinfo
			strResult.Delete(0, 19);
			int nPos = strResult.Find(":");
			if(nPos != -1)
			{
				m_nEvalResult = atoi(strResult.Left(nPos));
				strResult.Delete(0, nPos+1);
				strResult.TrimLeft();
			}
		}else
		{
			int nPosStack = strResult.Find("stack traceback:");
			if(nPosStack != -1)
			{
				// �����stack traceback:,��Ҫȥ���ٽ�������ķ���,�����Ľ��Ҫ����
				strResult.Delete(0, nPosStack+17);
			}
			
			// ����RunScriptFile�Ĵ�����Ϣ,��ʽΪ filepath:lineno:errorinfo
			// ������ַ����Ƚϳ��Ļ�,filepath���ֿ��ܻ�������ǰ���һЩ����,����...��ͷ
			int nPos = strResult.Find(":");
			int nPos3Dot = strResult.Find("...");
			if(((nPos > 0) && (nPos < 5)) || (nPos3Dot < 3))	// ·�����̷������:
			{
				if((nPos > 0) && (nPos < 5))
				{
					nPos = strResult.Find(":", nPos+1);
				}else
				{
					nPos = strResult.Find(":");
				}
				if(nPos != -1)
				{
					CString strFile = strResult.Left(nPos);
					strResult.Delete(0, nPos+1);
					nPos = strResult.Find(":");
					if(nPos != -1)
					{
						m_nEvalResult = atoi(strResult.Left(nPos));
						strResult.Delete(0, nPos+1);
						strResult.TrimLeft();
					}
				}
			}

			if(nPosStack != -1)
			{
				strResult = m_strResult;
			}
		}
	}

	return strResult;
}

BOOL IsNumber(CString str)
{
	int nLength = str.GetLength();
	if (nLength > 2 && str[0] == '0' && str[1] == 'x')
	{
		for (int I = 2; I < nLength; I ++)
		{
			if (isdigit(str[I]) || (str[I] >= 'A' && str[I] <= 'F') ||
										(str[I] >= 'a' && str[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (! isdigit(str[0]))
		return FALSE;
	for (int I = 1; I < nLength; I ++)
	{
		if (! isdigit(str[I]) && str[I] != '+' &&
			str[I] != '-' && str[I] != '.' && str[I] != 'e' &&
			str[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

// ���ñ���ֵ
int __stdcall CLuaInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	// TODO: Add your code here
	/*CString strCmd;
	if(IsNumber(lpszVarName))
	{
		strCmd.Format("%s=%s", lpszVarName, lpszVarValue);
	}else
	{
		strCmd.Format("%s=\"%s\"", lpszVarName, lpszVarValue);
	}
	RunScriptCommand(strCmd);*/
	lua_pushstring(pLState, lpszVarValue);
	lua_setglobal(pLState, lpszVarName);

	return 0;
}

// ��ȡ����ֵ
CString get_table_var_string(CLuaInterp* pInterp, lua_State* pLState)
{
	CString strValue = "{";
	//int t_idx = lua_gettop(pLState);
	lua_pushnil(pLState);
	while (lua_next(pLState, -2) != 0)
	{
		//if(lua_istable(pInterp->pLState, -1))	// Ƕ����������
		//{
			// ��ʱ��֧��
			/*CString strVarName = lua_tostring(pInterp->pLState, -2);
			if(strVarName == "1")
			{
				lua_pop(pInterp->pLState, 1);
				lua_pop(pInterp->pLState, 1);
			}*/

			//lua_pop(pInterp->pLState, 1);
		//}else
		{
			strValue += lua_tostring(pLState, -2);
			strValue += "=";
			int ntype = lua_type(pLState, -1);
			if(ntype == LUA_TSTRING)
			{
				strValue += "'";
			}
			strValue += pInterp->GetValueString(-1);
			if(ntype == LUA_TSTRING)
			{
				strValue += "'";
			}
			if(ntype == LUA_TTABLE)
			{
				strValue += "(table)";
			}
			strValue += ", ";
			//lua_typename(pInterp->pLState, lua_type(pInterp->pLState, -1))

			lua_pop(pLState, 1); // pop value, keep key for next iteration
		}
	}
	lua_pop(pLState, 1); // pop table

	if(strValue.GetLength() > 1)
	{
		strValue = strValue.Left(strValue.GetLength()-2);
	}
	strValue += "}";

	return strValue;
}

// ��ȡ����ֵ
CString __stdcall CLuaInterp::GetVar(LPCTSTR lpszVarName)
{
	// TODO: Add your code here
	/*
	CString strCmd;
	strCmd.Format("put_result(%s)", lpszVarName);
	RunScriptCommand(strCmd);

	if(GetErrorLine() == 0)
	{
		return GetResult();
	}*/

	// ���Ȼ�ȡ�ֲ�����
	CString strVarName;
	CString strValue;
	int nLevel = 0;
	lua_Debug ar;
	if ( lua_getstack (pLState, nLevel, &ar) )
	{
		int i = 1;
        const char *name;
        while ((name = lua_getlocal(pLState, &ar, i++)) != NULL) {
			int ntype = lua_type(pLState, -1);
			strVarName = name;
			if(strVarName == lpszVarName)
			{
				if(lua_istable(pLState, -1))
				{
					strValue = get_table_var_string(this, pLState);
				}else
				{
					strValue = GetValueString(-1);
					lua_pop(pLState, 1);  /* remove variable value */
				}
				//strType = lua_typename(pLState, ntype);
				return strValue;
			}
			lua_pop(pLState, 1);  /* remove variable value */
        }
	}

	// ��ȡȫ�ֱ���
	lua_getglobal(pLState, lpszVarName);
	if(lua_istable(pLState, -1))
	{
		return get_table_var_string(this, pLState);
	}
	strValue = lua_tostring(pLState,-1);
	lua_pop(pLState, 1);

	return strValue;
}

// ע����չ����
int __stdcall CLuaInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	// TODO: Add your code here

	return 0;
}

// ������չ����
int __stdcall CLuaInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��չ����
CString __stdcall CLuaInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	// TODO: Add your code here

	return "";
}

// ��������ģʽ
int __stdcall CLuaInterp::SetRunMode(int nRunMode)
{
	m_nRunMode = nRunMode;
	//��������ģʽ���õ�ǰ����ָ���Ҫ���ڵ���ʱ�������������Զϵ�
	switch(m_nRunMode)
	{
	case SCRIPT_RUN_DEBUG_GO:  //go���൱�����е��ϵ�λ��ֹͣ
		m_nCurCommand = IDB_GOTO;
		break;
	case SCRIPT_RUN_DEBUG_STEP:  //����ִ�У�һ��ʼ��������ֹͣ���൱�ڵ������뺯��
		m_nCurCommand = IDB_STEPINTO;
		break;
	default:  //����������ͨ���У������в���(SCRIPT_RUN_NORMAL,SCRIPT_RUN_CANSTOP)
		break;
	}

	return true;
}

// ��ȡ����ģʽ
int __stdcall CLuaInterp::GetRunMode()
{
	return m_nRunMode;
}

// ��������״̬
int __stdcall CLuaInterp::SetRunState(int nState)
{
	m_nRunState = nState;
	return true;
}

// ��ȡ����״̬
int __stdcall CLuaInterp::GetRunState()
{
	return m_nRunState;
}

#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

// ��ȡ��ǰ��ջ����
int __stdcall CLuaInterp::GetCurStackLevel()
{
	// TODO: Add your code here
	int level = 1;  /* skip level 0 (it's this function) */
	int firstpart = 1;  /* still before eventual `...' */
	lua_Debug ar;
	while (lua_getstack(pLState, level++, &ar)) 
	{
		if (level > LEVELS1 && firstpart) 
		{
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(pLState, level+LEVELS2, &ar))
				level--;  /* keep going */
			else 
			{
				while (lua_getstack(pLState, level+LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}
	}

	return level;
}

// ��ȡ�ؼ����б�
int __stdcall CLuaInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	// TODO: Add your code here
	CString strType = lpszType;
	if(strType == "platform")	// ƽ̨��չ�ؼ���
	{
		asKeyWord.Add("MessageBox");
		asKeyWord.Add("FileDlg");
		asKeyWord.Add("PathSelect");
		asKeyWord.Add("ProgressDlg");
		asKeyWord.Add("OptionTree");
		asKeyWord.Add("tclLogin");
		asKeyWord.Add("output");
		asKeyWord.Add("StatusBar");
		asKeyWord.Add("pwait");
		asKeyWord.Add("TaskDialog");
		asKeyWord.Add("PopupControl");
		asKeyWord.Add("ImageManager");
	}else
	if(strType == "platform_keyword_tooltip")	// ƽ̨��չ�ؼ���ʹ��˵��
	{
		asKeyWord.Add("DESCRIPTION:\r\n MessageBox\r\nSYNOPSIS:\r\n MessageBox(?-option?, ?-title?, message)");
		asKeyWord.Add("DESCRIPTION:\r\n File dialog\r\nSYNOPSIS:\r\n FileDlg ?-option?...");
		asKeyWord.Add("DESCRIPTION:\r\n Path select dialog\r\nSYNOPSIS:\r\n PathSelect title ?InitPath?");
		asKeyWord.Add("DESCRIPTION:\r\n Progress dialog\r\nSYNOPSIS:\r\n ProgressDlg create ?caption? ?status? ?range? ?cancelscript?\r\n ProgressDlg setpos pos\r\n ProgressDlg setstatus msg\r\n ProgressDlg end");
		asKeyWord.Add("DESCRIPTION:\r\n Option tree dialog\r\nSYNOPSIS:\r\n OptionTree -optionList\r\n -- ?-option? ?T\"text\"? ?E\"edit\"? ?L\"list\"?\r\n -- option:\r\n -- -P(parentindex)\r\n -- -CHECK\r\n -- -OPT_MSG\r\n -- -OPT_HELP\r\n -- -OPT_ITEM\r\n -- -OPT_GROUP\r\n -- -OPT_CHECKBOX\r\n -- -OPT_RADIOBUTTON\r\n -- -OPT_EDIT\r\n -- -OPT_COMBO\r\n -- -ICO_FOLDER\r\n -- -ICO_GROUP\r\n -- -ICO_EDIT\r\n -- -ICO_COMBO\r\n -- -ICO_MFOLDER\r\n -- -ICO_MGROUP\r\n -- -ICO_MEDIT\r\n -- -ICO_MCOMBO\r\n -- -ICO_INFO\r\n -- -ICO_DOWNARROW\r\n -- -ICO_UPARROW");
		asKeyWord.Add("DESCRIPTION:\r\n Common Login dialog\r\nSYNOPSIS:\r\n tclLogin ?-title text?\r\n          ?-info text?\r\n          ?-laddress label?\r\n          ?-lusername label?\r\n          ?-lpassword label?\r\n          ?-ldomain label?\r\n          ?-address var default?\r\n          ?-username var default?\r\n          ?-password var default?\r\n          ?-domain var default?\r\n          ?-domainlist list?\r\n          ?-check var text default(true|false)?\r\n          ?-focus inputcontrol?");
		asKeyWord.Add("DESCRIPTION:\r\n Output\r\nSYNOPSIS:\r\n output -format -id|title FormatStr\r\n output -create title\r\n output -close -id|title\r\n output -active -id|title\r\n output -clear -id|title\r\n output -getinfo -id|title param\r\n output -getinfo title window-id\r\n output -direct text");
		asKeyWord.Add("DESCRIPTION:\r\n StatusBar\r\nSYNOPSIS:\r\n StatusBar -createpane\r\n StatusBar -delpane\r\n StatusBar -settext\r\n StatusBar -setwidth\r\n StatusBar -seticon\r\n StatusBar -progress");
		asKeyWord.Add("DESCRIPTION:\r\n Wait with event loop\r\nSYNOPSIS:\r\n pwait time(ms)\r\n pwait CreateEvent ?id?\r\n pwait WaitEvent id ?timeout?\r\n pwait AwakeEvent id\r\n pwait DeleteEvent id");
		asKeyWord.Add("DESCRIPTION:\r\n Task dialog\r\nSYNOPSIS:\r\n TaskDialog create optionList\r\n TaskDialog refresh optionList");
		asKeyWord.Add("DESCRIPTION:\r\n Popup control\r\nSYNOPSIS:\r\n PopupControl show optionList");
		asKeyWord.Add("DESCRIPTION:\r\n Image manager\r\nSYNOPSIS:\r\n ImageManager seticon command-id icon-file");
	}

	return asKeyWord.GetSize();
}

// ���õ����¼�
int __stdcall CLuaInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	CString strParam = lpszParam;

	m_nCurCommand = nEvent;
	::SetEvent(m_hDebugEvent);

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
			SetBreakLineNo(nParam);
		}else
		{
			// �����ϵ�
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
int __stdcall CLuaInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CLuaInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CLuaInterp::GetEnableProfileAnalyse()
{
	// TODO: Add your code here
	return m_bProfileEnable;
}

// ��ȡ��Ч��������
int __stdcall CLuaInterp::GetAvailableLineCount()
{
	// TODO: Add your code here
	return m_nTDLineCount;
}

// ��ȡ�������и��Ǳ�
LPVOID __stdcall CLuaInterp::GetProfileMap()
{
	// TODO: Add your code here
	return (void*)(&m_ProfileMap);
}

// �ж�ָ�������Ƿ�֧��
int __stdcall CLuaInterp::MethodIsSupport(LPCTSTR lpszMethod)
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

#define SEPATATOE "<<&@&>>"
// ��ʼ������ģʽ
int __stdcall CLuaInterp::InitialDebugMode(CString& strResult)
{
	// TODO: Add your code here
	// ��ʼ���������Դ���

	CString strWinVar, strWinObj, strWinFunc, strWinStack;
	LOADRCSTRING( strWinVar, IDS_WINDOW_VAR );
	LOADRCSTRING( strWinObj, IDS_WINDOW_OBJ );
	LOADRCSTRING( strWinFunc, IDS_WINDOW_FUNC );
	LOADRCSTRING( strWinStack, IDS_WINDOW_STACK );

	CString strColName, strColValue, strColType, strColSource, strColLine, strColWhat;
	LOADRCSTRING( strColName, IDS_COLUMN_NAME );
	LOADRCSTRING( strColValue, IDS_COLUMN_VALUE );
	LOADRCSTRING( strColType, IDS_COLUMN_TYPE );
	LOADRCSTRING( strColSource, IDS_COLUMN_SOURCE );
	LOADRCSTRING( strColLine, IDS_COLUMN_LINE );
	LOADRCSTRING( strColWhat, IDS_COLUMN_WHAT );

	// ��ʼ����������
	m_nOutputVar = ::atoi(pIPlatUI->OutputGet(-1, strWinVar));
	if(m_nOutputVar <= 0)
	{
		//�´��ڣ�����
		m_nOutputVar = pIPlatUI->AddOutput(strWinVar + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputVar, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputVar, "AddColumn " + strColName + ",120");
	pIPlatUI->OutputSet(m_nOutputVar, "AddColumn " + strColValue + ",100");
	pIPlatUI->OutputSet(m_nOutputVar, "AddColumn " + strColType + ",60,AutoSize");
	pIPlatUI->OutputSet(m_nOutputVar, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputVar, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputVar, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputVar, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputVar, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputVar = pIPlatUI->AddOutput(strWinVar + "##GRID");

	//pIPlatUI->OutputSet(m_nOutputVar, "DelItem ALL");

	// ��ʼ�����󴰿�
	m_nOutputObject = ::atoi(pIPlatUI->OutputGet(-1, strWinObj));
	if(m_nOutputObject <= 0)
	{
		//�´��ڣ�����
		m_nOutputObject = pIPlatUI->AddOutput(strWinObj + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputObject, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputObject, "AddColumn " + strColName + ",120");
	pIPlatUI->OutputSet(m_nOutputObject, "AddColumn " + strColValue + ",100");
	pIPlatUI->OutputSet(m_nOutputObject, "AddColumn " + strColType + ",60,AutoSize");
	pIPlatUI->OutputSet(m_nOutputObject, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputObject, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputObject, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputObject, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputObject, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputObject = pIPlatUI->AddOutput(strWinObj + "##GRID");

	//pIPlatUI->OutputSet(m_nOutputObject, "DelItem ALL");

	// ��ʼ����������
	m_nOutputFunc = ::atoi(pIPlatUI->OutputGet(-1, strWinFunc));
	if(m_nOutputFunc <= 0)
	{
		//�´��ڣ�����
		m_nOutputFunc = pIPlatUI->AddOutput(strWinFunc + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputFunc, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputFunc, "AddColumn " + strColName + ",200");
	pIPlatUI->OutputSet(m_nOutputFunc, "AddColumn " + strColType + ",80,AutoSize");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputFunc, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputFunc, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputFunc = pIPlatUI->AddOutput(strWinFunc + "##GRID");

	//pIPlatUI->OutputSet(m_nOutputFunc, "DelItem ALL");

	// ��ʼ����ջ����
	m_nOutputStack = ::atoi(pIPlatUI->OutputGet(-1, strWinStack));
	if(m_nOutputStack <= 0)
	{
		//�´��ڣ�����
		m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputStack, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColName + ",100");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColSource + ",100");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColLine + ",50");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColWhat + ",50");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColType + ",60,AutoSize");
	pIPlatUI->OutputSet(m_nOutputStack, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputStack, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputStack, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Add(NoSort)");
	pIPlatUI->OutputSet(m_nOutputStack, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID");

	//pIPlatUI->OutputSet(m_nOutputStack, "DelItem ALL");

	// �����������
	pIPlatUI->ActiveOutput(m_nOutputVar);

	return TRUE;
}

CString CLuaInterp::LuaMakeNumber(double dnum)
{
	CString strRet;
    long num = (long) dnum;
    if ((double) num == dnum)
	{
        strRet.Format("%ld (0x%lx)", num, num);
	}else
	{
		strRet.Format("%g", dnum);
	}

    return strRet;
}

CString CLuaInterp::LuaMakeBoolean(int num)
{
    return num != 0 ? "true" : "false";
}

CString CLuaInterp::LuaGetTableInfo(int nIndex)
{
	CString strRet;
    //int         nTag   = wxlua_ttag(pLState, nIndex);
	#ifdef USE_LUA_51
	int         nItems = luaL_getn(pLState, nIndex);
	#endif
	#ifdef USE_LUA_52
    int         nItems = lua_objlen(pLState, nIndex);
	#endif
    const void *pItem  = lua_topointer(pLState, nIndex);

    strRet.Format("0x%p", pItem);

    if (nItems > 0)
        strRet.Format("0x%p (approx %d items)", pItem, nItems);

    //if (nTag != WXLUA_NOTAG)
    //    strRet.Format("0x%p (tag %d)"), pItem, nTag);
	return strRet;
}

int CLuaInterp::EnumerateTable(int nTableRef, int nIndex, CUIntArray& auReferences)
{
    int count = 0;

    CString strType;
    CString strValue;
    CString strName;

    //int flag_type = WXLUA_DEBUGITEM_TUNKNOWN;

    if ((nTableRef == -1) || (nTableRef == LUA_GLOBALSINDEX))
    {
        /*wxlState.GetGlobals();
        GetTypeValue(wxlState, -1, type, value, &flag_type);
        int nRef = wxlua_tinsert(L, -1);
        Add(new wxLuaDebugItem(wxT("Globals"), wxT("Table"), value, wxT(""), nRef, 0, WXLUA_DEBUGITEM_TTABLE|WXLUA_DEBUGITEM_LUAREFED));
        references.Add(nRef);*/
    }
    else if (nTableRef == LUA_REGISTRYINDEX)
    {
        /*wxlState.lua_PushValue(LUA_REGISTRYINDEX);
        GetTypeValue(wxlState, -1, type, value, &flag_type);
        int nRef = wxlua_tinsert(L, -1);
        Add(new wxLuaDebugItem(wxT("Lua Registry"), wxT("Table"), value, wxT(""), nRef, 0, WXLUA_DEBUGITEM_TTABLE|WXLUA_DEBUGITEM_LUAREFED));
        references.Add(nRef);*/
    }
    else
    {
        // push the table onto the stack to iterate through
        //if (wxlState.tget(tableRef))
        {
            int nTop = lua_gettop(pLState);

            //wxlua_pushkey_wxLuaObjects(L);
            lua_rawget(pLState, LUA_REGISTRYINDEX);       // pop key, push result (the obj table)
            bool is_lreg_objTable = (lua_equal(pLState, -1, -2) != 0);
            lua_pop(pLState, 1);
            
            // start iterating
            lua_pushnil(pLState);
            while (lua_next(pLState, nTop) != 0)
            {
                // get the index, just want the name=value, type is dummy here
                //GetTypeValue(wxlState, -2, type, name);
                // get the type and value
                //GetTypeValue(wxlState, -1, type, value, &flag_type);

                if (is_lreg_objTable)
                {
                    long tag = -1;
                    //if (value.BeforeFirst(wxT(' ')).ToLong(&tag))
                    //    value = value.BeforeFirst(wxT(' ')) + wxT(" (") + wxlState.GetLuaTagName(tag) + wxT(")");
                }

                //wxPrintf(wxT("wxLuaDebugData::EnumerateTable %d type='%s' key='%s' val='%s'\n"), count, type.c_str(), name.c_str(), value.c_str());

                int nRef = LUA_NOREF;
                if (lua_istable(pLState, -1))
                {
                    /*nRef = wxlua_tisrefed(L, -1); // don't duplicate refs

                    if (nRef == LUA_NOREF)
                    {
                        flag_type |= WXLUA_DEBUGITEM_LUAREFED;
                        nRef = wxlua_tinsert(L, -1); // pops value
                        references.Add(nRef);
                    }
                    else
                        lua_pop(pLState, 1);
					*/
                }
                else
                    lua_pop(pLState, 1);

                //Add(new wxLuaDebugItem(name, type, value, wxT(""), nRef, nIndex, flag_type));
                count++;
            }

            lua_pop(pLState, 1); // remove reference
        }
    }

    return count;
}

// ��ȡָ��������ֵ(ת��Ϊ�ַ���)
CString CLuaInterp::GetValueString(int nIndex)
{
	CString strValue = "";
	int ntype = lua_type(pLState, nIndex);
	switch(ntype)
	{
	case LUA_TNIL:
		strValue = "nil";
		break;
	case LUA_TBOOLEAN:
		strValue = LuaMakeBoolean(lua_toboolean(pLState, nIndex));
		break;
	case LUA_TNUMBER:
		strValue = LuaMakeNumber(lua_tonumber(pLState, nIndex));
		break;
	case LUA_TSTRING:
		strValue.Format("%s", lua_tostring(pLState, nIndex));
		break;
	case LUA_TTABLE:
		strValue = LuaGetTableInfo(nIndex);
		break;
	case LUA_TFUNCTION:
		if (lua_iscfunction(pLState, nIndex))
		{
			strValue.Format("%p", lua_tocfunction(pLState, nIndex));
		}else
		{
			strValue.Format("%p", lua_topointer(pLState, nIndex));
		}
		break;
	case LUA_TTHREAD:
		strValue.Format("%p", lua_topointer(pLState, nIndex));
		break;
	default:
		break;
	}
	return strValue;
}

// ��ȡָ��������������Ϣ
CString CLuaInterp::Describe(int nIndex)
{
	CString strRet;
	int ntype = lua_type(pLState, nIndex);
	strRet.Format("%s : %.64s", lua_typename(pLState, ntype), GetValueString(nIndex));
	return strRet;
}

// ��ȡָ���������������·����
CString get_var_fullname(CLuaInterp* pInterp, CString strhItem)
{
	CString strArrayName = "";
	CString strTmp;
	CString strhParent = "";
	while((strhParent = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ItemParent " + strhItem)) != "ROOT")
	{
		strTmp.Format("<#%s,2>", strhItem);
		CString strItemType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
		if(strItemType == "table")
		{
			strTmp.Format("<#%s,0>", strhItem);
			strArrayName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp) + strArrayName;
		}
		strhItem = strhParent;
	}

	strTmp.Format("<#%s,2>", strhItem);
	CString strItemType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	if(strItemType == "table")
	{
		strTmp.Format("<#%s,0>", strhItem);
		strArrayName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp) + strArrayName;
	}

	return strArrayName;
}

// ��������˫������
void callback_lua_ldclick(CLuaInterp* pInterp, LPCTSTR lpszInfo)
{
	TRACE("callback info=%s\n", lpszInfo);
	CString strInfo = lpszInfo;

	if(pInterp == NULL || !CheckLuaInterpExist(pInterp))
	{
		return;
	}

	int nPos = strInfo.Find(",");
	if(nPos == -1)
	{
		return;
	}

	LPVOID lpItem = (LPVOID)(atoi(strInfo.Left(nPos)));

	CString strTmp;
	strTmp.Format("<%d,0>", lpItem);
	CString strVarName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	strTmp.Format("<%d,1>", lpItem);
	CString strVarValue = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	strTmp.Format("<%d,2>", lpItem);
	CString strVarType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);

	if(strVarType == "")
	{
		return;
	}

	if(strVarType == "table")
	{
		// �����table,��չ��
		CString strhItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "SelItem");

		// �ж��Ǿֲ���������ȫ�ֱ���
		CString strParentItem = strhItem;
		while(pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ItemParent " + strParentItem) != "ROOT")
		{
			strParentItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ItemParent " + strParentItem);
		}
		CString strLocalGlobal = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "<#" + strParentItem + ",0>");
		BOOL bIsLocal = (strLocalGlobal == "Locals");

		CString strTableName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "<#" + strhItem + ",0>");
		int nChildCount = atoi(pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ChildCount " + strhItem));
		if(nChildCount == 0)
		{
			CString strTmp;

			if(bIsLocal)
			{
				// ��ȡ�ֲ�����Table
				int nLevel = 0;
				lua_Debug ar;
				if ( lua_getstack (pInterp->pLState, nLevel, &ar) )
				{
					lua_getlocal(pInterp->pLState, &ar, 1);
				}
			}else
			{
				// ��ȡȫ�ֱ���Table
				lua_getglobal(pInterp->pLState, strTableName);
			}
			if(lua_istable(pInterp->pLState, -1))
			{
				lua_pushnil(pInterp->pLState);
				while (lua_next(pInterp->pLState, -2))
				{
					//if(lua_istable(pInterp->pLState, -1))	// Ƕ����������
					//{
						// ��ʱ��֧��
						/*CString strVarName = lua_tostring(pInterp->pLState, -2);
						if(strVarName == "1")
						{
							lua_pop(pInterp->pLState, 1);
							lua_pop(pInterp->pLState, 1);
						}*/

						//lua_pop(pInterp->pLState, 1);
					//}else
					{
						strTmp.Format("<%s,7>%s%s%s%s%s", strhItem,
								lua_tostring(pInterp->pLState, -2), SEPATATOE,
								pInterp->GetValueString(-1), SEPATATOE,
								lua_typename(pInterp->pLState, lua_type(pInterp->pLState, -1)));
						pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, strTmp);

						// ����˫��������
						CString strlasthItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "LastItem");
						pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, "<I>Set Script<#" + strlasthItem + "> <1>");

						lua_pop(pInterp->pLState, 1); // pop value, keep key for next iteration
					}
				}
				lua_pop(pInterp->pLState, 1); // pop table

				pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, "<T>ExpandItem<#" + strhItem + ">");
			}
		}
	}else
	{
		DLL_LOADSTRING(strWatchVarTitle, IDS_WATCH_VAR_TITLE);
		DLL_LOADSTRING(strWatchVarName, IDS_WATCH_VAR_NAME);
		DLL_LOADSTRING(strWatchVarValue, IDS_WATCH_VAR_VALUE);
		DLL_LOADSTRING(strWatchVarType, IDS_WATCH_VAR_TYPE);
		CString strhItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "SelItem");
		strVarName = get_var_fullname(pInterp, strhItem) + "[" + strVarName + "]";
		CString strMsg;
		strMsg.Format("%s:%s\n%s:%s\n%s:%s", strWatchVarName, strVarName, strWatchVarValue, strVarValue, strWatchVarType, strVarType);
		::MessageBox(AfxGetMainWnd()->m_hWnd, strMsg, strWatchVarTitle, MB_OK | MB_ICONINFORMATION);
	}
}

// ˢ�±�������
int CLuaInterp::RefreshVarWindow()
{
	CString strOutput, strName, strValue, strType;
	int nParent;

	CTTreeItemArray aItems;

	TTREEITEM item;
	item.hItem = NULL;
	item.nParent = -1;
	item.nExpand = 0;

	// Locals
	strOutput = "Locals";
	item.strLine = strOutput;
	item.nParent = -1;
	item.nImage = 4;
	item.nExpand = 1;
	item.nClickFunc = 0;
	aItems.Add(item);
	nParent = aItems.GetSize()-1;

	int nLevel = 0;
	lua_Debug ar;
	if ( lua_getstack (pLState, nLevel, &ar) )
	{
		int i = 1;
        const char *name;
        while ((name = lua_getlocal(pLState, &ar, i++)) != NULL) {
			int ntype = lua_type(pLState, -1);
			strName = name;
			if(strName == "(*temporary)")	// ���˵���ʱ����
			{
				lua_pop(pLState, 1);  /* remove variable value */
				continue;
			}

			strValue = GetValueString(-1);
			strType = lua_typename(pLState, ntype);

			strOutput =	strName + SEPATATOE + strValue + SEPATATOE + strType;
			item.strLine = strOutput;
			item.nParent = nParent;
			item.nImage = 5;
			item.nExpand = 0;
			item.nClickFunc = 1;
			aItems.Add(item);

			lua_pop(pLState, 1);  /* remove variable value */
        }
	}

	// Globals
	strOutput = "Globals";
	item.strLine = strOutput;
	item.nParent = -1;
	item.nImage = 4;
	item.nExpand = 1;
	item.nClickFunc = 0;
	aItems.Add(item);
	nParent = aItems.GetSize()-1;

	#ifdef USE_LUA_51
	lua_pushvalue(pLState, LUA_GLOBALSINDEX);
	#endif
	#ifdef USE_LUA_52
	//lua_pushvalue(pLState, LUA_GLOBALSINDEX);	// Lua 5.2û��global
	#endif

	lua_pushnil(pLState);  /* first key */
	while (lua_next(pLState, -2))
	{
		// ���˵�����
		if(LUA_TFUNCTION == lua_type(pLState, -1))
		{
			lua_pop(pLState, 1); // pop value, keep key for next iteration;
			continue;
		}

		// ��ȡÿ��������Ϣ
		strName = lua_tostring(pLState, -2);
		strValue = GetValueString(-1);
		strType = lua_typename(pLState, lua_type(pLState, -1));
		strOutput =	strName + SEPATATOE + strValue + SEPATATOE + strType;
		item.strLine = strOutput;
		item.nParent = nParent;
		item.nImage = 5;
		item.nExpand = 0;
		item.nClickFunc = 1;
		aItems.Add(item);
		
		lua_pop(pLState, 1); // pop value, keep key for next iteration;
	}
	lua_pop(pLState, 1); // pop table of globals;

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputVar, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_lua_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputVar, strCallback);

	return TRUE;
}

// ˢ�º�������
int CLuaInterp::RefreshFuncWindow()
{
	CString strOutput, strName, strType;

	CTTreeItemArray aItems;

	TTREEITEM item;
	item.hItem = NULL;
	item.nParent = -1;
	item.nExpand = 0;

	#ifdef USE_LUA_51
	lua_pushvalue(pLState, LUA_GLOBALSINDEX);
	#endif
	#ifdef USE_LUA_52
	//lua_pushvalue(pLState, LUA_GLOBALSINDEX);	// Lua 5.2û��global
	#endif

	lua_pushnil(pLState);  /* first key */
	while (lua_next(pLState, -2))
	{
		int nType = lua_type(pLState, -1);
		if(LUA_TFUNCTION != nType)
		{
			lua_pop(pLState, 1); // pop value, keep key for next iteration;
			continue;
		}

		// ��ȡÿ��������Ϣ
		strName = lua_tostring(pLState, -2);
		if(lua_iscfunction(pLState, -1))
		{
			strType = "cfunction";
		}else
		{
			strType = lua_typename(pLState, nType);
		}
		strOutput =	strName + SEPATATOE + strType;
		item.strLine = strOutput;
		item.nImage = 5;
		aItems.Add(item);
		
		lua_pop(pLState, 1); // pop value, keep key for next iteration;
	}
	lua_pop(pLState, 1); // pop table of globals;

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputFunc, (LPARAM)(LPVOID)(&aItems));

	return TRUE;
}

// Lua��ջ������˫������
void callback_lua_stack_ldclick(CLuaInterp* pInterp, LPCTSTR lpszInfo)
{
	TRACE("callback info=%s\n", lpszInfo);
	CString strInfo = lpszInfo;

	if(pInterp == NULL || !CheckLuaInterpExist(pInterp))
	{
		return;
	}

	int nPos = strInfo.Find(",");
	if(nPos == -1)
	{
		return;
	}

	LPVOID lpItem = (LPVOID)(atoi(strInfo.Left(nPos)));

	CString strTmp;
	strTmp.Format("<%d,1>", lpItem);
	CString strStackSource = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputStack, strTmp);
	strTmp.Format("<%d,2>", lpItem);
	int nStackLine = atoi(pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputStack, strTmp));

	if(GetFileAttributes(strStackSource) == 0xFFFFFFFF)
	{
		return;
	}

	pInterp->pIPlatUI->SendOwmCmd("org.owm.edit", OC_OPENFILE, (WPARAM)(LPTSTR)(LPCTSTR)strStackSource, 0, NULL);
	pInterp->pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETCURENTLINE, (WPARAM)(LPTSTR)(LPCTSTR)strStackSource, nStackLine, NULL);
}

// ˢ�¶�ջ����
int CLuaInterp::RefreshStackWindow()
{
	CString strOutput;

	CTTreeItemArray aItems;

	TTREEITEM item;
	item.hItem = NULL;
	item.nParent = -1;
	item.nExpand = 0;
	item.nImage = 27;
	item.nClickFunc = 1;

	int nLevel = 0;
	lua_Debug ar;
	CString strTmp;
	while ( lua_getstack (pLState, nLevel, &ar) )
	{
		lua_getinfo(pLState, "lnuS", &ar);
		if ( ar.source[0]=='@' )
		{
			// ��һ���ַ�Ϊ@��ʾ��һ���ļ�
			strOutput = "";

			if ( ar.name )
				strOutput += ar.name;
			strOutput += SEPATATOE;

			strOutput += ar.source+1;
			strOutput += SEPATATOE;

			strTmp.Format("%d", ar.currentline);
			strOutput += strTmp;
			strOutput += SEPATATOE;

			if ( ar.what )
				strOutput += ar.what;
			strOutput += SEPATATOE;

			if ( ar.namewhat )
				strOutput += ar.namewhat;

			item.strLine = strOutput;
			aItems.Add(item);
		}else
		{
			// ��һ���ű��ַ���
		}

		++nLevel;
	};

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputStack, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_lua_stack_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputStack, strCallback);

	return TRUE;
}

// ���µ��Դ�����Ϣ
int __stdcall CLuaInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
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
		bWindowProc = TRUE;
		bWindowStack = TRUE;
	}

	if(bWindowVar)
	{
		RefreshVarWindow();
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputVar);
		}
	}
	if(bWindowObject)
	{
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputObject);
		}
	}
	if(bWindowProc)
	{
		RefreshFuncWindow();
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputFunc);
		}
	}
	if(bWindowStack)
	{
		RefreshStackWindow();
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputStack);
		}
	}

	return TRUE;
}

// ���ýű�����
int __stdcall CLuaInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
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
int __stdcall CLuaInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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

	// ����״̬
	m_nRunState = SCRIPT_STATE_RUN;

	CString strProCompFile	= GetInterpRootPath();
	strProCompFile	+= "bin\\luac5.1.exe";

	CString strOutFile = strSrcFile;
	strOutFile.MakeLower();
	int nDotPos = strOutFile.ReverseFind('.');
	if(nDotPos != -1)
	{
		strOutFile.Delete(nDotPos, strOutFile.GetLength() - nDotPos);
	}

	// �Զ���������ļ�
	BOOL bAutoOutFile = AfxGetApp()->GetProfileInt(REG_LUA_COMPILE_SUBKEY, REG_LUA_COMPILE_AUTO_OUTFILE, TRUE);
	if(bAutoOutFile)
	{
		strOutFile += ".luac";
	}else
	{
		// �û���������ļ�
		CFileDialog dialog(FALSE, "*.*", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "All files(*.*)|*.*||");
		if(IDOK == dialog.DoModal())
		{
			strOutFile = dialog.GetPathName();
		}else
		{
			m_nRunState = SCRIPT_STATE_END;
			return FALSE;
		}
	}

	// �ļ�������ڣ��Ƿ�ǿ�Ƹ���
	BOOL bForceOverwrite = AfxGetApp()->GetProfileInt(REG_LUA_COMPILE_SUBKEY, REG_LUA_COMPILE_FORCE_OVERWRITE, FALSE);
	if(!bForceOverwrite)
	{
		if(GetFileAttributes(strOutFile) != 0xFFFFFFFF)
		{
			if(AfxMessageBox("Compile output file is exist, overwrite it or not?", MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				m_nRunState = SCRIPT_STATE_END;
				return FALSE;
			}
		}
	}

	CString strParams = "";

	strParams += "-o \"";
	strParams += strOutFile;
	strParams += "\"";

	strParams += " \"";
	strParams += strSrcFile;
	strParams += "\"";

	strDestFile = strOutFile;

	try {
	////////////////////////////////////////////
	if(!RunProgramWithPipe(strProCompFile, strParams))
	{
		m_nRunState = SCRIPT_STATE_END;
		return FALSE;
	}
	////////////////////////////////////////////
	}catch(...)
	{
		m_nRunState = SCRIPT_STATE_END;
	}

	m_nRunState = SCRIPT_STATE_END;
	return TRUE;
}

// ����
int __stdcall CLuaInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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

	// ����ļ���
	CString strOutFile = strSrcFile;
	CString strOutFileName = ""; // �ļ���,����·��
	strOutFile.MakeLower();
	strOutFile.Replace("/", "\\");
	int nPos = strOutFile.ReverseFind('\\');
	if(nPos != -1)
	{
		strOutFileName = strOutFile;
		strOutFileName.Delete(0, nPos+1);
		strOutFile = strOutFile.Left(nPos);
	}
	int nDotPos = strOutFileName.ReverseFind('.');
	if(nDotPos != -1)
	{
		strOutFileName.Delete(nDotPos, strOutFileName.GetLength() - nDotPos);
	}

	// wxluafreeze.lua�ļ���
	CString strFreezeLua = GetInterpRootPath();
	strFreezeLua += "bin\\wxluafreeze.lua";

	// wxluafreeze.exe�ļ���
	CString strFreezeLuaApp = GetInterpRootPath();
	strFreezeLuaApp += "bin\\wxluafreeze.exe";

	CString strCmd;
	strCmd.Format("arg={\"%s\",\"%s\",\"%s\"}", strFreezeLuaApp, strSrcFile, strOutFile + "\\" + strOutFileName + ".exe");
	strCmd.Replace("\\", "\\\\");
	RunScriptCommand(strCmd);

	// ִ��wxluafreeze.lua
	RunScriptFile(strFreezeLua);

	strDestFile = strOutFile + "\\" + strOutFileName + ".exe";

	// �滻��ִ���ļ���ͼ��
	CString strIconFile = GetInterpRootPath();
	strIconFile += "bin\\luaexe.ico";	// Ĭ�ϵ�ͼ���ļ�
	strIconFile.Replace("/", "\\");
	BOOL bReplaceIcon = AfxGetApp()->GetProfileInt(REG_LUA_BUILD_SUBKEY, REG_LUA_BUILD_REPLACE_ICON, FALSE);
	if(bReplaceIcon)
	{
		// ͼ��ѡ��ķ�ʽ
		CString strNewIcon = "";
		int nSelectIconType = AfxGetApp()->GetProfileInt(REG_LUA_BUILD_SUBKEY, REG_LUA_BUILD_SELECT_ICON_TYPE, 0);
		if(nSelectIconType == 0)	// ʹ��Դ����Ŀ¼�µ�ͬ��ͼ���ļ�
		{
			strNewIcon = strOutFile + "\\" + strOutFileName + ".ico";
		}else
		if(nSelectIconType == 1)	// �û�ѡ��ͼ���ļ�
		{
			// �û���������ļ�
			CFileDialog dialog(FALSE, "*.ico", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Icon files(*.ico)|*.ico||");
			if(IDOK == dialog.DoModal())
			{
				strNewIcon = dialog.GetPathName();
			}else
			{
				if(pIPlatUI)
				{
					pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=255,0,0");
					pIPlatUI->Output(_WND_DEBUG, "Not select program icon, use default icon!\r\n");
					pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,0");
				}
				return TRUE;
			}
		}
		if(GetFileAttributes(strNewIcon) == 0xFFFFFFFF)
		{
			if(pIPlatUI)
			{
				pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=255,0,0");
				pIPlatUI->Output(_WND_DEBUG, "Not exist specified icon file, use default icon!\r\n");
				pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,0");
			}
			return TRUE;
		}

		// ��ȡ�滻ͼ��ű�������ִ��
		CString strIconReplaceScript = GetPlatRootPath();
		strIconReplaceScript += _T("interps\\tcl\\bin\\iconreplace.tcl");
		strIconReplaceScript.Replace("\\", "/");
		CString strRes;
		CString strCmd;
		strCmd.Format("source %s;set filename {%s};set newicon {%s}; set fwname {%s};IconReplace $filename $newicon $fwname",
			strIconReplaceScript,
			strDestFile,
			strNewIcon,
			strIconFile);

		if(pIPlatUI)
		{
			IInterp* pInterp = (IInterp*)(pIPlatUI->CreateVciObject("org.interp.tcl", ""));
			if(pInterp)
			{
				pInterp->RunScriptCommand(strCmd);
				int nLine = pInterp->GetErrorLine();
				if(nLine != 0)
				{
					CString strResult = pInterp->GetResult();
					AfxMessageBox(strResult);
				}
				pIPlatUI->ReleaseVciObject(pInterp, TRUE);
			}
		}
	}

	return TRUE;
}

//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(LuaInterp)
CONTROL_INTERFACE_IMPLEMENT(LuaInterp)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CLuaInterp::XLuaInterp::GetExtraInfo(LPVOID lpRefData)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
//{{VCI_INTERFACE_IMPLEMENT_BEGIN
// ����ƽ̨UI�ӿ�ָ��
int __stdcall CLuaInterp::XLuaInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetIPlatUI(lpIPlatUI);
}

int __stdcall CLuaInterp::XLuaInterp::SetLibPath(LPCTSTR lpszPath)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetLibPath(lpszPath);
}

int __stdcall CLuaInterp::XLuaInterp::IsInterpRunning()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->IsInterpRunning();
}

int __stdcall CLuaInterp::XLuaInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetScriptFile(lpszPathName);
}

CString __stdcall CLuaInterp::XLuaInterp::GetScriptFile()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetScriptFile();
}

int __stdcall CLuaInterp::XLuaInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->RunScriptFile(lpszPathName);
}

int __stdcall CLuaInterp::XLuaInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->RunScriptCommand(lpszTclCmd);
}

int __stdcall CLuaInterp::XLuaInterp::StopScript()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->StopScript();
}

LPVOID __stdcall CLuaInterp::XLuaInterp::GetInterp()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetInterp();
}

int __stdcall CLuaInterp::XLuaInterp::GetInterpID()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetInterpID();
}

void __stdcall CLuaInterp::XLuaInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	pObj->SetInterpName(lpszInterptName);
}

CString __stdcall CLuaInterp::XLuaInterp::GetInterpName()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetInterpName();
}

int __stdcall CLuaInterp::XLuaInterp::GetErrorLine()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetErrorLine();
}

CString __stdcall CLuaInterp::XLuaInterp::GetResult()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetResult();
}

int __stdcall CLuaInterp::XLuaInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetVar(lpszVarName, lpszVarValue);
}

CString __stdcall CLuaInterp::XLuaInterp::GetVar(LPCTSTR lpszVarName)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetVar(lpszVarName);
}

int __stdcall CLuaInterp::XLuaInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->RegisterCommand(pInterp, lpszCmd, lpFunc);
}

int __stdcall CLuaInterp::XLuaInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetInterpParam(lpszParamName, lpszParamValue);
}

CString __stdcall CLuaInterp::XLuaInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetInterpParam(lpszParamName);
}

int __stdcall CLuaInterp::XLuaInterp::SetRunMode(int nRunMode)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetRunMode(nRunMode);
}

int __stdcall CLuaInterp::XLuaInterp::GetRunMode()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetRunMode();
}

int __stdcall CLuaInterp::XLuaInterp::SetRunState(int nRunState)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetRunState(nRunState);
}

int __stdcall CLuaInterp::XLuaInterp::GetRunState()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetRunState();
}

int __stdcall CLuaInterp::XLuaInterp::GetCurStackLevel()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetCurStackLevel();
}

// ��ȡ�ؼ����б�
int __stdcall CLuaInterp::XLuaInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetKeyWordList(lpszType, asKeyWord);
}

int __stdcall CLuaInterp::XLuaInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetDebugEvent(nEvent, nParam, lpszParam);
}

int __stdcall CLuaInterp::XLuaInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->SetBreakList(lpszFile, auBreaks);
}

int __stdcall CLuaInterp::XLuaInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetBreakList(lpszFile, auBreaks);
}

int __stdcall CLuaInterp::XLuaInterp::GetEnableProfileAnalyse()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetEnableProfileAnalyse();
}

int __stdcall CLuaInterp::XLuaInterp::GetAvailableLineCount()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetAvailableLineCount();
}

LPVOID __stdcall CLuaInterp::XLuaInterp::GetProfileMap()
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->GetProfileMap();
}

int __stdcall CLuaInterp::XLuaInterp::MethodIsSupport(LPCTSTR lpszMethod)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->MethodIsSupport(lpszMethod);
}

int __stdcall CLuaInterp::XLuaInterp::InitialDebugMode(CString& strResult)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->InitialDebugMode(strResult);
}

int __stdcall CLuaInterp::XLuaInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->RefreshDebugWindow(lpszWindow, bActive, strResult);
}

int __stdcall CLuaInterp::XLuaInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->CallMethod(lpszMethod, asParam);
}

int __stdcall CLuaInterp::XLuaInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->Compile(lpszSrcFile, strDestFile, lpszParams);
}

int __stdcall CLuaInterp::XLuaInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CLuaInterp *pObj = GET_INTERFACE_OBJECT(LuaInterp);
	return pObj->Build(lpszSrcFile, strDestFile, lpszParams);
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

#include "stdafx.h"
#include "CPerlInterp.h"
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
CPerlInterp* GetPerlInterp(LPVOID lpInterp)
{
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CPerlInterp* pInterp = (CPerlInterp*)g_InterpList.GetAt(pos);
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
BOOL CheckPerlInterpExist(CPerlInterp* pPerlInterp)
{
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CPerlInterp* pInterp = (CPerlInterp*)g_InterpList.GetAt(pos);
		if(pPerlInterp == pInterp)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// defines 

#define IS_LOADED (pInterp!=NULL)
#define IS_UNLOADED (pInterp==NULL)

#define SET_CONTEXT PERL_SET_CONTEXT((PerlInterpreter*)pInterp)

#define IS_INVALID_SOBJ(s) (s.m_nIndex < 0 || s.m_nIndex > m_nMaxPackageIndex || m_strAPackages.GetAt(s.m_nIndex).IsEmpty())

#define MAKE_SCOPE_VAR(s, var) \
	if (IS_INVALID_SOBJ(s) || !var || !*var) \
	return false; \
	char beef[256]; \
	if (var[0] == ':' && var[1] == ':') \
	strcpy(beef, var + 2); \
	else \
{ \
	strcpy(beef, m_strAPackages.GetAt(s.m_nIndex)); \
	strcat(beef, "::"); \
	strcat(beef, var); \
} \
	var = (const char*)beef;

#define COMMON_SET(s, var) \
	if (IS_UNLOADED) return false; \
	SET_CONTEXT; \
	MAKE_SCOPE_VAR(s, var);

#define COMMON_GET(s, var) COMMON_SET(s, var)


LPVOID PXReadPerlResource(int nResID, DWORD &dwSize)
{
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hRes = FindResource(hModule,
		MAKEINTRESOURCE(nResID), MAKEINTRESOURCE(RT_PERL));
	if (!hRes)
		return NULL;
	dwSize = SizeofResource(hModule, hRes);
	if (!dwSize)
		return NULL;
	HANDLE hResData = LoadResource(hModule, hRes);
	if (!hResData)
		return NULL;
	return LockResource(hResData);
}

int PXGetFileSize(int fh)
{
	int nPrevPos = _tell(fh);
	_lseek(fh, 0, SEEK_SET);
	int size = _lseek(fh, 0, SEEK_END);
	_lseek(fh, nPrevPos, SEEK_SET);
	return size;
}

bool PXReadFile(LPCSTR szFile, CString &strRead)
{
	bool bRet = false;
	int fh;
	if ((fh = _open(szFile, _O_RDONLY | _O_BINARY)) == -1)
		return false;
	int size = PXGetFileSize(fh);
	if (size > 0)
	{
		LPSTR sBuffer = strRead.GetBuffer(size + 8);
		int nRead = _read(fh, (void*)sBuffer, size);
		sBuffer[size] = 0;
		strRead.ReleaseBuffer();
		bRet = (nRead == size);
	}
	_close(fh);

	return bRet;
}

//////////////////////////////////////////////////////////////////////
// ƽ̨��չ�������
// Perl����C�����Ĳ���˵��,ǰ���Ǵ���Ĳ���,����֮���ǵ���ʱ��ĺ�����
void platform_Print(pTHXo_ CV* cv)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items < 1)
       Perl_croak(aTHX_ "Usage: %s(%s)", "platform_print", "channel, text...");

	CPerlInterp* pInterp = GetPerlInterp((PerlInterpreter*)PERL_GET_CONTEXT);
	if(pInterp == NULL)
	{
		XSRETURN(0);
		return;
	}

	IPlatUI* pIPlatUI = pInterp->pIPlatUI;
	if(pIPlatUI == NULL)
	{
		XSRETURN(0);
		return;
	}

	BOOL bDirectOutput = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT, TRUE);
	int nOT = 2;
	if(bDirectOutput)
	{
		nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
	}

    PERL_UNUSED_VAR(cv); //-W
    {
	BOOL bOutToResult = FALSE;
	//void *	handle = INT2PTR(void *,SvIV(ST(0)));
	char *	strChannel = (char *)SvPV_nolen(ST(0));
	if(strcmp(strChannel, "stdout") == 0)
	{
		pIPlatUI->OutputSet(nOT, "COLOR=0,0,255");	// ���������ɫ
	}else
	if(strcmp(strChannel, "stderr") == 0)
	{
		pIPlatUI->OutputSet(nOT, "COLOR=255,0,0");	// ���������ɫ
	}else
	if(strcmp(strChannel, "toresult") == 0)
	{
		bOutToResult = TRUE;	// ������ű�����ַ���
	}

	for(int i=1; i<items; i++)
	{
		char *	print_text = (char *)SvPV_nolen(ST(i));
		if(bOutToResult)
		{
			pInterp->m_strResult += print_text;
		}else
		{
			pIPlatUI->Output(nOT, print_text);
		}		
	}

	//char *	print_sub = (char *)SvPV_nolen(ST(1));
	//void *	RETVAL;
	//dXSTARG;
	//TRACE("%s\n", outputstr);
    }
    XSRETURN(1);
}

//////////////////////////////////////////////////////////////////////
// ƽ̨��չ���Ժ���,pdb('cmd', lineno, fileindex)
void platform_debug(pTHXo_ CV* cv)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items < 1)
       Perl_croak(aTHX_ "Usage: %s(%s)", "pdb", "'cmd', lineno, ?fileindex?");

	CPerlInterp* pInterp = GetPerlInterp((PerlInterpreter*)PERL_GET_CONTEXT);
	if(pInterp == NULL)
	{
		XSRETURN(0);
		return;
	}

	IPlatUI* pIPlatUI = pInterp->pIPlatUI;
	if(pIPlatUI == NULL)
	{
		XSRETURN(0);
		return;
	}

	// �������ͨ����ģʽ,�������˵�������
	if(SCRIPT_RUN_NORMAL == pInterp->GetRunMode())
	{
		return;
	}

    PERL_UNUSED_VAR(cv); //-W
    {
	////////////////////////////////////////////////////////
	CString strOption = (char *)SvPV_nolen(ST(0));
	int nBreakLineNo = -1;
	if (items > 1)
	{
		nBreakLineNo = SvIV(ST(1));
	}
	CString strBreakName = "";
	CString strBreakFile = "";
	CDebugFile* pDebugFile = NULL;
	int nDebugFileIndex = -1;
	if (items > 2)
	{
		nDebugFileIndex = SvIV(ST(2));
		if(nDebugFileIndex < pInterp->m_aDebugFile.GetSize())
		{
			pDebugFile = &(pInterp->m_aDebugFile[nDebugFileIndex]);
		}
		if(pDebugFile)
		{
			strBreakFile = pDebugFile->m_strFile;
			if(pInterp->GetEnableProfileAnalyse())
			{
				CMap<int,int,int,int>* pProfileMap = (CMap<int,int,int,int>*)(&(pDebugFile->m_ProfileMap));
				pProfileMap->SetAt(nBreakLineNo, 1);
			}
		}
	}else
	{
		if(pInterp->GetEnableProfileAnalyse())
		{
			CMap<int,int,int,int>* pProfileMap = (CMap<int,int,int,int>*)(pInterp->GetProfileMap());
			pProfileMap->SetAt(nBreakLineNo, 1);
		}
	}

	strOption.MakeUpper();
	if(strOption != "")
	{
		if(strOption == "STOP")	// ��ֹ�ű�
		{
			pInterp->StopScript();
		}else
		if(strOption == "NEXT")	// ����һ���ϵ�
		{
			pInterp->SetBreakMode(SCRIPT_BREAK_NORMAL);
			pInterp->SetBreakSignal(TRUE);	// ����ű�
			pInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
		}else
		if(strOption == "GOTO")	// ִ�е�ĳ���жϵ�������ϵ�
		{
			if (items < 2)
			{
				Perl_croak(aTHX_ "Usage: %s(%s)", "pdb", "'goto', lineno");
				return;
			}
			// ��������һ��������ȡ�кŻ�ϵ���
			pInterp->SetBreakMode(SCRIPT_BREAK_LINE);
			pInterp->SetBreakLineNo(nBreakLineNo);	// �ϵ��к�
			pInterp->SetBreakSignal(TRUE);	// ����ű�
			pInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
		}else
		if(strOption == "STEP")	// ����������
		{	// ���õ�����ģʽ(��ʱ�ڶ�������nBreakLineNo��ʾ����������,1=����,2=����,-1=���������)
			if (nBreakLineNo == 1)
			{
				// ������̵ĵ���
				pInterp->SetBreakMode(SCRIPT_STEP_INTO);
			}else
			if (nBreakLineNo == 2)
			{
				// �ɹ��̷���
				pInterp->SetBreakMode(SCRIPT_STEP_RETURN);
			}else
			{
				// ��������̵ĵ���
				pInterp->SetBreakMode(SCRIPT_STEP_NEXT);
			}
			// ���õ�ǰ��ջ���
			//pInterp->SetBreakLevel();
			pInterp->SetBreakSignal(TRUE);	// ����ű�
			pInterp->SetRunMode(SCRIPT_RUN_DEBUG_STEP);	// ����Ϊ����ģʽ
		}
		return;
	}

	// ���ű���ֹ�ź�,�ж��Ƿ�����ű�����
	if(pInterp->GetStopSignal())
	{
		// ��OwmEdit���͵�ǰ�����������Ϣ
		pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
		pInterp->SetStopSignal(FALSE);
		pInterp->SetRunState(SCRIPT_STATE_STOP);
		Perl_croak(aTHX_ "script terminated at user request.");
		return;
	}

	// �ϵ�����ģʽ
	if(SCRIPT_RUN_DEBUG_GO == pInterp->GetRunMode())
	{
		// ����Ƿ���϶ϵ�����
		if(SCRIPT_BREAK_LINE == pInterp->GetBreakMode())	// �жϵ�
		{
			if(nBreakLineNo != pInterp->GetBreakLineNo())
			{
				// �жϵ�����������,������ϵ�
				// ��OwmEdit���͵�ǰ�����������Ϣ
				pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return;
			}
		}else
		if(SCRIPT_BREAK_NAME == pInterp->GetBreakMode())	// �����ϵ�
		{
			if(strBreakName != pInterp->GetBreakName())
			{
				// �����ϵ�����������,������ϵ�
				// ��OwmEdit���͵�ǰ�����������Ϣ
				pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return;
			}
		}else
		{	// ��ͨ�ϵ�
			CUIntArray auBreakList;
			pInterp->GetBreakList("", auBreakList);
			BOOL bFind = FALSE;
			for(int i=0; i<auBreakList.GetSize(); i++)
			{
				if((UINT)nBreakLineNo == auBreakList[i])
				{
					bFind = TRUE;
					break;
				}
			}
			if(!bFind)
			{	// ��ͨ�ϵ�����������,������ϵ�
				// ��OwmEdit���͵�ǰ�����������Ϣ
				pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return;
			}
		}
	}else
	if(SCRIPT_RUN_DEBUG_STEP == pInterp->GetRunMode())	// ����
	{
		int nLevel = pInterp->GetCurStackLevel();
		int nOldLevel = pInterp->GetStackLevel();
		if(SCRIPT_STEP_NEXT == pInterp->GetBreakMode())
		{	// �������������
			if(nLevel > nOldLevel)
			{
				return;
			}
		}else
		if(SCRIPT_STEP_RETURN == pInterp->GetBreakMode())
		{	// �ɹ��̷���
			if(nLevel >= nOldLevel && nOldLevel != -1)
			{
				return;
			}
		}else
		if(SCRIPT_STEP_INTO == pInterp->GetBreakMode())
		{	// �����������
		}
	}

	// �����ջ��Ȳ���,������һ�εıȽ�
	pInterp->SetStackLevel(pInterp->GetCurStackLevel());

	if(pDebugFile && pIPlatUI)
	{
		pIPlatUI->OpenDocumentFile(strBreakFile, "");
		// ��OwmEdit������������ģʽΪ�ӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)strBreakFile, 1, NULL);
	}

	// ��Edit���Ͷϵ��к���Ϣ
	pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, nBreakLineNo, strBreakFile);

	// ���õ�ǰ״̬Ϊ�ж�״̬
	pInterp->SetRunState(SCRIPT_STATE_BREAK);

	MSG msg;
	while(TRUE)
	{
		//if(::PeekMessage(&msg, NULL, NULL, NULL,PM_REMOVE))
		// ʹ��GetMessage��PeekMessage��,�������ϵͳ��ӳ���������
		if(::GetMessage(&msg, NULL, NULL, NULL))
		{															
			::TranslateMessage(&msg);										
			::DispatchMessage(&msg);
		}

		// �ٴμ��ű���ֹ�ź�,�ж��Ƿ�����ű�����
		if(pInterp->GetStopSignal())
		{
			// ��OwmEdit���͵�ǰ�����������Ϣ
			pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
			pInterp->SetStopSignal(FALSE);
			pInterp->SetBreakSignal(FALSE);
			pInterp->SetRunState(SCRIPT_STATE_STOP);
			Perl_croak(aTHX_ "script terminated at user request.");
			return;
		}

		// ���ϵ㼤���ź�
		if(pInterp->GetBreakSignal())
		{
			pInterp->SetBreakSignal(FALSE);
			break;
		}
	}

	// ���õ�ǰ״̬Ϊ����״̬
	pInterp->SetRunState(SCRIPT_STATE_RUN);

	// ��Edit���͵�ǰ�����������Ϣ
	pInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);

	////////////////////////////////////////////////////////
    }
    XSRETURN(1);
}

EXTERN_C void boot_DynaLoader(pTHXo_ CV* cv);

// ��ǶPerl������ʱ������ⲿXSUB�����ĳ�ʼ������
EXTERN_C void
xs_init(pTHXo)
{
	UNUSED_ALWAYS(my_perl); // compiler warning level 4... argh !

	char *file = __FILE__;
	dXSUB_SYS;

	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
	// ��ʼ��ƽ̨��׼������غ���
	newXS("platform_print", platform_Print, file);
	// ��ʼ��ƽ̨������չ����
	newXS("pdb", platform_debug, file);
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ���ļ����Ե��ļ���
//////////////////////////////////////////////////////////////////////
CDebugFile::CDebugFile()
{
	m_nFileIndex	= 0;
	m_strFile		= "";
	m_pRunBuffer	= NULL;
	m_nTDLineCount	= 0;
	m_ProfileMap.InitHashTable(128);	// ��ʼ�������ʷ�����ϣ��
}

CDebugFile::~CDebugFile()
{
	if(m_pRunBuffer != NULL)
		delete m_pRunBuffer;
	m_pRunBuffer = NULL;

	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();
}

CDebugFile::CDebugFile(const CDebugFile& other)
{
	m_nFileIndex	= other.m_nFileIndex;
	m_strFile		= other.m_strFile;
	m_pRunBuffer	= other.m_pRunBuffer;
	m_nTDLineCount	= other.m_nTDLineCount;
}

//////////////////////////////////////////////////////////////////////
// ���ظ�ֵ������
//////////////////////////////////////////////////////////////////////
CDebugFile& CDebugFile::operator = (const CDebugFile& other)
{
	m_nFileIndex	= other.m_nFileIndex;
	m_strFile		= other.m_strFile;
	m_pRunBuffer	= other.m_pRunBuffer;
	m_nTDLineCount	= other.m_nTDLineCount;

	return *this;
};

/////////////////////////////////////////////////////////////////////////////
// ��������ģʽ,���л���������
/////////////////////////////////////////////////////////////////////////////
int CDebugFile::ProcessRunBuffer()
{
	if(m_pRunBuffer != NULL)
		delete m_pRunBuffer;
	m_pRunBuffer = NULL;

	// ��ո����ʷ�������
	m_nTDLineCount = 0;
	m_ProfileMap.RemoveAll();

	try
	{

	}catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ִ�л������е�Perl�ű�
/////////////////////////////////////////////////////////////////////////////
int CDebugFile::RunScriptBuffer(PerlInterpreter*	pInterp)
{
	if(NULL == m_pRunBuffer)
		return FALSE;

	SET_CONTEXT;

	eval_pv(m_pRunBuffer, FALSE);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CPerlInterp::CPerlInterp()
{
	m_nInterpID		= g_InterpID++;
	if(g_InterpID > 0x01000000)
		g_InterpID  = 0x100;
	pIPlatUI		= NULL;
	m_dwRunThreadId	= NULL;
	m_strInterpName	= _T("");
	m_strScriptFile	= _T("");
	m_pRunBuffer	= NULL;
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

	// ����������
	PERL_SYS_INIT3(0, NULL, NULL);
	pInterp = perl_alloc();
	if(pInterp)
	{
		SET_CONTEXT;

		PL_perl_destruct_level = 1;	// ensure proper and complete cleanup when closing Perl
		perl_construct(pInterp);

		// ����PERL5LIB��������Ϊ��ǰʹ�õ�Perl�汾��lib·��,����Ĭ�ϻᰴ��binĿ¼ƽ����libĿ¼������
		CString strPerlLibEnv;
		strPerlLibEnv.Format("PERL5LIB=%s\\lib", GetInterpRootPath() + PERL_LIB_PATH);
		putenv(strPerlLibEnv);

		//////////////////////////////////////////////////////////////////
		// NOTE: if you get an assert from below, that's because
		// you missed something setting up this class in your project -
		CString strInitScript = GetInterpRootPath();
		//strInitScript += "bin\\perlsistent.pl";
		strInitScript += "bin\\perlinit.pl";
		CString strScript;
		if (!PXReadFile(strInitScript, strScript))
		{
			strScript = "BEGIN{ $SIG{__WARN__} = sub { $__w_warnings .= $_[0] } }";
		}

		DWORD dwSize = strScript.GetLength();
		LPSTR buffer = new CHAR[dwSize+8];
		memcpy(buffer, strScript, dwSize);
		buffer[dwSize] = 0;

		// Much of this is straight out of perlembed. We are setting up an independent
		// Perl instance in its own thread. The 'BEGIN' line is to allow warning messages
		// that are normally sent to STDERR (and therefore lost in this environment) to be
		// stored in the $__w_warnings variable. This gives the user half a chance to debug
		// a script. The getWarnings() and clearWarnings() methods are used to get to this
		// variable
		//char *embedding[] = {"", "-e", "BEGIN{ $SIG{__WARN__} = sub { $__w_warnings .= $_[0] } }" };

		char *embedding[] = { "", "-e", buffer };
		int ret = perl_parse(pInterp, xs_init, 3, //sizeof(*embedding) - 1, 
				embedding, NULL);
		if(ret == 0)
		{
			PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
			ret = perl_run(pInterp);
			/*if(ret == 0)
			{
				// Make sure it's all right
				char *args[] = { NULL };
				int nCount = call_argv("test", G_EVAL | G_SCALAR, args);
				if (nCount != 1 || SvTRUE(ERRSV))
				{
					ret = -1;
				}
			}*/
		}
		if(ret != 0)
		{
			CString strErr("Error while running persistent interpreter (perlinit.pl) ");
			if (SvTRUE(ERRSV))
			{
				strErr += ":\n\n";
				strErr += (LPCSTR)SvPV_nolen(ERRSV);
			}
			else
			{
				strErr += ".";
			}

			AfxMessageBox(strErr);

			perl_destruct(pInterp);
			perl_free(pInterp);
			pInterp = NULL;
		}

	}

	m_last_eval = (SV *)&PL_sv_undef;	// saved result of last eval() (from doScript())
}

CPerlInterp::~CPerlInterp()
{
	if(m_pRunBuffer != NULL)
		delete m_pRunBuffer;
	m_pRunBuffer = NULL;

	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();

	try {
	//////////////////////////////////////////////
	if(pInterp != NULL)
	{
		SET_CONTEXT;
		PL_perl_destruct_level = 0;
		perl_destruct(pInterp);
		perl_free(pInterp);
		pInterp = NULL;
		//PERL_SYS_TERM();
	}
	//////////////////////////////////////////////
	}catch(...)
	{
	}

	// ɾ�������������б��е���
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CPerlInterp* pInterp = (CPerlInterp*)g_InterpList.GetAt(pos);
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
int CPerlInterp::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
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
int CPerlInterp::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
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

//{{VCI_IMPLEMENT_BEGIN
int __stdcall CPerlInterp::SetIPlatUI(LPVOID lpIPlatUI)
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
int __stdcall CPerlInterp::SetLibPath(LPCTSTR lpszPath)
{
	// TODO: Add your code here

	return 0;
}

// �������Ƿ�������
int __stdcall CPerlInterp::IsInterpRunning()
{
	// TODO: Add your code here
	// �ж����н������Ƿ��Ѿ���ֹ
	int count = g_InterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_InterpList.FindIndex(i);
		CPerlInterp* pInterp = (CPerlInterp*)g_InterpList.GetAt(pos);
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
int CPerlInterp::FindDebugFile(CString strFileName)
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
int CPerlInterp::AddDebugFile(CString strFileName)
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
BOOL CPerlInterp::ClearDebugFile()
{
	m_asDebugScript.RemoveAll();
	return TRUE;
}

// ���ýű��ļ���
int __stdcall CPerlInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	m_strScriptFile = lpszPathName;

	return TRUE;
}

// ��ȡ�ű��ļ���
CString __stdcall CPerlInterp::GetScriptFile()
{
	// TODO: Add your code here

	return m_strScriptFile;
}

// ���н��̲��ҽ��Զ�������ܵ�
BOOL CPerlInterp::RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId, BOOL bActiveOutput)
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
BOOL CPerlInterp::TerminatePipeProcess()
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
BOOL CPerlInterp::ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd)
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
/*
// that's tricky void *m_pMyPerl, huh?
bool CPerlInterp::PXPerlCallRetString(LPCSTR szFunc, CString &strRet)
{
	if (IS_LOADED)
	{
		SET_CONTEXT;

		dSP;
		bool ret = false;
		int count;
		ENTER ;
		SAVETMPS;
		PUSHMARK(SP);
		PUTBACK;
		count = call_pv(szFunc, G_EVAL | G_SCALAR);
		SPAGAIN;

		if (count != 1)
			AfxMessageBox((LPCSTR)SvPV_nolen(ERRSV));
		else if (SvTRUE(ERRSV))
		{
			AfxMessageBox((LPCSTR)SvPV_nolen(ERRSV));
			POPs;
		}
		else
		{
			strRet = POPp;//(LPCSTR)SvPV_nolen(POPs);
			ret = true;
		}

		PUTBACK;
		FREETMPS;
		LEAVE;

		return ret;
	}
	return false;
}

bool CPerlInterp::PXPerlCall(LPCSTR szFunc)
{
	if (IS_LOADED)
	{
		char *args[] = { NULL };

		SET_CONTEXT;
		call_argv(szFunc, G_EVAL | G_DISCARD, args);

		if (SvTRUE(ERRSV))
		{
			AfxMessageBox((LPCSTR)SvPV_nolen(ERRSV));
			return false;
		}
		return true;
	}
	return false;
}
*/

/////////////////////////////////////////////////////////////////////////////
// ��������ģʽ,���л���������
/////////////////////////////////////////////////////////////////////////////
int CPerlInterp::ProcessRunBuffer()
{
	if(SCRIPT_RUN_NORMAL == m_nRunMode)
		return TRUE;

	if(m_pRunBuffer != NULL)
		delete m_pRunBuffer;
	m_pRunBuffer = NULL;

	// ��ո����ʷ�������
	m_nTDLineCount = 0;
	m_ProfileMap.RemoveAll();

	try
	{
		CStdioFile fi(m_strScriptFile, CFile::modeRead);
		int nLineNo = 0;		// �к�
		int nBufSize = 0;		// ��������С
		BOOL bEndLine1 = TRUE;	// �Ƿ�����'\'
		BOOL bEndLine2 = TRUE;	// �Ƿ�����'"'
		BOOL bEndCommand = TRUE;	// �Ƿ�������(��;��β)
		BOOL bPodBlock = FALSE;	// �Ƿ���Perl POD����
		BOOL bComment = FALSE;	// ע��
		int  nVarLevel = 0;		// ���б����е����
		BOOL bMultiBracketLine = FALSE;	// ���е��������(��(���еı���)
		int  nBracketLevel = 0;		// ��������������������
		CString strLine;
		CStringArray asScript;	// ������

		while(fi.ReadString(strLine))
		{
			nLineNo++;
			CString strLineSrc = strLine;
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ");

			// �жϲ�ȥ����β��ע��
			int nPos_ = strLine.ReverseFind('#');
			if(nPos_ > 0)
			{
				CString strTemp1 = strLine.Left(nPos_);
				strTemp1.TrimRight("\t ");
				int nLen = strTemp1.GetLength();
				if((nLen > 0) && (strTemp1[nLen-1]==';'))
				{
					strLine = strTemp1;
				}
			}

			if(strLine.GetLength() == 0)	// ����
			{
				bEndLine1 = TRUE;
				asScript.Add(strLineSrc);
				nBufSize += strLineSrc.GetLength();
				nBufSize += 1;	// "\n"
				continue;
			}else
			if((strLine[0] == '#') && bEndLine2)	// ע����(��Ҫ�ų���"���е����)
			{
				bEndLine1 = TRUE;
				bEndLine2 = TRUE;
				bComment = TRUE;
				int nLen = strLine.GetLength();
				if( (strLine[nLen-1]=='\\') && ((nLen>1) && (strLine[nLen-2]!='\\')) )
				{	// ����
					bEndLine1 = FALSE;
				}
				asScript.Add(strLineSrc);
				nBufSize += strLineSrc.GetLength();
				nBufSize += 1;	// "\n"
				continue;
			}else
			if(bComment && !bEndLine1)
			{
				asScript.Add(strLineSrc);
				nBufSize += strLineSrc.GetLength();
				nBufSize += 1;	// "\n"
				int nLen = strLine.GetLength();
				if( (strLine[nLen-1]=='\\') && ((nLen>1) && (strLine[nLen-2]!='\\')) )
				{
					continue;
				}
				bEndLine1 = TRUE;	// ��������
				continue;
			}

			bComment = FALSE;

			if(!bPodBlock)	// �ж��Ƿ�POD����
			{
				if((strLine.Find("=head1") == 0) || (strLine.Find("=head2") == 0)
				|| (strLine.Find("=over") == 0) || (strLine.Find("=item") == 0)
				|| (strLine.Find("=back") == 0) || (strLine.Find("=for") == 0)
				|| (strLine.Find("=begin") == 0) || (strLine.Find("=end") == 0)
				|| (strLine.Find("=pod") == 0) || (strLine.Find("=cut") == 0) )
				{
					asScript.Add(strLineSrc);
					nBufSize += strLineSrc.GetLength();
					nBufSize += 1;	// "\n"
					bPodBlock = TRUE;
					continue;
				}
			}
	
			if(!bMultiBracketLine && !bPodBlock)	// �ж��Ƿ���������������ʼ��
			{
				nBracketLevel = 0;
				int nPos = strLine.Find("(");
				if((nPos != -1) && ((strLine.Find("\"") == -1) || (strLine.Find("\"") > nPos)) )
				{
					nBracketLevel++;
					CString strLineTmp = strLine;
					strLineTmp.Delete(0, nPos+1);
					BOOL bStringState = FALSE;
					for(int i=0; i<strLineTmp.GetLength(); i++)
					{
						if(!bStringState)
						{
							if(strLineTmp[i] == '(')
							{
								nBracketLevel++;
							}else
							if(strLineTmp[i] == ')')
							{
								nBracketLevel--;
							}else
							if((strLineTmp[i] == '\"') || (strLineTmp[i] == '\''))
							{
								bStringState = TRUE;
							}
						}else
						{
							if((strLineTmp[i] == '\"') || (strLineTmp[i] == '\''))
							{
								bStringState = FALSE;
							}
						}
					}
					if(nBracketLevel > 0)
					{
						bMultiBracketLine = TRUE;

						CString strBreak;
						strBreak.Format("main::pdb('', %d);", nLineNo);
						strLineSrc = strBreak + strLineSrc;
						asScript.Add(strLineSrc);
						nBufSize += strLineSrc.GetLength();
						nBufSize += 1;	// "\n"
						m_nTDLineCount ++;
						continue;
					}
				}
			}

			if( bEndCommand && (strLine[strLine.GetLength()-1] != ';')
				&& (strLine[strLine.GetLength()-1] != '{') && (strLine[strLine.GetLength()-1] != '}') )	// �ж��Ƿ��н���(��;��β��{��β)
			{
				bEndCommand = FALSE;
			}

			if(bEndLine1 && bEndLine2 && bEndCommand && !bPodBlock && /*!bVarMultiLine &&*/ !bMultiBracketLine)	// �����������(������switch������,�Ҳ����ڶ��б�������״̬),��ӵ�����־
			{
				CString strBreak;
				strBreak.Format("main::pdb('', %d);", nLineNo);
				strLineSrc = strBreak + strLineSrc;
				asScript.Add(strLineSrc);
				nBufSize += strLineSrc.GetLength();
				nBufSize += 1;	// "\n"
				m_nTDLineCount ++;

				int nLen = strLine.GetLength();
				if( (strLine[nLen-1]=='\\') && ((nLen>1) && (strLine[nLen-2]!='\\')) )
				{	// ����'\'
					bEndLine1 = FALSE;
					continue;
				}
				/*if((strLine[nLen-1]=='"') && (strLine.Find('"')==(nLen-1)))
				{	// ����'"'
					bEndLine2 = FALSE;
					continue;
				}*/
				// ����"���е����,��Ҫ����"�ĸ�����������״̬
				int nPos = -1;
				while((nPos = strLine.Find('"')) != -1)
				{
					if((nPos == 0) || (strLine[nPos-1] != '\\'))
					{
						bEndLine2 = !bEndLine2;
					}
					strLine.Delete(0, nPos+1);
				}
				continue;
			}

			asScript.Add(strLineSrc);
			nBufSize += strLineSrc.GetLength();
			nBufSize += 1;	// "\n"

			if(!bEndLine1)
			{
				int nLen = strLine.GetLength();
				if( (strLine[nLen-1]=='\\') && ((nLen>1) && (strLine[nLen-2]!='\\')) )
				{
					continue;
				}
				bEndLine1 = TRUE;	// ��������
			}

			if(!bEndLine2)
			{
				if(strLine.Find('"') == -1)
				{
					continue;
				}
				//bEndLine2 = TRUE;	// ��������
				int nPos = -1;
				while((nPos = strLine.Find('"')) != -1)
				{
					if((nPos == 0) || (strLine[nPos-1] != '\\'))
					{
						bEndLine2 = !bEndLine2;
					}
					strLine.Delete(0, nPos+1);
				}
			}

			if(!bEndCommand && (strLine[strLine.GetLength()-1] == ';'))	// �ж��Ƿ�ָ��н���״̬
			{
				bEndCommand = TRUE;
			}

			if(bPodBlock)
			{
				// ��ǰ����POD������״̬,�ж�ʲôʱ���������״̬
				if(strLine.Find("=cut") == 0)
				{
					bPodBlock = FALSE;
				}
			}

			if(bMultiBracketLine)
			{
				// ��ǰ���ڶ����������������״̬,�ж�ʲôʱ���������״̬
				BOOL bStringState = FALSE;
				for(int i=0; i<strLine.GetLength(); i++)
				{
					if(!bStringState)
					{
						if(strLine[i] == '(')
						{
							nBracketLevel++;
						}else
						if(strLine[i] == ')')
						{
							nBracketLevel--;
						}else
						if((strLine[i] == '\"') || (strLine[i] == '\''))
						{
							bStringState = TRUE;
						}
					}else
					{
						if((strLine[i] == '\"') || (strLine[i] == '\''))
						{
							bStringState = FALSE;
						}
					}
				}

				if(nBracketLevel <= 0)
				{
					bMultiBracketLine = FALSE;
				}
			}
		}

		// ����������
		m_pRunBuffer = new char[nBufSize+1];
		m_pRunBuffer[nBufSize] = 0;
		if(nBufSize > 0)
		{	// ��֤�������������ַ����������
			m_pRunBuffer[nBufSize-2] = 0;
			m_pRunBuffer[nBufSize-1] = 0;
		}
		int nPos = 0;
		for(int i=0; i<asScript.GetSize(); i++)
		{
			CString strLine = asScript[i];
			//TRACE("%s\n", strLine);
			//pIPlatUI->Output(2, strLine);
			//pIPlatUI->Output(2, "\r\n");
			memcpy(m_pRunBuffer+nPos, strLine.GetBuffer(strLine.GetLength()), strLine.GetLength());
			strLine.ReleaseBuffer();
			nPos += strLine.GetLength();
			if(i < (asScript.GetSize()-1))
			{				
				memcpy(m_pRunBuffer+nPos, "\n", 1);
				nPos +=1;
			}
		}

	}catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ִ�л������е�Perl�ű�
/////////////////////////////////////////////////////////////////////////////
int CPerlInterp::RunScriptBuffer()
{
	if(NULL == m_pRunBuffer)
		return FALSE;

	m_bProfileEnable = AfxGetApp()->GetProfileInt(REG_PERL_DEBUG_SUBKEY, REG_PERL_ENABLE_PROFILE, FALSE);

	m_nRunState = SCRIPT_STATE_RUN;

	SET_CONTEXT;

	eval_pv(m_pRunBuffer, FALSE);

	// �����debug�ļ�
	for(int i=0; i<m_aDebugFile.GetSize(); i++)
	{
		CDebugFile* pDebugFile = &(m_aDebugFile[i]);
		CString strFileName = pDebugFile->m_strFile;
		// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
		if(pIPlatUI)
		{
			pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG, (WPARAM)(LPTSTR)(LPCTSTR)strFileName, -1, NULL);
		}
		// ������ļ��ĵ��Լ�ͷ
		DebugCallback(IDB_USER_SETCURRENTLINE, 0, strFileName);
	}
	m_aDebugFile.RemoveAll();

	m_nRunState = SCRIPT_STATE_END;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���öϵ㼤���ź�
/////////////////////////////////////////////////////////////////////////////
void CPerlInterp::SetBreakSignal(BOOL bBreak)
{
	m_bBreakSignal = bBreak;	// ���öϵ㼤���־
	// ����Ϣ������Ϣѭ��
	// ע��m_dwRunThreadId�Ǳ���Ľű�����ʱ���߳�ID������������߳���
	// ���ô˺���,�ͻ�ͨ���߳���Ϣȥ���������߳��е�td����
	if(m_dwRunThreadId)
	{
		::PostThreadMessage(m_dwRunThreadId, WM_USER, 0, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���õ��Իص�����
/////////////////////////////////////////////////////////////////////////////
void CPerlInterp::DebugCallback(int nEvent, int nLine, CString strFile)
{
	if(!m_lpfnDebug)
	{
		return;
	}

	if(strFile == "")
	{
		m_lpfnDebug(nEvent, GetScriptFile(), nLine);
	}else
	{
		m_lpfnDebug(nEvent, strFile, nLine);
	}
}

// ���нű��ļ�
int __stdcall CPerlInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	if(lpszPathName != NULL)
	{
		if(!SetScriptFile(lpszPathName))
			return FALSE;
	}

	m_bMultiDebugKeepSelect = FALSE;
	m_bMultiDebugLoadSource = FALSE;

	m_dwRunThreadId = ::GetCurrentThreadId();

	if(SCRIPT_RUN_NORMAL != m_nRunMode)
	{
		if(!ProcessRunBuffer())
		{
			return FALSE;
		}
		return RunScriptBuffer();
	}

	m_nRunState = SCRIPT_STATE_RUN;

	int nRet;

	CString strScript;
	if (!PXReadFile(m_strScriptFile, strScript))
		return FALSE;
	nRet = RunScriptCommand(strScript);

	m_nRunState = SCRIPT_STATE_END;

	m_dwRunThreadId = NULL;
	//m_nEvalResult	= TCL_OK;
	return nRet;
}

// ִ�нű�����
int __stdcall CPerlInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	// TODO: Add your code here
	//CString strCmd = lpszTclCmd;
	//char *args[] = { "main", strCmd.GetBuffer(0), "1", NULL };

	SET_CONTEXT;
	//perl_parse(pInterp, NULL, 3, args, (char **)NULL);
	//strCmd.ReleaseBuffer();

	m_strResult = "";

	//����ǵ���ģʽ��������ģʽ������ô���������¼�
	if(m_nRunMode != SCRIPT_RUN_NORMAL)
	{
		CString strEvalCmd = lpszTclCmd;

		/*
		// ��������
		CString strDebugCmd = "\n\
			require 'perl5db.pl';\n\
			package DB;\n\
			sub DB {};\n\
			sub sub {main::platform_print('stdout', \" $sub\\n\"); &$sub};\n\
			package main;\n\
			";
		eval_pv(strDebugCmd, FALSE);
		*/
		//strEvalCmd = "DB::eval {\n" + strEvalCmd + "\n}";

		try{
		m_last_eval = eval_pv(strEvalCmd, FALSE);	// eval(p) and don't abort if 'p' throws an exception
		//m_str_eval_error = SvPV_nolen(get_sv("@", FALSE));	// Get any error/exception message from eval()
		}catch(...)
		{
		}

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
		}
	}else
	{
		try{
		m_last_eval = eval_pv(lpszTclCmd, FALSE);	// eval(p) and don't abort if 'p' throws an exception
		//m_str_eval_error = SvPV_nolen(get_sv("@", FALSE));	// Get any error/exception message from eval()
		}catch(...)
		{
		}
	}

	// �ж��﷨����
	if (SvTRUE(ERRSV))
	{
		//AfxMessageBox((LPCSTR)SvPV_nolen(ERRSV));
		return FALSE;
	}

	//m_nEvalResult = TCL_OK;
	return TRUE;
}

// ֹͣ�ű�����
int __stdcall CPerlInterp::StopScript()
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
LPVOID __stdcall CPerlInterp::GetInterp()
{
	// TODO: Add your code here

	return pInterp;
}

// ��ȡ������ID
int __stdcall CPerlInterp::GetInterpID()
{
	// TODO: Add your code here

	return m_nInterpID;
}

// ���ý�������
void __stdcall CPerlInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	// TODO: Add your code here
	m_strInterpName = lpszInterptName;
}

// ��ȡ��������
CString __stdcall CPerlInterp::GetInterpName()
{
	// TODO: Add your code here

	return m_strInterpName;
}

// ���������к�
int CPerlInterp::ParseErrorLineNo(CString strError)
{
	// xxx at (eval xxx) line xxx
	int nPos = strError.Find("at (eval ");
	if(nPos != -1)
	{
		strError.Delete(0, nPos + 9);
		strError.TrimLeft();
		nPos = strError.Find(") line ");
		if(nPos != -1)
		{
			strError.Delete(0, nPos + 7);
			strError.TrimRight(".\n");
			return atoi(strError);
		}
	}

	return 0;
}

// ��ȡ�����к�
int __stdcall CPerlInterp::GetErrorLine()
{
	// TODO: Add your code here
	// �﷨����
	if (SvTRUE(ERRSV))
	{
		return ParseErrorLineNo((LPCSTR)SvPV_nolen(ERRSV));
	}

	return 0;
}
/*
bool CPerlInterp::GetSTDERR(CString &strContents)
{
//	if (!PXPerlCallRetString("get_stderr", strContents))
//		return false;
	return (strContents.IsEmpty()==0);
}

bool CPerlInterp::GetSTDOUT(CString &strContents)
{
//	if (!PXPerlCallRetString("get_stdout", strContents))
//		return false;
	return (strContents.IsEmpty()==0);
}
*/
// ��ȡ�����Ϣ
CString __stdcall CPerlInterp::GetResult()
{
	// TODO: Add your code here
	// �﷨����
	if (SvTRUE(ERRSV))
	{
		return (LPCSTR)SvPV_nolen(ERRSV);
	}

	// �����ͨ��platform_print("toresult", ...)���صĽ��,������ʹ�ô˽��
	if(m_strResult != "")
	{
		return m_strResult;
	}

	// ���һ�����еĽ����Ϣ
	if(m_last_eval)
	{
		//return (char *)SvPV_nolen((SV*)m_last_eval);
		return getSVAsString(m_last_eval);
	}

	return "";
}

// ��ȡPerl��������(����Perl�����Ͷ���)
int CPerlInterp::getVarType(void* sv)
{
	if(sv != NULL)
	{
		return SvTYPE((SV*)sv);
	}

	return SVt_NULL;
}

// ��һ��Perl����ֵת��Ϊ�ַ�������
CString CPerlInterp::getSVAsString(void* sv)
{
	int nType = getVarType(sv);
	CString strValue = "";
	switch(nType)
	{
	case SVt_IV://strValue.Format("%d", SvIV((SV*)sv));break;	// ����
	case SVt_NV://strValue.Format("%f", SvNV((SV*)sv));break;	// ������
	case SVt_PV:strValue = (char *)SvPV_nolen((SV*)sv);break;	// �ַ�������
	case SVt_PVAV:	// ��������
		{
			strValue = "(";
			int len = av_len((AV*)sv) + 1;		// number of elements in array
			for(int i = 0; i < len; i++)
			{
				SV **val = av_fetch((AV*)sv, i, 0);	// 0 means to not change the value
				if(val)
				{
					int nItemType = getVarType(*val);
					if(SVt_PV == nItemType)
					{
						strValue += "\"";
					}
					strValue += SvPV_nolen(*val);
					if(SVt_PV == nItemType)
					{
						strValue += "\"";
					}
					if(i < (len-1))
					{
						strValue += ", ";
					}
				}
			}
			strValue = ")";
		}
		break;
	case SVt_PVHV:	// ��ϣ����
		{
			strValue = "{";
			// Now we have to get a list of keys
			hv_iterinit((HV*)sv);	// get beginning iterator
			HE *hashEntry;	// pointer to current hashentry
			while((hashEntry = hv_iternext((HV*)sv)) != NULL)
			{
				long keylen;
				char keybuf[10240];	// bigger-hammer-is-better school of software engineering

				// Get the key
				char *keyname = hv_iterkey(hashEntry, &keylen);
				if(keyname)
				{
					strncpy(keybuf,keyname,keylen);
					keybuf[keylen] = 0;

					// Now get the value -- its OK to have an empty value in a hash
					CString val;
					SV *hssv = hv_iterval((HV*)sv, hashEntry);
					if(!hssv)
					{
						val = "";
					}
					else
					{
						val = SvPV_nolen(hssv);
					}

					strValue += "\"";
					strValue += keybuf;
					strValue += "\" => ";

					int nItemType = getVarType(hssv);
					if(SVt_PV == nItemType)
					{
						strValue += "\"";
					}
					strValue += val;
					if(SVt_PV == nItemType)
					{
						strValue += "\"";
					}
					strValue += ", ";
				}
			}
			strValue = "}";
		}
		break;
	}

	return strValue;
}

// ��һ��Perl����ֵת��Ϊ�ַ������鷵��
int CPerlInterp::getVarVal(CString varName, CStringArray& asValue)
{
	CString strValue = "";
	if(varName.Find("@") == 0)	// �������
	{
		varName.Delete(0, 1);
		getArrayVal(varName, asValue);
	}else
	if(varName.Find("\%") == 0)	// ��ϣ����
	{
		varName.Delete(0, 1);
		CMapStringToString map;
		POSITION pos;
		getHashVal(varName, map);
		CString strKey;
		CString strValue;
		for(pos = map.GetStartPosition(); pos != NULL; )
		{
			map.GetNextAssoc(pos, strKey, strValue);
			asValue.Add(strKey + " => " + strValue);
		}
	}else
	if(varName.Find("$") == 0)	// ����
	{
		varName.Delete(0, 1);
		getStringVal(varName, strValue);
		asValue.Add(strValue);
	}

	return asValue.GetSize();
}

BOOL CPerlInterp::setFloatVal(CString varName, double value)
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Create the scalar if it doesn't exist, turn off spurious warnings
	SV *v = get_sv(p, TRUE|GV_ADDMULTI);
	if(!v) return FALSE;

	sv_setnv(v, value);
	return TRUE;
}

BOOL CPerlInterp::getFloatVal(CString varName, double &val)
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread
	SV *v = get_sv(p, FALSE);	// FALSE means don't create if it doesn't already exist
	if(!v) return FALSE;

	val = SvNV(v);
	return TRUE;
}

BOOL CPerlInterp::setIntVal(CString varName, int value) 
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Create the scalar if it doesn't exist, turn off spurious warnings
	SV *v = get_sv(p, TRUE|GV_ADDMULTI);
	if(!v) return FALSE;

	sv_setiv(v, value);
	return TRUE;
}

BOOL CPerlInterp::getIntVal(CString varName, int &val)
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread
	SV *v = get_sv(p, FALSE);	// FALSE means don't create if it doesn't already exist
	if(!v) return FALSE;
	
	val = SvIV(v);
	return TRUE;
}

BOOL CPerlInterp::setStringVal(CString varName, CString value)
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Create the scalar if it doesn't exist, turn off spurious warnings
	SV *v = get_sv(p, TRUE|GV_ADDMULTI);
	if(!v) return FALSE;
	sv_setpv(v, value);
	return TRUE;
}

BOOL CPerlInterp::getStringVal(CString varName, CString &val)
{
	LPCTSTR p = varName;		// $a is "a"
	SET_CONTEXT;	// ensure correct Perl thread
	SV *v = get_sv(p, FALSE);	// FALSE means don't create if it doesn't already exist
	if(!v) return FALSE;
	
	val = SvPV_nolen(v);
	return TRUE;
}

BOOL CPerlInterp::setArrayVal(CString varName, CStringArray &value)
{
	LPCTSTR p = varName;		// @a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Create the array if it doesn't exist, turn off spurious warnings
	AV *v = get_av(p, TRUE|GV_ADDMULTI);
	if(!v) return FALSE;

	av_clear(v);	// clear out all previous values (if any)

	int loop;
	for(loop = 0; loop < value.GetSize(); loop++)
	{
		SV *val = newSVpv(value[loop],0);
		if(!val) return FALSE;
		av_push(v, val);
	}
	return TRUE;
}

BOOL CPerlInterp::getArrayVal(CString varName, CStringArray &values)
{
	LPCTSTR p = varName;		// @a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	AV *v = get_av(p, FALSE);	// don't create if it doesn't exist

	if(!v) return FALSE;

	int len = av_len(v) + 1;		// number of elements in array
	values.RemoveAll();

	int loop;
	for(loop = 0; loop < len; loop++)
	{
		SV **val = av_fetch(v, loop, 0);	// 0 means to not change the value
		if(!val) return FALSE;

		values.Add(SvPV_nolen(*val));
	}

	return TRUE;
}

BOOL CPerlInterp::setHashVal(CString varName, CMapStringToString &value)
{
	LPCTSTR p = varName;		// %a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Create the hash if it doesn't exist, turn off spurious warnings
	HV *v = get_hv(p, TRUE|GV_ADDMULTI);
	if(!v) return FALSE;
	hv_clear(v);	// clear out all previous values (if any)

	CString hkey;
	CString hval;
	POSITION loop = value.GetStartPosition();
	while(loop != NULL)
	{
		value.GetNextAssoc(loop,hkey,hval);
		SV *svval = newSVpv(hval,0);	// make a string SV
		if(!svval) return FALSE;
		hv_store(v,hkey,hkey.GetLength(), svval, 0);
	}
	return TRUE;
}

BOOL CPerlInterp::getHashVal(CString varName, CMapStringToString &value)
{
	LPCTSTR p = varName;		// %a is "a"
	SET_CONTEXT;	// ensure correct Perl thread

	// Get the hash if it exists
	HV *v = get_hv(p, FALSE);
	if(!v) return FALSE;

	// Now we have to get a list of keys
	hv_iterinit(v);	// get beginning iterator
	HE *hashEntry;	// pointer to current hashentry
	while((hashEntry = hv_iternext(v)) != NULL)
	{
		long keylen;
		char keybuf[10240];	// bigger-hammer-is-better school of software engineering

		// Get the key
		char *keyname = hv_iterkey(hashEntry, &keylen);
		if(!keyname) return FALSE;

		strncpy(keybuf,keyname,keylen);
		keybuf[keylen] = 0;

		// Now get the value -- its OK to have an empty value in a hash
		CString val;
		SV *sv = hv_iterval(v, hashEntry);
		if(!sv)
		{
			val = "";
		}
		else
		{
			val = SvPV_nolen(sv);
		}

		value.SetAt(keybuf,val);
	}
	return TRUE;
}

// ���ñ���ֵ
int __stdcall CPerlInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ����ֵ
CString __stdcall CPerlInterp::GetVar(LPCTSTR lpszVarName)
{
	// TODO: Add your code here

	// ��ȡȫ�ֱ���
	CString strCmd = "$__platform_result=''; foreach $__platform_symname (sort keys \%main::) { local *sym = $main::{$__platform_symname}; $__platform_result .= \"\\$$__platform_symname \" if defined $sym; $__platform_result .= \"\\@$__platform_symname \" if @sym; $__platform_result .= \"\\\%$__platform_symname \" if \%sym;}; $__platform_result;";
	SV* vRet = eval_pv(strCmd, FALSE);
	CString strGolbals = SvPV_nolen(vRet);
	CStringArray asGlobals;
	int nPos = -1;
	while((nPos = strGolbals.Find(" ")) != -1)
	{
		asGlobals.Add(strGolbals.Left(nPos));
		strGolbals.Delete(0, nPos+1);
	}
	if(strGolbals.GetLength() > 0)
	{
		asGlobals.Add(strGolbals);
	}

	for(int i=0; i<asGlobals.GetSize(); i++)
	{
		CString strName = asGlobals[i];
		if(strName != lpszVarName)
		{
			continue;
		}
		CString strType = "";
		if(strName.Find("$") == 0)
		{
			strType = "Scalar";
		}else
		if(strName.Find("@") == 0)
		{
			strType = "Array";
		}else
		if(strName.Find("\%") == 0)
		{
			strType = "Hash";
		}

		CString strValue = "";
		CStringArray asValue;
		if((strType == "Array") || (strType == "Hash"))
		{
			//strValue = "{...}";			
			getVarVal(strName, asValue);
			for(int i=0; i<asValue.GetSize(); i++)
			{
				strValue += asValue[i];
				if(i < (asValue.GetSize()-1))
				{
					strValue += " , ";
				}
			}
		}else
		{
			getVarVal(strName, asValue);
			strValue = asValue.GetAt(0);
		}
		return strValue;
	}

	return "";
}

// ע����չ����
int __stdcall CPerlInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	// TODO: Add your code here

	return 0;
}

// ������չ����
int __stdcall CPerlInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��չ����
CString __stdcall CPerlInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	// TODO: Add your code here

	return "";
}

// ��������ģʽ
int __stdcall CPerlInterp::SetRunMode(int nRunMode)
{
	m_nRunMode = nRunMode;
	return true;
}

// ��ȡ����ģʽ
int __stdcall CPerlInterp::GetRunMode()
{
	return m_nRunMode;
}

// ��������״̬
int __stdcall CPerlInterp::SetRunState(int nState)
{
	m_nRunState = nState;
	return true;
}

// ��ȡ����״̬
int __stdcall CPerlInterp::GetRunState()
{
	return m_nRunState;
}

// ��ȡ��ǰ��ջ����
int __stdcall CPerlInterp::GetCurStackLevel()
{
	// TODO: Add your code here
	/* //����1:ѭ������caller(level++),ֱ����ȡ�������Ϊֹ
	CString strTempCmd;
	int nLevel = 0;
	strTempCmd.Format("caller(%d)", nLevel);
	SV* vRet = eval_pv(strTempCmd, FALSE);
	CString strCaller;
	while((strCaller = SvPV_nolen(vRet)) != "")
	{
		nLevel++;
		strTempCmd.Format("caller(%d)", nLevel);
		vRet = eval_pv(strTempCmd, FALSE);
	}
	*/

	// ����2:ֱ��ʹ�ý������еĶ�ջ��ȱ���
	int nLevel = 0;
	if(pInterp)
	{
		nLevel = pInterp->Iscopestack_ix - 1;
	}

	return nLevel;
}

// ��ȡ�ؼ����б�
int __stdcall CPerlInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
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
int __stdcall CPerlInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	CString strParam = lpszParam;

	CString strDebugCmd;
	if(nEvent == IDB_STOP)
	{
		strDebugCmd = "main::pdb('stop')";
	}else
	if(nEvent == IDB_NEXT)
	{
		strDebugCmd = "main::pdb('next')";
	}else
	if(nEvent == IDB_GOTO)
	{
		if(strParam.GetLength() == 0)
		{
			// �жϵ�
			strDebugCmd.Format("main::pdb('goto')", nParam);;
		}else
		{
			// �����ϵ�
		}
	}else
	if(nEvent == IDB_STEPINTO)
	{
		strDebugCmd = "main::pdb('step', 1)";
	}else
	if(nEvent == IDB_SETPOUT)
	{
		strDebugCmd = "main::pdb('step', 2)";
	}else
	if(nEvent == IDB_STEP)
	{
		strDebugCmd = "main::pdb('step')";
	}else
	if(nEvent == IDB_END)
	{
		return TRUE;
	}

	SET_CONTEXT;
	eval_pv(strDebugCmd, FALSE);

	return TRUE;
}

// ���öϵ��б�
int __stdcall CPerlInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CPerlInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CPerlInterp::GetEnableProfileAnalyse()
{
	// TODO: Add your code here
	return m_bProfileEnable;
}

// ��ȡ��Ч��������
int __stdcall CPerlInterp::GetAvailableLineCount()
{
	// TODO: Add your code here
	return m_nTDLineCount;
}

// ��ȡ�������и��Ǳ�
LPVOID __stdcall CPerlInterp::GetProfileMap()
{
	// TODO: Add your code here
	return (void*)(&m_ProfileMap);
}

// �ж�ָ�������Ƿ�֧��
int __stdcall CPerlInterp::MethodIsSupport(LPCTSTR lpszMethod)
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
int __stdcall CPerlInterp::InitialDebugMode(CString& strResult)
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
	//pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColName + ",100");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColSource + ",160");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColLine + ",50");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColWhat + ",80,AutoSize");
	//pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColType + ",60,AutoSize");
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

// ��ȡָ��������ϣ����������·����
CString get_var_fullname(CPerlInterp* pInterp, CString strhItem)
{
	CString strArrayHashName = "";
	CString strTmp;
	CString strhParent = "";
	while((strhParent = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ItemParent " + strhItem)) != "ROOT")
	{
		strTmp.Format("<#%s,2>", strhItem);
		CString strItemType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
		if((strItemType == "Array") || (strItemType == "Hash"))
		{
			strTmp.Format("<#%s,0>", strhItem);
			strArrayHashName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp) + strArrayHashName;
		}
		strhItem = strhParent;
	}

	strTmp.Format("<#%s,2>", strhItem);
	CString strItemType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	if((strItemType == "Array") || (strItemType == "Hash"))
	{
		strTmp.Format("<#%s,0>", strhItem);
		strArrayHashName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp) + strArrayHashName;
	}

	return strArrayHashName;
}

// ��������˫������
void callback_perl_ldclick(CPerlInterp* pInterp, LPCTSTR lpszInfo)
{
	TRACE("callback info=%s\n", lpszInfo);
	CString strInfo = lpszInfo;

	if(pInterp == NULL || !CheckPerlInterpExist(pInterp))
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

	if((strVarType == "Array") || (strVarType == "Hash"))
	{
		// �����������ϣ,��չ��
		CString strhItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "SelItem");

		int nChildCount = atoi(pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "ChildCount " + strhItem));
		if(nChildCount == 0)
		{
			CString strTmp;

			CStringArray asValue;
			int nValSize = pInterp->getVarVal(strVarName, asValue);
			for(int i=0; i<nValSize; i++)
			{
				if(strVarType == "Hash")
				{
					CString strHashKey = "";
					CString strHashVal = asValue[i];
					int nPos = asValue[i].Find(" => ");
					if(nPos != -1)
					{
						strHashKey = asValue[i].Left(nPos);
						strHashVal.Delete(0, nPos + 4);
					}
					strTmp.Format("<%s,7>%s%s%s%s%s", strhItem,
							strHashKey, SEPATATOE,
							strHashVal, SEPATATOE,
							"Item");
				}else
				{
					strTmp.Format("<%s,7>[%d]%s%s%s%s", strhItem,
							i, SEPATATOE,
							asValue[i], SEPATATOE,
							"Item");
				}
				pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, strTmp);

				// ����˫��������
				CString strlasthItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "LastItem");
				pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, "<I>Set Script<#" + strlasthItem + "> <1>");
			}
			pInterp->pIPlatUI->Output(pInterp->m_nOutputVar, "<T>ExpandItem<#" + strhItem + ">");
		}
	}else
	{
		DLL_LOADSTRING(strWatchVarTitle, IDS_WATCH_VAR_TITLE);
		DLL_LOADSTRING(strWatchVarName, IDS_WATCH_VAR_NAME);
		DLL_LOADSTRING(strWatchVarValue, IDS_WATCH_VAR_VALUE);
		DLL_LOADSTRING(strWatchVarType, IDS_WATCH_VAR_TYPE);
		CString strhItem = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, "SelItem");
		if((strVarName.Find("[") == 0) || (strVarType == "Scalar"))	// ��������Ԫ���Ѿ�����[]	
		{
			strVarName = get_var_fullname(pInterp, strhItem) + strVarName;
		}else
		{
			strVarName = get_var_fullname(pInterp, strhItem) + "[" + strVarName + "]";
		}
		CString strMsg;
		strMsg.Format("%s:%s\n%s:%s\n%s:%s", strWatchVarName, strVarName, strWatchVarValue, strVarValue, strWatchVarType, strVarType);
		::MessageBox(AfxGetMainWnd()->m_hWnd, strMsg, strWatchVarTitle, MB_OK | MB_ICONINFORMATION);
	}
}

// ˢ�±�������
int CPerlInterp::RefreshVarWindow()
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
/*
	for(int i=0; i<nSize; i++)
	{

		strOutput =	strName + SEPATATOE + strValue + SEPATATOE + strType;
		item.strLine = strOutput;
		item.nParent = nParent;
		item.nImage = 5;
		item.nExpand = 0;
		item.nClickFunc = 1;
		aItems.Add(item);
	}
*/
	// Globals
	strOutput = "Globals";
	item.strLine = strOutput;
	item.nParent = -1;
	item.nImage = 4;
	item.nExpand = 1;
	item.nClickFunc = 0;
	aItems.Add(item);
	nParent = aItems.GetSize()-1;

	CString strCmd = "$__platform_result=''; foreach $__platform_symname (sort keys \%main::) { local *sym = $main::{$__platform_symname}; $__platform_result .= \"\\$$__platform_symname \" if defined $sym; $__platform_result .= \"\\@$__platform_symname \" if @sym; $__platform_result .= \"\\\%$__platform_symname \" if \%sym;}; $__platform_result;";
	SV* vRet = eval_pv(strCmd, FALSE);
	CString strGolbals = SvPV_nolen(vRet);
	CStringArray asGlobals;
	int nPos = -1;
	while((nPos = strGolbals.Find(" ")) != -1)
	{
		asGlobals.Add(strGolbals.Left(nPos));
		strGolbals.Delete(0, nPos+1);
	}
	if(strGolbals.GetLength() > 0)
	{
		asGlobals.Add(strGolbals);
	}

	for(int i=0; i<asGlobals.GetSize(); i++)
	{
		// ȥ��%main����
		if(asGlobals[i] == "%main::")
		{
			continue;
		}
		// ��$_<��ͷ�ı���������ʾ
		if(asGlobals[i].Find("$_<") == 0)
		{
			continue;
		}
		// ȥ��ƽ̨�ڲ�����
		if(asGlobals[i].Find("$__platform_") == 0)
		{
			continue;
		}
		// ȥ���������������ַ��ı�����
		if((asGlobals[i].Find("E_TRIE_MAXBUF") != -1) || (asGlobals[i].Find("ARNING_BITS") != -1))
		{
			continue;
		}

		CString strName = asGlobals[i];
		CString strType = "";
		if(strName.Find("$") == 0)
		{
			strType = "Scalar";
		}else
		if(strName.Find("@") == 0)
		{
			strType = "Array";
		}else
		if(strName.Find("\%") == 0)
		{
			strType = "Hash";
		}

		CString strValue = "";
		CStringArray asValue;
		if((strType == "Array") || (strType == "Hash"))
		{
			//strValue = "{...}";			
			getVarVal(strName, asValue);
			for(int i=0; i<asValue.GetSize(); i++)
			{
				strValue += asValue[i];
				if(i < (asValue.GetSize()-1))
				{
					strValue += " , ";
				}
			}
		}else
		{
			getVarVal(strName, asValue);
			strValue = asValue.GetAt(0);
		}
		strOutput =	strName + SEPATATOE + strValue + SEPATATOE + strType;
		item.strLine = strOutput;
		item.nParent = nParent;
		item.nImage = 5;
		item.nExpand = 0;
		item.nClickFunc = 1;
		aItems.Add(item);
	}

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputVar, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_perl_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputVar, strCallback);

	return TRUE;
}

// Perl��ջ������˫������
void callback_perl_stack_ldclick(CPerlInterp* pInterp, LPCTSTR lpszInfo)
{
	TRACE("callback info=%s\n", lpszInfo);
	CString strInfo = lpszInfo;

	if(pInterp == NULL)
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
	CString strStackSource = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputStack, strTmp);
	strTmp.Format("<%d,1>", lpItem);
	int nStackLine = atoi(pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputStack, strTmp));

	if(nStackLine == 0)
	{
		return;
	}

	if(strStackSource.Find("(eval ") == 0)
	{
		strStackSource = pInterp->GetScriptFile();
	}

	if(GetFileAttributes(strStackSource) == 0xFFFFFFFF)
	{
		return;
	}

	pInterp->pIPlatUI->SendOwmCmd("org.owm.edit", OC_OPENFILE, (WPARAM)(LPTSTR)(LPCTSTR)strStackSource, 0, NULL);
	pInterp->pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETCURENTLINE, (WPARAM)(LPTSTR)(LPCTSTR)strStackSource, nStackLine, NULL);
}

// ˢ�¶�ջ����
int CPerlInterp::RefreshStackWindow()
{
	CString strOutput;

	CTTreeItemArray aItems;

	TTREEITEM item;
	item.hItem = NULL;
	item.nParent = -1;
	item.nExpand = 0;
	item.nImage = 27;
	item.nClickFunc = 1;

	int nLevel = GetCurStackLevel()-1;
	for(int i=0; i<nLevel; i++)
	{
		CString strVar;	// ����
		CString strCmd;
		strCmd.Format(_T("(caller(%d))[1]"), i);
		SV* vRet = eval_pv(strCmd, FALSE);
		CString strFileName = SvPV_nolen(vRet);

		strCmd.Format(_T("(caller(%d))[2]"), i);
		vRet = eval_pv(strCmd, FALSE);
		CString strLineNo = SvPV_nolen(vRet);

		strCmd.Format(_T("(caller(%d))[3]"), i);
		vRet = eval_pv(strCmd, FALSE);
		CString strSubName = SvPV_nolen(vRet);

		strOutput = "";

		strOutput += strFileName;
		strOutput += SEPATATOE;
		strOutput += strLineNo;
		strOutput += SEPATATOE;
		strOutput += strSubName;

		item.strLine = strOutput;
		aItems.Add(item);
	}

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputStack, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_perl_stack_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputStack, strCallback);

	return TRUE;
}

// ���µ��Դ�����Ϣ
int __stdcall CPerlInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
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
	}
	if(bWindowProc)
	{
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
int __stdcall CPerlInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
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
int __stdcall CPerlInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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
int __stdcall CPerlInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(PerlInterp)
CONTROL_INTERFACE_IMPLEMENT(PerlInterp)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CPerlInterp::XPerlInterp::GetExtraInfo(LPVOID lpRefData)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
//{{VCI_INTERFACE_IMPLEMENT_BEGIN
// ����ƽ̨UI�ӿ�ָ��
int __stdcall CPerlInterp::XPerlInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetIPlatUI(lpIPlatUI);
}

int __stdcall CPerlInterp::XPerlInterp::SetLibPath(LPCTSTR lpszPath)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetLibPath(lpszPath);
}

int __stdcall CPerlInterp::XPerlInterp::IsInterpRunning()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->IsInterpRunning();
}

int __stdcall CPerlInterp::XPerlInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetScriptFile(lpszPathName);
}

CString __stdcall CPerlInterp::XPerlInterp::GetScriptFile()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetScriptFile();
}

int __stdcall CPerlInterp::XPerlInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->RunScriptFile(lpszPathName);
}

int __stdcall CPerlInterp::XPerlInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->RunScriptCommand(lpszTclCmd);
}

int __stdcall CPerlInterp::XPerlInterp::StopScript()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->StopScript();
}

LPVOID __stdcall CPerlInterp::XPerlInterp::GetInterp()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetInterp();
}

int __stdcall CPerlInterp::XPerlInterp::GetInterpID()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetInterpID();
}

void __stdcall CPerlInterp::XPerlInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	pObj->SetInterpName(lpszInterptName);
}

CString __stdcall CPerlInterp::XPerlInterp::GetInterpName()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetInterpName();
}

int __stdcall CPerlInterp::XPerlInterp::GetErrorLine()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetErrorLine();
}

CString __stdcall CPerlInterp::XPerlInterp::GetResult()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetResult();
}

int __stdcall CPerlInterp::XPerlInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetVar(lpszVarName, lpszVarValue);
}

CString __stdcall CPerlInterp::XPerlInterp::GetVar(LPCTSTR lpszVarName)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetVar(lpszVarName);
}

int __stdcall CPerlInterp::XPerlInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->RegisterCommand(pInterp, lpszCmd, lpFunc);
}

int __stdcall CPerlInterp::XPerlInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetInterpParam(lpszParamName, lpszParamValue);
}

CString __stdcall CPerlInterp::XPerlInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetInterpParam(lpszParamName);
}

int __stdcall CPerlInterp::XPerlInterp::SetRunMode(int nRunMode)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetRunMode(nRunMode);
}

int __stdcall CPerlInterp::XPerlInterp::GetRunMode()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetRunMode();
}

int __stdcall CPerlInterp::XPerlInterp::SetRunState(int nRunState)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetRunState(nRunState);
}

int __stdcall CPerlInterp::XPerlInterp::GetRunState()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetRunState();
}

int __stdcall CPerlInterp::XPerlInterp::GetCurStackLevel()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetCurStackLevel();
}

// ��ȡ�ؼ����б�
int __stdcall CPerlInterp::XPerlInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetKeyWordList(lpszType, asKeyWord);
}

int __stdcall CPerlInterp::XPerlInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetDebugEvent(nEvent, nParam, lpszParam);
}

int __stdcall CPerlInterp::XPerlInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->SetBreakList(lpszFile, auBreaks);
}

int __stdcall CPerlInterp::XPerlInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetBreakList(lpszFile, auBreaks);
}

int __stdcall CPerlInterp::XPerlInterp::GetEnableProfileAnalyse()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetEnableProfileAnalyse();
}

int __stdcall CPerlInterp::XPerlInterp::GetAvailableLineCount()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetAvailableLineCount();
}

LPVOID __stdcall CPerlInterp::XPerlInterp::GetProfileMap()
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->GetProfileMap();
}

int __stdcall CPerlInterp::XPerlInterp::MethodIsSupport(LPCTSTR lpszMethod)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->MethodIsSupport(lpszMethod);
}

int __stdcall CPerlInterp::XPerlInterp::InitialDebugMode(CString& strResult)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->InitialDebugMode(strResult);
}

int __stdcall CPerlInterp::XPerlInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->RefreshDebugWindow(lpszWindow, bActive, strResult);
}

int __stdcall CPerlInterp::XPerlInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->CallMethod(lpszMethod, asParam);
}

int __stdcall CPerlInterp::XPerlInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
	return pObj->Compile(lpszSrcFile, strDestFile, lpszParams);
}

int __stdcall CPerlInterp::XPerlInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CPerlInterp *pObj = GET_INTERFACE_OBJECT(PerlInterp);
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

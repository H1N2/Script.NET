#include "stdafx.h"
#include "CTclInterp.h"
#include "TclPcom.h"
#include "TclXml.h"
#include "TclExCmd.h"
#include "TclDuiVision.h"
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

// ��ǰ����
static int g_nCurrentLanguage = VCI_LANGUAGE_ENGLISH;
int gGetCurrentLanguage()
{
	return g_nCurrentLanguage;
}

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

/////////////////////////////////////////////////////////////////////////////
// ת�����,��ͨ�ַ���ת��ΪUTF-8����
/////////////////////////////////////////////////////////////////////////////
void ConvertResultToUTF(Tcl_Interp* interp)
{
	Tcl_DString dst;

	//Tcl_ExternalToUtfDString(NULL, interp->result, -1, &dst);
	Tcl_ExternalToUtfDString(NULL, Tcl_GetStringResult(interp), -1, &dst);

	CString szResult = dst.string;
	//strcpy(interp->result, szResult);
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, szResult, (char*) NULL);
	Tcl_DStringFree(&dst);
}

/////////////////////////////////////////////////////////////////////////////
// ת���ַ���,��ͨ�ַ���ת��ΪUTF-8����
/////////////////////////////////////////////////////////////////////////////
CString ConvertStringToUTF(CString strParam)
{
	Tcl_DString dst;
	
	Tcl_ExternalToUtfDString(NULL, strParam, -1, &dst);
	
	CString szResult = dst.string;
	Tcl_DStringFree(&dst);

	return szResult;
}

/////////////////////////////////////////////////////////////////////////////
// ת������,UTF-8�����ַ���ת��Ϊ��ͨ�ַ���
/////////////////////////////////////////////////////////////////////////////
void ConvertUTFToString(int argc, char* argv[])
{
	int i;
	Tcl_DString dst;
	char* result;

	for(i = 0; i < argc; i++ )
	{
		result = Tcl_UtfToExternalDString(NULL, argv[i], strlen(argv[i]), &dst);
		strcpy(argv[i], result);
		Tcl_DStringFree(&dst);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ת����������,UTF-8�����ַ���ת��Ϊ��ͨ�ַ���
/////////////////////////////////////////////////////////////////////////////
CString ConvertUTFParam(CString strParam)
{
	Tcl_DString dst;

	Tcl_UtfToExternalDString(NULL, strParam, strlen(strParam), &dst);
	CString szResult = dst.string;

	Tcl_DStringFree(&dst);

	return szResult;
}

/////////////////////////////////////////////////////////////////////////////
// ����һ��Tcl��List����,������ַ������������,����Tcl����ָ��
/////////////////////////////////////////////////////////////////////////////
Tcl_Obj* CreateTclList(Tcl_Interp* interp, CStringArray& asItem)
{
	Tcl_Obj* listObj = Tcl_NewListObj(0, NULL);
	for(int el = 0; el < asItem.GetSize(); el++)
	{
		CString strElement = asItem[el];
		strElement = ConvertStringToUTF(strElement);
		Tcl_Obj* objPtr = Tcl_NewStringObj(strElement.GetBuffer(strElement.GetLength()), strElement.GetLength());
		strElement.ReleaseBuffer();
		Tcl_ListObjAppendElement(interp, listObj, objPtr);
	}

	return listObj;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:TCL�ű�������չ����
// ����1:TCL���Ͷ���ָ��
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_Plat_Debug(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	// ��ȡTCL����������
	CTclInterp* pTclInterp = (CTclInterp*)(GetTclInterp(interp));
	if(pTclInterp == NULL)
	{
		Tcl_AppendResult(interp, "tcl debug error, tclinterp pointer invalid", (char *) NULL);
		return TCL_ERROR;
	}

	// �������ͨ����ģʽ,�������˵�������
	if(SCRIPT_RUN_NORMAL == pTclInterp->GetRunMode())
	{
		return TCL_OK;
	}

	IPlatUI* pIPlatUI = pTclInterp->pIPlatUI;

	int nBreakLineNo = 0;
	CString strBreakName = "";
	CString strBreakFile = "";
	CDebugFile* pDebugFile = NULL;

	for(int i=1; i<argc; i++)
	{
		CString strOption = argv[i];
		if(strOption[0] == '*')	// �����жϵ�
		{
			strOption.Delete(0, 1);
			nBreakLineNo = atoi(strOption);	// �ϵ��к�

			// ������滹��һ������,��ʾ�Ƕ��ļ����Ե��ļ�����Ϣ
			if((i+1) < argc)
			{
				CString strDebugFileIndex = argv[i+1];
				int nDebugFileIndex = atoi(strDebugFileIndex);
				if(nDebugFileIndex < pTclInterp->m_aDebugFile.GetSize())
				{
					pDebugFile = &(pTclInterp->m_aDebugFile[nDebugFileIndex]);
				}
				if(pDebugFile)
				{
					strBreakFile = pDebugFile->m_strFile;
					if(pTclInterp->GetEnableProfileAnalyse())
					{
						CMap<int,int,int,int>* pProfileMap = (CMap<int,int,int,int>*)(&(pDebugFile->m_ProfileMap));
						pProfileMap->SetAt(nBreakLineNo, 1);
					}
				}

				i++;
			}else
			{
				if(pTclInterp->GetEnableProfileAnalyse())
				{
					CMap<int,int,int,int>* pProfileMap = (CMap<int,int,int,int>*)(pTclInterp->GetProfileMap());
					pProfileMap->SetAt(nBreakLineNo, 1);
				}
			}
		}else
		if(strOption[0] == '#')	// ���������ϵ�
		{
			strOption.Delete(0, 1);
			strBreakName = strOption;	// �ϵ���
		}
		if(strOption[0] == '-')	// �����������
		{
			strOption.Delete(0, 1);
			strOption.MakeUpper();
			if(strOption == "STOP")	// ��ֹ�ű�
			{
				pTclInterp->StopScript();
			}else
			if(strOption == "NEXT")	// ����һ���ϵ�
			{
				pTclInterp->SetBreakMode(SCRIPT_BREAK_NORMAL);
				pTclInterp->SetBreakSignal(TRUE);	// ����ű�
				pTclInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
			}else
			if(strOption == "GOTO")	// ִ�е�ĳ���жϵ�������ϵ�
			{
				if(argc < (i+2))
				{
					interp->result = "wrong # args: should be \"td -goto *lineno|#breakname\"";
					return TCL_ERROR;
				}
				// ��������һ��������ȡ�кŻ�ϵ���
				CString strBreak = argv[i+1];
				if(strBreak[0] == '*')
				{
					strBreak.Delete(0, 1);
					pTclInterp->SetBreakMode(SCRIPT_BREAK_LINE);
					pTclInterp->SetBreakLineNo(atoi(strBreak));	// �ϵ��к�
					pTclInterp->SetBreakSignal(TRUE);	// ����ű�
				}else
				if(strBreak[0] == '#')
				{
					strBreak.Delete(0, 1);
					pTclInterp->SetBreakMode(SCRIPT_BREAK_NAME);
					pTclInterp->SetBreakName(strBreak);	// �ϵ���
					pTclInterp->SetBreakSignal(TRUE);	// ����ű�
				}
				pTclInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
			}else
			if(strOption == "STEP")	// ����������
			{	// ���õ�����ģʽ
				if((argc > (i+1)) && (strcmp(argv[i+1], "into") == 0))
				{	// ������̵ĵ���
					pTclInterp->SetBreakMode(SCRIPT_STEP_INTO);
				}else
				if((argc > (i+1)) && (strcmp(argv[i+1], "return") == 0))
				{	// �ɹ��̷���
					pTclInterp->SetBreakMode(SCRIPT_STEP_RETURN);
				}else
				{	// ��������̵ĵ���
					pTclInterp->SetBreakMode(SCRIPT_STEP_NEXT);
				}
				// ���õ�ǰ��ջ���
				//pTclInterp->SetBreakLevel();
				pTclInterp->SetBreakSignal(TRUE);	// ����ű�
				pTclInterp->SetRunMode(SCRIPT_RUN_DEBUG_STEP);	// ����Ϊ����ģʽ
			}
			if(strOption == "LOAD")	// ��OwmEdit����һ���ű����е���
			{
				if(argc < (i+2))
				{
					interp->result = "wrong # args: should be \"td -load tclfile\"";
					return TCL_ERROR;
				}
				// ��OwmEdit,�����ؽű��ļ�
				// δ���
			}
			return TCL_OK;
		}
	}

	// ���ű���ֹ�ź�,�ж��Ƿ�����ű�����
	if(pTclInterp->GetStopSignal())
	{
		// ��OwmEdit���͵�ǰ�����������Ϣ
		pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
		pTclInterp->SetStopSignal(FALSE);
		pTclInterp->SetRunState(SCRIPT_STATE_STOP);
		Tcl_AppendResult(interp, "script terminated at user request.", (char *) NULL);
		pTclInterp->TkDestroyWindow();
		return TCL_ERROR;
	}

	// �ϵ�����ģʽ
	if(SCRIPT_RUN_DEBUG_GO == pTclInterp->GetRunMode())
	{
		// ����Ƿ���϶ϵ�����
		if(SCRIPT_BREAK_LINE == pTclInterp->GetBreakMode())	// �жϵ�
		{
			if(nBreakLineNo != pTclInterp->GetBreakLineNo())
			{
				// �жϵ�����������,������ϵ�
				// ��OwmEdit���͵�ǰ�����������Ϣ
				pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return TCL_OK;
			}
		}else
		if(SCRIPT_BREAK_NAME == pTclInterp->GetBreakMode())	// �����ϵ�
		{
			if(strBreakName != pTclInterp->GetBreakName())
			{
				// �����ϵ�����������,������ϵ�
				// ��OwmEdit���͵�ǰ�����������Ϣ
				pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return TCL_OK;
			}
		}else
		{	// ��ͨ�ϵ�
			CUIntArray auBreakList;
			pTclInterp->GetBreakList("", auBreakList);
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
				pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
				return TCL_OK;
			}
		}
	}else
	if(SCRIPT_RUN_DEBUG_STEP == pTclInterp->GetRunMode())	// ����
	{
		int nLevel = pTclInterp->GetCurStackLevel();
		int nOldLevel = pTclInterp->GetStackLevel();
		if(SCRIPT_STEP_NEXT == pTclInterp->GetBreakMode())
		{	// �������������
			if(nLevel > nOldLevel)
			{
				return TCL_OK;
			}
		}else
		if(SCRIPT_STEP_RETURN == pTclInterp->GetBreakMode())
		{	// �ɹ��̷���
			if(nLevel >= nOldLevel && nOldLevel != -1)
			{
				return TCL_OK;
			}
		}else
		if(SCRIPT_STEP_INTO == pTclInterp->GetBreakMode())
		{	// �����������
		}
		//pTclInterp->SetStackLevel(nLevel);
	}

	// �����ջ��Ȳ���,������һ�εıȽ�
	pTclInterp->SetStackLevel(pTclInterp->GetCurStackLevel());

	if(pDebugFile && pIPlatUI)
	{
		pIPlatUI->OpenDocumentFile(strBreakFile, "");
		// ��OwmEdit������������ģʽΪ�ӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)strBreakFile, 1, NULL);
	}

	// ��OwmEdit���Ͷϵ��к���Ϣ
	pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, nBreakLineNo, strBreakFile);

	pTclInterp->SetRunState(SCRIPT_STATE_BREAK);

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
		if(pTclInterp->GetStopSignal())
		{
			// ��OwmEdit���͵�ǰ�����������Ϣ
			pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);
			pTclInterp->SetStopSignal(FALSE);
			pTclInterp->SetBreakSignal(FALSE);
			pTclInterp->SetRunState(SCRIPT_STATE_STOP);
			Tcl_AppendResult(interp, "script terminated at user request.", (char *) NULL);
			pTclInterp->TkDestroyWindow();
			return TCL_ERROR;
		}

		// ���ϵ㼤���ź�
		if(pTclInterp->GetBreakSignal())
		{
			pTclInterp->SetBreakSignal(FALSE);
			break;
		}
	}

	pTclInterp->SetRunState(SCRIPT_STATE_RUN);

	// ��OwmEdit���͵�ǰ�����������Ϣ
	pTclInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0, strBreakFile);

	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:�滻TCL�����source����
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_Source(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	if(argc < 2)
	{
		interp->result = "wrong # args: should be \"source file\"";
		return TCL_ERROR;
	}

	CTclInterp* pTclInterp = (CTclInterp*)(GetTclInterp(interp));
	if(pTclInterp == NULL)
	{
		Tcl_AppendResult(interp, "source error, tclinterp pointer invalid", (char *) NULL);
		return TCL_ERROR;
	}

	CString strFile = ConvertUTFParam(argv[1]);
	strFile.MakeLower();
	strFile.Replace("\\", "/");
	int nPos = strFile.ReverseFind('/');
	if(nPos != -1)
	{
		CString strFileName = strFile;
		CString strDir = strFile.Left(nPos);
		strFileName.Delete(0, nPos+1);
		if(strFileName == "pkgindex.tcl")
		{
			// pkgindex.tcl�ļ�����Ҫ���Լ���,ʹ��ԭʼ��source�������
			// ע��Ҫ����dir����Ϊpkgindex.tcl���ļ�·��
			CString strTemp;
			strTemp.Format("uplevel #0 {set dir %s;source_std \"%s\"};", strDir, strFile);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return TCL_ERROR;
			}
			return TCL_OK;
		}
	}

	// �ж�������ع��Ͳ����ټ�����
	for(int i=0; i<pTclInterp->m_aDebugFile.GetSize(); i++)
	{
		if(strFile == pTclInterp->m_aDebugFile[i].m_strFile)
		{
			return TCL_OK;
		}
	}

	// ���ļ�����ʱ���ų�������libĿ¼�µ��ļ�
	BOOL bMultiDebugExcludeLib = AfxGetApp()->GetProfileInt(REG_TCL_DEBUG_SUBKEY, REG_TCL_MULTIDEBUG_EXCLUDE_LIB, TRUE);
	if(bMultiDebugExcludeLib)
	{
		CString strLibPath = GetInterpRootPath() + "lib/";
		strLibPath.MakeLower();
		strLibPath.Replace("\\", "/");
		if(strFile.Find(strLibPath) == 0)
		{
			// �����е��Լ���,��ʹ��ԭʼ��source�������
			CString strTemp;
			strTemp.Format("uplevel #0 {source_std \"%s\"};", strFile);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return TCL_ERROR;
			}
			return TCL_OK;
		}
	}

	// ȷ���Ƿ���شӵ����ļ�
	BOOL bMultiDebugQuery = AfxGetApp()->GetProfileInt(REG_TCL_DEBUG_SUBKEY, REG_TCL_MULTIDEBUG_QUERY, FALSE);
	if(bMultiDebugQuery)
	{
		CString strTemp, strTemp1;
		LOADRCSTRING( strTemp1, IDS_MULTIDEBUG_QUERY);
		strTemp.Format(strTemp1, strFile);

		if(pTclInterp->m_bMultiDebugKeepSelect && !pTclInterp->m_bMultiDebugLoadSource)
		{
			// �����е��Լ���,��ʹ��ԭʼ��source�������
			strTemp.Format("uplevel #0 {source_std \"%s\"};", strFile);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return TCL_ERROR;
			}
			return TCL_OK;
		}else
		if(!pTclInterp->m_bMultiDebugKeepSelect)
		{
			TTaskDialogInfo info;
			int nRet;
			LOADRCSTRING( info.strWindowTitle, IDS_MULTIDEBUG_QUERY_TITLE );
			//info.strMainInstruction = "";
			info.strContent = strTemp;
			info.strMainIcon = "Information";
			LOADRCSTRING( info.strVerificationText, IDS_MULTIDEBUG_QUERY_INHERIT );
			info.bCommonButtonYes = TRUE;
			info.bCommonButtonNo = TRUE;
			info.bCommonButtonOk = FALSE;
			info.bCommonButtonCancel = FALSE;
			pTclInterp->pIPlatUI->SendOwmCmd("", OC_TASK_DIALOG, 0, (LPARAM)(&info), &nRet);
			pTclInterp->m_bMultiDebugKeepSelect = info.bIsVerificiationChecked;
			pTclInterp->m_bMultiDebugLoadSource = (nRet != IDNO);
			if(nRet != IDYES)
			{
				// �����е��Լ���,��ʹ��ԭʼ��source�������
				strTemp.Format("uplevel #0 {source_std \"%s\"};", strFile);
				if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
				{
					return TCL_ERROR;
				}
				return TCL_OK;
			}
		}
	}

	int nFileIndex = pTclInterp->m_aDebugFile.Add(CDebugFile());
	CDebugFile* pDebugFile = &(pTclInterp->m_aDebugFile[nFileIndex]);
	pDebugFile->m_nFileIndex = nFileIndex;
	pDebugFile->m_strFile = strFile;
	if(!pDebugFile->ProcessRunBuffer())
	{
		Tcl_AppendResult(interp, "source error, process debug file ", strFile, "failed", (char *) NULL);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}

	IPlatUI* pIPlatUI = pTclInterp->pIPlatUI;
	if(pIPlatUI)
	{
		int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);
		CString strTmp;
		strTmp.Format("[DEBUG]processed file %s for debug.\r\n", strFile);
		pIPlatUI->Output(nOT, strTmp);
	}

	if(!pDebugFile->RunScriptBuffer(interp))
	{
		return TCL_ERROR;
	}

	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:��ȡƽ̨��Ϣ
// platform_info application-name
// platform_info copyright
// platform_info homepage
// platform_info download-url
// platform_info startpage
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_PlatformInfo(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	if(argc < 2)
	{
		interp->result = "wrong # args: should be \"platform_info option\"";
		return TCL_ERROR;
	}

	CTclInterp* pTclInterp = (CTclInterp*)(GetTclInterp(interp));
	if(pTclInterp == NULL)
	{
		Tcl_AppendResult(interp, "platform_info error, tclinterp pointer invalid", (char *) NULL);
		return TCL_ERROR;
	}

	IPlatUI* pIPlatUI = pTclInterp->pIPlatUI;
	if(pIPlatUI == NULL)
	{
		Tcl_AppendResult(interp, "platform_info error, get platform interface failed", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOption = ConvertUTFParam(argv[1]);
	CString strValue;
	if(strOption == "application-name")
	{
		strValue = pIPlatUI->GetAppName();
	}else
	if(strOption == "copyright")
	{
		strValue = pIPlatUI->GetPlatCopyRight();
	}else
	if(strOption == "homepage")
	{
		strValue = pIPlatUI->GetPlatHomeURL();
	}else
	if(strOption == "download-url")
	{
		strValue = pIPlatUI->GetPlatDownloadURL();
	}else
	if(strOption == "startpage")
	{
		pIPlatUI->GetVciParameter("", "StartPage", strValue, strValue);
	}else
	{
		Tcl_AppendResult(interp, "option must be application-name, copyright, homepage, download-url or startpage.",  (char *) NULL);
	}	

	Tcl_AppendResult(interp, ConvertStringToUTF(strValue), (char *)NULL);

	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:�滻TCL�����exit����
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_Exit(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	//VERIFY_TCLCMD_RIGHT(USER_NONE);

	CTclInterp* pTclInterp = (CTclInterp*)(GetTclInterp(interp));
	if(pTclInterp == NULL)
	{
		Tcl_AppendResult(interp, "exit error, tclinterp pointer invalid", (char *) NULL);
		return TCL_ERROR;
	}

	return pTclInterp->Exit();
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:��ȡ����򿪵��ļ��б�
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_RecentFileList(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	for(int i=0; i<10; i++)
	{
		CString strTmp;
		strTmp.Format("File%d", i+1);
		CString strFile = AfxGetApp()->GetProfileString("Recent File List", strTmp, "");
		if(strFile != "")
		{
			Tcl_AppendElement(interp, ConvertStringToUTF(strFile));
		}
	}

	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:��ȡ����򿪵Ĺ����б�
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_RecentProjectList(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	for(int i=0; i<10; i++)
	{
		CString strTmp;
		strTmp.Format("PROJECT%d", i+1);
		CString strProject = AfxGetApp()->GetProfileString("RecentProject", strTmp, "");
		if(strProject != "")
		{
			Tcl_AppendElement(interp, ConvertStringToUTF(strProject));
		}
	}

	return TCL_OK;
}

CString GetOutputIdByName(Tcl_Interp *interp, CString strName)
{
	CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
	IPlatUI* pIPlatUI = NULL;
	if(pTclInterp)
	{
		pIPlatUI = pTclInterp->pIPlatUI;
	}
	if(pIPlatUI == NULL)
	{
		return "";
	}

	if((strName.Find("<") == 0) && (strName.Right(1) == '>'))
	{
		strName.Delete(0, 1);
		strName = strName.Left(strName.GetLength()-1);
		strName = ConvertUTFParam(strName);
		CString strId = "-1";
		strId = pIPlatUI->OutputGet(-1, strName);
		return strId;
	}
	return "";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_PutsObjCmd_Ex --
 *
 *	This procedure is invoked to process the "puts" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	Produces output on a channel.
 *
 *----------------------------------------------------------------------
 */
// �Ա�׼puts�������չ
int Tcl_PutsObjCmd_Ex(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Channel chan;			/* The channel to puts on. */
    int i;				/* Counter. */
    int newline;			/* Add a newline at end? */
    char *channelId;			/* Name of channel for puts. */
    int result;				/* Result of puts operation. */
    int mode;				/* Mode in which channel is opened. */
    char *arg;
    int length;

    i = 1;
    newline = 1;
    if ((objc >= 2) && (strcmp(Tcl_GetString(objv[1]), "-nonewline") == 0)) 
	{
		newline = 0;
		i++;
    }
    if ((i < (objc-3)) || (i >= objc)) 
	{
		Tcl_WrongNumArgs(interp, 1, objv, "?-nonewline? ?channelId? string");
		return TCL_ERROR;
    }

    /*
     * The code below provides backwards compatibility with an old
     * form of the command that is no longer recommended or documented.
     */

    if (i == (objc-3)) 
	{
		arg = Tcl_GetStringFromObj(objv[i + 2], &length);
		if (strncmp(arg, "nonewline", (size_t) length) != 0) 
		{
			Tcl_AppendResult(interp, "bad argument \"", arg,
				"\": should be \"nonewline\"", (char *) NULL);
			return TCL_ERROR;
		}
		newline = 0;
    }
    if (i == (objc - 1)) 
	{
		channelId = "stdout";	// �޸�,��׼���������stdout
								// �˴���Ϊȱʡ�����ƽ̨ĳ��������ڻ�����λ��
								// �˴��ںſ��Ը�
    } else 
	{
		channelId = Tcl_GetString(objv[i]);
		i++;
    }

	CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
	IPlatUI* pIPlatUI = NULL;
	if(pTclInterp)
	{
		pIPlatUI = pTclInterp->pIPlatUI;
	}

	CString strChannelId = channelId;
	if(strcmp(channelId, "stdout") == 0)	// ��׼����ض���ƽָ̨������
	{
		// �����ƽ̨�������
		if(pIPlatUI)
		{
			BOOL bTclDirectOutput = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT, TRUE);
			if(bTclDirectOutput)
			{
				int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
				pIPlatUI->OutputSet(nOT, "COLOR=0,0,255");	// ���������ɫ
				pIPlatUI->Output(nOT, ConvertUTFParam(Tcl_GetString(objv[i])));
				if(newline)	// ��newline�����Ĵ���
				{
					CString strNewLine = "\r\n";
					pIPlatUI->Output(nOT, strNewLine);
				}
			}
		}
		// �����TCL��׼���
		BOOL bTclDirectResult = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_RESULT, TRUE);
		if(bTclDirectResult)
		{	// ���������
			Tcl_AppendResult(interp, Tcl_GetString(objv[i]), (char *)NULL);
			if(newline)	// ��newline�����Ĵ���
			{
				Tcl_AppendResult(interp, "\r\n", (char *)NULL);
			}
			// �ж��Ƿ�����Ϣ���õı�־
			if(strcmp(Tcl_GetString(objv[i]), "-reset") == 0)
			{
				Tcl_ResetResult(interp);
			}
		}
		return TCL_OK;
	}else
	if(strcmp(channelId, "stderr") == 0)	// ��׼�����ض���ƽָ̨������,��ʹ��������ɫ��ʾ
	{
		if(pIPlatUI)
		{
			int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
			pIPlatUI->OutputFormat(nOT, 2, ConvertUTFParam(Tcl_GetString(objv[i])));
			if(newline)	// ��newline�����Ĵ���
			{
				CString strNewLine = "\r\n";
				pIPlatUI->OutputFormat(nOT, 2, strNewLine);
			}
		}
		return TCL_OK;
	}else
	if(strChannelId.Find("format") == 0)	// ���ô��ڸ�ʽ
	{
		int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
		CString strOT = channelId;
		CString strFormat = "";
		if(strOT.Find("format,") == 0)
		{
			strOT.Delete(0, 7);
			if((strOT.Find("<") == 0) && (strOT.Right(1) == '>'))
			{
				nOT = atoi(GetOutputIdByName(interp, strOT));
			}else
			{
				nOT = atoi(strOT);
			}
		}
		strFormat = ConvertUTFParam(Tcl_GetString(objv[i]));

		if(pIPlatUI)
		{
			pIPlatUI->OutputSet(nOT, strFormat);
		}
		return TCL_OK;
	}else
	if(strChannelId.Find("getinfo") == 0)	// ��ȡ������Ϣ
	{
		CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));

		int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
		CString strOT = channelId;
		if(strOT.Find("getinfo,") == 0)
		{
			strOT.Delete(0, 8);
			if((strOT == "id") && (strParam.Find("<") == 0) && (strParam.Right(1) == '>'))
			{
				Tcl_AppendResult(interp, GetOutputIdByName(interp, Tcl_GetString(objv[i])), (char *) NULL);
			}else
			if((strOT.Find("<") == 0) && (strOT.Right(1) == '>'))
			{
				nOT = atoi(GetOutputIdByName(interp, strOT));
			}else
			{
				nOT = atoi(strOT);
			}
		}

		if(pIPlatUI)
		{
			CString strInfo = pIPlatUI->OutputGet(nOT, strParam);
			Tcl_AppendResult(interp, strInfo, (char *) NULL);
		}
		return TCL_OK;
	}else
	if(strcmp(channelId, "clear") == 0)	// ��մ�������
	{
		int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
		CString strOT = ConvertUTFParam(Tcl_GetString(objv[i]));
		if((strOT.Find("<") == 0) && (strOT.Right(1) == '>'))
		{
			nOT = atoi(GetOutputIdByName(interp, strOT));
		}else
		if(strOT.Find("-") == 0)
		{
			strOT.Delete(0, 1);
			nOT = atoi(strOT);			
		}else
		{
			nOT = atoi(strOT);
		}

		if(pIPlatUI)
		{
			pIPlatUI->OutputClear(nOT);
		}
		return TCL_OK;
	}else
	if(strcmp(channelId, "create") == 0)	// ��������
	{
		CString strOT = ConvertUTFParam(Tcl_GetString(objv[i]));
		int nPos = strOT.Find("##");

		if(pIPlatUI)
		{
			int nOT = pIPlatUI->AddOutput(strOT);
			CString strTmp;
			strTmp.Format("%d", nOT);
			Tcl_AppendResult(interp, strTmp, (char *) NULL);
		}
		return TCL_OK;
	}else
	if(strcmp(channelId, "active") == 0)	// �����
	{
		int nOT = AfxGetApp()->GetProfileInt(REG_DIRECT_SUBKEY, REG_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
		CString strOT = ConvertUTFParam(Tcl_GetString(objv[i]));
		if((strOT.Find("<") == 0) && (strOT.Right(1) == '>'))
		{
			nOT = atoi(GetOutputIdByName(interp, strOT));
		}else
		if(strOT.Find("-") == 0)
		{
			strOT.Delete(0, 1);
			nOT = atoi(strOT);
		}else
		{
			nOT = atoi(strOT);
		}

		if(pIPlatUI)
		{
			pIPlatUI->ActiveOutput(nOT);
		}
		return TCL_OK;
	}else
	{
		CString strChannel = channelId;
		if(strChannel.Find("-") == 0)
		{
			strChannel.Delete(0, 1);
			int nOT = atoi(strChannel);
			if(pIPlatUI)
			{
				pIPlatUI->Output(nOT, ConvertUTFParam(Tcl_GetString(objv[i])));
				if(newline)	// ��newline�����Ĵ���
				{
					CString strNewLine = "\r\n";
					pIPlatUI->Output(nOT, strNewLine);
				}
			}
			return TCL_OK;
		}
	}

    chan = Tcl_GetChannel(interp, channelId, &mode);
    if (chan == (Tcl_Channel) NULL) 
	{
        return TCL_ERROR;
    }
    if ((mode & TCL_WRITABLE) == 0) 
	{
		Tcl_AppendResult(interp, "channel \"", channelId,
                "\" wasn't opened for writing", (char *) NULL);
        return TCL_ERROR;
    }

    result = Tcl_WriteObj(chan, objv[i]);
    if (result < 0) 
	{
        goto error;
    }
    if (newline != 0) 
	{
        result = Tcl_WriteChars(chan, "\n", 1);
        if (result < 0) 
		{
            goto error;
        }
    }
    return TCL_OK;

error:
    Tcl_AppendResult(interp, "error writing \"", channelId, "\": ",
	    Tcl_PosixError(interp), (char *) NULL);

    return TCL_ERROR;
}

//////////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////////
static CPtrList	g_TclInterpList;	// �����������б�
static int g_TclInterpID = 0x100;	// ������ID������

// ��ȡTCL�������б�ָ��
CPtrList* GetTclInterpList()
{
	return &g_TclInterpList;
}

// ��һ��������
int OpenTclInterp(LPCTSTR lpszInterptName, BOOL bSupportTk)
{
	// �����Ƿ��Ѿ�����
	if(lpszInterptName != NULL && lpszInterptName != "")
	{
		int count = g_TclInterpList.GetCount();
		for(int i = 0; i < count; i ++)
		{
			POSITION pos = g_TclInterpList.FindIndex(i);
			CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
			if(lpszInterptName == pTclInterp->GetInterpName()) 
			{
				//pTclInterp->AddRef();	// �������ü���
				return pTclInterp->GetInterpID();
			}
		}
	}
	// ���������½�
	CTclInterp* pTclInterp = new CTclInterp();
	if(pTclInterp)
	{
		pTclInterp->SetInterpName(lpszInterptName);
		return pTclInterp->GetInterpID();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ���������ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CTclInterp* GetTclInterp(LPCTSTR lpszInterptName)
{
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(lpszInterptName == pTclInterp->GetInterpName())
		{
			return pTclInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ�����ID��ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CTclInterp* GetTclInterp(int nInterpID)
{
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(nInterpID == pTclInterp->GetInterpID())
		{
			return pTclInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ�����ָ���ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CTclInterp* GetTclInterp(LPVOID lpTclInterp)
{
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(lpTclInterp == pTclInterp->GetInterp())
		{
			return pTclInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// �رս�����(���ݽ�������)
//////////////////////////////////////////////////////////////////////
BOOL CloseTclInterp(LPCTSTR lpszInterptName)
{
	if(NULL == lpszInterptName)
		return FALSE;

	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(lpszInterptName == pTclInterp->GetInterpName())
		{
			//if(pTclInterp->ReleaseRef() == 0)	// ���ü�����0��ɾ������
			//	delete pTclInterp;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// �رս�����(���ݽ�����ID)
//////////////////////////////////////////////////////////////////////
BOOL CloseTclInterp(int nInterpID)
{
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(nInterpID == pTclInterp->GetInterpID())
		{
			//if(pTclInterp->ReleaseRef() == 0)	// ���ü�����0��ɾ������
			//	delete pTclInterp;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ɾ������δɾ���Ľ�����(ƽ̨�ر�ʱ����)
//////////////////////////////////////////////////////////////////////
void CloseAllTclInterp()
{
	// ɾ�������������б��е���
	int count = g_TclInterpList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		delete pTclInterp;
	}
}


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
// ע:�˴�Ŀǰȫ����ΪUNIX�����л��з�����,��ΪTCLȱʡ�������ַ��
// �����ΪWindows���,��BufSize�ȵط���Ҫע��
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
		CStdioFile fi(m_strFile, CFile::modeRead);
		int nLineNo = 0;		// �к�
		int nBufSize = 0;		// ��������С
		BOOL bEndLine1 = TRUE;	// �Ƿ�����'\'
		BOOL bEndLine2 = TRUE;	// �Ƿ�����'"'
		BOOL bComment = FALSE;	// ע��
		BOOL bVarMultiLine = FALSE;	// ���еı���(��Ҫָ��{���еı���)
		int  nVarLevel = 0;		// ���б����е����
		BOOL bCmdSwitch = FALSE;// ����switch������
		int  nSwitchLevel = 0;	// switch�����е����
		CString strLine;
		CStringArray asScript;	// ������

		while(fi.ReadString(strLine))
		{
			nLineNo++;
			CString strLineSrc = strLine;
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ");

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

			if(!bVarMultiLine)	// �ж��Ƿ���б�������ʼ��
			{
				int nLen = strLine.GetLength();
				if(strLine[nLen-1] == '{')
				{
					CString strLineTmp = strLine;
					int nPos = strLineTmp.ReverseFind(';');
					if(nPos != -1)
					{
						strLineTmp.Delete(0, nPos+1);
					}
					strLineTmp.TrimLeft("\t ");
					//if(((strLineTmp.Find("set") == 0) || (strLineTmp.Find("append") == 0) || (strLineTmp.Find("lappend") == 0)) &&
					//	(strLineTmp.GetLength() > 4) &&
					//	((strLineTmp[3] == ' ') || (strLineTmp[3] == '\t')) )
					if(((strLineTmp.Find("set") == 0) &&
						(strLineTmp.GetLength() > 4) &&
						((strLineTmp[3] == ' ') || (strLineTmp[3] == '\t')) ) ||
					   ((strLineTmp.Find("lappend") == 0) &&
						(strLineTmp.GetLength() > 8) &&
						((strLineTmp[7] == ' ') || (strLineTmp[7] == '\t')) ) ||
					   ((strLineTmp.Find("append") == 0) &&
						(strLineTmp.GetLength() > 7) &&
						((strLineTmp[6] == ' ') || (strLineTmp[6] == '\t')) ) ||
					   ((strLineTmp.Find("array") == 0) &&
						(strLineTmp.GetLength() > 12) &&
						((strLineTmp[5] == ' ') || (strLineTmp[5] == '\t')) &&
						(strLineTmp.Find(" set ") > 4) ) )
					{
						bVarMultiLine = TRUE;
						nVarLevel = 1;

						CString strBreak;
						strBreak.Format("td *%d %d;", nLineNo, m_nFileIndex);
						strLineSrc = strBreak + strLineSrc;
						asScript.Add(strLineSrc);
						nBufSize += strLineSrc.GetLength();
						nBufSize += 1;	// "\n"
						m_nTDLineCount ++;
						continue;
					}
				}
			}			

			if(bEndLine1 && bEndLine2 && !bCmdSwitch && !bVarMultiLine)	// �����������(������switch������,�Ҳ����ڶ��б�������״̬),��ӵ�����־
			{
				CString strBreak;
				strBreak.Format("td *%d %d;", nLineNo, m_nFileIndex);
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

			if( (strLine.Find("switch") == 0) && (strLine[strLine.GetLength()-1] == '{') &&
				((strLine[6] == ' ') || (strLine[6] == '\t')) )	// ����switch����״̬
			{
				bCmdSwitch = TRUE;
				nSwitchLevel = 0;
			}else
			if(bCmdSwitch)	// ����switch����״̬��
			{
				if((nSwitchLevel <= 0) && (strLine[0] == '}'))	// �˳�switch����״̬
				{
					bCmdSwitch = FALSE;
				}else
				{
					if(strLine.Find('{'))
					{
						nSwitchLevel++;
					}
					if(strLine.Find('}'))
					{
						nSwitchLevel--;
					}
				}
			}

			if(bVarMultiLine)
			{
				// ��ǰ���ڶ��б���������״̬,�ж�ʲôʱ���������״̬
				int nPosLeft = -1;
				int nPosRight = -1;
				while( ((nPosRight = strLine.Find('}')) != -1) || ((nPosLeft = strLine.Find('{')) != -1) )
				{
					if((nPosLeft != -1) && ((nPosLeft == 0) || (strLine[nPosLeft-1] != '\\')))
					{
						nVarLevel++;
					}
					if((nPosRight != -1) && ((nPosRight == 0) || (strLine[nPosRight-1] != '\\')))
					{
						nVarLevel--;
					}

					if(nVarLevel <= 0)
					{
						bVarMultiLine = FALSE;
						break;
					}

					//if((nPos == 0) || (strLine[nPos-1] != '\\'))
					//{
					//	bVarMultiLine = FALSE;
					//	break;
					//}
					if(nPosLeft != -1)
					{
						strLine.Delete(0, nPosLeft+1);
					}else
					if(nPosRight != -1)
					{
						strLine.Delete(0, nPosRight+1);
					}
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
// ִ�л������е�TCL����
/////////////////////////////////////////////////////////////////////////////
int CDebugFile::RunScriptBuffer(Tcl_Interp*	pInterp)
{
	if(NULL == m_pRunBuffer)
		return FALSE;

	// �޸���ͨ�ַ���ΪUTF-8�����ַ���
	Tcl_DString dst;
	char* script = Tcl_ExternalToUtfDString(NULL, m_pRunBuffer, -1, &dst);
	// ִ�нű�����
	if(TCL_OK != Tcl_Eval(pInterp, script))
	{
		Tcl_DStringFree(&dst);
		//ConvertResultToUTF(pInterp);
		return FALSE;
	}

	Tcl_DStringFree(&dst);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CTclInterp::CTclInterp()
{
	m_nInterpID		= g_TclInterpID++;
	if(g_TclInterpID > 0x01000000)
		g_TclInterpID = 0x100;
	pIPlatUI		= NULL;
	m_strAppProcess	= _T("");
	m_dwRunThreadId	= NULL;
	#ifdef _USE_TCL84
	Tcl_FindExecutable(Tcl_GetNameOfExecutable());
	#endif
	#ifdef _USE_TCL85
	Tcl_FindExecutable(_T(""));	// Tcl8.5�����ϰ汾����ʹ��Tcl_GetNameOfExecutable��������쳣
	#endif
	#ifdef _USE_TCL86
	Tcl_FindExecutable(_T(""));	// Tcl8.5�����ϰ汾����ʹ��Tcl_GetNameOfExecutable��������쳣
	#endif

	m_strInterpName	= _T("");
	pInterp			= Tcl_CreateInterp();
	Tcl_SetServiceMode(TCL_SERVICE_NONE);
/*
	// ------------
	m_TclNotifier = Tcl_InitNotifier();
	Tcl_AlertNotifier(m_TclNotifier);
	Tcl_SetServiceMode(TCL_SERVICE_ALL);

	Tcl_Time timeout;
	timeout.sec = 0;
	timeout.usec = 25000;
	Tcl_SetTimer(&timeout);
*/
	m_strTclLibPath	= GetInterpRootPath();
	m_strTclLibPath	+= "Lib\\";
	m_strScriptFile	= _T("");
	m_pRunBuffer	= NULL;
	m_nRunMode		= SCRIPT_RUN_NORMAL;
	m_nRunState		= SCRIPT_STATE_IDLE;
	m_nRunLevel		= SCRIPT_LEVEL_NONE;
	m_nEvalResult	= TCL_OK;
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
	m_nSupportTk	= 1;//2;//nSupportTk;
	m_bIsTkFile		= FALSE;
	m_bHide			= FALSE;
	m_lpfnExit		= NULL;
	m_hPipeProcess	= NULL;
	m_bPipeProcessTerminated = FALSE;
	// ��ʼ������Tcl������
	InitTclBaseInterp();
	g_TclInterpList.AddTail(this);
}

CTclInterp::~CTclInterp()
{
	if(m_pRunBuffer != NULL)
		delete m_pRunBuffer;
	m_pRunBuffer = NULL;

	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();

	// �رմ˽���������������DUI����
	CloseInterpAllDuiObject(pInterp);

	try {
	Tcl_DeleteInterp(pInterp);
	}catch(...)
	{
	}

	// ɾ�������������б��е���
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		if(pTclInterp == this) 
		{
			g_TclInterpList.RemoveAt(pos);
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ�����
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::Init(LPCSTR lpcsParams)
{
	// ִ��Ԥ����ű�
	PreScript();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// �������
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::Done(LPCSTR lpcsParams)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
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
int CTclInterp::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
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
// Exit�麯��,�ɽű�����exit����
// ֮��������Tclȱʡ��exit��������Ϊȱʡexit����������������쳣
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::Exit()
{
	//Tcl_Exit(0);
	// ִ��Tk���ڵ����ٲ���,ʹTk�ű����������˳�
	TkDestroyWindow();
	TRACE("CTclInterp::Exit\n");

	if(m_lpfnExit)
	{
		m_lpfnExit((void*)(&m_xTclInterp));
	}

	return TCL_OK;
}

//{{VCI_IMPLEMENT_BEGIN
int __stdcall CTclInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	pIPlatUI = (IPlatUI*)lpIPlatUI;

	// �������Ա���
	CString strCurrentLanguage = _T("english");
	g_nCurrentLanguage = VCI_LANGUAGE_ENGLISH;
	if(VCI_LANGUAGE_CHINESE == GetLanguage())
	{
		strCurrentLanguage = _T("chinese");
		g_nCurrentLanguage = VCI_LANGUAGE_CHINESE;
	}

	// ����ƽ̨�汾����
	CString strPlatformVersion = "";
	if(pIPlatUI)
	{
		strPlatformVersion = pIPlatUI->GetPlatVersion();
	}else
	{
		strPlatformVersion = AfxGetApp()->GetProfileString(REG_VERSION_SUBKEY, REG_VER_PLATVERSION, VAR_PLATFORM_VERSION);
	}

	CString strCmd;
	strCmd.Format("set platform_language %s;set platform_version %s;set platform_interface %d;",
				strCurrentLanguage, strPlatformVersion, pIPlatUI);

	Tcl_DString dst;
	strCmd = Tcl_ExternalToUtfDString(NULL, strCmd, -1, &dst);
	Tcl_DStringFree(&dst);

	if(TCL_OK != Tcl_Eval(pInterp, strCmd.GetBuffer(strCmd.GetLength())))
	{
		strCmd.ReleaseBuffer();
		return FALSE;
	}
	strCmd.ReleaseBuffer();

	return 0;
}

// ���ÿ�·��
int __stdcall CTclInterp::SetLibPath(LPCTSTR lpszPath)
{
	m_strTclLibPath = lpszPath;
	return 0;
}

// �������Ƿ�������
int __stdcall CTclInterp::IsInterpRunning()
{
	// TODO: Add your code here
	// �ж����й�λ�Ƿ��Ѿ���ֹ
	int count = g_TclInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclInterpList.FindIndex(i);
		CTclInterp* pTclInterp = (CTclInterp*)g_TclInterpList.GetAt(pos);
		int nRunState = pTclInterp->GetRunState();
		if( (SCRIPT_STATE_RUN == nRunState) ||
			(SCRIPT_STATE_BREAK == nRunState) ||
			(SCRIPT_STATE_STEP == nRunState) )
			return TRUE;
	}
	return FALSE;
}

// ���ýű��ļ���
int __stdcall CTclInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	CString strPathName = lpszPathName;
	m_strScriptFile = "";
	if(lpszPathName != NULL)
	{
		m_strScriptFile = lpszPathName;
		if(m_strScriptFile.GetLength() == 0)
		{
			return FALSE;
		}

		if(m_strScriptFile[0] == '"')
		{	// ����0�п�����"��Χ������ȥ�����ܽ����������Ľű��ļ���
			m_strScriptFile.Delete(0, 1);
			int nPos = m_strScriptFile.Find("\"");
			if(nPos >= 0)
			{
				m_strScriptFile = m_strScriptFile.Left(nPos);
				strPathName.Delete(0, nPos+2);
			}
		}else
		{	// ��һ��?֮ǰ���ǽű��ļ���
			int nPos = m_strScriptFile.Find("?");
			if(nPos > 0)
			{
				m_strScriptFile = m_strScriptFile.Left(nPos);
				strPathName.Delete(0, nPos+1);
			}else
			{
				strPathName = "";
			}
		}
		strPathName.TrimLeft();
	}

	// �������õ�argv��������ڿո������ɴ���,��˶�����һ���������,����Script.NET��Ч
	// ���Դ��������argv,��������һ������
	CString strArgvTemp = strPathName;
	if(strArgvTemp.Find('"') == 0)
	{
		strArgvTemp.Delete(0, 1);
	}
	if(strArgvTemp.ReverseFind('"') == (strArgvTemp.GetLength()-1))
	{
		strArgvTemp = strArgvTemp.Left(strArgvTemp.GetLength()-1);
	}
	SetVar("platform_argv", ConvertStringToUTF(strArgvTemp));

	// �ļ�����Ҫʹ��TCL���ϵ�/��ʽ
//	CString strPathName = lpszPathName;
//	strPathName.Replace("\\", "/");

	// ��ʼ��argc��argv
	// ע:ĳЩ��\������\\\�����,��β�����\n
	//strPathName.Replace("\\", "\\\\");
	strPathName.Replace("\\\"", "\\\\\"");// ������һ��\"�滻Ϊ\\"
	// ע:�˴���argv�Ѿ�����TCL��׼,����ִ���ļ�����argv��ȥ��,����������argv0
	CString strFileName = m_strScriptFile;
	strFileName.Replace("\\", "/");
	CString strPreScript;
	strPreScript.Format("set argv {%s};\n\
						set argc [llength $argv];\n\
						set argv0 \"%s\";",
						strPathName, strFileName);

	Tcl_DString dst;
	strPreScript = Tcl_ExternalToUtfDString(NULL, strPreScript, -1, &dst);
	Tcl_DStringFree(&dst);

	try {
	////////////////////////////////////////////

	if(TCL_OK != Tcl_Eval(pInterp, strPreScript.GetBuffer(strPreScript.GetLength())))
	{
		strPreScript.ReleaseBuffer();
		//PF_TRACE_FMT(2, "TCL script argc&argv error:%s\r\n", Tcl_GetStringResult(pInterp));
		return FALSE;
	}
	strPreScript.ReleaseBuffer();

	////////////////////////////////////////////
	}catch(...)
	{
		TRACE("SetScriptFile Eval Tcl command '%s' exception!\n", strPreScript);
		return FALSE;
	}

	Tcl_ResetResult(pInterp);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ������Tcl������
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::InitTclBaseInterp()
{
	// ��ʼ���ű�(������׼�����������)
	// ע��ĳЩ��\������\\\�����,��β�����\r\n
	// tcl_library
	CString strTclLibrary = m_strTclLibPath;
	#ifdef _USE_TCL84
	strTclLibrary += "tcl8.4";
	#endif
	#ifdef _USE_TCL85
	strTclLibrary += "tcl8.5";
	#endif
	#ifdef _USE_TCL86
	strTclLibrary += "tcl8.6";
	#endif
	strTclLibrary.Replace('\\', '/');
	// tk_library
	CString strTkLibrary = m_strTclLibPath;
	#ifdef _USE_TCL84
	strTkLibrary += "tk8.4";
	#endif
	#ifdef _USE_TCL85
	strTkLibrary += "tk8.5";
	#endif
	#ifdef _USE_TCL86
	strTkLibrary += "tk8.6";
	#endif
	strTkLibrary.Replace('\\', '/');

	CString strPlatPath = GetPlatRootPath();
	if(strPlatPath[strPlatPath.GetLength()-1] == '\\')
		strPlatPath.Delete(strPlatPath.GetLength()-1, 1);
	strPlatPath.Replace('\\', '/');

	CString strInterpPath = GetInterpRootPath();
	if(strInterpPath[strInterpPath.GetLength()-1] == '\\')
		strInterpPath.Delete(strInterpPath.GetLength()-1, 1);
	strInterpPath.Replace('\\', '/');

#ifdef _DEBUG
	CString strPlatType = "debug";
#else
	CString strPlatType = "release";
#endif

	CString strPreScript;
	strPreScript.Format("rename puts puts_std;\n\
						rename exit exit_std;\n\
						set platform_path \"%s\";\n\
						set interp_path \"%s\";\n\
						set platform_type %s;\n\
						set tcl_library \"%s\";\n\
						set tk_library \"%s\";\n\
						set thisinterp %d;",
						strPlatPath, strInterpPath, strPlatType,
						strTclLibrary, strTkLibrary, pInterp);

	Tcl_DString dst;
	strPreScript = Tcl_ExternalToUtfDString(NULL, strPreScript, -1, &dst);
	Tcl_DStringFree(&dst);

	if(TCL_OK != Tcl_Eval(pInterp, strPreScript.GetBuffer(strPreScript.GetLength())))
	{
		strPreScript.ReleaseBuffer();
		//PF_TRACE_FMT(2, "TCL script preprocess error:%s\r\n", Tcl_GetStringResult(pInterp));
		return FALSE;
	}
	strPreScript.ReleaseBuffer();

	// ��ʼ��Tcl��
	Tcl_Init(pInterp);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Ԥ����ű�
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::PreScript()
{
	// ��ȡƽ̨��Process����
	if(pIPlatUI)
	{
		CString strDefaultValue;
		pIPlatUI->GetVciParameter("", "Process", m_strAppProcess, strDefaultValue);
	}

	// ��ȡ���ؿ���Ϣ
	BOOL bTclLibTk = FALSE;
	BOOL bTclLibiTcl = TRUE;
	BOOL bTclLibiTk = FALSE;
	if(m_strAppProcess != "Service")
	{
		bTclLibTk = AfxGetApp()->GetProfileInt(REG_TCL_LIB_SUBKEY, REG_TCL_LIB_TK, FALSE);
		bTclLibiTcl = AfxGetApp()->GetProfileInt(REG_TCL_LIB_SUBKEY, REG_TCL_LIB_ITCL, TRUE);
		bTclLibiTk = AfxGetApp()->GetProfileInt(REG_TCL_LIB_SUBKEY, REG_TCL_LIB_ITK, FALSE);
	}

	// ��ʼ���ű�(������׼�����������)
	// ע��ĳЩ��\������\\\�����,��β�����\r\n
	// tcl_library
	CString strTclLibrary = m_strTclLibPath;
	#ifdef _USE_TCL84
	strTclLibrary += "tcl8.4";
	#endif
	#ifdef _USE_TCL85
	strTclLibrary += "tcl8.5";
	#endif
	#ifdef _USE_TCL86
	strTclLibrary += "tcl8.6";
	#endif
	strTclLibrary.Replace('\\', '/');
	// tk_library
	CString strTkLibrary = m_strTclLibPath;
	#ifdef _USE_TCL84
	strTkLibrary += "tk8.4";
	#endif
	#ifdef _USE_TCL85
	strTkLibrary += "tk8.5";
	#endif
	#ifdef _USE_TCL86
	strTkLibrary += "tk8.6";
	#endif
	strTkLibrary.Replace('\\', '/');

	CString strPlatPath = GetPlatRootPath();
	if(strPlatPath[strPlatPath.GetLength()-1] == '\\')
		strPlatPath.Delete(strPlatPath.GetLength()-1, 1);
	strPlatPath.Replace('\\', '/');

	CString strInterpPath = GetInterpRootPath();
	if(strInterpPath[strInterpPath.GetLength()-1] == '\\')
		strInterpPath.Delete(strInterpPath.GetLength()-1, 1);
	strInterpPath.Replace('\\', '/');

#ifdef _DEBUG
	CString strPlatType = "debug";
#else
	CString strPlatType = "release";
#endif

	// ע:�˴����õ����Ա�������Ч��,��Ϊ��������ڽ���������������̵���,
	// ��ʱƽ̨��û�жԽ������������Ա���,��SetIPlatUI�����е����ò�������֤��Ч��,
	// �˴�ֻ������һ����ʼֵ,Ӧ�ö���english
	CString strCurrentLanguage = _T("english");
	g_nCurrentLanguage = VCI_LANGUAGE_ENGLISH;
	if(VCI_LANGUAGE_CHINESE == GetLanguage())
	{
		strCurrentLanguage = _T("chinese");
		g_nCurrentLanguage = VCI_LANGUAGE_CHINESE;
	}

	// ƽ̨�汾
	// modify at 2007-07-12,ԭ���Ļ�ȡƽ̨�汾����������,�޸�Ϊͨ��ƽ̨�ṩ�Ľӿ�����ȡ
	// ע:�˴�pIPlatUI��Ϊ��,��Ϊ���ָ�뻹û�г�ʼ��,���Ҫ��SetIPlatUI�����и�����ر���
	CString strPlatformVersion = "";
	if(pIPlatUI)
	{
		strPlatformVersion = pIPlatUI->GetPlatVersion();
	}else
	{
		strPlatformVersion = AfxGetApp()->GetProfileString(REG_VERSION_SUBKEY, REG_VER_PLATVERSION, VAR_PLATFORM_VERSION);
	}

	// ������
	CString strUIStyle = "default";
	if(m_strAppProcess != "Service")
	{
		strUIStyle = AfxGetApp()->GetProfileString(REG_UISTYLE_SUBKEY, REG_UISTYLE_HTML, "default");
	}

	CString strPreScript;
	strPreScript.Format("set platform_version %s;\n\
						set platform_interface %d;\n\
						set interp_path \"%s\";\n\
						set platform_language %s;\n\
						set platform_ui_style %s;\n\
						set interpName \"%s\";\n\
						set thisinterp %d;",
						strPlatformVersion, pIPlatUI, strInterpPath,
						strCurrentLanguage, strUIStyle,
						m_strInterpName, pInterp);

	Tcl_DString dst;
	strPreScript = Tcl_ExternalToUtfDString(NULL, strPreScript, -1, &dst);
	Tcl_DStringFree(&dst);

	if(TCL_OK != Tcl_Eval(pInterp, strPreScript.GetBuffer(strPreScript.GetLength())))
	{
		strPreScript.ReleaseBuffer();
		//PF_TRACE_FMT(2, "TCL script preprocess error:%s\r\n", Tcl_GetStringResult(pInterp));
		return FALSE;
	}
	strPreScript.ReleaseBuffer();

	// ��ʼ��Tcl��
	//Tcl_Init(pInterp);

//	ClientData TclNotifier = Tcl_InitNotifier();
//	Tcl_AlertNotifier(TclNotifier);
//	Tcl_SetServiceMode(TCL_SERVICE_ALL);

	// ��ʼ��Tk��(���m_nSupportTk == 2,��ʾǿ��֧��TK,����Ҫϵͳ����)
	if((m_nSupportTk == 2) || ((m_nSupportTk == 1) && bTclLibTk))
	{
		m_bIsTkFile = TRUE;
/*
		CWnd* pMainFrame = AfxGetMainWnd();
		char buffer[500];
		sprintf(buffer, "-use 0x%8.8x", pMainFrame->m_hWnd);
		Tcl_SetVar(pInterp, "argv", buffer, TCL_GLOBAL_ONLY);
*/
		#ifdef _SUPPORT_TK
		Tk_Init(pInterp);
		Tcl_StaticPackage(pInterp, "Tk", Tk_Init, Tk_SafeInit);
		#endif
	}

	// ��ʼ��iTcl��
	if(bTclLibiTcl)
	{
		//Itcl_Init(pInterp);
		//Tcl_Eval(pInterp, "package require Itcl");
	}

	// ��ʼ��iTk��
	if(bTclLibiTk)
	{
		//Itk_Init(pInterp);
		//Tcl_Eval(pInterp, "package require Itk");
	}

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

    Tcl_SetVar(pInterp, "tcl_rcFileName", "~/.wishrc", TCL_GLOBAL_ONLY);
	Tcl_SetVar(pInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
	Tcl_SourceRCFile(pInterp);

	// ��ʼ��i18n֧�ֽű�
	strPreScript = "source $platform_path/lib/plat/i18n.tcl";
	strPreScript = Tcl_ExternalToUtfDString(NULL, strPreScript, -1, &dst);
	Tcl_DStringFree(&dst);
	Tcl_Eval(pInterp, strPreScript.GetBuffer(strPreScript.GetLength()));
	strPreScript.ReleaseBuffer();

	// ע��ƽ̨��չ����
	Tcl_RegisterCmd(pInterp);

	// ע����չ����
	TclExCmd_Init(pInterp);

	// ע��TclWorkTree����
	TclWorkTree_Init(pInterp);

	// ע��TclDuiVision����
	TclDuiVision_Init(pInterp);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �Զ����ű��Ƿ���һ��Tk�ű�,�����ǲ����Ƿ���package require Tk����
/////////////////////////////////////////////////////////////////////////////
BOOL CTclInterp::CheckIsTkFile()
{
	BOOL bIsTkFile = FALSE;

	try
	{
		CStdioFile fi(m_strScriptFile, CFile::modeRead);		
		CString strLine;

		// ֻ���ǰ100���Ƿ���
		int nCount = 0;
		while(fi.ReadString(strLine) && nCount < 100)
		{
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ");

			if(strLine.Find("package require Tk") == 0)	// Tk�ű�
			{
				bIsTkFile = TRUE;
				break;
			}

			nCount++;
		}

	}catch(...)
	{
		return FALSE;
	}

	return bIsTkFile;
}

/////////////////////////////////////////////////////////////////////////////
// �Զ����ű��Ƿ���һ��TkView�ű�,�����ǲ����Ƿ���package require TkView����
/////////////////////////////////////////////////////////////////////////////
BOOL CTclInterp::CheckIsTkvFile()
{
	BOOL bIsTkvFile = FALSE;

	try
	{
		CStdioFile fi(m_strScriptFile, CFile::modeRead);		
		CString strLine;

		// ֻ���ǰ50���Ƿ���
		int nCount = 0;
		while(fi.ReadString(strLine) && (nCount < 50))
		{
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ");

			if(strLine.Find("package require TkView") == 0)	// Tkv�ű�
			{
				bIsTkvFile = TRUE;
				break;
			}

			nCount++;
		}

	}catch(...)
	{
		return FALSE;
	}

	return bIsTkvFile;
}

/////////////////////////////////////////////////////////////////////////////
// ��������ģʽ,���л���������
// ע:�˴�Ŀǰȫ����ΪUNIX�����л��з�����,��ΪTCLȱʡ�������ַ��
// �����ΪWindows���,��BufSize�ȵط���Ҫע��
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::ProcessRunBuffer()
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
		BOOL bComment = FALSE;	// ע��
		BOOL bVarMultiLine = FALSE;	// ���еı���(��Ҫָ��{���еı���)
		int  nVarLevel = 0;		// ���б����е����
		BOOL bCmdSwitch = FALSE;// ����switch������
		int  nSwitchLevel = 0;	// switch�����е����
		CString strLine;
		CStringArray asScript;	// ������

		while(fi.ReadString(strLine))
		{
			nLineNo++;
			CString strLineSrc = strLine;
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ");

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

			if(!bVarMultiLine)	// �ж��Ƿ���б�������ʼ��
			{
				int nLen = strLine.GetLength();
				if(strLine[nLen-1] == '{')
				{
					CString strLineTmp = strLine;
					int nPos = strLineTmp.ReverseFind(';');
					if(nPos != -1)
					{
						strLineTmp.Delete(0, nPos+1);
					}
					strLineTmp.TrimLeft("\t ");
					//if(((strLineTmp.Find("set") == 0) || (strLineTmp.Find("append") == 0) || (strLineTmp.Find("lappend") == 0)) &&
					//	(strLineTmp.GetLength() > 4) &&
					//	((strLineTmp[3] == ' ') || (strLineTmp[3] == '\t')) )
					if(((strLineTmp.Find("set") == 0) &&
						(strLineTmp.GetLength() > 4) &&
						((strLineTmp[3] == ' ') || (strLineTmp[3] == '\t')) ) ||
					   ((strLineTmp.Find("lappend") == 0) &&
						(strLineTmp.GetLength() > 8) &&
						((strLineTmp[7] == ' ') || (strLineTmp[7] == '\t')) ) ||
					   ((strLineTmp.Find("append") == 0) &&
						(strLineTmp.GetLength() > 7) &&
						((strLineTmp[6] == ' ') || (strLineTmp[6] == '\t')) ) ||
					   ((strLineTmp.Find("array") == 0) &&
						(strLineTmp.GetLength() > 12) &&
						((strLineTmp[5] == ' ') || (strLineTmp[5] == '\t')) &&
						(strLineTmp.Find(" set ") > 4) ) )
					{
						bVarMultiLine = TRUE;
						nVarLevel = 1;

						CString strBreak;
						strBreak.Format("td *%d;", nLineNo);
						strLineSrc = strBreak + strLineSrc;
						asScript.Add(strLineSrc);
						nBufSize += strLineSrc.GetLength();
						nBufSize += 1;	// "\n"
						m_nTDLineCount ++;
						continue;
					}
				}
			}			

			if(bEndLine1 && bEndLine2 && !bCmdSwitch && !bVarMultiLine)	// �����������(������switch������,�Ҳ����ڶ��б�������״̬),��ӵ�����־
			{
				CString strBreak;
				strBreak.Format("td *%d;", nLineNo);
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

			if( (strLine.Find("switch") == 0) && (strLine[strLine.GetLength()-1] == '{') &&
				((strLine[6] == ' ') || (strLine[6] == '\t')) )	// ����switch����״̬
			{
				bCmdSwitch = TRUE;
				nSwitchLevel = 0;
			}else
			if(bCmdSwitch)	// ����switch����״̬��
			{
				if((nSwitchLevel <= 0) && (strLine[0] == '}'))	// �˳�switch����״̬
				{
					bCmdSwitch = FALSE;
				}else
				{
					if(strLine.Find('{'))
					{
						nSwitchLevel++;
					}
					if(strLine.Find('}'))
					{
						nSwitchLevel--;
					}
				}
			}

			if(bVarMultiLine)
			{
				// ��ǰ���ڶ��б���������״̬,�ж�ʲôʱ���������״̬
				int nPosLeft = -1;
				int nPosRight = -1;
				while( ((nPosRight = strLine.Find('}')) != -1) || ((nPosLeft = strLine.Find('{')) != -1) )
				{
					if((nPosLeft != -1) && ((nPosLeft == 0) || (strLine[nPosLeft-1] != '\\')))
					{
						nVarLevel++;
					}
					if((nPosRight != -1) && ((nPosRight == 0) || (strLine[nPosRight-1] != '\\')))
					{
						nVarLevel--;
					}

					if(nVarLevel <= 0)
					{
						bVarMultiLine = FALSE;
						break;
					}

					//if((nPos == 0) || (strLine[nPos-1] != '\\'))
					//{
					//	bVarMultiLine = FALSE;
					//	break;
					//}
					if(nPosLeft != -1)
					{
						strLine.Delete(0, nPosLeft+1);
					}else
					if(nPosRight != -1)
					{
						strLine.Delete(0, nPosRight+1);
					}
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

// ��ȡ�ű��ļ���
CString __stdcall CTclInterp::GetScriptFile()
{
	// TODO: Add your code here
	return m_strScriptFile;
}

/////////////////////////////////////////////////////////////////////////////
// ִ��TK����ѭ��
/////////////////////////////////////////////////////////////////////////////
void CTclInterp::TkMainLoop()
{
#ifdef _SUPPORT_TK
	if(m_bIsTkFile)
	{
		Tk_Window tkwin = Tk_MainWindow(pInterp);
		if(tkwin)
		{
			Tk_MainLoop();
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// ����TK��������
/////////////////////////////////////////////////////////////////////////////
void CTclInterp::TkDestroyWindow()
{
#ifdef _SUPPORT_TK
	if(m_bIsTkFile)
	{
		Tk_Window tkwin = Tk_MainWindow(pInterp);
		if(tkwin)
		{
			Tk_DestroyWindow(tkwin);
		}
	}
#endif
}

// ���нű��ļ�
int __stdcall CTclInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
//	if((m_dwRunThreadId != NULL) && (::GetCurrentThreadId() != m_dwRunThreadId))
//	{
//		AfxMessageBox("tcl thread error!");
//		return FALSE;
//	}

	if(lpszPathName != NULL)
	{
		if(!SetScriptFile(lpszPathName))
			return FALSE;
	}

	m_bMultiDebugKeepSelect = FALSE;
	m_bMultiDebugLoadSource = FALSE;

	if(!m_bIsTkFile && (m_nSupportTk == 1))
	{
		// m_nSupportTk==1��ʾ�����Զ��жϽű��Ƿ�֧��Tk
		if(CheckIsTkFile())
		{
			m_bIsTkFile = TRUE;
/*
			CWnd* pMainFrame = AfxGetMainWnd();
			char buffer[500];
			sprintf(buffer, "-use 0x%8.8x", pMainFrame->m_hWnd);
			Tcl_SetVar(pInterp, "argv", buffer, TCL_GLOBAL_ONLY);
*/
			#ifdef _SUPPORT_TK
			Tk_Init(pInterp);
			Tcl_StaticPackage(pInterp, "Tk", Tk_Init, Tk_SafeInit);
			#endif
		}
	}

	pInterp->errorLine = 0;	// �����������Ϣ
	//nBreakMode = SCRIPT_BREAK_NORMAL;	// ����ȱʡ�Ķϵ�ģʽ
	//nRunMode = SCRIPT_RUN_DEBUG_STEP;

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
	//char szScriptFile[256];
	//strcpy(szScriptFile, m_strScriptFile);

	Tcl_DString dst;
	char* script = Tcl_ExternalToUtfDString(NULL, m_strScriptFile, -1, &dst);

	try {
	////////////////////////////////////////////
	// ִ�нű��ļ�
	if(TCL_OK != Tcl_EvalFile(pInterp, script))
	{
		Tcl_DStringFree(&dst);
		m_nRunState = SCRIPT_STATE_END;
		m_nEvalResult = TCL_ERROR;
		//ConvertResultToUTF(pInterp);
		return FALSE;
	}
	////////////////////////////////////////////
	}catch(...)
	{
		TRACE("RunScriptFile %s exception!\n", m_strScriptFile);
	}

	Tcl_DStringFree(&dst);

	TkMainLoop();

	m_nRunState = SCRIPT_STATE_END;
	m_nEvalResult	= TCL_OK;
	return TRUE;
}

// ִ�нű�����
int __stdcall CTclInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	// TODO: Add your code here
	pInterp->errorLine = 0;	// �����������Ϣ
	//nBreakMode = SCRIPT_BREAK_NORMAL;	// ����ȱʡ�Ķϵ�ģʽ

	try {
	////////////////////////////////////////////

	// �޸���ͨ�ַ���ΪUTF-8�����ַ���
	Tcl_DString dst;
	char* script = Tcl_ExternalToUtfDString(NULL, lpszTclCmd, -1, &dst);
	// ִ�нű�����
	if(TCL_OK != Tcl_Eval(pInterp, script))
	{
		Tcl_DStringFree(&dst);
		m_nEvalResult = TCL_ERROR;
		//ConvertResultToUTF(pInterp);
		return FALSE;
	}

	Tcl_DStringFree(&dst);

	////////////////////////////////////////////
	}catch(...)
	{
		// TRACE������֧��512�ֽ�,�����Ҫ�ض�
		CString strTclCmd = lpszTclCmd;
		if(strTclCmd.GetLength() > 400)
		{
			strTclCmd = strTclCmd.Left(400);
		}
		TRACE("RunScriptCommand '%s' exception!\n", strTclCmd);
	}

	m_nEvalResult = TCL_OK;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ִ�л������е�TCL����
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::RunScriptBuffer()
{
	if(NULL == m_pRunBuffer)
		return FALSE;

	m_bProfileEnable = AfxGetApp()->GetProfileInt(REG_TCL_DEBUG_SUBKEY, REG_TCL_ENABLE_PROFILE, FALSE);

	m_nRunState = SCRIPT_STATE_RUN;

	// �޸���ͨ�ַ���ΪUTF-8�����ַ���
	Tcl_DString dst;
	char* script = Tcl_ExternalToUtfDString(NULL, m_pRunBuffer, -1, &dst);
	// ִ�нű�����
	if(TCL_OK != Tcl_Eval(pInterp, script))
	{
		Tcl_DStringFree(&dst);
		m_nRunState = SCRIPT_STATE_END;
		m_nEvalResult = TCL_ERROR;
		//ConvertResultToUTF(pInterp);

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

		return FALSE;
	}

	Tcl_DStringFree(&dst);

	TkMainLoop();

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
	m_nEvalResult	= TCL_OK;
	return TRUE;
}

// ֹͣ�ű�����
int __stdcall CTclInterp::StopScript()
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
LPVOID __stdcall CTclInterp::GetInterp()
{
	// TODO: Add your code here
	return pInterp;
}

// ��ȡ������ID
int __stdcall CTclInterp::GetInterpID()
{
	// TODO: Add your code here
	return m_nInterpID;
}

// ���ý�������
void __stdcall CTclInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	// TODO: Add your code here
	m_strInterpName = lpszInterptName;

	CString strCmd = "set interpName " + m_strInterpName;
	Tcl_DString dst;
	strCmd = Tcl_ExternalToUtfDString(NULL, strCmd, -1, &dst);
	Tcl_DStringFree(&dst);
	Tcl_Eval(pInterp, strCmd.GetBuffer(0));
	strCmd.ReleaseBuffer();
}

// ��ȡ��������
CString __stdcall CTclInterp::GetInterpName()
{
	// TODO: Add your code here

	return m_strInterpName;
}

// ��ȡ�����к�
int __stdcall CTclInterp::GetErrorLine()
{
	// TODO: Add your code here
	if(TCL_ERROR == m_nEvalResult)
		return pInterp->errorLine;
	else
		return 0;
}

// ��ȡ�����Ϣ
CString __stdcall CTclInterp::GetResult()
{
	// TODO: Add your code here
	// ת��UTF-8����Ϊ��ͨ�ַ���	
	Tcl_DString dst;
	CString strResult = Tcl_UtfToExternalDString(NULL, Tcl_GetStringResult(pInterp), -1, &dst);
	Tcl_DStringFree(&dst);
	return strResult;
}

// ���ñ���ֵ
int __stdcall CTclInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	// TODO: Add your code here
	CString strVarName = lpszVarName;
	CString strVarValue = lpszVarValue;
	char* varName = strVarName.GetBuffer(strVarName.GetLength());
	char* strValue = strVarValue.GetBuffer(strVarValue.GetLength());
	Tcl_SetVar(pInterp, varName, strValue, TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);
	strVarName.ReleaseBuffer();
	strVarValue.ReleaseBuffer();

	return 0;
}

// ��ȡ����ֵ
CString __stdcall CTclInterp::GetVar(LPCTSTR lpszVarName)
{
	// TODO: Add your code here
	CString strVarName = lpszVarName;
	char* varName = strVarName.GetBuffer(strVarName.GetLength());
	// Tcl_GetVar���һ������Ϊ0��ʾ�Ȼ�ȡ�ֲ�����,���û���ٻ�ȡȫ�ֱ���
	CString strVarValue = Tcl_GetVar(pInterp, varName, 0);//TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);
	strVarName.ReleaseBuffer();
	// ת��UTF-8����Ϊ��ͨ�ַ���	
	Tcl_DString dst;
	strVarValue = Tcl_UtfToExternalDString(NULL, strVarValue, -1, &dst);
	Tcl_DStringFree(&dst);
	return strVarValue;
}

// ע����չ����
int __stdcall CTclInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	// TODO: Add your code here
	Tcl_CreateObjCommand((Tcl_Interp*)pInterp, lpszCmd, (Tcl_ObjCmdProc*)lpFunc, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	return 0;
}

// ������չ����
int __stdcall CTclInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	// TODO: Add your code here
	CString strParamName = lpszParamName;
	CString strParamValue = lpszParamValue;

	if(strParamName == "usetk" && strParamValue == "true")
	{
		if(!m_bIsTkFile)
		{
			m_bIsTkFile = TRUE;
/*
			CWnd* pMainFrame = AfxGetMainWnd();
			char buffer[500];
			sprintf(buffer, "-use 0x%8.8x", pMainFrame->m_hWnd);
			Tcl_SetVar(pInterp, "argv", buffer, TCL_GLOBAL_ONLY);
*/
			#ifdef _SUPPORT_TK
			Tk_Init(pInterp);
			Tcl_StaticPackage(pInterp, "Tk", Tk_Init, Tk_SafeInit);
			#endif
		}
	}else
	if(strParamName == "usetkv" && strParamValue == "true")
	{
		if(!m_bIsTkFile)
		{
			m_bIsTkFile = TRUE;

			#ifdef _SUPPORT_TK
			Tk_Init(pInterp);
			Tcl_StaticPackage(pInterp, "Tk", Tk_Init, Tk_SafeInit);
			#endif
		}

		// ����TkView�ĳ�ʼ������
		//m_TclNotifier = Tcl_InitNotifier();
		//Tcl_AlertNotifier(m_TclNotifier);
		Tcl_SetServiceMode(TCL_SERVICE_ALL);

		Tcl_Time timeout;
		timeout.sec = 0;
		timeout.usec = 25000;
		Tcl_SetTimer(&timeout);
	}else
	if(strParamName == "TSRInterp")	// פ����������־
	{
		SetVar(lpszParamName, lpszParamValue);
	}

	return 0;
}

// ��ȡ��չ����
CString __stdcall CTclInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	// TODO: Add your code here
	CString strParamName = lpszParamName;
	if(strParamName == "usetk")
	{
		return m_bIsTkFile ? "1" : "0";
	}else
	if(strParamName == "istkview")
	{
		return CheckIsTkvFile() ? "1" : "0";
	}else
	if(strParamName == "TSRInterp")	// פ����������־
	{
		return GetVar(lpszParamName);
	}

	return "";
}

// ��������ģʽ
int __stdcall CTclInterp::SetRunMode(int nRunMode)
{
	m_nRunMode = nRunMode;
	/*if(SCRIPT_RUN_DEBUG_STEP == nRunMode)
	{
		SetBreakMode(SCRIPT_STEP_NEXT);
	}*/
	return true;
}

// ��ȡ����ģʽ
int __stdcall CTclInterp::GetRunMode()
{
	return m_nRunMode;
}

// ��������ģʽ
int __stdcall CTclInterp::SetRunState(int nState)
{
	m_nRunState = nState;
	return true;
}

// ��ȡ����ģʽ
int __stdcall CTclInterp::GetRunState()
{
	return m_nRunState;
}

// ��ȡ��ǰ��ջ����
int __stdcall CTclInterp::GetCurStackLevel()
{
	// TODO: Add your code here
	char szTclCmd[128];
	// ����ǰ��Ҫ��::,�����ڽ���ĳЩ�ű������Ϊ�����ռ�����⵼��info level�����޷�ʶ��
	sprintf(szTclCmd, "::info level");

	if(TCL_OK != Tcl_Eval(pInterp, szTclCmd))
	{
		return -1;
	}

	return atoi(Tcl_GetStringResult(pInterp));
}

// ��ȡ�ؼ����б�
int __stdcall CTclInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	// TODO: Add your code here
	CString strType = lpszType;
	if(strType == "platform")	// ƽ̨��չ�ؼ���
	{
	}else
	if(strType == "platform_keyword_tooltip")	// ƽ̨��չ�ؼ���ʹ��˵��
	{
	}

	return asKeyWord.GetSize();
}

// ���õ����¼�
int __stdcall CTclInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	// TODO: Add your code here
	CString strParam = lpszParam;
	char szTclCmd[128];

	if(nEvent == IDB_STOP)
	{
		sprintf(szTclCmd, "td -stop");
	}else
	if(nEvent == IDB_NEXT)
	{
		sprintf(szTclCmd, "td -next");
	}else
	if(nEvent == IDB_GOTO)
	{
		if(strParam.GetLength() == 0)
		{
			// �жϵ�
			sprintf(szTclCmd, "td -goto *%d", nParam);
		}else
		{
			// �����ϵ�
			sprintf(szTclCmd, "td -goto #%s", lpszParam);
		}
	}else
	if(nEvent == IDB_STEPINTO)
	{
		sprintf(szTclCmd, "td -step into");
	}else
	if(nEvent == IDB_SETPOUT)
	{
		sprintf(szTclCmd, "td -step return");
	}else
	if(nEvent == IDB_STEP)
	{
		sprintf(szTclCmd, "td -step");
	}else
	if(nEvent == IDB_END)
	{
		return TRUE;
	}

	if(TCL_OK != Tcl_Eval(pInterp, szTclCmd))
	{
		return FALSE;
	}

	return TRUE;
}

// ���öϵ��б�
int __stdcall CTclInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CTclInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CTclInterp::GetEnableProfileAnalyse()
{
	// TODO: Add your code here
	return m_bProfileEnable;
}

// ��ȡ��Ч��������
int __stdcall CTclInterp::GetAvailableLineCount()
{
	// TODO: Add your code here
	return m_nTDLineCount;
}

// ��ȡ�������и��Ǳ�
LPVOID __stdcall CTclInterp::GetProfileMap()
{
	// TODO: Add your code here
	return (void*)(&m_ProfileMap);
}

// �ж�ָ�������Ƿ�֧��
int __stdcall CTclInterp::MethodIsSupport(LPCTSTR lpszMethod)
{
	// TODO: Add your code here
	CString strMethod = lpszMethod;
	if( strMethod == "run" )		// ֧������
	{
		return TRUE;
	}

	if(	(strMethod == "debug") ||	// ֧�ֵ���
		(strMethod == "break") )	// ֧�ֶϵ�
	{
		// ���procomp.exe�ļ������δ��װ����������TclҲ��������е���
		CString strProCompFile	= GetInterpRootPath();
		strProCompFile	+= "bin\\procomp.exe";
		if(GetFileAttributes(strProCompFile) != 0xFFFFFFFF)
		{
			return TRUE;
		}
	}

	if(strMethod == "compile")		// ֧�ֱ���
	{
		// ���procomp.exe�ļ�
		CString strProCompFile	= GetInterpRootPath();
		strProCompFile	+= "bin\\procomp.exe";
		if(GetFileAttributes(strProCompFile) != 0xFFFFFFFF)
		{
			return TRUE;
		}
	}

	if(strMethod == "build")		// ֧�ֹ�����ִ�г���
	{
		// ���tclkit.exe�ļ�
		CString strTclKitFile	= GetInterpRootPath();
		strTclKitFile	+= "bin\\tclkit.exe";
		if(GetFileAttributes(strTclKitFile) != 0xFFFFFFFF)
		{
			return TRUE;
		}
	}

	return FALSE;
}

#define SEPATATOE ","
// ��ʼ������ģʽ
int __stdcall CTclInterp::InitialDebugMode(CString& strResult)
{
	// TODO: Add your code here
	// ��ȡƽ̨�������ű�������
	CString strPlatDebugScript = GetPlatRootPath();
	strPlatDebugScript += _T("lib\\plat\\platdbg.tcl");
	strPlatDebugScript.Replace("\\", "/");
	CString strRes;
	CString strCmd;
	strCmd.Format("source %s", strPlatDebugScript);

	if(!RunCommand(strCmd))
	{
		strResult = GetResult();
		return FALSE;
	}

	// ��ʼ���������Դ���
	CString strWinVar, strWinObj, strWinFunc, strWinStack;
	LOADRCSTRING( strWinVar, IDS_WINDOW_VAR );
	LOADRCSTRING( strWinObj, IDS_WINDOW_OBJ );
	LOADRCSTRING( strWinFunc, IDS_WINDOW_FUNC );
	LOADRCSTRING( strWinStack, IDS_WINDOW_STACK );

	CString strColName, strColValue, strColParam;
	LOADRCSTRING( strColName, IDS_COLUMN_NAME );
	LOADRCSTRING( strColValue, IDS_COLUMN_VALUE );
	LOADRCSTRING( strColParam, IDS_COLUMN_PARAM );

	// ��ʼ����������
	m_nOutputVar = ::atoi(pIPlatUI->OutputGet(-1, strWinVar));
	if(m_nOutputVar <= 0)
	{
		//�´��ڣ�����
		m_nOutputVar = pIPlatUI->AddOutput(strWinVar + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputVar, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputVar, "AddColumn " + strColName + ",150");
	pIPlatUI->OutputSet(m_nOutputVar, "AddColumn " + strColValue + ",150,AutoSize");
	pIPlatUI->OutputSet(m_nOutputVar, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputVar, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputVar, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputVar, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputVar, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputVar = pIPlatUI->AddOutput(strWinVar + "##GRID");

	// ��ʼ�����󴰿�
	m_nOutputObject = ::atoi(pIPlatUI->OutputGet(-1, strWinObj));
	if(m_nOutputObject <= 0)
	{
		//�´��ڣ�����
		m_nOutputObject = pIPlatUI->AddOutput(strWinObj + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputObject, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputObject, "AddColumn " + strColName + ",150");
	pIPlatUI->OutputSet(m_nOutputObject, "AddColumn " + strColValue + ",150,AutoSize");
	pIPlatUI->OutputSet(m_nOutputObject, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputObject, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputObject, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputObject, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputObject, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputObject = pIPlatUI->AddOutput(strWinObj + "##GRID");

	// ��ʼ����������
	m_nOutputFunc = ::atoi(pIPlatUI->OutputGet(-1, strWinFunc));
	if(m_nOutputFunc <= 0)
	{
		//�´��ڣ�����
		m_nOutputFunc = pIPlatUI->AddOutput(strWinFunc + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputFunc, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputFunc, "AddColumn " + strColName + ",300,AutoSize");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputFunc, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputFunc, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputFunc, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputFunc = pIPlatUI->AddOutput(strWinFunc + "##GRID");

	// ��ʼ����ջ����
	m_nOutputStack = ::atoi(pIPlatUI->OutputGet(-1, strWinStack));
	if(m_nOutputStack <= 0)
	{
		//�´��ڣ�����
		m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputStack, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColName + ",200");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColParam + ",100,AutoSize");
	pIPlatUI->OutputSet(m_nOutputStack, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputStack, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputStack, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Add(NoSort)");
	pIPlatUI->OutputSet(m_nOutputStack, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID");

	// �����������
	pIPlatUI->ActiveOutput(m_nOutputVar);

	BOOL bEnableMulitFileDebug = AfxGetApp()->GetProfileInt(REG_TCL_DEBUG_SUBKEY, REG_TCL_ENABLE_MULTIFILEDEBUG, FALSE);
	if(bEnableMulitFileDebug)
	{
		// ����source����,����֧�ֶ��ļ�����
		if(!RunCommand(_T("rename source source_std;rename source_debug source;")))
		{
			strResult = GetResult();
			return FALSE;
		}
	}

	strResult = GetResult();

	return TRUE;
}

// ���µ��Դ�����Ϣ
int __stdcall CTclInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
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
		RunCommand(_T("global _plat_Tcl_WatchVariables;eval $_plat_Tcl_WatchVariables"));
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputVar);
		}
	}
	if(bWindowObject)
	{
		RunCommand(_T("package require Itcl;global _plat_Tcl_WatchObjects;eval $_plat_Tcl_WatchObjects"));
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputObject);
		}
	}
	if(bWindowProc)
	{
		RunCommand(_T("global _plat_Tcl_WatchProcedures;eval $_plat_Tcl_WatchProcedures"));
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputFunc);
		}
	}
	if(bWindowStack)
	{
		RunCommand(_T("global _plat_Tcl_WatchCallStack;eval $_plat_Tcl_WatchCallStack"));
		if(bActive)
		{
			pIPlatUI->ActiveOutput(m_nOutputStack);
		}
	}

	strResult = GetResult();

	return TRUE;
}

// ���ýű�����
int __stdcall CTclInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
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

// ���н��̲��ҽ��Զ�������ܵ�
BOOL CTclInterp::RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId, BOOL bActiveOutput)
{
	m_hPipeProcess = NULL;

	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead,&hWrite,&sa,0))
	{
		AfxMessageBox("Create Pipe process failed!");
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
		AfxMessageBox("Create Pipe process failed!");
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

BOOL CTclInterp::TerminatePipeProcess()
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
BOOL CTclInterp::ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd)
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

/////////////////////////////////////////////////////////////////////////////
// ɾ��һ��ָ����Ŀ¼������Ŀ¼�µ��ļ�
// Return Values
// If the function succeeds, the return value is nonzero.
// If the function fails, the return value is zero. 
// To get extended error information, call GetLastError().
/////////////////////////////////////////////////////////////////////////////
int DeleteDir(LPCTSTR strPath)
{
	CFileFind  fileFind;
	int found=0;
	char charPath[MAX_PATH];
	int iDelete=0;
	//ɾ�������ļ�
	sprintf(charPath,"%s\\*.*",strPath);
	found = fileFind.FindFile(charPath,0);
	if(!found) return 1 ;//Ŀ¼������

	while(found)
	{
	    found = fileFind.FindNextFile();
		if(fileFind.IsDots() )
			continue;
		if(fileFind.IsDirectory() )
		{
			DeleteDir(fileFind.GetFilePath());
			continue;
		}
		//�޸��ļ����ԣ��������ɾ��
		SetFileAttributes(fileFind.GetFilePath(),FILE_ATTRIBUTE_NORMAL);
	
		iDelete = DeleteFile(fileFind.GetFilePath());
		//TRACE("%d--Delete File:%s\r\n",iDelete,fileFind.GetFilePath());
		if(iDelete==0)
			return iDelete;
	
	}
	fileFind.Close();

	//ɾ��Ŀ¼
	iDelete = RemoveDirectory(strPath);
	//TRACE("%d--Delete Dir :%s\r\n",iDelete,strPath);
	return iDelete;
}

/////////////////////////////////////////////////////////////////////////////
// ����Ŀ¼��������Ŀ¼
// excludeΪ�����Ŀ¼
/////////////////////////////////////////////////////////////////////////////
void CopyDirectory(LPCTSTR src,LPCTSTR dst, LPCTSTR exclude)   
{   
	WIN32_FIND_DATA   FindFileData;   
	HANDLE   hFind;   
	char   tmpsrc[256];   
	strcpy(tmpsrc,src);   
	strcat(tmpsrc,"\\*.*");   
	hFind   =   FindFirstFile(tmpsrc,   &FindFileData);   
	if(hFind   ==   INVALID_HANDLE_VALUE)   
		return;

	CreateDirectory(dst,0);

	do   
	{   
		char   newdst[256];   
		strcpy(newdst,dst);   
		if(newdst[strlen(newdst)]!='\\')   
		strcat(newdst,"\\");   
		strcat(newdst,FindFileData.cFileName);   

		char   newsrc[256];   
		strcpy(newsrc,src);   
		if(newsrc[strlen(newsrc)]!='\\')   
		strcat(newsrc,"\\");   
		strcat(newsrc,FindFileData.cFileName);   
		if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   
		{
			CString strFileName = FindFileData.cFileName;
			//if( strcmp(FindFileData.cFileName, ".") != 0 &&
			//	strcmp(FindFileData.cFileName, "..") != 0 &&
			//	strcmp(FindFileData.cFileName, exclude) != 0)
			if( strFileName.CompareNoCase(".") != 0 &&
				strFileName.CompareNoCase("..") != 0 &&
				strFileName.CompareNoCase(exclude) != 0)
			{   
				CopyDirectory(newsrc,newdst,exclude);   
			}   
		}else   
		{
			CString strFileName = newsrc;
			strFileName.MakeLower();
			// ����exe�ļ����������ļ���,����������ļ�Խ��Խ��
			if(strFileName.Find(".exe") != (strFileName.GetLength()-4))
			{
				CopyFile(newsrc,newdst,true);
			}
		}   
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);   
} 

/////////////////////////////////////////////////////////////////////////////
// ���ָ���ű��ļ�����Ҫ�������ٸ���չ��,�ظ��Ĳ���
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::GetPackageRequire(LPCTSTR lpszFile, CStringArray& asPackage)
{
	int nPackage = 0;

	try
	{
		CStdioFile fi(lpszFile, CFile::modeRead);		
		CString strLine;

		while(fi.ReadString(strLine))
		{
			strLine.TrimLeft("\t ");
			strLine.TrimRight("\t ;");

			int nPos = strLine.Find("package require");
			if(nPos != -1)
			{
				strLine.Delete(0, nPos + 15);
				strLine.TrimLeft("\t ");
				nPos = strLine.Find(";");
				if(nPos != -1)
				{
					strLine = strLine.Left(nPos);
					strLine.TrimRight("\t ");
				}
				nPos = strLine.Find("}");
				if(nPos != -1)
				{
					strLine = strLine.Left(nPos);
					strLine.TrimRight("\t ");
				}
				nPos = strLine.Find(" ");
				if(nPos != -1)
				{
					strLine = strLine.Left(nPos);
					strLine.TrimRight("\t ");
				}

				BOOL bFind = FALSE;
				for(int i=0; i<asPackage.GetSize(); i++)
				{
					if(strLine == asPackage[i])
					{
						bFind = TRUE;
						break;
					}
				}
				if(!bFind)
				{
					asPackage.Add(strLine);
					nPackage++;
				}
			}
		}

	}catch(...)
	{
		return nPackage;
	}

	return nPackage;
}

/////////////////////////////////////////////////////////////////////////////
// ���ָ����չ��Ŀ¼���Ƿ�����Ҫ����չ��
/////////////////////////////////////////////////////////////////////////////
BOOL CTclInterp::CheckPackageProvider(LPCTSTR lpszIndexFile, CStringArray& asPackage)
{
	BOOL bFind = FALSE;

	try
	{
		CStdioFile fi(lpszIndexFile, CFile::modeRead);		
		CString strLine;

		while(fi.ReadString(strLine))
		{
			strLine.TrimLeft("\t ");

			if(strLine.Find("package ifneeded") == 0)
			{
				strLine.Delete(0, 16);
				strLine.TrimLeft("\t ");
				int nPos = strLine.Find(" ");
				if(nPos != -1)
				{
					strLine = strLine.Left(nPos);
					strLine.TrimRight("\t ");

					for(int i=0; i<asPackage.GetSize(); i++)
					{
						if(strLine == asPackage[i])
						{
							return TRUE;
						}
					}
				}
			}
		}

	}catch(...)
	{
		return FALSE;
	}

	return FALSE;
}

// ѡ��lib��Ŀ¼
BOOL CTclInterp::SelectLibPath(CStringArray& asLibPath, CStringArray& asPackage)
{
	CTOptArray aopt;	// ����������Ԫ������
	TOPTITEM item;		// ������Ԫ��
	item.hItem		= NULL;
	item.iType		= TOPT_GROUP;
	CString strTemp;
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_TCLLIB_GROUP)
	strTemp = strTemp;//"ѡ��LibĿ¼";
	item.lpszText	= strTemp;
	item.lpszEdit	= "";
	item.lpszList	= "";
	item.bCheck		= FALSE;
	item.iImage		= TOPTI_FOLDER;
	item.iParent	= -1;
	aopt.Add(item);		// ��Ӹ��ڵ�

	item.iParent	= 0;
	item.iType		= TOPT_CHECKBOX;
	item.iImage		= TOPTI_INFO;

	// �ҵ�libĿ¼
	CString strInterpLibPath = GetInterpRootPath();
	if(strInterpLibPath[strInterpLibPath.GetLength()-1] == '\\')
		strInterpLibPath.Delete(strInterpLibPath.GetLength()-1, 1);
	strInterpLibPath += "\\lib";

	// ����lib�µ�������Ŀ¼
	WIN32_FIND_DATA   FindFileData;
	HANDLE   hFind;
	char   tmpsrc[256];
	strcpy(tmpsrc,strInterpLibPath);
	strcat(tmpsrc,"\\*.*");
	hFind   =   FindFirstFile(tmpsrc,   &FindFileData);
	if(hFind   ==   INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if( strcmp(FindFileData.cFileName, ".") != 0 &&
				strcmp(FindFileData.cFileName, "..") != 0)
			{
				item.lpszText = FindFileData.cFileName;
				if(strcmp(FindFileData.cFileName, "tcl8.4") == 0)
				{
					// Ĭ�����tcl8.4Ŀ¼
					item.bCheck = TRUE;
				}else
				if(CheckPackageProvider(strInterpLibPath + "\\" + FindFileData.cFileName + "\\pkgIndex.tcl", asPackage))
				{
					item.bCheck = TRUE;
				}else
				{
					item.bCheck = FALSE;
				}

				aopt.Add(item);
			}
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);

	// ��ӱ������ʾ��Ϣ
	item.iType		= TOPT_MSG;
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_TCLLIB_TITLE);
	item.lpszText	= strTemp;//"ѡ��Tcl Lib��Ŀ¼";
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_TCLLIB_MSG);
	item.lpszEdit	= strTemp;//"��ѡ��Tcl Lib��Ŀ¼��ѡ���Ŀ¼����ӵ����ɵĿ�ִ���ļ���";
	item.iParent	= -1;
	aopt.Add(item);

	int nRet = FALSE;

	try{
		pIPlatUI->SendOwmCmd("", OC_EXECTREEOPTION, 0, (LPARAM)(&aopt), &nRet);
	}catch(...)
	{
		return FALSE;
	}

	if(nRet != IDOK)
	{
		return FALSE;
	}

	for(int i=0; i<aopt.GetSize(); i++)
	{
		if(TOPT_CHECKBOX == aopt[i].iType)
		{
			if(aopt[i].bCheck)
			{
				asLibPath.Add(aopt[i].lpszText);
			}
		}
	}
	
	return TRUE;
}

// ����
int __stdcall CTclInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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
	strProCompFile	+= "bin\\procomp.exe";

	CString strOutFile = strSrcFile;
	strOutFile.MakeLower();
	int nDotPos = strOutFile.ReverseFind('.');
	if(nDotPos != -1)
	{
		strOutFile.Delete(nDotPos, strOutFile.GetLength() - nDotPos);
	}

	// �Զ���������ļ�
	BOOL bAutoOutFile = AfxGetApp()->GetProfileInt(REG_TCL_COMPILE_SUBKEY, REG_TCL_COMPILE_AUTO_OUTFILE, TRUE);
	if(bAutoOutFile)
	{
		strOutFile += ".tbc";
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
	BOOL bForceOverwrite = AfxGetApp()->GetProfileInt(REG_TCL_COMPILE_SUBKEY, REG_TCL_COMPILE_FORCE_OVERWRITE, FALSE);
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

	// �Ƿ�����ļ�ͷǰ׺
	BOOL bAddPrefix = AfxGetApp()->GetProfileInt(REG_TCL_COMPILE_SUBKEY, REG_TCL_COMPILE_ADD_PREFIX, TRUE);
	if(!bAddPrefix)
	{
		strParams += "-prefix none ";
	}

	strParams += "-out \"";
	strParams += strOutFile;
	strParams += "\"";

	strParams += " \"";
	strParams += strSrcFile;
	strParams += "\"";

	strDestFile = strOutFile;

	//return ExecProcess(strProCompFile, strParams, TRUE);

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
int __stdcall CTclInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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

	CString strTclKitFile	= GetInterpRootPath();
	strTclKitFile	+= "bin\\tclkit.exe";

	CString strTclKitFile2	= GetInterpRootPath();

	BOOL bIsTkFile = FALSE;	// �Ƿ�ΪTk�ű�

	// ǿ�ư�Tk�ű�����
	BOOL bForceTkKit = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_FORCE_TKKIT, FALSE);
	if(bForceTkKit || CheckIsTkFile())
	{
		strTclKitFile2	+= "bin\\tclkit2_tk.exe";
		bIsTkFile = TRUE;
	}else
	{
		strTclKitFile2	+= "bin\\tclkit2.exe";
		if(GetFileAttributes(strTclKitFile2) == 0xFFFFFFFF)
		{
			CopyFile(strTclKitFile, strTclKitFile2, TRUE);
		}
	}

	CString strSdxFileBin	= GetInterpRootPath();
	strSdxFileBin	+= "bin\\sdx.kit";
	CString strSdxFile = "sdx.kit";

	CString strOutFile = strSrcFile;
	strOutFile.MakeLower();
	int nDotPos = strOutFile.ReverseFind('.');
	if(nDotPos != -1)
	{
		strOutFile.Delete(nDotPos, strOutFile.GetLength() - nDotPos);
	}

	CString strOutFileName = strOutFile;	// ������׺�������ļ���

	// �Զ���������ļ�
	BOOL bAutoOutFile = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_AUTO_OUTFILE, TRUE);
	if(bAutoOutFile)
	{
		strOutFile += ".exe";
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

	// ��¼��ǰ·��,�����õ�ǰ·��
	char szOldPath[256];
	memset(szOldPath, 0, 256);
	DWORD dwLen = GetCurrentDirectory(255, szOldPath);
	CString strPath = strOutFileName;
	strPath.Replace("/", "\\");
	int nPos = strPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		strPath = strPath.Left(nPos);
	}
	SetCurrentDirectory(strPath);

	// ����sdx.kit�ļ�
	CString strSdxFileDest = strPath + "\\" + strSdxFile;
	CopyFile(strSdxFileBin, strSdxFileDest, TRUE);


	CString strCmdBase = "\"";
	strCmdBase += strSdxFile;
	strCmdBase += "\" ";

	CString strCmd = strCmdBase;

	// 1.����kit�ļ�(tclkit sdx.kit qwrap sourcefile)
	// �ļ�������ڿ�����Ӱ��,�����ɾ��
	CString strKitFileDest = strOutFileName + ".kit";
	DeleteFile(strKitFileDest);

	strCmd += "qwrap \"";
	strCmd += strSrcFile;
	strCmd += "\"";
	//if(!ExecProcess(strTclKitFile, strCmd, TRUE))
	if(!RunProgramWithPipe(strTclKitFile, strCmd))
	{
		DeleteFile(strSdxFileDest);
		DeleteFile(strKitFileDest);
		m_nRunState = SCRIPT_STATE_END;
		return FALSE;
	}

	// 2.�⿪kit�ļ�ΪvsfĿ¼(tclkit sdx.kit unwrap destfile)
	CString strVfsDir = strOutFileName + ".vfs";
	// Ŀ¼������ڿ�����Ӱ��,�����ɾ��
	DeleteDir(strVfsDir);

	strOutFileName.Replace("/", "\\");
	nPos = strOutFileName.ReverseFind('\\');
	if(nPos >= 0)
	{
		strOutFileName.Delete(0, nPos+1);
	}
	strCmd = strCmdBase;
	strCmd += "unwrap \"";
	strCmd += strOutFileName;
	strCmd += ".kit\"";
	//if(!ExecProcess(strTclKitFile, strCmd, TRUE))
	if(!RunProgramWithPipe(strTclKitFile, strCmd))
	{
		if(dwLen > 0)
		{
			CString strOldPath = szOldPath;
			SetCurrentDirectory(strOldPath);
		}

		DeleteFile(strSdxFileDest);
		DeleteFile(strKitFileDest);
		DeleteDir(strVfsDir);

		m_nRunState = SCRIPT_STATE_END;
		return FALSE;
	}

	// 3.��vsf�������չ���ļ�
	CStringArray asPackage;
	GetPackageRequire(strSrcFile, asPackage);

	// 3.1 ���Tcl LibĿ¼
	BOOL bAddTclLib = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_ADD_TCL_LIB, TRUE);
	if(bAddTclLib)
	{
		CStringArray asLibPath;
		if(SelectLibPath(asLibPath, asPackage))
		{
			CString strInterpLibPath = GetInterpRootPath();
			if(strInterpLibPath[strInterpLibPath.GetLength()-1] == '\\')
				strInterpLibPath.Delete(strInterpLibPath.GetLength()-1, 1);
			strInterpLibPath += "\\lib";

			//Copy the source path files to the vfs lib path
			CString strDestPath = strVfsDir + "\\lib";
			for(int i=0; i<asLibPath.GetSize(); i++)
			{
				CopyDirectory(strInterpLibPath + "\\" + asLibPath[i], strDestPath + "\\" + asLibPath[i], "");
			}
		}
	}

	// 3.2 ���Դ����Ŀ¼�����ļ�
	CString strSourcePath = strSrcFile;
	strSourcePath.Replace("/", "\\");
	nPos = strSourcePath.ReverseFind('\\');
	if(nPos != -1)
	{
		strSourcePath = strSourcePath.Left(nPos);
	}
	BOOL bAddSourcePathFiles = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_ADD_SRC_PATHFILES, FALSE);
	if(bAddSourcePathFiles)
	{
		//Copy the source path files to the vfs lib path
		CString strDestPath = strVfsDir + "\\lib\\app-";
		strDestPath += strOutFileName;

		CString strVfsExcludeDir = strOutFileName + ".vfs";
		CopyDirectory(strSourcePath, strDestPath, strVfsExcludeDir);

		// ɾ�����������ļ�
		DeleteFile(strDestPath + "\\sdx.kit");
		DeleteFile(strDestPath + "\\" + strOutFileName + ".kit");
		DeleteDir(strDestPath + "\\lib");
	}

	// 3.3 ���Դ����Ŀ¼libĿ¼�µ�������Ŀ¼��libĿ¼
	BOOL bAddSourceLib = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_ADD_SRC_LIB, TRUE);
	if(bAddSourceLib && (GetFileAttributes(strSourcePath + "\\lib") != 0xFFFFFFFF))
	{
		//Copy the source lib path to the vfs lib path
		CString strDestPath = strVfsDir + "\\lib";
		CopyDirectory(strSourcePath + "\\lib", strDestPath, "");
	}

	// 4.�������յĿ�ִ���ļ�(tclkit sdx.kit wrap destfile -runtime tclkit2.exe)
	strCmd = strCmdBase;
	strCmd += "wrap \"";
	strCmd += strOutFile;
	strCmd += "\" -runtime \"";
	strCmd += strTclKitFile2;
	strCmd += "\"";
	//if(!ExecProcess(strTclKitFile, strCmd, TRUE))
	if(!RunProgramWithPipe(strTclKitFile, strCmd))
	{
		if(dwLen > 0)
		{
			CString strOldPath = szOldPath;
			SetCurrentDirectory(strOldPath);
		}

		DeleteFile(strSdxFileDest);
		DeleteFile(strKitFileDest);
		DeleteDir(strVfsDir);

		m_nRunState = SCRIPT_STATE_END;
		return FALSE;
	}

	// 5.ɾ��vsfĿ¼��kit�ļ�

	// ִ�н���,�ָ���ǰ·��������(add at 2004-09-07)
	if(dwLen > 0)
	{
		CString strOldPath = szOldPath;
		SetCurrentDirectory(strOldPath);
	}

	// ɾ����ʱ�ļ�
	DeleteFile(strSdxFileDest);
	DeleteFile(strKitFileDest);
	DeleteDir(strVfsDir);

	strDestFile = strOutFile;

	// 6.�滻��ִ���ļ���ͼ��(������Tk�ű�)
	BOOL bReplaceIcon = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_REPLACE_ICON, FALSE);
	if(bIsTkFile && bReplaceIcon)
	{
		// Ĭ�ϵ�ͼ���ļ�
		CString strTkExeIcon = GetInterpRootPath();
		strTkExeIcon += "bin\\tkexe.ico";

		// ͼ��ѡ��ķ�ʽ
		CString strNewIcon = "";
		int nSelectIconType = AfxGetApp()->GetProfileInt(REG_TCL_BUILD_SUBKEY, REG_TCL_BUILD_SELECT_ICON_TYPE, 0);
		if(nSelectIconType == 0)	// ʹ��Դ����Ŀ¼�µ�ͬ��ͼ���ļ�
		{
			strNewIcon = strSourcePath + "/" + strOutFileName + ".ico";
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
				m_nRunState = SCRIPT_STATE_END;
				return TRUE;
			}
		}
		if(GetFileAttributes(strNewIcon) == 0xFFFFFFFF)
		{
			m_nRunState = SCRIPT_STATE_END;
			return TRUE;
		}

		// ��ȡ�滻ͼ��ű�������ִ��
		CString strIconReplaceScript = GetInterpRootPath();
		strIconReplaceScript += _T("bin\\iconreplace.tcl");
		strIconReplaceScript.Replace("\\", "/");
		CString strRes;
		CString strCmd;
		strCmd.Format("source %s;set filename {%s};set newicon {%s}; set fwname {%s};IconReplace $filename $newicon $fwname",
			strIconReplaceScript,
			strOutFile,
			strNewIcon,
			strTkExeIcon);

		if(!RunCommand(strCmd))
		{
			CString strResult = GetResult();
			AfxMessageBox(strResult);
			m_nRunState = SCRIPT_STATE_END;
			return TRUE;
		}
	}

	m_nRunState = SCRIPT_STATE_END;

	return TRUE;
}

//}}VCI_IMPLEMENT_END

/////////////////////////////////////////////////////////////////////////////
// ���öϵ㼤���ź�
/////////////////////////////////////////////////////////////////////////////
void CTclInterp::SetBreakSignal(BOOL bBreak)
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
void CTclInterp::DebugCallback(int nEvent, int nLine, CString strFile)
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

// ִ�нű�����(�ڲ�ʹ��)
int CTclInterp::RunCommand(CString strCmd)
{
	Tcl_DString dst;
	strCmd = Tcl_ExternalToUtfDString(NULL, strCmd, -1, &dst);
	Tcl_DStringFree(&dst);
	if(TCL_OK != Tcl_Eval(pInterp, strCmd.GetBuffer(strCmd.GetLength())))
	{
		strCmd.ReleaseBuffer();
		return FALSE;
	}
	strCmd.ReleaseBuffer();

	return TRUE;
}

// ִ�нű�����(�ڲ�ʹ��),֧����ȫ�ֻ�ֲ��ռ����нű�
int CTclInterp::RunCommandEx(CString strCmd, int nFlag)
{
	Tcl_DString dst;
	strCmd = Tcl_ExternalToUtfDString(NULL, strCmd, -1, &dst);
	Tcl_DStringFree(&dst);
	if(TCL_OK != Tcl_EvalEx(pInterp, strCmd.GetBuffer(strCmd.GetLength()), -1, nFlag))
	{
		strCmd.ReleaseBuffer();
		return FALSE;
	}
	strCmd.ReleaseBuffer();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ע����չ����
/////////////////////////////////////////////////////////////////////////////
int CTclInterp::Tcl_RegisterCmd(Tcl_Interp* pInterp)
{
	// ע��ű���������
	Tcl_CreateCommand(pInterp, "td", Tcl_Cmd_Plat_Debug, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ע������õ�source����
	Tcl_CreateCommand(pInterp, "plat_debug_source", Tcl_Cmd_Source, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ע���µ�exit����
	Tcl_CreateCommand(pInterp, "exit", Tcl_Cmd_Exit, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// �Ա�׼puts�������չ
	Tcl_CreateObjCommand(pInterp, "puts", Tcl_PutsObjCmd_Ex, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ��ȡƽ̨��Ϣ
	Tcl_CreateCommand(pInterp, "platform_info", Tcl_Cmd_PlatformInfo, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ��ȡ����򿪵��ļ��б�
	Tcl_CreateCommand(pInterp, "recent-file-list", Tcl_Cmd_RecentFileList, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ��ȡ����򿪵Ĺ����б�
	Tcl_CreateCommand(pInterp, "recent-project-list", Tcl_Cmd_RecentProjectList, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ע��TclVci����
	Tclvci_Init(pInterp);

	// ע��TclXml����
	TclXml_Init(pInterp);

	return TCL_OK;
}

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(TclInterp)
CONTROL_INTERFACE_IMPLEMENT(TclInterp)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CTclInterp::XTclInterp::GetExtraInfo(LPVOID lpRefData)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
//{{VCI_INTERFACE_IMPLEMENT_BEGIN
// ����ƽ̨UI�ӿ�ָ��
int __stdcall CTclInterp::XTclInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetIPlatUI(lpIPlatUI);
}

int __stdcall CTclInterp::XTclInterp::SetLibPath(LPCTSTR lpszPath)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetLibPath(lpszPath);
}

int __stdcall CTclInterp::XTclInterp::IsInterpRunning()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->IsInterpRunning();
}

int __stdcall CTclInterp::XTclInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetScriptFile(lpszPathName);
}

CString __stdcall CTclInterp::XTclInterp::GetScriptFile()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetScriptFile();
}

int __stdcall CTclInterp::XTclInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->RunScriptFile(lpszPathName);
}

int __stdcall CTclInterp::XTclInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->RunScriptCommand(lpszTclCmd);
}

int __stdcall CTclInterp::XTclInterp::StopScript()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->StopScript();
}

LPVOID __stdcall CTclInterp::XTclInterp::GetInterp()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetInterp();
}

int __stdcall CTclInterp::XTclInterp::GetInterpID()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetInterpID();
}

void __stdcall CTclInterp::XTclInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	pObj->SetInterpName(lpszInterptName);
}

CString __stdcall CTclInterp::XTclInterp::GetInterpName()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetInterpName();
}

int __stdcall CTclInterp::XTclInterp::GetErrorLine()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetErrorLine();
}

CString __stdcall CTclInterp::XTclInterp::GetResult()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetResult();
}

int __stdcall CTclInterp::XTclInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetVar(lpszVarName, lpszVarValue);
}

CString __stdcall CTclInterp::XTclInterp::GetVar(LPCTSTR lpszVarName)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetVar(lpszVarName);
}

int __stdcall CTclInterp::XTclInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->RegisterCommand(pInterp, lpszCmd, lpFunc);
}

int __stdcall CTclInterp::XTclInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetInterpParam(lpszParamName, lpszParamValue);
}

CString __stdcall CTclInterp::XTclInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetInterpParam(lpszParamName);
}

int __stdcall CTclInterp::XTclInterp::SetRunMode(int nRunMode)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetRunMode(nRunMode);
}

int __stdcall CTclInterp::XTclInterp::GetRunMode()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetRunMode();
}

int __stdcall CTclInterp::XTclInterp::SetRunState(int nRunState)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetRunState(nRunState);
}

int __stdcall CTclInterp::XTclInterp::GetRunState()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetRunState();
}

int __stdcall CTclInterp::XTclInterp::GetCurStackLevel()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetCurStackLevel();
}

// ��ȡ�ؼ����б�
int __stdcall CTclInterp::XTclInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetKeyWordList(lpszType, asKeyWord);
}

int __stdcall CTclInterp::XTclInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetDebugEvent(nEvent, nParam, lpszParam);
}

int __stdcall CTclInterp::XTclInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->SetBreakList(lpszFile, auBreaks);
}

int __stdcall CTclInterp::XTclInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetBreakList(lpszFile, auBreaks);
}

int __stdcall CTclInterp::XTclInterp::GetEnableProfileAnalyse()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetEnableProfileAnalyse();
}

int __stdcall CTclInterp::XTclInterp::GetAvailableLineCount()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetAvailableLineCount();
}

LPVOID __stdcall CTclInterp::XTclInterp::GetProfileMap()
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->GetProfileMap();
}

int __stdcall CTclInterp::XTclInterp::MethodIsSupport(LPCTSTR lpszMethod)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->MethodIsSupport(lpszMethod);
}

int __stdcall CTclInterp::XTclInterp::InitialDebugMode(CString& strResult)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->InitialDebugMode(strResult);
}

int __stdcall CTclInterp::XTclInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->RefreshDebugWindow(lpszWindow, bActive, strResult);
}

int __stdcall CTclInterp::XTclInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->CallMethod(lpszMethod, asParam);
}

int __stdcall CTclInterp::XTclInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
	return pObj->Compile(lpszSrcFile, strDestFile, lpszParams);
}

int __stdcall CTclInterp::XTclInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CTclInterp *pObj = GET_INTERFACE_OBJECT(TclInterp);
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

#include "stdafx.h"

#include "CPythonInterp.h"

#include "resource.h"
#include "PythonContainer.h"
#include <osdefs.h>
#include "dll_loader.h"

#include <fcntl.h>
#include <Python.h>
#include <frameobject.h>

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

//PyThreadState *CPythonInterp::m_mainInterpreter = 0L;

static CPythonInterp *g_pPythonInterp = NULL;

class CPyThreadState
{
public:
	CPyThreadState(PyThreadState *p) : m_p(p)
	{
		m_p = PyThreadState_Swap(p);
	}
	~CPyThreadState()
	{
		PyThreadState_Swap(m_p);
	}
protected:
	PyThreadState *m_p;
};

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:��������֧�ֺ���
// ����:
// 0.������ID	
// 1.�¼�����(����:USER_CALL,USER_LINE,USER_RETURN,USER_EXCEPTION)
// 2.�ļ���(�ַ���)
// 3.������(�ַ���)
// 4.�к�(����)
// 5.����ֵ����(�ַ���)
// --ȡ��6.����(�б�)
// ����ֵ: �û��������(����,��ֹ,����,�˳�)// 
/////////////////////////////////////////////////////////////////////////////
static PyObject *Python_Debug(PyObject *self, PyObject *args)
{
	int nInterpID;
	int nEventType;
	char *sFileName;
	char *sFuncName;
	int nLineNo;
	char *sResult;

	// ��������
	if(!PyArg_ParseTuple(args, "iissis", &nInterpID, &nEventType, &sFileName, &sFuncName, &nLineNo, &sResult))
		return PyContainer::Void();

	// ��ȡ����������ָ��
	CPythonInterp* pPythonInterp = GetPythonInterp(nInterpID);
	if(pPythonInterp == NULL)
		return PyContainer::Void();

	IPlatUI* pIPlatUI = NULL;
	if(pPythonInterp)
	{
		pIPlatUI = pPythonInterp->pIPlatUI;
	}
	if(pIPlatUI == NULL)
	{
		return PyContainer::Void();
	}

	// �������ͨ����ģʽ,�������˵�������
	if(SCRIPT_RUN_NORMAL == pPythonInterp->GetRunMode())
	{
		return PyContainer::Void();
	}

	// ���ñ����ԵĽű��ļ�
	if(nEventType == PDB_SETFILE)
	{
		pPythonInterp->SetScriptFile(sFileName);
		// ��յ����ļ��б�
		pPythonInterp->ClearDebugFile();
		return PyContainer::Void();
	}

	CString strFileName = sFileName;
	//strFileName.Replace("\\", "/");

	// --(ֻ�ڵ�ǰ���Ե��ļ��и���)
	// 2004-06-25:֧�ֶ��ļ�����,�ļ�δ��,����ô�����
	CString strFile1 = strFileName;
	strFile1.Replace("\\", "/");
	strFile1.MakeLower();
	CString strFile2 = pPythonInterp->GetScriptFile();
	strFile2.Replace("\\", "/");
	strFile2.MakeLower();
	//if(strcmp(strFileName, pPythonInterp->GetScriptFile()) != 0)
	if(strcmp(strFile1, strFile2) != 0)
	{
		//PF_TRACE_EX_ID(1, "$$%s\r\n", sFileName);
		//PF_TRACE_EX_ID(1, ">>%s\r\n", pPythonInterp->GetScriptFile());
		if(strcmp(strFileName, "<string>") == 0)
		{
			return PyContainer::Void();
		}
		// �ļ�δ��,���
		if(pPythonInterp->AddDebugFile(strFileName) != -1)
		{
			// ��OwmEdit���ʹ��ļ�����
			//COM_CMD(OWM_OWMEDIT, OC_OPENFILE, (WPARAM)(LPCTSTR)strFileName, 0);
			pIPlatUI->OpenDocumentFile(strFileName, "");

			// ��OwmEdit������������ģʽΪ�ӵ���ģʽ������
			//COM_CMD(OWM_OWMEDIT, OC_SETSLAVEDEBUG, (WPARAM)(LPCTSTR)strFileName, TRUE);
			pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG, (WPARAM)(LPTSTR)(LPCTSTR)strFileName, 1, NULL);
		}
	}

	CString strBreakName = "";
	int nOperate = 0;	// �û�����

	//CString str;
	//str.Format("InterpName=%s,InterpID=%d,EventType=%d,FileName=%s,Func=%s,LineNo=%d,Result=%s\r\n", 
	//	pPythonInterp->GetInterpName(), nInterpID, 
	//	nEventType, sFileName, sFuncName, nLineNo, sResult);
	//PF_OUTPUT_DIRECT_ID(1, str);
	
	TRY
	{
		if(nEventType == PDB_STOP)	// ��ֹ
		{
			pPythonInterp->StopScript();
			return PyContainer::Void();
		}else
		if(nEventType == PDB_NEXT)	// ִ�е���һ���ϵ�
		{
			pPythonInterp->SetBreakMode(SCRIPT_BREAK_NORMAL);
			pPythonInterp->SetBreakSignal(TRUE);	// ����ű�
			pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
			return PyContainer::Void();
		}else
		if(nEventType == PDB_GOTO)	// ִ�е�ĳ���жϵ�������ϵ�
		{
			pPythonInterp->SetBreakMode(SCRIPT_BREAK_NORMAL);
			pPythonInterp->SetBreakSignal(TRUE);	// ����ű�
			pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
			return PyContainer::Void();
		}else
		if(nEventType == PDB_STEPINTO)	// �������뺯��		
		{
			pPythonInterp->SetBreakMode(SCRIPT_STEP_INTO);
			pPythonInterp->SetBreakSignal(TRUE);	// ����ű�
			pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_STEP);	// ����Ϊ����ģʽ
			return PyContainer::Void();
		}else
		if(nEventType == PDB_SETPOUT)	// �Ӻ�������
		{
			pPythonInterp->SetBreakMode(SCRIPT_STEP_RETURN);
			pPythonInterp->SetBreakSignal(TRUE);	// ����ű�
			pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_STEP);	// ����Ϊ����ģʽ
			return PyContainer::Void();
		}else
		if(nEventType == PDB_STEP)	// ���������뺯��
		{
			pPythonInterp->SetBreakMode(SCRIPT_STEP_NEXT);
			pPythonInterp->SetBreakSignal(TRUE);	// ����ű�
			pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_STEP);	// ����Ϊ����ģʽ
			return PyContainer::Void();
		}else
		if(nEventType == PDB_END)	// ֪ͨ�������ļ����Խ���,����ִ���ƺ���
		{
			CStringArray* pasScript = pPythonInterp->GetDebugScriptList();
			for(int i=0; i<pasScript->GetSize(); i++)
			{
				CString strFileName = pasScript->GetAt(i);
				// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
				//COM_CMD(OWM_OWMEDIT, OC_SETSLAVEDEBUG, (WPARAM)(LPCTSTR)strFileName, FALSE);
				pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG, (WPARAM)(LPTSTR)(LPCTSTR)strFileName, 0, NULL);
			}
			return PyContainer::Void();
		}

		// ���ű���ֹ�ź�,�ж��Ƿ�����ű�����
		if(pPythonInterp->GetStopSignal())
		{
			// ��OwmEdit���͵�ǰ�����������Ϣ
			//OWM_LINKAGE* pOwmEdit = theApp.GetOWM(OWM_OWMEDIT);
			//if(pOwmEdit)
			//{
			//	CString strScriptFile = pPythonInterp->GetScriptFile();
			//	pOwmEdit->CommandOWM(OC_SETCURENTEXECLINE, (WPARAM)(LPCTSTR)strScriptFile, 0);
			//}
			pPythonInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0);
			pPythonInterp->SetStopSignal(FALSE);
			pPythonInterp->SetRunState(SCRIPT_STATE_STOP);
			PyRun_SimpleString("_platform.dbg.set_quit()");
			return PyContainer::Void();
		}

		// �ϵ�����ģʽ
		if(SCRIPT_RUN_DEBUG_GO == pPythonInterp->GetRunMode())
		{
			// ���ϵ��б�,���Ƿ���ĳ���ϵ���
			int nRet = pPythonInterp->RunScriptCommand("[KeepState]_platform.write(0, '%d' % _platform.dbg.break_here(_platform.dbg.curframe), 18, 0)");
			if(nRet != -1)
			{
				// ����'1'��ʾ�˴��Ƕϵ���
				if("0" == pPythonInterp->GetResult())
				{	// ��ͨ�ϵ�����������,����һ���ϵ�ִ��
					pPythonInterp->RunScriptCommand("[KeepState]_platform.dbg.set_continue()");
					pPythonInterp->SetBreakMode(SCRIPT_BREAK_NORMAL);
					pPythonInterp->SetRunMode(SCRIPT_RUN_DEBUG_GO);	// ����Ϊ�ϵ�ģʽ
					return PyContainer::Void();
				}
			}
			// ����ǰ�ϵ��кű�����ֵ
			pPythonInterp->SetBreakLineNo(nLineNo);
		}else
		if(SCRIPT_RUN_DEBUG_STEP == pPythonInterp->GetRunMode())	// ����
		{
		}

		// ��OwmEdit���Ͷϵ��к���Ϣ
		//OWM_LINKAGE* pOwmEdit = theApp.GetOWM(OWM_OWMEDIT);
		//if(pOwmEdit)
		//{
		//	pOwmEdit->CommandOWM(OC_SETCURENTEXECLINE, (WPARAM)(LPCTSTR)strFileName, nLineNo);
		//}
		pPythonInterp->DebugCallback(IDB_USER_SETCURRENTLINE, nLineNo);

		pPythonInterp->SetRunState(SCRIPT_STATE_BREAK);

		// ����ˢ�±�������Ϣ��python�ű�����
		// �ж��Ƿ�ʵʱ���¸��ֵ�����Ϣ
		// ���±����б�
		BOOL bPythonDebugRTVar = AfxGetApp()->GetProfileInt(REG_PY_DEBUG_SUBKEY, REG_PY_DEBUG_RTVAR, FALSE);
		if(bPythonDebugRTVar)
		{
			pPythonInterp->RunScriptCommand("[KeepState]_platform.dbg.refresh_vars(_platform.dbg.curframe)");
		}

		// ���¶�ջ�б�
		BOOL bPythonDebugRTStack = AfxGetApp()->GetProfileInt(REG_PY_DEBUG_SUBKEY, REG_PY_DEBUG_RTSTACK, TRUE);
		if(bPythonDebugRTStack)
		{
			pPythonInterp->RunScriptCommand("[KeepState]_platform.dbg.refresh_stack()");
		}

		// ������Ϣѭ��
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
			if(pPythonInterp->GetStopSignal())
			{
				// ��OwmEdit���͵�ǰ�����������Ϣ
				//OWM_LINKAGE* pOwmEdit = theApp.GetOWM(OWM_OWMEDIT);
				//if(pOwmEdit)
				//{
				//	pOwmEdit->CommandOWM(OC_SETCURENTEXECLINE, (WPARAM)(LPCTSTR)strFileName, 0);
				//}
				pPythonInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0);
				pPythonInterp->SetStopSignal(FALSE);
				pPythonInterp->SetBreakSignal(FALSE);
				pPythonInterp->SetRunState(SCRIPT_STATE_STOP);
				PyRun_SimpleString("_platform.dbg.set_quit()");
				return PyContainer::Void();
			}

			// ���ϵ㼤���ź�
			if(pPythonInterp->GetBreakSignal())
			{
				pPythonInterp->SetBreakSignal(FALSE);
				break;
			}
		}

		// ��OwmEdit���͵�ǰ�����������Ϣ
		//if(pOwmEdit)
		//{
		//	pOwmEdit->CommandOWM(OC_SETCURENTEXECLINE, (WPARAM)(LPCTSTR)strFileName, 0);
		//}
		pPythonInterp->DebugCallback(IDB_USER_SETCURRENTLINE, 0);
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return PyContainer::Void();
	}
	END_CATCH_ALL

	return Py_BuildValue("i", nOperate);
}

/////////////////////////////////////////////////////////////////////////////
// ���������Ϣ
/////////////////////////////////////////////////////////////////////////////
static CString g_strErrorInfo = "";
void plat_err(char* str)
{
	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	// ��ȡ��׼�������ID
	int nOT = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_OUTPUT_ID, 2);

	if(pIPlatUI)
	{
		pIPlatUI->OutputSet(nOT, "COLOR=255,0,0");	// ���������ɫ
		pIPlatUI->Output(nOT, str);
		g_strErrorInfo += str;
		if(nOT == 2)
			pIPlatUI->OutputSet(nOT, "0,0,255");
		else
			pIPlatUI->OutputSet(nOT, "0,0,0");
	}
}

CString GetOutputIdByName(CString strName)
{
	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}
	if(pIPlatUI == NULL)
	{
		return "";
	}

	if((strName.Find("<") == 0) && (strName.Right(1) == '>'))
	{
		strName.Delete(0, 1);
		strName = strName.Left(strName.GetLength()-1);
		CString strId = "-1";
		strId = pIPlatUI->OutputGet(-1, strName);
		return strId;
	}
	return "";
}

// �����������
enum
{
	_stdout_style = 15,
	_stderr_style = 16,
	_trace_style = 17,
	_interval_style,		// ƽ̨�ڲ������Ϣ
};

static CString g_strStdOut = "";
static CString g_strInterval = "";
// ע:�µĵ�������״̬�õ��ǹܵ��ķ�ʽ���б�׼����ʹ�������������plat_write�Ⱥ������ᱻ�õ���
/////////////////////////////////////////////////////////////////////////////
// Python��չ����:��ƽ̨���������Ϣ
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_write(PyObject *self, PyObject *args)
{
	int nOT;
	char *str;
	int style;
	PyObject *shell;

	if(!PyArg_ParseTuple(args, "isiO", &nOT, &str, &style, &shell))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	TRY
	{
		if(pIPlatUI)
		{
			if(style == _stdout_style)
			{
				pIPlatUI->OutputSet(nOT, "COLOR=0,0,255");	// ���������ɫ
				pIPlatUI->Output(nOT, str);

				// �Ƿ�������ű���׼���
				BOOL bPythonDirectResult = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_RESULT, TRUE);
				if(bPythonDirectResult)
				{
					g_strStdOut += str;
				}
			}
			else
			if(style == _stderr_style)
			{
				plat_err(str);
				//int nOT = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_OUTPUT_ID, 2);	// ��Ϣ����
				//pIPlatUI->OutputFormat(nOT, 2, str);
			}else
			if(style == _trace_style)
			{
				// ��ȡ��׼�������ID
				int nOT = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_OUTPUT_ID, 2);

				pIPlatUI->OutputSet(nOT, "COLOR=125,0,255");	// ���������ɫ
				pIPlatUI->Output(nOT, str);
			}else
			if(style == _interval_style)
			{
				g_strInterval += str;
			}
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:��ȡ�������ID
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_getwndid(PyObject *self, PyObject *args)
{
	char *sot;

	if(!PyArg_ParseTuple(args, "s", &sot))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	// ��ȡ�������ID
	int nOT = 0;
	CString strOT = sot;
	if(strOT.Find('-') == 0)
	{
		strOT.Delete(0, 1);
		nOT = atoi(strOT);
	}else
	{
		nOT = atoi(GetOutputIdByName(strOT));
	}

	return Py_BuildValue("i", nOT);
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:�����������
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_create(PyObject *self, PyObject *args)
{
	char *str;
	int  nID = 0;

	if(!PyArg_ParseTuple(args, "s|i", &str, &nID))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	CString strOT = str;

	TRY
	{
		if(pIPlatUI)
		{
			nID = pIPlatUI->AddOutput(strOT);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return Py_BuildValue("i", nID);
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:�ر��������
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_close(PyObject *self, PyObject *args)
{
	int  nOT = 0;

	if(!PyArg_ParseTuple(args, "i", &nOT))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	TRY
	{
		if(pIPlatUI)
		{
			pIPlatUI->DelOutput(nOT);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:�����������
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_active(PyObject *self, PyObject *args)
{
	int  nOT = 0;

	if(!PyArg_ParseTuple(args, "i", &nOT))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	TRY
	{
		if(pIPlatUI)
		{
			pIPlatUI->ActiveOutput(nOT);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:����������
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_clear(PyObject *self, PyObject *args)
{
	int  nOT = 0;

	if(!PyArg_ParseTuple(args, "i", &nOT))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	TRY
	{
		if(pIPlatUI)
		{
			pIPlatUI->OutputClear(nOT);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:����������ڸ�ʽ
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_format(PyObject *self, PyObject *args)
{
	int  nOT = 0;
	char *str;

	if(!PyArg_ParseTuple(args, "is", &nOT, &str))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	TRY
	{
		if(pIPlatUI)
		{
			pIPlatUI->OutputSet(nOT, str);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:��ȡ���������Ϣ
/////////////////////////////////////////////////////////////////////////////
static PyObject *plat_output_getinfo(PyObject *self, PyObject *args)
{
	int  nOT = 0;
	char *str;

	if(!PyArg_ParseTuple(args, "is", &nOT, &str))
		return NULL;

	IPlatUI* pIPlatUI = NULL;
	if(g_pPythonInterp)
	{
		pIPlatUI = g_pPythonInterp->pIPlatUI;
	}

	CString strInfo;

	TRY
	{
		if(pIPlatUI)
		{
			strInfo = pIPlatUI->OutputGet(nOT, str);
		}
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL

	return Py_BuildValue("s", strInfo);
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:ִ��Tcl������������
/////////////////////////////////////////////////////////////////////////////
static PyObject *TclInterp_Execute(PyObject *self, PyObject *args)
{
/*	PyTclInterp* pPyTclInterp = NULL;
	char *str;

	if(!PyArg_ParseTuple(args, "is", &pPyTclInterp, &str))
		return NULL;

	TRY
	{
		if(pPyTclInterp)
			return pPyTclInterp->Execute(args);
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL
*/
	return PyContainer::Void();
}

/////////////////////////////////////////////////////////////////////////////
// Python��չ����:ִ��Tcl���������ļ�
/////////////////////////////////////////////////////////////////////////////
static PyObject *TclInterp_RunScriptFile(PyObject *self, PyObject *args)
{
/*	PyTclInterp* pPyTclInterp = NULL;
	char *str;

	if(!PyArg_ParseTuple(args, "is", &pPyTclInterp, &str))
		return NULL;

	TRY
	{
		if(pPyTclInterp)
			return pPyTclInterp->RunScriptFile(args);
	}
	CATCH_ALL(e)
	{
		PyHandlePlatError(e);
		return NULL;
	}
	END_CATCH_ALL
*/
	return PyContainer::Void();
}

static PyMethodDef plat_methods[] =
{
	{"pydbg", (PyCFunction)Python_Debug, METH_VARARGS, "python interactivce debug function"},
	{"write", (PyCFunction)plat_write, METH_VARARGS, ""},
	{"GetOutputWndID", (PyCFunction)plat_output_getwndid, METH_VARARGS, ""},
	{"CreateOutputWnd", (PyCFunction)plat_output_create, METH_VARARGS, ""},
	{"CloseOutputWnd", (PyCFunction)plat_output_close, METH_VARARGS, ""},
	{"ActiveOutputWnd", (PyCFunction)plat_output_active, METH_VARARGS, ""},
	{"ClearOutputWnd", (PyCFunction)plat_output_clear, METH_VARARGS, ""},
	{"FormatOutputWnd", (PyCFunction)plat_output_format, METH_VARARGS, ""},
	{"GetOutputWndInfo", (PyCFunction)plat_output_getinfo, METH_VARARGS, ""},
	{"TclInterpExecute", (PyCFunction)TclInterp_Execute, METH_VARARGS, ""},
	{"TclInterpRunScriptFile", (PyCFunction)TclInterp_RunScriptFile, METH_VARARGS, ""},
	{NULL, NULL}
};

static std::vector<PyMethodDef> gAllMethods;


// ����_platformģ�����ָ��
static PyObject* InitializeInterp(PyThreadState *interp)//, CString strInterpName, int nInterpID, int pInterp)
{
	//delete by lcmao
	//CPyThreadState lock(interp);
	//end delete

	// first time, create the method table for cvsgui module
	if(gAllMethods.empty())
	{
		PyMethodDef *tmp = plat_methods;
		while(tmp->ml_name)
		{
			gAllMethods.push_back(*tmp);
			tmp++;
		}

		PyRegistry::RegisterAllObjects(gAllMethods);
	}

	// this module contains the glue code functions
	PyObject *m = Py_InitModule4("_platform", &gAllMethods[0],
		"The platform glue functions", (PyObject*)NULL,PYTHON_API_VERSION);
	
	if(m == NULL)
	{
		plat_err("Could not create the Python gui module !\n");
		return NULL;
	}

	//ע�ᵽbuiltins
	if(PyDict_SetItemString(interp->interp->builtins, "_platform", m))
	{
		::AfxMessageBox("ע��ʧ��");
	}

	// this is the exception we raised when the error comes from us
	PyObject *d = PyModule_GetDict(m);
	if(gPlatErr == NULL)
		gPlatErr = PyErr_NewException("_platform.error", NULL, NULL);
	if (gPlatErr != NULL)
		PyDict_SetItemString(d, "error", gPlatErr);

	PyRegistry::CallInitValues(d);

	// ƽ̨�汾
	CString strPlatPath = GetPlatRootPath();
	if(strPlatPath[strPlatPath.GetLength()-1] == '\\')
		strPlatPath.Delete(strPlatPath.GetLength()-1, 1);
#ifdef _DEBUG
	CString strPlatType = "debug";
#else
	CString strPlatType = "release";
#endif

	// Initial platform var
	PyObject *platpath = PyString_FromString(strPlatPath);
	if (platpath != NULL) {
		PyDict_SetItemString(d, "platform_path", platpath);
		Py_DECREF(platpath);
	}

	PyObject *plattype = PyString_FromString(strPlatType);
	if (plattype != NULL) {
		PyDict_SetItemString(d, "platform_type", plattype);
		Py_DECREF(plattype);
	}

	// ���õ�ǰ�������ID����
	int nOT = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_OUTPUT_ID, 2);
	PyObject *pyOT = PyInt_FromLong(nOT);
	if (pyOT != NULL) {
		PyObject_SetAttrString(m, "_outputwnd", pyOT);
		Py_DECREF(pyOT);
	}
/*
	// ���õ�ǰ��������
	PyObject *pyInterpName = PyString_FromString(strInterpName);
	if (pyInterpName != NULL) {
		PyObject_SetAttrString(m, "interpname", pyInterpName);
		Py_DecRef(pyInterpName);
	}

	// ���õ�ǰ������ID����
	PyObject *pyInterpID = PyInt_FromLong(nInterpID);
	if (pyInterpID != NULL) {
		PyObject_SetAttrString(m, "interpid", pyInterpID);
		Py_DecRef(pyInterpID);
	}

	// ���õ�ǰ����������ָ��
	PyObject *pyInterp = PyInt_FromLong(pInterp);
	if (pyInterp != NULL) {
		PyObject_SetAttrString(m, "thisinterp", pyInterp);
		Py_DecRef(pyInterp);
	}
*/

/*	char *script = NULL;

	HINSTANCE hInst = ::AfxGetInstanceHandle();//GetDllInstance();
	HRSRC rsrc = FindResource(hInst, "PYSHELL", "PYSCRIPT");

	if(rsrc)
	{
		HGLOBAL h = LoadResource(hInst, rsrc);
		if(h)
		{
			DWORD size = SizeofResource(hInst, rsrc);
			script = (char *)malloc(size + 1);
			char *ptr = (char *)LockResource(h);

			if(script)
			{
				char *tmp = script;
				while(size--)
				{
					char c = *ptr++;
					if(c == '\r')
						continue;
					*tmp++ = c;
				}
				*tmp = '\0';
			}

			FreeResource(h);
		}
	}
*/
	// augment the library path of python with our library
	/*CString libPath = "";
	BOOL bUseExteriorPythonLib = AfxGetApp()->GetProfileInt(REG_PY_LIB_SUBKEY, REG_PY_USE_EXTERIOR_LIB, FALSE);
	if(bUseExteriorPythonLib)
	{
		libPath = Py_GetPath();
		libPath += DELIM;
	}

	// ��ע����·���ַ���(����ʹ��ƽ̨·����)
	CString strLibPath = AfxGetApp()->GetProfileString(REG_PY_LIB_SUBKEY, REG_PY_LIB_PATH, PYTHON_LIB_PATH);
	strLibPath.Replace("INTERP://", GetInterpRootPath() + PYTHON_HOME_PATH);
	strLibPath.Replace("\\\\", "\\");
	libPath += strLibPath;

	PySys_SetPath((char *)(const char *)libPath);*/
/*
	// this is the script to setup stdout/stderr and reroute it
	// to the console window
	if(script)
	{
		PyRun_SimpleString(script);

		free(script);
	}
	else
	{
		plat_err("Could not initialize properly the Python interpreter !\n");
	}
*/
	/*CString strShellPyFile = GetInterpRootPath();
	strShellPyFile += "\\plat\\mfcshell.py";
	FILE *f = fopen(strShellPyFile, "r");
	if(f)
	{
		int nResult = PyRun_SimpleFile(f, (char*)(LPCTSTR)strShellPyFile);
		fclose(f);
	}*/

	return m;
}

//////////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////////
static CPtrList	g_PythonInterpList;	// �����������б�
static int g_InterpID = 0x100;	// ������ID������

// ��ȡPython�������б�ָ��
CPtrList* GetPythonInterpList()
{
	return &g_PythonInterpList;
}

// ��һ��������
int OpenPythonInterp(LPCTSTR lpszInterptName)
{
	if(!CPythonInterp::IsAvail())
	{
		return 0;
	}

	// �����Ƿ��Ѿ�����
	if(lpszInterptName != NULL && lpszInterptName != "")
	{
		int count = g_PythonInterpList.GetCount();
		for(int i = 0; i < count; i ++)
		{
			POSITION pos = g_PythonInterpList.FindIndex(i);
			CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
			if(lpszInterptName == pPythonInterp->GetInterpName()) 
			{
				//pPythonInterp->AddRef();	// �������ü���
				return pPythonInterp->GetInterpID();
			}
		}
	}
	// ���������½�
	CPythonInterp* pPythonInterp = new CPythonInterp();
	if(pPythonInterp)
	{
		pPythonInterp->SetInterpName(lpszInterptName);
		return pPythonInterp->GetInterpID();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ���������ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CPythonInterp* GetPythonInterp(LPCTSTR lpszInterptName)
{
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(lpszInterptName == pPythonInterp->GetInterpName())
		{
			return pPythonInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ�����ID��ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CPythonInterp* GetPythonInterp(int nInterpID)
{
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(nInterpID == pPythonInterp->GetInterpID())
		{
			return pPythonInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// ���ݽ�����ָ���ȡ����������ָ��
//////////////////////////////////////////////////////////////////////
CPythonInterp* GetPythonInterp(LPVOID lpPythonInterp)
{
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(lpPythonInterp == pPythonInterp->GetInterp())
		{
			return pPythonInterp;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// �رս�����(���ݽ�������)
//////////////////////////////////////////////////////////////////////
BOOL ClosePythonInterp(LPCTSTR lpszInterptName)
{
	if(NULL == lpszInterptName)
		return FALSE;

	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(lpszInterptName == pPythonInterp->GetInterpName())
		{
			//if(pPythonInterp->ReleaseRef() == 0)	// ���ü�����0��ɾ������
			//	delete pPythonInterp;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// �رս�����(���ݽ�����ID)
//////////////////////////////////////////////////////////////////////
BOOL ClosePythonInterp(int nInterpID)
{
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(nInterpID == pPythonInterp->GetInterpID())
		{
			//if(pPythonInterp->ReleaseRef() == 0)	// ���ü�����0��ɾ������
			//	delete pPythonInterp;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ɾ������δɾ���Ľ�����(ƽ̨�ر�ʱ����)
//////////////////////////////////////////////////////////////////////
void CloseAllPythonInterp()
{
	// ɾ�������������б��е���
	int count = g_PythonInterpList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		delete pPythonInterp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CPythonInterp::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
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
int CPythonInterp::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
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
int __stdcall CPythonInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	pIPlatUI = (IPlatUI*)lpIPlatUI;

	// �������Ա���
	CString strCurrentLanguage = _T("english");
	if(VCI_LANGUAGE_CHINESE == GetLanguage())
	{
		strCurrentLanguage = _T("chinese");
	}

	//add by lcmao
	//���õ�ǰIPlatUI,�������
	m_pCurPlatUI = pIPlatUI;
	//end add

	// TODO: Add your code here

	return 0;
}

// ���ÿ�·��
int __stdcall CPythonInterp::SetLibPath(LPCTSTR lpszPath)
{
	// TODO: Add your code here

	return 0;
}

// �������Ƿ�������
int __stdcall CPythonInterp::IsInterpRunning()
{
	// TODO: Add your code here
	// �ж����й�λ�Ƿ��Ѿ���ֹ
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		int nRunState = pPythonInterp->GetRunState();
		if( (SCRIPT_STATE_RUN == nRunState) ||
			(SCRIPT_STATE_BREAK == nRunState) ||
			(SCRIPT_STATE_STEP == nRunState) )
			return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////
//add by lmcao
#	define PyFrame_LocalsToFast GLUE_WRAP(PyFrame_LocalsToFast)
#	define PyFrame_FastToLocals GLUE_WRAP(PyFrame_FastToLocals)
////////end add
/////////////////////////////////////////////////////////////

#if !qCvsDebug
#	define DEC_GLUE(f) (*f##_glue)
#	define VAR_GLUE(f) f##_glue

	PyThreadState * DEC_GLUE(PyThreadState_Swap) (PyThreadState *);
	int DEC_GLUE(PyDict_SetItemString) (PyObject *dp, char *key, PyObject *item);
	void  DEC_GLUE(PyErr_Clear) (void);
	PyObject * DEC_GLUE(PyInt_FromLong) (long);
	void DEC_GLUE(PyErr_SetString) (PyObject *, const char *);
	int DEC_GLUE(PyArg_ParseTuple) (PyObject *, char *, ...);
	PyObject * DEC_GLUE(PyTuple_New) (int size);
	PyObject * DEC_GLUE(Py_BuildValue) (char *, ...);
	FILE * DEC_GLUE(PyFile_AsFile) (PyObject *);
	char * DEC_GLUE(PyString_AsString) (PyObject *);
	int DEC_GLUE(PyTuple_Size) (PyObject *);
	int DEC_GLUE(PyList_Size) (PyObject *);
	int DEC_GLUE(PyRun_SimpleString) (char *);
	int DEC_GLUE(PyRun_SimpleFile) (FILE *, char *);
	void DEC_GLUE(Py_SetProgramName) (char *);
	void DEC_GLUE(Py_Initialize) (void);
	void DEC_GLUE(Py_Finalize) (void);
	int DEC_GLUE(Py_IsInitialized) (void);
	PyThreadState * DEC_GLUE(Py_NewInterpreter) (void);
	void DEC_GLUE(Py_EndInterpreter) (PyThreadState *);
	PyObject * DEC_GLUE(PyString_FromString) (const char *);
	PyObject * DEC_GLUE(PyErr_NewException) (char *name, PyObject *base, PyObject *dict);
	PyObject * DEC_GLUE(PyModule_GetDict) (PyObject *);
	PyObject * DEC_GLUE(Py_InitModule4) (char *name, PyMethodDef *methods, char *doc, PyObject *self, int apiver);
	void DEC_GLUE(PyEval_InitThreads) (void);
	PyObject * DEC_GLUE(Py_FindMethod) (PyMethodDef[], PyObject *, char *);
	PyObject * DEC_GLUE(PyList_GetItem) (PyObject *, int);
	int DEC_GLUE(PyType_IsSubtype) (PyTypeObject *, PyTypeObject *);
	PyObject * DEC_GLUE(PyDict_GetItemString) (PyObject *dp, const char *key);
	PyObject * DEC_GLUE(PyImport_ImportModule) (char *name);
	PyObject * DEC_GLUE(_PyObject_New) (PyTypeObject *);
#	if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 3
	void DEC_GLUE(_PyObject_Del) (PyObject *);
#	else
	void DEC_GLUE(PyObject_Free) (void *);
#	endif
	void DEC_GLUE(PySys_SetPath) (char *);
	char * DEC_GLUE(Py_GetPath) (void);
	///////////////add by lcmao
	void DEC_GLUE(Py_SetPythonHome) (char *);
	void DEC_GLUE(PyEval_AcquireLock) (void);
	void DEC_GLUE(PyEval_ReleaseLock) (void);
	void DEC_GLUE(PySys_SetArgv) (int argc, char **argv);
	PyObject* DEC_GLUE(PyFile_FromFile)(FILE *fp, char *name, char *mode, int (*close)(FILE*));
	void DEC_GLUE(PyEval_SetProfile)( Py_tracefunc func, PyObject *obj);
	void DEC_GLUE(PyEval_SetTrace)( Py_tracefunc func, PyObject *obj);
	PyObject* DEC_GLUE(PyLong_FromLong)( long v);
	long DEC_GLUE(PyLong_AsLong)( PyObject *pylong);
	void DEC_GLUE(Py_DecRef)(PyObject *object);
	PyObject* DEC_GLUE(PyObject_GetAttr)(PyObject *o, PyObject *attr_name);
	int (*PySys_SetObject)(char *, PyObject *);
	PyObject * VAR_GLUE(PyExc_SystemExit);
	PyObject* DEC_GLUE(PyObject_Str)( PyObject *o);
	int DEC_GLUE(PyDict_Next)( PyObject *p, Py_ssize_t *ppos, PyObject **pkey, PyObject **pvalue);
	void DEC_GLUE(PyFrame_LocalsToFast)( PyFrameObject *frame, int unknow);
	void DEC_GLUE(PyFrame_FastToLocals)( PyFrameObject *frame);
	///////////////end add

	//////////////////////////////////////////
	PyObject * DEC_GLUE(PyObject_GetAttrString) (PyObject *, char *);
	int DEC_GLUE(PyObject_SetAttrString) (PyObject *, char *, PyObject *);
	int DEC_GLUE(PyObject_HasAttrString) (PyObject *, char *);
	//////////////////////////////////////////

	PyTypeObject * VAR_GLUE(PyType_Type);
	PyTypeObject * VAR_GLUE(PyFile_Type);
	PyTypeObject * VAR_GLUE(PyList_Type);
	PyTypeObject * VAR_GLUE(PyString_Type);
	PyTypeObject * VAR_GLUE(PyTuple_Type);
	PyTypeObject * VAR_GLUE(PyInt_Type);
	PyObject * VAR_GLUE(_Py_NoneStruct);
	PyObject * VAR_GLUE(_Py_ZeroStruct);
	PyObject * VAR_GLUE(_Py_TrueStruct);

	PyObject *GetPyNone() { return VAR_GLUE(_Py_NoneStruct); }
	PyTypeObject *GetPyType_Type() { return VAR_GLUE(PyType_Type); }
#endif

/*!
	Check whether Python is available and load if not
	\param forceReload Force reload of Python library
	\return true if Python is available, false otherwise
*/
bool CPythonInterp::IsAvail(const bool forceReload /*= false*/)
{
#	if !qCvsDebug
	static bool firstTime = true;
	static bool pyAvail = false;

	if( firstTime || forceReload )
	{
		static CompatConnectID connID = 0L;

		if( connID )
		{
			if( CompatCloseLibrary(&connID) )
			{
				connID = 0L;
				pyAvail = false;
			}
		}

		if( !pyAvail )
		{
			CString strPyhonPath = GetInterpRootPath();
			strPyhonPath += "bin\\";
			pyAvail = CompatLoadLibrary(&connID, strPyhonPath + "python27.dll") ||
				CompatLoadLibrary(&connID, strPyhonPath + "python26.dll") ||
				CompatLoadLibrary(&connID, strPyhonPath + "python25.dll") ||
				CompatLoadLibrary(&connID, strPyhonPath + "python24.dll") || 
				CompatLoadLibrary(&connID, strPyhonPath + "python23.dll") || 
				CompatLoadLibrary(&connID, strPyhonPath + "python22.dll");
		}

#	define LOAD_CODEFRAG(name) \
		if( pyAvail ) \
			pyAvail = (*(void **)&name##_glue = \
				_CompatCallLibrary(connID, #name)) != 0L

		LOAD_CODEFRAG(PyThreadState_Swap);
		LOAD_CODEFRAG(PyDict_SetItemString);
		LOAD_CODEFRAG(PyErr_Clear);
		LOAD_CODEFRAG(PyInt_FromLong);
		LOAD_CODEFRAG(PyErr_SetString);
		LOAD_CODEFRAG(PyArg_ParseTuple);
		LOAD_CODEFRAG(PyTuple_New);
		LOAD_CODEFRAG(Py_BuildValue);
		LOAD_CODEFRAG(PyFile_AsFile);
		LOAD_CODEFRAG(PyString_AsString);
		LOAD_CODEFRAG(PyTuple_Size);
		LOAD_CODEFRAG(PyList_Size);
		LOAD_CODEFRAG(PyRun_SimpleString);
		LOAD_CODEFRAG(PyRun_SimpleFile);
		LOAD_CODEFRAG(Py_SetProgramName);
		LOAD_CODEFRAG(Py_Initialize);
		LOAD_CODEFRAG(Py_Finalize);
		LOAD_CODEFRAG(Py_IsInitialized);
		LOAD_CODEFRAG(Py_NewInterpreter);
		LOAD_CODEFRAG(Py_EndInterpreter);
		LOAD_CODEFRAG(PyString_FromString);
		LOAD_CODEFRAG(PyErr_NewException);
		LOAD_CODEFRAG(PyModule_GetDict);
		LOAD_CODEFRAG(Py_InitModule4);
		LOAD_CODEFRAG(PyEval_InitThreads);
		LOAD_CODEFRAG(Py_FindMethod);
		LOAD_CODEFRAG(PyList_GetItem);
		LOAD_CODEFRAG(PyType_IsSubtype);
		LOAD_CODEFRAG(PyDict_GetItemString);
		LOAD_CODEFRAG(PyImport_ImportModule);
		LOAD_CODEFRAG(_PyObject_New);
#	if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 3
		LOAD_CODEFRAG(_PyObject_Del);
#	else
		LOAD_CODEFRAG(PyObject_Free);
#	endif
		LOAD_CODEFRAG(PySys_SetPath);
		LOAD_CODEFRAG(Py_GetPath);

		//////////////////////////////////////////
		LOAD_CODEFRAG(PyObject_GetAttrString);
		LOAD_CODEFRAG(PyObject_SetAttrString);
		LOAD_CODEFRAG(PyObject_HasAttrString);
		//////////////////////////////////////////

		LOAD_CODEFRAG(PyType_Type);
		LOAD_CODEFRAG(PyFile_Type);
		LOAD_CODEFRAG(PyString_Type);
		LOAD_CODEFRAG(PyTuple_Type);
		LOAD_CODEFRAG(PyList_Type);
		LOAD_CODEFRAG(PyInt_Type);
		LOAD_CODEFRAG(_Py_NoneStruct);
		LOAD_CODEFRAG(_Py_ZeroStruct);
		LOAD_CODEFRAG(_Py_TrueStruct);

		///////////////////////////add by lcmao
		LOAD_CODEFRAG(PySys_SetArgv);
		LOAD_CODEFRAG(PyEval_AcquireLock);
		LOAD_CODEFRAG(PyEval_ReleaseLock);
		LOAD_CODEFRAG(PyFile_FromFile);
		LOAD_CODEFRAG(PySys_SetObject);
		LOAD_CODEFRAG(PyEval_SetProfile);
		LOAD_CODEFRAG(PyEval_SetTrace);
		LOAD_CODEFRAG(PyLong_FromLong);
		LOAD_CODEFRAG(PyLong_AsLong);
		LOAD_CODEFRAG(Py_DecRef);
		LOAD_CODEFRAG(PyObject_GetAttr);
		LOAD_CODEFRAG(PyExc_SystemExit);
		LOAD_CODEFRAG(PyObject_Str);
		LOAD_CODEFRAG(PyDict_Next);
		LOAD_CODEFRAG(PyFrame_LocalsToFast);
		LOAD_CODEFRAG(PyFrame_FastToLocals);
		*((void **)&Py_SetPythonHome_glue) = _CompatCallLibrary(connID, "Py_SetPythonHome");
		///////////////////////////end add

		if( !pyAvail )
		{
			AfxMessageBox("Not found Python library!");
		}

		firstTime = false;
	}

	return pyAvail;
#	else
	return true;
#	endif
}


//add by lcmao
CString CPythonInterp::m_strPythonHome = ""; //python����Ŀ¼
HANDLE CPythonInterp::m_hStdoutPipeRead = NULL;
HANDLE CPythonInterp::m_hStdoutPipeWrite = NULL;
IPlatUI*	CPythonInterp::m_pCurPlatUI = NULL;
PyObject*	CPythonInterp::m_stdout = NULL;
HANDLE CPythonInterp::m_hStderrPipeRead = NULL;
HANDLE CPythonInterp::m_hStderrPipeWrite = NULL;
PyObject*	CPythonInterp::m_stderr = NULL;

UINT __cdecl CPythonInterp::RedirectOuputThread(LPVOID parameter)
{
	char buffer[2048];
	DWORD bytesRead;
	CString str;
	while(ReadFile(m_hStdoutPipeRead, buffer, 2047, &bytesRead, NULL))
	{
		buffer[bytesRead] = 0;
		str = buffer;
		//::AfxMessageBox(str);
		//����׼���
		if(m_pCurPlatUI)
		{
			//1 ΪOUtPut
			m_pCurPlatUI->Output(1, str);
		}
	}
	::AfxMessageBox("read pipe failed, thread exit");
	return 0;
}

UINT __cdecl CPythonInterp::RedirectErrorThread(LPVOID parameter)
{
	char buffer[2048];
	DWORD bytesRead;
	CString str;

	// ��ȡ��׼�������ID
	int nOT = AfxGetApp()->GetProfileInt(REG_PY_DIRECT_SUBKEY, REG_PY_DIRECT_STDOUT_OUTPUT_ID, 2);

	while(ReadFile(m_hStderrPipeRead, buffer, 2047, &bytesRead, NULL))
	{
		buffer[bytesRead] = 0;
		str = buffer;
		//::AfxMessageBox(str);
		g_strErrorInfo += str;
		//����׼���
		if(m_pCurPlatUI)
		{
			//1 ΪOUtPut
			m_pCurPlatUI->OutputSet(nOT, "COLOR=255,0,0");
			m_pCurPlatUI->Output(nOT, str);
			m_pCurPlatUI->OutputSet(nOT, "COLOR=0,0,0");
		}
	}
	::AfxMessageBox("read pipe failed, thread exit");
	return 0;
}

PyObject* CPythonInterp::GetPythonAttr(PyObject* obj, char* attrName)
{
	PyObject* attrOName = PyString_FromString(attrName);
	PyObject* attr = PyObject_GetAttr(obj, attrOName);
	Py_DecRef(attrOName);
	return attr;
}

int CPythonInterp::PythonTrace(PyFrameObject *frame, int what, PyObject *arg)
{
	// ��ͨ����ģʽ����������return
	if(SCRIPT_RUN_NORMAL == m_nRunMode)
	{
		//�ж��Ƿ�ֹͣ���У��ǵĻ�������ֹ�쳣������ֱ�ӷ���
		if(m_nCurCommand == IDB_STOP)
		{
			return -1;
		}
		return 0;
	}

	//ÿ��what��������忴python�ĵ���PyTrace_LINE��ʾִ��һ��
	switch(what)
	{
	case PyTrace_CALL:
		break;
	case PyTrace_EXCEPTION:
		break;
	case PyTrace_LINE:
		/*The value passed as the what parameter to a trace function 
		(but not a profiling function) when a line-number event is being reported. */
		return PyTraceLine(frame);
		break;
	case PyTrace_RETURN:
		break;
	case PyTrace_C_CALL:
		break;
	case PyTrace_C_RETURN:
		break;
	default:
		//����Ӧ����Զ���ᵽ�����default��Ϊ�˴���������
		break;
	}

	return 0;
}

/**
**�ϵ㴦��
**һ������¶ϵ��ֱ��return
**��������0���쳣���ط�0
*/
int CPythonInterp::PyTraceLine(PyFrameObject *frame)
{	
	//��ȡ�ļ�
	CString fileName = PyString_AsString(frame->f_code->co_filename);
	//���ļ�ʹ�ã�����ֱ��ִ���ַ������ļ�����"<string>"��ϵͳ�ļ���������'<'����
	if(fileName.Find('<') >= 0)  // 
	{
		return 0;
	}
	int line = frame->f_lineno;
	//�жϵ�ǰģʽ
	if(m_nCurCommand == IDB_STEPINTO) //�������뺯����ÿ����ͣ
	{
		return BreakHear(frame, GetStackLevel(frame), fileName, line);
	}
	else
	{
		if(m_nCurCommand == IDB_STEP) //�����������뺯������ͬһ֡����һ������ֹͣ
		{
			int stackLev = GetStackLevel(frame);
			if(m_nStackLevel == stackLev)
			{
				return BreakHear(frame, stackLev, fileName, line);
			}
		}
		
		//�ж��Ƿ������õĶϵ�λ�ã��ǵĻ��ϵ㣬��ʱֻ֧�ֵ��ļ��ϵ�
		CString file1 = fileName;
		file1.Replace('\\', '/');
		CString file2 = m_strScriptFile;
		file2.Replace('\\', '/');
		if(file1 == file2)
		{
			for(int i=0; i<m_auBreakList.GetSize(); i++)
			{
				if(line == m_auBreakList.GetAt(i))
				{
					return BreakHear(frame, GetStackLevel(frame), fileName, line);
				}
			}
		}

		if(m_nCurCommand == IDB_SETPOUT) //�˳����������ص���һ��
		{
			int stackLev = GetStackLevel(frame);
			if(stackLev < m_nStackLevel)
			{
				return BreakHear(frame, stackLev, fileName, line);
			}
		}
		else if(m_nCurCommand == IDB_GOTO) //ִ�е�ָ����
		{
			if((line == GetBreakLineNo())&&(file1 == file2))  //��ʱֻ֧�ֱ��ļ�goto
			{
				return BreakHear(frame, GetStackLevel(frame), fileName, line);
			}
		}
	}
	return 0;
}

/**
**  �ϵ㴦����
** frame  ��ǰ֡
** stackLevel ��ǰ��ջ����
** fileName ��ǰ�ϵ����ڵ��ļ�
** line     ��ǰ�ϵ����ڵ���
**��������0���쳣���ط�0
*/
int CPythonInterp::BreakHear(PyFrameObject *frame, int stackLevel, CString& fileName, int line)
{
	//�ͷ�����ʹ����python�����������
	PyEval_ReleaseLock();
	// �����ջ�������ṹ��û�����ֵ����Ҫ����
	m_nStackLevel = stackLevel;

	m_currentframe = frame;

	//���ļ�
	//pIPlatUI->OpenDocumentFile(fileName, "");
	//�����Ƿ���Ҫ�����ļ�
	CString file1 = fileName;
	file1.Replace('\\', '/');
	CString file2 = m_strScriptFile;
	file2.Replace('\\', '/');
	if(file1 != file2)
	{
		if(FindDebugFile(fileName) < 0)
		{
			//����ļ�
			if(AddDebugFile(fileName) >= 0)
			{
				pIPlatUI->OpenDocumentFile(fileName, "");
			}
			//��ӵ����ļ�ʧ�ܣ������жϵ�
			else
			{
				//TODO
				//���»�ȡ��
				PyEval_AcquireLock();
				PyThreadState_Swap(frame->f_tstate);
				return 0;
			}
		}
		// ��OwmEdit������������ģʽΪ�ӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)fileName, 1, NULL);
	}
	else
	{
		// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
		pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG,
			(WPARAM)(LPTSTR)(LPCTSTR)fileName, 0, NULL);
	}
	
	//���õ�ǰ״̬Ϊ�ж�״̬
	SetRunState(SCRIPT_STATE_BREAK);
	//���õ�ǰ��
	if(m_lpfnDebug)
	{
		m_lpfnDebug(IDB_USER_SETCURRENTLINE, fileName, line);
	}

	//���������¼�
	::ResetEvent(m_hDebugEvent);
	
	//�ȴ���������
	::WaitForSingleObject(m_hDebugEvent, INFINITE);

	// ���õ�ǰ״̬Ϊ����״̬
	SetRunState(SCRIPT_STATE_RUN);
	// ��Edit���͵�ǰ���������
	if(m_lpfnDebug)
	{
		m_lpfnDebug(IDB_USER_SETCURRENTLINE, fileName, 0);
	}

	//���»�ȡ��
	PyEval_AcquireLock();
	PyThreadState_Swap(frame->f_tstate);

	//�ж��Ƿ�ָֹͣ��ǵĻ������˳�
	if(m_nCurCommand == IDB_STOP)
	{
		return -1;
	}
	return 0;
}

void CPythonInterp::OutPutVars(int outputWin, PyFrameObject *frame, BOOLEAN acquireLock)
{
	if(frame == NULL)
	{
		//����Ӧ����Զ���ᵽ��Ŷԣ�����Ϊ����߿ɿ���
		return;
	}
	//���»�ȡ��
	if(acquireLock)
	{
		PyEval_AcquireLock();
		PyThreadState_Swap(frame->f_tstate);
	}

	PyObject *key, *value, *strKey, *strValue;
	Py_ssize_t pos = 0;
	CString strOutput;
	if(frame->f_locals)
	{
		while (PyDict_Next(frame->f_locals, &pos, &key, &value)) 
		{
			//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
			strKey = PyObject_Str(key);
			strValue = PyObject_Str(value);
			strOutput.Format("<ROOT,4>%s<<&@&>>%s<<&@&>>%s", PyString_AsString(strKey),
				PyString_AsString(strValue), value->ob_type->tp_name);
			Py_DecRef(strKey);
			Py_DecRef(strValue);
			pIPlatUI->Output(outputWin, strOutput);
		}
	}
	pIPlatUI->Output(outputWin," <<&@&>> <<&@&>> ");
	pos = 0;
	if(frame->f_globals)
	{
		while (PyDict_Next(frame->f_globals, &pos, &key, &value)) 
		{
			//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
			strKey = PyObject_Str(key);
			strValue = PyObject_Str(value);
			strOutput.Format("<ROOT,5>%s<<&@&>>%s<<&@&>>%s", PyString_AsString(strKey),
				PyString_AsString(strValue), value->ob_type->tp_name);
			Py_DecRef(strKey);
			Py_DecRef(strValue);
			pIPlatUI->Output(outputWin, strOutput);
		}
	}

	if(acquireLock)
	{
		PyEval_ReleaseLock();
	}
}

int CPythonInterp::GetStackLevel(PyFrameObject *frame)
{
	if(frame == NULL)
	{
		return 0;
	}
	int stackLevel = 1;
	while(frame->f_back)
	{
		frame = frame->f_back;
		stackLevel++;
	}
	/*frame = CPythonInterp::GetPythonAttr(frame);
	while(frame)
	{
		//��ɾ�����ã����ﲻ�ᵼ�¶��󱻴ݻ٣��϶��������ط������������
		Py_DecRef(frame);
		frame = CPythonInterp::GetPythonAttr(frame, f_back);
		stackLevel++;
	}*/
	return stackLevel;
}

int __cdecl CPythonInterp::MyPy_tracefunc(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
	if(obj != NULL)
	{
		return ((CPythonInterp*)PyLong_AsLong(obj))->PythonTrace(frame, what, arg);
	}
	return 0;
}

//end by lcmao

void CPythonInterp::InitPythonSupport(void)
{
	if(!CPythonInterp::IsAvail())
		return;

	/////////////////////edit by lcmao
	//���û�г�ʼ�����Ž��г�ʼ��
	if (!Py_IsInitialized())
	{
		Py_SetProgramName("platform");
		//��ʼ��home·��
		CPythonInterp::m_strPythonHome = GetInterpRootPath() + PYTHON_HOME_PATH;
		if(Py_SetPythonHome)
		{
			Py_SetPythonHome(CPythonInterp::m_strPythonHome.LockBuffer());
		}
		Py_Initialize();
		//��ʼ�����߳�֧�֣�PyEval_InitThreads���ȡȫ����������Ҫ�ͷ���
		PyEval_InitThreads();
		PyEval_ReleaseLock();

		//�����ܵ�
		SECURITY_ATTRIBUTES sa;		
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&CPythonInterp::m_hStdoutPipeRead,&CPythonInterp::m_hStdoutPipeWrite,&sa,0))
		{
			DWORD dwError = ::GetLastError();
			CString str;
			str.Format("Failed to create pipe, errorcode:%8x", dwError); 
			AfxMessageBox(str);
			CPythonInterp::m_hStdoutPipeRead = CPythonInterp::m_hStdoutPipeWrite = NULL;
		}
		else
		{
			//�����ļ�
			int (*my_open_osfhandle)(HANDLE osfhandle,int flags);
			*((void**)(&my_open_osfhandle)) = GetFuncAddress("_open_osfhandle");
			if(my_open_osfhandle != NULL)
			{
				int tmp = my_open_osfhandle(m_hStdoutPipeWrite, _O_WRONLY);
				if(tmp != -1)
				{
					FILE * (*my_fdopen)(int fd, const char *mode);
					*((void**)(&my_fdopen)) = GetFuncAddress("_fdopen");
					if(my_fdopen)
					{
						FILE* file = my_fdopen(tmp, "w");
						if(file)
						{
							m_stdout = PyFile_FromFile(file, "<stdout>", "w", (int (*)(FILE*))GetFuncAddress("fflush"));
							if(m_stdout)
							{
								int (*mysetvbuf)(FILE *stream,char *buffer, int mode, size_t size);
								*((void**)(&mysetvbuf)) = GetFuncAddress("setvbuf");
								mysetvbuf(file, NULL, _IONBF, 0);
								//�����߳������ض������
								::AfxBeginThread(CPythonInterp::RedirectOuputThread, NULL);
							}
						}
					}
				}
			}
		}
		
		if (!CreatePipe(&CPythonInterp::m_hStderrPipeRead,&CPythonInterp::m_hStderrPipeWrite,&sa,0))
		{
			DWORD dwError = ::GetLastError();
			CString str;
			str.Format("Failed to create pipe, errorcode:%8x", dwError); 
			AfxMessageBox(str);
			CPythonInterp::m_hStderrPipeRead = CPythonInterp::m_hStderrPipeWrite = NULL;
		}
		else
		{
			//�����ļ�
			int (*my_open_osfhandle)(HANDLE osfhandle,int flags);
			*((void**)(&my_open_osfhandle)) = GetFuncAddress("_open_osfhandle");
			if(my_open_osfhandle != NULL)
			{
				int tmp = my_open_osfhandle(m_hStderrPipeWrite, _O_WRONLY);
				if(tmp != -1)
				{
					FILE * (*my_fdopen)(int fd, const char *mode);
					*((void**)(&my_fdopen)) = GetFuncAddress("_fdopen");
					if(my_fdopen)
					{
						FILE* file = my_fdopen(tmp, "w");
						if(file)
						{
							m_stderr = PyFile_FromFile(file, "<stderr>", "w", (int (*)(FILE*))GetFuncAddress("fflush"));
							if(m_stderr)
							{
								int (*mysetvbuf)(FILE *stream,char *buffer, int mode, size_t size);
								*((void**)(&mysetvbuf)) = GetFuncAddress("setvbuf");
								mysetvbuf(file, NULL, _IONBF, 0);
								//�����߳������ض������
								::AfxBeginThread(CPythonInterp::RedirectErrorThread, NULL);
							}
						}
					}
				}
			}
		}
		
	//}
	////end edit
	
	//delete by lcmao
	//m_mainInterpreter = PyThreadState_Swap(NULL);
	
	PyEval_AcquireLock();
	PyThreadState* ts = PyThreadState_Swap(NULL);
	PyThreadState_Swap(ts);
	InitializeInterp(ts);//, m_strInterpName, m_nInterpID, (int)this);
	PyEval_ReleaseLock();
	//end delete

	}
}

void CPythonInterp::ClosePythonSupport(void)
{
	if(!CPythonInterp::IsAvail())
		return;

	//PyThreadState_Swap(m_mainInterpreter);
	
	Py_Finalize();

	//m_mainInterpreter = 0L;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�����ļ���������,�����ж��ļ��Ƿ��Ѿ�����
/////////////////////////////////////////////////////////////////////////////
int CPythonInterp::FindDebugFile(CString strFileName)
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
int CPythonInterp::AddDebugFile(CString strFileName)
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
BOOL CPythonInterp::ClearDebugFile()
{
	m_asDebugScript.RemoveAll();
	return TRUE;
}

// ���ýű��ļ���
int __stdcall CPythonInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	m_strScriptFile = lpszPathName;

	return TRUE;
}

// ��ȡ�ű��ļ���
CString __stdcall CPythonInterp::GetScriptFile()
{
	// TODO: Add your code here

	return m_strScriptFile;
}

// ���н��̲��ҽ��Զ�������ܵ�
BOOL CPythonInterp::RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId, BOOL bActiveOutput)
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
BOOL CPythonInterp::TerminatePipeProcess()
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
BOOL CPythonInterp::ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd)
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

void* CPythonInterp::GetFuncAddress(const char* funcName)
{
	CString configFile = GetInterpRootPath() + "conf.ini";
	char ret[200];
	int len = ::GetPrivateProfileString("CONF", "msvcrt", "", ret, 199, configFile);
	ret[len] = 0;
	//��ȡfopen��λ��
	HMODULE module = ::LoadLibrary(ret);//::GetModuleHandle(ret);
	if(module == NULL)
	{
		CString str;
		str.Format("Can not find %s module in process", ret);
		::AfxMessageBox(str);
		return NULL;
	}
	void* func = ::GetProcAddress(module, funcName);
	if(func == NULL)
	{
		CString str;
		str.Format("Can not find function %s in module %s", funcName, ret);
		::AfxMessageBox(str);
		return NULL;
	}
	return func;
}

FILE* CPythonInterp::OpenFile(const char* fileName, const char* mode)
{
	FILE* (*MyOpenFile)(const char* , const char* );
	*((void**)&MyOpenFile) = GetFuncAddress("fopen");
	if(MyOpenFile == NULL)
	{
		return NULL;
	}
	return MyOpenFile(fileName, mode);
}

int CPythonInterp::CloseFile(FILE* file)
{
	int (*MyCloseFile)(FILE*);
	*((void**)&MyCloseFile) = GetFuncAddress("fclose");
	if(MyCloseFile == NULL)
	{
		return -1;
	}
	return MyCloseFile(file);
}


int CPythonInterp::RunScriptCommandWithNewInt(LPCTSTR lpszCmd)
{
	//��ȡ�������õ�ǰ�߳�
	PyEval_AcquireLock();
	
	//�����µĽ�����������ת����ǰ�߳�
	PyThreadState* tmpTS = Py_NewInterpreter();
	//���ôν�������Ӧ�����̣߳����ض�����
	if(CPythonInterp::m_stdout != NULL)
	{
		PySys_SetObject("stdout", m_stdout);
		PySys_SetObject("__stdout__", m_stdout);
	}
	if(CPythonInterp::m_stderr != NULL)
	{
		PySys_SetObject("stderr", m_stderr);
		PySys_SetObject("__stderr__", m_stderr);
	}
	if(tmpTS == NULL)
	{
		::AfxMessageBox(_T("Can not Crate Interpreter"));
		PyEval_ReleaseLock();
		return -1;
	}
	
	//�������в���
	char* argv[] = {(char*)""};
	PySys_SetArgv(1, argv);

	//////////////////////////////////////
	//��ʼ������������ʵ����ʹ_platformģ����Ч
	InitializeInterp(tmpTS);
	
	strErrorInfo = "";
	g_strErrorInfo = "";
	g_strStdOut = "";
	g_strInterval = "";

	// PyRun_SimpleString Returns 0 on success or -1 if an exception was raised. 
	int nResult = PyRun_SimpleString((char *)lpszCmd);

	if(nResult == -1)
	{
		strErrorInfo = g_strErrorInfo;
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\n') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\r') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
	}else
	{
		strStdOut = g_strStdOut;
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\n') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\r') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);

		strIntervalOut = g_strInterval;
	}
	
	PyInterpreterState *interp = tmpTS->interp;
	if((tmpTS->next == NULL)&&(tmpTS == interp->tstate_head))
	{
		Py_EndInterpreter(tmpTS);
	}
	else
	{
		//�����߳�û���˳������ٽ������Ļ��������쳣
	}

	tmpTS = NULL;
	
	//�����꣬�ͷ���
	PyEval_ReleaseLock();
	
	//m_pCurPlatUI->Output(1, "RunScriptCommand only support when state is break\r\n");
	return nResult;
}

// ���нű��ļ�
int __stdcall CPythonInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	// TODO: Add your code here
	if(lpszPathName != NULL)
	{
		if(!SetScriptFile(lpszPathName))
			return FALSE;
	}
	/*if(m_myTS == NULL)
	{
		return FALSE;
	}*/

	FILE* f = OpenFile(lpszPathName, "r");
	if(f)
	{
		m_dwRunThreadId = ::GetCurrentThreadId();
		m_nRunState = SCRIPT_STATE_RUN;

		CString strFile = m_strScriptFile;
		strFile.Replace("\\", "/");

		//����ǵ���ģʽ��������ģʽ������ô���������¼�
		if(m_nRunMode != SCRIPT_RUN_NORMAL)
		{
			static int eventNum = 0;
			CString eventName;
			eventName.Format("PythonDebug%d", eventNum);
			m_hDebugEvent = ::CreateEvent(NULL, FALSE, FALSE, eventName);
			eventNum++;
			if(!m_hDebugEvent)
			{
				::AfxMessageBox("Create Debug Event Failed, Can not debug");
				CloseFile(f);
				m_nRunState = SCRIPT_STATE_END;
				return FALSE;
			}
		}

		//��ȡ�������õ�ǰ�߳�
		PyEval_AcquireLock();
		
		//ԭ������һ�����������Ƚ������٣����������µĽ��������������ڴ�й©
		if(m_myTS != NULL)
		{
			PyInterpreterState *interp = m_myTS->interp;
			if((m_myTS->next == NULL)&&(m_myTS == interp->tstate_head))
			{
				Py_EndInterpreter(m_myTS);
			}
			else
			{
				//�����߳�û���˳������ٽ������Ļ��������쳣
			}
		}
		
		//�����µĽ�������python���߳�ִ�н���ת����ǰ�̣߳������µ��߳��������л���ȥȫ�ֵ��߳�������
		m_myTS = Py_NewInterpreter();
		//���ô˽�������Ӧ�����̣߳����ض�����
		if(CPythonInterp::m_stdout != NULL)
		{
			PySys_SetObject("stdout", m_stdout);
			PySys_SetObject("__stdout__", m_stdout);
		}
		if(CPythonInterp::m_stderr != NULL)
		{
			PySys_SetObject("stderr", m_stderr);
			PySys_SetObject("__stderr__", m_stderr);
		}
		if(m_myTS == NULL)
		{
			::AfxMessageBox(_T("Can not Crate Interpreter"));
			PyEval_ReleaseLock();
			m_nRunState = SCRIPT_STATE_END;
			return FALSE;
		}
		//PyThreadState_Swap(m_myTS);

		//ע��ص�/���Ը��ٽӿ�
		m_debugArg = PyLong_FromLong((long)this);
		PyEval_SetProfile(CPythonInterp::MyPy_tracefunc, m_debugArg);
		PyEval_SetTrace(CPythonInterp::MyPy_tracefunc, m_debugArg);

		//�������в���
		char* argv[] = {(char*)lpszPathName};
		PySys_SetArgv(1, argv);

		//////////////////////////////////////
		//��ʼ������������ʵ����ʹ_platformģ����Ч
		//InitializeInterp(m_myTS);

		strErrorInfo = "";
		g_strErrorInfo = "";
		g_strStdOut = "";
		g_strInterval = "";

		int nResult = PyRun_SimpleFile(f, (char*)(LPCTSTR)strFile);
		
		PyInterpreterState *interp = m_myTS->interp;
		if((m_myTS->next == NULL)&&(m_myTS == interp->tstate_head))
		{
			Py_EndInterpreter(m_myTS);
		}
		else
		{
			//�����߳�û���˳������ٽ������Ļ��������쳣
		}
		m_myTS = NULL;

		//�����꣬�ͷ���
		PyEval_ReleaseLock();
		CloseFile(f);
		
	/*CString str;
	str.Format("return code:%d", nResult);
	::AfxMessageBox(str);*/
	

		if(nResult == -1)
		{
			strErrorInfo = g_strErrorInfo;
			if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\n') )
				strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
			if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\r') )
				strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
		}else
		{
			strStdOut = g_strStdOut;
			if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\n') )
				strStdOut.Delete(strStdOut.GetLength()-1, 1);
			if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\r') )
				strStdOut.Delete(strStdOut.GetLength()-1, 1);

			strIntervalOut = g_strInterval;
		}

		m_nRunState = SCRIPT_STATE_END;
		m_dwRunThreadId = NULL;

		//�����debug�ļ�
		for(int i=0; i<m_asDebugScript.GetSize(); i++)
		{
			CString strFileName = m_asDebugScript.GetAt(i);
			// ��OwmEdit������������ģʽΪ�Ǵӵ���ģʽ������
			//COM_CMD(OWM_OWMEDIT, OC_SETSLAVEDEBUG, (WPARAM)(LPCTSTR)strFileName, FALSE);
			pIPlatUI->SendOwmCmd("org.owm.edit", OC_SETSLAVEDEBUG, (WPARAM)(LPTSTR)(LPCTSTR)strFileName, -1, NULL);
			//������ļ��ĵ��Լ�ͷ
			if(m_lpfnDebug)
			{
				m_lpfnDebug(IDB_USER_SETCURRENTLINE, strFileName, 0);
			}
		}
		ClearDebugFile();

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

		return nResult;
	}

	return FALSE;
}

// ִ�нű�����
int __stdcall CPythonInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	// TODO: Add your code here
/*	if(m_nRunState != SCRIPT_STATE_BREAK)
	{
		//���Ƕϵ�����£�����һ���µĽ��������������Ϊʹ���������еĽ�������������
		//���ܻ���������еĳ������Ӱ��
		return RunScriptCommandWithNewInt(lpszTclCmd);
	}
	if(m_myTS == 0L)
		return -1;
*/
/*	//��ȡ��
	PyEval_AcquireLock();
	PyThreadState_Swap(m_myTS);
	//��������
	int nResult = PyRun_SimpleString((char *)lpszTclCmd);
	//�ͷ���
	PyEval_ReleaseLock();
*/


	//��ȡ��
	PyEval_AcquireLock();

	//���û�н���������ô�����µĽ�����
	if(m_myTS == 0L)
	{
		//�����µĽ�����������ת����ǰ�߳�
		m_myTS = Py_NewInterpreter();
		//���ôν�������Ӧ�����̣߳����ض�����
		if(CPythonInterp::m_stdout != NULL)
		{
			PySys_SetObject("stdout", m_stdout);
			PySys_SetObject("__stdout__", m_stdout);
		}
		if(CPythonInterp::m_stderr != NULL)
		{
			PySys_SetObject("stderr", m_stderr);
			PySys_SetObject("__stderr__", m_stderr);
		}
		if(m_myTS == NULL)
		{
			::AfxMessageBox(_T("Can not Crate Interpreter"));
			PyEval_ReleaseLock();
			return -1;
		}
		
		//�������в���
		char* argv[] = {(char*)""};
		PySys_SetArgv(1, argv);
		
		//////////////////////////////////////
		//��ʼ������������ʵ����ʹ_platformģ����Ч
		InitializeInterp(m_myTS);
	}
	//ʹ�þɵĽ��������л��̵߳���ǰ������
	else
	{
		PyThreadState_Swap(m_myTS);
	}
	
	strErrorInfo = "";
	g_strErrorInfo = "";
	g_strStdOut = "";
	g_strInterval = "";

	//��������
	int nResult = PyRun_SimpleString((char *)lpszTclCmd);

	if(nResult == -1)
	{
		strErrorInfo = g_strErrorInfo;
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\n') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\r') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
	}else
	{
		strStdOut = g_strStdOut;
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\n') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\r') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);

		strIntervalOut = g_strInterval;
	}

	//�ͷ���
	PyEval_ReleaseLock();



	////���أ�����Ĵ�����ʱʧЧ
	return nResult;

	CString strScript = lpszTclCmd;
	BOOL bKeepState = FALSE;

	if(strcmp(strScript, "[GetCurrentThreadId]") == 0)
	{
		// ��������:��¼��ǰ�߳�ID
		m_dwRunThreadId = ::GetCurrentThreadId();
		return 0;
	}else
	if(strScript.Find("[KeepState]") == 0)
	{
		// ��������:����nRunState��״̬,���ڲ�Ӱ�쵱ǰ�ű����õ����
		bKeepState = TRUE;
		strScript.Delete(0, 11);
	}else

	if(!bKeepState)
	{
		m_nRunState = SCRIPT_STATE_RUN;
	}

	//delete by lcmao
	//CPyThreadState lock(fInterp);
	//end delete

	if(!bKeepState)
	{
		// ��ʼ��argv�б�Ϊ��
		CString strInitArgv = _T("sys.argv = [];");
		PyEval_AcquireLock();
		PyThreadState_Swap(m_myTS);
		char* argv[] = {(char*)(LPCTSTR)m_strScriptFile};
		PySys_SetArgv(1, argv);
		//PyRun_SimpleString((char *)(LPCTSTR)strInitArgv);
		PyEval_ReleaseLock();
	}

	g_strErrorInfo = "";
	g_strStdOut = "";
	g_strInterval = "";
	PyEval_AcquireLock();
	PyThreadState_Swap(m_myTS);
	nResult = PyRun_SimpleString((char *)(LPCTSTR)strScript);
	PyEval_ReleaseLock();

	if(nResult == -1)
	{
		strErrorInfo = g_strErrorInfo;
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\n') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
		if( (strErrorInfo.GetLength() > 0) && (strErrorInfo[strErrorInfo.GetLength()-1] == '\r') )
			strErrorInfo.Delete(strErrorInfo.GetLength()-1, 1);
	}else
	{
		strStdOut = g_strStdOut;
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\n') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);
		if( (strStdOut.GetLength() > 0) && (strStdOut[strStdOut.GetLength()-1] == '\r') )
			strStdOut.Delete(strStdOut.GetLength()-1, 1);

		strIntervalOut = g_strInterval;
	}

	if(!bKeepState)
	{
		m_nRunState = SCRIPT_STATE_END;
	}

	return nResult;
}

// ֹͣ�ű�����
int __stdcall CPythonInterp::StopScript()
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
LPVOID __stdcall CPythonInterp::GetInterp()
{
	// TODO: Add your code here

	return NULL;
}

// ��ȡ������ID
int __stdcall CPythonInterp::GetInterpID()
{
	// TODO: Add your code here

	return m_nInterpID;
}

// ���ý�������
void __stdcall CPythonInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	// TODO: Add your code here
	m_strInterpName = lpszInterptName;
}

// ��ȡ��������
CString __stdcall CPythonInterp::GetInterpName()
{
	// TODO: Add your code here

	return m_strInterpName;
}

// ��ȡ�����к�
int __stdcall CPythonInterp::GetErrorLine()
{
	// TODO: Add your code here
	if(strErrorInfo.GetLength() > 0)
	{
		int nPos = strErrorInfo.Find("File \"");
		if(nPos != -1)
		{//AfxMessageBox(strErrorInfo);
			CString strTmp = strErrorInfo;
			strTmp.Delete(0, nPos+6);
			nPos = strTmp.Find("\"");
			if(nPos != -1)
			{
				CString strErrorFile = strTmp.Left(nPos);	// ���������ļ�
				strTmp.Delete(0, nPos+1);
				nPos = strTmp.Find(", line ");
				if(nPos != -1)
				{
					strTmp.Delete(0, 7);
					nPos = strTmp.Find(",");
					if(nPos != -1)
					{
						strTmp = strTmp.Left(nPos);
						strTmp.TrimRight();
						int nErrorLine = atoi(strTmp);
						return nErrorLine;
					}
					// ���û�н�β�Ķ��ţ����жϻس�����
					nPos = strTmp.Find("\r");
					if(nPos != -1)
					{
						strTmp = strTmp.Left(nPos);
						strTmp.TrimRight();
						int nErrorLine = atoi(strTmp);
						return nErrorLine;
					}
					nPos = strTmp.Find("\n");
					if(nPos != -1)
					{
						strTmp = strTmp.Left(nPos);
						strTmp.TrimRight();
						int nErrorLine = atoi(strTmp);
						return nErrorLine;
					}
				}
			}
		}else
		{
			nPos = strErrorInfo.Find("Traceback (most recent call last):");
			if(nPos != -1)
			{
				return -1;
			}
		}
	}
	return 0;
}

// ��ȡ�����Ϣ
CString __stdcall CPythonInterp::GetResult()
{
	// TODO: Add your code here
	if(strErrorInfo.GetLength() > 0)
		return strErrorInfo;
	else if(strStdOut.GetLength() > 0)
		return strStdOut;
	else
		return strIntervalOut;
}

/////////////////////////////////////////////////////////////////////////////
// ����_platformģ���е�Python����
/////////////////////////////////////////////////////////////////////////////
void CPythonInterp::SetVarInPlatModule(CString strVarName, CString strVarValue)
{
	if(pyPlatModule == NULL)
	{
		return;
	}

	PyObject *pyVar = PyString_FromString(strVarValue);
	if (pyVar != NULL) {
		PyObject_SetAttrString(pyPlatModule, (char*)(const char*)strVarValue, pyVar);
		Py_DecRef(pyVar);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ_platformģ���е�Python����
/////////////////////////////////////////////////////////////////////////////
CString CPythonInterp::GetVarInPlatModule(CString strVarName)
{
	if(pyPlatModule == NULL)
	{
		return _T("");
	}
	return _T("");
}

// ���ñ���ֵ
int __stdcall CPythonInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	// TODO: Add your code here
	SetVarInPlatModule(lpszVarName, lpszVarValue);

	return 0;
}

// ��ȡ����ֵ
CString __stdcall CPythonInterp::GetVar(LPCTSTR lpszVarName)
{
	// TODO: Add your code here
	//return GetVarInPlatModule(lpszVarName);

	if(m_currentframe == NULL)
	{
		//����Ӧ����Զ���ᵽ��Ŷԣ�����Ϊ����߿ɿ���
		return "";
	}

	PyFrameObject* frame = m_currentframe;

	//���acquireLockΪtrue���˺��������ȡpython���߳���
	//������Ѿ�ռ���߳�����ʱ��������������һ��Ҫ����acquireLockΪFALSE�����������
	BOOL acquireLock = TRUE;

	//���»�ȡ��
	if(acquireLock)
	{
		PyEval_AcquireLock();
		PyThreadState_Swap(frame->f_tstate);
	}

	CString strName, strValue, strType;
	BOOL bLocal = FALSE;

	// ��ȡ�ֲ�����Locals
	//��FastLocalת����Locals���������Ҫ��PyFrame_LocalsToFast�ָ�
	PyFrame_FastToLocals(frame);

	PyObject *key, *value, *strKey, *szValue;
	Py_ssize_t pos = 0;
	if(frame->f_locals)
	{
		while (PyDict_Next(frame->f_locals, &pos, &key, &value)) 
		{
			//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
			strKey = PyObject_Str(key);
			strName = PyString_AsString(strKey);
			if(strName != lpszVarName)
			{
				continue;
			}
			szValue = PyObject_Str(value);
			strValue = PyString_AsString(szValue);
			strType = value->ob_type->tp_name;
			Py_DecRef(strKey);
			Py_DecRef(szValue);
			bLocal = TRUE;
			break;
		}
	}

	//�ָ�FastLocal��ǰ������PyFrame_FastToLocals��������Ҫ�ָ�
	PyFrame_LocalsToFast(frame, 0);

	// ��ȡȫ�ֱ���
	if(!bLocal)
	{
		pos = 0;
		if(frame->f_globals)
		{
			while (PyDict_Next(frame->f_globals, &pos, &key, &value)) 
			{
				//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
				strKey = PyObject_Str(key);
				strName = PyString_AsString(strKey);
				if(strName != lpszVarName)
				{
					continue;
				}
				szValue = PyObject_Str(value);
				strValue = PyString_AsString(szValue);
				strType = value->ob_type->tp_name;
				Py_DecRef(strKey);
				Py_DecRef(szValue);
				break;
			}
		}
	}

	if(acquireLock)
	{
		PyEval_ReleaseLock();
	}

	if(strType == "str")
	{
		strValue = "'" + strValue;
		strValue += "'";
	}

	return strValue;
}

// ע����չ����
int __stdcall CPythonInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	// TODO: Add your code here

	return 0;
}

// ������չ����
int __stdcall CPythonInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	// TODO: Add your code here

	return 0;
}

// ��ȡ��չ����
CString __stdcall CPythonInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	// TODO: Add your code here

	return "";
}

// ��������ģʽ�������нű�ǰ�����������ֵ
int __stdcall CPythonInterp::SetRunMode(int nRunMode)
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
int __stdcall CPythonInterp::GetRunMode()
{
	return m_nRunMode;
}

// ��������״̬
int __stdcall CPythonInterp::SetRunState(int nState)
{
	m_nRunState = nState;
	return true;
}

// ��ȡ����״̬
int __stdcall CPythonInterp::GetRunState()
{
	return m_nRunState;
}

// ��ȡ��ǰ��ջ����
int __stdcall CPythonInterp::GetCurStackLevel()
{
	// TODO: Add your code here

	return m_nStackLevel;
}

// ��ȡ�ؼ����б�
int __stdcall CPythonInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
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
int __stdcall CPythonInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
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
					//sprintf(szTclCmd, "td -goto *%d", nParam);
			SetBreakLineNo(nParam);
		}
		else
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
int __stdcall CPythonInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CPythonInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
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
int __stdcall CPythonInterp::GetEnableProfileAnalyse()
{
	// TODO: Add your code here
	return m_bProfileEnable;
}

// ��ȡ��Ч��������
int __stdcall CPythonInterp::GetAvailableLineCount()
{
	// TODO: Add your code here
	return m_nTDLineCount;
}

// ��ȡ�������и��Ǳ�
LPVOID __stdcall CPythonInterp::GetProfileMap()
{
	// TODO: Add your code here
	return (void*)(&m_ProfileMap);
}

// �ж�ָ�������Ƿ�֧��
int __stdcall CPythonInterp::MethodIsSupport(LPCTSTR lpszMethod)
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
int __stdcall CPythonInterp::InitialDebugMode(CString& strResult)
{
	// TODO: Add your code here
	// ��ʼ���������Դ���
	CString strWinVar, strWinObj, strWinFunc, strWinStack;
	LOADRCSTRING( strWinVar, IDS_WINDOW_VAR );
	LOADRCSTRING( strWinObj, IDS_WINDOW_OBJ );
	LOADRCSTRING( strWinFunc, IDS_WINDOW_FUNC );
	LOADRCSTRING( strWinStack, IDS_WINDOW_STACK );

	CString strColName, strColValue, strColType, strColSource, strColLine;
	LOADRCSTRING( strColName, IDS_COLUMN_NAME );
	LOADRCSTRING( strColValue, IDS_COLUMN_VALUE );
	LOADRCSTRING( strColType, IDS_COLUMN_TYPE );
	LOADRCSTRING( strColSource, IDS_COLUMN_SOURCE );
	LOADRCSTRING( strColLine, IDS_COLUMN_LINE );

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

	// ��ʼ����ջ����
	m_nOutputStack = ::atoi(pIPlatUI->OutputGet(-1, strWinStack));
	if(m_nOutputStack <= 0)
	{
		//�´��ڣ�����
		m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID##" + strWinVar);
	}
	pIPlatUI->OutputSet(m_nOutputStack, "DelColumn ALL;DelItem ALL");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColName + ",100");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColSource + ",150");
	pIPlatUI->OutputSet(m_nOutputStack, "AddColumn " + strColLine + ",50,AutoSize");
	pIPlatUI->OutputSet(m_nOutputStack, "SetTreeColumn 0");
	pIPlatUI->OutputSet(m_nOutputStack, "SetRowColors RGB(0,0,128) RGB(255,255,205)");
	pIPlatUI->OutputSet(m_nOutputStack, "SetGridColor RGB(192,192,192)");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid;NoSort");
	pIPlatUI->OutputSet(m_nOutputStack, "ModifyStyle Add(NoSort)");
	pIPlatUI->OutputSet(m_nOutputStack, "SEPARATOR=" SEPATATOE); //���÷ָ���
	m_nOutputStack = pIPlatUI->AddOutput(strWinStack + "##GRID");

	// �����������
	pIPlatUI->ActiveOutput(m_nOutputVar);

	return TRUE;
}

// Python����������˫������
void callback_python_var_ldclick(CPythonInterp* pInterp, LPCTSTR lpszInfo)
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
	CString strVarName = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	strTmp.Format("<%d,1>", lpItem);
	CString strVarValue = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);
	strTmp.Format("<%d,2>", lpItem);
	CString strVarType = pInterp->pIPlatUI->OutputGet(pInterp->m_nOutputVar, strTmp);

	if(strVarType == "")
	{
		return;
	}

	DLL_LOADSTRING(strWatchVarTitle, IDS_WATCH_VAR_TITLE);
	DLL_LOADSTRING(strWatchVarName, IDS_WATCH_VAR_NAME);
	DLL_LOADSTRING(strWatchVarValue, IDS_WATCH_VAR_VALUE);
	DLL_LOADSTRING(strWatchVarType, IDS_WATCH_VAR_TYPE);
	CString strMsg;
	strMsg.Format("%s:%s\n%s:%s\n%s:%s", strWatchVarName, strVarName, strWatchVarValue, strVarValue, strWatchVarType, strVarType);
	::MessageBox(AfxGetMainWnd()->m_hWnd, strMsg, strWatchVarTitle, MB_OK | MB_ICONINFORMATION);

}

// ˢ�±�������
int CPythonInterp::RefreshVarWindow()
{
	if(m_currentframe == NULL)
	{
		//����Ӧ����Զ���ᵽ��Ŷԣ�����Ϊ����߿ɿ���
		return FALSE;
	}

	PyFrameObject* frame = m_currentframe;

	//���acquireLockΪtrue���˺��������ȡpython���߳���
	//������Ѿ�ռ���߳�����ʱ��������������һ��Ҫ����acquireLockΪFALSE�����������
	BOOL acquireLock = TRUE;

	//���»�ȡ��
	if(acquireLock)
	{
		PyEval_AcquireLock();
		PyThreadState_Swap(frame->f_tstate);
	}

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

	//��FastLocalת����Locals���������Ҫ��PyFrame_LocalsToFast�ָ�
	PyFrame_FastToLocals(frame);

	PyObject *key, *value, *strKey, *szValue;
	Py_ssize_t pos = 0;
	if(frame->f_locals)
	{
		while (PyDict_Next(frame->f_locals, &pos, &key, &value)) 
		{
			//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
			strKey = PyObject_Str(key);
			szValue = PyObject_Str(value);
			strOutput.Format("%s%s%s%s%s", PyString_AsString(strKey),
				SEPATATOE, PyString_AsString(szValue), 
				SEPATATOE, value->ob_type->tp_name);
			Py_DecRef(strKey);
			Py_DecRef(szValue);

			item.strLine = strOutput;
			item.nParent = nParent;
			item.nImage = 5;
			item.nExpand = 0;
			item.nClickFunc = 1;
			aItems.Add(item);
		}
	}

	//�ָ�FastLocal��ǰ������PyFrame_FastToLocals��������Ҫ�ָ�
	PyFrame_LocalsToFast(frame, 0);

	// Globals
	strOutput = "Globals";
	item.strLine = strOutput;
	item.nParent = -1;
	item.nImage = 4;
	item.nExpand = 1;
	item.nClickFunc = 0;
	aItems.Add(item);
	nParent = aItems.GetSize()-1;

	pos = 0;
	if(frame->f_globals)
	{
		while (PyDict_Next(frame->f_globals, &pos, &key, &value)) 
		{
			//TODO ���������bug��strKey��strValue��Ȼ�п�����NULL����
			strKey = PyObject_Str(key);
			szValue = PyObject_Str(value);
			strOutput.Format("%s%s%s%s%s", PyString_AsString(strKey),
				SEPATATOE, PyString_AsString(szValue),
				SEPATATOE, value->ob_type->tp_name);
			Py_DecRef(strKey);
			Py_DecRef(szValue);

			item.strLine = strOutput;
			item.nParent = nParent;
			item.nImage = 5;
			item.nExpand = 0;
			item.nClickFunc = 1;
			aItems.Add(item);
		}
	}

	if(acquireLock)
	{
		PyEval_ReleaseLock();
	}


	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputVar, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_python_var_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputVar, strCallback);

	return TRUE;
}

// Python��ջ������˫������
void callback_python_stack_ldclick(CPythonInterp* pInterp, LPCTSTR lpszInfo)
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
int CPythonInterp::RefreshStackWindow()
{
	CString strOutput;

	CTTreeItemArray aItems;

	TTREEITEM item;
	item.hItem = NULL;
	item.nParent = -1;
	item.nExpand = 0;
	item.nImage = 27;
	item.nClickFunc = 1;

	PyFrameObject* frame = m_currentframe;

	while(frame)
	{
		strOutput.Format("%s%s%s%s%d",
			PyString_AsString(frame->f_code->co_name), //names,
			SEPATATOE,
			PyString_AsString(frame->f_code->co_filename),
			SEPATATOE,
			frame->f_lineno);
		frame = frame->f_back;

		item.strLine = strOutput;
		aItems.Add(item);
	}

	pIPlatUI->SendCommand(OC_TREEOUTPUT, m_nOutputStack, (LPARAM)(LPVOID)(&aItems));

	CString strCallback;
	strCallback.Format("<I>Set Script<0> <<cfunction:%d>>%d,%d", (int)callback_python_stack_ldclick, (int)this, 1);
	pIPlatUI->Output(m_nOutputStack, strCallback);

	return TRUE;
}

// ���µ��Դ�����Ϣ,����״ֻ̬�����ж�����²��ܿ����Դ�����Ϣ
int __stdcall CPythonInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	if(m_nRunState != SCRIPT_STATE_BREAK)
	{
		m_pCurPlatUI->Output(1, "Reflash window only support when state is break\r\n");
		return 0;
	}

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
		//RefreshFuncWindow();
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
int __stdcall CPythonInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
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

// ѡ���ִ���ļ�����
int CPythonInterp::SelectSetupType()
{
	CTOptArray aopt;	// ����������Ԫ������
	TOPTITEM item;		// ������Ԫ��
	item.hItem		= NULL;
	item.iType		= TOPT_GROUP;
	CString strTemp;
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_SETUP_TYPE)
	strTemp = strTemp;//"ѡ���ִ���ļ�����";
	item.lpszText	= strTemp;
	item.lpszEdit	= "";
	item.lpszList	= "";
	item.bCheck		= FALSE;
	item.iImage		= TOPTI_FOLDER;
	item.iParent	= -1;
	aopt.Add(item);		// ��Ӹ��ڵ�

	item.iParent	= 0;
	item.iType		= TOPT_RADIOBUTTON;
	//item.iImage		= TOPTI_INFO;
	item.bCheck		= TRUE;
	item.lpszText	= "Console Application";
	aopt.Add(item);
	item.bCheck		= FALSE;
	item.lpszText	= "GUI Application";
	aopt.Add(item);
	item.lpszText	= "WMI Application";
	aopt.Add(item);
	item.lpszText	= "Windows Service";
	aopt.Add(item);
	item.lpszText	= "Windows COM EXE";
	aopt.Add(item);
	item.lpszText	= "Windows COM DLL";
	aopt.Add(item);

	// ��ӱ������ʾ��Ϣ
	item.iType		= TOPT_MSG;
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_SETUP_TYPE_TITLE);
	item.lpszText	= strTemp;//"ѡ���ִ���ļ�����";
	LOADRCSTRING( strTemp, IDS_OPTDLG_SELECT_SETUP_TYPE_MSG);
	item.lpszEdit	= strTemp;//"��ѡ���ִ���ļ����ͣ��ᰴ�յ�ѡ����������ɿ�ִ���ļ�";
	item.iParent	= -1;
	aopt.Add(item);

	int nRet = -1;

	try{
		pIPlatUI->SendOwmCmd("", OC_EXECTREEOPTION, 0, (LPARAM)(&aopt), &nRet);
	}catch(...)
	{
		return -1;
	}

	if(nRet != IDOK)
	{
		return -1;
	}

	for(int i=0; i<aopt.GetSize(); i++)
	{
		if(TOPT_RADIOBUTTON == aopt[i].iType)
		{
			if(aopt[i].bCheck)
			{
				return i-1;
			}
		}
	}
	
	return -1;
}

// ����
int __stdcall CPythonInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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

	CString strOutFile = strSrcFile;
	strOutFile.MakeLower();
	int nDotPos = strOutFile.ReverseFind('.');
	if(nDotPos != -1)
	{
		strOutFile.Delete(nDotPos, strOutFile.GetLength() - nDotPos);
	}

	// �Զ���������ļ�
	BOOL bAutoOutFile = AfxGetApp()->GetProfileInt(REG_PY_COMPILE_SUBKEY, REG_PY_COMPILE_AUTO_OUTFILE, TRUE);
	if(bAutoOutFile)
	{
		strOutFile += ".pyc";
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
	BOOL bForceOverwrite = AfxGetApp()->GetProfileInt(REG_PY_COMPILE_SUBKEY, REG_PY_COMPILE_FORCE_OVERWRITE, FALSE);
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

	CString strCmd;
	strCmd.Format("import compiler;compiler.compileFile(\"%s\");", strSrcFile);
	int nRet = RunScriptCommand(strCmd);
	if(nRet == 0)
	{
		// PyRun_SimpleString Returns 0 on success or -1 if an exception was raised. 
		strDestFile = strSrcFile;
		strDestFile.MakeLower();
		strDestFile.Replace("/", "\\");
		int nPos = strDestFile.ReverseFind('.');
		if(nPos != -1)
		{
			strDestFile = strDestFile.Left(nPos);
			strDestFile += ".pyc";
		}
		if(GetFileAttributes(strDestFile) == 0xFFFFFFFF)
		{
			strDestFile = "";
			m_nRunState = SCRIPT_STATE_END;
			return FALSE;
		}
		m_nRunState = SCRIPT_STATE_END;
		return TRUE;
	}

	m_nRunState = SCRIPT_STATE_END;
	return FALSE;
}

// ����
int __stdcall CPythonInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
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

	// �滻��ִ���ļ���ͼ��
	CString strIconFile = GetInterpRootPath();
	strIconFile += "bin\\pyexe.ico";	// Ĭ�ϵ�ͼ���ļ�
	strIconFile.Replace("/", "\\");
	BOOL bReplaceIcon = AfxGetApp()->GetProfileInt(REG_PY_BUILD_SUBKEY, REG_PY_BUILD_REPLACE_ICON, FALSE);
	if(bReplaceIcon)
	{
		// ͼ��ѡ��ķ�ʽ
		CString strNewIcon = "";
		int nSelectIconType = AfxGetApp()->GetProfileInt(REG_PY_BUILD_SUBKEY, REG_PY_BUILD_SELECT_ICON_TYPE, 0);
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
				//return FALSE;
				if(pIPlatUI)
				{
					pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=255,0,0");
					pIPlatUI->Output(_WND_DEBUG, "Not select program icon, use default icon!\r\n");
					pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,0");
				}
			}
		}
		if(GetFileAttributes(strNewIcon) == 0xFFFFFFFF)
		{
			//return FALSE;
			if(pIPlatUI)
			{
				pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=255,0,0");
				pIPlatUI->Output(_WND_DEBUG, "Not exist specified icon file, use default icon!\r\n");
				pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,0");
			}
		}else
		{
			strIconFile = strNewIcon;
		}
	}
	strIconFile.Replace("\\", "\\\\");

	// setup.py�ļ���
	CString strSetupPy = strSrcFile;
	strSetupPy.Replace("/", "\\");
	nPos = strSetupPy.ReverseFind('\\');
	if(nPos != -1)
	{
		strSetupPy = strSetupPy.Left(nPos);
	}
	strSetupPy += "\\setup.py";

	// ��¼�Ƿ��Ѿ�����setup.py,���������ʹ��,���������,�����ɾ��
	BOOL bHaveSetupPy = (GetFileAttributes(strSetupPy) != 0xFFFFFFFF);
	
	if(!bHaveSetupPy)
	{
		// ѡ���ļ�����
		BOOL bSelectSetupType = AfxGetApp()->GetProfileInt(REG_PY_BUILD_SUBKEY, REG_PY_BUILD_SELECT_TYPE, TRUE);
		int nSelectSetupType = 0;
		if(bSelectSetupType)
		{
			nSelectSetupType = SelectSetupType();
			if(nSelectSetupType == -1)
			{
				return FALSE;
			}
		}

		CString strScriptFile = strSrcFile;
		strScriptFile.Replace("\\", "\\\\");

		// û��setup.py�����ģ�崴��һ��
		CString strSetupTemplateFile = GetInterpRootPath();
		strSetupTemplateFile += "bin\\setup.py";
		CStdioFile fiTemplate(strSetupTemplateFile, CFile::modeRead);	// ģ���ļ�
		CStdioFile fiSetup;	// ���ɵ�setup.py�ļ�
		fiSetup.Open( strSetupPy, CFile::modeCreate | CFile::modeWrite | CFile::typeText );
		CString strLine;
		while(fiTemplate.ReadString(strLine))
		{
			strLine.Replace("$$SCRIPT_FILE$$", strScriptFile);
			strLine.Replace("$$PROGRAM_NAME$$", strOutFileName);
			strLine.Replace("$$PROGRAM_ICON$$", strIconFile);
			switch(nSelectSetupType)
			{
			case 0: strLine.Replace("$$SETUP_TYPE$$", "console=[setup_wx_console]");break;
			case 1: strLine.Replace("$$SETUP_TYPE$$", "windows=[setup_wx]");break;
			case 2: strLine.Replace("$$SETUP_TYPE$$", "console=[setup_wmi]");break;
			case 3: strLine.Replace("$$SETUP_TYPE$$", "service=[setup_service]");break;
			case 4: strLine.Replace("$$SETUP_TYPE$$", "com_server=[setup_com_exe]");break;
			case 5: strLine.Replace("$$SETUP_TYPE$$", "com_server=[setup_com_dll]");break;
			}
			//strLine.Replace("$$SETUP_TYPE$$", strSrcFile);
			fiSetup.WriteString(strLine);
			fiSetup.WriteString("\n");
		}
		fiSetup.Close();
		fiTemplate.Close();
	}else
	{
		// ʹ��Ŀ¼���Ѿ����ڵ�setup.py�ļ�
		if(pIPlatUI)
		{
			pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,128");
			pIPlatUI->Output(_WND_DEBUG, "Attention: The setup.py have exist in script folder, use this setup script to build!\r\n");
			pIPlatUI->OutputSet(_WND_DEBUG, "COLOR=0,0,0");
		}
	}

	BOOL bHaveBuildDir = (GetFileAttributes(strOutFile + "\\build") != 0xFFFFFFFF);
	BOOL bHaveDistDir = (GetFileAttributes(strOutFile + "\\dist") != 0xFFFFFFFF);

	// ��¼��ǰ·��,�����õ�ǰ·��
	char szOldPath[256];
	memset(szOldPath, 0, 256);
	DWORD dwLen = GetCurrentDirectory(255, szOldPath);
	CString strPath = strSrcFile;
	strPath.Replace("/", "\\");
	nPos = strPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		strPath = strPath.Left(nPos);
	}
	SetCurrentDirectory(strPath);

	// ִ��py2exe,ʹ�ý�����binĿ¼�µ�pythonw.exe�����нű�,ע��pythonw.exe�����python25.dll��ͬһ��Ŀ¼��
	CString strCmd;
	CString strPythonFile	= GetInterpRootPath();
	strPythonFile	+= "bin\\pythonw.exe";
	strCmd.Format("\"%s\" py2exe", strSetupPy);
	if(!RunProgramWithPipe(strPythonFile, strCmd))
	{
		// ɾ����ʱĿ¼
		if((GetFileAttributes(strOutFile + "\\build") != 0xFFFFFFFF) && !bHaveBuildDir)
		{
			DeleteDir(strOutFile + "\\build");
		}
		if((GetFileAttributes(strOutFile + "\\dist") != 0xFFFFFFFF) && !bHaveDistDir)
		{
			DeleteDir(strOutFile + "\\dist");
		}
		return FALSE;
	}

	strDestFile = strOutFile + "\\dist\\" + strOutFileName + ".exe";
	if(GetFileAttributes(strDestFile) == 0xFFFFFFFF)
	{
		strDestFile = "";
		return FALSE;
	}

	// ɾ����ʱĿ¼
	if((GetFileAttributes(strOutFile + "\\build") != 0xFFFFFFFF) && !bHaveBuildDir)
	{
		DeleteDir(strOutFile + "\\build");
	}

	// ɾ���Լ�������setup.py
	if(!bHaveSetupPy)
	{
		DeleteFile(strSetupPy);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CPythonInterp::CPythonInterp()
{
	m_myTS = 0L;

	if(!CPythonInterp::IsAvail())
	{
		return;
	}

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

	pyPlatModule	= NULL;

	// ��һ�ε�ʱ���ʼ��Python֧��
	if(g_PythonInterpList.GetCount() == 0)
	{
		// init-------------------------
		InitPythonSupport();
		g_pPythonInterp = this;
	}

	g_PythonInterpList.AddTail(this);
	
	/*//��ȡ��
	PyEval_AcquireLock();
	//�����µĽ�����������ת����ǰ�߳�
	m_myTS = Py_NewInterpreter();
	//���ôν�������Ӧ�����̣߳����ض�����
	if(CPythonInterp::m_stdout != NULL)
	{
		PySys_SetObject("stdout", m_stdout);
		PySys_SetObject("__stdout__", m_stdout);
		PySys_SetObject("stderr", m_stdout);
		PySys_SetObject("__stderr__", m_stdout);
	}
	//�ͷ���
	PyEval_ReleaseLock();

	if(m_myTS == NULL)
	{
		::AfxMessageBox(_T("Can not Crate Interpreter"));
	}*/
}

CPythonInterp::~CPythonInterp()
{
	// ��ո����ʷ�����ϣ��
	m_ProfileMap.RemoveAll();

			//edit by lcmao
			/*
	if(fInterp != 0L)
	{
		if(fInterp != m_mainInterpreter)
		{
			CPyThreadState lock(fInterp);

			Py_EndInterpreter(fInterp);
		}
	}
			*/
	//���ٽ�����
	if(m_myTS != NULL)
	{
		PyInterpreterState *interp = m_myTS->interp;
		if((m_myTS->next == NULL)&&(m_myTS == interp->tstate_head))
		{
			//
			PyEval_AcquireLock();
			PyThreadState_Swap(m_myTS);
			Py_EndInterpreter(m_myTS);
			PyEval_ReleaseLock();
		}
		else
		{
			//�����߳�û���˳������ٽ������Ļ��������쳣
		}
	}
	m_myTS = NULL;
			//end edit

	// ɾ�������������б��е���
	int count = g_PythonInterpList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_PythonInterpList.FindIndex(i);
		CPythonInterp* pPythonInterp = (CPythonInterp*)g_PythonInterpList.GetAt(pos);
		if(pPythonInterp == this) 
		{
			g_PythonInterpList.RemoveAt(pos);
			// ���������������,�͹ر�Python֧��
			//delete by lcmao
			/*
			if(g_PythonInterpList.GetCount() == 0)
			{
				//done-------------------
				ClosePythonSupport();
				g_pPythonInterp = NULL;
			}*/
			///////end delete
			break;
		}
	}
	//add by lcmao
	if(m_debugArg != NULL)
	{
		Py_DecRef(m_debugArg);
		m_debugArg = NULL;
	}
	//end by lcmao
}
//}}VCI_IMPLEMENT_END

/////////////////////////////////////////////////////////////////////////////
// ���öϵ㼤���ź�
/////////////////////////////////////////////////////////////////////////////
void CPythonInterp::SetBreakSignal(BOOL bBreak)
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
void CPythonInterp::DebugCallback(int nEvent, int nLine)
{
	if(!m_lpfnDebug)
	{
		return;
	}

	m_lpfnDebug(nEvent, GetScriptFile(), nLine);
}

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(PythonInterp)
CONTROL_INTERFACE_IMPLEMENT(PythonInterp)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CPythonInterp::XPythonInterp::GetExtraInfo(LPVOID lpRefData)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
//{{VCI_INTERFACE_IMPLEMENT_BEGIN
// ����ƽ̨UI�ӿ�ָ��
int __stdcall CPythonInterp::XPythonInterp::SetIPlatUI(LPVOID lpIPlatUI)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetIPlatUI(lpIPlatUI);
}

int __stdcall CPythonInterp::XPythonInterp::SetLibPath(LPCTSTR lpszPath)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetLibPath(lpszPath);
}

int __stdcall CPythonInterp::XPythonInterp::IsInterpRunning()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->IsInterpRunning();
}

int __stdcall CPythonInterp::XPythonInterp::SetScriptFile(LPCTSTR lpszPathName)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetScriptFile(lpszPathName);
}

CString __stdcall CPythonInterp::XPythonInterp::GetScriptFile()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetScriptFile();
}

int __stdcall CPythonInterp::XPythonInterp::RunScriptFile(LPCTSTR lpszPathName)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->RunScriptFile(lpszPathName);
}

int __stdcall CPythonInterp::XPythonInterp::RunScriptCommand(LPCTSTR lpszTclCmd)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->RunScriptCommand(lpszTclCmd);
}

int __stdcall CPythonInterp::XPythonInterp::StopScript()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->StopScript();
}

LPVOID __stdcall CPythonInterp::XPythonInterp::GetInterp()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetInterp();
}

int __stdcall CPythonInterp::XPythonInterp::GetInterpID()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetInterpID();
}

void __stdcall CPythonInterp::XPythonInterp::SetInterpName(LPCTSTR lpszInterptName)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	pObj->SetInterpName(lpszInterptName);
}

CString __stdcall CPythonInterp::XPythonInterp::GetInterpName()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetInterpName();
}

int __stdcall CPythonInterp::XPythonInterp::GetErrorLine()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetErrorLine();
}

CString __stdcall CPythonInterp::XPythonInterp::GetResult()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetResult();
}

int __stdcall CPythonInterp::XPythonInterp::SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetVar(lpszVarName, lpszVarValue);
}

CString __stdcall CPythonInterp::XPythonInterp::GetVar(LPCTSTR lpszVarName)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetVar(lpszVarName);
}

int __stdcall CPythonInterp::XPythonInterp::RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->RegisterCommand(pInterp, lpszCmd, lpFunc);
}

int __stdcall CPythonInterp::XPythonInterp::SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetInterpParam(lpszParamName, lpszParamValue);
}

CString __stdcall CPythonInterp::XPythonInterp::GetInterpParam(LPCTSTR lpszParamName)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetInterpParam(lpszParamName);
}

int __stdcall CPythonInterp::XPythonInterp::SetRunMode(int nRunMode)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetRunMode(nRunMode);
}

int __stdcall CPythonInterp::XPythonInterp::GetRunMode()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetRunMode();
}

int __stdcall CPythonInterp::XPythonInterp::SetRunState(int nRunState)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetRunState(nRunState);
}

int __stdcall CPythonInterp::XPythonInterp::GetRunState()
{
	try
	{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetRunState();
	}catch(...)
	{
	}
	return SCRIPT_STATE_END;  //�쳣Ӧ������Ϊ�Ѿ��ͷŶ���
}

int __stdcall CPythonInterp::XPythonInterp::GetCurStackLevel()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetCurStackLevel();
}

// ��ȡ�ؼ����б�
int __stdcall CPythonInterp::XPythonInterp::GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetKeyWordList(lpszType, asKeyWord);
}

int __stdcall CPythonInterp::XPythonInterp::SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetDebugEvent(nEvent, nParam, lpszParam);
}

int __stdcall CPythonInterp::XPythonInterp::SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->SetBreakList(lpszFile, auBreaks);
}

int __stdcall CPythonInterp::XPythonInterp::GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetBreakList(lpszFile, auBreaks);
}

int __stdcall CPythonInterp::XPythonInterp::GetEnableProfileAnalyse()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetEnableProfileAnalyse();
}

int __stdcall CPythonInterp::XPythonInterp::GetAvailableLineCount()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetAvailableLineCount();
}

LPVOID __stdcall CPythonInterp::XPythonInterp::GetProfileMap()
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->GetProfileMap();
}

int __stdcall CPythonInterp::XPythonInterp::MethodIsSupport(LPCTSTR lpszMethod)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->MethodIsSupport(lpszMethod);
}

int __stdcall CPythonInterp::XPythonInterp::InitialDebugMode(CString& strResult)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->InitialDebugMode(strResult);
}

int __stdcall CPythonInterp::XPythonInterp::RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->RefreshDebugWindow(lpszWindow, bActive, strResult);
}

int __stdcall CPythonInterp::XPythonInterp::CallMethod(LPCTSTR lpszMethod, CStringArray& asParam)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->CallMethod(lpszMethod, asParam);
}

int __stdcall CPythonInterp::XPythonInterp::Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
	return pObj->Compile(lpszSrcFile, strDestFile, lpszParams);
}

int __stdcall CPythonInterp::XPythonInterp::Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams)
{
	CPythonInterp *pObj = GET_INTERFACE_OBJECT(PythonInterp);
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

#if !defined(__VCI_INTERFACE_CPythonInterp_H_INCLUDED__)
#define __VCI_INTERFACE_CPythonInterp_H_INCLUDED__



#include "vcicomm.h"
#include "IInterp.h"

#define INF_IInterp \
{\
	"IInterp",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"{87AA1D66-6BC3-4C12-9459-E70C6DB08349}",\
	"Python Interp",\
	"Blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IInterp = INF_IInterp;


// Python�����¼�����
enum{
	// ����ΪPython�������ص��¼�
	PDB_USER_CALL,		// ���ú���
	PDB_USER_LINE,		// ִ��һ��
	PDB_USER_RETURN,	// ��������
	PDB_USER_EXCEPTION,	// �����쳣

	// ����Ϊƽ̨���������¼�
	PDB_STOP,			// ��ֹ
	PDB_NEXT,			// ִ�е���һ���ϵ�
	PDB_GOTO,			// ִ�е�ĳ���жϵ�������ϵ�
	PDB_STEPINTO,		// �������뺯��
	PDB_SETPOUT,		// �Ӻ�������
	PDB_STEP,			// ���������뺯��
	PDB_END,			// ֪ͨ�������ļ����Խ���,����ִ���ƺ���

	PDB_SETFILE = 20,	// ���ñ����ԵĽű��ļ�
};


typedef struct _ts PyThreadState;
typedef struct _object PyObject;

////////////////////////////////add by lcmao
typedef struct _frame PyFrameObject;
/////////////////////////////////end by lcmao

class CPythonInterp : public CVisualComponent
{
public:
	CPythonInterp();
	~CPythonInterp();

	// tells if Python is available
	static bool IsAvail(const bool forceReload = false);
	
	static void InitPythonSupport(void);
	static void ClosePythonSupport(void);

	inline operator PyThreadState *(void) {return m_myTS;}

	// ���ļ�����
	CStringArray* GetDebugScriptList() { return &m_asDebugScript; }
	int FindDebugFile(CString strFileName);
	int AddDebugFile(CString strFileName);
	BOOL ClearDebugFile();

//////////////////////////////////////////////////////////////////////////
// �ṩ���ӿڵĹ��ܺ���
//////////////////////////////////////////////////////////////////////////
public:
	//{{VCI_IMPLEMENT_DEFINE_BEGIN
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI);// ����ƽ̨UI�ӿ�ָ��
	virtual int __stdcall SetLibPath(LPCTSTR lpszPath);
	virtual int __stdcall IsInterpRunning();
	virtual int __stdcall SetScriptFile(LPCTSTR lpszPathName);
	virtual CString __stdcall GetScriptFile();
	virtual int __stdcall RunScriptFile(LPCTSTR lpszPathName);
	virtual int __stdcall RunScriptCommand(LPCTSTR lpszTclCmd);
	virtual int __stdcall StopScript();
	virtual LPVOID __stdcall GetInterp();
	virtual int __stdcall GetInterpID();
	virtual void __stdcall SetInterpName(LPCTSTR lpszInterptName);
	virtual CString __stdcall GetInterpName();
	virtual int __stdcall GetErrorLine();
	virtual CString __stdcall GetResult();
	virtual int __stdcall SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue);
	virtual CString __stdcall GetVar(LPCTSTR lpszVarName);
	virtual int __stdcall RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc);
	virtual int __stdcall SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue);
	virtual CString __stdcall GetInterpParam(LPCTSTR lpszParamName);
	virtual int __stdcall SetRunMode(int nRunMode);
	virtual int __stdcall GetRunMode();
	virtual int __stdcall SetRunState(int nRunState);
	virtual int __stdcall GetRunState();
	virtual int __stdcall GetCurStackLevel();
	virtual int __stdcall GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord);
	virtual int __stdcall SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam);
	virtual int __stdcall SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks);
	virtual int __stdcall GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks);
	virtual int __stdcall GetEnableProfileAnalyse();
	virtual int __stdcall GetAvailableLineCount();
	virtual LPVOID __stdcall GetProfileMap();
	virtual int __stdcall MethodIsSupport(LPCTSTR lpszMethod);
	virtual int __stdcall InitialDebugMode(CString& strResult);
	virtual int __stdcall RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult);
	virtual int __stdcall CallMethod(LPCTSTR lpszMethod, CStringArray& asParam);
	virtual int __stdcall Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams);
	virtual int __stdcall Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams);
	//}}VCI_IMPLEMENT_DEFINE_END

	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData);
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler);

	void SetHide(BOOL bHide) { m_bHide = bHide; }
	BOOL GetHide() { return m_bHide; }
	void SetRunLevel(int nLevel) { m_nRunLevel = nLevel; }
	int	 GetRunLevel() { return m_nRunLevel; }
	void SetStopSignal(BOOL bStop) { m_bStopSignal = bStop; }
	BOOL GetStopSignal() { return m_bStopSignal; }
	void SetBreakSignal(BOOL bBreak);
	BOOL GetBreakSignal() { return m_bBreakSignal; }
	void SetBreakMode(int nMode) { m_nBreakMode = nMode; }
	int  GetBreakMode() { return m_nBreakMode; }
	void SetBreakLineNo(int nLineNo) { m_nBreakLineNo = nLineNo; }
	int  GetBreakLineNo() { return m_nBreakLineNo; }
	void SetBreakName(LPCTSTR lpszName) { m_strBreakName = lpszName; }
	CString GetBreakName() { return m_strBreakName; }
	void SetBreakLevel(int nLevel) { m_nBreakLevel = nLevel; }
	int  GetBreakLevel() { return m_nBreakLevel; }
	void SetStackLevel(int nLevel) { m_nStackLevel = nLevel; }
	int	 GetStackLevel() { return m_nStackLevel; }

	//////////////////////////////////////////add by lcmao
	//��Ӵ򿪺͹ر��ļ��ĺ���
public:
	static void* GetFuncAddress(const char* funcName);
	FILE* OpenFile(const char* fileName, const char* mode);
	int CloseFile(FILE* );
	//MyPy_tracefunc�ص��������
	int PythonTrace(PyFrameObject *frame, int what, PyObject *arg);
	int PyTraceLine(PyFrameObject *frame);
	//��ȡPyFrameObject���ڵĲ�Σ��ײ�Ϊ1��frame��null�Ƿ���0
	int GetStackLevel(PyFrameObject *frame);
	//�ϵ㣬���������������ֱ����һ��ָ���
	int BreakHear(PyFrameObject *frame, int stackLevel, CString& fileName, int line);
	//�����ǰ֡���Ի�ȡ���ı�����ֵ,���acquireLockΪtrue���˺��������ȡpython���߳���
	//������Ѿ�ռ���߳�����ʱ��������������һ��Ҫ����acquireLockΪFALSE�����������
	void OutPutVars(int outputWin, PyFrameObject *frame,BOOLEAN acquireLock = TRUE);
	//���µĽ�������������һ������
	int RunScriptCommandWithNewInt(LPCTSTR lpszCmd);

	static CString m_strPythonHome;
	static HANDLE m_hStdoutPipeRead;
	static HANDLE m_hStdoutPipeWrite;
	static HANDLE m_hStderrPipeRead;
	static HANDLE m_hStderrPipeWrite;
	static UINT __cdecl RedirectOuputThread(LPVOID parameter);
	static UINT __cdecl RedirectErrorThread(LPVOID parameter);
	//��ȡpython���������
	static PyObject* GetPythonAttr(PyObject* obj, char* attrName);
	//���Ժ������ṩ��python�ص�
	static int __cdecl MyPy_tracefunc(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

	static IPlatUI*	m_pCurPlatUI;  //��ǰ��ƽ̨UI
	static PyObject* m_stdout;     //�ض������
	static PyObject* m_stderr;     //�ض������

	PyObject* m_debugArg;  //����python����debug�Ĳ���
	HANDLE m_hDebugEvent;     //�����¼������ڶϵ㣬����ģʽ����Ż����
	int m_nCurCommand;     //��ǰ�յ��ĵ���ָ�IDB_STEPINTO��IDB_NEXT��
	PyFrameObject* m_currentframe;	// ��ǰ����ʱ���frame

	int			m_nOutputVar;      // ��������ID
	int			m_nOutputObject;   // ���󴰿�ID
	int			m_nOutputFunc;	   // ��������ID
	int			m_nOutputStack;    // ��ջ����ID
	/////////////////////////////////////////end add

public:
	void DebugCallback(int nEvent, int nLine);

	BOOL RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId = 1, BOOL bActiveOutput = FALSE);
	BOOL TerminatePipeProcess();
	BOOL ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd = FALSE);

	void SetVarInPlatModule(CString strVarName, CString strVarValue);
	CString GetVarInPlatModule(CString strVarName);

	int  RefreshVarWindow();
	int  RefreshStackWindow();

	int	 SelectSetupType();

// attribute
public:
	PyThreadState * m_myTS;        //����������ʱ��һ���߳�״̬�����߳������ֻ��һ����
	//static PyThreadState *m_mainInterpreter;
	PyObject*	pyPlatModule;	// _platformģ��Ķ���ָ��
	CStringArray m_asDebugScript;	// ���Թ����и��ٽ�ȥ���ļ�

	//Tcl_Interp*	pInterp;		// ������
	int			m_nInterpID;	// ������ID(������Ψһ��ʶ)
	IPlatUI*	pIPlatUI;		// ƽ̨����ӿ�
	DWORD		m_dwRunThreadId;// �ű����е��߳�ID
	HANDLE		m_hPipeProcess;	// �ܵ����̾��
	BOOL		m_bPipeProcessTerminated;	// �ܵ������Ƿ���ֹ��
	BOOL		m_bHide;		// �������Ƿ�����(�ڵ������в��ɼ�)
	CString		m_strInterpName;// ������(��������������)
	CString		m_strScriptFile;// �ű��ļ�
	int			m_nRunMode;		// �ű�����ģʽ
	int			m_nRunState;	// �ű�����״̬
	int			m_nRunLevel;	// �ű����м���(Ȩ��)
	BOOL		m_bStopSignal;	// �ű���ֹ�ź�
	BOOL		m_bBreakSignal;	// �ϵ㼤���ź�
	int			m_nEvalResult;	// �ű����н��
	int			m_nBreakMode;	// ��ǰ�ϵ�ģʽ
	int			m_nBreakLineNo;	// �ϵ��к�
	CString		m_strBreakName;	// �����ϵ���
	int			m_nBreakLevel;	// ��ǰ��ջ���
	CUIntArray	m_auBreakList;	// �ϵ��б�
	int			m_nStackLevel;	// ����Ķ�ջ����
	BOOL		m_bProfileEnable;	// �Ƿ�������и����ʷ���
	int			m_nTDLineCount;	// ��td��ǵ�����
	CMap<int,int,int,int> m_ProfileMap;	// �洢�����еĹ�ϣ��
	INTERP_EXIT_FUNC*	m_lpfnExit;	// �˳��ص�����
	INTERP_DEBUG_FUNC*	m_lpfnDebug;// ���Իص�����

	CString		strErrorInfo;	// ������Ϣ
	CString		strStdOut;		// ��׼���
	CString		strIntervalOut;	// �ڲ������Ϣ
//////////////////////////////////////////////////////////////////////////
// Ҫ����Ĺ��ܽӿ�
//////////////////////////////////////////////////////////////////////////
protected:
	// �ӿڶ���
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

	BEGIN_INTERFACE_PART(PythonInterp, IInterp)
		//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
		// ��ȡ�ӿڵĶ�����Ϣ
		virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData);
		//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_PART_BEGIN
		virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI);// ����ƽ̨UI�ӿ�ָ��
		virtual int __stdcall SetLibPath(LPCTSTR lpszPath);
		virtual int __stdcall IsInterpRunning();
		virtual int __stdcall SetScriptFile(LPCTSTR lpszPathName);
		virtual CString __stdcall GetScriptFile();
		virtual int __stdcall RunScriptFile(LPCTSTR lpszPathName);
		virtual int __stdcall RunScriptCommand(LPCTSTR lpszTclCmd);
		virtual int __stdcall StopScript();
		virtual LPVOID __stdcall GetInterp();
		virtual int __stdcall GetInterpID();
		virtual void __stdcall SetInterpName(LPCTSTR lpszInterptName);
		virtual CString __stdcall GetInterpName();
		virtual int __stdcall GetErrorLine();
		virtual CString __stdcall GetResult();
		virtual int __stdcall SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue);
		virtual CString __stdcall GetVar(LPCTSTR lpszVarName);
		virtual int __stdcall RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc);
		virtual int __stdcall SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue);
		virtual CString __stdcall GetInterpParam(LPCTSTR lpszParamName);
		virtual int __stdcall SetRunMode(int nRunMode);
		virtual int __stdcall GetRunMode();
		virtual int __stdcall SetRunState(int nRunState);
		virtual int __stdcall GetRunState();
		virtual int __stdcall GetCurStackLevel();
		virtual int __stdcall GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord);
		virtual int __stdcall SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam);
		virtual int __stdcall SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks);
		virtual int __stdcall GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks);
		virtual int __stdcall GetEnableProfileAnalyse();
		virtual int __stdcall GetAvailableLineCount();
		virtual LPVOID __stdcall GetProfileMap();
		virtual int __stdcall MethodIsSupport(LPCTSTR lpszMethod);
		virtual int __stdcall InitialDebugMode(CString& strResult);
		virtual int __stdcall RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult);
		virtual int __stdcall CallMethod(LPCTSTR lpszMethod, CStringArray& asParam);
		virtual int __stdcall Compile(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams);
		virtual int __stdcall Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams);
	//}}VCI_INTERFACE_PART_END
	END_INTERFACE_PART(PythonInterp)
	EXPORT_INTERFACE_PART(PythonInterp)
};

// ��������������
CPtrList* GetPythonInterpList();
int  OpenPythonInterp(LPCTSTR lpszInterptName);
CPythonInterp* GetPythonInterp(LPCTSTR lpszInterptName);
CPythonInterp* GetPythonInterp(int nInterpID);
CPythonInterp* GetPythonInterp(LPVOID lpPythonInterp);
BOOL ClosePythonInterp(LPCTSTR lpszInterptName);
BOOL ClosePythonInterp(int nInterpID);
void CloseAllPythonInterp();

#endif // !defined(__VCI_INTERFACE_CPythonInterp_H_INCLUDED__)

#if !defined(__VCI_INTERFACE_CPerlInterp_H_INCLUDED__)
#define __VCI_INTERFACE_CPerlInterp_H_INCLUDED__


#include "vcicomm.h"
#include "IInterp.h"

#define INF_IInterp \
{\
	"IInterp",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"{006045DF-55CF-4283-BBA3-FEC8A63F1E57}",\
	"Perl Interp",\
	"Blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IInterp = INF_IInterp;


// ���ļ����Ե��ļ���
class CDebugFile
{
public:
	CDebugFile();
	~CDebugFile();

	// �������캯��
	CDebugFile(const CDebugFile& other);
	// ���ظ�ֵ�����
	CDebugFile& operator = (const CDebugFile& other);

// function
public:
	int ProcessRunBuffer();
	int RunScriptBuffer(PerlInterpreter*	pInterp);

// attribute
public:
	int				m_nFileIndex;	// �ļ�����
	CString			m_strFile;		// �ļ���
	char*			m_pRunBuffer;	// �ű����л�����
	int				m_nTDLineCount;	// ��td��ǵ�����
	CMap<int,int,int,int> m_ProfileMap;	// �洢�����еĹ�ϣ��
};

typedef CArray<CDebugFile, CDebugFile&> CDebugFileArray;


class CPerlInterp : public CVisualComponent
{
public:
	CPerlInterp();
	~CPerlInterp();

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

	BOOL RunProgramWithPipe(CString strExePath, CString strParam, int nOutputId = 1, BOOL bActiveOutput = FALSE);
	BOOL TerminatePipeProcess();
	BOOL ExecProcess(CString strExePath, CString strParam, BOOL bWaitEnd = FALSE);

	int ProcessRunBuffer();
	int RunScriptBuffer();
	void DebugCallback(int nEvent, int nLine, CString strFile = "");

	// ���ļ�����
	CStringArray* GetDebugScriptList() { return &m_asDebugScript; }
	int  FindDebugFile(CString strFileName);
	int  AddDebugFile(CString strFileName);
	BOOL ClearDebugFile();

	//bool PXPerlCallRetString(LPCSTR szFunc, CString &strRet);
	//bool PXPerlCall(LPCSTR szFunc);

	//bool GetSTDERR(CString &strContents);
	//bool GetSTDOUT(CString &strContents);

	int  ParseErrorLineNo(CString strError);

	int  getVarType(void* sv);
	CString getSVAsString(void* sv);
	int  getVarVal(CString varName, CStringArray& asValue);

	// These are used to create and populate arbitrary variables.
	// Good for setting up data to be processed by the script.
	// They all return TRUE if the 'set' was successful.
	BOOL	setIntVal(CString varName, int value);	// set scalar ($varName) to integer value
	BOOL	setFloatVal(CString varName, double value);	// set scalar ($varName) to double value
	BOOL	setStringVal(CString varName, CString value);	// set scalar ($varName) to string value
	BOOL	setArrayVal(CString varName, CStringArray &value);	// set array (@varName) to CStringArray value
	BOOL	setHashVal(CString varName, CMapStringToString &value);	// set hash (%varName) to CMapStringToString value

	// These are used to get the values of arbitrary variables ($a, $abc, @xyx, %gwxy, etc.)
	// They all return TRUE if the variable was defined and set
	BOOL	getIntVal(CString varName, int &val);	// get scalar ($varName) as an int
	BOOL	getFloatVal(CString varName, double &val);	// get scalar ($varName) as a double
	BOOL	getStringVal(CString varName, CString &val);	// get scalar ($varName) as a string
	BOOL	getArrayVal(CString varName, CStringArray &values);	// get array (@varName) as a CStringArray
	BOOL	getHashVal(CString varName, CMapStringToString &value);	// get hash (%varName) as a CMapStringToString

	int  RefreshVarWindow();
	//int  RefreshFuncWindow();
	int  RefreshStackWindow();

// attribute
public:
	PerlInterpreter*	pInterp;	// ������
	int			m_nInterpID;	// ������ID(������Ψһ��ʶ)
	IPlatUI*	pIPlatUI;		// ƽ̨����ӿ�
	DWORD		m_dwRunThreadId;// �ű����е��߳�ID
	HANDLE		m_hPipeProcess;	// �ܵ����̾��
	BOOL		m_bPipeProcessTerminated;	// �ܵ������Ƿ���ֹ��
	BOOL		m_bHide;		// �������Ƿ�����(�ڵ������в��ɼ�)
	CString		m_strInterpName;// ������(��������������)
	CString		m_strScriptFile;// �ű��ļ�
	char*		m_pRunBuffer;	// �ű����л�����
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
	CString		m_strResult;		// �����Ϣ

	CStringArray m_asDebugScript;	// ���Թ����и��ٽ�ȥ���ļ�

	CDebugFileArray	m_aDebugFile;	// ���ļ����Ե��ļ���Ϣ����
	BOOL		m_bMultiDebugKeepSelect;	// ���ļ�����ʱ���Ƿ񱣳�ѡ��
	BOOL		m_bMultiDebugLoadSource;	// ���ļ�����ʱ���Ƿ���ص����ļ�

	//HANDLE		m_hDebugEvent;     //�����¼������ڶϵ㣬����ģʽ����Ż����
	int			m_nCurCommand;     //��ǰ�յ��ĵ���ָ�IDB_STEPINTO��IDB_NEXT��

	int			m_nOutputVar;      // ��������ID
	int			m_nOutputObject;   // ���󴰿�ID
	int			m_nOutputFunc;	   // ��������ID
	int			m_nOutputStack;    // ��ջ����ID

	void		*m_last_eval;		// return VALUE from last eval() (not success/fail)
	//CString		m_str_eval_error;	// error message from last eval (found in $@)

//////////////////////////////////////////////////////////////////////////
// Ҫ����Ĺ��ܽӿ�
//////////////////////////////////////////////////////////////////////////
protected:
	// �ӿڶ���
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

	BEGIN_INTERFACE_PART(PerlInterp, IInterp)
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
	END_INTERFACE_PART(PerlInterp)
	EXPORT_INTERFACE_PART(PerlInterp)
};

// ��������������
CPtrList* GetInterpList();
CPerlInterp* GetPerlInterp(LPVOID lpInterp);

#endif // !defined(__VCI_INTERFACE_CPerlInterp_H_INCLUDED__)

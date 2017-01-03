#if !defined(__VCI_INTERFACE_I[!output INTERFACE_NAME]_H_INCLUDED__)
#define __VCI_INTERFACE_I[!output INTERFACE_NAME]_H_INCLUDED__


[!if VCITYPE_PROJECT]
// ���ؽ��
enum{
	trpOk,					// ����
	trpFail,				// �򿪹���ʧ��
	trpCancel,				// ����ȡ��
	trpWorking,				// ������������
};

// ����״̬״̬
enum{
	PROJECT_CLOSE,			// �ر�
	PROJECT_IDLE,			// ����
	PROJECT_WORKING,		// ����
};
[!endif]

#define IID_I[!output INTERFACE_NAME] "I[!output INTERFACE_NAME]"

interface I[!output INTERFACE_NAME] : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
[!if VCITYPE_INTERP]
	// ����ƽ̨UI�ӿ�ָ��
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI) = 0;
	// ���ÿ�·��
	virtual int __stdcall SetLibPath(LPCTSTR lpszPath) = 0;
	// �������Ƿ�������
	virtual int __stdcall IsInterpRunning() = 0;
	// ���ýű��ļ���
	virtual int __stdcall SetScriptFile(LPCTSTR lpszPathName) = 0;
	// ��ȡ�ű��ļ���
	virtual CString __stdcall GetScriptFile() = 0;
	// ���нű��ļ�
	virtual int __stdcall RunScriptFile(LPCTSTR lpszPathName) = 0;
	// ִ�нű�����
	virtual int __stdcall RunScriptCommand(LPCTSTR lpszTclCmd) = 0;
	// ֹͣ�ű�����
	virtual int __stdcall StopScript() = 0;
	// ��ȡ������ָ��
	virtual LPVOID __stdcall GetInterp() = 0;
	// ��ȡ������ID
	virtual int __stdcall GetInterpID() = 0;
	// ���ý�������
	virtual void __stdcall SetInterpName(LPCTSTR lpszInterptName) = 0;
	// ��ȡ��������
	virtual CString __stdcall GetInterpName() = 0;
	// ��ȡ�����к�
	virtual int __stdcall GetErrorLine() = 0;
	// ��ȡ�����Ϣ
	virtual CString __stdcall GetResult() = 0;
	// ���ñ���ֵ
	virtual int __stdcall SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue) = 0;
	// ��ȡ����ֵ
	virtual CString __stdcall GetVar(LPCTSTR lpszVarName) = 0;
	// ע����չ����
	virtual int __stdcall RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc) = 0;
	// ������չ����
	virtual int __stdcall SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue) = 0;
	// ��ȡ��չ����
	virtual CString __stdcall GetInterpParam(LPCTSTR lpszParamName) = 0;
	// ��������ģʽ
	virtual int __stdcall SetRunMode(int nRunMode) = 0;
	// ��ȡ����ģʽ
	virtual int __stdcall GetRunMode() = 0;
	// ��������״̬
	virtual int __stdcall SetRunState(int nRunState) = 0;
	// ��ȡ����״̬
	virtual int __stdcall GetRunState() = 0;
	// ��ȡ��ǰ��ջ����
	virtual int __stdcall GetCurStackLevel() = 0;
	// ��ȡ�ؼ����б�
	virtual int __stdcall GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord) = 0;
	// ���õ����¼�
	virtual int __stdcall SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam) = 0;
	// ���öϵ��б�
	virtual int __stdcall SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// ��ȡ�ϵ��б�
	virtual int __stdcall GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// ��ȡ�Ƿ��������ʷ���
	virtual int __stdcall GetEnableProfileAnalyse() = 0;
	// ��ȡ��Ч��������
	virtual int __stdcall GetAvailableLineCount() = 0;
	// ��ȡ�������и��Ǳ�
	virtual LPVOID __stdcall GetProfileMap() = 0;
	// �ж�ָ�������Ƿ�֧��
	virtual int __stdcall MethodIsSupport(LPCTSTR lpszMethod) = 0;
	// ��ʼ������ģʽ
	virtual int __stdcall InitialDebugMode(CString& strResult) = 0;
	// ���µ��Դ�����Ϣ
	virtual int __stdcall RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult) = 0;
	// ���ýű�����
	virtual int __stdcall CallMethod(LPCTSTR lpszMethod, CStringArray& asParam) = 0;
[!endif]
[!if VCITYPE_PROJECT]
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
[!endif]
[!if VCITYPE_POLICY]
	// ���ò��Թ���������ӿ�ָ��
	virtual void __stdcall SetIAgent(LPVOID pIAgent) = 0;
	// ���Գ�ʼ��,����ʱ����
	virtual int	__stdcall init(LPCSTR lpszParameter) = 0;
	// ������ֹʱ����
	virtual int	__stdcall done(LPCSTR lpszParameter) = 0;
	// ���ԺϷ���У��
	virtual int	__stdcall IsValid(LPCSTR lpszConfig) = 0;
	// ����ִ��
	virtual int	__stdcall Exec(LPCSTR lpszConfig) = 0;
	// �����޸�
	virtual int __stdcall Repair(LPCSTR lpszConfig) = 0;
	// ���²���ִ�в���
	virtual int	__stdcall UpdateExecParam(LPCSTR lpszConfig) = 0;
	// ���Ա���
    virtual int	__stdcall Backup(LPCSTR lpszConfig) = 0;	
    // ���Իָ�
    virtual int	__stdcall Restore(LPCSTR lpszConfig) = 0;	
[!endif]
	//}}VCI_INTERFACE_END
};

[!if VCITYPE_INTERP]
// �����˳��ص�����
#define CALLBACK_EVENT_INTERP_EXIT	0x1000
typedef int (INTERP_EXIT_FUNC)(void* lpInterp);

// ������Իص�����
#define CALLBACK_EVENT_INTERP_DEBUG	0x1001
typedef int (INTERP_DEBUG_FUNC)(int nEvent, LPCTSTR lpszScriptFile, int nLine);
[!endif]

#endif // !defined(__VCI_INTERFACE_I[!output INTERFACE_NAME]_H_INCLUDED__)

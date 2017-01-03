#if !defined(AFX_PLATUI_H__48B86E81_729F_11D4_9671_00C0DF258498__INCLUDED_)
#define AFX_PLATUI_H__48B86E81_729F_11D4_9671_00C0DF258498__INCLUDED_
////////////////////////////////////////////////////////////////////////////
//	File:		PlatUI.h
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	header of Platform UI interface object.
////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlatUI.h : header file
//

#include "IPlatUI.h"

/////////////////////////////////////////////////////////////////////////////
// CPlatUI command target

#define INF_IPlatUI \
{\
	"IPlatUI",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"org.blueui.platui",\
	"PlatUI VCI",\
	"Blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IPlatUI = INF_IPlatUI;

class CPlatUI : public CVisualComponent
{
public:
	CPlatUI();           // protected constructor used by dynamic creation
	virtual ~CPlatUI();

// Attributes
public:

// Interface define
public:
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

    BEGIN_INTERFACE_PART(PlatUI, IPlatUI)

		// ƽ̨����
		STDMETHOD_( CString , GetAppName) ();				// ��ȡӦ�ó�����
		STDMETHOD_( CString , GetPlatPath) ();				// ��ȡƽ̨·��
		STDMETHOD_( CString , GetPlatVersion) ();			// ��ȡƽ̨�汾
		STDMETHOD_( int , GetCurrentLanguage) ();			// ��ȡ��ǰ����
		STDMETHOD_( CString , GetPlatRegistry) ();			// ��ȡƽ̨ע����·��
		STDMETHOD_( CString , GetPlatCopyRight) ();			// ��ȡƽ̨��Ȩ�ַ���
		STDMETHOD_( CString , GetPlatHomeURL) ();			// ��ȡ��ҳURL
		STDMETHOD_( CString , GetPlatDownloadURL) ();		// ��ȡ����URL
		STDMETHOD_( int  , SendMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ������Ϣ
		STDMETHOD_( int  , ProcessMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ƽ̨����Ϣ����
		STDMETHOD_( int  , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam);	// ����ƽ̨����
		STDMETHOD_( BOOL , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);	// ����ƽ̨����

		// ������ڲ���
		STDMETHOD_( int  , AddOutput) (CString strName);	//����������
		STDMETHOD_( BOOL , DelOutput) (int nOT);			//ɾ���������
		STDMETHOD_( BOOL , ActiveOutput) (int nOT);		//�����������
		STDMETHOD_( BOOL , Output) (int nOT, LPCTSTR lpszOut);//�����Ϣ
		STDMETHOD_( BOOL , OutputFormat) (int nOT, int nFmt, LPCTSTR lpszOut);//�����ʽ��Ϣ
		STDMETHOD_( BOOL , OutputVerify) (int nOT, LPCTSTR lpszOut, LPCTSTR lpszVerify);//�����Ϣ����У���ַ����Ƚ�,��һ�µ��ַ���ʾΪ��ɫ
		STDMETHOD_( BOOL , OutputSet) (int nOT, LPCTSTR lpszSet);//�����������
		STDMETHOD_( CString , OutputGet) (int nOT, LPCTSTR lpszParam);// ��ȡ���������Ϣ
		STDMETHOD_( BOOL , OutputClear) (int nOT);			//����������
		STDMETHOD_( BOOL , OutputAddTreeItems) (int nOT, CTTreeItemArray_* pItem);// ����������������

		// �û��˵�����
		STDMETHOD_( void* , GetCommandBars) ();			// ��ȡCommandBars
		STDMETHOD_( BOOL , AddExtMenu) (CMenu*& pMenu);	// �����չ�˵�
		STDMETHOD_( BOOL , DelExtMenu) (CMenu*& pMenu);	// ɾ����չ�˵�
		STDMETHOD_( BOOL , AddSubMenu) (CString strMainMenu, int nIndex, CMenu*& pMenu, LPVOID pProc, LPVOID pUpdateProc);	// �����չ�Ӳ˵�
		STDMETHOD_( BOOL , DelSubMenu) (CString strMainMenu, int nIndex, CMenu*& pMenu);	// ɾ����չ�Ӳ˵�
		STDMETHOD_( BOOL , TrackPopupMenu) (CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd);	// �����˵�

		// ״̬�в���
		STDMETHOD_( BOOL , AddStatusPane) (int& nID, int nWidth);	//���״̬��Pane
		STDMETHOD_( BOOL , DelStatusPane) (int nID);				//ɾ��״̬��Pane
		STDMETHOD_( BOOL , SetPaneText) (int nID, CString strPane);	//����Pane����
		STDMETHOD_( BOOL , SetPaneWidth) (int nID, int nWidth);		//����Pane���
		STDMETHOD_( BOOL , SetPaneImage) (int nID, int nGroup, int nImage);//����Paneͼ��

		// ���ڲ���
		STDMETHOD_( CDocument*, GetActiveDocument) ();	// ��ȡ��ǰ������ĵ�����
		STDMETHOD_( BOOL , CreateDockingPane) (CString strOwmId, CString strTitle, int nID, int nDirection, int nAttachType, HICON hIcon, int nAttachPane);	// ����DockingPane
		STDMETHOD_( BOOL , CreateDockingPane) (CString strOwmId, int nID);	// ����DockingPane
		STDMETHOD_( CWnd*, GetDockingPaneWnd) (int nPaneID);	// ��ȡDockingPane�����Ĵ��ڶ���
		STDMETHOD_( int , GetDockingPaneIdByName) (CString strName);	// ��ȡDockingPane ID(��������)
		STDMETHOD_( BOOL , ActiveDockingPane) (int nPaneID, BOOL bShow = TRUE);	// ����DockingPane
		STDMETHOD_( BOOL , CloseDockingPane) (int nPaneID);	// �ر�DockingPane����

		// ��־����
		STDMETHOD_( BOOL , AddSysLogInfo) (int nType, CString strFrom, CString strDesc);//������־��Ϣ

		// �û��������

		// OWM����
		STDMETHOD_( BOOL , IsOwmExist) (CString strOwmId);				//��ѯģ���Ƿ����
		STDMETHOD_( BOOL , SendOwmCmd) (CString strOwmId, int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);//����OWM����
		STDMETHOD_( BOOL , LoadOwm) (CString strOwmName, int& nID, BOOL bLoad);//������չģ��

		// �ĵ�����
		STDMETHOD_( CDocument* , OpenDocumentFile) (LPCTSTR lpszFileName, LPCTSTR lpszOwmId);		//���ĵ�

		// ���Դ��ڲ���
		STDMETHOD_( CWnd* , GetPropertyGridWnd) ();	// ��ȡ������ָ��
		STDMETHOD_( BOOL , EnableUpdatePropertyGridContent) ();	// �ж��Ƿ���Ը���������
		STDMETHOD_( void , UpdatePropertyGridContent) ();	// ˢ�����Դ�������
		STDMETHOD_( void , BeginUpdatePropertyGridContent) ();	// ��ʼ���Դ���ˢ��
		STDMETHOD_( void , EndUpdatePropertyGridContent) ();	// �������Դ���ˢ��

		// VCI������ýӿ�
		STDMETHOD_( void* , CreateVciObject) (CString strVciId, CString strInstName);	// ����һ���������
		STDMETHOD_( BOOL , ReleaseVciObject) (void* pObject, BOOL bForceDelete);	// �ر�һ���������
		STDMETHOD_( BOOL , ReleaseObjectByInstanceName) (CString strInstName);	// �ر�һ���������(ָ��ʵ����)
		STDMETHOD_( void* , GetObjectByInstanceName) (CString strInstName);	// ��ȡһ���������(����ʵ����)
		STDMETHOD_( void* , GetObjectControlPtrByInstancePtr) (LPVOID pVciObj);	// ��ȡһ�����������ƽӿ�(�������ʵ���ӿ�ָ��)
		STDMETHOD_( int , GetInstalledVci) (CStringArray& asVciId);	// ��ȡ���а�װ�����ID
		STDMETHOD_( CString , GetVciName) (CString strId);	// ��ȡһ�������(�������ID)
		STDMETHOD_( CString , GetVciPath) (CString strId);	// ��ȡһ�����·��(�������ID)
		STDMETHOD_( CString , GetVciVersion) (CString strId);	// ��ȡһ������汾(�������ID)
		STDMETHOD_( CString , GetVciProvider) (CString strId);	// ��ȡһ�������������Ϣ(�������ID)
		STDMETHOD_( CString , GetVciType) (CString strId);	// ��ȡһ���������(�������ID)
		STDMETHOD_( CString , GetVciClass) (CString strId);	// ��ȡһ�������ӿ�ID(�������ID)
		STDMETHOD_( CString , GetVciInterfaceFile) (CString strId);	// ��ȡһ������ӿڶ����ļ�(�������ID)
		STDMETHOD_( CTime , GetVciBuildTime) (CString strId);	// ��ȡһ���������ʱ��(�������ID)
		STDMETHOD_( CString , GetVciGUID) (CString strId);	// ��ȡһ�����GUID(�������ID)
		STDMETHOD_( CString , GetVciAboutPage) (CString strId);	// ��ȡһ����������ļ�λ��(�������ID)
		STDMETHOD_( int , GetVciDocExecuteModule) (CString strId, CString strDocId, CString strExecuteType, CStringArray& arExecuteModule);	// ��ȡ����ִ��һ���ĵ�����չģ��ID�б�(�������Id���ĵ�Id)
		STDMETHOD_( CString , GetVciDocIcon) (CString strId, CString strDocId, CString strIconType);	// ��ȡ�ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������)
		STDMETHOD_( void , SetVciDocIcon) (CString strId, CString strDocId, CString strIconType, HWND hWnd);	// �����ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������,���ھ��)
		STDMETHOD_( void , SetDocumentTitle) (CString strTitle, int nModifyType, HWND hWnd);	// �����ĵ��ı���(���ݴ��ھ��)
		STDMETHOD_( int , GetVciObjects) (CString strId, CPtrArray& auVciObj);	// ��ȡһ�����������ʵ������ָ��
		STDMETHOD_( CString , GetInstanceNameByObject) (CString strId, LPVOID pVciObj);	// ��ȡһ�����ʵ�������ʵ����
		STDMETHOD_( int , GetInstanceRefCount) (CString strId, LPVOID pVciObj);	// ��ȡһ�����ʵ����������ü���
		STDMETHOD_( int , EnumVciParameters) (CString strId, CStringArray& asParamName);	// ö��һ��VCI�������չ����
		STDMETHOD_( BOOL , GetVciParameter) (CString strId, CString strName, CString& strValue, CString& strDefaultValue);	// ��ȡһ��VCI����Ĳ���
		STDMETHOD_( BOOL , SetVciParameter) (CString strId, CString strName, CString strValue);	// ����һ��VCI����Ĳ���

		// ��ʱ������
		STDMETHOD_( void* , AddTimerTask) (CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds, BOOL bStart);	// ��Ӷ�ʱ������
		STDMETHOD_( void* , AddTimerTask) (CString strTaskName, LPVOID lpFunction, int nSeconds, BOOL bStart);	// ��Ӷ�ʱ������
		STDMETHOD_( void* , GetTimerTask) (CString strTaskName);	// ������������ȡ��ʱ����ָ��
		STDMETHOD_( BOOL , DeleteTimerTask) (LPVOID lpTimerTask);	// ɾ����ʱ����(��������ָ��)
		STDMETHOD_( BOOL , DeleteTimerTask) (CString strTaskName);	// ɾ����ʱ����(����������)
		STDMETHOD_( void , StartTimerTask) (LPVOID lpTimerTask);	// ������ʱ����
		STDMETHOD_( void , SetTimerStartTime) (LPVOID lpTimerTask, int tStart);	// ���ö�ʱ����ʼʱ��
		STDMETHOD_( void , SetTimerEndTime) (LPVOID lpTimerTask, int tEnd);	// ���ö�ʱ����ʼʱ��
		STDMETHOD_( void , SetTimerTrigger) (LPVOID lpTimerTask, CString strTrigger);	// ���ö�ʱ���񴥷������ű�
		STDMETHOD_( void , SetTimerExecCount) (LPVOID lpTimerTask, int nCount);	// ���ö�ʱ����ִ�д���

		// ���߲���
		STDMETHOD_( int , GetInstalledTool) (CStringArray& asToolId);	// ��ȡ���а�װ�Ĺ���ID
		STDMETHOD_( CString , GetToolName) (CString strId);	// ��ȡһ��������(���ݹ���ID)
		STDMETHOD_( CString , GetToolPath) (CString strId);	// ��ȡһ������·��(���ݹ���ID)
		STDMETHOD_( CString , GetToolVersion) (CString strId);	// ��ȡһ�����߰汾(���ݹ���ID)
		STDMETHOD_( CString , GetToolProvider) (CString strId);	// ��ȡһ�����߿�������Ϣ(���ݹ���ID)
		STDMETHOD_( BOOL , RunTool) (CString strId);	// ���й���(���ݹ���ID)

	END_INTERFACE_PART(PlatUI)
	EXPORT_INTERFACE_PART(PlatUI)

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATUI_H__48B86E81_729F_11D4_9671_00C0DF258498__INCLUDED_)

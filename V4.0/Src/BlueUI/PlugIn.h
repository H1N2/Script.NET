////////////////////////////////////////////////////////////////////////////
//	File:		PlugIn.h
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	PlugIn Interface
////////////////////////////////////////////////////////////////////////////
#ifndef __PLUGIN_H_
#define __PLUGIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �����ǰ�ļ���״̬
enum {
	PLUGIN_UNLOAD,		// ���δ�����κ���Ϣ
	PLUGIN_LOAD_NAME,	// ������������ֺ�·��
	PLUGIN_LOAD_INFO,	// ��������������ļ�����Ϣ
	PLUGIN_LOAD_LIB,	// ���������ִ����
};

//
// VCIʵ�����
//
class CVciObject : public CObject
{
public:
	CVciObject()
	{
		m_pVciObj		= NULL;
		m_pIVciControl	= NULL;
		m_strInstName	= "";
		m_nRefCount		= 0;
	}
	~CVciObject(){};

	CVciObject(const CVciObject& other)
	{
		m_pVciObj		= other.m_pVciObj;
		m_pIVciControl	= other.m_pIVciControl;
		m_strInstName	= other.m_strInstName;
		m_nRefCount		= other.m_nRefCount;
	}

	CVciObject& operator = (const CVciObject& other)
	{
		m_pVciObj		= other.m_pVciObj;
		m_pIVciControl	= other.m_pIVciControl;
		m_strInstName	= other.m_strInstName;
		m_nRefCount		= other.m_nRefCount;
		return *this;
	}

public:
	LPVOID	m_pVciObj;		// VCI����ָ��
	LPVOID	m_pIVciControl;	// VCI���ƽӿ�
	CString	m_strInstName;	// VCI����ʵ����
	int		m_nRefCount;	// �������ü���
};

typedef CArray<CVciObject, CVciObject&> CVciObjectArray;

//
// �ĵ���չ����Ϣ��
//
class COwmDocExtend : public CObject
{
public:
	COwmDocExtend();
	~COwmDocExtend();

	COwmDocExtend(const COwmDocExtend& other)
	{
		m_strDocName	= other.m_strDocName;
		m_strDocId		= other.m_strDocId;
		m_strOwmId		= other.m_strOwmId;
		m_strIcon		= other.m_strIcon;
		m_bEnableLink	= other.m_bEnableLink;
		m_arDocExt.RemoveAll();
		m_arExecuteModuleType.RemoveAll();
		m_arExecuteModule.RemoveAll();
		m_arDocIconType.RemoveAll();
		m_arDocIconFile.RemoveAll();
		int i;
		for(i=0; i<other.m_arDocExt.GetSize(); i++)
		{
			m_arDocExt.Add(other.m_arDocExt[i]);
		}
		for(i=0; i<other.m_arExecuteModuleType.GetSize(); i++)
		{
			m_arExecuteModuleType.Add(other.m_arExecuteModuleType[i]);
		}
		for(i=0; i<other.m_arExecuteModule.GetSize(); i++)
		{
			m_arExecuteModule.Add(other.m_arExecuteModule[i]);
		}
		for(i=0; i<other.m_arDocIconType.GetSize(); i++)
		{
			m_arDocIconType.Add(other.m_arDocIconType[i]);
		}
		for(i=0; i<other.m_arDocIconFile.GetSize(); i++)
		{
			m_arDocIconFile.Add(other.m_arDocIconFile[i]);
		}
	}

	COwmDocExtend& operator = (const COwmDocExtend& other)
	{
		m_strDocName	= other.m_strDocName;
		m_strDocId		= other.m_strDocId;
		m_strOwmId		= other.m_strOwmId;
		m_strIcon		= other.m_strIcon;
		m_bEnableLink	= other.m_bEnableLink;
		m_arDocExt.RemoveAll();
		m_arExecuteModuleType.RemoveAll();
		m_arExecuteModule.RemoveAll();
		m_arDocIconType.RemoveAll();
		m_arDocIconFile.RemoveAll();
		int i;
		for(i=0; i<other.m_arDocExt.GetSize(); i++)
		{
			m_arDocExt.Add(other.m_arDocExt[i]);
		}
		for(i=0; i<other.m_arExecuteModuleType.GetSize(); i++)
		{
			m_arExecuteModuleType.Add(other.m_arExecuteModuleType[i]);
		}
		for(i=0; i<other.m_arExecuteModule.GetSize(); i++)
		{
			m_arExecuteModule.Add(other.m_arExecuteModule[i]);
		}
		for(i=0; i<other.m_arDocIconType.GetSize(); i++)
		{
			m_arDocIconType.Add(other.m_arDocIconType[i]);
		}
		for(i=0; i<other.m_arDocIconFile.GetSize(); i++)
		{
			m_arDocIconFile.Add(other.m_arDocIconFile[i]);
		}
		return *this;
	}

public:
	CString	m_strDocName;	// �ĵ���
	CString	m_strDocId;		// �ĵ�ID
	CString	m_strOwmId;		// ����OWMģ��ID
	CString	m_strIcon;		// ͼ���ļ�
	BOOL	m_bEnableLink;	// �Ƿ���Խ�������
	CStringArray m_arDocExt;// �ļ���׺�б�
	CStringArray m_arExecuteModuleType;	// ����ִ�д��ļ�������ģ������
	CStringArray m_arExecuteModule;	// ����ִ�д��ļ�������ģ��
	CStringArray m_arDocIconType;	// �ĵ�ͼ������
	CStringArray m_arDocIconFile;	// �ĵ�ͼ���ļ�
};

typedef CArray<COwmDocExtend, COwmDocExtend&> COwmDocExtendArray;

//
// DockingPane��Ϣ��
//
class COwmDockingPane : public CObject
{
public:
	COwmDockingPane()
	{
		m_bFileDefined = FALSE;
		m_strName	= "";
		m_nId		= 0;
		m_nDirection= 0;
		m_strIcon	= "";
		m_nAttachType= PANE_ATTACH_WND;
		m_bAutoCreate= FALSE;
		m_bProjectCreate= FALSE;
		m_bSave		= FALSE;
		m_pWnd		= NULL;
		m_strOwmId	= "";
	}
	~COwmDockingPane() {};

	COwmDockingPane(const COwmDockingPane& other)
	{
		m_bFileDefined	= other.m_bFileDefined;
		m_strName		= other.m_strName;
		m_nId			= other.m_nId;
		m_nDirection	= other.m_nDirection;
		m_strIcon		= other.m_strIcon;
		m_nAttachType	= other.m_nAttachType;
		m_bAutoCreate	= other.m_bAutoCreate;
		m_bProjectCreate= other.m_bProjectCreate;
		m_bSave			= other.m_bSave;
		m_pWnd			= other.m_pWnd;
		m_strOwmId		= other.m_strOwmId;
	}

	COwmDockingPane& operator = (const COwmDockingPane& other)
	{
		m_bFileDefined	= other.m_bFileDefined;
		m_strName		= other.m_strName;
		m_nId			= other.m_nId;
		m_nDirection	= other.m_nDirection;
		m_strIcon		= other.m_strIcon;
		m_nAttachType	= other.m_nAttachType;
		m_bAutoCreate	= other.m_bAutoCreate;
		m_bProjectCreate= other.m_bProjectCreate;
		m_bSave			= other.m_bSave;
		m_pWnd			= other.m_pWnd;
		m_strOwmId		= other.m_strOwmId;
		return *this;
	}

public:
	BOOL	m_bFileDefined;	// �Ƿ��������ļ���Ԥ�����(��ЩDockingPane�Ƕ�̬������,����Tree����)
	CString	m_strName;		// Pane��
	int		m_nId;			// PaneID
	int		m_nDirection;	// ��ʾλ��
	CString	m_strIcon;		// ͼ���ļ�λ��
	int		m_nAttachType;	// �����Ĵ�������
	BOOL	m_bAutoCreate;	// �Ƿ���ģ�����ʱ�Զ�����
	BOOL	m_bProjectCreate;// �Ƿ��ڴ򿪹���ʱ�Զ�����
	BOOL	m_bSave;		// �Ƿ���ƽ̨�ر�ʱ����λ��
	CWnd*	m_pWnd;
	CString	m_strOwmId;		// ����OWMģ��ID
};

typedef CArray<COwmDockingPane, COwmDockingPane&> COwmDockingPaneArray;

//
// ͸��ͼ��Ϣ��
//
class COwmPerspective : public CObject
{
public:
	COwmPerspective();
	~COwmPerspective();

	COwmPerspective(const COwmPerspective& other)
	{
		m_strName		= other.m_strName;
		m_strId			= other.m_strId;
		m_strOwmId		= other.m_strOwmId;
		m_strViewName	= other.m_strViewName;
		m_arDockView.RemoveAll();
		for(int i=0; i<other.m_arDockView.GetSize(); i++)
		{
			m_arDockView.Add(other.m_arDockView[i]);
		}
	}

	COwmPerspective& operator = (const COwmPerspective& other)
	{
		m_strName		= other.m_strName;
		m_strId			= other.m_strId;
		m_strOwmId		= other.m_strOwmId;
		m_strViewName	= other.m_strViewName;
		m_arDockView.RemoveAll();
		for(int i=0; i<other.m_arDockView.GetSize(); i++)
		{
			m_arDockView.Add(other.m_arDockView[i]);
		}
		return *this;
	}

public:
	CString	m_strName;		// ͸��ͼ��
	CString	m_strId;		// ͸��ͼID
	CString	m_strOwmId;		// ����OWMģ��ID
	CString	m_strViewName;	// view��
	CStringArray m_arDockView;// ͣ����ͼ�б�
};

typedef CArray<COwmPerspective, COwmPerspective&> COwmPerspectiveArray;


//
// VCI��չ������
//
class CVciParameter : public CObject
{
public:
	CVciParameter();
	~CVciParameter();

	CVciParameter(const CVciParameter& other)
	{
		m_strName		= other.m_strName;
		m_strValue		= other.m_strValue;
		m_strDefaultValue= other.m_strDefaultValue;
	}

	CVciParameter& operator = (const CVciParameter& other)
	{
		m_strName		= other.m_strName;
		m_strValue		= other.m_strValue;
		m_strDefaultValue= other.m_strDefaultValue;
		return *this;
	}

public:
	CString	m_strName;			// ������
	CString	m_strValue;			// ����ֵ
	CString	m_strDefaultValue;	// ȱʡֵ
};

typedef CArray<CVciParameter, CVciParameter&> CVciParameterArray;

//
// VCI��չ�˵���
//
class CVciMenu : public CObject
{
public:
	CVciMenu();
	~CVciMenu();

	CVciMenu(const CVciMenu& other)
	{
		m_strMainMenu	= other.m_strMainMenu;
		m_strSubMenu	= other.m_strSubMenu;
		m_strAction		= other.m_strAction;
	}

	CVciMenu& operator = (const CVciMenu& other)
	{
		m_strMainMenu	= other.m_strMainMenu;
		m_strSubMenu	= other.m_strSubMenu;
		m_strAction		= other.m_strAction;
		return *this;
	}

public:
	CString	m_strMainMenu;			// ���˵�
	CString	m_strSubMenu;			// �Ӳ˵�
	CString	m_strAction;			// ����
};

typedef CArray<CVciMenu, CVciMenu&> CVciMenuArray;


//
// �����Ϣ��
//
class CPlugIn : public CObject
{
public:
	CPlugIn();	
	~CPlugIn();

	CPlugIn(CString strPath);

	CPlugIn& operator = (CPlugIn& other)
	{
		m_nLoadState= other.m_nLoadState;
		m_nOwmId	= other.m_nOwmId;
		m_strId		= other.m_strId;
		m_strName	= other.m_strName;
		m_strVersion= other.m_strVersion;
		m_strProvider= other.m_strProvider;
		m_strPath	= other.m_strPath;
		m_strType	= other.m_strType;
		m_strClass	= other.m_strClass;
		m_strClassVersion= other.m_strClassVersion;
		m_strProcess= other.m_strProcess;
		m_strFile	= other.m_strFile;
		m_strInterfaceFile = other.m_strInterfaceFile;
		m_hVciHandle= other.m_hVciHandle;
		m_pVciObj	= other.m_pVciObj;
		m_pIVciControl=other.m_pIVciControl;
		m_nRefCount	= other.m_nRefCount;
		m_nType		= other.m_nType;
		m_bAutoCreate	= other.m_bAutoCreate;
		m_strAutoCreateInstanceName	= other.m_strAutoCreateInstanceName;
		m_pDocTemplate	=other.m_pDocTemplate;
		m_pControlBar	=other.m_pControlBar;
		m_strInitParam	= other.m_strInitParam;
		m_strDoneParam	= other.m_strDoneParam;
		m_strAboutPage	= other.m_strAboutPage;
		int i;
		for(i=0; i<other.m_arDocExtend.GetSize(); i++)
		{
			m_arDocExtend.Add(other.m_arDocExtend[i]);
		}
		for(i=0; i<other.m_arDockingPane.GetSize(); i++)
		{
			m_arDockingPane.Add(other.m_arDockingPane[i]);
		}
		for(i=0; i<other.m_arParameter.GetSize(); i++)
		{
			m_arParameter.Add(other.m_arParameter[i]);
		}
		for(i=0; i<other.m_arMenu.GetSize(); i++)
		{
			m_arMenu.Add(other.m_arMenu[i]);
		}
		return *this;
	}

	int  GetLoadState() { return m_nLoadState; }

	void* OpenVciLib(CString strInterface);
	BOOL CloseVci();
	BOOL CanClose();
	BOOL HasLoad() { return m_hVciHandle != NULL; }

	void* CreateObject(CString strInstName = "");
	BOOL ReleaseObject(LPVOID pVciObj, BOOL bForceDelete = FALSE);
	BOOL ReleaseObject(CString strInstName);
	void* GetObjectByInstanceName(CString strInstName);
	void* GetObjectControlPtrByInstancePtr(LPVOID	pVciObj);
	BOOL IsInstancePtr(LPVOID	pVciObj);

	BOOL LoadInfo();
	void ShowAboutBox();
	CTime GetPlugInBuildTime();
	CString GetPlugInGUID();
	BOOL LoadPlugIn(BOOL bForce = FALSE);
	BOOL LoadVciPlugIn();
	BOOL LoadOwmPlugIn();
	BOOL LoadDockingPane(int nID, BOOL bProjectLoad = FALSE);
	BOOL IsVciOwm() { return (m_strType == "owm"); }
	IVciOwm* GetIVciOwm() { return (IVciOwm*)m_pVciObj; }
	IProject* GetIProject() { return (IProject*)m_pVciObj; }
	BOOL IsVciInterp() { return (m_strType == "interp"); }
	IInterp* GetIInterp() { return (IInterp*)m_pVciObj; }
	BOOL IsVciLicense() { return (m_strType == "license"); }

	int  EnumParameters(CStringArray& asName);
	BOOL GetParameter(CString strName, CString& strValue, CString& strDefaultValue);
	BOOL SetParameter(CString strName, CString strValue);

	BOOL AddOwmSubMenu(CString strMainMenu, CString strSubMenu, CString strAction);

	// OWM����
	void NewOwmFrame(LPCTSTR lpszPath = NULL);
	void CloseOwmFrame();
	BOOL NewOwmControlBar();
	BOOL CloseOwmControlBar();
	BOOL ShowOwmControlBar(BOOL bShow);
	BOOL ShowOwmView(BOOL bShow, BOOL bActive);
	BOOL CanCloseOwm();
	BOOL CloseOwm();
	CDocument* OpenDocument(LPCTSTR lpszFileName);
	int  RegisterOption(COWMOptionArray& aOWMOption);
	int	 RegisterPropertyPage(CVciPropertyPageArray& aPropertyPage);

	// Project����
	IProject* CreateProjectObject(CString strInstName);
	BOOL ReleaseProjectObject(LPVOID pVciObj, BOOL bForceDelete = FALSE);

	// Interp����
	BOOL CanCloseInterp();

	// ��Ϣ����
	int  SendMessage(CVciMessage* pIn, CVciMessage* ppOut);

public:
	int		m_nLoadState;		// �����ǰ�ļ���״̬
	int		m_nOwmId;			// OWMģ��ID
	CString	m_strId;			// ���ID
	CString	m_strName;			// �����
	CString	m_strVersion;		// ����汾
	CString	m_strProvider;		// ������
	CString	m_strPath;			// ���·��
	CString	m_strType;			// �������
	CString	m_strClass;			// �����ID
	CString	m_strClassVersion;	// �����汾
	CString	m_strProcess;		// ������ڽ���
	CString m_strFile;			// DLL�ļ�
	CString	m_strInterfaceFile;	// ͷ�ļ�
	HINSTANCE m_hVciHandle;		// ����VCI dll�ľ��
	CVciObjectArray	m_aVciObj;	// VCIʵ����������
	LPVOID	m_pVciObj;			// VCI�������
	LPVOID	m_pIVciControl;		// VCI���ƽӿ�
	int		m_nRefCount;		// ���ü���(����OWMģ��Ĵ��ڼ���)
	UINT	m_nType;			// OWMģ���������Ϣ(XML�л�ȡ)
	BOOL	m_bAutoCreate;		// �Ƿ��Զ�����ʵ��
	CString	m_strAutoCreateInstanceName;	// �Զ�����ʵ����ʵ����
	CString	m_strInitParam;		// ��ʼ������
	CString	m_strDoneParam;		// ��������
	CString	m_strAboutPage;		// ����ҳ��λ��
	CMultiDocTemplate* m_pDocTemplate; // OWM�ĵ�ģ��ָ��
	CControlBar* m_pControlBar;	// OWM Control Bar
	COwmDocExtendArray m_arDocExtend;	// �ĵ���չ������
	COwmDockingPaneArray m_arDockingPane;	// DockingPane����
	CVciParameterArray	m_arParameter;	// ��չ����
	CVciMenuArray		m_arMenu;	// ��չ�˵�
};

typedef CArray<CPlugIn, CPlugIn&> CPlugInArray;


#endif //__PLUGIN_H_
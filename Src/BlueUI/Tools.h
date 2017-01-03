////////////////////////////////////////////////////////////////////////////
//	File:		Tools.h
//	Version:	1.0.0.0
//	Created:	2007-08-26
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Tools manager
////////////////////////////////////////////////////////////////////////////
#ifndef __TOOLS_H_
#define __TOOLS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// ����״̬��Pane��
//
class CToolStatusPane : public CObject
{
public:
	CToolStatusPane();
	~CToolStatusPane();

	CToolStatusPane(const CToolStatusPane& other)
	{
		m_nID			= other.m_nID;
		m_nWidth		= other.m_nWidth;
		m_strText		= other.m_strText;
		m_strTooltip	= other.m_strTooltip;
		m_strIcon		= other.m_strIcon;
		m_strAction		= other.m_strAction;
	}

	CToolStatusPane& operator = (const CToolStatusPane& other)
	{
		m_nID			= other.m_nID;
		m_nWidth		= other.m_nWidth;
		m_strText		= other.m_strText;
		m_strTooltip	= other.m_strTooltip;
		m_strIcon		= other.m_strIcon;
		m_strAction		= other.m_strAction;
		return *this;
	}

public:
	UINT	m_nID;				// Pane ID
	int		m_nWidth;			// Pane���
	CString	m_strText;			// Pane����
	CString	m_strTooltip;		// Pane��ʾ��Ϣ
	CString	m_strIcon;			// Paneͼ��
	CString	m_strAction;		// ����
};

typedef CArray<CToolStatusPane, CToolStatusPane&> CToolStatusPaneArray;

//
// DockingPane��Ϣ��
//
class CToolDockingPane : public CObject
{
public:
	CToolDockingPane()
	{
		m_bFileDefined = FALSE;
		m_strName	= "";
		m_nId		= 0;
		m_nDirection= 0;
		m_strIcon	= "";
		m_nAttachType= PANE_ATTACH_WND;
		m_bAutoCreate= FALSE;
		m_bSave		= FALSE;
		m_strOwmId	= "";
	}
	~CToolDockingPane() {};

	CToolDockingPane(const CToolDockingPane& other)
	{
		m_bFileDefined	= other.m_bFileDefined;
		m_strName		= other.m_strName;
		m_nId			= other.m_nId;
		m_nDirection	= other.m_nDirection;
		m_strIcon		= other.m_strIcon;
		m_nAttachType	= other.m_nAttachType;
		m_bAutoCreate	= other.m_bAutoCreate;
		m_bSave			= other.m_bSave;
		m_strOwmId		= other.m_strOwmId;
	}

	CToolDockingPane& operator = (const CToolDockingPane& other)
	{
		m_bFileDefined	= other.m_bFileDefined;
		m_strName		= other.m_strName;
		m_nId			= other.m_nId;
		m_nDirection	= other.m_nDirection;
		m_strIcon		= other.m_strIcon;
		m_nAttachType	= other.m_nAttachType;
		m_bAutoCreate	= other.m_bAutoCreate;
		m_bSave			= other.m_bSave;
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
	BOOL	m_bSave;		// �Ƿ���ƽ̨�ر�ʱ����λ��
	CWnd*	m_pWnd;
	CString	m_strOwmId;		// ����OWMģ��ID
};

typedef CArray<CToolDockingPane, CToolDockingPane&> CToolDockingPaneArray;

//
// ������Ϣ��
//
class CTool : public CObject
{
public:
	CTool();	
	~CTool();

	CTool(CString strPath);

	CTool& operator = (const CTool& other)
	{
		m_nMenuIndex= other.m_nMenuIndex;
		m_strId		= other.m_strId;
		m_strName	= other.m_strName;
		m_strVersion= other.m_strVersion;
		m_strProvider= other.m_strProvider;
		m_strPath	= other.m_strPath;
		m_strCategory= other.m_strCategory;
		m_bShowMenu	= other.m_bShowMenu;
		m_bShowTask	= other.m_bShowTask;
		m_bShowToolbar = other.m_bShowToolbar;
		m_bShowStatusbar = other.m_bShowStatusbar;
		m_strType	= other.m_strType;
		m_strVciId	= other.m_strVciId;
		m_strInstanceName	= other.m_strInstanceName;
		m_strExecFile= other.m_strExecFile;
		m_strExecCommand	= other.m_strExecCommand;
		for(int i=0; i<other.m_arStatusPane.GetSize(); i++)
		{
			m_arStatusPane.Add((CToolStatusPane &)other.m_arStatusPane[i]);
		}
		for(int i=0; i<other.m_arDockingPane.GetSize(); i++)
		{
			m_arDockingPane.Add((CToolDockingPane &)other.m_arDockingPane[i]);
		}
		return *this;
	}

	BOOL LoadInfo();
	void ShowAboutBox();
	BOOL Init();
	BOOL LoadDockingPane(int nID);
	BOOL Run();

public:
	int		m_nMenuIndex;	// �˵�������λ��
	CString	m_strId;		// ����ID
	CString	m_strName;		// ������
	CString	m_strVersion;	// ���߰汾
	CString	m_strProvider;	// ������
	CString	m_strPath;		// ����·��
	CString	m_strCategory;	// ����
	BOOL	m_bShowMenu;	// �Ƿ��ڲ˵�����ʾ
	BOOL	m_bShowTask;	// �Ƿ�����������ʾ
	BOOL	m_bShowToolbar;	// �Ƿ��ڹ�������ʾ
	BOOL	m_bShowStatusbar;	// �Ƿ���״̬����ʾ
	CString	m_strType;		// ִ������
	CString	m_strVciId;		// ִ��VCI id
	CString	m_strInstanceName;// ִ��ʵ����
	CString	m_strExecFile;	// ִ���ļ�
	CString	m_strExecCommand;// ִ������
	CToolStatusPaneArray m_arStatusPane; // ״̬��Pane����
	CToolDockingPaneArray m_arDockingPane;	// DockingPane����
};

typedef CArray<CTool, CTool&> CToolArray;




#endif //__TOOLS_H_
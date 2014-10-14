////////////////////////////////////////////////////////////////////////////
//	File:		platdef.h
//	Version:	2.0.0.0
//	Created:	2000-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of platform.
////////////////////////////////////////////////////////////////////////////
#ifndef __PLATDEF_H_
#define __PLATDEF_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �����Զ���
#define LANGUAGE_PAGE_ENGLISH	1033
#define LANGUAGE_PAGE_CHINESE	2052

// ��������
enum {
	LANGUAGE_ENGLISH,
	LANGUAGE_CHINESE,
};

// DockingPane����
enum{
	PANE_ATTACH_WND,	// �ؼ����͵�ͣ������
	PANE_ATTACH_VIEW,	// ��ͼ���͵�ͣ������
};

// ����Ԥ���������ID
#define VCIID_PLATFORM			_T("org.platform")			// ƽ̨
#define VCIID_EDITOR			_T("org.owm.edit")			// �༭��
#define VCIID_HTMLVIEW			_T("org.owm.htmlview")		// HtmlView���
#define VCIID_PROJECTMANAGER	_T("org.vci.projectmanager")// ���̹������
#define VCIID_COMMUNICATE		_T("org.vci.communicate")	// ͨ�����
#define VCIID_IPC				_T("org.vci.ipc")			// IPC���
#define VCIID_MESSAGEQUEUE		_T("org.vci.messagequeue")	// ��Ϣ�������

////////////////////////////////////////////////////////////////////////////
// User and role define

// �û�����
class CUser : public CObject
{
public:
	CUser& operator = (const CUser& other)
	{
		bLogined	= other.bLogined;
		strUserID	= other.strUserID;
		strUserName	= other.strUserName;
		strPassword	= other.strPassword;
		nUserRight	= other.nUserRight;
		strUserDesc	= other.strUserDesc;
		strLoginDomain= other.strLoginDomain;
		return *this;
	}

public:
	BOOL	bLogined;			// �Ƿ��¼
	CString	strUserID;			// �û�ID
	CString	strUserName;		// �û���
	CString	strPassword;		// �û�����(MD5����)
	int		nUserRight;			// �û�Ȩ��
	CString	strUserDesc;		// �û�����
	CString	strLoginDomain;		// ��¼��
};

typedef CArray<CUser, CUser&> CUserArray;

// �����û�Ȩ��
enum
{
	USER_SUPER,			// �����û�
	USER_ADMIN,			// ����Ա
	USER_DEVELOP,		// ������Ա
	USER_OPERATOR,		// ������Ա
	USER_NONE,			// ��
};

// �����û���Ȩ��
class CRolePerm : public CObject
{
public:
	CRolePerm& operator = (const CRolePerm& other)
	{
		nCmdID		= other.nCmdID;
		nCmdType	= other.nCmdType;
		bSEnable	= other.bSEnable;
		bAEnable	= other.bAEnable;
		bDEnable	= other.bDEnable;
		bOEnable	= other.bOEnable;
		return *this;
	};

public:
	int		nCmdID;		// ����ID
	int		nCmdType;	// ��������
	BOOL	bSEnable;	// �����û�Ȩ��
	BOOL	bAEnable;	// ����ԱȨ��
	BOOL	bDEnable;	// ������ԱȨ��
	BOOL	bOEnable;	// ������ԱȨ��
};

typedef CArray<CRolePerm, CRolePerm&> CRolePermArray;

// �����û�Ȩ��
class CUserPerm : public CObject
{
public:
	CUserPerm& operator = (const CUserPerm& other)
	{
		nCmdID		= other.nCmdID;
		nCmdType	= other.nCmdType;
		strUserID	= other.strUserID;
		bEnable		= other.bEnable;
		return *this;
	};

public:
	int		nCmdID;		// ����ID
	int		nCmdType;	// ��������
	CString	strUserID;	// �û�ID
	BOOL	bEnable;	// �Ƿ�����
};

typedef CArray<CUserPerm, CUserPerm&> CUserPermArray;


////////////////////////////////////////////////////////////////////////////
// Log define

// ��־��������
#define OC_LOG_ADDITEM			100	// �����־��
#define OC_LOG_DELITEM			101	// ɾ����־��
#define OC_LOG_CLEAR			102	// �����־
#define OC_LOG_QUERY			103	// ��ѯ��־

// ������־��ṹ
typedef struct _LOGITEM
{
	int		nLogID;			// �¼�ID
	int		nLogType;		// �¼�����
	time_t	tTime;			// ʱ��
	CString	strFrom;		// ��Դ
	CString	strUser;		// �û�
	CString	strDesc;		// ����
} LOGITEM, FAR* LPLOGITEM;

// ������־�¼�����
enum
{
	SL_INFOMATION,			// ��Ϣ
	SL_WARNING,				// ����
	SL_ERROR,				// ����
	SL_AUDIT_SUCC,			// �ɹ����
	SL_AUDIT_FAIL,			// ʧ�����
};

//��־��Ϣ������
#define LOG_LEVEL_DEBUG		0x0001		//������Ϣ
#define LOG_LEVEL_INFO		0x0002		//һ����Ϣ
#define LOG_LEVEL_ERROR		0x0004		//������Ϣ
#define LOG_LEVEL_CRITICAL	0x0008		//������Ϣ

////////////////////////////////////////////////////////////////////////////
// Output define

// ������������нṹ
typedef struct _TTREEITEM
{
	LPVOID		hItem;		// ���ڵ���(�ڲ�ʹ��)
	int			nType;		// �ڵ�����
	int			nImage;		// ͼ��
	int			nParent;	// ���ڵ�����
	int			nCheck;		// �����
	int			nBold;		// �Ƿ����
	COLORREF	cText;		// ������ɫ
	COLORREF	cBack;		// ������ɫ
	int			nExpand;	// �Ƿ�չ��
	int			nClickFunc;	// ˫���ص���������
	CString		strLine;	// ����
public:
	_TTREEITEM& operator = (const _TTREEITEM& other)
	{
		hItem	= other.hItem;
		nType	= other.nType;
		nImage	= other.nImage;
		nParent	= other.nParent;
		nCheck	= other.nCheck;
		nBold	= other.nBold;
		cText	= other.cText;
		cBack	= other.cBack;
		nExpand	= other.nExpand;
		nClickFunc=other.nClickFunc;
		strLine	=other.strLine;
		return *this;
	};
} TTREEITEM, FAR* LPTTREEITEM;

typedef CArray<TTREEITEM, TTREEITEM&> CTTreeItemArray;


////////////////////////////////////////////////////////////////////////////
// Tree Option Dialog define

typedef struct _TOPTITEM
{
	HTREEITEM	hItem;		// ���ڵ���(�ڲ�ʹ��)
	INT			iType;		// �ڵ�����
	CString		lpszText;	// ��Ϣ
	CString		lpszEdit;	// �ɱ༭�ַ���
	CString		lpszList;	// Combo�б�
	BOOL		bCheck;		// �����
	INT			iImage;		// ͼ��
	INT			iParent;	// ���ڵ�����
	CString		strTclVar;	// TCL�б���������
public:
	_TOPTITEM& operator = (const _TOPTITEM& other)
	{
		hItem	= other.hItem;
		iType	= other.iType;
		lpszText= other.lpszText;
		lpszEdit= other.lpszEdit;
		lpszList= other.lpszList;
		bCheck	= other.bCheck;
		iImage	= other.iImage;
		iParent	= other.iParent;
		strTclVar=other.strTclVar;
		return *this;
	};
} TOPTITEM, FAR* LPTOPTITEM;

typedef CArray<TOPTITEM, TOPTITEM&> CTOptArray;

// ��������������
enum {
	TOPT_MSG,			// ���ñ������Ϣ
	TOPT_HELP,			// ���ð����ļ�
	TOPT_ITEM,			// �����
	TOPT_GROUP,			// �����
	TOPT_CHECKBOX,		// ��Ӽ���
	TOPT_RADIOBUTTON,	// ��ӹ㲥��ť
	TOPT_EDIT,			// ��ӱ༭��
	TOPT_COMBO,			// �����Ͽ�
	TOPT_FONT,			// ѡ������
	TOPT_TIME,			// ����ʱ������
	TOPT_COLOR,			// ѡ����ɫ
	TOPT_CHOOSEDIR,		// ѡ��·��
	TOPT_CHOOSEFILE,	// �ļ�����
};

// ����������ͼ��
enum {
	TOPTI_FOLDER,		// �ļ���
	TOPTI_GROUP,		// ��
	TOPTI_EDIT,			// �༭��
	TOPTI_COMBO,		// ��Ͽ�
	TOPTI_M_FOLDER,		// �ļ���(������)
	TOPTI_M_GROUP,		// ��(������)
	TOPTI_M_EDIT,		// �༭��(������)
	TOPTI_M_COMBO,		// ��Ͽ�(������)
	TOPTI_INFO,			// ��Ϣ
	TOPTI_PROJECT,		// ����
	TOPTI_FILE,			// �ļ�
	TOPTI_VERSION,		// �汾
	TOPTI_INTERFACE,	// �ӿ�
	TOPTI_ITEM,			// ��Ŀ
	TOPTI_HELP,			// ����
	TOPTI_BOARD,		// ����
	TOPTI_WORNING,		// �澯
	TOPTI_CONNECT,		// ��������
	TOPTI_USER,			// �û�
	TOPTI_FONT,			// ����
	TOPTI_TIME,			// ʱ��
	TOPTI_COLOR,		// ��ɫ
};


////////////////////////////////////////////////////////////////////////////
// ͨ�õ�¼��Ϣ�ṹ define

struct TCommLoginInfo
{
	CString	strAddress;		// ��¼��ַ
	CString	strUserName;	// �û���
	CString	strPassword;	// ����
	CString	strDomain;		// ��¼��
	CString	strDomainList;	// ��¼���б�
	CString	strTitle;		// ����
	CString	strInfo;		// ��ʾ��Ϣ
	CString	strLAddress;	// ��ַ�����ǩ
	CString	strLUserName;	// �û������ǩ
	CString	strLPassword;	// ���������ǩ
	CString	strLDomain;		// �������ǩ
	CString	strCheck;		// ��������
	BOOL	bCheck;			// ����ֵ
	BOOL	bUseAddress;	// ʹ�õ�ַ�ֶ�
	BOOL	bUseUserName;	// ʹ���û����ֶ�
	BOOL	bUsePassword;	// ʹ�ÿ����ֶ�
	BOOL	bUseDomain;		// ʹ�õ�¼���ֶ�
	CString	varAddress;		// ��¼��ַTCL����
	CString	varUserName;	// �û���TCL����
	CString	varPassword;	// ����TCL����
	CString	varDomain;		// ��¼��TCL����
	CString	varCheck;		// ����TCL����
	CString	strFocus;		// ���뽹��

public:
	TCommLoginInfo()
	{
		strAddress		= "";
		strUserName		= "";
		strPassword		= "";
		strDomain		= "";
		strDomainList	= "";
		strTitle		= "";
		strInfo			= "";
		strLAddress		= "";
		strLUserName	= "";
		strLPassword	= "";
		strLDomain		= "";
		strCheck		= "";
		bCheck			= FALSE;
		bUseAddress		= FALSE;
		bUseUserName	= FALSE;
		bUsePassword	= FALSE;
		bUseDomain		= FALSE;
		varAddress		= "";
		varUserName		= "";
		varPassword		= "";
		varDomain		= "";
		varCheck		= "";
		strFocus		= "";
	};
};


////////////////////////////////////////////////////////////////////////////
// CExtMenuInfo define

// �˵���������
enum {
	MENU_PROC_TYPE_NORMAL,		// ��ͨ���Ͳ˵�����
	MENU_PROC_TYPE_PARAM_ID,	// ��һ��������ID���Ĳ˵�����
};

// ����˵���ṹ
typedef int (TYPEOF_EXT_ClientMenu)();
typedef void (TYPEOF_EXT_UpdateClientMenu)(CCmdUI* pCmdUI);
typedef int (TYPEOF_EXT_ClientMenuParamId)(int nId);

//
// ��չ�˵���Ϣ��
//
class CExtMenuInfo : public CObject
{
public:
	CExtMenuInfo()
	{
		m_strVciId		= "";
		m_strMainMenu	= "";
		m_strSubMenu	= "";
		pProc			= NULL;
		pUpdateProc		= NULL;
		nId				= 0;
		nGrade			= 0;
		//--------��չ��Ա����----------
		nProcType		= MENU_PROC_TYPE_NORMAL;
		m_strAction		= "";
	}
	~CExtMenuInfo() {};

	CExtMenuInfo(const CExtMenuInfo& other)
	{
		m_strVciId		= other.m_strVciId;
		m_strMainMenu	= other.m_strMainMenu;
		m_strSubMenu	= other.m_strSubMenu;
		pProc			= other.pProc;
		pUpdateProc		= other.pUpdateProc;
		nId				= other.nId;
		nGrade			= other.nGrade;
		nProcType		= other.nProcType;
		m_strAction		= other.m_strAction;
	}

	CExtMenuInfo& operator = (const CExtMenuInfo& other)
	{
		m_strVciId		= other.m_strVciId;
		m_strMainMenu	= other.m_strMainMenu;
		m_strSubMenu	= other.m_strSubMenu;
		pProc			= other.pProc;
		pUpdateProc		= other.pUpdateProc;
		nId				= other.nId;
		nGrade			= other.nGrade;
		nProcType		= other.nProcType;
		m_strAction		= other.m_strAction;
		return *this;
	}

public:
	CString	m_strVciId;		// �����OWM����չ�˵�,���ʾOWM��ID
	CString	m_strMainMenu;	// ���˵���
	CString	m_strSubMenu;	// �Ӳ˵���
	TYPEOF_EXT_ClientMenu *pProc;	// �˵�����
	TYPEOF_EXT_UpdateClientMenu	*pUpdateProc;	// ˢ�²˵�����
	int nId;					// �˵�ID
	int nGrade;					// Ȩ��
	int nProcType;				// �˵���������(0��ʾ��ͨ����,1��ʾ��һ�������Ĳ˵�����)
	CString	m_strAction;	// �˵�ִ�еĶ���
};

typedef CArray<CExtMenuInfo, CExtMenuInfo&> CExtMenuInfoArray;


////////////////////////////////////////////////////////////////////////////
// TTaskDialogInfo define

class IInterp;
// ͨ������Ի�����Ϣ�ṹ
struct TTaskDialogInfo
{
	IInterp*	pInterp;		// ������ָ��
	CString	strWindowTitle;		// ���ڱ���
	CString	strVerificationText;// ��֤����
	CString	strExpandedControlText;	// ��չ�ؼ�����
	CString	strCollapsedControlText;// �����ؼ�����
	CString	strMainInstruction;	// ������
	CString	strContent;			// ����
	CString	strExpandedInformation;// չ����Ϣ
	CString	strFooter;			// ��ע
	CString	strMainIcon;		// ��ͼ��
	CString	strFooterIcon;		// ��עͼ��
	int		nWidth;				// ���ڿ��
	BOOL	bEnableInputBox;	// ���������
	CString	strInputBoxText;	// ���������
	CStringArray	asReplace;	// �滻�ַ�������
	CStringArray	asButtonText;// ��ť����
	CUIntArray		auButtonID;	// ��ťID����
	CStringArray	asButtonScript;// ��ť�ű�����
	CStringArray	asRadioButtonText;// Radio��ť����
	CUIntArray		auRadioButtonID;// Radio��ťID����
	BOOL	bEnableCommandLinks;// ������������
	BOOL	bEnableHyperlinks;	// �������ֳ�����
	BOOL	bEnableRelativePosition;// ������ݸ�����ȷ������λ��
	BOOL	bShowProgressBar;	// ��ʾ������
	int		nProgressBarPos;	// ������λ��
	int		nProgressBarRangeMin;// ��������Сֵ
	int		nProgressBarRangeMax;// ���������ֵ
	BOOL	bMarquee;			// ������ʹ�û���ģʽ
	BOOL	bExpandedByDefault;	// Ĭ��������Ƿ���ʾ����������
	BOOL	bExpandFooterArea;	// ���������Ƿ���ʾ�ڽ�ע��
	int		nDefaultButton;		// Ĭ�ϰ�ťID
	int		nDefaultRadioButton;// Ĭ��Radio��ťID
	BOOL	bNoDefaultRadioButton;// û��Ĭ�ϵ�radio��ť
	BOOL	bVerifyCheckState;	// check���״̬
	BOOL	bEnableCallbackTimer;// ����ص�ʱ��
	int		nTimerTimeout;		// ��ʱ����ʱʱ��
	CString	strTimerScript;		// ��ʱ����ʱ�ű�
	BOOL	bCommonButtonOk;	// ��ʾ������ťOk
	BOOL	bCommonButtonYes;	// ��ʾ������ťYes
	BOOL	bCommonButtonNo;	// ��ʾ������ťNo
	BOOL	bCommonButtonCancel;// ��ʾ������ťCancel
	BOOL	bCommonButtonRetry;	// ��ʾ������ťRetry
	BOOL	bCommonButtonClose;	// ��ʾ������ťClose
	int		nSelectedButtonId;	// ����ѡ��İ�ťID
	int		nSelectedRadioButtonId;// ����ѡ���Radio��ťID
	BOOL	bIsVerificiationChecked;// ����check���״̬

public:
	TTaskDialogInfo()
	{
		pInterp						= NULL;
		strWindowTitle				= "";
		strVerificationText			= "";
		strExpandedControlText		= "";
		strCollapsedControlText		= "";
		strMainInstruction			= "";
		strContent					= "";
		strExpandedInformation		= "";
		strFooter					= "";
		strFooterIcon				= "";
		nWidth						= 0;
		bEnableInputBox				= FALSE;
		strInputBoxText				= "";
		bEnableCommandLinks			= TRUE;
		bEnableHyperlinks			= TRUE;
		bEnableRelativePosition		= FALSE;
		bShowProgressBar			= FALSE;
		nProgressBarPos				= 0;
		nProgressBarRangeMin		= 0;
		nProgressBarRangeMax		= 100;
		bMarquee					= FALSE;
		bExpandedByDefault			= FALSE;
		bExpandFooterArea			= FALSE;
		nDefaultButton				= 0;
		nDefaultRadioButton			= 0;
		bNoDefaultRadioButton		= FALSE;
		bVerifyCheckState			= FALSE;
		bEnableCallbackTimer		= FALSE;
		nTimerTimeout				= 0;
		strTimerScript				= "";
		bCommonButtonOk				= TRUE;
		bCommonButtonYes			= FALSE;
		bCommonButtonNo				= FALSE;
		bCommonButtonCancel			= TRUE;
		bCommonButtonRetry			= FALSE;
		bCommonButtonClose			= FALSE;
		nSelectedButtonId			= 0;
		nSelectedRadioButtonId		= 0;
		bIsVerificiationChecked		= FALSE;
	};

	TTaskDialogInfo& operator = (const TTaskDialogInfo& other)
	{
		pInterp						= other.pInterp;
		strWindowTitle				= other.strWindowTitle;
		strVerificationText			= other.strVerificationText;
		strExpandedControlText		= other.strExpandedControlText;
		strCollapsedControlText		= other.strCollapsedControlText;
		strMainInstruction			= other.strMainInstruction;
		strContent					= other.strContent;
		strExpandedInformation		= other.strExpandedInformation;
		strFooter					= other.strFooter;
		strFooterIcon				= other.strFooterIcon;
		nWidth						= other.nWidth;
		bEnableInputBox				= other.bEnableInputBox;
		strInputBoxText				= other.strInputBoxText;
		bEnableCommandLinks			= other.bEnableCommandLinks;
		bEnableHyperlinks			= other.bEnableHyperlinks;
		bEnableRelativePosition		= other.bEnableRelativePosition;
		bShowProgressBar			= other.bShowProgressBar;
		nProgressBarPos				= other.nProgressBarPos;
		nProgressBarRangeMin		= other.nProgressBarRangeMin;
		nProgressBarRangeMax		= other.nProgressBarRangeMax;
		bMarquee					= other.bMarquee;
		bExpandedByDefault			= other.bExpandedByDefault;
		bExpandFooterArea			= other.bExpandFooterArea;
		nDefaultButton				= other.nDefaultButton;
		nDefaultRadioButton			= other.nDefaultRadioButton;
		bNoDefaultRadioButton		= other.bNoDefaultRadioButton;
		bVerifyCheckState			= other.bVerifyCheckState;
		bEnableCallbackTimer		= other.bEnableCallbackTimer;
		nTimerTimeout				= other.nTimerTimeout;
		strTimerScript				= other.strTimerScript;
		bCommonButtonOk				= other.bCommonButtonOk;
		bCommonButtonYes			= other.bCommonButtonYes;
		bCommonButtonNo				= other.bCommonButtonNo;
		bCommonButtonCancel			= other.bCommonButtonCancel;
		bCommonButtonRetry			= other.bCommonButtonRetry;
		bCommonButtonClose			= other.bCommonButtonClose;
		nSelectedButtonId			= other.nSelectedButtonId;
		nSelectedRadioButtonId		= other.nSelectedRadioButtonId;
		bIsVerificiationChecked		= other.bIsVerificiationChecked;

		int i;
		asReplace.RemoveAll();
		for(i=0; i<other.asReplace.GetSize(); i++)
		{
			asReplace.Add(other.asReplace[i]);
		}
		asButtonText.RemoveAll();
		for(i=0; i<other.asButtonText.GetSize(); i++)
		{
			asButtonText.Add(other.asButtonText[i]);
		}
		auButtonID.RemoveAll();
		for(i=0; i<other.auButtonID.GetSize(); i++)
		{
			auButtonID.Add(other.auButtonID[i]);
		}
		asButtonScript.RemoveAll();
		for(i=0; i<other.asButtonScript.GetSize(); i++)
		{
			asButtonScript.Add(other.asButtonScript[i]);
		}
		asRadioButtonText.RemoveAll();
		for(i=0; i<other.asRadioButtonText.GetSize(); i++)
		{
			asRadioButtonText.Add(other.asRadioButtonText[i]);
		}
		auRadioButtonID.RemoveAll();
		for(i=0; i<other.auRadioButtonID.GetSize(); i++)
		{
			auRadioButtonID.Add(other.auRadioButtonID[i]);
		}
		return *this;
	}
};


////////////////////////////////////////////////////////////////////////////
// PopupControl define

// ����PopupControl���ݵ�����
enum {
	POPUP_ITEM_TEXT,
	POPUP_ITEM_RTF,
	POPUP_ITEM_MARKUP,
};

// Popup control struct
struct TPopupControlInfo
{
	LPVOID	pInterp;		// ������ָ��
	CString	strTheme;		// ���
	CString	strIcon;		// ͼ��
	CString	strCaption;		// ���ڱ���
	CString	strTitle;		// �������ı�
	CString	strText;		// ������
	CString	strLinkTitle;	// ����������
	CString	strLinkText;	// ��������
	CString	strBackgroundImage;	// ����ͼƬ
	int		nItemType;		// �ı�����(Text,RTF,Markup)
	int		nShowDelay;		// ��ʾʱ��
	int		nTransparency;	// ͸����
	int		nWidth;			// ���ڿ��
	int		nHight;			// ���ڸ߶�
	int		nAddHight;		// ���ӵĸ߶�
	int		nAnimation;		// ��������
	CStringArray	asButtonText;// ��ť����
	CStringArray	asButtonScript;// ��ť�ű�����
	CStringArray	asRadioButtonText;// Radio��ť����

public:
	TPopupControlInfo()
	{
		pInterp			= NULL;
		strTheme		= "";
		strIcon			= "";
		strCaption		= "";
		strTitle		= "";
		strText			= "";
		strLinkTitle	= "";
		strLinkText		= "";
		strBackgroundImage = "";
		nItemType		= POPUP_ITEM_TEXT;
		nShowDelay		= 0;
		nTransparency	= 0;
		nWidth			= 0;
		nHight			= 0;
		nAddHight		= 0;
		nAnimation		= 2;
	};

	TPopupControlInfo& operator = (const TPopupControlInfo& other)
	{
		pInterp			= other.pInterp;
		strTheme		= other.strTheme;
		strIcon			= other.strIcon;
		strCaption		= other.strCaption;
		strTitle		= other.strTitle;
		strText			= other.strText;
		strLinkTitle	= other.strLinkTitle;
		strLinkText		= other.strLinkText;
		strBackgroundImage = other.strBackgroundImage;
		nItemType		= other.nItemType;
		nShowDelay		= other.nShowDelay;
		nTransparency	= other.nTransparency;
		nWidth			= other.nWidth;
		nHight			= other.nHight;
		nAddHight		= other.nAddHight;
		nAnimation		= other.nAnimation;
		asButtonText.RemoveAll();
		int i;
		for(i=0; i<other.asButtonText.GetSize(); i++)
		{
			asButtonText.Add(other.asButtonText[i]);
		}
		asButtonScript.RemoveAll();
		for(i=0; i<other.asButtonScript.GetSize(); i++)
		{
			asButtonScript.Add(other.asButtonScript[i]);
		}
		return *this;
	};
};


#endif // !defined(__PLATDEF_H_)
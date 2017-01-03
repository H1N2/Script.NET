////////////////////////////////////////////////////////////////////////////
//	File:		owm.h
//	Version:	2.1.0.1
//	Created:	2000-06-01
//	Last Update:2006-05-25
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of objwin module.
////////////////////////////////////////////////////////////////////////////
#ifndef __OWM_H_
#define __OWM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ��OWMģ����չ��Դ����ʼֵ
#define OWM_SID_FTLIST		11000
#define OWM_SID_LOG			11050
#define OWM_SID_REPORT		11100
#define OWM_SID_OWMEDIT		11150
#define OWM_SID_WORKTREE	11250
#define OWM_SID_RCADMIN		11300
#define OWM_SID_PTERMINAL	11350
#define OWM_SID_UMLEDIT		11400
#define OWM_SID_IMAGE		11500	// ͼ��༭��ģ��

// ����OWM����
#define OWM_DOC			0x00000001	// ���ĵ�ģ���OWM
#define OWM_TCL			0x00000002	// ��TCL��չ��OWM
#define OWM_MENU		0x00000004	// ����չ�˵���OWM
#define OWM_TOOLBAR		0x00000008	// ע�Ṥ����
#define OWM_ONEFRAME	0x00000010	// ֻ����һ��OWM���ڵ�ʵ��
#define OWM_TABVIEW		0x00000020	// �Ƿ�ΪTabView���͵Ĵ���
#define OWM_OPTION		0x00000040	// �Ƿ�������ҳ
#define OWM_CONTROLBAR	0x00000080	// �Ƿ����ControlBar����
#define OWM_TABCTRLBAR	0x00000100	// �Ƿ����TabCtrlBar����
#define OWM_AUTOCREATE	0x00000200	// ����OWMʱ�Ƿ��Զ���������
#define OWM_DYNLOAD		0x00000400	// ��̬������չģ��
#define OWM_BROADCAST	0x00010000	// �Ƿ�������չ㲥����
#define OWM_MAXWINDOW	0x00020000	// ������󻯴���

// ����OWM��չ����
#define OC_CMDALL				0	// ��ѯ��������
#define OC_QUERYOWMID			1	// ��ѯOWM(����ID)
#define OC_SETOWMLINK			2	// ����OWM����(to owm)
#define OC_RUNSCRIPT			3	// ���нű�(to plat)
#define OC_OPENVIEW				4	// ��OWM����(to owm)
#define OC_CLOSEVIEW			5	// �ر�OWM����(to owm)
#define OC_OPENBAR				6	// ��ControlBar(to owm)
#define OC_CLOSEBAR				7	// �ر�ControlBar(to owm)
#define OC_CMDHELP				8	// �������
#define OC_GETTCLEXKEYWORD		9	// ��ȡTCL��չ�����б�ָ��

// �����ļ���������
#define OC_GETINISTRING			10	// ȡINI�ַ�
#define OC_GETINIINT			11	// ȡINI int
#define OC_SETINISTRING			12	// ����INI�ַ�
#define OC_SETINIINT			13	// ����INI int
#define OC_QUERYUSERINFO		14	// ��ѯ�û���Ϣ

// ������ڲ�������
#define OC_TRACEFMT				20	// ��TRACE�������
#define OC_CREATEOUTPUT			21	// �����������
#define OC_CLOSEOUTPUT			22	// �ر��������
#define OC_OUTPUTSET			23	// �����������
#define OC_OUTPUTGET			24	// ��ȡ���������Ϣ
#define OC_OUTPUT				25	// ������������
#define OC_ACTIVEOUTPUT			26	// �����
#define OC_CLEAROUTPUT			27	// �����������
#define OC_TREEOUTPUT			28	// ���������
#define OC_OUTPUTVERIFY			29	// �����Ϣ����У���ַ����Ƚ�

// ״̬����������
#define OC_ADDPANE				30	// ���Pane
#define OC_REMOVEPANE			31	// ɾ��Pane
#define OC_SETPANETEXT			32	// ����Pane����
#define OC_SETPANEMENU			33	// ����Pane�˵�
#define OC_SETPANECONTROL		34	// ����Pane�ؼ�
#define OC_SETPANEIMAGE			35	// ����Paneͼ����
#define OC_SETPANEIMAGEITEM		36	// ����Paneͼ����
#define OC_SETPANEWIDTH			37	// ����Pane���
#define OC_ATTACHPANEPROGRESS	38	// ����Pane�ͽ������Ĺ���
#define OC_SHOWPANEPROGRESS		39	// ��ʾ/����Pane������
#define OC_SETPANEPROGRESSPOS	40	// ����Pane����������
#define OC_SETPANEBUTTON		41	// ����Pane��ť
#define OC_SETPANETOOLTIP		42	// ����Pane��ʾ��Ϣ

// �˵�����
#define OC_ADDEXTMENU			45	// �����չ�˵�
#define OC_DELEXTMENU			46	// ɾ����չ�˵�
#define OC_QUERYEXTMENU			47	// ��ѯ��չ�˵�
#define OC_QUERYEXTMENUFROMID	48	// ͨ���˵�ID��ѯ��չ�˵���Ϣ

// ���Բ���
#define OC_TESTINIT				50	// ��ʼ��
#define OC_TESTSTART			51	// ��������
#define OC_TESTSTOP				52	// ֹͣ����
#define OC_TESTHOLD				53	// ��ͣ����
#define OC_TESTRESUME			54	// �ָ�����
#define OC_TESTDISABLE			55	// ��ֹ����
#define OC_TESTRESET			56	// ��λ������
#define OC_TESTPROPERTY			57	// ����
#define OC_TESTSELFTEST			58	// �豸�Լ�
#define OC_TESTSTRATEGY			59	// ���Բ��Ե���

// ��־����
#define OC_LOGADDITEM			60	// �����־��
#define OC_LOGDELITEM			61	// ɾ����־��
#define OC_LOGCLEAR				62	// �����־

// ϵͳ��չ����
#define OC_SETACTIVEWS			70	// ���û��λ
#define OC_GOTOITEM				71	// ת��ָ������Ŀ
#define OC_GETFONTARRAY			72	// ��ȡ�������ñ�
#define OC_DYNCLOSE				73	// ֪ͨƽִ̨�ж�̬�ر�
#define OC_OPENPROJECT			74	// �򿪹���
#define OC_CLOSEPROJECT			75	// �رչ���
#define OC_VIEW_HISTORY			76	// ��ʾ��ʷ��¼
#define OC_CHECK_IDENABLE		77	// �ж�ĳ��ID�Ƿ���в���Ȩ��
#define OC_SETACTIVEGROUP		78	// ���û��
#define OC_EXECOPTION			79	// ִ������ҳ����
#define OC_EXECTREEOPTION		80	// ִ��ͨ����״���ԶԻ���
#define OC_EXECTCLCMD			81	// ִ��TCL����
#define OC_EXECTCLFILE			82	// ִ��TCL�ű��ļ�
#define OC_REGEXP				83	// ʹ��TCL������ʽ���ܽ��з���
#define	OC_VIEWHTMLPAGE			84	// ��ʾHtmlҳ��
#define OC_WEBDIALOG			85	// ִ��WEB�Ի���
#define OC_OPENDOCUMENTFILE		86	// ��һ���ļ����б༭
#define OC_SETDOCICON			87	// �����ĵ�ͼ��
#define OC_SETDOCTITLE			88	// �����ĵ�����
#define OC_MODIFYDOCTITLE		89	// �޸��ĵ�����
#define	OC_GETHTMLURL			90	// ��ȡHtmlҳ���URL
#define	OC_REFRESH_DOCKINGHTML	91	// ˢ��Docking Htmlҳ��
#define	OC_SETCOMMANDICON		92	// ����ĳ������ID��ͼ��
#define	OC_SETCOMMANDICONS		93	// ���ö������ID��ͼ��
#define	OC_SETOWMICONS			94	// ����ĳ��OWM��ͼ��
#define	OC_REFRESHDEBUGWIN		95	// ˢ�µ��Դ���
#define	OC_CHECK_ACTIVEOWM		96	// �ж�ָ��OWM�����Ƿ�Ϊ��ǰ�����
#define	OC_SHOWABOUTBOX			97	// ��ʾָ��ģ���About�Ի���
#define	OC_SENDVCICOMMAND		98	// ����VCI������(ProcessCommand����)

// ����������
#define OC_NAVTREE_HIDE			120	// ���ؽڵ�
#define OC_NAVTREE_REFRESH		121	// ˢ�½ڵ�
#define OC_NAVTREE_PROPERTY		122	// �ڵ�����

// XML����
#define OC_XML_OPENFILE			130	// ��XML�ļ�
#define OC_XML_CLOSEFILE		131	// �ر�XML�ļ�

// ���Դ��ڲ���
#define OC_PROPERTY_CLEAR		140	// ������Դ���


////////////////////////////////////////////////////////////////
// �������ڲ�����,������
// ��������
#define OC_TEST_INTERNALINIT	1010	// �ڲ��ĳ�ʼ������
#define OC_TEST_INTERNALSTART	1011	// �ڲ���������������

// ��������
#define OC_CONFIG_SWITCHBAR		1020	// �л�������Ϣע����
#define OC_REFRESH_RSMINTERP	1021	// ˢ����Դ�������Ľ������ڵ�

// ��չ����
#define OC_LOGIN_DIALOG			1030	// ͨ�õ�¼�Ի���
#define OC_PATHSELECT_DIALOG	1031	// ·��ѡ��Ի���

// ���ȶԻ�������
#define OC_PROGDLG_CREATE		1040	// ����ͨ�ý��ȶԻ���
#define OC_PROGDLG_SETPOS		1041	// ���ý���
#define OC_PROGDLG_SETSTATUS	1042	// ����״̬��Ϣ
#define OC_PROGDLG_END			1043	// �������ȶԻ���
#define OC_PROGDLG_CANCEL		1044	// cancel���ȶԻ���
#define OC_PROGDLG_CANCELSCRIPT	1045	// ����cancel�ű�

// TaskDialog����
#define OC_TASK_DIALOG			1050	// ͨ������Ի���

// Popup Control����
#define OC_POPUP_CONTROL		1060	// ��ʾPopup����

// HtmlView��������
#define OC_HTML_SET_ELEMENT_INNERHTML	1070	// ����HtmlԪ�ص�Html����
#define OC_HTML_GET_ELEMENT_INNERHTML	1071	// ��ȡHtmlԪ�ص�Html����
#define OC_HTML_SET_ELEMENT_INNERTEXT   1072	// ����HtmlԪ�ص�Text����
#define OC_HTML_GET_ELEMENT_INNERTEXT   1073	// ��ȡHtmlԪ�ص�Text����
#define OC_HTML_SET_ELEMENT_OUTERHTML	1074	// ����HtmlԪ�ص��ⲿHtml����
#define OC_HTML_GET_ELEMENT_OUTERHTML	1075	// ��ȡHtmlԪ�ص��ⲿHtml����
#define OC_HTML_SET_ELEMENT_OUTERTEXT	1076	// ����HtmlԪ�ص��ⲿText����
#define OC_HTML_GET_ELEMENT_OUTERTEXT	1077	// ��ȡHtmlԪ�ص��ⲿText����
#define OC_HTML_SET_ELEMENT_INPUTVALUE	1078	// ����HtmlԪ�ص�Input Value
#define OC_HTML_GET_ELEMENT_INPUTVALUE	1079	// ��ȡHtmlԪ�ص�Input Value
#define OC_HTML_CLEAR_MESSAGE_FILTER	1090	// ���ҳ����Ϣ�����б�
#define OC_HTML_ADD_MESSAGE_FILTER		1091	// ���ҳ�������Ϣ
#define OC_HTML_PROCESS_MESSAGE			1095	// ���洦��VciMessage

// MarkupDialog����
#define OC_MARKUP_DIALOG		2000	// ��ʾMarkup�Ի���

// Tray�������
#define OC_TRAY_ADD_ICON		2010	// ���Tray Icon
#define OC_TRAY_DEL_ICON		2011	// ɾ��Tray Icon
#define OC_TRAY_SET_ICON		2012	// ����Tray Icon
#define OC_TRAY_START_ANIMATION	2013	// ����Tary Icon����
#define OC_TRAY_STOP_ANIMATION	2014	// ֹͣTray Icon����
#define OC_TRAY_SET_TOOLTIP		2015	// ����Tray Icon��Tooltip
#define OC_TRAY_BALLOONTIP		2016	// ��ʾTary Icon������ʾ

// �����������
#define OC_TASK_GETSCRIPT		1501	// ��ȡĳ������Ľű�


#pragma pack(push,1)

// ����OWM�˵���ṹ
typedef int (TYPEOF_ClientMenu)();
typedef void (TYPEOF_UpdateClientMenu)(CCmdUI* pCmdUI);
typedef int (TYPEOF_ClientMenuParamId)(int nId);

typedef struct 
{
	TYPEOF_ClientMenu *pProc;	// �˵�����
	TYPEOF_UpdateClientMenu	*pUpdateProc;	// ˢ�²˵�����
	int nId;					// �˵�ID
	int nGrade;					// Ȩ��
} OWMMENUITEM;

typedef CArray<OWMMENUITEM, OWMMENUITEM> COWMMenuArray;

// ��������ҳ
typedef struct
{
	CString strParent;		// ���ڵ���
	CString strNode;		// �ڵ���
	CString strDesc;		// ����
	CWnd* pOptionPage;		// ����ҳָ��
	int nGrade;				// Ȩ��
} OWMOPTION;

typedef CArray<OWMOPTION, OWMOPTION> COWMOptionArray;

// �����������ýṹ
#define FON_FONT		0x00000001	// ����
#define FON_SIZE		0x00000002	// ��С
#define FON_FG			0x00000004	// ǰ��ɫ
#define FON_BK			0x00000008	// ����ɫ
#define FON_TYPE		0x00000010	// ����

struct FontItem
{
	DWORD	dwMask;			// ����
	CString strName;		// ��������
	CString strSubKey;		// �Ӽ���
	CString strFont;		// ����
	int		nSize;			// ��С
	COLORREF colorFg;		// ǰ��ɫ
	COLORREF colorBk;		// ����ɫ
	BOOL	bBold;			// ����
	BOOL	bItalic;		// б��
	BOOL	bUnderLine;		// �»���
	BOOL	bStrikeOut;		// ɾ����

	FontItem() {;};
	FontItem(DWORD _dwMask, CString _strName, CString _strSubKey, CString _strFont,
			int _nSize, COLORREF _colorFg, COLORREF _colorBk, BOOL _bBold,
			BOOL _bItalic, BOOL _bUnderLine, BOOL _bStrikeOut)
	{
		dwMask		= _dwMask;
		strName		= _strName;
		strSubKey	= _strSubKey;
		strFont		= _strFont;
		nSize		= _nSize;
		colorFg		= _colorFg;
		colorBk		= _colorBk;
		bBold		= _bBold;
		bItalic		= _bItalic;
		bUnderLine	= _bUnderLine;
		bStrikeOut	= _bStrikeOut;
	};

	FontItem& operator = (const FontItem& other)
	{
		dwMask		= other.dwMask;
		strName		= other.strName;
		strSubKey	= other.strSubKey;
		strFont		= other.strFont;
		nSize		= other.nSize;
		colorFg		= other.colorFg;
		colorBk		= other.colorBk;
		bBold		= other.bBold;
		bItalic		= other.bItalic;
		bUnderLine	= other.bUnderLine;
		bStrikeOut	= other.bStrikeOut;
		return *this;
	}
};

// ����OWM�ӿں�������
typedef int (TYPEOF_InitOWM)();
typedef int (TYPEOF_DoneOWM)();
typedef int (TYPEOF_SetOWM)(LPCTSTR lpszOption);
typedef int (TYPEOF_RegisterTclCmd)(void* interp);
typedef int (TYPEOF_UnregisterTclCmd)();
typedef int (TYPEOF_RegisterTclExKeyword)(CStringArray& aKeyList);
typedef int (TYPEOF_RunTclScript)(LPTSTR lpszPathName, int& nLine);
typedef int (TYPEOF_RegisterClientMenu)(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc);
typedef int (TYPEOF_UnregisterClientMenu)(COWMMenuArray& aOWMMenus);
typedef int (TYPEOF_RegisterClientToolBar)(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex);
typedef int (TYPEOF_UnregisterClientToolBar)(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart);
typedef int (TYPEOF_RegisterOption)(COWMOptionArray& aOWMOption);
typedef int (TYPEOF_SetDebugMode)(BOOL bDebug);
typedef int (TYPEOF_RunHelp)();
typedef int (TYPEOF_CommandOWM)(int nCmd, WPARAM wParam, LPARAM lParam);
typedef int (TYPEOF_CommandPlat)(int nCmd, WPARAM wParam, LPARAM lParam);
typedef int (TYPEOF_TraceFmt)(int nFmt, LPCTSTR format, ...);
typedef int (TYPEOF_CreateOutput)(LPCTSTR lpszName);
typedef int (TYPEOF_CloseOutput)(LPCTSTR lpszName);
typedef int (TYPEOF_OutputSet)(int nWinID, LPCTSTR lpszOption);
typedef int (TYPEOF_Output)(int nWinID, LPCTSTR format, ...);
typedef void (TYPEOF_GetDllResource)();
typedef void (TYPEOF_ResetDllResource)();


// ����OWM�ӿڽṹ
typedef struct 
{
	UINT							nOWMType;				// OWM����
	UINT							nOWMID;					// OWM��ʶ
	CRuntimeClass					*pDocClass;				// OWM�ĵ�
	CRuntimeClass					*pFrameClass;			// OWM���
	CRuntimeClass					*pViewClass;			// OWM��
	CRuntimeClass					*pCtrlBarClass;			// ��Ϊ��������Ӧ��
	CMultiDocTemplate				*pDocTemplate;			// OWM�ĵ�ģ��
	TYPEOF_InitOWM					*InitOWM;				// ��ʼ��OWM
	TYPEOF_DoneOWM					*DoneOWM;				// ж��OWM
	TYPEOF_SetOWM					*SetOWM;				// ����OWM;
	TYPEOF_RegisterTclCmd			*RegisterTclCmd;		// TCL����ע�ắ��
	TYPEOF_UnregisterTclCmd			*UnregisterTclCmd;		// TCL����ж�غ���
	TYPEOF_RegisterTclExKeyword		*RegisterTclExKeyword;	// TCL��չ�ؼ���ע�ắ��
	TYPEOF_RunTclScript				*RunTclScript;			// ����TCL�ű�,��ƽ̨�ṩ
	TYPEOF_RegisterClientMenu		*RegisterClientMenu;	// ע��˵�
	TYPEOF_UnregisterClientMenu		*UnregisterClientMenu;	// ж�ز˵�
	TYPEOF_RegisterClientToolBar	*RegisterClientToolBar;	// ע��ToolBar
	TYPEOF_UnregisterClientToolBar	*UnregisterClientToolBar;// ж��ToolBar
	TYPEOF_RegisterOption			*RegisterOption;		// ע��OWM����ҳ
	TYPEOF_SetDebugMode				*SetDebugMode;			// ���õ���ģʽ
	TYPEOF_RunHelp					*RunHelp;				// ����OWM�İ���
	TYPEOF_CommandOWM				*CommandOWM;			// OWM�������
	TYPEOF_CommandPlat				*CommandPlat;			// ƽ̨�������
	TYPEOF_TraceFmt					*TraceFmt;				// TRACE����,��ƽ̨�ṩ
	TYPEOF_CreateOutput				*CreateOutput;			// �����������,��ƽ̨�ṩ
	TYPEOF_CloseOutput				*CloseOutput;			// �ر��������,��ƽ̨�ṩ
	TYPEOF_OutputSet				*OutputSet;				// �����������,��ƽ̨�ṩ
	TYPEOF_Output					*Output;				// ��������������Ϣ,��ƽ̨�ṩ
	TYPEOF_GetDllResource			*GetDllResource;		// �л���DLL����Դ
	TYPEOF_ResetDllResource			*ResetDllResource;		// �л�����ǰ����Դ
} OWM_LINKAGE;

// ����OWM�汾�ṹ
typedef struct 
{
	char *svOWMName;		// OWM��
	char *svDescription;	// OWM����
	WORD wVersionLo;		// OWM�汾��λ
	WORD wVersionHi;		// OWM�汾��λ
	WORD wMVersionLo;		// ����ƽ̨�汾��λ
	WORD wMVersionHi;		// ����ƽ̨�汾��λ
	int  nLanguage;			// ����(0-����,1-Ӣ��)
	BOOL bDebugVer;			// �Ƿ�DEBUG�汾
} OWM_VERSION;

// ����OWMģ��ӿں���
typedef BOOL (TYPEOF_InstallOWM)(OWM_LINKAGE& ol);
typedef BOOL (TYPEOF_OWMVersion)(OWM_VERSION& pov);
typedef void (TYPEOF_TerminateOWM)(void);

// OWM��̬���ص�״̬
#define OWMDYN_TSR		0	// ����״̬
#define OWMDYN_UNTSR	1	// ж��״̬

#define _MAX_OWM_NAME	50	// OWM������󳤶�

// OWMģ����Ϣ�ṹ
typedef struct
{
	OWM_VERSION		ov;			// �汾��Ϣ
	OWM_LINKAGE		ol;			// �ӿ���Ϣ
	HINSTANCE		ins;		// ģ����
	CControlBar*	pBar;		// ��¼ControlBarָ��
	char			szName[_MAX_OWM_NAME];	// OWM����
	char			szPath[_MAX_PATH];	// DLL·����Ϣ
	int				nLoader;	// ��˭����(0=ƽ̨ע��,1=ƽ̨OWMĿ¼,2=����)
	int				nDnyLoad;	// ��̬����״̬
	BOOL			bEnableDynLoad;	// �Ƿ�����̬����
	BOOL			bEnable;	// ��ģ�鵱ǰ�Ƿ�ʹ��
} OWM_INFO;

typedef CArray<OWM_INFO, OWM_INFO> COWMArray;
/*
// �����û�Ȩ��
enum
{
	USER_SUPER,			// �����û�
	USER_ADMIN,			// ����Ա
	USER_DEVELOP,		// ������Ա
	USER_OPERATOR,		// ������Ա
	USER_NONE,			// ��
};
*/
#pragma pack(pop)

// OWM�ӿڶ��塢ʵ�ָ�����
#define OWM_INTERFACE_DEFINE \
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData); \
	virtual int __stdcall GetOwmName(CString& strOwmName); \
	virtual int __stdcall GetOwmDesc(CString& strOwmDesc); \
	virtual int __stdcall GetOwmVersion(int& nVersion); \
	virtual int __stdcall GetOwmType(UINT& nType); \
	virtual int __stdcall GetOwmID(UINT& nID); \
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI); \
	virtual int __stdcall SetDocTemplate(CMultiDocTemplate* pDocTemplate); \
	virtual int __stdcall GetDocTemplate(CMultiDocTemplate** ppDocTemplate); \
	virtual int __stdcall GetRuntimeClass(int nType, CRuntimeClass** ppRuntimeClass); \
	virtual CWnd* __stdcall OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam); \
	virtual int __stdcall Command(int nCmd, WPARAM wParam, LPARAM lParam); \
	virtual int __stdcall RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc); \
	virtual int __stdcall UnregisterClientMenu(COWMMenuArray& aOWMMenus); \
	virtual int __stdcall RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex); \
	virtual int __stdcall UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart);	\
	virtual int __stdcall RegisterOption(COWMOptionArray& aOWMOption);	\
	virtual int __stdcall RegisterPrintPageOption(COWMOptionArray& aOWMOption);	\
	virtual int __stdcall LoadDocumentSession(CDocument* pDoc, CStringArray& asSessionLine);	\
	virtual int __stdcall SaveDocumentSession(CDocument* pDoc, CStringArray& asSessionLine);	\
	virtual int __stdcall BackupDocument(CDocument* pDoc, LPCTSTR lpszBackupFile, int nBackupType);	\
	virtual int __stdcall GetImportTypeList(CStringArray& asImportType);	\
	virtual int __stdcall GetExportTypeList(CStringArray& asExportType);	\
	virtual int __stdcall ImportDocument(CDocument* pDoc, LPCTSTR lpszImportSource, int nImportType);	\
	virtual int __stdcall ExportDocument(CDocument* pDoc, LPCTSTR lpszExportDest, int nExportType);	\
	virtual int __stdcall Zoom(float fZoomFactor, CDocument* pDoc);	\
	virtual int __stdcall GetZoomFactor(float& fZoomFactor, CDocument* pDoc);	\
	virtual int __stdcall GetZoomFactorRange(CStringArray& asZoomFactor, float& fZoomMin, float& fZoomMax);


// ע����չ�˵�,�����ֱ���:�˵���ID,�˵�����,���и��º���,Ȩ��
#define OWM_REGISTERMENU(nItem, pproc, pupdateproc, grade) \
	{	\
		TYPEOF_ClientMenu* pProc = pproc;	\
		TYPEOF_UpdateClientMenu* pUpdateProc = pupdateproc;	\
		int nId = 0;	\
		BOOL bFind = FALSE;	\
		for(int i=0; i<aOWMMenus.GetSize(); i++)	\
		{	\
			if(aOWMMenus[i].pProc == pProc)	\
			{	\
				nId = aOWMMenus[i].nId;	\
				bFind = TRUE;	\
			}	\
			if((!bFind) && (aOWMMenus[i].nId >= nId))	\
				nId = aOWMMenus[i].nId + 1;	\
		}	\
		if((!bFind) && (pProc != NULL))	\
		{	\
			OWMMENUITEM omi;	\
			omi.nId = nId;	\
			omi.pProc = pProc;	\
			omi.pUpdateProc = pUpdateProc;	\
			omi.nGrade = grade;	\
			aOWMMenus.Add(omi);	\
		}	\
		if(nItem != 0)	\
		{	\
			CString strMenu;	\
			psubmenu->GetMenuString(nItem, strMenu, MF_BYCOMMAND);	\
			psubmenu->ModifyMenu(nItem, MF_BYCOMMAND, nOWMIdStart+nId, strMenu);	\
		}	\
		if(pProc == NULL)	\
		{	\
			psubmenu->EnableMenuItem(nOWMIdStart+nId, MF_GRAYED);	\
		}	\
	}	\


// ж����չ�˵�,�����ǲ˵�����
#define OWM_UNREGISTERMENU(pproc) \
	{	\
		TYPEOF_ClientMenu* pProc = pproc;	\
		for(int i=(aOWMMenus.GetSize()-1); i>=0; i--)	\
		{	\
			if(aOWMMenus[i].pProc == pProc)	\
			{	\
				aOWMMenus.RemoveAt(i);	\
			}	\
		}	\
	}	\


// ע�Ṥ������ť,�����ֱ���:λ������,����,���и��º���,Ȩ��
#define OWM_REGISTERTOOLBAR(imageindex, pproc, pupdateproc, grade) \
	{	\
		TYPEOF_ClientMenu* pProc = pproc;	\
		TYPEOF_UpdateClientMenu* pUpdateProc = pupdateproc;	\
		int nId = 0;	\
		BOOL bFind = FALSE;	\
		for(int i=0; i<aOWMMenus.GetSize(); i++)	\
		{	\
			if(aOWMMenus[i].pProc == pProc)	\
			{	\
				nId = aOWMMenus[i].nId;	\
				bFind = TRUE;	\
			}	\
			if((!bFind) && (aOWMMenus[i].nId >= nId))	\
				nId = aOWMMenus[i].nId + 1;	\
		}	\
		if((!bFind) && (pProc != NULL))	\
		{	\
			OWMMENUITEM omi;	\
			omi.nId = nId;	\
			omi.pProc = pProc;	\
			omi.pUpdateProc = pUpdateProc;	\
			omi.nGrade = grade;	\
			aOWMMenus.Add(omi);	\
			TBBUTTON but;	\
			VERIFY(pToolBar->SendMessage(TB_GETBUTTON, 0, (LPARAM)(&but)));	\
			but.iBitmap = imageindex;	\
			VERIFY(::SendMessage(pToolBar->m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)(&but)));	\
			VERIFY(::SendMessage(pToolBar->m_hWnd, TB_SETCMDID, nIndex, nOWMIdStart+nId));	\
		}	\
	}	\


// ж�ع�������ť,�����ǰ�ť����
#define OWM_UNREGISTERTOOLBAR(pproc) \
	{	\
		TYPEOF_ClientMenu* pProc = pproc;	\
		for(int i=(aOWMMenus.GetSize()-1); i>=0; i--)	\
		{	\
			if(aOWMMenus[i].pProc == pProc)	\
			{	\
				int nId = aOWMMenus[i].nId;	\
				int index = pToolBar->CommandToIndex(nOWMIdStart+nId);	\
				if(index >= 0)	\
				{	\
					VERIFY(::SendMessage(pToolBar->m_hWnd, TB_DELETEBUTTON, index, 0));	\
				}	\
				aOWMMenus.RemoveAt(i);	\
			}	\
		}	\
	}	\


// ע������ҳ,�����ֱ���:���ڵ���,�ڵ���,����,�Ի���ID,�Ի�������,Ȩ��
#define OWM_REGISTEROPTION(szParent, szNode, szDesc, idtemplate, dialog, grade) \
	{	\
		get_dll_resource();	\
		CString lpszParent = szParent;	\
		CString lpszNode = szNode;	\
		CString lpszDesc = szDesc;	\
		CWnd* pOptionPage = new dialog;	\
		if(pOptionPage->IsKindOf(RUNTIME_CLASS(CDialog)))	\
		{	\
			((CDialog*)pOptionPage)->Create(idtemplate);	\
		}	\
		OWMOPTION oo;	\
		oo.strParent = lpszParent;	\
		oo.strNode = lpszNode;	\
		oo.strDesc = lpszDesc;	\
		oo.pOptionPage = pOptionPage;	\
		oo.nGrade = grade;	\
		aOWMOption.Add(oo);	\
		reset_dll_resource();	\
	}	\


// ��ȡ������Ĳ˵�������,��OWM��Դ��ӽ�ȥ,������ToolBar��ID
#define OWM_LOADBARTOMAINFRAME(idbar)	\
	{	\
		get_dll_resource();	\
		CXTMDIFrameWnd* pMainFrame = (CXTMDIFrameWnd*)AfxGetMainWnd();	\
		CXTCoolMenu* pCoolMenu = pMainFrame->GetCoolMenu();	\
		pCoolMenu->LoadToolbar(idbar);	\
		reset_dll_resource();	\
	}	\


// ���˵��Ƿ��Ѿ�����,���������ɾ��,�����ǲ˵������Ͳ˵���
#define OWM_CHECKMENUISEXIST(indexmenu,smenuname)	\
	{	\
		CString strMenuName;	\
		pMenu->GetMenuString(indexmenu, strMenuName, MF_BYPOSITION);	\
		if(smenuname == strMenuName)	\
		{	\
			pMenu->RemoveMenu(indexmenu, MF_BYPOSITION);	\
		}	\
	}	\

// �����ַ�����Դ��,���ڹ��ʻ�,����Ϊ�ַ���������ԴID
#define LANG_LOADSTRING(ssname,ids)	\
		CString ssname;	\
		ssname.LoadString(ids);	\


// ���һ��OWM��չ�˵��������˵���ĳ���˵�������(��ʱ����,����ͨ��ƽ̨�ӿ�ע��ķ�ʽ��Ӳ˵���)
#define	OWM_ADDMENUITEM(mainmenu, menuid, menuindex)	\
{	\
	CMenu menu;	\
	get_dll_resource();	\
	menu.LoadMenu(menuid);	\
	reset_dll_resource();	\
	CString strMenuItem;	\
	menu.GetSubMenu(0)->GetMenuString(menuindex, strMenuItem, MF_BYPOSITION);	\
	CXTPCommandBar* pMenuBar = (CXTPCommandBar*)pMenu;	\
	CXTPControls* pControls = pMenuBar->GetControls();	\
	CXTPControl* pControl = pControls->GetFirst();	\
	CXTPCommandBar* pMenuBarSub = NULL;	\
	CXTPControl* pControlExtSub = NULL;	\
	while(pControl)	\
	{	\
		if(pControl->GetCaption() == mainmenu)	\
		{	\
			pMenuBarSub = pControl->GetCommandBar();	\
			CXTPControls* pControlsSub = pMenuBarSub->GetControls();	\
			pControl = pControlsSub->GetFirst();	\
			while(pControl)	\
			{	\
				if(pControl->GetCaption() == strMenuItem)	\
				{	\
					pControlExtSub = pControl;	\
					break;	\
				}	\
				pControlsSub->GetNext(pControl);	\
			}	\
			break;	\
		}	\
		pControls->GetNext(pControl);	\
	}	\
	if(pMenuBarSub && !pControlExtSub)	\
	{	\
		CXTPControls* pControlsSub = pMenuBarSub->GetControls();	\
		pControlsSub->AddMenuItem(menu.GetSubMenu(0), menuindex);	\
	}	\
}	\


// ɾ��һ��OWM��չ�˵���(��ʱ����,��Ϊͨ��ƽ̨�ӿ�ע��ķ�ʽƽ̨�����Լ�ɾ���˵���,����Ҫ�˹��ͷ�)
#define	OWM_REMOVEMENUITEM(mainmenu, menuid, menuindex)	\
{	\
	if(aOWMMenus.GetSize() == 0)	\
		return 0;	\
	CMenu menu;	\
	get_dll_resource();	\
	menu.LoadMenu(menuid);	\
	reset_dll_resource();	\
	CString strMenuItem;	\
	menu.GetSubMenu(0)->GetMenuString(menuindex, strMenuItem, MF_BYPOSITION);	\
	CXTPCommandBar* pMenuBar = (CXTPCommandBar*)(aOWMMenus[0].pProc);	\
	CXTPControls* pControls = pMenuBar->GetControls();	\
	CXTPControl* pControl = pControls->GetFirst();	\
	CXTPCommandBar* pMenuBarSub = NULL;	\
	while(pControl)	\
	{	\
		if(pControl->GetCaption() == mainmenu)	\
		{	\
			pMenuBarSub = pControl->GetCommandBar();	\
			CXTPControls* pControlsSub = pMenuBarSub->GetControls();	\
			pControl = pControlsSub->GetFirst();	\
			while(pControl)	\
			{	\
				if(pControl->GetCaption() == strMenuItem)	\
				{	\
					pControlsSub->Remove(pControl);	\
					break;	\
				}	\
				pControlsSub->GetNext(pControl);	\
			}	\
			break;	\
		}	\
		pControls->GetNext(pControl);	\
	}	\
}	\


// ���һ��OWM��չ�˵��������˵���ĳ���˵�������(����ƽ̨�ӿ�)
#define	PLATUI_ADDMENUITEM(mainmenu, menuid, menuindex, proc, updateproc)	\
	{	\
		get_dll_resource();	\
		CMenu* pmenu = new CMenu();	\
		pmenu->LoadMenu(menuid);	\
		reset_dll_resource();	\
		theApp.m_pIPlatUI->AddSubMenu(mainmenu, menuindex, pmenu, proc, updateproc);	\
		delete pmenu;	\
	}	\


// �����ַ�����Դ��,���ڹ��ʻ�(��OWMģ����ʹ��ʱ���Լ�����Դ�л�)
#define PLAT_LOADSTRING(s,id)	\
	CString s;	\
	s.LoadString(id);	\


// �����ַ�����Դ��,���ڹ��ʻ�(������Դ�л�)
#define DLL_LOADSTRING(s,id)	\
	get_dll_resource();	\
	CString s;	\
	s.LoadString(id);	\
	reset_dll_resource();	\


#endif // !defined(__OWM_H_)
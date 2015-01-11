////////////////////////////////////////////////////////////////////////////
//	File:		platreg.h
//	Version:	1.0.0.0
//	Created:	2000-06-01
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of platform register.
////////////////////////////////////////////////////////////////////////////
#ifndef __PLATREG_H_
#define __PLATREG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ����汾��Ϣ��ע����Ϣ�Ӽ�
#define REG_VERSION_SUBKEY		_T("Version")		// �汾��Ϣ
#define REG_VER_PLATVERSION		_T("CurVersion")	// ƽ̨�汾
#define REG_VER_NEWVERSION		_T("NewVersion")	// ���°汾
#define REG_REGISTER			_T("Register")		// �Ƿ��Ѿ�ע��
#define REG_REG_USER			_T("RegUser")		// ע���û���
#define REG_REG_USERID			_T("RegUserID")		// ע���û�ID
#define REG_REG_EDITION			_T("Edition")		// �汾����(0=�û��汾,1=�����汾)
#define REG_REG_USE				_T("Use")			// ��;(����/����)
#define REG_REG_PRODUCTLINE		_T("Category")		// ����(����)
#define REG_REG_PRODUCT			_T("Class")			// ����(С��)
#define REG_REG_LANGUAGE		_T("Language")		// ��ǰ����

// ����ƽ̨ͳһ��ʶ�Ӽ�
#define REG_GUID_SUBKEY			_T("EquipInfo")		// ͳһ��ʶ
#define REG_GUID_PLATGUID		_T("GUID")		// ƽ̨ͳһ��ʶ

// ����Path�Ӽ�
#define REG_PATH_SUBKEY			_T("Settings")		// ·���Ӽ�
#define REG_PATH_INSTALL		_T("Install")		// ��װ·��
#define REG_PATH_PROJECT		_T("Project")		// ����Ĺ���·��(v1.0��)

// ��������Ӽ�
#define REG_HELP_SUBKEY			_T("Help")
#define REG_HELP_USEAGENT		_T("UseAgent")		// �Ƿ���������
#define REG_HELP_DAYTIP			_T("DayTip")		// ÿ��һ���ļ�
#define REG_HELP_OPHANDBOOK		_T("OperatorBook")	// �����ֲ�
#define REG_HELP_STARTPAGE		_T("Option")		// ��ʼҳ

// ����������Ϣ�Ӽ�
#define REG_CONFIG_SUBKEY		_T("Settings")
#define REG_CONFIG_DEFFILEEXT	_T("DefaultFileExt")// ȱʡ�Ĵ��ļ��ĺ�׺
#define REG_CONFIG_DEFAULTTREE	_T("DefaultTree")	// ȱʡ�Ĺ�����ҳ��
#define REG_CONFIG_SPLASHTIME	_T("SplashTime")	// ����������ӳ�ʱ��
#define REG_CONFIG_SPLASH		_T("Splash")		// ���������ļ�
#define REG_CONFIG_AUTOLOGIN	_T("AutoLogin")		// �Զ���¼
#define REG_CONFIG_STARTRCSVR	_T("StartRCS")		// �Ƿ�����Զ�̹������
#define REC_CONFIG_ENABLERCADMIN	_T("EnableRCA")	// �Ƿ�����Զ�̹������
#define REG_CONFIG_SHOWADDRESSBAR	_T("ShowAddressBar")// �Ƿ���ʾHTMLҳ��ĵ�ַ������

#define REG_CONFIG_AUTO_RELOAD_DOCUMENT	_T("AutoReloadDocument")// �Զ�������һ�δ򿪵��ĵ�
#define REG_CONFIG_AUTO_OPEN_STARTPAGE	_T("AutoOpenStartPage")// �Զ�����ʼҳ

#define REG_CONFIG_IMAGESTYLE	_T("ImageStyle")		// ͼ����

#define REG_CONFIG_SKIN_TYPE			_T("SkinType")			// ��ǰƤ������
#define REG_CONFIG_SKIN_FILE			_T("SkinFile")			// ʹ����չƤ���ļ�
#define REG_CONFIG_SKIN_INI				_T("SkinIni")			// Ƥ���ļ�INI
#define REG_CONFIG_SKIN_THEME			_T("Theme")				// ����
#define REG_CONFIG_SKIN_THEMECOLOR		_T("ThemeColor")		// ������ɫ
#define REG_CONFIG_SKIN_COLORSCHEMES	_T("SkinColorSchemes")	// ��ɫ
#define REG_CONFIG_SKIN_FONTSIZE		_T("SkinFontSize")		// ���ִ�С
#define REG_CONFIG_SKIN_APPLY_FRAMESKIN	_T("SkinApplyFrameSkin")// Ӧ��FrameSkin
#define REG_CONFIG_SKIN_APPLY_METRICS	_T("SkinApplyMetrics")	// Ӧ��Metrics
#define REG_CONFIG_SKIN_APPLY_COLORS	_T("SkinApplyColors")	// Ӧ��Colors

// ����Ƥ������
enum {
	SKIN_NONE,
	SKIN_OFFICE2007,	// CodeJock Office2007 DLL File
	SKIN_CJINI,		// CodeJock INI File
	SKIN_CJSTYLE,	// CodeJock Style File
	SKIN_MSSTYLE	// Microsoft Style File
};

// ��������
enum {
	themeRibbon,
	themeOffice2003,
	themeOffice2007,
	themeOffice2010,
	themeVisualStudio2005,
	themeVisualStudio2008,
	themeVisualStudio2010,
	themeVisualStudio2012,
};

// ����������ɫ
enum {
	themeColorBlue,
	themeColorSilver,
	themeColorBlack,
};

// ����VCI�Ӽ�
#define REG_VCI_SUBKEY			_T("VCI")		// VCI�Ӽ�
#define REG_VCI_ENABLE_DEBUGOUT	_T("EnableDebugOut")// ����������������Ϣ

// ������������Ӽ�
#define REG_OUTPUT_SUBKEY		_T("Output")
#define REG_OUTPUT_COUNT		_T("Count")			// ƽ̨������ڸ���
#define REG_OUTPUT_DEFAULT		_T("Default")		// ƽ̨ȱʡ�������
#define REG_OUTPUT_LIMITTEXT	_T("LimitText")		// ���������������
#define REG_OUTPUT_SAVETYPE		_T("SaveType")		// �����ļ�����
#define REG_OUTPUT_TRACEID		_T("TraceID")		// Trace���ڵ�ID
#define REG_GRIDOUT_SUBKEY		_T("Output\\GridOut")// Grid��������Ӽ�

// ����״̬���Ӽ�
#define REG_STATUSBAR_SUBKEY	_T("StatusBar")
#define REG_VIEW_DATE			_T("ViewDate")		// ��ʾ����
#define REG_VIEW_TIME			_T("ViewTime")		// ��ʾʱ��
#define REG_VIEW_WEEK			_T("ViewWeek")		// ��ʾ����
#define REG_VIEW_USER			_T("ViewUser")		// ��ʾ��¼�û�
#define REG_VIEW_INFO			_T("ViewInfo")		// ��ʾ��Ϣ
#define REG_ENABLE_SCROLL		_T("EnableScroll")	// ������Ϣ����
#define REG_SETTINGS_SUBKEY		_T("Settings")		// �����Ӽ�
#define REG_SHOWMDITAB			_T("ShowMDITab")	// �Ƿ���ʾMDITab
#define REG_MDITABALIGN			_T("MDITabAlign")	// MDITab���뷽ʽ
#define REG_XPMODE				_T("XpMode")		// �Ƿ�XP������

// ���������ʽ�Ӽ�
#define REG_FORMAT_SUBKEY		_T("Format")
#define REG_FMFONT				_T("Font")			// ������
#define REG_FMSIZE				_T("Size")			// ��С
#define REG_FMCOLORFG			_T("ColorFg")		// ǰ��ɫ
#define REG_FMCOLORBK			_T("ColorBk")		// ����ɫ
#define REG_FMBOLD				_T("Bold")			// �Ƿ����
#define REG_FMITALIC			_T("Italic")		// �Ƿ�б��
#define REG_FMUNDERLINE			_T("UnderLine")		// �»���
#define REG_FMSTRIKEOUT			_T("StrikeOut")		// ɾ����

#define REG_FMT_OUTPUT			_T("Plat.Output")	// �������

// �ض������
#define REG_DIRECT_SUBKEY	_T("interp\\Redirect")			// �ض����Ӽ�
// ����Ϊ��׼����ض���
#define REG_DIRECT_STDOUT_OUTPUT	_T("stdout_output")		// �������
#define REG_DIRECT_STDOUT_FILE		_T("stdout_file")		// �ļ�
#define REG_DIRECT_STDOUT_RESULT	_T("stdout_result")		// �ű���׼���
#define REG_DIRECT_STDOUT_PIPE		_T("stdout_pipe")		// �ܵ�
#define REG_DIRECT_STDOUT_MEMORY	_T("stdout_memory")		// �����ڴ�
#define REG_DIRECT_STDOUT_ADMIN		_T("stdout_admin")		// ����Ա�û�
// �������
#define REG_DIRECT_STDOUT_OUTPUT_ID	_T("stdout_output_id")	// �������ID

// �����¼�Ӽ�
#define REG_LOGIN_SUBKEY		_T("Login")
#define REG_LG_USERNAME			_T("UserName")		// �û���
#define REG_LG_PASSWORD			_T("Key")			// �û�����
#define REG_LG_DOMAIN			_T("Domain")		// ��¼��
#define REG_LG_RCADMIN			_T("RCAdmin")		// Զ�̹���Ա�û���
#define REG_LG_RCPWD			_T("RCPWD")			// Զ�̹���Ա����

// ����OWM�༭���Ӽ�
#define REG_OWMEDIT				_T("OwmEdit")
#define REG_DEFAULTDOC			_T("DefaultDoc")	// ȱʡ�༭����

#define REG_FIND_SUBKEY			_T("OwmEdit\\Find")
#define REG_REPLACE_SUBKEY		_T("OwmEdit\\Replace")
#define REG_FIND_HISTORY		_T("OwmEdit\\FindHistory")
#define REG_MATCH_CASE			_T("MatchCase")		// ���ִ�Сд
#define REG_WHOLE_WORD			_T("WholeWord")		// ����ƥ��
#define REG_FIND_WHAT			_T("FindWhat")		// �����ִ�
#define REG_REPLACE_WITH		_T("ReplaceWith")	// �滻�ִ�

#define REG_PAGE_SUBKEY			_T("OwmEdit\\PageSetup")
#define REG_MARGIN_LEFT			_T("LeftMargin")
#define REG_MARGIN_RIGHT		_T("RightMargin")
#define REG_MARGIN_TOP			_T("TopMargin")
#define REG_MARGIN_BOTTOM		_T("BottomMargin")

// ���峬������Ա��Ϣ
#define USER_SUPERADMIN			_T("Administrator")
#define USER_SUPERADMINID		_T("00001")
#define USER_SUPERADMINPWD		_T("")
#define USER_SUPERADMINPWDV		_T("")				// ������֤
#define USER_SUPERADMINDESC		_T("SuperAdmin")

// ���幤���Ӽ�
#define REG_PROJECT_SUBKEY		_T("Project")
#define REG_PROJECT_LOADLAST	_T("LoadLast")		// �Զ������������
#define REG_PROJECT_AUTOSAVE	_T("AutoSave")		// �˳��Զ����湤��
#define REG_PROJECT_DEFFILEEXT	_T("DefaultProjectExt")// ȱʡ�Ĵ򿪹��̵ĺ�׺
#define REG_CUR_CLASS			_T("CurCategory")		// ��ǰ���̴���
#define REG_CUR_SCLASS			_T("CurClass")		// ��ǰ����С��
#define REG_CUR_PROJECT			_T("CurProject")	// ��ǰ����
#define REG_PROJECT_DEFAULTWS	_T("DefaultWS")		// ȱʡ��λ

// ������������Ӽ�
#define REG_RECENTPRJ_SUBKEY	_T("RecentProject")
#define REG_RECENT_PROJECT		_T("PROJECT")		// ����
#define	MAX_RECENT_PROJECT		10					// ��ౣ�����
#define ID_RECENT_PROJECT_1		900					// ��һ�����̲˵�ID
#define ID_RECENT_PROJECT_10	909					// ��10�����̲˵�ID

// ���幤���ļ���ʶ
#define PRJ_SIGN				_T("Script.NET Project File, Copyright Blueant Software.")
#define PRJ_VERSION				_T("Version=4.0.0.0")
#define PRJ_SERIAL				_T("{6084F200-C7F8-464B-B067-A4AE5B9CE9DD}")

// ������־�鿴����(����ϵͳ��������)
#define REG_LOG_SUBKEY		_T("Log")				// ��־�Ӽ�
#define REG_LOG_LISTRANGE	_T("ListRange")			// �鿴��Χ
#define REG_LOG_LASTDAY		_T("ListLastDay")		// �������
#define REG_LOG_DATARANGE	_T("ListDataRange")		// ��ʼ����

// ������־�������(����ϵͳ��������)	
#define REG_LOGSAVE_SUBKEY		_T("Log")			// ��־�����Ӽ�
#define REG_LOGSAVE_SAVERANGE	_T("SaveRange")		// ���淶Χ
#define REG_LOGSAVE_LASTDAY		_T("SaveLastDay")	// �������
#define REG_LOGSAVE_MAXITEM		_T("SaveMaxItem")	// �������
#define REG_LOGSAVE_AUDIT_SUCC	_T("AuditSucc")		// �ɹ����
#define REG_LOGSAVE_AUDIT_FAIL	_T("AuditFail")		// ʧ�����
#define REG_LOGSAVE_INFOMATION	_T("Information")	// ��Ϣ
#define REG_LOGSAVE_WARNING		_T("Warning")		// ����
#define REG_LOGSAVE_ERROR		_T("Error")			// ����

// ��Դ���������Ӽ�
#define REG_RESMG_SUBKEY		_T("Config\\ResManager")
#define REG_RESMG_SHOWRESMG		_T("ShowResManager")	// ��ʾ��Դ����ҳ��
#define REG_RESMG_SHOWPROJECT	_T("ShowProject")		// ��ʾ������
#define REG_RESMG_SHOWEQUIP		_T("ShowEquip")			// ��ʾ�豸��
#define REG_RESMG_SHOWDEVICE	_T("ShowDevice")		// ��ʾ������
#define REG_RESMG_SHOWOWM		_T("ShowOwm")			// ��ʾOWM��
#define REG_RESMG_SHOWTCLINTERP	_T("ShowTclInterp")		// ��ʾTCL��������
#define REG_RESMG_SHOWTOOLS		_T("ShowTools")			// ��ʾ������
#define REG_RESMG_SHOWDOCUMENT	_T("ShowDocument")		// ��ʾ�ĵ���

// ����������
#define REG_UISTYLE_SUBKEY		_T("Config\\UIStyle")	// �������Ӽ�
#define REG_UISTYLE_HTML		_T("Html")				// Htmlҳ����

#endif // !defined(__PLATREG_H_)
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A81570F9_8FF3_11D5_9949_0050BA0C48D0__INCLUDED_)
#define AFX_STDAFX_H__A81570F9_8FF3_11D5_9949_0050BA0C48D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

/*
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT
*/
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>

HINSTANCE GetDllInstance();

// ��Դ�л�����
void get_dll_resource(void);
void reset_dll_resource(void);

#define LOADRCSTRING(str,strID) \
	get_dll_resource();\
	str.LoadString(strID);\
	reset_dll_resource();

// ��ȡ��ǰ����
int gGetCurrentLanguage();

// ��ȡ������·��
CString GetInterpRootPath();

// �Ƿ�֧��TK�ı��뿪��
#define _SUPPORT_TK

// �Ƿ�ʹ��Tcl8.5�汾����
//#define _USE_TCL84
#define _USE_TCL85
//#define _USE_TCL86

#ifdef _USE_TCL84
#include "../include/tcl.h"
#ifdef _SUPPORT_TK
#include "../include/tk.h"
#endif
#endif

#ifdef _USE_TCL85
#include "../include/tcl8.5/tcl.h"
#ifdef _SUPPORT_TK
#include "../include/tcl8.5/tk.h"
#endif
#endif

#ifdef _USE_TCL86
#define USE_INTERP_RESULT
#define USE_INTERP_ERRORLINE
#include "../include/tcl8.6/tcl.h"
#ifdef _SUPPORT_TK
#include "../include/tcl8.6/tk.h"
#endif
#endif

#include "platdef.h"		// platform common define

#include "IPlatUI.h"	// ƽ̨���ܽӿ�
#include "owm.h"
#include "ILicense.h"

#include "XMLDom.h"

//////////////////////////////////////////////////////////////////////
// �Զ�����TCL��TK��
#ifdef _USE_TCL84
#pragma comment(lib, "tcl84.lib")		// �Զ�����Tcl��
#pragma message("Automatically linking with tcl84.lib")
#ifdef _SUPPORT_TK
#pragma comment(lib, "tk84.lib")		// �Զ�����Tk��
#pragma message("Automatically linking with tk84.lib")
#endif
#endif

#ifdef _USE_TCL85
#pragma comment(lib, "tcl85.lib")		// �Զ�����Tcl��
#pragma message("Automatically linking with tcl85.lib")
#ifdef _SUPPORT_TK
#pragma comment(lib, "tk85.lib")		// �Զ�����Tk��
#pragma message("Automatically linking with tk84.lib")
#endif
#endif

#ifdef _USE_TCL86
#pragma comment(lib, "tcl86.lib")		// �Զ�����Tcl��
#pragma message("Automatically linking with tcl86.lib")
#ifdef _SUPPORT_TK
#pragma comment(lib, "tk86.lib")		// �Զ�����Tk��
#pragma message("Automatically linking with tk86.lib")
#endif
#endif

// ����汾��Ϣ��ע����Ϣ�Ӽ�
#define REG_VERSION_SUBKEY		_T("Version")		// �汾��Ϣ
#define REG_VER_PLATVERSION		_T("CurVersion")	// ƽ̨�汾

// ����������
#define REG_UISTYLE_SUBKEY		_T("Config\\UIStyle")	// �������Ӽ�
#define REG_UISTYLE_HTML		_T("Html")				// Htmlҳ����

// ����ע����
#define REG_INTERP_SUBKEY			_T("interp\\tcl\\")

// ������ؿ��ӽ�
#define REG_TCL_LIB_SUBKEY			_T("interp\\tcl\\Lib")
#define REG_TCL_LIB_TK				_T("Tk")
#define REG_TCL_LIB_ITCL			_T("iTcl")
#define REG_TCL_LIB_ITK				_T("iTk")

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

// TCL����������
#define REG_TCL_DEBUG_SUBKEY	_T("interp\\tcl\\Debug")	// TCL�����Ӽ�
#define REG_TCL_DEBUG_RTVAR		_T("RTVariable")	// ʵʱ���±�����Ϣ
#define REG_TCL_DEBUG_RTOBJECT	_T("RTObject")		// ʵʱ���¶�����Ϣ
#define REG_TCL_DEBUG_RTPROC	_T("RTProcedure")	// ʵʱ���¹�����Ϣ
#define REG_TCL_DEBUG_RTSTACK	_T("RTStack")		// ʵʱ���¶�ջ��Ϣ

#define REG_TCL_ENABLE_PROFILE	_T("EnableProfileAnalysis")	// �������ʷ���
#define REG_TCL_ENABLE_MULTIFILEDEBUG	_T("EnableMultiFileDebug")	// ������ļ�����
#define REG_TCL_MULTIDEBUG_QUERY	_T("MultiFileDebugQuery")	// ���ļ�����ʱ���Ƿ���ʾ���شӵ����ļ�
#define REG_TCL_MULTIDEBUG_EXCLUDE_LIB	_T("MultiDebugExcludeLib")	// ���ļ�����ʱ���ų�������libĿ¼�µ��ļ�

// TCL����ѡ��
#define REG_TCL_COMPILE_SUBKEY	_T("interp\\tcl\\Compile")	// TCL����ѡ���Ӽ�
#define REG_TCL_COMPILE_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_TCL_COMPILE_FORCE_OVERWRITE	_T("ForceOverwrite")// ǿ�Ƹ���
#define REG_TCL_COMPILE_ADD_PREFIX		_T("AddPrefix")		// ����ļ�ǰ׺

// TCL����ѡ��
#define REG_TCL_BUILD_SUBKEY	_T("interp\\tcl\\Build")	// TCL����ѡ���Ӽ�
#define REG_TCL_BUILD_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_TCL_BUILD_ADD_TCL_LIB	_T("AddTclLib")		// �Ƿ�ѡ��Tcl��չ����ӵ������ļ���
#define REG_TCL_BUILD_ADD_SRC_PATHFILES	_T("AddSourcePathFiles")// �Ƿ����Դ�ļ�Ŀ¼�µ������ļ�
#define REG_TCL_BUILD_ADD_SRC_LIB	_T("AddSourceLib")	// �Ƿ����Դ�ļ�Ŀ¼�µ�LibĿ¼��������
#define REG_TCL_BUILD_FORCE_TKKIT	_T("ForceTkKit")	// ǿ�ư�TK�ű�����
#define REG_TCL_BUILD_REPLACE_ICON	_T("ReplaceIcon")	// �滻��ִ���ļ���ͼ��
#define REG_TCL_BUILD_SELECT_ICON_TYPE	_T("SelectIconType")	// �û�ѡ��ͼ��ķ�ʽ


// ����ƽ̨�汾
#define VAR_PLATFORM_VERSION	_T("4.0.0.0")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A81570F9_8FF3_11D5_9949_0050BA0C48D0__INCLUDED_)

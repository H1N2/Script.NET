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

#include "platdef.h"		// platform common define

#include "IPlatUI.h"	// ƽ̨���ܽӿ�
#include "owm.h"

#include "config.h"
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define USE_PERL_510	// ʹ��Perl5.10�汾

// ���嵱ǰʹ�õ�Perl�汾Lib�ĸ�Ŀ¼
#define PERL_LIB_PATH _T("perl-5.10")

// �Զ�����Perl��
#ifdef USE_PERL_510
#pragma comment(lib, "perl510.lib")		// �Զ�����Perl��
#pragma message("Automatically linking with perl510.lib")
#endif

// ����ע����
#define REG_INTERP_SUBKEY			_T("interp\\perl\\")

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

// PERL����������
#define REG_PERL_DEBUG_SUBKEY	_T("interp\\perl\\Debug")	// Perl�����Ӽ�

#define REG_PERL_ENABLE_PROFILE	_T("EnableProfileAnalysis")	// �������ʷ���
#define REG_PERL_ENABLE_MULTIFILEDEBUG	_T("EnableMultiFileDebug")	// ������ļ�����
#define REG_PERL_MULTIDEBUG_QUERY	_T("MultiFileDebugQuery")	// ���ļ�����ʱ���Ƿ���ʾ���شӵ����ļ�
#define REG_PERL_MULTIDEBUG_EXCLUDE_LIB	_T("MultiDebugExcludeLib")	// ���ļ�����ʱ���ų�������libĿ¼�µ��ļ�

// PERL����ѡ��
#define REG_PERL_COMPILE_SUBKEY	_T("interp\\perl\\Compile")	// Perl����ѡ���Ӽ�
#define REG_PERL_COMPILE_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_PERL_COMPILE_FORCE_OVERWRITE	_T("ForceOverwrite")// ǿ�Ƹ���

// PERL����ѡ��
#define REG_PERL_BUILD_SUBKEY		_T("interp\\perl\\Build")	// Perl����ѡ���Ӽ�
#define REG_PERL_BUILD_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_PERL_BUILD_ADD_PERL_LIB	_T("AddPerlLib")	// �Ƿ�ѡ��Perl��չ����ӵ������ļ���
#define REG_PERL_BUILD_ADD_SRC_PATHFILES	_T("AddSourcePathFiles")// �Ƿ����Դ�ļ�Ŀ¼�µ������ļ�
#define REG_PERL_BUILD_ADD_SRC_DATA	_T("AddSourceData")	// �Ƿ����Դ�ļ�Ŀ¼�µ�dataĿ¼��������
#define REG_PERL_BUILD_SELECT_TYPE	_T("SelectType")	// ѡ���ִ���ļ�����
#define REG_PERL_BUILD_REPLACE_ICON	_T("ReplaceIcon")	// �滻��ִ���ļ���ͼ��
#define REG_PERL_BUILD_SELECT_ICON_TYPE	_T("SelectIconType")	// �û�ѡ��ͼ��ķ�ʽ

// ���������Ϣ
#define _WND_DEBUG	1		// ������Դ���

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A81570F9_8FF3_11D5_9949_0050BA0C48D0__INCLUDED_)

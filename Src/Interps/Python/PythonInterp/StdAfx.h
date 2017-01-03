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

#define qCvsDebug 0

// TCL�ض������
#define REG_PY_DIRECT_SUBKEY		_T("interp\\python\\Redirect")	// Python�ض����Ӽ�
// ����Ϊ��׼����ض���
#define REG_PY_DIRECT_STDOUT_OUTPUT	_T("stdout_output")		// �������
#define REG_PY_DIRECT_STDOUT_FILE	_T("stdout_file")		// �ļ�
#define REG_PY_DIRECT_STDOUT_RESULT	_T("stdout_result")		// �ű���׼���
#define REG_PY_DIRECT_STDOUT_PIPE	_T("stdout_pipe")		// �ܵ�
#define REG_PY_DIRECT_STDOUT_MEMORY	_T("stdout_memory")		// �����ڴ�
#define REG_PY_DIRECT_STDOUT_ADMIN	_T("stdout_admin")		// ����Ա�û�
// �������
#define REG_PY_DIRECT_STDOUT_OUTPUT_ID	_T("stdout_output_id")	// �������ID

// Python���ؿ�·��
#define REG_PY_LIB_SUBKEY			_T("interp\\python\\Library")	// Python���ؿ��Ӽ�
#define REG_PY_LIB_PATH				_T("Path")				// ���ؿ�·��
#define REG_PY_USE_EXTERIOR_LIB		_T("UseExteriorPythonLib")	// ʹ���ⲿPython·������
#define REG_PY_LASTLIBVAL			"LastLoadLibrary"		// ��һ�μ��صĿ�

// Python����������
#define REG_PY_DEBUG_SUBKEY			_T("interp\\python\\Debug")	// Python�����Ӽ�
#define REG_PY_DEBUG_RTVAR			_T("RTVariable")		// ʵʱ���±�����Ϣ
#define REG_PY_DEBUG_RTOBJECT		_T("RTObject")			// ʵʱ���¶�����Ϣ
#define REG_PY_DEBUG_RTPROC			_T("RTProcedure")		// ʵʱ���¹�����Ϣ
#define REG_PY_DEBUG_RTSTACK		_T("RTStack")			// ʵʱ���¶�ջ��Ϣ

#define REG_PY_ENABLE_PROFILE		_T("EnableProfileAnalysis")	// �������ʷ���

// Python����ѡ��
#define REG_PY_COMPILE_SUBKEY		_T("interp\\python\\Compile")	// Python����ѡ���Ӽ�
#define REG_PY_COMPILE_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_PY_COMPILE_FORCE_OVERWRITE	_T("ForceOverwrite")// ǿ�Ƹ���

// Python����ѡ��
#define REG_PY_BUILD_SUBKEY			_T("interp\\python\\Build")	// Python����ѡ���Ӽ�
#define REG_PY_BUILD_AUTO_OUTFILE	_T("AutoOutFile")	// �Զ���������ļ�
#define REG_PY_BUILD_ADD_PYTHON_LIB	_T("AddPythonLib")	// �Ƿ�ѡ��Python��չ����ӵ������ļ���
#define REG_PY_BUILD_ADD_SRC_PATHFILES	_T("AddSourcePathFiles")// �Ƿ����Դ�ļ�Ŀ¼�µ������ļ�
#define REG_PY_BUILD_ADD_SRC_DATA	_T("AddSourceData")	// �Ƿ����Դ�ļ�Ŀ¼�µ�dataĿ¼��������
#define REG_PY_BUILD_SELECT_TYPE	_T("SelectType")	// ѡ���ִ���ļ�����
#define REG_PY_BUILD_REPLACE_ICON	_T("ReplaceIcon")	// �滻��ִ���ļ���ͼ��
#define REG_PY_BUILD_SELECT_ICON_TYPE	_T("SelectIconType")	// �û�ѡ��ͼ��ķ�ʽ

//////////////////////////////////////////////////////////////////////
//	����Python���ؿ�·��											//
//////////////////////////////////////////////////////////////////////
#define PYTHON_LIB_PATH	_T("INTERP://\\;INTERP://\\DLLs;INTERP://\\Lib;INTERP://\\Lib\\lib-tk;INTERP://\\Lib\\site-packages;INTERP://\\Lib\\site-packages\\win32;INTERP://\\Lib\\site-packages\\win32\\lib;INTERP://\\Lib\\site-packages\\PIL;INTERP://\\Plat")

// ����Python��Ŀ¼(�ڽ�����Ŀ¼�Ļ����ϵ���Ŀ¼)
#define PYTHON_HOME_PATH _T("Python2.7\\")
//#define PYTHON_HOME_PATH _T("")

// ���������Ϣ
#define _WND_DEBUG	1		// ������Դ���


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A81570F9_8FF3_11D5_9949_0050BA0C48D0__INCLUDED_)

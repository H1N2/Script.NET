// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A8E73573_3C54_415C_9986_6D8DF4D5883A__INCLUDED_)
#define AFX_STDAFX_H__A8E73573_3C54_415C_9986_6D8DF4D5883A__INCLUDED_

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


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>

#include "platdef.h"		// platform common define

#include "owm.h"		// OWM����ͷ�ļ�
#include "IPlatUI.h"	// ƽ̨���ܽӿ�
#include "IOwmGrep.h"	// Grep Owm�����

#import "RegExp.tlb"
using namespace VBScript_RegExp_10;

#include <XTToolkitPro.h>   // Xtreme Toolkit Pro components

// ����ID����
#define VIEW_ID_GREP			10002	// �����滻����
#define VIEW_ID_SEARCH_RESULT	10003	// ���ҽ������

#include "SearchOptions.h"
CSearchOptions* GetSearchOptions();

// ��Դ�л�����
void get_dll_resource(void);
void reset_dll_resource(void);

// OWM���Ժ궨��
#define _IF_OWM_DEBUG		if(theApp.m_bDebug){
#define _ENDIF_OWM_DEBUG	}
#define _IF_OWM_NDEBUG		if(!theApp.m_bDebug){
#define _ENDIF_OWM_NDEBUG	}

// ��ƽ̨��ѯOWM��װ��Ϣ
COWMArray* GetOwmInfo(int nId);

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

// ����GREP�Ӽ�
#define REG_GREP				_T("Grep")
#define REG_SHOW_REPLACE_LINE	_T("ShowReplaceLine")	// �Ƿ���ʾ�滻�����


////////////////////////////////////////////////////////////////////
// OwmEdit��չ����
#define OC_OPENFILE				102	// ��һ���ĵ�(to owm)
#define OC_OPENFILEFIRST		103	// �ڵ�һ��λ�ô��ĵ�(to owm)
#define OC_SETREADONLY			104	// �����ĵ���ֻ������
#define OC_GETOPENFILE			105	// ��ȡ�򿪵��ļ���

#define OC_SETEXECUTION			110	// ���ô�����
#define OC_SETCURENTLINE		111	// ���õ�ǰ��
#define OC_SETCURENTEXECLINE	112	// ���õ�ǰִ����
#define OC_SEARCHLINE			113	// ����ָ�����ݵ��в�����Ϊ��ǰ��
#define OC_GETBREAKLIST			115	// ��ȡ�ϵ��б�
#define OC_SETSLAVEDEBUG		116	// ���ôӵ���ģʽ(����ģʽ,���������Գ���)

#define OC_GET_LINECOUNT		117	// ��ȡ������
#define OC_GET_LINE				118	// ��ȡ��
#define OC_SET_LINE				119	// ������
#define OC_GET_SELECTLINE		120	// ��ȡ��ǰѡ����з�Χ
#define OC_SET_SELECTLINE		121	// ����ѡ�����
#define OC_GET_CURSORPOS		122	// ��ȡ���λ��
#define OC_SET_CURSORPOS		123	// ���ù��λ��
#define OC_SET_BREAKLIST		124	// ���öϵ��б�
#define OC_CLEAR_BREAK			125	// ��նϵ�

#define OC_EDIT_FIND			130	// ����
#define OC_EDIT_REPLACE			131	// �滻

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A8E73573_3C54_415C_9986_6D8DF4D5883A__INCLUDED_)

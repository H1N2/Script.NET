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

#include <afxsock.h>		// MFC socket extensions

#include <afxrich.h>

#include <afxtempl.h>

#include "owm.h"		// OWM����ͷ�ļ�
#include "IPlatUI.h"	// ƽ̨���ܽӿ�
#include "IInterp.h"	// �������ӿ�

#include <XTToolkitPro.h> // Xtreme Toolkit Pro components

#include "terminal.h"

#include "TermReg.h"

// �ж��Ƿ�Tcl������
inline BOOL IsTclInterp(int nInterpID) { return (nInterpID < 0x01000000); }

/////////////////////////////////////////////////////////////////////////////
// Telnet֧�ֲ��ֵ�ͷ�ļ�
#include "SocketRx.h"
#include "SocketDx.h"
#include "SocketTx.h"

/////////////////////////////////////////////////////////////////////////////
// ����Զ�̹�����뿪�أ�����򿪣���RCAdmin���ֹ��������������
// ʹ����̨֧��Զ�̳������
/////////////////////////////////////////////////////////////////////////////
#define INCLUDE_RCADMIN

// ����ID����
#define VIEW_ID_TERMINAL	10013	// ����̨����

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

int AboutOWM(void);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A8E73573_3C54_415C_9986_6D8DF4D5883A__INCLUDED_)

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D9999D8D_69D0_4E39_9D55_F698C1A1160B__INCLUDED_)
#define AFX_STDAFX_H__D9999D8D_69D0_4E39_9D55_F698C1A1160B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

//////////////////////////////////////////////////////////////////////
//	���³�������ע��Ľ��̼�ͨ����Ϣ								//
// registered message that our application will respond to			//
//////////////////////////////////////////////////////////////////////
static const UINT WM_CHECK_ITS_ME = RegisterWindowMessage("##SCRIPT.NET##");

//#include <XTSuitePro.h>	// Xtreme Toolkit Pro components
#include <XTToolkitPro.h>   // Xtreme Toolkit Pro components

//////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�����ƽ̨�����Ķ��壬�������ϵͳ��չģ��Ķ���		//
//////////////////////////////////////////////////////////////////////
#include "platdef.h"		// platform common define
#include "plat.h"			// platform internal define
#include "PlatReg.h"

#include "XMLDom.h"
#include "Timer.h"

#include "vcicomm.h"

#include "owm.h"
#include "vciowm.h"

#include "IInterp.h"
#include "IProjectManager.h"
#include "IProject.h"
#include "IMessageQueue.h"

#include "PlatUI.h"




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif

#endif // !defined(AFX_STDAFX_H__D9999D8D_69D0_4E39_9D55_F698C1A1160B__INCLUDED_)

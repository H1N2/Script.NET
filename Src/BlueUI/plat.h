////////////////////////////////////////////////////////////////////////////
//	File:		plat.h
//	Version:	2.0.0.0
//	Created:	2000-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of platform.
////////////////////////////////////////////////////////////////////////////
#ifndef __PLAT_H_
#define __PLAT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
// COM Message define
#define WM_PLAT_COMCMD		(WM_USER+50)

struct STRU_COMCMD
{
	BOOL	bPost;		// �Ƿ�PostMessage
	CString	strOwmId;	// ģ��ID(�ַ���)
	int		nCmd;		// ����
	WPARAM	wParam;		// ����1
	LPARAM	lParam;		// ����2(���nDataLen>0���ʾЯ�������ݵ�ָ��)
	LPVOID	lpResult;	// ����ֵ
	int		nDataLen;	// ���ݳ���
};

// �м��COM�ӿڵ���ƽ̨����
#define COM_CMD(owm, cmd, w, l)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD	comcmd;	\
			comcmd.bPost = FALSE;	\
			comcmd.strOwmId = owm;	\
			comcmd.nCmd = cmd;	\
			comcmd.wParam = (WPARAM)w;	\
			comcmd.lParam = (LPARAM)l;	\
			comcmd.lpResult = NULL;	\
			comcmd.nDataLen	= 0;	\
			pMainFrame->SendMessage(WM_PLAT_COMCMD, (WPARAM)0, (LPARAM)(&comcmd));	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(������ֵ��)
#define RCOM_CMD(owm, cmd, w, l, r)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD	comcmd;	\
			comcmd.bPost = FALSE;	\
			comcmd.strOwmId = owm;	\
			comcmd.nCmd = cmd;	\
			comcmd.wParam = (WPARAM)w;	\
			comcmd.lParam = (LPARAM)l;	\
			comcmd.lpResult = r;	\
			comcmd.nDataLen	= 0;	\
			pMainFrame->SendMessage(WM_PLAT_COMCMD, (WPARAM)0, (LPARAM)(&comcmd));	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(PostMessage��ʽ)
#define PCOM_CMD(owm, cmd, w, l)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD* pcomcmd = new STRU_COMCMD;	\
			pcomcmd->bPost = TRUE;	\
			pcomcmd->strOwmId = owm;	\
			pcomcmd->nCmd = cmd;	\
			pcomcmd->wParam = (WPARAM)w;	\
			pcomcmd->lParam = (LPARAM)l;	\
			pcomcmd->lpResult = NULL;	\
			pcomcmd->nDataLen = 0;	\
			if(!pMainFrame->PostMessage(WM_PLAT_COMCMD, (WPARAM)0, (LPARAM)(pcomcmd)))	\
			{	\
				delete pcomcmd;	\
			}	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(������ֵ��,PostMessage��ʽ)
#define PRCOM_CMD(owm, cmd, w, l, r)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD* pcomcmd = new STRU_COMCMD;	\
			pcomcmd->bPost = TRUE;	\
			pcomcmd->strOwmId = owm;	\
			pcomcmd->nCmd = cmd;	\
			pcomcmd->wParam = (WPARAM)w;	\
			pcomcmd->lParam = (LPARAM)l;	\
			pcomcmd->lpResult = r;	\
			pcomcmd->nDataLen = 0;	\
			if(!pMainFrame->PostMessage(WM_PLAT_COMCMD, (WPARAM)0, (LPARAM)(pcomcmd)))	\
			{	\
				delete pcomcmd;	\
			}	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(SendMessage��ʽ,����Я������)
#define DCOM_CMD(owm, cmd, w, l, dl)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD* pcomcmd = new STRU_COMCMD;	\
			pcomcmd->bPost = TRUE;	\
			pcomcmd->strOwmId = owm;	\
			pcomcmd->nCmd = cmd;	\
			pcomcmd->wParam = (WPARAM)w;	\
			pcomcmd->lpResult = NULL;	\
			BYTE* pData = new BYTE[dl];	\
			pcomcmd->lParam = (LPARAM)pData;	\
			memcpy((BYTE*)(pcomcmd->lParam), l, dl);	\
			pcomcmd->nDataLen = dl;	\
			if(!pMainFrame->SendMessage(WM_PLAT_COMCMD, (WPARAM)(LPCTSTR)(owm), (LPARAM)(pcomcmd)))	\
			{	\
				delete pData;	\
				delete pcomcmd;	\
			}	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(PostMessage��ʽ,����Я������)
#define PDCOM_CMD(owm, cmd, w, l, dl)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD* pcomcmd = new STRU_COMCMD;	\
			pcomcmd->bPost = TRUE;	\
			pcomcmd->strOwmId = owm;	\
			pcomcmd->nCmd = cmd;	\
			pcomcmd->wParam = (WPARAM)w;	\
			pcomcmd->lpResult = NULL;	\
			BYTE* pData = new BYTE[dl];	\
			pcomcmd->lParam = (LPARAM)pData;	\
			memcpy((BYTE*)(pcomcmd->lParam), l, dl);	\
			pcomcmd->nDataLen = dl;	\
			if(!pMainFrame->PostMessage(WM_PLAT_COMCMD, (WPARAM)(LPCTSTR)(owm), (LPARAM)(pcomcmd)))	\
			{	\
				delete pData;	\
				delete pcomcmd;	\
			}	\
		}	\
	}	\

// �м��COM�ӿڵ���ƽ̨����(������ֵ��,PostMessage��ʽ,����Я������)
#define PRDCOM_CMD(owm, cmd, w, l, r, dl)	\
	{	\
		CMainFrame* pMainFrame = (CMainFrame*)(theApp.m_pMainWnd);	\
		if(pMainFrame)	\
		{	\
			STRU_COMCMD* pcomcmd = new STRU_COMCMD;	\
			pcomcmd->bPost = TRUE;	\
			pcomcmd->strOwmId = owm;	\
			pcomcmd->nCmd = cmd;	\
			pcomcmd->wParam = (WPARAM)w;	\
			BYTE* pData = new BYTE[dl];	\
			pcomcmd->lParam = (LPARAM)pData;	\
			pcomcmd->lpResult = r;	\
			pcomcmd->nDataLen = dl;	\
			memcpy((BYTE*)(pcomcmd->lParam), l, dl);	\
			if(!pMainFrame->PostMessage(WM_PLAT_COMCMD, (WPARAM)(LPCTSTR)(owm), (LPARAM)(pcomcmd)))	\
			{	\
				delete pData;	\
				delete pcomcmd;	\
			}	\
		}	\
	}	\

#define WS_PLAT		-2
#define WS_ALL		-1


////////////////////////////////////////////////////////////////////////////
// Log define
#define MAXLOGFILESIZE			(1024 << 10)	// ��־�ļ���󳤶�
#define _LOG_DATABUFF			1024			// ��־���ݻ�������С

// ��־����
#define	__DEBUG					"DEBUG"
#define	__ERROR					"ERROR"
#define	__INFO					"INFO"
#define	__CRITICAL				"CRITICAL"

#define	LOG_FILE_NAME			"BlueUILog"		// ��־�ļ���

// ת���ļ���
#define	LOG_CONVEY_FILE_NAME	"BlueUILogBak"
#define	LOG_CONVEY_RULE			"BlueUILogBak.*"
#define	LOG_MAX_SAVE_NUM		5

////////////////////////////////////////////////////////////////////////////
// PlatForm Message define

#define WM_PLAT_DYNCLOSE		(WM_USER+51)


////////////////////////////////////////////////////////////////////////////
// PropertyPage define

// ��������ҳ���ڵ�����
#define OPT_PLATFORM	0	// ƽ̨
#define OPT_OWM			1	// OWM
#define OPT_VCI			2	// VCI���
#define OPT_TESTDLL		3	// ����DLL
#define OPT_INTERP		4	// ������
#define OPT_NODE		5	// һ��ڵ�


/////////////////////////////////////////////////////////////////////
// ����������ڿ��ƶ���
class COutputWndCtrl : public CObject
{
public:
	COutputWndCtrl(){};
	~COutputWndCtrl(){};

	COutputWndCtrl(const COutputWndCtrl& other)
	{
		nID		= other.nID;
		strTitle= other.strTitle;
		nCtrlType= other.nCtrlType;
		pWnd	= other.pWnd;
		pPane	= other.pPane;
		bSave	= other.bSave;
	}

	COutputWndCtrl& operator = (const COutputWndCtrl& other)
	{
		nID		= other.nID;
		strTitle= other.strTitle;
		nCtrlType= other.nCtrlType;
		pWnd	= other.pWnd;
		pPane	= other.pPane;
		bSave	= other.bSave;
		return *this;
	}

public:
	int		nID;		// ����ID
	CString	strTitle;	// ����
	int		nCtrlType;	// ��������
	CWnd*	pWnd;		// ����ָ��
	CXTPDockingPane* pPane; // Pane
	BOOL	bSave;		// �Ƿ񱣴�״̬
};

typedef CArray<COutputWndCtrl, COutputWndCtrl&> COutputCtrlArray;

// �����������
enum{
	OUTPUT_RICH,
	OUTPUT_GRID,
	OUTPUT_TREE,
};

// �������������ɫ
#define COLOR_NORMAL	0
#define COLOR_BLUE		1
#define COLOR_ERROR		2

// ������ڲ�������
void OUTPUT_CLEAR(int nID = 0);
int OUTPUT_ACTIVE_ID(int nID);
int OUTPUT_SET_ID(int nID, CString strFormat);
CString OUTPUT_GET_ID(int nID, CString strParam);
int OUTPUT_TREEITEMS_ID(int nID, LPVOID pItems);
int OUTPUT_COLOR(COLORREF color, LPCTSTR format, ...);
int OutputText(int nID, CString strText, COLORREF color);
int OUTPUT(int nFmt, LPCTSTR format, ...);
int OUTPUT_ID(int nID, LPCTSTR format, ...);
int OUTPUT_ID_DIRECT(int nID, LPCTSTR strOut);
BOOL OUTPUT_VERIFY(CString strActual, CString strVerify);
BOOL STRING_VERIFY(CString strActual, CString strVerify);
void StringFillSpace(CString& strLine, int nLen);
CString ReverseString(CString strInput);

// �������
//#define _ENABLE_DEBUG_OUTPUT
#ifdef _ENABLE_DEBUG_OUTPUT
	#define DEBUG_OUTPUT OUTPUT
#else
	#define DEBUG_OUTPUT //
#endif


/////////////////////////////////////////////////////////////////
// PopupControl define

// PopupControl�ؼ����¼�ID����
#define	ID_POPUP_CLOSE			1	// �رհ�ť
#define ID_POPUP_GOTO_SITE1		2	// ���������
#define ID_POPUP_GOTO_SITE2		3	// ���ֵ�����
#define ID_POPUP_BUTTON_CLICK	10	// ��ť��ID��ʼֵ

#endif // !defined(__PLAT_H_)
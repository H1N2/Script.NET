// ChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "OwmGrep.h"
#include "ChildFrame.h"
#include "OwmGrepView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};
/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}


BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	get_dll_resource();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		reset_dll_resource();
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// ����ͼ��(��Ҫ�л���Դ)
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_OWM);
	SetIcon(hIcon, TRUE);
	reset_dll_resource();

	// ˢ���Զ���ͼ����Դ
	theApp.m_ol.CommandPlat(OC_SETOWMICONS, (WPARAM)(LPCTSTR)"org.owm.grep", 0);

	return 0;
}

void CChildFrame::OnClose() 
{
	// ֪ͨƽִ̨�ж�̬�رղ���
	// ע:ֻ������̬���ص�ģ��ƽ̨�Ż���Ӧ����Ϣ
	if(theApp.m_ol.nOWMType & OWM_DYNLOAD)
	{
		theApp.m_ol.CommandPlat(OC_DYNCLOSE, theApp.m_ol.nOWMID, 0);
	}

	CMDIChildWnd::OnClose();
}


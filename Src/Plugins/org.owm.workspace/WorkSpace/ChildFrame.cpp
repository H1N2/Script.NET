// ChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "WorkSpace.h"
#include "ChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
/*
	// ����ͼ��(��Ҫ�л���Դ)
	get_dll_resource();
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_OWM);
	SetIcon(hIcon, TRUE);
	reset_dll_resource();
*/	
	return 0;
}

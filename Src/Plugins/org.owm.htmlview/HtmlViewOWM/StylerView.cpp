// StylerView.cpp : implementation of the CStylerView class
//

#include "stdafx.h"
#include "HtmlViewOWM.h"

#include "StylerDoc.h"
#include "StylerView.h"

//#include "PopupFilter.h"
//#include "ComboBoxSearch.h"

//#include "MouseManager.h"

//#include "FilterDialog.h"
#include "ChildFrame.h"
#include <Mshtml.h>

//#include "LinkProperties.h"
#include <intshcut.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_LINK_STARTDRAG 1002

#define ID_VIEW_ENCODING_AUTOSELECT	0x2000
#define ID_VIEW_ENCODING_FIRST		0x2001
#define ID_VIEW_ENCODING_LAST		0x2100

#define ID_GOTO_URL					26047

BOOL CStylerView::m_bShowImages = TRUE;
long CStylerView::m_nFontSize = 2;

BOOL CStylerView::m_bFilterAll = FALSE;
BOOL CStylerView::m_bFilterList = TRUE;

//CImageList CStylerView::m_lstTabImages;
HICON CStylerView::m_arrIcons[5];


// ȫ�ֱ���,��¼��ǰ������ָ��
static IInterp* g_pInterp = NULL;

IInterp* GetCurrentInterp()
{
	// �����ǰ�д򿪵Ľ�����,�ͷ���ָ��
	if(g_pInterp)
	{
		return g_pInterp;
	}
	// ����ʹ���һ��������
	g_pInterp = (IInterp*)(theApp.m_pIPlatUI->CreateVciObject("org.interp.tcl", "htmlview"));

	return g_pInterp;
}

void SetCurrentInterp(IInterp* pInterp)
{
	g_pInterp = pInterp;
}

IMPLEMENT_DYNCREATE(CStylerView, CHtmlView)

BEGIN_MESSAGE_MAP(CStylerView, CHtmlView)
//{{AFX_MSG_MAP(CChildFrame)	
	ON_WM_SIZE()
	ON_WM_PARENTNOTIFY()
	
	ON_COMMAND(ID_ADD_TO_FAV, OnAddToFavorites)

	ON_XTP_EXECUTE(ID_FAVORITE_LINK, OnFavorite)

	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_FILE_PRINT, CHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)

	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectall)

	ON_COMMAND(ID_GO_BACK, CHtmlView::GoBack)
	ON_COMMAND(ID_GO_FORWARD, CHtmlView::GoForward)
	ON_COMMAND(ID_VIEW_STARTPAGE, CHtmlView::GoHome)
	ON_COMMAND(ID_VIEW_REFRESH, CHtmlView::Refresh)
	ON_COMMAND(ID_VIEW_STOP, CHtmlView::Stop)

	ON_COMMAND(ID_VIEW_SOURCE, OnViewSource)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)

	ON_UPDATE_COMMAND_UI(ID_GO_BACK, OnUpdateGotoBack)
	ON_UPDATE_COMMAND_UI(ID_GO_FORWARD, OnUpdateGotoForward)

	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)

	ON_COMMAND_RANGE(ID_TEXTSIZE_LARGEST, ID_TEXTSIZE_SMALLEST, OnTextSizeRange)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TEXTSIZE_LARGEST, ID_TEXTSIZE_SMALLEST, OnUpdateTextSizeRange)

	ON_COMMAND_RANGE(ID_VIEW_ENCODING_FIRST, ID_VIEW_ENCODING_LAST, OnViewEncodingRange)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_ENCODING_FIRST, ID_VIEW_ENCODING_LAST, OnUpdateViewEncodingRange)

	ON_MESSAGE(WM_XTP_GETTABICON, OnGetTabIcon)

	ON_NOTIFY(ID_LINK_STARTDRAG, ID_GOTO_URL, OnLinkStartDrag)

	ON_COMMAND(ID_GO_START_PAGE, OnGoStartPage)
	ON_UPDATE_COMMAND_UI(ID_GO_START_PAGE, OnUpdateGoStartPage)
	ON_COMMAND(ID_GO_SEARCH_THE_WEB, OnGoSearchTheWeb)
	ON_UPDATE_COMMAND_UI(ID_GO_SEARCH_THE_WEB, OnUpdateGoSearchTheWeb)
	ON_COMMAND(ID_VIEW_FOLDERS, OnViewFolders)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOLDERS, OnUpdateViewFolders)
	ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateViewHistory)

//}}AFX_MSG_MAP
	//ON_XTP_EXECUTE(ID_ADDPAGEHERE, OnAddPageHere)
	//ON_UPDATE_COMMAND_UI(ID_ADDPAGEHERE, OnUpdateAddPageHere)

	//ON_COMMAND(ID_TEXTSIZE, OnTextSize)

	ON_UPDATE_COMMAND_UI(1, OnUpdateIndicator)

	//ON_COMMAND(ID_SHOWIMAGES, OnShowImages)
	//ON_UPDATE_COMMAND_UI(ID_SHOWIMAGES, OnUpdateShowImages)

END_MESSAGE_MAP()



CStylerView::CStylerView()
{
	EnableAutomation();

	if (m_arrIcons[0] == 0)
	{
		CImageList il;
		il.Create(16, 16, ILC_COLORDDB | ILC_MASK, 5, 1);
		CBitmap bmp;
		get_dll_resource();
		bmp.LoadBitmap(IDB_BITMAP_TABS);
		reset_dll_resource();
		il.Add(&bmp, RGB(255,0,255));
		for (int i = 0; i < 5; i++)
			m_arrIcons[i] = il.ExtractIcon(i);
	}

	m_nLoadingState = -1;
	m_nProgress = 0;
	m_bPopup = FALSE;
	m_bLock = FALSE;

	m_lpDisp = NULL;
	m_bActive = FALSE;
	
	m_infMouse.Clear();

	m_strURL = "";
}

CStylerView::~CStylerView()
{
}

// CStylerView diagnostics

#ifdef _DEBUG
void CStylerView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CStylerView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CStylerDoc* CStylerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStylerDoc)));
	return (CStylerDoc*)m_pDocument;
}
#endif //_DEBUG


// CStylerView Commons

void CStylerView::Load()
{
	m_nFontSize = AfxGetApp()->GetProfileInt(_T("Explorer"), _T("FontSize"), 2);
	m_bShowImages = AfxGetApp()->GetProfileInt(_T("Explorer"), _T("ShowImages"), 1);
	
	m_bFilterAll = AfxGetApp()->GetProfileInt(_T("Explorer"), _T("FilterAll"), 0);
	m_bFilterList = AfxGetApp()->GetProfileInt(_T("Explorer"), _T("FilterList"), 1);
}

void CStylerView::Save()
{
	AfxGetApp()->WriteProfileInt(_T("Explorer"), _T("FontSize"), m_nFontSize);
	AfxGetApp()->WriteProfileInt(_T("Explorer"), _T("ShowImages"), m_bShowImages);
	
	AfxGetApp()->WriteProfileInt(_T("Explorer"), _T("FilterAll"), m_bFilterAll);
	AfxGetApp()->WriteProfileInt(_T("Explorer"), _T("FilterList"), m_bFilterList);

}

CComPtr<IHTMLDocument2> CStylerView::GetHtmlDocument2()
{
	LPDISPATCH lpDisp = GetHtmlDocument();
	CComQIPtr<IHTMLDocument2> spDoc = lpDisp;
	if (lpDisp)	lpDisp->Release();
	
	return spDoc;
}

void CStylerView::SetAddress()
{
	if (m_nLoadingState == -1)
		return;

	try
	{
		CString strUrl = GetLocationURL();
		if (GetDocument() &&  !strUrl.IsEmpty() && strUrl != GetDocument()->GetCurrentUrl())
		{
			GetDocument()->SetUrl(strUrl);	
			if (m_bActive)		
			{
				//GetMainFrame()->SetAddress(strUrl);
			}
		}
	}
	catch (...)
	{
	}
}

void CStylerView::RefreshExplorer()
{
	/*
	CSourceCtrl* pBar = GetMainFrame()->GetSourceCtrl();
	if (pBar && pBar->m_hWnd && pBar->IsWindowVisible())
	{
		pBar->Refresh(this, FALSE);		
	}
	*/
}

EXTERN_C const GUID CGID_MSHTML;
#define CMDSETID_Forms3 CGID_MSHTML

void CStylerView::ExecFormsCommand(DWORD dwCommandID)
{
	CComQIPtr<IOleCommandTarget> spCmdTarget(m_pBrowserApp);
	if (spCmdTarget)
	{
		spCmdTarget->Exec(&CMDSETID_Forms3, dwCommandID, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0);
	}
}

BOOL CStylerView::QueryFormsCommand(DWORD dwCommandID)
{
	try
	{
		CComQIPtr<IOleCommandTarget> spCmdTarget(m_pBrowserApp);
		
		if (spCmdTarget)
		{
			OLECMD cmdInfo = {dwCommandID, 0};		
			if (SUCCEEDED(spCmdTarget->QueryStatus(&CMDSETID_Forms3, 1, &cmdInfo, NULL)))
				return cmdInfo.cmdf & OLECMDF_ENABLED;
		}
	}
	catch(...)
	{
	}

	return FALSE;
}

void CStylerView::SetLoading(BOOL bLoading)
{
	int nState = bLoading ? -1 : 3;

	if (m_nLoadingState != nState)
	{
		m_nLoadingState = nState;
		//GetMainFrame()->m_MTIClientWnd.Invalidate(FALSE);
	}
}


BOOL CStylerView::PopupFilter(CString str)
{	
/*
	for (int i = 0; i < CPopupFilter::m_lstFilter.GetSize(); i++)
	{		
		if (str.Find(CPopupFilter::m_lstFilter[i]) != -1)
		{
			return TRUE;
		}
	}*/
	return FALSE;
}

// CStylerView virtuals

BOOL CStylerView::Create(LPCTSTR /*lpszClassName*/, LPCTSTR lpszWindowName,
						DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
						UINT nID, CCreateContext* pContext)
{
	static CString strClassName;
	if (strClassName.IsEmpty())
	{
		get_dll_resource();
		strClassName = AfxRegisterWndClass(
			CS_DBLCLKS,                       // if you need double-clicks
			NULL,                             // no cursor (use default)
			NULL,                             // no background brush
			AfxGetApp()->LoadIcon(IDR_OWM)); // app icon
		ASSERT(strClassName);
		reset_dll_resource();
	}

	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(strClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	if (m_pBrowserApp)
	{
		CComPtr<IOleObject> spOleObj;
		
		m_pBrowserApp->QueryInterface(IID_IOleObject, (void**)&spOleObj);
		if (spOleObj)
			spOleObj->SetClientSite((IOleClientSite*)GetInterface(&IID_IOleClientSite));
	}

	return TRUE;
}

BOOL CStylerView::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg == NULL || m_hWnd == NULL)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		m_infMouse.nLClick = GetTickCount();
		m_infMouse.nRClick = 0;
	}

	if (CView::PreTranslateMessage(pMsg))
		return TRUE;

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

	if (pMsg->message == WM_KEYDOWN && m_pBrowserApp)
	{		
		CComQIPtr<IOleInPlaceActiveObject> spActiveObj(m_pBrowserApp);
		if (spActiveObj && spActiveObj->TranslateAccelerator(pMsg) == S_OK)
			return TRUE;
	}

	return FALSE;
}

BOOL CStylerView::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style &= ~WS_BORDER;

	return CHtmlView::PreCreateWindow(cs);
}

void CStylerView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	COleVariant vaZoomFactor(m_nFontSize);
	ExecWB(OLECMDID_ZOOM , OLECMDEXECOPT_DONTPROMPTUSER,  &vaZoomFactor, NULL);

	//CString strStartScript = theApp.m_strDefaultPage;//"local:OWM://\\html\\start.htm";
	//Navigate2(strStartScript);
	if (GetDocument()->GetCurrentUrl().IsEmpty())
	{	
		CString strStartScript = theApp.m_strDefaultPage;
		Navigate(strStartScript);
	}
}

void CStylerView::PostNcDestroy() 
{
	CHtmlView::PostNcDestroy();
}

void CStylerView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
{
	if (bActivate && !m_bActive)
	{
		//GetMainFrame()->SetAddress(GetDocument()->GetCurrentUrl());
		RefreshExplorer();		

	}
	m_bActive = bActivate;

	if (bActivate)
	{
		// �����Դ�����ʾ�ļ���Ϣ
		GetDocument()->RefreshDocProperty();
	}

	/* // HtmlView���ȡ�����˵��е�����
	if (bActivate)
	{
		get_dll_resource();
		CMenu* pMenu = new CMenu();
		pMenu->LoadMenu(IDR_OWM_VIEW);
		reset_dll_resource();
		theApp.m_pIPlatUI->AddExtMenu(pMenu);
	}else
	{
		get_dll_resource();
		CMenu* pMenu = new CMenu();
		pMenu->LoadMenu(IDR_OWM_VIEW);
		reset_dll_resource();
		theApp.m_pIPlatUI->DelExtMenu(pMenu);
	}*/
	
	CHtmlView:: OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CStylerView::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel)
{
	if(ppDisp == NULL || Cancel == NULL)
		return;

	if(m_bFilterAll)
	{
	
		double secs = (double)(GetTickCount() - m_infMouse.nLClick)/(double)1000;
		double Rsecs = (double)(GetTickCount() - m_infMouse.nRClick)/(double)1000;

		BSTR bstrTag = NULL;

		if((secs > 0.7 && secs < 4.0) && !IsLoading())
		{
			//get click point
			IHTMLElement* pElem = NULL;
			try
			{
				CComPtr<IHTMLDocument2> spDoc = GetHtmlDocument2();
				if(spDoc)
				{
					if(SUCCEEDED(spDoc->elementFromPoint(m_infMouse.ptClick.x, m_infMouse.ptClick.y, &pElem)))
					{
						if(pElem)
						{
							pElem->get_tagName(&bstrTag);
							pElem->Release();
						}
					}
				}
				
			}catch(...)
			{
			}
		}
		
		if( !(m_infMouse.nLClick && ((secs >= 0 && secs <= 0.7) || (bstrTag && wcscmp(bstrTag, L"A") == 0) /*|| !IsLoading()*/)) 
			&& !((Rsecs >= 0 && Rsecs <= 10 && m_infMouse.nRClick) /*|| !IsLoading()*/))
		{
			*ppDisp = NULL;
			*Cancel = TRUE;

			TRACE(_T("Filter Popup\n"));
			
			m_infMouse.Clear();
				
			if(bstrTag)	SysFreeString(bstrTag);
				
			return;
		}
			
		if(bstrTag)	SysFreeString(bstrTag);				
	}

	// Get the correct document template.
	CDocTemplate* pDocTemplate = theApp.m_pDocTemplate;
	
	CStylerDoc* pDoc = (CStylerDoc*)pDocTemplate->CreateNewDocument();
	pDoc->SetUrl(GetDocument()->GetCurrentUrl());
	
	// Create a new frame.
	CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDoc, (CFrameWnd*)AfxGetMainWnd() );
	
	// Activate the frame.
	
	//if (m_bLock) ((CChildFrame*)pFrame)->m_bActivate = FALSE;	
	pDocTemplate->InitialUpdateFrame( pFrame, NULL );
	CStylerView* pView = (CStylerView*)pFrame->GetActiveView();
	
	// Pass pointer of WebBrowser object.
	pView->SetRegisterAsBrowser( TRUE );   
	*ppDisp = pView->GetApplication();   
	
	pView->m_bPopup = TRUE;
}

void CStylerView::OnTitleChange(LPCTSTR lpszText)
{
	if (m_pDocument != NULL)
	{
		try{
		m_pDocument->SetTitle(lpszText);
		}catch(...)
		{
		}
	}

	SetAddress();
}

void CStylerView::NavigateComplete2(LPDISPATCH pDisp, VARIANT* /*URL*/)
{
	if (!m_lpDisp) m_lpDisp = pDisp;	
}

void CStylerView::DocumentComplete(LPDISPATCH /*pDisp*/, VARIANT* /*URL*/)
{

	if (m_lpDisp && m_lpDisp == m_lpDisp)
	{
		m_lpDisp = NULL;
		TRACE(_T("Document is done downloading\n"));
		try
		{
			SetLoading(FALSE);
		
			RefreshExplorer();		

			CComPtr<IHTMLDocument2> spDoc = GetHtmlDocument2();

			SetAddress();		
			
			if( spDoc )
			{	
				
				BSTR bstr = NULL;		
				spDoc->get_charset( &bstr );
				CString strCharset(bstr);
				SysFreeString(bstr);
				/*
				for (int i = 0; i < CMainFrame::m_arrEncodingPage.GetSize(); i++)
				{
					if (CMainFrame::m_arrEncodingPage[i] == strCharset)
					{
						m_nEncoding = i + ID_VIEW_ENCODING_FIRST;
						
						break;
					}
				}*/
			}
			m_bPopup = FALSE;
		}
		catch(...)
		{
		}	
		
		m_infMouse.Clear();
	}
	
}

void CStylerView::OnProgressChange(long nProgress, long nProgressMax)
{
	if (nProgressMax && nProgress >= 0)
	{
		long nIndex = nProgress * 4 / nProgressMax;
		if (nIndex > 3) nIndex = 3;

		m_nProgress = nProgress * 100 / nProgressMax;

		if (m_nLoadingState != nIndex)
		{
			m_nLoadingState = nIndex;
			//GetMainFrame()->m_MTIClientWnd.Invalidate(FALSE);
		}			
	} else m_nProgress = 0;
}

/////////////////////////////////////////////////////////////////////////////
// ���沢���õ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CStylerView::SaveAndSetCurPath(CString strURL)
{
	// ��¼��ǰ·��,�����õ�ǰ·��
	memset(m_szOldPath, 0, 256);
	m_dwPathLen = GetCurrentDirectory(255, m_szOldPath);
	CString strPath = strURL;
	strPath.Replace("/", "\\");
	int nPos = strPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		strPath = strPath.Left(nPos);
	}
	SetCurrentDirectory(strPath);
}

/////////////////////////////////////////////////////////////////////////////
// �ָ�����ĵ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CStylerView::RestoreCurPath()
{
	// ִ�н���,�ָ���ǰ·��������
	if(m_dwPathLen > 0)
	{
		CString strOldPath = m_szOldPath;
		SetCurrentDirectory(strOldPath);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����%�滻,��%XX�滻��ASCII�ַ�
/////////////////////////////////////////////////////////////////////////////
void ReplaceHtmlStrToAscii(CString& strFormParam)
{
	// ����%�滻
	int nPos = -1;
	int nStartPos = 0;
	int nLen = strFormParam.GetLength();
	while(((nPos=strFormParam.Find('%', nStartPos)) != -1) && (nPos<(nLen-2)))
	{
		if(strFormParam[nPos+1] == '%')
		{
			nStartPos = nPos+2;
		}else
		if(((strFormParam.GetAt(nPos+1)>='0' && strFormParam.GetAt(nPos+1)<='9') ||
			(strFormParam.GetAt(nPos+1)>='A' && strFormParam.GetAt(nPos+1)<='F')) &&
			((strFormParam.GetAt(nPos+2)>='0' && strFormParam.GetAt(nPos+2)<='9') ||
			(strFormParam.GetAt(nPos+2)>='A' && strFormParam.GetAt(nPos+2)<='F')) )
		{
			CString ch1 = strFormParam.GetAt(nPos+1);
			CString ch2 = strFormParam.GetAt(nPos+2);
			int n1,n2;
			if(strFormParam.GetAt(nPos+1) >= 'A')
				n1 = strFormParam.GetAt(nPos+1)-'A'+10;
			else
				n1 = atoi(ch1);
			if(strFormParam.GetAt(nPos+2) >= 'A')
				n2 = strFormParam.GetAt(nPos+2)-'A'+10;
			else
				n2 = atoi(ch2);
			int n = n1*16+n2;
			char ch = char(n);
			strFormParam.Delete(nPos, 3);
			//if(ch == '\\')
			//	strFormParam.Insert(nPos, "\\\\");
			//else
				strFormParam.Insert(nPos, ch);
			nStartPos = nPos+1;
			nLen = strFormParam.GetLength();
		}else
		{
			nStartPos = nPos+2;
		}
	}
}

void CStylerView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	if (m_bFilterList && m_bPopup && PopupFilter(lpszURL))
	{
		if (pbCancel) *pbCancel = TRUE;
		GetParentFrame()->PostMessage(WM_CLOSE);
		return;
	}

	if (m_bLock && !baPostedData.GetSize() && GetTickCount() - m_infMouse.nLClick < 600)
	{
		if(_tcsstr(lpszURL, _T("mailto:")) == NULL && _tcsstr(lpszURL, _T("javascript:")) == NULL)
		{
			if (pbCancel)
			{
				//OpenWindow(lpszURL, FALSE);

				 *pbCancel = TRUE;
			}
		}
	}

	CString strURL = lpszURL;

	if(strURL != "about:blank")
	{
		m_strURL = strURL;
	}

	// ��ȡ���а�װ�Ľ�������Ϣ
	CStringArray asVciId;
	CStringArray asInterpName;
	CStringArray asInterpId;
	theApp.m_pIPlatUI->GetInstalledVci(asVciId);
	for(int i=0; i<asVciId.GetSize(); i++)
	{
		if(theApp.m_pIPlatUI->GetVciType(asVciId[i]) == _T("interp"))
		{
			asInterpName.Add(theApp.m_pIPlatUI->GetVciName(asVciId[i]));
			asInterpId.Add(asVciId[i]);
		}
	}

	// ��һЩJave��Ƕ��Ӧ����,����ʹ��:/���ַ�,�����Ҫ������ؼ���,
	// �˹ؼ��ֺ���������ٽ�������Ľ���

	if( strURL.Find("local:") == 0 )
	{
		// �����ļ�
		strURL.Delete(0, 6);
		// ������лὫ�ո�任Ϊ%20,�˴����ȥ
		strURL.Replace("%20", " ");
		// ��������޷�����",��˽����Ÿ�Ϊ~
		strURL.Replace('~', '"');

		theApp.ProcessPath(strURL);

		Navigate2(strURL);

		*pbCancel = TRUE;

	}else
	// ����Э��
	if(strURL.Find("pfe:") == 0)		// Platform Event
	{
		// ������лὫ�ո�任Ϊ%20,�˴����ȥ
		strURL.Replace("%20", " ");
		// ��������޷�����",��˽����Ÿ�Ϊ~
		strURL.Replace('~', '"');

		strURL.Delete(0, 4);
		if(strURL.Find("open") == 0)	// ���ļ�
		{
			strURL.Delete(0, 4);
			strURL.TrimLeft();

			theApp.ProcessPath(strURL);

			theApp.m_pIPlatUI->OpenDocumentFile(strURL, "");
		}else
		if(strURL.Find("openwithowm<") == 0)	// ��ָ����OWM���ļ�
		{
			strURL.Delete(0, 12);
			int nPos = strURL.Find(">");
			if(nPos != -1)
			{
				CString strOwmId = strURL.Left(nPos);
				strURL.Delete(0, nPos+1);
				strURL.TrimLeft();

				theApp.ProcessPath(strURL);

				theApp.m_pIPlatUI->OpenDocumentFile(strURL, strOwmId);
			}
		}

		*pbCancel = TRUE;

	}else
	{
		// Interp command

		// ������лὫ�ո�任Ϊ%20,�˴����ȥ
		strURL.Replace("%20", " ");
		// ��������޷�����",��˽����Ÿ�Ϊ~
		strURL.Replace('~', '"');

		CString strInterpId = "";
		CString strInterpName = "";
		for(int i=0; i<asInterpName.GetSize(); i++)
		{
			CString strInterpPre = asInterpName[i] + ":";
			if(strURL.Find(strInterpPre) == 0)
			{
				strInterpId = asInterpId[i];
				strInterpName = asInterpName[i];
				break;
			}
		}

		if(strInterpId != "")
		{
			strURL.Delete(0, strInterpName.GetLength()+1);

			CString strInterp = _T("");
			// ������������(֧��������ʽ�Ľ�����)
			// �����ǰ����<<>>��Χ���ַ���,��ʾ��������ʽ��
			if(strURL.Find("<<") == 0)
			{
				int nPos = strURL.Find(">>");
				if(nPos != -1)
				{
					strURL.Delete(0, 2);
					strInterp = strURL.Left(nPos-2);
					strURL.Delete(0, nPos);
				}
			}

			// ����������
			IInterp* pInterp = (IInterp*)(theApp.m_pIPlatUI->CreateVciObject(strInterpId, strInterp));
			if(pInterp == NULL)
			{
				*pbCancel = TRUE;
				return;
			}

			if((g_pInterp != pInterp) && (g_pInterp != NULL))
			{
				// ����������Ѿ��ı�,��Ҫ���ͷ���ǰ�Ľ�����
				// ��Ҫ�Ȳ�һ����ǰ�Ľ������Ƿ�פ���Ľ�����,��פ���Ĳ��ͷ�
				CString strTSRInterp = g_pInterp->GetInterpParam("TSRInterp");
				if(strTSRInterp != "true")
				{
					theApp.m_pIPlatUI->ReleaseVciObject(g_pInterp, TRUE);
				}
			}
			g_pInterp = pInterp;

			// ��ִ�нű�֮ǰ�����ñ�ʾ��ǰҳ���ļ���TCL����
			CStylerDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			CString strHtmlPath = pDoc->GetCurrentUrl();
			strHtmlPath.Replace('\\', '/');
			pInterp->SetVar("_htmlCurrentFile", strHtmlPath);

			// ����HTML��FORM�ύ�����Ĳ���(GET��POST��ʽ)
			CString strFormParam = "";
			if (baPostedData.GetSize())
			{
				// �����POST��ʽ,��POST�����ݼӵ���������
				strURL += "?";
				// This is a kludgy copy, but you get the idea
				for(int i = 0;i < baPostedData.GetSize();i++)
				{
					if(baPostedData[i] != 0)
					{
						strURL += (char)baPostedData[i];
					}
				}
			}
			if(strURL.Find("<form>") == 0)	// ��ǰ����<form>��ʾ��һ��FORM�ύ�����ĵ�URL
			{	// ע:����ֻ�����ڿ��Դ�������<file>��ʽTCL����
				strURL.Delete(0, 6);
				int nPosForm = strURL.Find("?");
				if(nPosForm != -1)
				{
					// ���������ݵĲ���
					strFormParam = strURL.Right(strURL.GetLength()-nPosForm-1);
					strURL = strURL.Left(nPosForm);
					if(strFormParam.GetLength() > 0)
					{	// �������Ϊ�վͲ���������
						// ����%�滻
						ReplaceHtmlStrToAscii(strFormParam);
						// ��������""��Χ������Ϊһ���������ݸ�TCL
						strFormParam.Replace("\"", "\\\"");
						// ��Ϊ�������ڲ��Ѿ���Ϊ��?����,��˴˴���������ʺ�
						strURL += "?\"";
						strURL += strFormParam;
						strURL += "\"";
					}
				}
			}

			BOOL bRet = TRUE;
			if(strURL.Find("<file>") == 0)	// ��ȷָ����ִ���ļ�,���Դ������в���
			{
				strURL.Delete(0, 6);
				theApp.ProcessPath(strURL);	// ����·��

				//TRACE("[%s]\n", strURL);
				// ���沢���õ�ǰ·��
				SaveAndSetCurPath(strURL);

				bRet = pInterp->RunScriptFile(strURL);	// ִ�нű��ļ�

				// �ָ���ǰ·��
				RestoreCurPath();

			}else
			{
				bRet = pInterp->RunScriptCommand(strURL);	// ִ�нű�����
			}

			if(!bRet)	// �ű�ִ�д���
			{
				::MessageBox(AfxGetMainWnd()->m_hWnd, pInterp->GetResult(), "Error", MB_OK | MB_ICONERROR);
			}else
			{
				// �����HTMLҳ��URL��ֵ����
				CString strHtmlNewURL = pInterp->GetVar(_T("_htmlNewURL"));
				if(strHtmlNewURL != "")
				{	// ��ʾҳ��URL�����¸�ֵ
					try {
					Navigate2(strHtmlNewURL);
					}catch(...){
					}
				}else
				{
					// ���HTMLˢ�±�Ǳ���
					CString strHtmlRefreshSign = pInterp->GetVar(_T("_htmlRefreshSign"));
					if(strHtmlRefreshSign == "true")
					{	// ��ʾ��ҳ��仯,��Ҫˢ��
						try {
						Refresh();
						}catch(...){
						}
					}
				}
			}

			// �ж��Ƿ�פ��������(�����ڽű�������פ����������־TSRInterp)
			CString strTSRInterp = pInterp->GetInterpParam("TSRInterp");
			if(strTSRInterp == "false")
			{
				// ����Ҫפ����,��ǿ���ͷŽ�����
				theApp.m_pIPlatUI->ReleaseVciObject(pInterp, TRUE);
				g_pInterp = NULL;
			}else
			if(strTSRInterp != "true")
			{
				// ��פ��������,���ͷŽ�����
				theApp.m_pIPlatUI->ReleaseVciObject(pInterp, FALSE);
				g_pInterp = NULL;
			}

			*pbCancel = TRUE;
		}
		
	}

	if(*pbCancel == TRUE)
	{
		return;	// ������ȡ��
	}
	
	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}

void CStylerView::OnCommandStateChange(long nCommand, BOOL bEnable)
{
	if (nCommand == CSC_NAVIGATEFORWARD) m_bForward = bEnable;
	if (nCommand == CSC_NAVIGATEBACK) m_bBack = bEnable;
}

// CStylerView message handlers

void CStylerView::OnSize(UINT nType, int cx, int cy)
{
	
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndBrowser.m_hWnd))
	{
		HDWP wp = BeginDeferWindowPos(1);
		wp = DeferWindowPos(wp, m_wndBrowser.m_hWnd, 0, 0, 0, cx, cy, 
			SWP_NOZORDER | SWP_NOREPOSITION);	
		EndDeferWindowPos(wp);
		//m_wndBrowser.MoveWindow(0, 0, cx, cy);
	}	
}

void CStylerView::OnParentNotify(UINT message, LPARAM lParam) 
{
	if(message == WM_LBUTTONDOWN )
	{	
		m_infMouse.nLClick = GetTickCount();
		m_infMouse.nRClick = 0;
		m_infMouse.ptClick = CPoint(LOWORD((DWORD)lParam), HIWORD((DWORD)lParam));
	}
	else if(message == WM_RBUTTONDOWN ) 
	{
		m_infMouse.nRClick = GetTickCount();
		m_infMouse.nLClick = 0;
	
	} else if(message == WM_DESTROY)
	{		
		GetParentFrame()->PostMessage(WM_CLOSE);
	}
	
	CHtmlView::OnParentNotify(message, lParam);
}

LRESULT CStylerView::OnGetTabIcon(WPARAM /*lParam*/, LPARAM /*wParam*/)
{
	int nIndex = m_bLock ? 4 : m_nLoadingState < 0 ? 3 : m_nLoadingState;

	if (nIndex < 5)
		return (LRESULT)m_arrIcons[nIndex];

	return 0;
}

void CStylerView::OnUpdateGotoBack(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bBack);	
}

void CStylerView::OnUpdateGotoForward(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bForward);	
}


void CStylerView::OnFileProperties()
{
	ExecWB(OLECMDID_PROPERTIES, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

void CStylerView::OnEditSelectall()
{
	ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

void CStylerView::OnFilePrintPreview()
{
	ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

void CStylerView::OnFileSaveAs()
{
	ExecWB(OLECMDID_SAVEAS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

void CStylerView::OnEditFind()
{
	CComQIPtr<IOleCommandTarget> spCmdTarget = m_pBrowserApp;
	spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_FIND, 0, NULL, NULL);
 }

void CStylerView::OnViewSource()
{
	CComQIPtr<IOleCommandTarget> spCmdTarget = m_pBrowserApp;
	spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_VIEWSOURCE, 0, NULL, NULL);
}


void CStylerView::OnToolsOptions()
{
	CComQIPtr<IOleCommandTarget> spCmdTarget = m_pBrowserApp;
	spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_OPTIONS, 0, NULL, NULL);
}

void CStylerView::OnAddToFavorites()
{
	IShellUIHelper* pShell = NULL;
	HRESULT hr = CoCreateInstance(CLSID_ShellUIHelper, NULL, CLSCTX_SERVER,
		IID_IShellUIHelper, (void**)&pShell);
	if (hr == S_OK)
	{
		COleVariant vtTitle(GetDocument()->GetTitle());
		BSTR bstrUrl = GetDocument()->GetCurrentUrl().AllocSysString();
		
		pShell->AddFavorite(bstrUrl, &vtTitle);
		
		SysFreeString(bstrUrl);
		pShell->Release();
	}
}

void CStylerView::OnEditCopy() 
{
	ExecFormsCommand(IDM_COPY);
}

void CStylerView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(QueryFormsCommand(IDM_COPY));
}

void CStylerView::OnEditCut() 
{
	ExecFormsCommand(IDM_CUT);
}

void CStylerView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(QueryFormsCommand(IDM_CUT));
}

void CStylerView::OnEditPaste() 
{
	ExecFormsCommand(IDM_PASTE);
}

void CStylerView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(QueryFormsCommand(IDM_PASTE));
}

void CStylerView::OnTextSizeRange(UINT nID)
{
	m_nFontSize = ID_TEXTSIZE_SMALLEST - nID;
	ASSERT(m_nFontSize >=0 && m_nFontSize < 5);
	
	if (m_pBrowserApp)
	{	
		COleVariant vaZoomFactor(m_nFontSize);
		ExecWB(OLECMDID_ZOOM , OLECMDEXECOPT_DONTPROMPTUSER,  &vaZoomFactor, NULL);	
	}
}

void CStylerView::OnUpdateTextSizeRange(CCmdUI* pCmdUI) 
{	
	pCmdUI->SetCheck(long(ID_TEXTSIZE_SMALLEST - pCmdUI->m_nID) == m_nFontSize);
}

void CStylerView::OnUpdateViewEncodingRange(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nEncoding == pCmdUI->m_nID);
}
void CStylerView::OnViewEncodingRange(UINT nID)
{
	CComPtr<IHTMLDocument2> spDoc = GetHtmlDocument2();

	if( spDoc )
	{/*
		int nIndex = nID - ID_VIEW_ENCODING_FIRST;
		ASSERT(nIndex < CMainFrame::m_arrEncodingPage.GetSize());

		BSTR bstr;
		bstr = CMainFrame::m_arrEncodingPage[nIndex].AllocSysString();

		spDoc->put_charset( bstr );
		SysFreeString(bstr);
		Refresh2(1);

		m_nEncoding = nID;*/
	}

}

void CStylerView::OnLinkStartDrag(NMHDR* /*pNMHDR*/, LRESULT* /*pRes*/)
{
	USES_CONVERSION;

	CString strTitle = GetDocument()->GetTitle();
	CString strAddress = GetDocument()->GetCurrentUrl();
	
	strTitle.Replace('\\', '-');
	strTitle.Replace('/', '-');
	strTitle.Replace(':', ' ');
	strTitle.Replace('|', ' ');
	
	CString strDirectory;
	GetTempPath (MAX_PATH, strDirectory.GetBuffer(MAX_PATH));
	strDirectory.ReleaseBuffer();
	
	CString strPath = strDirectory + strTitle + ".url";
	
	WritePrivateProfileString(_T("InternetShortcut"), _T("URL"), strAddress,  strPath);
	
	TCHAR chFilePath[_MAX_PATH];
	ZeroMemory(chFilePath, sizeof(chFilePath));

	STRCPY_S(chFilePath, _MAX_PATH, (LPCTSTR)strPath);

	COleDataSource ds;
	
	CSharedFile sharedFile;
	CArchive ar (&sharedFile, CArchive::store);

	DROPFILES df;
	df.pFiles = sizeof(DROPFILES);
	df.pt = CPoint(0, 0);
	df.fNC = 0;
#ifdef _UNICODE
	df.fWide = TRUE;
#else
	df.fWide = FALSE;
#endif

	ar.Write(&df, sizeof(DROPFILES));
	ar.Write(chFilePath, (strPath.GetLength() + 2) * sizeof(TCHAR));

	ar.Close();
	
	HGLOBAL hGlobal = sharedFile.Detach();

	ds.CacheGlobalData(CF_HDROP, hGlobal);
	ds.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_LINK);

	return;
}

void CStylerView::OnUpdateIndicator(CCmdUI *pCmdUI)
{
	//ASSERT_KINDOF(CXTPStatusBar, pCmdUI->m_pOther);
	//CIndicatorStatusBar* bar =(CIndicatorStatusBar*) pCmdUI->m_pOther;
	
	int nProgress = (m_nLoadingState != -1 && m_nLoadingState != 3) ? m_nProgress : 0;
	//bar->SetProgress(nProgress);
	if(nProgress != 0)
	{
		theApp.m_ol.CommandPlat(OC_ATTACHPANEPROGRESS, MAKELONG(0, 100), NULL);
		theApp.m_ol.CommandPlat(OC_SETPANEPROGRESSPOS, 0, nProgress);
	}else
	{
		theApp.m_ol.CommandPlat(OC_SHOWPANEPROGRESS, 0, FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼҳ
/////////////////////////////////////////////////////////////////////////////
void CStylerView::OnGoStartPage() 
{
	//GoStartPage();
	CString strStartScript = theApp.m_strDefaultPage;
	Navigate2(strStartScript);
}

void CStylerView::OnUpdateGoStartPage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CStylerView::OnGoSearchTheWeb() 
{
	// ����������
	IInterp* pInterp = (IInterp*)(theApp.m_pIPlatUI->CreateVciObject("org.interp.tcl", ""));
	if(pInterp == NULL)
	{
		return;
	}

	CString strScriptFile;
	strScriptFile.Format("\"%sLib\\Plat\\htmlview.tcl\" search", theApp.GetPlatRootPath());
	pInterp->RunScriptFile(strScriptFile);

	theApp.m_pIPlatUI->ReleaseVciObject(pInterp, FALSE);
}

void CStylerView::OnUpdateGoSearchTheWeb(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// �ļ���
/////////////////////////////////////////////////////////////////////////////
void CStylerView::OnViewFolders() 
{
	// ����������
	IInterp* pInterp = (IInterp*)(theApp.m_pIPlatUI->CreateVciObject("org.interp.tcl", ""));
	if(pInterp == NULL)
	{
		return;
	}

	CString strScriptFile;
	strScriptFile.Format("\"%sLib\\Plat\\htmlview.tcl\" folder", theApp.GetPlatRootPath());
	pInterp->RunScriptFile(strScriptFile);

	theApp.m_pIPlatUI->ReleaseVciObject(pInterp, FALSE);
}

void CStylerView::OnUpdateViewFolders(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// �ļ���
/////////////////////////////////////////////////////////////////////////////
void CStylerView::OnViewHistory() 
{
	// ����������
	IInterp* pInterp = (IInterp*)(theApp.m_pIPlatUI->CreateVciObject("org.interp.tcl", ""));
	if(pInterp == NULL)
	{
		return;
	}

	CString strScriptFile;
	strScriptFile.Format("\"%sLib\\Plat\\htmlview.tcl\" history", theApp.GetPlatRootPath());
	pInterp->RunScriptFile(strScriptFile);

	theApp.m_pIPlatUI->ReleaseVciObject(pInterp, FALSE);
}

void CStylerView::OnUpdateViewHistory(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CStylerView::OnFavorite(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	//OpenWindow(tagNMCONTROL->pControl->GetParameter(), TRUE, TRUE);
	Navigate2(tagNMCONTROL->pControl->GetParameter());
	*pResult = 1;
}
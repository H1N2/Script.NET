// HtmlViewOWMView.cpp : implementation of the CHtmlViewOWMView class
//

#include "stdafx.h"
#include "HtmlViewOWM.h"
#include "HtmlViewOWMDoc.h"
#include "HtmlViewOWMView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

// ����DISPID
#define DISPID_EX_HANDLER	1

// ��������
/////////////////////////////////////////////////////////////////////////////
// CMyUIHandler

class CExCommandHandler : public IDispatch
{
	long		refcount;
	_bstr_t		m_bstrParam;	// ������
public:

	CExCommandHandler() {refcount = 0;}
	//virtual ~CExCommandHandler() {}

	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (iid == IID_IDispatch)
		{
			*ppvObj = (IDispatch*)this;
			AddRef();
		}else
		{
			return E_NOINTERFACE;
			//return E_NOTIMPL;
		}
		return S_OK;
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		InterlockedIncrement(&refcount);
		return refcount;
	}

	STDMETHOD_(ULONG,Release)()
	{
		InterlockedDecrement(&refcount);
		if (refcount == 0)
			delete this;
		return refcount;
	}

	STDMETHOD(GetTypeInfoCount)(/* [out] */ UINT *pctinfo)
	{
		return S_OK;
	}

	STDMETHOD(GetTypeInfo)(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ ITypeInfo **ppTInfo)
	{
		return S_OK;
	}

	STDMETHOD(GetIDsOfNames)( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
	{
		m_bstrParam = *rgszNames;
		*rgDispId = DISPID_EX_HANDLER;
		return S_OK;
	}

	STDMETHOD(Invoke)( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
	{
		if(dispIdMember==DISPID_EX_HANDLER)
		{
			if(wFlags == DISPATCH_PROPERTYGET)	// ��ȡ����
			{
				IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					CString strValue = pInterp->GetVar(m_bstrParam);
					COleVariant* pRetVal = (COleVariant*)pVarResult;
					*pRetVal = strValue;
				}
			}else
			if(wFlags == DISPATCH_PROPERTYPUT)	// ��������
			{
				IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					if(pDispParams->cArgs > 0)
					{
						COleVariant* pRetVal = (COleVariant*)(pDispParams->rgvarg);
						CString str(pRetVal->bstrVal);
						pInterp->SetVar(m_bstrParam, str);
					}
				}
			}else
			if((wFlags & DISPATCH_METHOD) != 0)	// ���÷���
			{
				IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					CStringArray asParam;
					// ˵��:��������pDispParamsΪ����,������������ǰ��,�˴�ʡ����������
					int nArgs = pDispParams->cArgs + pDispParams->cNamedArgs;
					int nNamedArgs = pDispParams->cNamedArgs;
					for(int i = nArgs-1; i >= nNamedArgs; i--)
					{
						COleVariant* pRetVal = (COleVariant*)(&(pDispParams->rgvarg[i]));
						CString str(pRetVal->bstrVal);
						asParam.Add(str);
					}
					CString strRes = "";
					if(pInterp->CallMethod(m_bstrParam, asParam))
					{
						strRes = pInterp->GetResult();
					}else
					{
						strRes = pInterp->GetResult();
					}
					if((wFlags & DISPATCH_PROPERTYGET) != 0)
					{
						// ���Ի�ȡ����ֵ
						COleVariant* pRetVal = (COleVariant*)pVarResult;
						*pRetVal = strRes;
					}
				}
			}

              //MessageBox(0,"Hello World","Hello",0); //place your code here
			  //frmweb->Edit1->Text="Hello World(��Ҳ���ԣ�)";
		}

		return S_OK;
	}
	
};

/////////////////////////////////////////////////////////////////////////////
// CExUIHandler

class CExUIHandler : public IDocHostUIHandler
{
public:

	long refcount;

	BOOL m_vbEnableCtxMenus;
	CExUIHandler():m_vbEnableCtxMenus(FALSE)	// FALSE ��ʾ����ʾ����ʽ�˵�
	{
		refcount = 0;
	}
	virtual ~CExUIHandler()
	{
	}

	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (iid == IID_IUnknown)
		{
			*ppvObj = (IUnknown*)this;
			AddRef();
		}else
		if (iid == IID_IDocHostUIHandler)
		{
			*ppvObj = (IDocHostUIHandler*)this;
			AddRef();
		}else
		{
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		InterlockedIncrement(&refcount);
		return refcount;
	}

	STDMETHOD_(ULONG,Release)()
	{
		InterlockedDecrement(&refcount);
		if (refcount == 0)
			delete this;
		return refcount;
	}

	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdtReserved,
                              IDispatch FAR* pdispReserved)
	{
		return E_NOTIMPL;
		if (m_vbEnableCtxMenus == TRUE)  // Show Context Menu
			return S_FALSE;
		else
			return S_OK;
	}

	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR *pInfo)
	{
		// ע:�˴��������ΪE_NOTIMPL,��ҳ�治����Ӧ�κ����ͼ��̲���
		return S_OK;
		//return E_NOTIMPL;
	}

	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
                    IOleCommandTarget FAR* pCommandTarget,
                    IOleInPlaceFrame  FAR* pFrame,
                    IOleInPlaceUIWindow FAR* pDoc)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(HideUI)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(UpdateUI)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(EnableModeless)(BOOL fEnable)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow,
                           BOOL fRameWindow)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup,
                                   DWORD nCmdID)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget,
				IDropTarget** ppDropTarget)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetExternal)(IDispatch** ppDispatch)
	{
		// ������չ�����������
		CExCommandHandler* pHandler = new CExCommandHandler();
		pHandler->QueryInterface(IID_IDispatch, (void**)ppDispatch);
		//*ppDispatch = new CExCommandHandler();
		return S_OK;
	}

	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn,
				OLECHAR** ppchURLOut)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
	{
		return E_NOTIMPL;
	}
};



/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView

IMPLEMENT_DYNCREATE(CHtmlViewOWMView, CXTHtmlView)

BEGIN_MESSAGE_MAP(CHtmlViewOWMView, CXTHtmlView)
	//{{AFX_MSG_MAP(CHtmlViewOWMView)
	ON_COMMAND(ID_GO_BACK, OnGoBack)
	ON_UPDATE_COMMAND_UI(ID_GO_BACK, OnUpdateGoBack)
	ON_COMMAND(ID_GO_FORWARD, OnGoForward)
	ON_UPDATE_COMMAND_UI(ID_GO_FORWARD, OnUpdateGoForward)
	ON_COMMAND(ID_VIEW_STOP, OnViewStop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STOP, OnUpdateViewStop)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, OnUpdateViewRefresh)
	ON_COMMAND(ID_GO_START_PAGE, OnGoStartPage)
	ON_UPDATE_COMMAND_UI(ID_GO_START_PAGE, OnUpdateGoStartPage)
	ON_COMMAND(ID_GO_SEARCH_THE_WEB, OnGoSearchTheWeb)
	ON_UPDATE_COMMAND_UI(ID_GO_SEARCH_THE_WEB, OnUpdateGoSearchTheWeb)
	ON_COMMAND(ID_VIEW_FOLDERS, OnViewFolders)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOLDERS, OnUpdateViewFolders)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateViewHistory)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CXTHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CXTHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	// ���У�ճ�������
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView construction/destruction

CHtmlViewOWMView::CHtmlViewOWMView()
{
	// TODO: add construction code here
	CExUIHandler* pUI = new CExUIHandler();
	pUI->QueryInterface(IID_IDocHostUIHandler, (void**)&m_IExUIHandler);
	m_pUI = pUI;
}

CHtmlViewOWMView::~CHtmlViewOWMView()
{
	if(m_pUI)
	{
		delete (CExUIHandler*)m_pUI;
	}
}

BOOL CHtmlViewOWMView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CXTHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView drawing

void CHtmlViewOWMView::OnDraw(CDC* pDC)
{
	CHtmlViewOWMDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView printing

void CHtmlViewOWMView::OnFilePrintPreview() 
{
	//DoPrintPreview (this);
	//CXTHtmlView::OnFilePrintPreview();
}

BOOL CHtmlViewOWMView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
	//return TRUE;
}

void CHtmlViewOWMView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CHtmlViewOWMView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView diagnostics

#ifdef _DEBUG
void CHtmlViewOWMView::AssertValid() const
{
	CXTHtmlView::AssertValid();
}

void CHtmlViewOWMView::Dump(CDumpContext& dc) const
{
	CXTHtmlView::Dump(dc);
}

CHtmlViewOWMDoc* CHtmlViewOWMView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHtmlViewOWMDoc)));
	return (CHtmlViewOWMDoc*)m_pDocument;
}
#endif //_DEBUG

// ���У�ճ�������
void CHtmlViewOWMView::OnEditCut() 
{
      ExecWB(OLECMDID_CUT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);  
}

void CHtmlViewOWMView::OnEditCopy() 
{
      ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);  
}

void CHtmlViewOWMView::OnEditPaste() 
{
      ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);  
}

void CHtmlViewOWMView::OnEditSelectAll() 
{
      ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
}

void CHtmlViewOWMView::OnEditUndo() 
{
      ExecWB(OLECMDID_UNDO, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMView message handlers
void CHtmlViewOWMView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	get_dll_resource();
	CMenu* pMenu = new CMenu();
	pMenu->LoadMenu(IDR_OWM);
	reset_dll_resource();
	CMenu* pmenu = (CMenu*)pMenu->GetSubMenu(0);
	//pmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
	//		point.x, point.y, this);
	//CXTPCommandBars::TrackPopupMenu(pmenu, 0, point.x, point.y, this);
	theApp.m_pIPlatUI->TrackPopupMenu(pmenu, 0, point.x, point.y, this);
	delete pMenu;
}

void CHtmlViewOWMView::OnTitleChange(LPCTSTR lpszText) 
{
	GetDocument()->SetTitle(lpszText);
}

void CHtmlViewOWMView::OnInitialUpdate() 
{
	CXTHtmlView::OnInitialUpdate();
	
	CString strStartScript = theApp.m_strDefaultPage;//"local:OWM://\\html\\start.htm";
	Navigate2(strStartScript);
}

/////////////////////////////////////////////////////////////////////////////
// ���沢���õ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::SaveAndSetCurPath(CString strURL)
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
void CHtmlViewOWMView::RestoreCurPath()
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

void CHtmlViewOWMView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel) 
{
	CString strURL = lpszURL;

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
		for(i=0; i<asInterpName.GetSize(); i++)
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
				theApp.m_pIPlatUI->ReleaseVciObject(g_pInterp, TRUE);
			}
			g_pInterp = pInterp;

			// ��ִ�нű�֮ǰ�����ñ�ʾ��ǰҳ���ļ���TCL����
			CHtmlViewOWMDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			CString strHtmlPath = pDoc->m_strCurURL;
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
				//int nPos = strURL.Find(".tcl");
				//if (nPos == (strURL.GetLength()-4))	// ִ��TCL�ļ�,���ܴ������в���
				//{
				//	theApp.ProcessPath(strURL);	// ����·��
				//	bRet = pInterp->RunScriptFile(strURL);	// ִ�нű��ļ�
				//}else	// ִ��TCL����
				{
					bRet = pInterp->RunScriptCommand(strURL);	// ִ�нű�����
				}
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
					Navigate2(strHtmlNewURL);
				}else
				{
					// ���HTMLˢ�±�Ǳ���
					CString strHtmlRefreshSign = pInterp->GetVar(_T("_htmlRefreshSign"));
					if(strHtmlRefreshSign == "true")
					{	// ��ʾ��ҳ��仯,��Ҫˢ��
						Refresh();
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
	
	CXTHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}

void CHtmlViewOWMView::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	ICustomDoc * pCustdoc = NULL; 
	IHTMLDocument2Ptr pDoc(GetHtmlDocument());
	if (pDoc)
	{
		pDoc->QueryInterface(&pCustdoc);
	}
	//GetHtmlDocument()->QueryInterface(&pCustdoc);
	if (pCustdoc)
	{
		pCustdoc->SetUIHandler(m_IExUIHandler);  
		//if call pCustdoc->SetUIHandler(m_pHandler),then you can't using
		//mouse to drag-select html's text...why?
		pCustdoc->Release();
	}

	IHTMLElementPtr e = GetHtmlElement("button_tlf1");
	if(e)
	{
		IHTMLInputButtonElementPtr btn = (IHTMLInputButtonElementPtr)e;
		btn->put_disabled(1L);
		//btn->put_value(L"jhdsfihf");
	}

	CXTHtmlView::OnDocumentComplete(lpszURL);
}

IHTMLDocument2Ptr CHtmlViewOWMView::GetHtmlFrameWindowDOM(LPCTSTR lpszFrameId)
{
	IHTMLDocument2Ptr pDoc(GetHtmlDocument());
	if(pDoc == NULL)
	{
		return NULL;
	}

	IHTMLFramesCollection2Ptr piframes = NULL;
	pDoc->get_frames(&piframes);
	if(piframes == NULL)
	{
		return NULL;
	}

	IHTMLWindow2Ptr pifrm = NULL;	
	COleVariant varid(lpszFrameId);
	VARIANT frameOut;
	piframes->item(varid, &frameOut);

	frameOut.pdispVal->QueryInterface(IID_IHTMLWindow2, (void**)&pifrm);
	if(pifrm == NULL)
	{
		return NULL;
	}

	IHTMLDocument2Ptr pDocIFrm = NULL;
	pifrm->get_document(&pDocIFrm);

	return pDocIFrm;
}

IHTMLElementPtr CHtmlViewOWMView::GetHtmlElement(LPCTSTR lpszId)
{
	IHTMLDocument2Ptr pDoc(GetHtmlDocument());
	if(pDoc == NULL)
	{
		return NULL;
	}

	IHTMLElementCollectionPtr ec;
	IHTMLElementPtr e;
	pDoc->get_all(&ec);
	COleVariant varid(lpszId);
	COleVariant varIndex((long)0, VT_I4);
	ec->item(varid, varIndex, (IDispatch**)(&e));
	//IHTMLElementPtr e(pDoc->Getall()->item(lpszId));
	return e;
}

void CHtmlViewOWMView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (bActivate)
	{
		// �����Դ�����ʾͼ����Ϣ
		GetDocument()->RefreshDocProperty();
	}

	CXTHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnGoBack() 
{
	GoBack();
}

void CHtmlViewOWMView::OnUpdateGoBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ǰ��
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnGoForward() 
{
	GoForward();
}

void CHtmlViewOWMView::OnUpdateGoForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ֹͣ
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnViewStop() 
{
	Stop();
}

void CHtmlViewOWMView::OnUpdateViewStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ˢ��
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnViewRefresh() 
{
	Refresh();
}

void CHtmlViewOWMView::OnUpdateViewRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼҳ
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnGoStartPage() 
{
	//GoStartPage();
	CString strStartScript = theApp.m_strDefaultPage;
	Navigate2(strStartScript);
}

void CHtmlViewOWMView::OnUpdateGoStartPage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// ����
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnGoSearchTheWeb() 
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

void CHtmlViewOWMView::OnUpdateGoSearchTheWeb(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// �ļ���
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnViewFolders() 
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

void CHtmlViewOWMView::OnUpdateViewFolders(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// �ļ���
/////////////////////////////////////////////////////////////////////////////
void CHtmlViewOWMView::OnViewHistory() 
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

void CHtmlViewOWMView::OnUpdateViewHistory(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

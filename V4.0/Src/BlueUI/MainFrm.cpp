// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "BlueUI.h"

#include "MainFrm.h"
#include "ComboBoxSearch.h"
#include "InetTools.h"
#include "SkinProgress.h"

#include "childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlTools - ��չ���߲˵���

class CControlTools : public CXTPControlButton
{
	DECLARE_XTP_CONTROL(CControlTools)

protected:
	virtual void OnCalcDynamicSize(DWORD dwMode);
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);
};

IMPLEMENT_XTP_CONTROL(CControlTools, CXTPControlButton)

void CControlTools::OnCalcDynamicSize(DWORD /*dwMode*/)
{
	if (GetParent()->GetType() !=xtpBarTypePopup)
		return;

	ASSERT(m_pControls->GetAt(m_nIndex) == this);

	while (m_nIndex + 1 < m_pControls->GetCount())
	{
		CXTPControl* pControl = m_pControls->GetAt(m_nIndex + 1);
		if (pControl->GetID() >= FIRST_TOOL_COMMAND && pControl->GetID() < LAST_TOOL_COMMAND)
		{
			m_pControls->Remove(pControl);
		}
		else break;
	}

	m_dwHideFlags = 0;

	if (m_pParent->IsCustomizeMode())
		return;

	CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
	ASSERT(pFrame);

	m_dwHideFlags |= xtpHideGeneric;

	int nCount = theApp.m_aTools.GetSize();
	int nIndex = 0;
	for (int i = 0; i < nCount; i++)
	{
		if(!theApp.m_aTools[i].m_bShowMenu)
		{
			continue;
		}

		CString strTitle;
		strTitle = theApp.m_aTools[i].m_strName;
		theApp.m_aTools[i].m_nMenuIndex = nIndex;

		CXTPControl* pControl = m_pControls->Add(xtpControlButton, FIRST_TOOL_COMMAND + nIndex, _T(""), m_nIndex + 1 + i, TRUE);

		pControl->SetCaption(strTitle);
		pControl->SetFlags(xtpFlagManualUpdate);

		if (GetBeginGroup() && (i == 0))
			pControl->SetBeginGroup(TRUE);

		nIndex++;
	}
}

BOOL CControlTools::IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint /*point*/, DROPEFFECT& dropEffect)
{
	if (pCommandBar->GetType() != xtpBarTypePopup)
	{
		dropEffect = DROPEFFECT_NONE;
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CControlExports - �����˵���

class CControlExports : public CXTPControlButton
{
	DECLARE_XTP_CONTROL(CControlExports)

protected:
	virtual void OnCalcDynamicSize(DWORD dwMode);
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);
};

IMPLEMENT_XTP_CONTROL(CControlExports, CXTPControlButton)

void CControlExports::OnCalcDynamicSize(DWORD /*dwMode*/)
{
	if (GetParent()->GetType() !=xtpBarTypePopup)
		return;

	ASSERT(m_pControls->GetAt(m_nIndex) == this);

	while (m_nIndex + 1 < m_pControls->GetCount())
	{
		CXTPControl* pControl = m_pControls->GetAt(m_nIndex + 1);
		if (pControl->GetID() >= FIRST_EXPORT_COMMAND && pControl->GetID() < LAST_EXPORT_COMMAND)
		{
			m_pControls->Remove(pControl);
		}
		else break;
	}

	m_dwHideFlags = 0;

	if (m_pParent->IsCustomizeMode())
		return;

	CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
	ASSERT(pFrame);

	m_dwHideFlags |= xtpHideGeneric;

	// ��ȡ��ǰ�ĵ��ĵ����˵��б�
	CPlugIn* pPlugIn = NULL;
	theApp.GetOwmDocumentFromFrame(pFrame->MDIGetActive(), pPlugIn);
	if(pPlugIn == NULL)
	{
		return;
	}
	IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
	if(pIVciOwm == NULL)
	{
		return;
	}

	CStringArray asExportType;
	pIVciOwm->GetExportTypeList(asExportType);
	if(asExportType.GetSize() == 0)
	{
		return;
	}

	for(int i=0; i<asExportType.GetSize(); i++)
	{
		CXTPControl* pControl = m_pControls->Add(xtpControlButton, FIRST_EXPORT_COMMAND + i, _T(""), -1, TRUE);

		pControl->SetCaption(asExportType[i]);
		pControl->SetFlags(xtpFlagManualUpdate);

		if (GetBeginGroup() && (i == 0))
			pControl->SetBeginGroup(TRUE);
	}
}

BOOL CControlExports::IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint /*point*/, DROPEFFECT& dropEffect)
{
	if (pCommandBar->GetType() != xtpBarTypePopup)
	{
		dropEffect = DROPEFFECT_NONE;
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_COMMAND(ID_FILE_NEW_BLANK, OnFileNewBlank)
	ON_COMMAND(ID_REFRESH_GOTO_URL, OnRefreshGotoUrl)
	ON_UPDATE_COMMAND_UI(ID_REFRESH_GOTO_URL, OnUpdateRefreshGotoUrl)
	ON_COMMAND(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSE_ALL, OnUpdateWindowCloseAll)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_MESSAGE(WM_XTP_PRETRANSLATEMOUSEMSG, OnTabbarMouseMsg)
	ON_XTP_CREATECONTROL()
    ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)

	ON_MESSAGE(XTPWM_POPUPCONTROL_NOTIFY, OnPopUpNotify)
	ON_MESSAGE(XTPWM_TASKPANEL_NOTIFY, OnTaskPanelNotify)

	ON_REGISTERED_MESSAGE( WM_CHECK_ITS_ME, OnCheckItsMe )

	ON_MESSAGE(WM_PLAT_COMCMD, OnPlatComCmd)
	ON_MESSAGE(WM_PLAT_DYNCLOSE, OnPlatOWMDynClose)

	ON_COMMAND_RANGE(IDM_OWMCMD, IDM_OWMCMD+4095, OnOwmExCmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_OWMCMD, IDM_OWMCMD+4095, OnUpdateOwmExCmd)

	ON_XTP_EXECUTE(ID_GOTO_URL, OnGotoUrl)
	ON_UPDATE_COMMAND_UI(ID_GOTO_URL, OnUpdateGotoUrl)

	ON_COMMAND(ID_TOGGLE_DOCK, OnToggleDock)

	ON_COMMAND_RANGE(FIRST_STATUS_COMMAND, LAST_STATUS_COMMAND, OnIndicatorButton)
	ON_UPDATE_COMMAND_UI_RANGE(FIRST_STATUS_COMMAND, LAST_STATUS_COMMAND, OnUpdateIndicatorButton)

	ON_COMMAND_RANGE(FIRST_TOOL_COMMAND, LAST_TOOL_COMMAND, OnToolClick)

	ON_COMMAND_RANGE(FIRST_EXPORT_COMMAND, LAST_EXPORT_COMMAND, OnExportClick)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

static UINT uHideCmds[] = 
{
	ID_WINDOW_CASCADE,
	ID_FILE_PRINT,
	ID_FILE_PRINT_PREVIEW,
};

static CSkinProgress* g_pSkinProgress = NULL;	// ����״̬������

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// ����Ƥ��
	LoadSkin();

	m_strIniFileName = theApp.GetPlatRootPath();
#ifdef _DEBUG
	m_strIniFileName += "bind\\";
#else
	m_strIniFileName += "bin\\";
#endif
	if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
	{
		m_strIniFileName += "BlueUI_cn.xml";
	}else
	{
		m_strIniFileName += "BlueUI.xml";
	}

	m_bPropInUpdate = FALSE;
	m_bEnableUpdatePropertyGrid = TRUE;
	m_pComboBoxURL = NULL;
}

CMainFrame::~CMainFrame()
{
}

// ����ʱ�����һ��
void CMainFrame::LoadSkin()
{
#ifdef _XTP_INCLUDE_SKINFRAMEWORK
	XTPSkinManager()->GetResourceFile()->SetModuleHandle(AfxGetInstanceHandle());
#endif

	m_nSkinType = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_TYPE, SKIN_OFFICE2007);

	//m_nSkinType = SKIN_OFFICE2007;

	CString strSkinFile = "";

	strSkinFile = theApp.GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_FILE);
	if(GetFileAttributes(strSkinFile) != 0xFFFFFFFF)
	{
		DWORD dwApplyOptions = xtpSkinApplyMenus;
		BOOL bApplyMetrics = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_METRICS, TRUE);
		dwApplyOptions |= bApplyMetrics ? xtpSkinApplyMetrics: 0;
		BOOL bApplyFrame = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_FRAMESKIN, TRUE);
		dwApplyOptions |= bApplyFrame ? xtpSkinApplyFrame: 0;
		BOOL bApplyColors = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_COLORS, TRUE);
		dwApplyOptions |= bApplyColors ? xtpSkinApplyColors: 0;
		XTPSkinManager()->SetApplyOptions(dwApplyOptions);
	}

	CString strSkinPath = theApp.GetPlatRootPath();
	strSkinPath += "\\Skins\\";

	switch (m_nSkinType)
	{
	case SKIN_OFFICE2007:
		break;
	case SKIN_CJINI:
		strSkinFile = strSkinPath + "Le5";
		XTPSkinManager()->LoadSkin(strSkinFile, _T("NormalBlue.INI"));
		break;
	case SKIN_CJSTYLE:
		strSkinFile = strSkinPath + "Office2010.cjstyles";
		XTPSkinManager()->LoadSkin(strSkinFile, "");
		break;
	case SKIN_MSSTYLE:
		strSkinFile = strSkinPath + "MsStyle\\Tiger.msstyles";
		XTPSkinManager()->LoadSkin(strSkinFile, "");
		break;
	default:
		XTPSkinManager()->LoadSkin(_T(""), _T(""));
	}

	//if (m_hWnd) OnSkinChanged();
}

void CMainFrame::RedrawFrame(CWnd* pWnd)
{
	pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ALLCHILDREN);		
}

// ����Ƥ��ʱ�����
void CMainFrame::OnSkinChanged()
{
	if (m_nTheme == themeOffice2003)
	{
		if (XTPSkinManager()->GetResourceFile() &&
			XTPSkinManager()->GetResourceFile()->GetResourcePath().Find(_T("Office2007")) > 0)
		{
			XTPPaintManager()->SetTheme(xtpThemeOffice2007);
		}
		else
		{		
			XTPPaintManager()->SetTheme(xtpThemeOffice2003);
		}
	}
	XTPPaintManager()->RefreshMetrics();

	RedrawFrame(this);	
	
	m_wndStatusBar.Invalidate();

	CWnd* pWnd = CWnd::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	while(pWnd)
	{
		RedrawFrame(pWnd);
		
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	GetCommandBars()->GetPaintManager()->RefreshMetrics();

	GetCommandBars()->RedrawCommandBars();
}

// ���������ļ��е������ַ������Ҷ�һ���Ķ���
int CMainFrame::GetThemeString(CString strTheme)
{
	int nTheme = themeVisualStudio2010;
	if(strTheme == "ThemeRibbon")
		nTheme = themeRibbon;
	else if(strTheme == "ThemeOffice2003")
		nTheme = themeOffice2003;
	else if(strTheme == "ThemeOffice2007")
		nTheme = themeOffice2007;
	else if(strTheme == "ThemeOffice2010")
		nTheme = themeOffice2010;
	else if(strTheme == "ThemeVisualStudio2005")
		nTheme = themeVisualStudio2005;
	else if(strTheme == "ThemeVisualStudio2008")
		nTheme = themeVisualStudio2008;
	else if(strTheme == "ThemeVisualStudio2010")
		nTheme = themeVisualStudio2010;
	return nTheme;
}

// ���õ�ǰ����
void CMainFrame::SetTheme(int nTheme, int nThemeColor)
{
	m_nTheme = nTheme;
	m_nThemeColor = nThemeColor;

	// ���������Office2003,��Ҫ������Դ�ļ������Ƿ񻻳�Office2007
	if (m_nTheme == themeOffice2003)
	{
		//XTPColorManager()->m_bEnableLunaBlueForRoyaleTheme = TRUE;

		if (XTPSkinManager()->GetResourceFile() &&
			XTPSkinManager()->GetResourceFile()->GetResourcePath().Find(_T("Office2007")) > 0)
		{
			nTheme = themeOffice2007;
		}
	}

	HMODULE hModule = AfxGetInstanceHandle();
	CString strSkinPath = theApp.GetPlatRootPath();
	strSkinPath += "\\Skins\\";
	LPCTSTR lpszIniFile = 0;

	switch (nTheme)
	{
	case themeRibbon:
		XTPPaintManager()->SetTheme(xtpThemeRibbon);
		hModule = LoadLibrary(strSkinPath + _T("Office2007.dll"));
		((CXTPOffice2007Theme*)GetCommandBars()->GetPaintManager())->SetImageHandle(hModule, lpszIniFile);
		m_paneManager.SetTheme(xtpPaneThemeResource);
		XTPPaintManager()->m_bEnableAnimation = TRUE;	// ����Ч��
		break;

	case themeOffice2003:
		XTPPaintManager()->SetTheme(xtpThemeOffice2003);
		m_paneManager.SetTheme(xtpPaneThemeOffice2003);
		break;

	case themeOffice2007:
		XTPPaintManager()->SetTheme(xtpThemeRibbon);
		hModule = LoadLibrary(strSkinPath + _T("Office2007.dll"));
		if(nThemeColor == themeColorBlue)
		{
			lpszIniFile = _T("OFFICE2007BLUE.INI");
		}else
		if(nThemeColor == themeColorSilver)
		{
			lpszIniFile = _T("OFFICE2007SILVER.INI");
		}else
		if(nThemeColor == themeColorBlack)
		{
			lpszIniFile = _T("OFFICE2007BLACK.INI");
		}
		((CXTPOffice2007Theme*)GetCommandBars()->GetPaintManager())->SetImageHandle(hModule, lpszIniFile);
		m_paneManager.SetTheme(xtpPaneThemeResource);
		XTPPaintManager()->m_bEnableAnimation = TRUE;	// ����Ч��
		break;

	case themeOffice2010:
		XTPPaintManager()->SetTheme(xtpThemeRibbon);
		hModule = LoadLibrary(strSkinPath + _T("Office2010.dll"));
		if(nThemeColor == themeColorBlue)
		{
			lpszIniFile = _T("OFFICE2010BLUE.INI");
		}else
		if(nThemeColor == themeColorSilver)
		{
			lpszIniFile = _T("OFFICE2010SILVER.INI");
		}else
		if(nThemeColor == themeColorBlack)
		{
			lpszIniFile = _T("OFFICE2010BLACK.INI");
		}
		((CXTPOffice2007Theme*)GetCommandBars()->GetPaintManager())->SetImageHandle(hModule, lpszIniFile);
		m_paneManager.SetTheme(xtpPaneThemeResource);
		XTPPaintManager()->m_bEnableAnimation = TRUE;	// ����Ч��
		break;

	case themeVisualStudio2005:
		XTPPaintManager()->SetTheme(xtpThemeWhidbey);
		m_paneManager.SetTheme(xtpPaneThemeVisualStudio2005);
		//m_wndTaskPanel.SetTheme(xtpTaskPanelThemeToolboxWhidbey);
		//m_wndProperties.m_wndPropertyGrid.SetTheme(xtpGridThemeWhidbey);
		break;

	case themeVisualStudio2008:
		XTPPaintManager()->SetTheme(xtpThemeVisualStudio2008);
		m_paneManager.SetTheme(xtpPaneThemeVisualStudio2008);
		break;

	case themeVisualStudio2010:
		XTPPaintManager()->SetTheme(xtpThemeVisualStudio2010);
		m_paneManager.SetTheme(xtpPaneThemeVisualStudio2010);
		break;

	default:
		XTPPaintManager()->SetTheme(xtpThemeVisualStudio2010);
		m_paneManager.SetTheme(xtpPaneThemeVisualStudio2010);
		break;
	}

	//GetCommandBars()->EnableFrameTheme(TRUE);

	// Pane�����VS2010���,��Ҫ����һЩ�������
	if (m_paneManager.GetCurrentTheme() == xtpPaneThemeVisualStudio2010)
	{
		m_paneManager.SetClientMargin(6);
		m_paneManager.SetThemedFloatingFrames(TRUE);
	}
	else
	{
		m_paneManager.SetClientMargin(0);
		m_paneManager.SetThemedFloatingFrames(FALSE);
	}

	// ������ͼ���һЩѡ������
	XTPPaintManager()->GetIconsInfo()->bUseFadedIcons = FALSE;
	XTPPaintManager()->GetIconsInfo()->bIconsWithShadow = TRUE;	// ����ƶ���������ͼ��ʱ��,ͼ�����ʾ��ӰЧ��

	// ����2007��ܵ�Ч���ǿ�ܴ��ڱ�����Բ����,����Ӱ��Ƥ�����л�
	CString strSkinFile = theApp.GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_FILE);
	if(strSkinFile == "")
	{
		//EnableOffice2007Frame((nTheme == themeRibbon) ? GetCommandBars() : 0);
	}

	GetCommandBars()->RedrawCommandBars();

	m_MTIClientWnd.GetPaintManager()->RefreshMetrics();	
	
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    // Create Status bar. 
    // Important: All control bars including the Status Bar 
    // must be created before CommandBars....
   if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
	//m_wndStatusBar.UseCommandBarsTheme(TRUE);
   m_wndStatusBar.EnableCustomization();

    // Initialize the command bars
    if (!InitCommandBars())
        return -1;

	// ���غͳ�ʼ��Ƥ��������
	/*CString strSkinFile = theApp.GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_FILE);
	CString strSkinIni = theApp.GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_INI);
	if(GetFileAttributes(strSkinFile) != 0xFFFFFFFF)
	{
		//strSkinFile = theApp.GetPlatRootPath();
		//strSkinFile += "\\Skins\\Office2007.cjstyles";
		DWORD dwApplyOptions = xtpSkinApplyMenus;
		BOOL bApplyMetrics = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_METRICS, TRUE);
		dwApplyOptions |= bApplyMetrics ? xtpSkinApplyMetrics: 0;
		BOOL bApplyFrame = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_FRAMESKIN, TRUE);
		dwApplyOptions |= bApplyFrame ? xtpSkinApplyFrame: 0;
		BOOL bApplyColors = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_APPLY_COLORS, TRUE);
		dwApplyOptions |= bApplyColors ? xtpSkinApplyColors: 0;
		XTPSkinManager()->SetApplyOptions(dwApplyOptions);
		XTPSkinManager()->LoadSkin(strSkinFile, strSkinIni);
		OnSkinChanged();
	}*/
	m_nTheme = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_THEME, themeVisualStudio2010);
	m_nThemeColor = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_SKIN_THEMECOLOR, themeColorBlue);
	//CString strTheme = theApp.GetConfText("Theme\\CommandBar");
	//m_nTheme = GetThemeString(strTheme);
	//m_nTheme = themeVisualStudio2010;
	SetTheme(m_nTheme, m_nThemeColor);
/*
	//XTPOffice2007Images()->SetHandle(_T("Office2007Blue.dll"));

	// ���Ƥ�����Ͳ���DLL����,��ThemeĬ������Ϊ2003���
	if ((m_nSkinType == SKIN_CJINI) || (m_nSkinType == SKIN_CJSTYLE) || (m_nSkinType == SKIN_MSSTYLE))
	{
		strTheme = "ThemeOffice2003";
	}
*/
    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars == NULL)
    {
        TRACE0("Failed to create command bars object.\n");
        return -1;      // fail to create
    }

	m_wndStatusBar.SetFont(pCommandBars->GetPaintManager()->GetIconFont());
	m_wndStatusBar.SetDrawDisabledText(FALSE);
	m_wndStatusBar.SetCommandBars(pCommandBars);
	m_wndStatusBar.GetStatusBarCtrl().SetMinHeight(22);
	m_wndStatusBar.GetPane(0)->SetMargins(8, 1, 2, 1);
	//m_wndStatusBar.GetImageManager()->SetIcons(IDR_OUTPUT);

	// ���һ��Pane,���ڽ�����ʾ,Pane ID=1
	AddPane(1);
	SetPaneWidth(1, 0);
	
	// Load menu bar caption.
	CString strCapt;
	strCapt.LoadString(IDS_MENUBAR);

    // Add the menu bar
    CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(
        strCapt, IDR_MAINFRAME);       
    if(pMenuBar == NULL)
    {
        TRACE0("Failed to create menu bar.\n");
        return -1;      // fail to create
    }
	//pMenuBar->SetFlags(xtpFlagHideMinimizeBox | xtpFlagHideMaximizeBox | xtpFlagIgnoreSetMenuMessage);

	// Load command bar caption.
	strCapt.LoadString(IDS_STANDARD);

    // Create Stand ToolBar
    CXTPToolBar* pToolBar = (CXTPToolBar*)
        pCommandBars->Add(strCapt, xtpBarTop);
    if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;
    }

	CXTPToolBar* pExplorerBar = (CXTPToolBar*)pCommandBars->Add(_T("Explorer"), xtpBarTop);
	if (!pExplorerBar ||
		!pExplorerBar->LoadToolBar(IDR_EXPLORER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	DockRightOf(pExplorerBar, pToolBar);

	//CXTPOffice2003Theme::LoadModernToolbarIcons(IDR_MAINFRAME);

	// Hide array of commands
	pCommandBars->HideCommands(uHideCmds, _countof(uHideCmds));

	// Set "Always Show Full Menus" option to the FALSE
	pCommandBars->GetCommandBarsOptions()->bAlwaysShowFullMenus = FALSE;

	// ����ƽ̨ͼ����
	CString strImagePath = theApp.GetPlatRootPath() + "conf\\image";
	if(!LoadImageStyleIcons(strImagePath))
	{
		// �������ʧ�ܾͼ��س����ڲ���
		XTPImageManager()->SetIcons(IDR_MAINFRAME);
	}

	pCommandBars->GetCommandBarsOptions()->ShowKeyboardCues(xtpKeyboardCuesShowWindowsDefault);
	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);

	// toolbar��tooltip����
	pCommandBars->GetToolTipContext()->SetStyle((XTPToolTipStyle)xtpToolTipOffice2007);
	pCommandBars->GetToolTipContext()->ShowTitleAndDescription(TRUE);
	pCommandBars->GetToolTipContext()->ShowImage(TRUE);	

    // Load the previous state for toolbars and menus.
    //LoadCommandBars(_T("CommandBars"));

	// Initialize the docking pane manager and set the
	// initial them for the docking panes.  Do this only after all
	// control bars objects have been created and docked.
	m_paneManager.InstallDockingPanes(this);

	// dockingpaneƤ��ģʽ
	m_paneManager.SetThemedFloatingFrames(!m_paneManager.IsThemedFloatingFrames());
	m_paneManager.SetStickyFloatingFrames(!m_paneManager.IsStickyFloatingFrames());
	// dockingpane����Ϊ͸��ģʽ
	m_paneManager.SetFloatingFramesOpacity(m_paneManager.GetFloatingFramesOpacity() != 255 ? 255 : 200);

	// Create docking panes.
	// �������Դ���
	m_paneManager.CreatePane(IDR_PANE_PROPERTIES, CRect(0, 0,200, 120), xtpPaneDockLeft);

	// �����������(��Ҫ����״̬��)
	//CreateOutputWnd("�������");
	DOMNode* pNode = theApp.m_xmlPlat.GetNode("output\\window");
	while (pNode != NULL)
	{
		CString strWindowName = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Name");
		CString strWindowType = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Type");
		int nWindowType = OUTPUT_RICH;
		if(strWindowType == "GRID")
		{
			nWindowType = OUTPUT_GRID;
		}else
		if(strWindowType == "TREE")
		{
			nWindowType = OUTPUT_TREE;
		}
		CString strWindowSave = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Save");
		int nWindowId = -1;
		if(theApp.m_xmlPlat.GetNodeAttribute(pNode, "ID") != "")
		{
			nWindowId = atoi(theApp.m_xmlPlat.GetNodeAttribute(pNode, "ID"));
		}
		if(strWindowSave == "true")
		{
			// ��Ҫ����״̬�Ĵ���
			COutputWndCtrl* pOutputWndCtrl = CreateOutputWnd(strWindowName, nWindowType, nWindowId);
			pOutputWndCtrl->bSave = TRUE;
		}
		pNode = theApp.m_xmlPlat.GetNextNode(pNode, "window");
	}

	// Set the icons for the docking pane tabs.
	//int nIDIcons[] = {IDR_PANE_OPTIONS, IDR_PANE_PROPERTIES};
	//	m_paneManager.SetIcons(IDB_BITMAP_ICONS, nIDIcons, 
	//	_countof(nIDIcons), RGB(0, 255, 0));

    // Load the previous state for docking panes.
//    CXTPDockingPaneLayout layoutNormal(&m_paneManager);
//    if (layoutNormal.Load(_T("NormalLayout"))) 
//    {
//        m_paneManager.SetLayout(&layoutNormal); 
//    }

	//CXTPPropExchangeIniFile px(TRUE, 0, _T("Settings")); // To serialize to ini file
	CXTPPropExchangeXMLNode px(TRUE, 0, _T("Settings"));   // To serialize to XML file

	if (px.LoadFromFile(m_strIniFileName))
	{
#ifdef _XTP_INCLUDE_CONTROLS
		CXTPPropExchangeSection pxTaskPanel(px.GetSection(_T("TaskPanel")));
//		m_wndTaskPanel.GetGroups()->DoPropExchange(&pxTaskPanel);
#endif

		CXTPPropExchangeSection pxCommandBars(px.GetSection(_T("CommandBars")));
		XTP_COMMANDBARS_PROPEXCHANGE_PARAM param;
		param.bSerializeControls = TRUE; 
		GetCommandBars()->DoPropExchange(&pxCommandBars, &param);

		CXTPPropExchangeSection pxDockingPane(px.GetSection(_T("DockingPane")));
		CXTPDockingPaneLayout layoutNormal(&m_paneManager);
		if (layoutNormal.DoPropExchange(&pxDockingPane))
		{
			m_paneManager.SetLayout(&layoutNormal);
		}		
	}
	else
	{
		//ResetToolboxItems();
	}

	CXTPImageManager* pImageManager = pCommandBars->GetImageManager();

	pImageManager->InternalAddRef();
	m_paneManager.SetImageManager(pImageManager);

	m_paneManager.SetAlphaDockingContext(TRUE);
	m_paneManager.SetShowDockingContextStickers(TRUE);
	m_paneManager.SetDockingContextStickerStyle(xtpPaneStickerStyleWhidbey);

	// ע�����Ҫ֧�ַ��飬�ͼӵڶ���������������֮���ֻ����󻯴�����
	VERIFY(m_MTIClientWnd.Attach(this/*, TRUE*/));
	m_MTIClientWnd.LoadState();
	m_MTIClientWnd.EnableToolTips();
	m_MTIClientWnd.SetFlags(xtpWorkspaceHideClose | xtpWorkspaceHideArrows | xtpWorkspaceShowCloseSelectedTab);

	//m_MTIClientWnd.GetPaintManager()->m_bShowIcons = FALSE;

	// �������Դ��ڵ�ͼ��
	if(m_paneManager.FindPane(IDR_PANE_PROPERTIES) != NULL)
	{
		// ȡ��ԭ��ͨ����ʾ����Pane������ͼ��ķ���,��Ϊֱ�Ӵ���ͼ��,����ÿ�ζ��������Pane��Ϊ�����
		//if(!m_paneManager.IsPaneClosed(IDR_PANE_PROPERTIES))
		//{
		//	m_paneManager.ShowPane(IDR_PANE_PROPERTIES);
		//}
		m_paneManager.SetIcon(IDR_PANE_PROPERTIES, CXTPImageManagerIconHandle(theApp.LoadIcon(IDI_PROPERTY)));
	}
	
	// �����������(����Ҫ����״̬��)
	pNode = theApp.m_xmlPlat.GetNode("output\\window");
	while (pNode != NULL)
	{
		CString strWindowName = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Name");
		CString strWindowType = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Type");
		int nWindowType = OUTPUT_RICH;
		if(strWindowType == "GRID")
		{
			nWindowType = OUTPUT_GRID;
		}else
		if(strWindowType == "TREE")
		{
			nWindowType = OUTPUT_TREE;
		}
		CString strWindowSave = theApp.m_xmlPlat.GetNodeAttribute(pNode, "Save");
		int nWindowId = -1;
		if(theApp.m_xmlPlat.GetNodeAttribute(pNode, "ID") != "")
		{
			nWindowId = atoi(theApp.m_xmlPlat.GetNodeAttribute(pNode, "ID"));
		}
		if(strWindowSave != "true")
		{
			// ����Ҫ����״̬�Ĵ���
			COutputWndCtrl* pOutputWndCtrl = CreateOutputWnd(strWindowName, nWindowType, nWindowId);
			pOutputWndCtrl->bSave = FALSE;
		}

		if(nWindowId != -1)
		{
			m_paneManager.SetIcon(IDR_PANE_OUTPUT+nWindowId, CXTPImageManagerIconHandle(theApp.LoadIcon(IDI_DISPLAY)));
		}

		pNode = theApp.m_xmlPlat.GetNextNode(pNode, "window");
	}

	// ����������ʱ��
	SetTimer(1, 1000, NULL);

	return 0;
}

BOOL CMainFrame::ShowWindowEx(int nCmdShow)
{
	// Restore frame window size and position.
	m_wndPosition.LoadWindowPos(this);
	nCmdShow = m_wndPosition.showCmd;

	return ShowWindow(nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// ����ĳ������ID��ͼ��
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ImageManagerSetIcon(int nID, LPCTSTR lpszIconFile)
{
	BOOL bRet = XTPImageManager()->SetIconFromIcoFile(lpszIconFile, nID, 0, xtpImageNormal);
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// ���ö������ID��ͼ��
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ImageManagerSetIcons(CUIntArray* paIDs, LPCTSTR lpszBmpFile)
{
	if(paIDs == NULL)
		return FALSE;

	CBitmap bmp;

	HBITMAP hBitmap = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), lpszBmpFile,
        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if(hBitmap == NULL)
		return FALSE;

	BITMAP bmpInfo;
	::GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

	bmp.Attach(hBitmap);

	UINT uIDs[100];
	for(int i=0; i<(*paIDs).GetSize(); i++)
	{
		if(i < 100)
		{
			uIDs[i] = (*paIDs)[i];
		}
	}

	BOOL bRet = XTPImageManager()->SetIcons(bmp, uIDs, (*paIDs).GetSize(), CSize(bmpInfo.bmHeight,bmpInfo.bmHeight), xtpImageNormal);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// ���ص�ǰ����ͼ���ļ���
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::LoadImageStyleIcons(CString strImagePath)
{
	CString strImageStyle = theApp.GetProfileString(REG_CONFIG_SUBKEY, REG_CONFIG_IMAGESTYLE, "default");
	
	if(GetFileAttributes(strImagePath + "\\" + strImageStyle) == 0xFFFFFFFF)
	{
		// ���·�������ھͲ�����û������ΪAppName��·��
		CString strAppName = theApp.m_xmlPlat.GetNodeText("application\\appName");
		if(GetFileAttributes(strImagePath + "\\" + strAppName) == 0xFFFFFFFF)
		{
			return FALSE;
		}
		strImageStyle = strAppName;
	}

	strImagePath += "\\";
	strImagePath += strImageStyle;

	CString strPlugin = strImagePath;
	if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
	{
		strPlugin += "\\plugin_cn.xml";
		// ������������ĵ������ļ�,����ȱʡ��
		if(GetFileAttributes(strPlugin) == 0xFFFFFFFF)
		{
			strPlugin = strImagePath + "\\plugin.xml";
		}
	}else
	{
		strPlugin += "\\plugin.xml";
	}
	CXmlParser parser;
	if(parser.Open(strPlugin))
	{
		OUTPUT(COLOR_ERROR, "Load %s fail!\r\n", strPlugin);
		return FALSE;
	}

	DOMNode* pNode = parser.GetRootNode();
	DOMNode* pSubNode = parser.GetChildNode(pNode);
	while (pSubNode != NULL)
	{
		CString strBmpFile = strImagePath + "\\" + parser.GetNodeAttribute(pSubNode, "file");
		CString strIDs = parser.GetNodeText(pSubNode);
		CUIntArray auID;
		int nPos = strIDs.Find(",");
		while(nPos != -1)
		{
			auID.Add(atoi(strIDs.Left(nPos)));
			strIDs.Delete(0, nPos+1);
			nPos = strIDs.Find(",");
		}
		auID.Add(atoi(strIDs));

		if(!ImageManagerSetIcons(&auID, strBmpFile))
		{
			return FALSE;
		}

		pSubNode = parser.GetNextNode(pSubNode);
	}

	parser.Close();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::AddPane(UINT nPaneID)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
	{
		int nPaneCount = m_wndStatusBar.GetPaneCount();
		nPaneIndex = nPaneCount;
		m_wndStatusBar.AddIndicator(nPaneID, nPaneIndex);
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �ر�Pane
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ClosePaneItem(UINT nPaneID)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	m_wndStatusBar.RemoveIndicator(nPaneID);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �ر������û�������Pane(ID>=500˵�����û�������)
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CloseAllSelfPane()
{
	for(int i = m_wndStatusBar.GetPaneCount()-1; i >=0; i--)
	{
		UINT nPaneID = m_wndStatusBar.GetPane(i)->GetID();
		if(nPaneID >= 500)
		{
			m_wndStatusBar.RemoveIndicator(nPaneID);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane����
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SetPaneText(UINT nPaneID, LPCTSTR lpszText)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	m_wndStatusBar.SetPaneText(nPaneIndex, lpszText);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane�ؼ�
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPaneControl(UINT nPaneID, CWnd* pWnd)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return;
	m_wndStatusBar.AddControl(pWnd, nPaneID);
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane���
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SetPaneWidth(UINT nPaneID, int nWidth)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	m_wndStatusBar.SetPaneWidth(nPaneIndex, nWidth);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Paneͼ����
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SetPaneImageItem(UINT nPaneID, int nItem)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	CXTPStatusBarPane* pPane = m_wndStatusBar.GetPane(nPaneIndex);
	if(!pPane)
	{
		return FALSE;
	}

	pPane->SetIconIndex(nItem);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����һ�������������ض�Pane��������
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::AttachPaneProgress(UINT nPaneID, CString strMsg, int nMaxValue)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return;
	if(g_pSkinProgress !=NULL)
	{
		// �Ѿ������˾��˳�
		if(g_pSkinProgress->GetPane() == nPaneIndex)
		{
			return;
		}

		delete g_pSkinProgress;
		g_pSkinProgress = NULL;
	}

	g_pSkinProgress = new CSkinProgress(strMsg, 200, nMaxValue, nPaneIndex, cSPT_PERCENT, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ��ʾ/���ؽ�����
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ShowPaneProgress(UINT nPaneID, BOOL bShow)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return;

	if(g_pSkinProgress && !bShow)
	{
		delete g_pSkinProgress;
		g_pSkinProgress = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���������ȿ���
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPaneProgressPos(UINT nPaneID, int nPos)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return;

	if(g_pSkinProgress)
	{
		g_pSkinProgress->SetPos(nPos);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane��ť
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SetPaneButton(UINT nPaneID, CExtMenuInfo* pExtMenuInfo)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	CXTPStatusBarPane* pPane = m_wndStatusBar.GetPane(nPaneIndex);
	if(!pPane)
	{
		return FALSE;
	}

	pPane->SetButton(TRUE);

	if(pExtMenuInfo != NULL)
	{
		// �������չ�˵���Ϣ,����ӻ���´���Ϣ
		CExtMenuInfo* pMenuInfo = GetExtMenuInfo(pExtMenuInfo->nId);
		if(pMenuInfo == NULL)
		{
			m_arExtMenuInfo.Add(*pExtMenuInfo);
		}else
		{
			*pMenuInfo = *pExtMenuInfo;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane Tooltip
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SetPaneTooltip(UINT nPaneID, LPCTSTR lpszTooltip)
{
	int nPaneIndex = m_wndStatusBar.CommandToIndex(nPaneID);
	if(nPaneIndex < 0)
		return FALSE;
	CXTPStatusBarPane* pPane = m_wndStatusBar.GetPane(nPaneIndex);
	if(!pPane)
	{
		return FALSE;
	}

	pPane->SetTooltip(lpszTooltip);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ״̬�а�ť�¼�����
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIndicatorButton(UINT nID)
{
	CExtMenuInfo* pMenuInfo = GetExtMenuInfo(nID);
	if(pMenuInfo)
	{
		if(pMenuInfo->pProc)
		{
			// ִ�а�ť����
			if(pMenuInfo->nProcType == MENU_PROC_TYPE_NORMAL)
			{
				pMenuInfo->pProc();
			}else
			if(pMenuInfo->nProcType == MENU_PROC_TYPE_PARAM_ID)
			{
				TYPEOF_ClientMenuParamId* ProcMenu = (TYPEOF_ClientMenuParamId*)(pMenuInfo->pProc);
				ProcMenu(nID);
			}
		}else
		if(pMenuInfo->m_strAction != "")
		{
			CString strAction = pMenuInfo->m_strAction;
			if(strAction.Find("tool:") == 0)
			{
				// ִ��ָ���Ĺ���
				strAction.Delete(0, 5);
				for(int i=0; i<theApp.m_aTools.GetSize(); i++)
				{
					if(theApp.m_aTools[i].m_strId == strAction)
					{
						theApp.m_aTools[i].Run();
						return;
					}
				}
			}else
			{
				// ִ�а�ť����
				// TODO:δ���......,��Ҫ��App�����һ��ͨ�õ�Action����������ִ�к���,֧��interp,html,owm,tool�����͵Ķ���
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ״̬�а�ťˢ��
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateIndicatorButton(CCmdUI* pCmdUI)
{
	CExtMenuInfo* pMenuInfo = GetExtMenuInfo(pCmdUI->m_nID);
	if(pMenuInfo && pMenuInfo->pUpdateProc)
	{
		pMenuInfo->pUpdateProc(pCmdUI);
	}
}

/////////////////////////////////////////////////////////////////////////////
// �����ĵ�ͼ��
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetDocumentIcon(CString strIconFile, HWND hWnd)
{
	CXTPTabManagerItem* pItem = m_MTIClientWnd.FindItem(hWnd);
	if(pItem)
	{
		CWnd* pFrame = CWnd::FromHandle(pItem->GetHandle());
		WORD wIndex = 0;
		CString strIconFileTmp = strIconFile;
		HICON hIcon = ::ExtractAssociatedIcon(AfxGetApp()->m_hInstance,
					strIconFile.GetBuffer(0), &wIndex);
		//strIconFile.ReleaseBuffer();
		pFrame->SetIcon(hIcon, TRUE);
		m_MTIClientWnd.Refresh();
	}
}

/////////////////////////////////////////////////////////////////////////////
// �����ĵ�����
// ���strTitleΪ��,�����nModifyType����,1��ʾ��*,0��ʾȥ��*,2��ʾ��>>>
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetDocumentTitle(CString strTitle, int nModifyType, HWND hWnd)
{
	CXTPTabManagerItem* pItem = m_MTIClientWnd.FindItem(hWnd);
	if(pItem)
	{
		CWnd* pFrame = CWnd::FromHandle(pItem->GetHandle());
		CDocument* pDoc = ((CMDIChildWnd*)pFrame)->GetActiveDocument();
		if(pDoc)
		{
			if(strTitle != "")
			{
				pDoc->SetTitle(strTitle);
			}else
			{
				CString strOldTitle = pDoc->GetTitle();
				if(nModifyType == 0)	// ����ģʽ
				{
					strOldTitle.Replace("*", "");
					strOldTitle.Replace(">", "");
				}else
				if(nModifyType == 1)	// �ļ����޸�
				{
					strOldTitle.Replace("*", "");
					strOldTitle += "*";
				}else
				if(nModifyType == 2)	// ����ģʽ
				{
					strOldTitle.Replace(">", "");
					strOldTitle = ">>>" + strOldTitle;
				}else
				if(nModifyType == 3)	// �ӵ���ģʽ
				{
					strOldTitle.Replace(">", "");
					strOldTitle = ">>" + strOldTitle;
				}
				pDoc->SetTitle(strOldTitle);
			}
		}
		m_MTIClientWnd.Refresh();
	}
}

void CMainFrame::ResetCommandBars()
{
	m_bInRecalcLayout = TRUE;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	for (int i = 0; i < pCommandBars->GetCount(); i++)
	{
		CXTPToolBar* pToolBar = pCommandBars->GetAt(i);
		if (pToolBar->IsBuiltIn())
		{
			if (pToolBar->GetType() != xtpBarTypeMenuBar)
			{
				pToolBar->LoadToolBar(pToolBar->GetBarID(), FALSE);
			}
			else
			{
				CMenu menu;
				XTPResourceManager()->LoadMenu(&menu, IDR_MAINFRAME);

				pToolBar->LoadMenu(&menu);
				pToolBar->GetControls()->CreateOriginalControls();

				((CXTPMenuBar*)pToolBar)->RefreshMenu();
			}
		}
	}

	CString strCapt;
	XTPResourceManager()->LoadString(&strCapt, IDS_MENUBAR);
	pCommandBars->GetMenuBar()->SetTitle(strCapt);

	XTPResourceManager()->LoadString(&strCapt, IDS_STANDARD);
	pCommandBars->GetAt(1)->SetTitle(strCapt);


	m_bInRecalcLayout = FALSE;
	RecalcLayout();
	AfxGetApp()->OnIdle(0);
	UpdateWindow();
}

void CMainFrame::OnClose() 
{
	// ֹͣ������ʱ��
	KillTimer(1);

	// �رչ���
	if(theApp.m_pIProject)
	{
		if(PROJECT_WORKING == theApp.m_pIProject->GetProjectState(""))
		{
			// ������������
			PLAT_LOADSTRING(strProjectWorking, IDS_PROJECT_WORKING);
			AfxMessageBox(strProjectWorking);
			return;
		}
		theApp.OnProjectClose();
		if(theApp.m_pIProject)
		{
			return;
		}
	}

	// �����̹رչ���������Ȼ��۵���Ϣ�����
	if(::IsWindow(m_hWnd))
	{
		PostMessage(WM_PLAT_COMCMD, 1, 1);	// ����һ����־��Ϣ����֤�����ѭ�����˳�
		MSG msg;
		while(::PeekMessage(&msg, m_hWnd, WM_PLAT_COMCMD, WM_PLAT_COMCMD, PM_REMOVE))
		{	// ѭ��ȡ��Ϣ������
			if((1==msg.wParam) && (1==msg.lParam))	// �ж��˳���־
				break;
			TranslateMessage(&msg);										
			DispatchMessage(&msg);
		}
	}

	// ѯ��ÿ��VCI����Ƿ���Թر�
	// ��ʾ����Ķ������ĵ�(Ŀǰֻ֧��OwmEdit)
	int i;
	for(i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		if(!theApp.m_aPlugIns[i].CanClose())
		{
			PLAT_LOADSTRING(strStopInterp, IDS_STOP_INTERP_FIRST);
			AfxMessageBox(strStopInterp);
			//AfxMessageBox("���������еĴ��ڻ������������ֹͣ�����У�");
			return;
		}
	}

	// �ر����в���Ҫ����λ�õ��������
	for(i=0; i<m_aOutputCtrl.GetSize(); i++)
	{
		if(!m_aOutputCtrl[i].bSave)
		{
			m_paneManager.DestroyPane(m_aOutputCtrl[i].nID);
		}
		delete m_aOutputCtrl[i].pWnd;
	}

	// �ر�����VCI���������DockingPane(������λ�õĳ���)
	for(i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
		{
			if(PANE_ATTACH_VIEW == theApp.m_aPlugIns[i].m_arDockingPane[j].m_nAttachType)
			{
				// ������View��Ҫ���ͷ�,����MFC��������Ѿ�ɾ���Ĵ��ڷ����ͷ���Ϣ,�����˳�ʱ�쳣
				CPlugIn* pPlugIn = &(theApp.m_aPlugIns[i]);
				if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd)// &&
					//(theApp.m_aPlugIns[i].m_strType != "project"))
				{
					theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd->DestroyWindow();
				}
			}
			if(!theApp.m_aPlugIns[i].m_arDockingPane[j].m_bSave)
			{
				// ɾ��������λ�õ�Docking����
				m_paneManager.DestroyPane(theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId);
			}
		}
	}

	// �ر�����Tool������DockingPane(������λ�õĳ���)
	for(i=theApp.m_aTools.GetSize()-1; i>=0; i--)
	{
		for(int j=0; j<theApp.m_aTools[i].m_arDockingPane.GetSize(); j++)
		{
			if(PANE_ATTACH_VIEW == theApp.m_aTools[i].m_arDockingPane[j].m_nAttachType)
			{
				// ������View��Ҫ���ͷ�,����MFC��������Ѿ�ɾ���Ĵ��ڷ����ͷ���Ϣ,�����˳�ʱ�쳣
				if(theApp.m_aTools[i].m_arDockingPane[j].m_pWnd)
				{
					theApp.m_aTools[i].m_arDockingPane[j].m_pWnd->DestroyWindow();
				}
			}
			if(!theApp.m_aTools[i].m_arDockingPane[j].m_bSave)
			{
				// ɾ��������λ�õ�Docking����
				m_paneManager.DestroyPane(theApp.m_aTools[i].m_arDockingPane[j].m_nId);
			}
		}
	}

	// ɾ�����д�������չ�˵�
	for(i=m_arExtMenuInfo.GetSize()-1; i>=0; i--)
	{
		if(m_arExtMenuInfo[i].m_strMainMenu != "")
		{			
			// ���Ҳ�ɾ�������Ĳ˵�
			CXTPCommandBar* pMenuBar = GetCommandBars()->GetMenuBar();
			CXTPControls* pControls = pMenuBar->GetControls();
			CXTPControl* pControl = pControls->GetFirst();
			while(pControl)
			{
				if(pControl->GetCaption() == m_arExtMenuInfo[i].m_strMainMenu)
				{
					if(m_arExtMenuInfo[i].m_strSubMenu == "")
					{
						// ɾ�����˵���
						pControls->Remove(pControl);
						m_arExtMenuInfo.RemoveAt(i);
					}else
					{
						// ɾ��һ���Ӳ˵���
						CXTPCommandBar* pMenuBarSub = pControl->GetCommandBar();
						CXTPControls* pControlsSub = pMenuBarSub->GetControls();
						pControl = pControlsSub->GetFirst();
						while(pControl)
						{
							if(pControl->GetCaption() == m_arExtMenuInfo[i].m_strSubMenu)
							{
								pControlsSub->Remove(pControl);
								m_arExtMenuInfo.RemoveAt(i);
								break;
							}
							pControlsSub->GetNext(pControl);
						}
					}
					break;
				}
				pControls->GetNext(pControl);
			}
		}
	}

	//CXTPPropExchangeIniFile px(FALSE, 0, _T("Settings"));    // To serialize to ini file
	CXTPPropExchangeXMLNode px(FALSE, 0, _T("Settings"));      // To serialize to XML file

#ifdef _XTP_INCLUDE_CONTROLS
	CXTPPropExchangeSection pxTaskPanel(px.GetSection(_T("TaskPanel")));
//	m_wndTaskPanel.GetGroups()->DoPropExchange(&pxTaskPanel);
#endif

	CXTPPropExchangeSection pxCommandBars(px.GetSection(_T("CommandBars")));
	XTP_COMMANDBARS_PROPEXCHANGE_PARAM param;
	param.bSerializeControls = TRUE;
	param.bSaveOriginalControls = FALSE;
	GetCommandBars()->DoPropExchange(&pxCommandBars, &param);

	CXTPPropExchangeSection pxDockingPane(px.GetSection(_T("DockingPane")));
	
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.DoPropExchange(&pxDockingPane);

	px.SaveToFile(m_strIniFileName);
/*
    // Save the current state for toolbars and menus.
    SaveCommandBars(_T("CommandBars"));

    // Save the current state for docking panes.
    CXTPDockingPaneLayout layoutNormal(&m_paneManager);
    m_paneManager.GetLayout(&layoutNormal); 
    layoutNormal.Save(_T("NormalLayout"));
*/
	// ���������
	UpdatePropertyGridContent();
	m_bEnableUpdatePropertyGrid = FALSE;

	// �ͷ�������Pane
	//m_paneManager.ClosePane(IDR_PANE_PROPERTIES);

	// ����򿪵������ļ������ӵ��ļ�
	BOOL bAutoReloadDocument = theApp.GetProfileInt(REG_CONFIG_SUBKEY, REG_CONFIG_AUTO_RELOAD_DOCUMENT, TRUE);
	if(bAutoReloadDocument)
	{
		CString strFileWorkspace = theApp.GetPlatRootPath();
		strFileWorkspace += "conf\\workspace.session";
		SaveSessionFile(strFileWorkspace);
	}

	// �ͷ�����VCI���
	for(i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		theApp.m_aPlugIns[i].CloseVci();
	}
	theApp.m_aPlugIns.RemoveAll();

	// ��ΪPost����Ϣ���ȼ��Ƚϵ�,����м�����Щ��Ϣ����ʧ,ʹ�ڴ�й©
	// ����˳�֮ǰ���뽫��Ϣ������ʣ���м����Ϣȫ��������.
	if(::IsWindow(m_hWnd))	// ������ڴ�ӡԤ��״ֱ̬�ӹر���������,�����ж�m_hWnd����Ч��
	{
		PostMessage(WM_PLAT_COMCMD, 1, 1);	// ����һ����־��Ϣ����֤�����ѭ�����˳�
		MSG msg;
		while(::PeekMessage(&msg, m_hWnd, WM_PLAT_COMCMD, WM_PLAT_COMCMD, PM_REMOVE))
		{	// ѭ��ȡ��Ϣ������
			if((1==msg.wParam) && (1==msg.lParam))	// �ж��˳���־
				break;
			TranslateMessage(&msg);										
			DispatchMessage(&msg);
		}
	}

	// Save frame window size and position.
	m_wndPosition.SaveWindowPos(this);

	// ɾ������״̬������
	if(g_pSkinProgress !=NULL)
	{
		delete g_pSkinProgress;
		g_pSkinProgress = NULL;
	}

	if(::IsWindow(m_hWnd))
	{
		CMDIFrameWnd::OnClose();
	}
}

void CMainFrame::OnCustomize()
{
    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars != NULL)
    {
        // Instanciate the customize dialog object.
        CXTPCustomizeSheet dlg(pCommandBars);

		CXTPCustomizeKeyboardPage pageKeyboard(&dlg);
		dlg.AddPage(&pageKeyboard);
		pageKeyboard.AddCategories(IDR_MAINFRAME);

        // Add the options page to the customize dialog.
        CXTPCustomizeOptionsPage pageOptions(&dlg);
        dlg.AddPage(&pageOptions);

        // Add the commands page to the customize dialog.
        CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
        pCommands->AddCategories(IDR_MAINFRAME);

        // Use the command bar manager to initialize the 
        // customize dialog.
        pCommands->InsertAllCommandsCategory();
        pCommands->InsertBuiltInMenus(IDR_MAINFRAME);
        pCommands->InsertNewMenuCategory();

        // Dispaly the dialog.
        dlg.DoModal();
    }
}

/////////////////////////////////////////////////////////////////////////////
// ��չ���߲˵��������(�˺�����ʱ������)
/////////////////////////////////////////////////////////////////////////////
void OnExtToolMenuProc(int nID) 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CExtMenuInfo* pMenuInfo = pMainFrame->GetExtMenuInfo(nID);
	if(pMenuInfo)
	{
		AfxMessageBox(pMenuInfo->m_strAction);
	}
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	CXTPMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CXTPMenuBar, lpCreateControl->pCommandBar);
	if (pMenuBar)
	{
		ASSERT(lpCreateControl->pMenu);
		CMenu* pPopupMenu = lpCreateControl->pMenu->GetSubMenu(lpCreateControl->nIndex);

		if ((lpCreateControl->strCaption == "����(&W)") ||
			(lpCreateControl->strCaption == _T("&Window")))		
		{
			CXTPControlPopup* pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlPopup);

			// Exclude only IDR_MAINFRAME template
			pControlPopup->ExcludeDocTemplate(IDR_MAINFRAME);

			pControlPopup->SetCommandBar(pPopupMenu);
			
			lpCreateControl->pControl = pControlPopup;
			return TRUE;
		}
		
		if ((lpCreateControl->strCaption == "�༭(&E)") ||
			(lpCreateControl->strCaption == _T("&Edit")))
		{
			CXTPControlPopup* pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlPopup);

			// Present only in IDR_BLUEUITYPE template
			pControlPopup->AssignDocTemplate(IDR_BLUEUITYPE);

			pControlPopup->SetCommandBar(pPopupMenu);
			
			lpCreateControl->pControl = pControlPopup;
			return TRUE;
		}
		/*
		// ��ӹ�����չ�˵�
		if ((lpCreateControl->strCaption == "����(&T)") ||
			(lpCreateControl->strCaption == _T("&Tools")))		
		{
			CExtMenuInfo extMenuInfo;
			extMenuInfo.m_strMainMenu = lpCreateControl->strCaption;
			extMenuInfo.pProc = (TYPEOF_ClientMenu*)OnExtToolMenuProc;
			extMenuInfo.pUpdateProc = NULL;
			extMenuInfo.nProcType = MENU_PROC_TYPE_PARAM_ID;
			
			int nExMenuId = IDM_OWMCMD+4000;
			pPopupMenu->AppendMenu(MF_SEPARATOR);
			pPopupMenu->AppendMenu(MF_STRING, nExMenuId, "hdfsdf");
			extMenuInfo.m_strAction = "hello";
			extMenuInfo.nId = nExMenuId-IDM_OWMCMD;
			m_arExtMenuInfo.Add(extMenuInfo);

			nExMenuId++;
			pPopupMenu->AppendMenu(MF_STRING, nExMenuId, "hsdfeeee2");
			extMenuInfo.m_strAction = "hello2";
			extMenuInfo.nId = nExMenuId-IDM_OWMCMD;
			m_arExtMenuInfo.Add(extMenuInfo);

			//return TRUE;
		}
		*/

		/*
		// �鿴����OWM��չ�˵�,����û����Ҫ��ӵĲ˵�
		for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_arMenu.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_arMenu[j].m_strMainMenu == lpCreateControl->strCaption)
				{
					CExtMenuInfo extMenuInfo;
					extMenuInfo.m_strMainMenu = lpCreateControl->strCaption;
					extMenuInfo.pProc = (TYPEOF_ClientMenu*)OnExtToolMenuProc;
					extMenuInfo.pUpdateProc = NULL;
					extMenuInfo.nProcType = MENU_PROC_TYPE_PARAM_ID;
					
					int nExMenuId = IDM_OWMCMD+g_nIdOwmExMenu;
					pPopupMenu->AppendMenu(MF_STRING, nExMenuId, theApp.m_aPlugIns[i].m_arMenu[j].m_strSubMenu);
					extMenuInfo.m_strAction = theApp.m_aPlugIns[i].m_arMenu[j].m_strAction;
					extMenuInfo.nId = nExMenuId-IDM_OWMCMD;
					m_arExtMenuInfo.Add(extMenuInfo);

					g_nIdOwmExMenu++;
				}
			}
		}
		*/

	}else
	if (lpCreateControl->bToolBar)
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
		if (!pToolBar)
			return FALSE;

		if (lpCreateControl->nID == ID_GOTO_URL && pToolBar->GetBarID() == IDR_EXPLORER)
		{
			CComboBoxURL* pComboAddress = new CComboBoxURL();
			pComboAddress->UpdateComboBox();
			pComboAddress->SetWidth(250);
			pComboAddress->SetDropDownListStyle();
			pComboAddress->EnableShellAutoComplete();

			m_pComboBoxURL = pComboAddress;
		
			lpCreateControl->pControl = pComboAddress;
			return TRUE;
		}
	}else
	if (lpCreateControl->nID == ID_EXTEND_TOOLS)
	{
		lpCreateControl->pControl = new CControlTools();
		return TRUE;
	}else
	if (lpCreateControl->nID == ID_EXPORT_FILES)
	{
		lpCreateControl->pControl = new CControlExports();
		return TRUE;
	}
	
	return FALSE;
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
    if (wParam == XTP_DPN_SHOWWINDOW)
    {
        CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
        
        if (!pPane->IsValid())
        {
			int nPaneID = pPane->GetID();
			
			if((nPaneID >= IDR_PANE_OUTPUT) && (nPaneID < (IDR_PANE_OUTPUT+10000)))
			{
				// �������Pane

				// ���ҿ���û���ĸ�OWM�д����˴˴���
				IVciOwm* pIVciOwm = NULL;
				BOOL bOwmDockSave = FALSE;
				for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
				{
					for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
					{
						if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId == nPaneID)
						{
							// �ҵ�һ��OWM,ִ�д�������
							CPlugIn* pPlugIn = &(theApp.m_aPlugIns[i]);
							if(!pPlugIn->IsVciOwm())
							{
								continue;
							}

							pIVciOwm = theApp.m_aPlugIns[i].GetIVciOwm();
							if(pIVciOwm == NULL)
							{
								// ���OWM�����δ����,���ȼ���
								theApp.m_aPlugIns[i].LoadOwmPlugIn();
								pIVciOwm = theApp.m_aPlugIns[i].GetIVciOwm();
							}
							if(pIVciOwm)
							{
								bOwmDockSave = theApp.m_aPlugIns[i].m_arDockingPane[j].m_bSave;
								break;
							}
						}
					}
					if(pIVciOwm) break;
				}

				COutputWndCtrl* pOutputCtrl = GetOutputCtrl(nPaneID);
				if(pOutputCtrl == NULL)
				{
					if(pIVciOwm)
					{
						pIVciOwm->OnShowDockingPane(nPaneID, this, "");
						pOutputCtrl = GetOutputCtrl(nPaneID);
						if(pOutputCtrl)
						{
							pOutputCtrl->bSave = bOwmDockSave;
						}
					}else
					{
						return FALSE;
					}
				}

				if (pOutputCtrl->pWnd->GetSafeHwnd() == 0)
				{
					try {
					//////////////////////
					if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
					{
						DWORD dwStyle = WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VSCROLL;
						((COutputCtrl*)(pOutputCtrl->pWnd))->Create(dwStyle, CRect(0,0,0,0), this, IDR_PANE_OUTPUT );
					}else
					if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
					{
						DWORD dwStyle = WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
						((CReportOutCtrl*)(pOutputCtrl->pWnd))->Create(dwStyle, CRect(0,0,0,0), this, IDR_PANE_OUTPUT );
					}
					if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
					{
						DWORD dwStyle = WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
						((CTreeOutCtrl*)(pOutputCtrl->pWnd))->Create(dwStyle, CRect(0,0,0,0), this, IDR_PANE_OUTPUT );
						((CTreeOutCtrl*)(pOutputCtrl->pWnd))->Init();
					}
					///////////////////////
					}catch(...)
					{
					}
				}
				// ��Skin�ĳ�ͻ����Ҫ�Ծ�������ж�
				HWND hSafeWnd = pOutputCtrl->pWnd->GetSafeHwnd();
				if((hSafeWnd != NULL) && (hSafeWnd != (HWND)0xFEEEFEEE) && (hSafeWnd != (HWND)0xCDCDCDCD))
				{
					pPane->Attach(pOutputCtrl->pWnd);
				}
				if(pIVciOwm == NULL)
				{
					// �������OWM�д�����������ڲ���Ҫ����Ĭ�ϵ�ͼ��
					m_paneManager.SetIcon(nPaneID, CXTPImageManagerIconHandle(theApp.LoadIcon(IDI_DISPLAY)));
				}
				pPane->SetTitle(pOutputCtrl->strTitle);
			}else
			{
				switch (nPaneID)
				{
				case IDR_PANE_PROPERTIES:
					{                           
						if (m_panePropertyGrid.GetSafeHwnd() == 0)
						{
							m_panePropertyGrid.Create(_T("STATIC"), NULL,
								WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
								CRect(0, 0, 0, 0), this, 0);
						}
						pPane->Attach(&m_panePropertyGrid);
						m_paneManager.SetIcon(nPaneID, CXTPImageManagerIconHandle(theApp.LoadIcon(IDI_PROPERTY)));
						UpdatePropertyGridContent();
						break;
					}

				default:
					{
						// ���ҿ���û���ĸ�OWM�д����˴˴���
						BOOL bFind = FALSE;
						for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
						{
							for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
							{
								if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId == nPaneID)
								{
									// �ҵ�һ��OWM,ִ�д�������
									CPlugIn* pPlugIn = &(theApp.m_aPlugIns[i]);
									if(!pPlugIn->IsVciOwm())
									{
										continue;
									}

									IVciOwm* pIVciOwm = theApp.m_aPlugIns[i].GetIVciOwm();
									if(pIVciOwm == NULL)
									{
										// ���OWM�����δ����,���ȼ���
										theApp.m_aPlugIns[i].LoadOwmPlugIn();
										pIVciOwm = theApp.m_aPlugIns[i].GetIVciOwm();
									}
									if(pIVciOwm)
									{
										CWnd* pWnd = pIVciOwm->OnShowDockingPane(nPaneID, this, "");
										if(pWnd)
										{
											if(PANE_ATTACH_WND == theApp.m_aPlugIns[i].m_arDockingPane[j].m_nAttachType)
											{
												pPane->Attach(pWnd);
											}else
											{
												CWnd* pView = pPane->AttachView(this, (CRuntimeClass*)pWnd);
												if (pView && ::IsWindow(pView->m_hWnd))
												{
													pView->SendMessage(WM_INITIALUPDATE);
												}
												theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd = pView;
											}
											pPane->SetTitle(theApp.m_aPlugIns[i].m_arDockingPane[j].m_strName);
										}
										bFind = TRUE;
										break;
									}
								}
							}
							if(bFind) break;
						}

						// ���ҿ���û���ĸ�Project�д����˴˴���
						bFind = FALSE;
						for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
						{
							for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
							{
								if( (theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId == nPaneID) &&
									(theApp.m_aPlugIns[i].m_strType == "project") &&
									(theApp.m_aPlugIns[i].HasLoad()) &&
									(theApp.m_pIProject != NULL) &&									// ���̱����Ѿ���
									(theApp.m_aPlugIns[i].GetIProject() == theApp.m_pIProject) )	// �˲��Ϊ�򿪵Ĺ��̲��
								{
									// �ҵ�һ��Project,����Project��Ӧ��OWM,Ȼ�󴴽�DockingPane
									CPlugIn* pPlugIn = theApp.GetOWM(theApp.m_aPlugIns[i].m_arDockingPane[j].m_strOwmId);
									if(pPlugIn == NULL)
									{
										continue;
									}
									IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
									if(pIVciOwm == NULL)
									{
										// ���OWM�����δ����,���ȼ���
										pPlugIn->LoadOwmPlugIn();
										pIVciOwm = pPlugIn->GetIVciOwm();
									}
									if(pIVciOwm)
									{
										// ���ӵ���������,���ڴ���һЩ������Ϣ
										CWnd* pWnd = pIVciOwm->OnShowDockingPane(nPaneID, this, "");
										if(pWnd)
										{
											if(PANE_ATTACH_WND == theApp.m_aPlugIns[i].m_arDockingPane[j].m_nAttachType)
											{
												pPane->Attach(pWnd);
											}else
											{
												CWnd* pView = pPane->AttachView(this, (CRuntimeClass*)pWnd);
												if (pView && ::IsWindow(pView->m_hWnd))
												{
													pView->SendMessage(WM_INITIALUPDATE);
												}
												theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd = pView;
											}
											pPane->SetTitle(theApp.m_aPlugIns[i].m_arDockingPane[j].m_strName);
										}
										bFind = TRUE;
										// ���õ�ǰ�򿪵�Project�����OnShowDockingPane����,����ˢ��DockingPane
										// �ڴ˴�����������,Project��OnShowDockingPane��ʱ����
										//theApp.m_pIProject->OnShowDockingPane(nPaneID, this, "");
										// ����DockingPane��ˢ�½ű�
										CString strTmp;
										strTmp.Format("dockingpane\\pane_%d", nPaneID);
										theApp.ExecProjectScript(strTmp);
										break;
									}
								}
							}
							if(bFind) break;
						}

						// ���ҿ���û���ĸ�Tool�д����˴˴���
						bFind = FALSE;
						for(int i=0; i<theApp.m_aTools.GetSize(); i++)
						{
							for(int j=0; j<theApp.m_aTools[i].m_arDockingPane.GetSize(); j++)
							{
								if(theApp.m_aTools[i].m_arDockingPane[j].m_nId == nPaneID)
								{
									// �ҵ�һ��Tool,����Tool��Ӧ��OWM,Ȼ�󴴽�DockingPane
									CPlugIn* pPlugIn = theApp.GetOWM(theApp.m_aTools[i].m_strVciId);
									if(pPlugIn == NULL)
									{
										continue;
									}
									IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
									if(pIVciOwm == NULL)
									{
										// ���OWM�����δ����,���ȼ���
										pPlugIn->LoadOwmPlugIn();
										pIVciOwm = pPlugIn->GetIVciOwm();
									}
									if(pIVciOwm)
									{
										// ���ӵ���������,���ڴ���һЩ������Ϣ
										CWnd* pWnd = pIVciOwm->OnShowDockingPane(nPaneID, this, theApp.m_aTools[i].m_strExecCommand);
										if(pWnd)
										{
											if(PANE_ATTACH_WND == theApp.m_aTools[i].m_arDockingPane[j].m_nAttachType)
											{
												pPane->Attach(pWnd);
											}else
											{
												CWnd* pView = pPane->AttachView(this, (CRuntimeClass*)pWnd);
												if (pView && ::IsWindow(pView->m_hWnd))
												{
													pView->SendMessage(WM_INITIALUPDATE);
												}
												theApp.m_aTools[i].m_arDockingPane[j].m_pWnd = pView;
											}
											pPane->SetTitle(theApp.m_aTools[i].m_arDockingPane[j].m_strName);
										}
										bFind = TRUE;
										break;
									}
								}
							}
							if(bFind) break;
						}

					}
				}
			}
        }
        return TRUE;
    }
    return FALSE;
}

LRESULT CMainFrame::OnTaskPanelNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam) {
	case XTP_TPN_CLICK:
		{
			CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)lParam;
			TRACE(_T("Click Event: pItem.Caption = %s, pItem.ID = %i\n"), pItem->GetCaption(), pItem->GetID());
			SendMessage(WM_COMMAND, pItem->GetID(), 0);
		}
		break;

	case XTP_TPN_RCLICK:
		{				
			CXTPTaskPanelItem* pItem = (CXTPTaskPanelItem*)lParam;
			TRACE(_T("RClick Event: pItem.Caption = %s, pItem.ID = %i\n"), pItem->GetCaption(), pItem->GetID());

		}
		break;
	}

	return 0;
}

// Popup control notify
LRESULT CMainFrame::OnPopUpNotify(WPARAM wParam, LPARAM lParam)
{
	// process notify from popup wnd
	if(wParam == XTP_PCN_ITEMCLICK)
	{
		CXTPPopupItem* pItem = (CXTPPopupItem*)lParam;
		ASSERT(pItem);

		theApp.m_wndPopupControl.Close();

		if(pItem->GetID() == ID_POPUP_GOTO_SITE1)
		{
			// �������������
			//::ShellExecute(NULL, _T("open"), theApp.m_infoPopupControl.strLinkTitle, NULL, NULL, SW_SHOW);
			theApp.RunPopupControlLink(theApp.m_infoPopupControl.strLinkTitle);
		}else
		if(pItem->GetID() == ID_POPUP_GOTO_SITE2)
		{
			// ����ı�����
			//::ShellExecute(NULL, _T("open"), theApp.m_infoPopupControl.strLinkText, NULL, NULL, SW_SHOW);
			theApp.RunPopupControlLink(theApp.m_infoPopupControl.strLinkText);
		}else
		if((pItem->GetID() >= ID_POPUP_BUTTON_CLICK) && (pItem->GetID() < (ID_POPUP_BUTTON_CLICK+5)))
		{
			// �����ť
			int nIndex = pItem->GetID() - ID_POPUP_BUTTON_CLICK;
			if((nIndex < theApp.m_infoPopupControl.asButtonScript.GetSize()) && (theApp.m_infoPopupControl.asButtonScript[nIndex].GetLength() > 0))
			{
				theApp.RunPopupControlLink(theApp.m_infoPopupControl.asButtonScript[nIndex]);
			}
		}
	}
	else if(wParam == XTP_PCN_STATECHANGED)
	{
		CXTPPopupControl* pCntrol = (CXTPPopupControl*)lParam;
		ASSERT(pCntrol);

		if(pCntrol->GetPopupState() == xtpPopupStateClosed)
		{
			//EnableItems(TRUE);
		}
	}
	return TRUE;
}

LRESULT CMainFrame::OnTabbarMouseMsg(WPARAM wParam,LPARAM lParam)
{
	if (wParam != WM_RBUTTONDOWN)
		return FALSE;

	CPoint point = CPoint((DWORD)lParam);
	CXTPTabManagerItem* pItem =  m_MTIClientWnd.HitTest(point);

	if (pItem)
	{
		CWnd* pFrame = CWnd::FromHandle(pItem->GetHandle());
		MDIActivate(pFrame);

		m_MTIClientWnd.Refresh();

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_TABS));
		CMenu* pPopup = menu.GetSubMenu(0);


		m_MTIClientWnd.WorkspaceToScreen(&point);

		CXTPCommandBars::TrackPopupMenu(pPopup, 0, point.x, point.y, this);

		m_MTIClientWnd.Refresh();

		return TRUE;
	}

	if (wParam == WM_RBUTTONDOWN)
	{
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_WORKSPACE));
		CMenu* pPopup = menu.GetSubMenu(0);

		m_MTIClientWnd.WorkspaceToScreen(&point);
		CXTPCommandBars::TrackPopupMenu(pPopup, TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
		
		m_MTIClientWnd.Refresh();
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��չ�˵���Ϣָ��
/////////////////////////////////////////////////////////////////////////////
CExtMenuInfo* CMainFrame::GetExtMenuInfo(UINT nID)
{
	for(int i=0; i<m_arExtMenuInfo.GetSize(); i++)
	{
		if(m_arExtMenuInfo[i].nId == nID)
		{
			return &(m_arExtMenuInfo[i]);
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ�˵��������(OWM�˵�)
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnOwmExCmd(UINT nID) 
{
	for(int i=0; i<m_arExtMenuInfo.GetSize(); i++)
	{
		if(m_arExtMenuInfo[i].nId == (int)(nID-IDM_OWMCMD))
		{
			if(m_arExtMenuInfo[i].pProc)
			{
				if(m_arExtMenuInfo[i].nProcType == MENU_PROC_TYPE_NORMAL)
				{
					m_arExtMenuInfo[i].pProc();
				}else
				if(m_arExtMenuInfo[i].nProcType == MENU_PROC_TYPE_PARAM_ID)
				{
					TYPEOF_ClientMenuParamId* ProcMenu = (TYPEOF_ClientMenuParamId*)(m_arExtMenuInfo[i].pProc);
					ProcMenu(nID - IDM_OWMCMD);
				}
			}
			//TRACE("OnOwmExCmd: id=%d\n", nID);
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��չ�˵�����ˢ�º���(OWM�˵�)
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateOwmExCmd(CCmdUI* pCmdUI)
{
	UINT nID = pCmdUI->m_nID;
	int nOID = (nID - IDM_OWMCMD);

	for(int i=0; i<m_arExtMenuInfo.GetSize(); i++)
	{
		if(m_arExtMenuInfo[i].nId == nOID)
		{
			/*
			if(m_arExtMenuInfo[i].nGrade < theApp.m_CurUser.nUserRight)
			{	// Ȩ�޿���
				pCmdUI->Enable(FALSE);
				return;
			}
			*/
			if(m_arExtMenuInfo[i].pUpdateProc)
				m_arExtMenuInfo[i].pUpdateProc(pCmdUI);
			//TRACE("OnUpdateOwmExCmd: id=%d\n", nID);
			return;
		}
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////
// ƽ̨COM�ӿ���Ϣ
/////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnPlatComCmd(WPARAM wParam, LPARAM lParam)
{
	theApp.PlatComCmd(wParam, lParam);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ƽ̨OWM��Ϣ,ĳ����չģ��Ķ�̬ж������
/////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnPlatOWMDynClose(WPARAM wParam, LPARAM lParam)
{
	CPlugIn* pOWM = theApp.GetOWM(wParam);
	if(pOWM)
	{
		pOWM->CloseOwmFrame();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���ڿ���̴����ļ�����ע����Ϣ,�ڶ���ƽ̨����ͨ������Ϣ���ļ���
// ���ݸ���һ��ƽ̨����
/////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnCheckItsMe(WPARAM wParam, LPARAM lParam)
{
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = 1024;

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-9025-4A0CD86F6C9F");

	ShowWindow(SW_SHOW);

	if(CCommandLineInfo::FileOpen == wParam)
	{
		HANDLE hFileMapping = NULL;
		LPVOID pViewOfFile = NULL;

		// Create file mapping which can contain dwMemoryFileSize characters
		hFileMapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,           // system paging file
			NULL,                           // security attributes
			PAGE_READWRITE,                 // protection
			0,                              // high-order DWORD of size
			dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
			sMemoryFileName);               // name

		DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
				// this instance is not first (other instance created file mapping)

		if(! hFileMapping)
		{
			TRACE(_T("Creating of file mapping failed.\n"));
			return FALSE;
		}

        pViewOfFile = MapViewOfFile(
            hFileMapping,               // handle to file-mapping object
            FILE_MAP_ALL_ACCESS,        // desired access
            0,
            0,
            0);                         // map all file

        if(!pViewOfFile)
        {
            TRACE(_T("MapViewOfFile failed.\n"));
			CloseHandle(hFileMapping);
			return FALSE;
        }

		CString strFileNames;
		TCHAR s[dwMemoryFileSize];
		lstrcpy( s, (LPTSTR) pViewOfFile);
		strFileNames = s;

		UnmapViewOfFile(pViewOfFile);
		CloseHandle(hFileMapping);

		//theApp.ShellOpenFile(strFileName);
		// ���������в���,��Ҫ�Ƚ�����ÿһ���ļ���,��ѭ�����ô��ļ��ķ���
		CString strCommand = _T("");
		CStringArray asFile;
		int nPos = -1;
		while((nPos = strFileNames.Find("\"")) != -1)
		{
			strFileNames.Delete(0, nPos+1);
			int nPosEnd = strFileNames.Find("\"");
			if(nPosEnd != -1)
			{
				CString strFile = strFileNames.Left(nPosEnd);
				strFileNames.Delete(0, nPosEnd+1);
				if((strCommand == _T("")) && (strFile.Find(_T("CMD_") == 0)))
				{
					// �������������CMD_��ͷ,���Һ������������ID����
					strCommand = strFile;
				}else
				{
					asFile.Add(strFile);
				}
			}
		}

		if(strCommand != _T(""))
		{
			// ���������в���
			theApp.ShellCommand(strCommand, asFile);
		}else
		{
			// û�������в��������ʾҪ���ļ�
			for(int i=0; i<asFile.GetSize(); i++)
			{
				theApp.ShellOpenFile(asFile[i]);
			}
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	
	m_MTIClientWnd.Detach();	
}

void CMainFrame::OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) 
{
	// Toggle CommandBars
	GetCommandBars()->OnSetPreviewMode(bPreview);
	// Toggle Tab Client
	m_MTIClientWnd.ShowWorkspace(!bPreview);

	// Toggle Docking Panes.
	m_paneManager.OnSetPreviewMode(bPreview);
	
	CMDIFrameWnd::OnSetPreviewMode(bPreview, pState);
}

/////////////////////////////////////////////////////////////////////
// ������ڲ���
// 0-999 Ϊ����ID,����ָ��ID��ʽ��������
// 1000����Ϊ�Զ������õ�ID
static int g_nOutputID = IDR_PANE_OUTPUT+1000;

/////////////////////////////////////////////////////////////////////////////
// ��ȡ������ڽṹ��ͨ������
/////////////////////////////////////////////////////////////////////////////
COutputWndCtrl* CMainFrame::GetOutputCtrl(LPCTSTR lpszTitle)
{
	for(int i=0; i<m_aOutputCtrl.GetSize(); i++)
	{
		if(m_aOutputCtrl[i].strTitle == lpszTitle)
			return &(m_aOutputCtrl[i]);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ������ڽṹ��ͨ��ID
/////////////////////////////////////////////////////////////////////////////
COutputWndCtrl* CMainFrame::GetOutputCtrl(int nID)
{
	for(int i=0; i<m_aOutputCtrl.GetSize(); i++)
	{
		if(m_aOutputCtrl[i].nID == nID)
			return &(m_aOutputCtrl[i]);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ��ָ��������ڵĽṹ
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::DeleteOutputCtrl(int nID)
{
	for(int i=0; i<m_aOutputCtrl.GetSize(); i++)
	{
		if(m_aOutputCtrl[i].nID == nID)
		{			
			if(m_aOutputCtrl[i].pWnd)
			{
				delete m_aOutputCtrl[i].pWnd;
			}
			m_aOutputCtrl.RemoveAt(i);
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// �����������
/////////////////////////////////////////////////////////////////////
COutputWndCtrl* CMainFrame::CreateOutputWnd(CString strTitle, int nType, int nId, int nAttachPane)
{
	COutputWndCtrl* pOutputCtrl = GetOutputCtrl(strTitle);
	if(pOutputCtrl)
	{
		if(m_paneManager.FindPane(pOutputCtrl->nID) != NULL)
		{
			m_paneManager.ShowPane(pOutputCtrl->nID);
		}
		return pOutputCtrl;
	}

	COutputWndCtrl outputCtrl;
	if(nId == -1)
	{
		outputCtrl.nID = g_nOutputID++;
	}else
	{
		outputCtrl.nID = IDR_PANE_OUTPUT+nId;
	}

	if(strTitle == "")
	{
		// ����Ϊ��,�����Ƿ�ĳ������д����Ĵ���,�ҵ���ʹ��
		BOOL bFind = FALSE;
		for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId == outputCtrl.nID)
				{
					strTitle = theApp.m_aPlugIns[i].m_arDockingPane[j].m_strName;
					bFind = TRUE;
					break;
				}
			}
			if(bFind) break;
		}
		if(!bFind)
		{
			// �����Ƿ�ĳ��Tool�д����Ĵ���
			for(int i=0; i<theApp.m_aTools.GetSize(); i++)
			{
				for(int j=0; j<theApp.m_aTools[i].m_arDockingPane.GetSize(); j++)
				{
					if(theApp.m_aTools[i].m_arDockingPane[j].m_nId == outputCtrl.nID)
					{
						strTitle = theApp.m_aTools[i].m_arDockingPane[j].m_strName;
						bFind = TRUE;
						break;
					}
				}
				if(bFind) break;
			}
		}
	}

	outputCtrl.strTitle = strTitle;
	outputCtrl.nCtrlType = nType;
	outputCtrl.bSave = FALSE;

	if(nType == OUTPUT_RICH)
	{
		COutputCtrl* pRunCtrl = new COutputCtrl;
		outputCtrl.pWnd = pRunCtrl;
	}else
	if(nType == OUTPUT_GRID)
	{
		CReportOutCtrl* pRunCtrl = new CReportOutCtrl;
		outputCtrl.pWnd = pRunCtrl;
	}else
	if(nType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pRunCtrl = new CTreeOutCtrl;
		outputCtrl.pWnd = pRunCtrl;
	}else
	{
		return NULL;
	}
	m_aOutputCtrl.Add(outputCtrl);

	if(m_paneManager.FindPane(outputCtrl.nID) != NULL)
	{
		m_paneManager.ShowPane(outputCtrl.nID);
	}

	int nFirstId = m_aOutputCtrl[0].nID;

	if(outputCtrl.nID == nFirstId)//IDR_PANE_OUTPUT)
	{
		m_paneManager.CreatePane(outputCtrl.nID, CRect(0, 0,200, 120), xtpPaneDockBottom);
	}else
	if(nAttachPane != 0)
	{
		// ������ָ����Pane����
		CXTPDockingPane* pBasePane = m_paneManager.FindPane(nAttachPane);
		if(pBasePane == NULL)
		{
			// ���û�о��õ�һ������
			pBasePane = m_paneManager.FindPane(nFirstId);
		}
		CXTPDockingPane* pPane = m_paneManager.CreatePane(
			outputCtrl.nID, CRect(0, 0,300, 120), xtpPaneDockRight, pBasePane);
		if((nAttachPane != -1) && pBasePane && !pBasePane->IsClosed())
		{
			// ���nAttachPane==-1��ʾ����Ҫ������ĳ�����ں���
			m_paneManager.AttachPane(pPane, pBasePane);
		}
	}else
	{
		CXTPDockingPane* pBasePane = m_paneManager.FindPane(nFirstId);//IDR_PANE_OUTPUT);
		CXTPDockingPane* pPane = m_paneManager.FindPane(outputCtrl.nID);
		if(pPane != NULL)
		{
			m_paneManager.ShowPane(outputCtrl.nID);
		}else
		{
			pPane = m_paneManager.CreatePane(
				outputCtrl.nID, CRect(0, 0,200, 120), xtpPaneDockBottom, pBasePane);
			if(pBasePane && !pBasePane->IsClosed())
			{
				m_paneManager.AttachPane(pPane, pBasePane);
			}
		}
	}

	return GetOutputCtrl(outputCtrl.nID);	
}

/////////////////////////////////////////////////////////////////////
// ����DockingBar
// nAttachPane : �����Ϊ0��ʾ�����ڴ�Pane֮��
/////////////////////////////////////////////////////////////////////
CXTPDockingPane* CMainFrame::CreateDockingBar(int nID, int nDirection, HICON hIcon, int nAttachPane)
{
	CXTPDockingPane* pPane = m_paneManager.FindPane(nID);
	if(pPane)
	{
		// �Ѿ���������,ֻ����ʾ����
		m_paneManager.ShowPane(nID);
		if(hIcon != NULL)
		{
			m_paneManager.SetIcon(nID, CXTPImageManagerIconHandle(hIcon));
		}
		return pPane;
	}

	pPane = m_paneManager.CreatePane(
			nID, CRect(0, 0,200, 120), (XTPDockingPaneDirection)nDirection);

	if(nAttachPane != 0)
	{
		CXTPDockingPane* pBasePane = m_paneManager.FindPane(nAttachPane);
		if(pBasePane && !pBasePane->IsClosed())
		{
			m_paneManager.AttachPane(pPane, pBasePane);
		}
	}

	// Set the icons for the docking pane tabs.
	if(hIcon != NULL)
	{
		m_paneManager.SetIcon(nID, CXTPImageManagerIconHandle(hIcon));
	}

	return pPane;	
}

/////////////////////////////////////////////////////////////////////
// ��ȡDockingPane�����Ĵ��ڶ���
/////////////////////////////////////////////////////////////////////
CWnd* CMainFrame::GetDockingPaneWnd(int nPaneID)
{
	CXTPDockingPane* pPane = m_paneManager.FindPane(nPaneID);
	if(!pPane)
	{
		return NULL;
	}
	return pPane->GetChild();
}

/////////////////////////////////////////////////////////////////////
// ��ȡDockingPane ID(��������)
/////////////////////////////////////////////////////////////////////
int CMainFrame::GetDockingPaneIdByName(CString strName)
{
	POSITION pos = m_paneManager.GetPaneList().GetHeadPosition();
	while(pos)
	{
		CXTPDockingPane* pPane = m_paneManager.GetPaneList().GetNext(pos).pPane;
		if(pPane && (pPane->GetTitle() == strName))
		{
			return pPane->GetID();
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// ����DockingPane����
/////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ActiveDockingPane(int nPaneID, BOOL bShow)
{
	CXTPDockingPane* pPane = m_paneManager.FindPane(nPaneID);
	if(!pPane)
	{
		return FALSE;
	}

	if(!m_paneManager.IsPaneClosed(nPaneID))
	{
		// bShow������ʾ����ʾ���ǹر����Pane
		if(bShow)
		{
			m_paneManager.ShowPane(nPaneID);
		}else
		{
			m_paneManager.ClosePane(nPaneID);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// �ر�DockingPane����
/////////////////////////////////////////////////////////////////////
BOOL CMainFrame::CloseDockingPane(int nPaneID)
{
	CXTPDockingPane* pPane = m_paneManager.FindPane(nPaneID);
	if(!pPane)
	{
		return FALSE;
	}

	// ����Plugins�е�DockingPane
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
		{
			if(nPaneID == theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId)
			{
				if(PANE_ATTACH_VIEW == theApp.m_aPlugIns[i].m_arDockingPane[j].m_nAttachType)
				{
					// ������View��Ҫ���ͷ�,����MFC��������Ѿ�ɾ���Ĵ��ڷ����ͷ���Ϣ,�����˳�ʱ�쳣
					if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd)
					{
						theApp.m_aPlugIns[i].m_arDockingPane[j].m_pWnd->DestroyWindow();
					}
				}
				theApp.m_aPlugIns[i].m_arDockingPane.RemoveAt(j);
				m_paneManager.DestroyPane(nPaneID);
				return TRUE;
			}
		}
	}

	// ����Tools�е�DockingPane
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		for(int j=0; j<theApp.m_aTools[i].m_arDockingPane.GetSize(); j++)
		{
			if(nPaneID == theApp.m_aTools[i].m_arDockingPane[j].m_nId)
			{
				if(PANE_ATTACH_VIEW == theApp.m_aTools[i].m_arDockingPane[j].m_nAttachType)
				{
					// ������View��Ҫ���ͷ�,����MFC��������Ѿ�ɾ���Ĵ��ڷ����ͷ���Ϣ,�����˳�ʱ�쳣
					if(theApp.m_aTools[i].m_arDockingPane[j].m_pWnd)
					{
						theApp.m_aTools[i].m_arDockingPane[j].m_pWnd->DestroyWindow();
					}
				}
				theApp.m_aTools[i].m_arDockingPane.RemoveAt(j);
				m_paneManager.DestroyPane(nPaneID);
				return TRUE;
			}
		}
	}

	m_paneManager.DestroyPane(nPaneID);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ˢ�����Դ�������
/////////////////////////////////////////////////////////////////////
void CMainFrame::UpdatePropertyGridContent()
{
	CXTPPropertyGrid& wndPropertyGrid = GetPropertyGrid(this);
	if (!wndPropertyGrid.m_hWnd) return;

	if (m_bPropInUpdate)
		return;

	m_bPropInUpdate = TRUE;

	wndPropertyGrid.BeginUpdate(m_stateExpanding);
/*
	CXTPPropertyGridItem* pCategoryObjects = wndPropertyGrid.AddCategory(IDR_PANE_PROPERTIES);
	CXTPPropertyGridItem* pItemObjectsCount = pCategoryObjects->AddChildItem(new CXTPPropertyGridItemNumber(_T("Objects Count"), 0));
	pItemObjectsCount->SetReadOnly(TRUE);
	pCategoryObjects->Expand();
*/
/*
	CFrameWnd* pFrame = MDIGetActive();
	if (!pFrame)
	{
		wndPropertyGrid.EndUpdate(m_stateExpanding);
		m_bPropInUpdate = FALSE;
		return;
	}
*/
/*
	CDrawObjList* pObjects = &pView->m_selection;

	CXTPPropertyGridItem* pCategoryObjects = wndPropertyGrid.AddCategory(ID_CATEGORY_OBJECTS);
	CXTPPropertyGridItem* pItemObjectsCount = pCategoryObjects->AddChildItem(new CXTPPropertyGridItemNumber(_T("Objects Count"), (long)pView->GetDocument()->GetObjects()->GetCount()));
	pItemObjectsCount->SetReadOnly(TRUE);
	pCategoryObjects->Expand();


	CXTPPropertyGridItem* pCategorySelected = wndPropertyGrid.AddCategory(ID_CATEGORY_SELECTED);

	CXTPPropertyGridItem* pItemSelectedCount =  pCategorySelected->AddChildItem(new CXTPPropertyGridItemNumber(_T("Selected Count"), (long)pObjects->GetCount()));
	pItemSelectedCount->SetReadOnly(TRUE);
	if (pObjects->GetCount() > 0)
	{
		CDrawObj* pSelected = pObjects->GetHead();

		pCategorySelected->AddChildItem(new CXTPPropertyGridItemColor(ID_ITEM_LINECOLOR, pSelected->m_logpen.lopnColor));
		pCategorySelected->AddChildItem(new CXTPPropertyGridItemColor(ID_ITEM_FILLCOLOR, pSelected->m_logbrush.lbColor));

		if (pObjects->GetCount() == 1)
		{

			pCategorySelected->AddChildItem(new CXTPPropertyGridItemBool(ID_ITEM_SOLID, pSelected->m_bBrush));

			int nPenSize = pSelected->m_bPen ? pSelected->m_logpen.lopnWidth.x : 0;
			pCategorySelected->AddChildItem(new CXTPPropertyGridItemNumber(ID_ITEM_PENSIZE, nPenSize));
		}
	}

	pCategorySelected->Expand();

	if (pView->GetDocument()->m_pSummInfo)
	{

		CXTPPropertyGridItem* pCategorySummary = wndPropertyGrid.AddCategory(ID_CATEGORY_SUMMARY);



		pCategorySummary->AddChildItem(new CXTPPropertyGridItem(ID_ITEM_AUTHOR,
			pView->GetDocument()->m_pSummInfo->GetAuthor()));

		pCategorySummary->AddChildItem(new CXTPPropertyGridItem(ID_ITEM_TITLE,
			pView->GetDocument()->m_pSummInfo->GetTitle()));
		pCategorySummary->AddChildItem(new CXTPPropertyGridItem(ID_ITEM_SUBJECT,
			pView->GetDocument()->m_pSummInfo->GetSubject()));

		CXTPPropertyGridItem* pItemCreated =  pCategorySummary->AddChildItem(new CXTPPropertyGridItem(_T("Created"),
			pView->GetDocument()->m_pSummInfo->GetCreateDate()));
		pItemCreated->SetReadOnly(TRUE);

		pCategorySummary->Expand();

	}
*/
/*
	{
		wndPropertyGrid.GetVerbs()->Add(_T("Shape Properties"), 1);
		wndPropertyGrid.GetVerbs()->Add(_T("Paper Color"), 2);
	}
*/
	wndPropertyGrid.EndUpdate(m_stateExpanding);
	m_bPropInUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////
// ��ʼ���Դ���ˢ��
/////////////////////////////////////////////////////////////////////
void CMainFrame::BeginUpdatePropertyGridContent()
{
	CXTPPropertyGrid& wndPropertyGrid = GetPropertyGrid(this);
	if (!wndPropertyGrid.m_hWnd) return;

	if (m_bPropInUpdate)
		return;

	m_bPropInUpdate = TRUE;

	wndPropertyGrid.BeginUpdate(m_stateExpanding);
}

/////////////////////////////////////////////////////////////////////
// �������Դ���ˢ��
/////////////////////////////////////////////////////////////////////
void CMainFrame::EndUpdatePropertyGridContent()
{
	CXTPPropertyGrid& wndPropertyGrid = GetPropertyGrid(this);
	if (!wndPropertyGrid.m_hWnd) return;

	if (!m_bPropInUpdate)
		return;

	wndPropertyGrid.EndUpdate(m_stateExpanding);
	m_bPropInUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////
// ���Դ�������֪ͨ
/////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGrid& wndPropertyGrid = GetPropertyGrid(this);
	if (!wndPropertyGrid.m_hWnd) return FALSE;

	if (wParam == XTP_PGN_VERB_CLICK)
	{
		// Verb������¼�֪ͨ
		CXTPPropertyGridVerb* pVerb = (CXTPPropertyGridVerb*)lParam;
		if(pVerb->GetID() > 10000)
		{
			// ����10000��ʾ��һ���˵�����
			OnCommand(pVerb->GetID(), 0);
		}else
		{
		}
		//if (pVerb->GetID() == 1) OnCommand(ID_VIEW_PROPERTIES, 0);
		//if (pVerb->GetID() == 2) OnCommand(ID_VIEW_PAPERCOLOR, 0);
		return TRUE;
	}
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

		CChildFrame* pFrame = (CChildFrame*)((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
		if(pFrame)
		{
			CView* pActiveView = pFrame->GetActiveView();
			if(pActiveView)
			{
				pActiveView->SendMessage(XTPWM_PROPERTYGRID_NOTIFY, XTP_PGN_ITEMVALUE_CHANGED, lParam);
			}
		}
		return TRUE;
/*
		switch (pItem->GetID())
		{
			
			case ID_ITEM_AUTHOR: m_pView->GetDocument()->m_pSummInfo->SetAuthor(pItem->GetValue()); break;
			case ID_ITEM_SUBJECT: m_pView->GetDocument()->m_pSummInfo->SetSubject(pItem->GetValue()); break;
			case ID_ITEM_TITLE: m_pView->GetDocument()->m_pSummInfo->SetTitle(pItem->GetValue()); break;
			case ID_ITEM_PENSIZE:
				{
					CDrawObj* pSelected = m_pView->m_selection.GetHead();

					int nPenSize = min(100, ((CXTPPropertyGridItemNumber*)pItem)->GetNumber());
					if (nPenSize > 0)
					{
						pSelected->m_logpen.lopnWidth.x = nPenSize;
						pSelected->m_logpen.lopnWidth.y = nPenSize;
					}
					pSelected->Invalidate();
					pSelected->m_pDocument->SetModifiedFlag();
				}
				break;
			case ID_ITEM_SOLID:
				{
					CDrawObj* pSelected = m_pView->m_selection.GetHead();

					BOOL bSolid = min(100, ((CXTPPropertyGridItemBool*)pItem)->GetBool());
					pSelected->m_bBrush = bSolid;
					pSelected->Invalidate();
					pSelected->m_pDocument->SetModifiedFlag();
				}
				break;

			case ID_ITEM_LINECOLOR:
				{
					COLORREF color = ((CXTPPropertyGridItemColor*)pItem)->GetColor();

					POSITION pos = m_pView->m_selection.GetHeadPosition();
					while (pos != NULL)
					{
						CDrawObj* pObj = m_pView->m_selection.GetNext(pos);
						pObj->SetLineColor(color);
					}
				}
				break;
			case ID_ITEM_FILLCOLOR:
				{
					COLORREF color = ((CXTPPropertyGridItemColor*)pItem)->GetColor();

					POSITION pos = m_pView->m_selection.GetHeadPosition();
					while (pos != NULL)
					{
						CDrawObj* pObj = m_pView->m_selection.GetNext(pos);
						pObj->SetFillColor(color);
					}
				}
				break;

			default:
				TRACE(_T("Value Changed. Caption = %s, ID = %i, Value = %s\n"), pItem->GetCaption(), pItem->GetID(), pItem->GetValue());

		}
*/
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// ��ʾ�������
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewOutput() 
{
	if(m_aOutputCtrl.GetSize() > 0)
	{
		int nFirstId = m_aOutputCtrl[0].nID;
		m_paneManager.ShowPane(nFirstId);//IDR_PANE_OUTPUT);
		m_paneManager.ShowPane(nFirstId+1);
		m_paneManager.ShowPane(nFirstId+2);
	}
}

/////////////////////////////////////////////////////////////////////
// ��ʾ���Դ���
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewProperties() 
{
	m_paneManager.ShowPane(IDR_PANE_PROPERTIES);	
}
/*
/////////////////////////////////////////////////////////////////////
// ��ʾ�����滻����
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnFindandreplaceFindinfiles() 
{
	CPlugIn* pPlugIn = theApp.GetOWM("org.owm.grep");
	if(pPlugIn)
	{
		pPlugIn->LoadDockingPane(10002);
	}
}
*/

void CommonNavigate(CMDIChildWnd* pFrame, CString& strUrl)
{
	if (pFrame == NULL)
	{
		//COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strUrl, 0);
		theApp.OpenHtmlWindow(strUrl);
	}
	else
	{
		//COM_CMD(VCIID_HTMLVIEW, OC_VIEWHTMLPAGE, (WPARAM)(LPCTSTR)strUrl, (int)(pFrame->GetActiveView()));
		theApp.OpenHtmlWindow(strUrl, TRUE, TRUE);
	}

}

/////////////////////////////////////////////////////////////////////
// ����ҳ
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnGotoUrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	
	CXTPControlComboBox* pCombo = (CXTPControlComboBox*)tagNMCONTROL->pControl;
	ASSERT(pCombo);
	ASSERT(pCombo->GetType() == xtpControlComboBox);

	CString strUrl = pCombo->GetEditText();
	
	strUrl.TrimLeft();
	
	if (strUrl.IsEmpty())
		return;

	int nIndex = strUrl.Find(' ');
	if (nIndex != -1)
	{
		
		CString strKeyword = strUrl.Left(nIndex);/*
		CString strSearch = strUrl.Mid(nIndex + 1);
		strSearch.TrimLeft();
		for (int i = 0; i < CPageSearch::m_arrEngines.GetSize(); i++)
		{
			CPageSearch::ENGINE& engine = CPageSearch::m_arrEngines[i];
			if (engine.strKeyword.CompareNoCase(strKeyword) == 0)
			{
				strUrl = engine.strSearch;
				strUrl.Replace(_T("%s"), strSearch);
				CommonNavigate(MDIGetActive(), strUrl);
				pCombo->SetEditText(strUrl);
				return;
			} 			
		}*/
	}

	nIndex = strUrl.Find(':');
	if(nIndex >= 0) strUrl.Replace('\\','/');
	else strUrl = _T("http://") + strUrl;

	CommonNavigate(MDIGetActive(), strUrl);
	
	if (strUrl.Right(1) == "/")
		strUrl.Delete(strUrl.GetLength() - 1);

	if (pCombo->FindStringExact(0, strUrl) == CB_ERR)
		pCombo->InsertString(0, strUrl);

	*pResult = 1;
}

void CMainFrame::OnUpdateGotoUrl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

/////////////////////////////////////////////////////////////////////
// ˢ����ҳ
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnRefreshGotoUrl() 
{
	if(m_pComboBoxURL == NULL)
		return;

	CXTPControlComboBox* pCombo = m_pComboBoxURL;
	ASSERT(pCombo->GetType() == xtpControlComboBox);

	CString strUrl = pCombo->GetEditText();
	
	strUrl.TrimLeft();
	
	if (strUrl.IsEmpty())
		return;

	int nIndex = strUrl.Find(' ');
	if (nIndex != -1)
	{
		CString strKeyword = strUrl.Left(nIndex);
	}

	nIndex = strUrl.Find(':');
	if(nIndex >= 0) strUrl.Replace('\\','/');
	else strUrl = _T("http://") + strUrl;

	CommonNavigate(MDIGetActive(), strUrl);
	
	if (strUrl.Right(1) == "/")
		strUrl.Delete(strUrl.GetLength() - 1);

	if (pCombo->FindStringExact(0, strUrl) == CB_ERR)
		pCombo->InsertString(0, strUrl);
}

void CMainFrame::OnUpdateRefreshGotoUrl(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////
// ������ҳ����
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileNewBlank() 
{
	theApp.OpenHtmlWindow("about:blank");
	CXTPControlComboBox* pCombo = (CXTPControlComboBox*)(GetCommandBars()->FindControl(xtpControlComboBox, ID_GOTO_URL, FALSE, FALSE));
	if (pCombo) pCombo->GetEditCtrl()->SetFocus();
}

/////////////////////////////////////////////////////////////////////
// �л�Tabҳ��λ��
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnToggleDock(void)
{
	if (m_MTIClientWnd.GetPaintManager()->GetPosition() != xtpTabPositionTop)
		m_MTIClientWnd.GetPaintManager()->SetPosition(xtpTabPositionTop);
	else
		m_MTIClientWnd.GetPaintManager()->SetPosition(xtpTabPositionBottom);

	m_MTIClientWnd.SaveState();
	RecalcLayout(TRUE);

}

/////////////////////////////////////////////////////////////////////
// �ر����д���
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnWindowCloseAll() 
{
//	if (AfxMessageBox(IDS_CONFIRM_CLOSEALL,  MB_YESNO | MB_ICONQUESTION ) != IDYES)
//		return;
//	AfxGetApp()->CloseAllDocuments(FALSE);

	// ѯ��ÿ��VCI����Ƿ���Թر�
	// ��ʾ����Ķ������ĵ�(Ŀǰֻ֧��OwmEdit)
	for(int i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		if(!theApp.m_aPlugIns[i].CanClose())
		{
			PLAT_LOADSTRING(strStopInterp, IDS_STOP_INTERP_FIRST);
			AfxMessageBox(strStopInterp);
			//AfxMessageBox("���������еĴ��ڻ������������ֹͣ�����У�");
			return;
		}
	}

	// �ر�����OWM�������
	for(int i=theApp.m_aPlugIns.GetSize()-1; i>=0; i--)
	{
		if(theApp.m_aPlugIns[i].IsVciOwm() && theApp.m_aPlugIns[i].m_pDocTemplate)
		{
			theApp.m_aPlugIns[i].m_pDocTemplate->CloseAllDocuments(FALSE);
		}
	}
}

void CMainFrame::OnUpdateWindowCloseAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((CChildFrame*)MDIGetActive() != NULL);
}

/////////////////////////////////////////////////////////////////////
// ��ʱ��
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		// ������ʱ��
		theApp.RunTimerTask();
	}

	CMDIFrameWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////
// ��չ���߲˵�����¼�
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnToolClick(UINT nID)
{
	int nIndex = nID - FIRST_TOOL_COMMAND;

	if (nIndex >= 0 && nIndex < theApp.m_aTools.GetSize())
	{
		for(int i=0; i<theApp.m_aTools.GetSize(); i++)
		{
			if(theApp.m_aTools[i].m_nMenuIndex == nIndex)
			{
				theApp.m_aTools[i].Run();
				return;
			}
		}		
	}
}

/////////////////////////////////////////////////////////////////////
// �����˵�����¼�
/////////////////////////////////////////////////////////////////////
void CMainFrame::OnExportClick(UINT nID)
{
	int nIndex = nID - FIRST_EXPORT_COMMAND;

	// ��ȡ��ǰ�ĵ��ĵ����˵��б�
	CPlugIn* pPlugIn = NULL;
	CDocument* pDoc = theApp.GetOwmDocumentFromFrame(MDIGetActive(), pPlugIn);
	if(pPlugIn == NULL)
	{
		return;
	}
	IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
	if(pIVciOwm == NULL)
	{
		return;
	}
	if(pDoc == NULL)
	{
		return;
	}

	pIVciOwm->ExportDocument(pDoc, "", nIndex);
}

CString GetValueFromSessionLine(CString strLine, CString strName)
{
	int nPos = strLine.Find(strName+"=");
	if(nPos != -1)
	{
		strLine.Delete(0, nPos+strName.GetLength()+1);
		int nPosEnd = strLine.Find(",");
		if(nPosEnd != -1)
		{
			strLine = strLine.Left(nPosEnd);
		}
		strLine.TrimLeft();
		strLine.TrimRight();
		return strLine;
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ���ػỰ�ļ�
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::LoadSessionFile(CString strSessionFile) 
{
	CStdioFile fSessionFile;
	if(fSessionFile.Open( strSessionFile, CFile::modeRead | CFile::typeText ))
	{
		CString strTemp;
		CDocument* pActiveDoc = NULL;
		CString strOwmId = "";
		CDocument* pDoc = NULL;
		CStringArray asSessionLine;
		while(fSessionFile.ReadString(strTemp))
		{
			if(strTemp.Find("[[[=OWMFILE=]]]") == 0)
			{
				if((strOwmId != "") && pDoc && (asSessionLine.GetSize() > 0))
				{
					CPlugIn* pPlugIn = theApp.GetOWM(strOwmId);
					if(pPlugIn)
					{
						IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
						if(pIVciOwm)
						{
							pIVciOwm->LoadDocumentSession(pDoc, asSessionLine);
						}
					}
				}
				asSessionLine.RemoveAll();

				strOwmId = GetValueFromSessionLine(strTemp, "owmid");
				CString strFile = GetValueFromSessionLine(strTemp, "file");
				BOOL bActiveDoc = (GetValueFromSessionLine(strTemp, "active") == "true");
				if((strOwmId != "") && (strFile != ""))
				{
					if(strOwmId == VCIID_HTMLVIEW)
					{
						pDoc = theApp.OpenHtmlWindow(strFile, FALSE);
					}else
					{
						pDoc = theApp.OpenDocumentFileWithOwm(strFile, strOwmId);
					}
					if(bActiveDoc)
					{
						pActiveDoc = pDoc;
					}
				}
			}else
			{
				asSessionLine.Add(strTemp);
			}
		}

		if((strOwmId != "") && pDoc && (asSessionLine.GetSize() > 0))
		{
			CPlugIn* pPlugIn = theApp.GetOWM(strOwmId);
			if(pPlugIn)
			{
				IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
				if(pIVciOwm)
				{
					pIVciOwm->LoadDocumentSession(pDoc, asSessionLine);
				}
			}
		}

		fSessionFile.Close();

		// ���õ�ǰ�������ͼ
		if(pActiveDoc)
		{
			POSITION posview = pActiveDoc->GetFirstViewPosition();
			if(posview)
			{
				CView* pView = pActiveDoc->GetNextView(posview);
				pView->GetParentFrame()->ActivateFrame();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����Ự�ļ�
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SaveSessionFile(CString strSessionFile) 
{
	CStdioFile fSessionFile;
	fSessionFile.Open( strSessionFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText );

	int nTabCount = m_MTIClientWnd.GetItemCount();
	for(int i=0; i<nTabCount; i++)
	{
		CXTPTabManagerItem* pItem = m_MTIClientWnd.GetItem(i);
		if(pItem)
		{
			CWnd* pFrame = CWnd::FromHandle(pItem->GetHandle());
			CPlugIn* pPlugIn = NULL;
			CDocument* pDoc = theApp.GetOwmDocumentFromFrame(pFrame, pPlugIn);
			if(pDoc)
			{
				IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
				if(pIVciOwm)
				{
					CString strDocPathName = pDoc->GetPathName();
					if(pPlugIn->m_strId == VCIID_HTMLVIEW)
					{
						POSITION posview = pDoc->GetFirstViewPosition();
						if(posview)
						{
							CView* pView = pDoc->GetNextView(posview);
							int nRet = 0;
							RCOM_CMD(VCIID_HTMLVIEW, OC_GETHTMLURL, 0, (int)pView, &nRet);
							if(nRet != 0)
							{
								strDocPathName = (LPCTSTR)nRet;
								if(strDocPathName.Find("file:///") == 0)
								{
									strDocPathName.Delete(0, 8);
									strDocPathName.Replace("/", "\\");
								}
							}
						}
						// ���˵�û���õ�����
						CString strPlatIndexFile = theApp.GetPlatRootPath() + "html\\index.htm";
						if( (strDocPathName.CompareNoCase(strPlatIndexFile) == 0) ||
							(strDocPathName.Find("pfe:open") != -1) )
						{
							strDocPathName = "";
						}
					}
					if(strDocPathName == "")
					{
						continue;
					}

					CString strTmp;
					strTmp.Format("[[[=OWMFILE=]]]owmid=%s,file=%s,%s\n",
						pPlugIn->m_strId, strDocPathName, (MDIGetActive() == pFrame) ? "active=true" : "");
					fSessionFile.WriteString(strTmp);

					CStringArray asSessionLine;
					pIVciOwm->SaveDocumentSession(pDoc, asSessionLine);
					for(int j=0; j<asSessionLine.GetSize(); j++)
					{
						strTmp.Format("%s\n", asSessionLine[j]);
						fSessionFile.WriteString(strTmp);
					}
				}
			}
		}
	}

	fSessionFile.Close();
}

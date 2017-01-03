////////////////////////////////////////////////////////////////////////////
//	File:		TaskDialog.cpp
//	Version:	1.0.0.0
//	Created:	2007-06-15
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Implementation of Task Dialog.
////////////////////////////////////////////////////////////////////////////
// TaskDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "TaskDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskDialog

/////////////////////////////////////////////////////////////////////////////
// ���沢���õ�ǰ·��
/////////////////////////////////////////////////////////////////////////////
void CTaskDialogEx::SaveAndSetCurPath(CString strURL)
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
void CTaskDialogEx::RestoreCurPath()
{
	// ִ�н���,�ָ���ǰ·��������
	if(m_dwPathLen > 0)
	{
		CString strOldPath = m_szOldPath;
		SetCurrentDirectory(strOldPath);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���нű�
/////////////////////////////////////////////////////////////////////////////
BOOL CTaskDialogEx::RunScript(CString strURL)
{
	if(strURL == "")
	{
		return FALSE;
	}

	if(m_pInfo->pInterp == NULL)
	{
		return FALSE;
	}

	int nPos = strURL.Find("<file>");
	if(nPos == 0)
	{
		strURL.Delete(0, 6);
		theApp.ProcessPath(strURL);

		// ���沢���õ�ǰ·��
		SaveAndSetCurPath(strURL);

		BOOL bRet = m_pInfo->pInterp->RunScriptFile(strURL);

		// �ָ���ǰ·��
		RestoreCurPath();

		return bRet;
	}else
	{
		return m_pInfo->pInterp->RunScriptCommand(strURL);
	}

	return TRUE;
}

void CTaskDialogEx::OnDialogConstructed()
{
	if(m_pInfo->bShowProgressBar)
	{
		SetProgressBarPos(m_pInfo->nProgressBarPos);
		SetProgressBarRange(m_pInfo->nProgressBarRangeMin, m_pInfo->nProgressBarRangeMax);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ť����¼�����
/////////////////////////////////////////////////////////////////////////////
void CTaskDialogEx::OnButtonClicked(int nButtonID, BOOL& bCloseDialog)
{
	for(int i=0; i<m_pInfo->asButtonScript.GetSize(); i++)
	{
		if(m_pInfo->auButtonID.GetAt(i) == nButtonID)
		{
			CString strURL = m_pInfo->asButtonScript.GetAt(i);
			if(strURL.GetLength() > 0)
			{
				bCloseDialog = FALSE;
				RunScript(strURL);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �����¼�����
/////////////////////////////////////////////////////////////////////////////
void CTaskDialogEx::OnVerificationClicked(BOOL bChecked)
{
}

/////////////////////////////////////////////////////////////////////////////
// ��ʱ���¼�����
/////////////////////////////////////////////////////////////////////////////
void CTaskDialogEx::OnTimer(DWORD dwMilliSeconds, BOOL& bReset)
{
	if(m_pInfo->nTimerTimeout == 0)
	{
		return;
	}

	if(dwMilliSeconds > m_pInfo->nTimerTimeout)
	{
		RunScript(m_pInfo->strTimerScript);
		bReset = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// �������¼�����
/////////////////////////////////////////////////////////////////////////////
void CTaskDialogEx::OnHyperlinkClicked(LPCTSTR pszURL)
{
	RunScript(pszURL);
}

static CTaskDialogEx* g_pTaskDialog = NULL;	// ����TaskDialog�Ի���ָ��
static CTaskDialogInfoArray g_aTaskDialogInfo;	// TaskDialog���͵���ҳ����Ϣ����

/////////////////////////////////////////////////////////////////////////////
// ͨ������Ի���
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::DoTaskDialog(void* p)
{
	TTaskDialogInfo* pInfo = (TTaskDialogInfo*)p;
	CTaskDialogEx taskDialog(AfxGetMainWnd());
	g_pTaskDialog = &taskDialog;

	taskDialog.m_pInfo	= pInfo;

	// ���Ի��������������ֵ
	taskDialog.SetWindowTitle(pInfo->strWindowTitle);
	taskDialog.SetVerificationText(pInfo->strVerificationText);
	taskDialog.SetExpandedControlText(pInfo->strExpandedControlText);
	taskDialog.SetCollapsedControlText(pInfo->strCollapsedControlText);
	taskDialog.SetMainInstruction(pInfo->strMainInstruction);
	taskDialog.SetContent(pInfo->strContent);
	taskDialog.SetExpandedInformation(pInfo->strExpandedInformation);
	taskDialog.SetFooter(pInfo->strFooter);

	// Main Icon
	if(pInfo->strMainIcon == "Error")
	{
		taskDialog.SetMainIcon(TD_ERROR_ICON);
	}else
	if(pInfo->strMainIcon == "Warning")
	{
		taskDialog.SetMainIcon(TD_WARNING_ICON);
	}else
	if(pInfo->strMainIcon == "Information")
	{
		taskDialog.SetMainIcon(TD_INFORMATION_ICON);
	}else
	if(pInfo->strMainIcon == "Shield")
	{
		taskDialog.SetMainIcon(TD_SHIELD_ICON);
	}else
	if(pInfo->strMainIcon.GetLength() > 0)
	{
		HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strMainIcon, IMAGE_ICON,
			::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
		taskDialog.SetMainIcon(hIcon);
	}

	// Footer Icon
	if(pInfo->strFooterIcon == "Error")
	{
		taskDialog.SetFooterIcon(TD_ERROR_ICON);
	}else
	if(pInfo->strFooterIcon == "Warning")
	{
		taskDialog.SetFooterIcon(TD_WARNING_ICON);
	}else
	if(pInfo->strFooterIcon == "Information")
	{
		taskDialog.SetFooterIcon(TD_INFORMATION_ICON);
	}else
	if(pInfo->strFooterIcon == "Shield")
	{
		taskDialog.SetFooterIcon(TD_SHIELD_ICON);
	}else
	if(pInfo->strFooterIcon.GetLength() > 0)
	{
		HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strFooterIcon, IMAGE_ICON,
			::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
		taskDialog.SetFooterIcon(hIcon);
	}

	taskDialog.SetWidth(pInfo->nWidth);

	if(pInfo->bEnableInputBox)
	{
		taskDialog.AddInputBox();
		taskDialog.SetInputBoxText(pInfo->strInputBoxText);
	}

	int i;
	for(i=0; i<pInfo->asButtonText.GetSize(); i++)
	{
		taskDialog.AddButton(pInfo->asButtonText.GetAt(i), pInfo->auButtonID.GetAt(i));
	}
	for(i=0; i<pInfo->asRadioButtonText.GetSize(); i++)
	{
		taskDialog.AddRadioButton(pInfo->asRadioButtonText.GetAt(i), pInfo->auRadioButtonID.GetAt(i));
	}
	// ���û�ж���Button,��CommandLinks���Ա�������ΪFALSE,����Ի����޷���ʾ
	taskDialog.EnableCommandLinks((pInfo->asButtonText.GetSize() > 0) ? pInfo->bEnableCommandLinks : FALSE);
	taskDialog.EnableHyperlinks(pInfo->bEnableHyperlinks);
	taskDialog.EnableRelativePosition(pInfo->bEnableRelativePosition);
	taskDialog.ShowProgressBar(pInfo->bShowProgressBar, pInfo->bMarquee);
	taskDialog.ExpandedByDefault(pInfo->bExpandedByDefault);
	taskDialog.ExpandFooterArea(pInfo->bExpandFooterArea);
	taskDialog.SetDefaultButton(pInfo->nDefaultButton);
	taskDialog.SetDefaultRadioButton(pInfo->nDefaultRadioButton);
	taskDialog.NoDefaultRadioButton(pInfo->bNoDefaultRadioButton);
	taskDialog.SetVerifyCheckState(pInfo->bVerifyCheckState);
	taskDialog.EnableCallbackTimer(pInfo->bEnableCallbackTimer);
	taskDialog.SetCommonButtons(pInfo->bCommonButtonOk,
								pInfo->bCommonButtonYes,
								pInfo->bCommonButtonNo,
								pInfo->bCommonButtonCancel,
								pInfo->bCommonButtonRetry,
								pInfo->bCommonButtonClose);

	taskDialog.EnableMessageBoxStyle(TRUE);

	int nRet = taskDialog.DoModal();

	//if((nRet != IDCANCEL) && (nRet != IDNO))
	{	// �ش�����ֵ
		pInfo->nSelectedButtonId		= taskDialog.GetSelectedButtonId();
		pInfo->nSelectedRadioButtonId	= taskDialog.GetSelectedRadioButtonId();
		pInfo->bIsVerificiationChecked	= taskDialog.IsVerificiationChecked();

		if(pInfo->bEnableInputBox)
		{
			pInfo->strInputBoxText = taskDialog.GetInputBoxText();
		}
	}

	g_pTaskDialog = NULL;

	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// ��XML�����ز�ִ��TaskDialog
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::DoXmlTaskDialog(void* p)
{
	TTaskDialogInfo* pInfo = (TTaskDialogInfo*)p;
	CTaskDialogEx taskDialog(AfxGetMainWnd());
	g_pTaskDialog = &taskDialog;

	taskDialog.EnableMessageBoxStyle(TRUE);
	taskDialog.EnableMarkup(TRUE);

	taskDialog.m_pInfo	= pInfo;

	CXTPPropExchangeXMLNode px(TRUE, NULL, "Dialogs");
	if(!px.LoadFromString(pInfo->strContent))
	{
		return -1;
	}

	if (!px.OnBeforeExchange())
	{
		return -1;
	}

	px.SetCompactMode(TRUE);

	CString strDialogSection;
	// ����Ի���Section������,�Ͱ������ֲ���,�������Dialog�ڵ�
	if(pInfo->strWindowTitle != "")
	{
		strDialogSection.Format(_T("Dialog[@Name='%s']"), pInfo->strWindowTitle);
	}else
	{
		strDialogSection = "Dialog";
	}
	if (!px.IsSectionExists(strDialogSection))
	{
		return -1;
	}

	CXTPPropExchangeSection pxDialog(px.GetSection(strDialogSection));
	if (!pxDialog->OnBeforeExchange())
	{
		return -1;
	}

	if(pInfo->asReplace.GetSize() > 0)
	{
		// XML�д����滻�ַ���
		LPCTSTR* prglpsz = new LPCTSTR[pInfo->asReplace.GetSize()];
		int i;
		for(i = 0; i < pInfo->asReplace.GetSize(); i++)
		{
			prglpsz[i] = pInfo->asReplace[i];
		}
		taskDialog.CreateFromResourceParam((CXTPPropExchangeXMLNode*)&pxDialog, prglpsz, pInfo->asReplace.GetSize());
	}else
	{
		taskDialog.CreateFromResourceParam((CXTPPropExchangeXMLNode*)&pxDialog, NULL, 0);
	}

	if(pInfo->bEnableInputBox)
	{
		taskDialog.AddInputBox();
		taskDialog.SetInputBoxText(pInfo->strInputBoxText);
	}

	int nRet = taskDialog.DoModal();

	// �ش�����ֵ
	pInfo->nSelectedButtonId		= taskDialog.GetSelectedButtonId();
	pInfo->nSelectedRadioButtonId	= taskDialog.GetSelectedRadioButtonId();
	pInfo->bIsVerificiationChecked	= taskDialog.IsVerificiationChecked();

	if(pInfo->bEnableInputBox)
	{
		pInfo->strInputBoxText = taskDialog.GetInputBoxText();
	}

	g_pTaskDialog = NULL;

	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// ˢ��ͨ������Ի���
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::RefreshTaskDialog(void* p)
{
	TTaskDialogInfo* pInfo = (TTaskDialogInfo*)p;
	if(g_pTaskDialog == NULL)
	{
		return FALSE;
	}

	TTaskDialogInfo* gpInfo = g_pTaskDialog->m_pInfo;

	// ���Ի��������������ֵ
	if((gpInfo->strWindowTitle != pInfo->strWindowTitle) && (pInfo->strWindowTitle != ""))
	{
		g_pTaskDialog->SetWindowTitle(pInfo->strWindowTitle);
	}
	if((gpInfo->strVerificationText != pInfo->strVerificationText) && (pInfo->strVerificationText != ""))
	{
		g_pTaskDialog->SetVerificationText(pInfo->strVerificationText);
	}
	if((gpInfo->strExpandedControlText != pInfo->strExpandedControlText) && (pInfo->strExpandedControlText != ""))
	{
		g_pTaskDialog->SetExpandedControlText(pInfo->strExpandedControlText);
	}
	if((gpInfo->strCollapsedControlText != pInfo->strCollapsedControlText) && (pInfo->strCollapsedControlText != ""))
	{
		g_pTaskDialog->SetCollapsedControlText(pInfo->strCollapsedControlText);
	}
	if((gpInfo->strMainInstruction != pInfo->strMainInstruction) && (pInfo->strMainInstruction != ""))
	{
		g_pTaskDialog->SetMainInstruction(pInfo->strMainInstruction);
	}
	if((gpInfo->strContent != pInfo->strContent) && (pInfo->strContent != ""))
	{
		g_pTaskDialog->SetContent(pInfo->strContent);
	}
	if((gpInfo->strExpandedInformation != pInfo->strExpandedInformation) && (pInfo->strExpandedInformation != ""))
	{
		g_pTaskDialog->SetExpandedInformation(pInfo->strExpandedInformation);
	}
	if((gpInfo->strFooter != pInfo->strFooter) && (pInfo->strFooter != ""))
	{
		g_pTaskDialog->SetFooter(pInfo->strFooter);
	}

	// Main Icon
	if((gpInfo->strMainIcon != pInfo->strMainIcon) && (pInfo->strMainIcon != ""))
	{
		if(pInfo->strMainIcon == "Error")
		{
			g_pTaskDialog->SetMainIcon(TD_ERROR_ICON);
		}else
		if(pInfo->strMainIcon == "Warning")
		{
			g_pTaskDialog->SetMainIcon(TD_WARNING_ICON);
		}else
		if(pInfo->strMainIcon == "Information")
		{
			g_pTaskDialog->SetMainIcon(TD_INFORMATION_ICON);
		}else
		if(pInfo->strMainIcon == "Shield")
		{
			g_pTaskDialog->SetMainIcon(TD_SHIELD_ICON);
		}else
		if(pInfo->strMainIcon.GetLength() > 0)
		{
			HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strMainIcon, IMAGE_ICON,
				::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
			g_pTaskDialog->SetMainIcon(hIcon);
		}
	}

	// Footer Icon
	if((gpInfo->strFooterIcon != pInfo->strFooterIcon) && (pInfo->strFooterIcon != ""))
	{
		if(pInfo->strFooterIcon == "Error")
		{
			g_pTaskDialog->SetFooterIcon(TD_ERROR_ICON);
		}else
		if(pInfo->strFooterIcon == "Warning")
		{
			g_pTaskDialog->SetFooterIcon(TD_WARNING_ICON);
		}else
		if(pInfo->strFooterIcon == "Information")
		{
			g_pTaskDialog->SetFooterIcon(TD_INFORMATION_ICON);
		}else
		if(pInfo->strFooterIcon == "Shield")
		{
			g_pTaskDialog->SetFooterIcon(TD_SHIELD_ICON);
		}else
		if(pInfo->strFooterIcon.GetLength() > 0)
		{
			HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strFooterIcon, IMAGE_ICON,
				::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
			g_pTaskDialog->SetFooterIcon(hIcon);
		}
	}

	if((gpInfo->nWidth != pInfo->nWidth) && (pInfo->nWidth != 0))
	{
		g_pTaskDialog->SetWidth(pInfo->nWidth);
	}
	int i;
	for(i=0; i<pInfo->asButtonText.GetSize(); i++)
	{
		if(pInfo->asButtonText[i] != "")
		{
			g_pTaskDialog->EnableButton(pInfo->auButtonID.GetAt(i), pInfo->asButtonText[i] == "true");
		}
	}
	for(i=0; i<pInfo->asRadioButtonText.GetSize(); i++)
	{
		if(pInfo->asRadioButtonText[i] != "")
		{
			g_pTaskDialog->EnableRadioButton(pInfo->auRadioButtonID.GetAt(i), pInfo->asRadioButtonText[i] == "true");
		}
	}

	if(gpInfo->nProgressBarPos != pInfo->nProgressBarPos)
	{
		g_pTaskDialog->SetProgressBarPos(pInfo->nProgressBarPos);
	}
	if( (gpInfo->nProgressBarRangeMin != pInfo->nProgressBarRangeMin) ||
		(gpInfo->nProgressBarRangeMax != pInfo->nProgressBarRangeMax) )
	{
		g_pTaskDialog->SetProgressBarRange(pInfo->nProgressBarRangeMin, pInfo->nProgressBarRangeMax);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ��������ҳ��
/////////////////////////////////////////////////////////////////////////////
void CBlueUIApp::ClearTaskDialogPage()
{
	g_aTaskDialogInfo.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// �����ҳ��
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::AddTaskDialogPage(void* p)
{
	TTaskDialogInfo* pInfo = (TTaskDialogInfo*)p;
	if(pInfo == NULL)
	{
		return FALSE;
	}

	g_aTaskDialogInfo.Add(*pInfo);

	return g_aTaskDialogInfo.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ִ����
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::DoTaskDialogSheets()
{
	if(g_aTaskDialogInfo.GetSize() == 0)
	{
		return -1;
	}

	CPtrArray apTaskPage;
	int i;
	for(i=0; i<g_aTaskDialogInfo.GetSize(); i++)
	{
		CTaskDialogPage* pTaskDialogPage = NULL;
		if(g_aTaskDialogInfo.GetSize() == 0)
		{
			pTaskDialogPage = new CTaskDialogPage(AfxGetMainWnd());
		}else
		{
			pTaskDialogPage = new CTaskDialogPage();
		}
		apTaskPage.Add(pTaskDialogPage);

		TTaskDialogInfo* pInfo = &(g_aTaskDialogInfo[i]);

		/////////////////////////////////////////////////////////////
		pTaskDialogPage->m_pInfo	= pInfo;

		// ���Ի��������������ֵ
		pTaskDialogPage->SetWindowTitle(pInfo->strWindowTitle);
		pTaskDialogPage->SetVerificationText(pInfo->strVerificationText);
		pTaskDialogPage->SetExpandedControlText(pInfo->strExpandedControlText);
		pTaskDialogPage->SetCollapsedControlText(pInfo->strCollapsedControlText);
		pTaskDialogPage->SetMainInstruction(pInfo->strMainInstruction);
		pTaskDialogPage->SetContent(pInfo->strContent);
		pTaskDialogPage->SetExpandedInformation(pInfo->strExpandedInformation);
		pTaskDialogPage->SetFooter(pInfo->strFooter);

		// Main Icon
		if(pInfo->strMainIcon == "Error")
		{
			pTaskDialogPage->SetMainIcon(TD_ERROR_ICON);
		}else
		if(pInfo->strMainIcon == "Warning")
		{
			pTaskDialogPage->SetMainIcon(TD_WARNING_ICON);
		}else
		if(pInfo->strMainIcon == "Information")
		{
			pTaskDialogPage->SetMainIcon(TD_INFORMATION_ICON);
		}else
		if(pInfo->strMainIcon == "Shield")
		{
			pTaskDialogPage->SetMainIcon(TD_SHIELD_ICON);
		}else
		if(pInfo->strMainIcon.GetLength() > 0)
		{
			HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strMainIcon, IMAGE_ICON,
				::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
			pTaskDialogPage->SetMainIcon(hIcon);
		}

		// Footer Icon
		if(pInfo->strFooterIcon == "Error")
		{
			pTaskDialogPage->SetFooterIcon(TD_ERROR_ICON);
		}else
		if(pInfo->strFooterIcon == "Warning")
		{
			pTaskDialogPage->SetFooterIcon(TD_WARNING_ICON);
		}else
		if(pInfo->strFooterIcon == "Information")
		{
			pTaskDialogPage->SetFooterIcon(TD_INFORMATION_ICON);
		}else
		if(pInfo->strFooterIcon == "Shield")
		{
			pTaskDialogPage->SetFooterIcon(TD_SHIELD_ICON);
		}else
		if(pInfo->strFooterIcon.GetLength() > 0)
		{
			HICON hIcon = (HICON)::LoadImage(NULL, pInfo->strFooterIcon, IMAGE_ICON,
				::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
			pTaskDialogPage->SetFooterIcon(hIcon);
		}

		pTaskDialogPage->SetWidth(pInfo->nWidth);

		if(pInfo->bEnableInputBox)
		{
			pTaskDialogPage->AddInputBox();
			pTaskDialogPage->SetInputBoxText(pInfo->strInputBoxText);
		}

		int j;
		for(j=0; j<pInfo->asButtonText.GetSize(); j++)
		{
			pTaskDialogPage->AddButton(pInfo->asButtonText.GetAt(j), pInfo->auButtonID.GetAt(j));
		}
		for(j=0; j<pInfo->asRadioButtonText.GetSize(); j++)
		{
			pTaskDialogPage->AddRadioButton(pInfo->asRadioButtonText.GetAt(j), pInfo->auRadioButtonID.GetAt(j));
		}
		// ���û�ж���Button,��CommandLinks���Ա�������ΪFALSE,����Ի����޷���ʾ
		pTaskDialogPage->EnableCommandLinks((pInfo->asButtonText.GetSize() > 0) ? pInfo->bEnableCommandLinks : FALSE);
		pTaskDialogPage->EnableHyperlinks(pInfo->bEnableHyperlinks);
		pTaskDialogPage->EnableRelativePosition(pInfo->bEnableRelativePosition);
		pTaskDialogPage->ShowProgressBar(pInfo->bShowProgressBar, pInfo->bMarquee);
		pTaskDialogPage->ExpandedByDefault(pInfo->bExpandedByDefault);
		pTaskDialogPage->ExpandFooterArea(pInfo->bExpandFooterArea);
		pTaskDialogPage->SetDefaultButton(pInfo->nDefaultButton);
		pTaskDialogPage->SetDefaultRadioButton(pInfo->nDefaultRadioButton);
		pTaskDialogPage->NoDefaultRadioButton(pInfo->bNoDefaultRadioButton);
		pTaskDialogPage->SetVerifyCheckState(pInfo->bVerifyCheckState);
		pTaskDialogPage->EnableCallbackTimer(pInfo->bEnableCallbackTimer);
		pTaskDialogPage->SetCommonButtons(pInfo->bCommonButtonOk,
									pInfo->bCommonButtonYes,
									pInfo->bCommonButtonNo,
									pInfo->bCommonButtonCancel,
									pInfo->bCommonButtonRetry,
									pInfo->bCommonButtonClose);

		pTaskDialogPage->EnableMessageBoxStyle(TRUE);
		/////////////////////////////////////////////////////////////

		if(i > 0)
		{
			pTaskDialogPage->m_pPagePrev = (CTaskDialogPage*)(apTaskPage[i-1]);
			((CTaskDialogPage*)(apTaskPage[i-1]))->m_pPageNext = pTaskDialogPage;
		}
	}

	int nRet = ((CTaskDialogPage*)(apTaskPage[0]))->DoModal();

	for(i=0; i<g_aTaskDialogInfo.GetSize(); i++)
	{
		// �ش�����ֵ
		TTaskDialogInfo* pInfo = &(g_aTaskDialogInfo[i]);
		CTaskDialogPage* pTaskDialogPage = (CTaskDialogPage*)(apTaskPage[i]);
		pInfo->nSelectedButtonId		= pTaskDialogPage->GetSelectedButtonId();
		pInfo->nSelectedRadioButtonId	= pTaskDialogPage->GetSelectedRadioButtonId();
		pInfo->bIsVerificiationChecked	= pTaskDialogPage->IsVerificiationChecked();

		if(pInfo->bEnableInputBox)
		{
			pInfo->strInputBoxText = pTaskDialogPage->GetInputBoxText();
		}
	}

	return nRet;
}

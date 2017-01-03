////////////////////////////////////////////////////////////////////////////
//	File:		PlatUI.cpp
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Implementation of Platform UI interface object.
////////////////////////////////////////////////////////////////////////////
// PlatUI.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "PlatUI.h"
#include "MainFrm.h"
#include "FileVersion.h"		// Get version of file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlatUI

CPlatUI::CPlatUI()
{
}

CPlatUI::~CPlatUI()
{
}

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(PlatUI)
CONTROL_INTERFACE_IMPLEMENT(PlatUI)

/////////////////////////////////////////////////////////////////////////////
// CPlatUI Interface handlers �������

// ע:�˽ӿ��еĴ󲿷ֺ���������ڶ��̳߳�ͻ����,��Ϊ�˽ӿ����
// �������п���ͬʱ������߳�ͬʱ����.
// Ŀǰ���õķ�ֹ��ͻ�ķ�����ͨ��һ����Ϣ��������ӵ�ִ�в���,
// ÿ�����ö�ͨ����Ϣ��ʽ����MainFrame,MainFrame�ٵ���PlatForm
// �еĺ�����ִ��ʵ�ʵĲ���.��Ϊ��Ϣ�Ǵ��д����,��˾Ϳ��Ա�
// ֤ͬʱֻ��һ������.
// һ��ͨ��SendMessage��ʽ��ͬ������,����ʱ����Ҫ�첽����,����
// ͨ��PostMessage��ʽ����,����ṩ��������COM_CMD��PCOM_CMD��
// �ṩͬ�����첽���ò���.�첽��ʽ,�����Ϣ��ĳ��������ָ��һ��
// �ṹ��ָ��,����Ҫע��,��������Ϣ�Ľ��ܷ�ִ��ɾ��������
// RCOM_CMD��PRCOM_CMD�������˷���ֵ,�����Ǵ���һ�������ĵ�ַ��
// ��Щ����½���SendMessage��PostMessage�����������ǲ�����,����
// ��Ҫ���ݸ��������,����ṩ��PDCOM_CMD��PRDCOM_CMD��,����Я��
// һ���ṹ,�˽ṹ�ڽ��շ�����ɾ����

/////////////////////////////////////////////////////////////////////////////
// ��ȡӦ�ó�����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetAppName()
{
	return theApp.m_xmlPlat.GetNodeText("application\\appName");
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨·��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatPath()
{
	return theApp.GetPlatRootPath();
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨�汾
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatVersion()
{
	// ��ȡƽ̨����汾,��Ϊʹ�ó���汾������ע���汾
	CString strVersion;
	CFileVersion fileversion;
	TCHAR szFullPath[MAX_PATH];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	// Get version number
	if (!fileversion.Open(szFullPath))
		return _T("0.0.0.0");
	strVersion = fileversion.GetFileVersion();
	fileversion.Close();
	strVersion.Replace(" ", "");
	strVersion.Replace(",", ".");
/*
	// ��ȡע����г���汾
	CXTRegistryManager registry(HKEY_LOCAL_MACHINE);
	CString strSection = "Setup";
	// ע��CXTRegistryManager��Section������Ե�ǰӦ�ó����,��˲���Ҫ������Ӧ�ó���ע���·��,ֻҪ��Section����ΪSetup�Ϳ���
	//strSection.Format("Software\\%s\\%s\\Setup",
	//		theApp.m_xmlPlat.GetNodeText("application\\RegCompany"),
	//		theApp.m_xmlPlat.GetNodeText("application\\RegProfile"));
	CString strVersion = registry.GetProfileString(strSection, "Version", "unknown");
*/
	return strVersion;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��ǰ����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetCurrentLanguage()
{
	return (LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage) ? LANGUAGE_CHINESE : LANGUAGE_ENGLISH;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨ע����·��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatRegistry()
{
	CString strSection;
	strSection.Format("Software\\%s\\%s",
			theApp.m_xmlPlat.GetNodeText("application\\RegCompany"),
			theApp.m_xmlPlat.GetNodeText("application\\RegProfile"));

	return strSection;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨��Ȩ�ַ���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatCopyRight()
{
	return theApp.m_xmlPlat.GetNodeText("application\\CopyRight");
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��ҳURL
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatHomeURL()
{
	return theApp.m_xmlPlat.GetNodeText("application\\HomeURL");
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ����URL
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetPlatDownloadURL()
{
	return theApp.m_xmlPlat.GetNodeText("application\\DownloadURL");
}

/////////////////////////////////////////////////////////////////////////////
// ������Ϣ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::SendMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	if(pIn == NULL)
	{
		return 0;
	}

	if((pIn->GetMsgType() == VMT_BROADCAST) || (pIn->GetMsgType() == VMT_MULTICAST))
	{
		// ����ǹ㲥��Ϣ���鲥��Ϣ,�����ƽ̨����Ϣ���н��й㲥����
		if(theApp.m_pIMessageQueue)
		{
			theApp.m_pIMessageQueue->PostMessage(pIn, "");
		}
		return 0;
	}

	CPlugIn* pPlugIn = theApp.FindPlugIn(pIn->GetFirstNodeId());
	if(pPlugIn)	// �����������Ϣ
	{
		if((pPlugIn->m_strProcess == theApp.m_strProcessId) || (pPlugIn->m_strProcess == ""))
		{
			// Ŀ��������ҵ����ҽ�����ͬ���߽���Ϊ��,��ֱ�ӵ���
			return pPlugIn->SendMessage(pIn, ppOut);
		}else
		{
			// ���Ŀ��������ҵ������̲�ͬ,���͸�IPC�������
			pPlugIn = theApp.FindPlugIn(VCIID_IPC);
			if(pPlugIn)
			{
				return pPlugIn->SendMessage(pIn, ppOut);
			}
		}
	}else
	if(pIn->GetFirstNodeId() == VCIID_PLATFORM)	// ����ƽ̨����Ϣ
	{
		if(pIn->GetSync() == VMT_SYNC)
		{
			// ͬ����Ϣ,ֱ�ӵ���ƽ̨����Ϣ������
			return ProcessMessage(pIn, ppOut);
		}else
		{
			// �첽��Ϣ,������Ϣ����
			if(theApp.m_pIMessageQueue)
			{
				theApp.m_pIMessageQueue->PostMessage(pIn, "");
			}
		}
	}else
	{
		// Ŀ������Ҳ������͸�ͨ���������
		pPlugIn = theApp.FindPlugIn(VCIID_COMMUNICATE);
		if(pPlugIn)
		{
			if((pPlugIn->m_strProcess == theApp.m_strProcessId) || (pPlugIn->m_strProcess == ""))
			{
				// ���ͨ������ڵ�ǰ�������͸�ͨ�����
				return pPlugIn->SendMessage(pIn, ppOut);
			}else
			{
				// ͨ��������ڵ�ǰ�������͸�IPC���
				pPlugIn = theApp.FindPlugIn(VCIID_IPC);
				if(pPlugIn)
				{
					return pPlugIn->SendMessage(pIn, ppOut);
				}
			}
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ƽ̨����Ϣ����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	if(pIn == NULL)
	{
		return 0;
	}

	if(pIn->GetMsgType() == VMT_BROADCAST)
	{
		// ����ǹ㲥��Ϣ,��������������ProcessMessage����
		for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
		{
			if(theApp.m_aPlugIns[i].m_pIVciControl == NULL)
			{
				// ������ʵ��δ����,�򴴽�ʵ������
				theApp.m_aPlugIns[i].CreateObject();
			}
			if(theApp.m_aPlugIns[i].m_pIVciControl != NULL)
			{
				#ifdef _DEBUG
				TRACE("Send broadcast message to %s\n", theApp.m_aPlugIns[i].m_strId);
				#endif
				// Ŀ��ID����Ϊ��Ӧ�����ID
				CVciMessage msg = *pIn;
				msg.SetDest(theApp.m_aPlugIns[i].m_strId);
				((IVciControl*)(theApp.m_aPlugIns[i].m_pIVciControl))->ProcessMessage(&msg, NULL);
			}
		}
		return 0;
	}else
	if(pIn->GetMsgType() == VMT_MULTICAST)
	{
		// ������鲥��Ϣ,�����Ŀ�����,��ȡ����Ҫ���͵�Ŀ�����(Ŀ������ֶ���,���зָ�)
		CString strMultiDest = pIn->GetDest();
		CStringArray asDest;
		int nPos = -1;
		while((nPos = strMultiDest.Find(",")) != -1)
		{
			asDest.Add(strMultiDest.Left(nPos));
			strMultiDest.Delete(0, nPos+1);
		}
		if(strMultiDest != "")
		{
			asDest.Add(strMultiDest);
		}

		// ��������Ŀ�������ProcessMessage����
		for(int i=0; i<asDest.GetSize(); i++)
		{
			CPlugIn* pPlugIn = theApp.FindPlugIn(asDest[i]);
			if(pPlugIn)
			{
				if(pPlugIn->m_pIVciControl == NULL)
				{
					// ������ʵ��δ����,�򴴽�ʵ������
					pPlugIn->CreateObject();
				}
				if(pPlugIn->m_pIVciControl != NULL)
				{
					#ifdef _DEBUG
					TRACE("Send broadcast message to %s\n", pPlugIn->m_strId);
					#endif
					// Ŀ��ID����Ϊ��Ӧ�����ID
					CVciMessage msg = *pIn;
					msg.SetDest(pPlugIn->m_strId);
					((IVciControl*)(pPlugIn->m_pIVciControl))->ProcessMessage(&msg, NULL);
				}
			}
		}
		return 0;		
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ����ƽ̨����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::SendCommand(int nCmd, WPARAM wParam, LPARAM lParam)
{
	int nRet;
	RCOM_CMD("", nCmd, wParam, lParam, &nRet)
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// ����ƽ̨����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SendCommand(int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult)
{
	RCOM_CMD("", nCmd, wParam, lParam, lpResult)
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::AddOutput(CString strName)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	// �޸�Ϊͨ����Ϣ��ʽ����,��2���������ڻش�ID
	int nID = 0;
	COM_CMD("", OC_CREATEOUTPUT, (WPARAM)(LPCTSTR)strName, &nID);

	return nID;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ���������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DelOutput(int nOT)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	// �޸�Ϊͨ����Ϣ��ʽ����
	PCOM_CMD("", OC_CLOSEOUTPUT, (WPARAM)nOT, 0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::ActiveOutput(int nOT)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	// �޸�Ϊͨ����Ϣ��ʽ����,����һЩ���⣬�ᶪʧ�ַ�����˸Ļ�ȥ
	PCOM_CMD("", OC_ACTIVEOUTPUT, (WPARAM)nOT, 0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����Ϣ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::Output(int nOT, LPCTSTR lpszOut)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	int nLen = strlen(lpszOut)+1;
	//PDCOM_CMD("", OC_OUTPUT, (WPARAM)nOT, lpszOut, nLen);
	DCOM_CMD("", OC_OUTPUT, (WPARAM)nOT, lpszOut, nLen);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����Ϣ�����ض���ɫ
// nFmt�ĸ��ֵĺ���:
// 0x10000   - �������
// 0x20000   - ��λ����ɫ��Ч(������������֮ǰ����OutputSet��������ɫ)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::OutputFormat(int nOT, int nFmt, LPCTSTR lpszOut)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	CString strSet;
	switch(nFmt & 0x0000FFFF)
	{
	case 0: OutputText(nOT, lpszOut, RGB(0,0,0));break;
	case 1: OutputText(nOT, lpszOut, RGB(0,0,255));break;
	case 2: OutputText(nOT, lpszOut, RGB(255,0,0));break;
	case 3: OutputText(nOT, lpszOut, RGB(0,255,0));break;
	case 4: OutputText(nOT, lpszOut, RGB(128,128,128));break;
	default: OutputText(nOT, lpszOut, RGB(0,0,0));
	}
/*
	BOOL bQuickOutput = (nFmt & 0x10000);	// ���������־

	CString strSet;
	switch(nFmt & 0x0000FFFF)
	{
	case 0: strSet = "COLOR=0,0,0";break;
	case 1: strSet = "COLOR=0,0,255";break;
	case 2: strSet = "COLOR=255,0,0";break;
	case 3: strSet = "COLOR=0,255,0";break;
	case 4: strSet = "COLOR=128,128,128";break;
	default: strSet = "COLOR=0,0,0";
	}

	if((nFmt & 0x20000) == 0)	// �Ƿ����ø�ʽ��־
	{
		int nLen = strSet.GetLength()+1;
		LPTSTR lpszSet = strSet.GetBuffer(nLen);
		if(bQuickOutput)	// �������,����PostMessage
			COM_CMD(0, OC_OUTPUTSET, (WPARAM)nOT, lpszSet)
		else
			PDCOM_CMD(0, OC_OUTPUTSET, (WPARAM)nOT, lpszSet, nLen);
		strSet.ReleaseBuffer();
	}
	
	if(bQuickOutput)	// �������,����PostMessage
	{
		COM_CMD(0, OC_OUTPUT, (WPARAM)nOT, lpszOut);
	}else
	{
		int nLen1 = strlen(lpszOut)+1;
		PDCOM_CMD(0, OC_OUTPUT, (WPARAM)nOT, lpszOut, nLen1);
	}
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����Ϣ����У���ַ����Ƚ�,��һ�µ��ַ���ʾΪ��ɫ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::OutputVerify(int nOT, LPCTSTR lpszOut, LPCTSTR lpszVerify)
{
	CPlatUI *pObj = GET_INTERFACE_OBJECT(PlatUI);

	int nLen = strlen(lpszOut)+1;
	DCOM_CMD("", OC_OUTPUT, (WPARAM)nOT, lpszOut, nLen);	// ��δ����У�麯��

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���������Ϣ��ʽ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::OutputSet(int nOT, LPCTSTR lpszSet)
{
	int nLen = strlen(lpszSet)+1;
	//PDCOM_CMD("", OC_OUTPUTSET, (WPARAM)nOT, lpszSet, nLen);
	DCOM_CMD("", OC_OUTPUTSET, (WPARAM)nOT, lpszSet, nLen);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ���������Ϣ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::OutputGet(int nOT, LPCTSTR lpszParam)
{
	int nRet;
	RCOM_CMD("", OC_OUTPUTGET, (WPARAM)nOT, (LPARAM)lpszParam, &nRet);
	CString* pStr = (CString*)nRet;
	CString strRet = *pStr;
	delete pStr;

	return strRet;
}

/////////////////////////////////////////////////////////////////////////////
// ��������Ϣ
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::OutputClear(int nOT)
{
	// �޸�Ϊͨ����Ϣ��ʽ����
	//PCOM_CMD("", OC_CLEAROUTPUT, (WPARAM)nOT, 0);
	COM_CMD("", OC_CLEAROUTPUT, (WPARAM)nOT, 0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����������������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::OutputAddTreeItems(int nOT, CTTreeItemArray_* pItem)
{
	// �޸�Ϊͨ����Ϣ��ʽ����
	COM_CMD("", OC_TREEOUTPUT, (WPARAM)nOT, (LPARAM)pItem);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡCommandBars
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::GetCommandBars()
{
	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
	return (void*)(pMainFrame->GetCommandBars());
}

/////////////////////////////////////////////////////////////////////////////
// �����չ�˵�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::AddExtMenu(CMenu*& pMenu)
{
	if(pMenu == NULL)
	{
		return FALSE;
	}

	CString strTitle;
	pMenu->GetMenuString(0, strTitle, MF_BYPOSITION);

	BOOL bLoadMenu = FALSE;
	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
	CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
	if(pCommandBars)
	{
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();

		// ���Ҳ˵��Ƿ����,������ھͲ��ô�����
		CXTPControls* pControls = pMenuBar->GetControls();
		CXTPControl* pControl = pControls->GetFirst();
		CXTPControl* pControlExt = NULL;
		while(pControl)
		{
			if(pControl->GetCaption() == strTitle)
			{
				pControlExt = pControl;
				break;
			}
			pControls->GetNext(pControl);
		}

		if(!pControlExt)
		{
			pMenuBar->LoadMenu(pMenu, FALSE);
			bLoadMenu = TRUE;

			pControl = pControls->GetFirst();
			while(pControl)
			{
				if(pControl->GetCaption() == strTitle)
				{
					pControlExt = pControl;
					break;
				}
				pControls->GetNext(pControl);
			}

			CExtMenuInfo extMenuInfo;
			extMenuInfo.m_strMainMenu = strTitle;
			extMenuInfo.m_strSubMenu = "";
			pMainFrame->m_arExtMenuInfo.Add(extMenuInfo);
		}

		if(pControlExt)
		{
			// ����չ�˵��ƶ������߲˵�֮ǰ
			CXTPControl* pControlTool = NULL;
			pControl = pControls->GetFirst();
			while(pControl)
			{
				CString strTools = "&Tools";
				if(LANGUAGE_PAGE_CHINESE == theApp.m_curLanguage)
				{
					strTools = "����(&T)";
				}
				if(pControl->GetCaption() == strTools)
				{
					pControlTool = pControl;
					break;
				}
				pControls->GetNext(pControl);
			}
			if(pControlTool)
			{
				pControls->MoveBefore(pControlExt, pControlTool->GetIndex());
			}
		}
	}

	delete pMenu;
	pMenu = NULL;

	return bLoadMenu;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����չ�˵�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DelExtMenu(CMenu*& pMenu)
{
	if(pMenu == NULL)
	{
		return FALSE;
	}

	CString strTitle;
	pMenu->GetMenuString(0, strTitle, MF_BYPOSITION);
	delete pMenu;
	pMenu = NULL;

	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
	CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
	if(pCommandBars)
	{
		// ���Ҳ�ɾ�������Ĳ˵�
		CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
		CXTPControls* pControls = pMenuBar->GetControls();
		CXTPControl* pControl = pControls->GetFirst();
		while(pControl)
		{
			if(pControl->GetCaption() == strTitle)
			{
				pControls->Remove(pControl);
				for(int i=0; i<pMainFrame->m_arExtMenuInfo.GetSize(); i++)
				{
					if(pMainFrame->m_arExtMenuInfo[i].m_strMainMenu == strTitle)
					{
						pMainFrame->m_arExtMenuInfo.RemoveAt(i);
					}
				}
				return TRUE;
			}
			pControls->GetNext(pControl);
		}
	}	

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �����չ�Ӳ˵�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::AddSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu, LPVOID pProc, LPVOID pUpdateProc)
{
	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
	CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
	if(!pCommandBars)
	{
		return FALSE;		
	}
	CXTPCommandBar* pMenuBar = pCommandBars->GetMenuBar();
	if(!pMenuBar)
	{
		return FALSE;
	}

	CString strMenuItem;
	pMenu->GetSubMenu(0)->GetMenuString(nIndex, strMenuItem, MF_BYPOSITION);
	CXTPControls* pControls = pMenuBar->GetControls();
	CXTPControl* pControl = pControls->GetFirst();
	CXTPCommandBar* pMenuBarSub = NULL;
	CXTPControl* pControlExtSub = NULL;
	while(pControl)
	{
		if(pControl->GetCaption() == strMainMenu)
		{
			pMenuBarSub = pControl->GetCommandBar();
			CXTPControls* pControlsSub = pMenuBarSub->GetControls();
			pControl = pControlsSub->GetFirst();
			while(pControl)
			{
				if(pControl->GetCaption() == strMenuItem)
				{
					pControlExtSub = pControl;
					break;
				}
				pControlsSub->GetNext(pControl);
			}			
			break;
		}
		pControls->GetNext(pControl);
	}
	if(pMenuBarSub && !pControlExtSub)
	{
		CXTPControls* pControlsSub = pMenuBarSub->GetControls();
		CMenu* pmenu = pMenu->GetSubMenu(0);
		if(pmenu)
		{
			pControlsSub->AddMenuItem(pmenu, nIndex);
			CExtMenuInfo extMenuInfo;
			extMenuInfo.m_strMainMenu = strMainMenu;
			extMenuInfo.m_strSubMenu = strMenuItem;
			extMenuInfo.nId = pmenu->GetMenuItemID(nIndex) - IDM_OWMCMD;
			extMenuInfo.pProc = (TYPEOF_ClientMenu*)pProc;
			extMenuInfo.pUpdateProc = (TYPEOF_UpdateClientMenu*)pUpdateProc;
			pMainFrame->m_arExtMenuInfo.Add(extMenuInfo);
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����չ�Ӳ˵�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DelSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �����˵�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::TrackPopupMenu(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd)
{
	return CXTPCommandBars::TrackPopupMenu(pMenu, nFlags, x, y, pWnd);
}

/////////////////////////////////////////////////////////////////////////////
// ���״̬��Pane
/////////////////////////////////////////////////////////////////////////////
static int g_nPaneID = 500;	// Status Pane����ʼ����ID

STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::AddStatusPane(int& nID, int nWidth)
{
	if((nID>10) && (nID<100))
	{
		PCOM_CMD("", OC_ADDPANE, nID, nWidth);
	}else
	{
		PCOM_CMD("", OC_ADDPANE, g_nPaneID, nWidth);
		nID = g_nPaneID;
		g_nPaneID++;		
	}

	// ע��Status Pane��IDС��500��ʾ��ƽ̨������,����500
	// ��ʾ����ƽ̨��̬������û���

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ��״̬��Pane
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DelStatusPane(int nID)
{
	if(nID != 0)
	{
		PCOM_CMD("", OC_REMOVEPANE, nID, 0);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SetPaneText(int nID, CString strPane)
{
	int nLen = strPane.GetLength()+1;
	LPTSTR lpszPane = strPane.GetBuffer(nLen);
	PDCOM_CMD("", OC_SETPANETEXT, nID, lpszPane, nLen);
	strPane.ReleaseBuffer();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Pane���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SetPaneWidth(int nID, int nWidth)
{
	PCOM_CMD("", OC_SETPANEWIDTH, nID, nWidth);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����Paneͼ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SetPaneImage(int nID, int nGroup, int nImage)
{
/*
	PCOM_CMD(0, OC_SETPANEIMAGE, nID, nGroup);
	PCOM_CMD(0, OC_SETPANEIMAGEITEM, nID, nImage);
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��ǰ������ĵ�����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CDocument*)
CPlatUI::XPlatUI::GetActiveDocument()
{
	CMainFrame* pMainFrame = (CMainFrame*)(theApp.GetMainWnd());
/*	CFrameWnd* pFrame = pMainFrame->GetActiveFrame();
	if(pFrame == NULL)
	{
		return NULL;
	}
	return pFrame->GetActiveDocument();*/
	return pMainFrame->GetActiveDocument();
/*	CMDIChildWnd* pActiveChildWnd = pMainFrame->MDIGetActive();
	if(pActiveChildWnd == NULL)
	{
		return NULL;
	}
	return pActiveChildWnd->GetActiveDocument();*/
	/*
	CView* pView = reinterpret_cast<CView*>(pActiveChildWnd->GetActiveView());
	if(pView == NULL)
	{
		return NULL;
	}
	return pView->GetDocument();*/
}

/////////////////////////////////////////////////////////////////////////////
// ����DockingPane
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::CreateDockingPane(CString strOwmId, CString strTitle, int nID, int nDirection, int nAttachType, HICON hIcon, int nAttachPane)
{
	CPlugIn* pPlugIn = theApp.GetOWM(strOwmId);
	if(pPlugIn == NULL)
	{
		return FALSE;
	}

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame->GetDockingPaneWnd(nID))
	{
		for(int i=0; i<pPlugIn->m_arDockingPane.GetSize(); i++)
		{
			if(pPlugIn->m_arDockingPane[i].m_nId == nID)
			{
				pMainFrame->CreateDockingBar(nID, nDirection, hIcon, nAttachPane);
				return TRUE;
			}
		}

		CXTPDockingPane* pPane = pMainFrame->CreateDockingBar(nID, nDirection, hIcon, nAttachPane);
		if(pPane == NULL)
		{
			return FALSE;
		}

		return TRUE;
	}

	COwmDockingPane paneInfo;
	paneInfo.m_strName = strTitle;
	paneInfo.m_nId = nID;
	paneInfo.m_strOwmId = strOwmId;
	paneInfo.m_nAttachType = nAttachType;

	// �������PaneID���Ƿ�������PlugIn��Tool��Ԥ���������Pane��Ϣ,���������,�����������Ϣ��ȫ�˴���Pane��Ϣ
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		for(int j=0; j<theApp.m_aPlugIns[i].m_arDockingPane.GetSize(); j++)
		{
			if( (theApp.m_aPlugIns[i].m_arDockingPane[j].m_nId == nID) &&
				(theApp.m_aPlugIns[i].m_arDockingPane[j].m_bFileDefined) )
			{
				if((theApp.m_aPlugIns[i].m_strType == "project") &&
					!theApp.m_aPlugIns[i].HasLoad() )
				{
					// ���̲��ֻ�м����˲Ž����ж�
					continue;
				}
				paneInfo.m_nDirection = theApp.m_aPlugIns[i].m_arDockingPane[j].m_nDirection;
				paneInfo.m_strIcon = theApp.m_aPlugIns[i].m_arDockingPane[j].m_strIcon;
				if(theApp.m_aPlugIns[i].m_arDockingPane[j].m_strIcon.Find(":") == -1)
				{
					paneInfo.m_strIcon = theApp.m_aPlugIns[i].m_strPath+"\\"+theApp.m_aPlugIns[i].m_arDockingPane[j].m_strIcon;
				}
				paneInfo.m_bAutoCreate = theApp.m_aPlugIns[i].m_arDockingPane[j].m_bAutoCreate;
				paneInfo.m_bSave = theApp.m_aPlugIns[i].m_arDockingPane[j].m_bSave;
			}
		}
	}
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		for(int j=0; j<theApp.m_aTools[i].m_arDockingPane.GetSize(); j++)
		{
			if( (theApp.m_aTools[i].m_arDockingPane[j].m_nId == nID) &&
				(theApp.m_aTools[i].m_arDockingPane[j].m_bFileDefined) )
			{
				paneInfo.m_nDirection = theApp.m_aTools[i].m_arDockingPane[j].m_nDirection;
				paneInfo.m_strIcon = theApp.m_aTools[i].m_arDockingPane[j].m_strIcon;
				if(theApp.m_aTools[i].m_arDockingPane[j].m_strIcon.Find(":") == -1)
				{
					paneInfo.m_strIcon = theApp.m_aTools[i].m_strPath+"\\"+theApp.m_aTools[i].m_arDockingPane[j].m_strIcon;
				}
				paneInfo.m_bAutoCreate = theApp.m_aTools[i].m_arDockingPane[j].m_bAutoCreate;
				paneInfo.m_bSave = theApp.m_aTools[i].m_arDockingPane[j].m_bSave;
			}
		}
	}

	pPlugIn->m_arDockingPane.Add(paneInfo);

	return pPlugIn->LoadDockingPane(nID);
}

/////////////////////////////////////////////////////////////////////////////
// ����DockingPane
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::CreateDockingPane(CString strOwmId, int nID)
{
	CPlugIn* pPlugIn = theApp.GetOWM(strOwmId);
	if(pPlugIn == NULL)
	{
		return FALSE;
	}
	return pPlugIn->LoadDockingPane(nID);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡDockingPane�����Ĵ��ڶ���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CWnd*)
CPlatUI::XPlatUI::GetDockingPaneWnd(int nPaneID)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return pMainFrame->GetDockingPaneWnd(nPaneID);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡDockingPane ID(��������)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetDockingPaneIdByName(CString strName)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return pMainFrame->GetDockingPaneIdByName(strName);
}

/////////////////////////////////////////////////////////////////////////////
// ����DockingPane
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::ActiveDockingPane(int nPaneID, BOOL bShow)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return pMainFrame->ActiveDockingPane(nPaneID, bShow);
}

/////////////////////////////////////////////////////////////////////////////
// �ر�DockingPane����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::CloseDockingPane(int nPaneID)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return pMainFrame->CloseDockingPane(nPaneID);
}

/////////////////////////////////////////////////////////////////////////////
// �����־
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::AddSysLogInfo(int nType, CString strFrom, CString strDesc)
{
/*
	LOGITEM LogItem;
	LogItem.nLogType = nType;
	LogItem.tTime = CTime::GetCurrentTime().GetTime();
	LogItem.strFrom = strFrom;
	if(theApp.m_CurUser.bLogined)	// ����ѵ�¼�ͼ�¼��¼�û���
	{
		LogItem.strUser = theApp.m_CurUser.strUserName;
	}else
	{
		LogItem.strUser = "------";
	}
	LogItem.strDesc = strDesc;
	
	// ֪ͨƽִ̨����Ӳ���
	// 0��ʾ����ƽ̨������ĳ��OWM,Ҳ���ǵ���CommandPlat
	COM_CMD(0, OC_LOGADDITEM, (WPARAM)(&LogItem), 0);
*/
	// ע:΢���DAO����һЩ����,�ڶ��̻߳�����ʹ�û������,
	// �˴�����־������DAO���������ݿ�,Ϊ�˷�ֹ���ֶ��̳߳�ͻ,
	// ��˽�ֱ�ӵ��÷�ʽ��Ϊ����Ϣ��ʽ,������Ϣ����MainFrame,
	// Ȼ��ͨ��MainFrame����ӵ���
	// ���̳߳�ͻ�����ڴ˽ӿڵ������������ձ����,�����Ҫ����
	// �нӿں�����Ϊͨ��MainFrame��Ϣ����ķ�ʽ������

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �ж���չģ���Ƿ����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::IsOwmExist(CString strOwmId)
{
	return (theApp.GetOWM(strOwmId) != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ������չģ������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SendOwmCmd(CString strOwmId, int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult)
{
	// ֪ͨƽִ̨��ĳ������
	// �������0��ʾ����ƽ̨����COM_CMD��-1��ʾ����ƽ̨
	if(strOwmId == "0")
	{	// SendMessage��ʽ��ƽ̨����Ϣ
		if(lpResult == NULL)
		{
			COM_CMD("0", nCmd, wParam, lParam);
		}else
		{
			RCOM_CMD("0", nCmd, wParam, lParam, lpResult);
		}
	}else
	if(strOwmId == "-1")
	{	// PostMessage��ʽ��ƽ̨����Ϣ
		if(lpResult == NULL)
		{
			PCOM_CMD("0", nCmd, wParam, lParam);
		}else
		{
			PRCOM_CMD("0", nCmd, wParam, lParam, lpResult);
		}
	}else
	{
		//BOOL bPost = ((nOwmID & 0xFFFF0000) != 0);
		// ���ģ��ID����65535,˵����Post��ʽ,������ID�ǵ�16λ
		//int nRealOwm = nOwmID&0x0000FFFF;
		BOOL bPost = (strOwmId.Find("post|") == 0);
		if(bPost)
		{
			strOwmId.Delete(0, 5);
		}

		// ���ģ�鴦�ڶ�̬Ϊפ��״̬,���ô˺�������ʹ��פ��,��֤������Ե���
		theApp.GetDynamicOWM(strOwmId);

		if(!bPost)
		{
			// SendMessage��ʽ��ĳ����չģ�鷢��Ϣ
			if(lpResult == NULL)
			{
				COM_CMD(strOwmId, nCmd, wParam, lParam);
			}else
			{	// ��䷵�ؽ��
				RCOM_CMD(strOwmId, nCmd, wParam, lParam, lpResult);
			}
		}else
		{
			// PostMessage��ʽ��ĳ����չģ�鷢��Ϣ
			if(lpResult == NULL)
			{
				PCOM_CMD(strOwmId, nCmd, wParam, lParam);
			}else
			{	// ��䷵�ؽ��
				PRCOM_CMD(strOwmId, nCmd, wParam, lParam, lpResult);
			}
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ع�����չģ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::LoadOwm(CString strOwmName, int& nID, BOOL bLoad)
{

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ĵ�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CDocument*)
CPlatUI::XPlatUI::OpenDocumentFile(LPCTSTR lpszFileName, LPCTSTR lpszOwmId)
{
	//return theApp.OpenDocumentFileWithOwm(lpszFileName, lpszOwmId);
	int nRet;
	RCOM_CMD("", OC_OPENDOCUMENTFILE, (WPARAM)(LPCTSTR)lpszFileName, (WPARAM)(LPCTSTR)lpszOwmId, &nRet);
	return (CDocument*)(nRet);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ������ָ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CWnd*)
CPlatUI::XPlatUI::GetPropertyGridWnd()
{
	return &GetPropertyGrid(AfxGetMainWnd());
}

/////////////////////////////////////////////////////////////////////////////
// �ж��Ƿ���Ը���������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::EnableUpdatePropertyGridContent()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return pMainFrame->m_bEnableUpdatePropertyGrid;
}

/////////////////////////////////////////////////////////////////////////////
// ˢ�����Դ�������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::UpdatePropertyGridContent()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->UpdatePropertyGridContent();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ���Դ���ˢ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::BeginUpdatePropertyGridContent()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->BeginUpdatePropertyGridContent();
}

/////////////////////////////////////////////////////////////////////////////
// �������Դ���ˢ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::EndUpdatePropertyGridContent()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EndUpdatePropertyGridContent();
}

/////////////////////////////////////////////////////////////////////////////
// ����һ���������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::CreateVciObject(CString strVciId, CString strInstName)
{
	return theApp.CreateVciObject(strVciId, strInstName);
}

/////////////////////////////////////////////////////////////////////////////
// �ر�һ���������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::ReleaseVciObject(void* pObject, BOOL bForceDelete)
{
	return theApp.ReleaseObject(pObject, bForceDelete);
}

/////////////////////////////////////////////////////////////////////////////
// �ر�һ���������(ָ��ʵ����)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::ReleaseObjectByInstanceName(CString strInstName)
{
	return theApp.ReleaseObjectByInstanceName(strInstName);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ���������(����ʵ����)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::GetObjectByInstanceName(CString strInstName)
{
	return theApp.GetObjectByInstanceName(strInstName);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����������ƽӿ�(�������ʵ���ӿ�ָ��)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::GetObjectControlPtrByInstancePtr(LPVOID pVciObj)
{
	return theApp.GetObjectControlPtrByInstancePtr(pVciObj);
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ���а�װ�����ID(����ָ���������)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetInstalledVci(CStringArray& asVciId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		asVciId.Add(theApp.m_aPlugIns[i].m_strId);
	}
	return asVciId.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�������(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciName(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strName;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����·��(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciPath(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strPath;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ������汾(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciVersion(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strVersion;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�������������Ϣ(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciProvider(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strProvider;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ���������(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciType(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strType;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�������ӿ�ID(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciClass(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strClass;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ������ӿڶ����ļ�(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciInterfaceFile(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strInterfaceFile;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ���������ʱ��(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CTime)
CPlatUI::XPlatUI::GetVciBuildTime(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].GetPlugInBuildTime();
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����GUID(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciGUID(CString strId)
{
	CString strVciId = "";
	int nPos = strId.Find("##");
	if(nPos != -1)
	{
		strVciId = strId;
		strVciId.Delete(0, nPos+2);
		strId = strId.Left(nPos);
	}
	if(strVciId != strId)
	{
		return "";
	}

	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].GetPlugInGUID();
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ����������ļ�λ��(�������ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciAboutPage(CString strId)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].m_strAboutPage;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ����ִ��һ���ĵ�����չģ��ID�б�(�������Id���ĵ�Id)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetVciDocExecuteModule(CString strId, CString strDocId, CString strExecuteType, CStringArray& arExecuteModule)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_arDocExtend.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_arDocExtend[j].m_strDocId == strDocId)
				{
					for(int k=0; k<theApp.m_aPlugIns[i].m_arDocExtend[j].m_arExecuteModule.GetSize(); k++)
					{
						if(strExecuteType == theApp.m_aPlugIns[i].m_arDocExtend[j].m_arExecuteModuleType[k])
						{
							arExecuteModule.Add(theApp.m_aPlugIns[i].m_arDocExtend[j].m_arExecuteModule[k]);
						}
					}
					return arExecuteModule.GetSize();
				}
			}

			return 0;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetVciDocIcon(CString strId, CString strDocId, CString strIconType)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_arDocExtend.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_arDocExtend[j].m_strDocId == strDocId)
				{
					for(int k=0; k<theApp.m_aPlugIns[i].m_arDocExtend[j].m_arDocIconType.GetSize(); k++)
					{
						if(strIconType == theApp.m_aPlugIns[i].m_arDocExtend[j].m_arDocIconType[k])
						{
							return theApp.m_aPlugIns[i].m_arDocExtend[j].m_arDocIconFile.GetAt(k);
						}
					}
					if(strIconType == "normal")
					{
						return theApp.m_aPlugIns[i].m_arDocExtend[j].m_strIcon;
					}
					return "";
				}
			}
			return "";
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// �����ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������,���ھ��)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetVciDocIcon(CString strId, CString strDocId, CString strIconType, HWND hWnd)
{
	CString strIconPath = GetVciPath(strId);
	CString strIconFile = GetVciDocIcon(strId, strDocId, strIconType);
	if(strIconFile != "")
	{
		strIconFile = strIconPath + "\\" + strIconFile;
		if(GetFileAttributes(strIconFile) != 0xFFFFFFFF)
		{
			COM_CMD("", OC_SETDOCICON, (WPARAM)(LPCTSTR)strIconFile, (LPARAM)hWnd);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �����ĵ��ı���(���ݴ��ھ��)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetDocumentTitle(CString strTitle, int nModifyType, HWND hWnd)
{
	if(strTitle != "")
	{
		COM_CMD("", OC_SETDOCTITLE, (WPARAM)(LPCTSTR)strTitle, (LPARAM)hWnd);
	}else
	{
		COM_CMD("", OC_MODIFYDOCTITLE, nModifyType, (LPARAM)hWnd);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����������ʵ������ָ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetVciObjects(CString strId, CPtrArray& auVciObj)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_aVciObj.GetSize(); j++)
			{
				auVciObj.Add(theApp.m_aPlugIns[i].m_aVciObj[j].m_pVciObj);
			}
			return theApp.m_aPlugIns[i].m_aVciObj.GetSize();
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����ʵ�������ʵ����
// strId = 0��ʾ�����������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetInstanceNameByObject(CString strId, LPVOID pVciObj)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if((strId != "") && (theApp.m_aPlugIns[i].m_strId == strId))
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_aVciObj.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_aVciObj[j].m_pVciObj == pVciObj)
				{
					return theApp.m_aPlugIns[i].m_aVciObj[j].m_strInstName;
				}
			}
			return "";
		}else
		if(strId == "")
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_aVciObj.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_aVciObj[j].m_pVciObj == pVciObj)
				{
					return theApp.m_aPlugIns[i].m_aVciObj[j].m_strInstName;
				}
			}
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����ʵ����������ü���
// strId = 0��ʾ�����������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetInstanceRefCount(CString strId, LPVOID pVciObj)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if((strId != "") && (theApp.m_aPlugIns[i].m_strId == strId))
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_aVciObj.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_aVciObj[j].m_pVciObj == pVciObj)
				{
					return theApp.m_aPlugIns[i].m_aVciObj[j].m_nRefCount;
				}
			}
			return 0;
		}else
		if(strId == "")
		{
			for(int j=0; j<theApp.m_aPlugIns[i].m_aVciObj.GetSize(); j++)
			{
				if(theApp.m_aPlugIns[i].m_aVciObj[j].m_pVciObj == pVciObj)
				{
					return theApp.m_aPlugIns[i].m_aVciObj[j].m_nRefCount;
				}
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ö��һ��VCI�������չ����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::EnumVciParameters(CString strId, CStringArray& asParamName)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].EnumParameters(asParamName);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ��VCI����Ĳ���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::GetVciParameter(CString strId, CString strName, CString& strValue, CString& strDefaultValue)
{
	if(strId == "")
	{
		// ��ȡƽ̨����
		strValue = theApp.m_xmlPlat.GetNodeText("application\\" + strName);
		strDefaultValue = strValue;
		return TRUE;
	}

	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].GetParameter(strName, strValue, strDefaultValue);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ����һ��VCI����Ĳ���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::SetVciParameter(CString strId, CString strName, CString strValue)
{
	for(int i=0; i<theApp.m_aPlugIns.GetSize(); i++)
	{
		if(theApp.m_aPlugIns[i].m_strId == strId)
		{
			return theApp.m_aPlugIns[i].SetParameter(strName, strValue);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӷ�ʱ������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::AddTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds, BOOL bStart)
{
	return theApp.AddTimerTask(strTaskName, strInterpId, strInterpName, strCommand, nSeconds, bStart);
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӷ�ʱ������
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
	CPlatUI::XPlatUI::AddTimerTask(CString strTaskName, LPVOID lpFunction, int nSeconds, BOOL bStart)
{
	return theApp.AddTimerTask(strTaskName, lpFunction, nSeconds, bStart);
}

/////////////////////////////////////////////////////////////////////////////
// ������������ȡ��ʱ����ָ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void*)
CPlatUI::XPlatUI::GetTimerTask(CString strTaskName)
{
	return theApp.FindTimerTask(strTaskName);
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����ʱ����(��������ָ��)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DeleteTimerTask(LPVOID lpTimerTask)
{
	return theApp.DeleteTimerTask((CTimerTask*)lpTimerTask);
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����ʱ����(����������)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::DeleteTimerTask(CString strTaskName)
{
	return theApp.DeleteTimerTask(strTaskName);
}

/////////////////////////////////////////////////////////////////////////////
// ������ʱ����
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::StartTimerTask(LPVOID lpTimerTask)
{
	CTimerTask* pTimer = (CTimerTask*)lpTimerTask;
	if(pTimer)
	{
		pTimer->StartTask();
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���ö�ʱ����ʼʱ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetTimerStartTime(LPVOID lpTimerTask, int tStart)
{
	CTimerTask* pTimer = (CTimerTask*)lpTimerTask;
	if(pTimer)
	{
		pTimer->SetStartTime(CTime(tStart));
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���ö�ʱ�������ʱ��
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetTimerEndTime(LPVOID lpTimerTask, int tEnd)
{
	CTimerTask* pTimer = (CTimerTask*)lpTimerTask;
	if(pTimer)
	{
		pTimer->SetEndTime(CTime(tEnd));
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���ö�ʱ���񴥷������ű�
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetTimerTrigger(LPVOID lpTimerTask, CString strTrigger)
{
	CTimerTask* pTimer = (CTimerTask*)lpTimerTask;
	if(pTimer)
	{
		pTimer->SetTrigger(strTrigger);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���ö�ʱ����ִ�д���
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(void)
CPlatUI::XPlatUI::SetTimerExecCount(LPVOID lpTimerTask, int nCount)
{
	CTimerTask* pTimer = (CTimerTask*)lpTimerTask;
	if(pTimer)
	{
		pTimer->SetExecCount(nCount);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ���а�װ�Ĺ���ID
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(int)
CPlatUI::XPlatUI::GetInstalledTool(CStringArray& asToolId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		asToolId.Add(theApp.m_aTools[i].m_strId);
	}
	return asToolId.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ��������(���ݹ���ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetToolName(CString strId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		if(theApp.m_aTools[i].m_strId == strId)
		{
			return theApp.m_aTools[i].m_strName;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ������·��(���ݹ���ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetToolPath(CString strId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		if(theApp.m_aTools[i].m_strId == strId)
		{
			return theApp.m_aTools[i].m_strPath;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����߰汾(���ݹ���ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetToolVersion(CString strId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		if(theApp.m_aTools[i].m_strId == strId)
		{
			return theApp.m_aTools[i].m_strVersion;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡһ�����߿�������Ϣ(���ݹ���ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(CString)
CPlatUI::XPlatUI::GetToolProvider(CString strId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		if(theApp.m_aTools[i].m_strId == strId)
		{
			return theApp.m_aTools[i].m_strProvider;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ���й���(���ݹ���ID)
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(BOOL)
CPlatUI::XPlatUI::RunTool(CString strId)
{
	for(int i=0; i<theApp.m_aTools.GetSize(); i++)
	{
		if(theApp.m_aTools[i].m_strId == strId)
		{
			return theApp.m_aTools[i].Run();
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////
//	File:		PlatOwmCommand.cpp
//	Version:	1.0.0.0
//	Created:	2005-08-18
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Plat Owm Command Processer implement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlueUI.h"
#include "PlugIn.h"
#include "MainFrm.h"
#include "resource.h"
#include "ProgDlg.h"
#include "TreeOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ���ȶԻ���
static CProgressDlg* g_ProgressDlg = NULL;

/////////////////////////////////////////////////////////////////////////////
// ƽ̨�������
/////////////////////////////////////////////////////////////////////////////
int CBlueUIApp::CommandPlat(int nCmd, WPARAM wParam, LPARAM lParam)
{
	switch(nCmd)
	{
	case OC_QUERYOWMID:	// ��ѯOWM
		{
			return FALSE;
		}

	case OC_RUNSCRIPT:	// ����TCL�ű�
		{
		}

	case OC_OPENVIEW:	// ��OWM����
		{
			if(wParam == NULL)
				return FALSE;

			CPlugIn* pOWM = theApp.GetDynamicOWM((LPCTSTR)wParam);
			if(pOWM)
			{
				//pOWM->NewOwmFrame();
				BOOL bActive = lParam;
				pOWM->ShowOwmView(TRUE, bActive);
				return TRUE;
			}

			return FALSE;
		}

	case OC_CLOSEVIEW:	// �ر�OWM����
		{
			if(wParam == NULL)
				return FALSE;

			CPlugIn* pOWM = theApp.GetDynamicOWM((LPCTSTR)wParam);
			if(pOWM)
			{
				pOWM->CloseOwmFrame();
				return TRUE;
			}

			return FALSE;
		}

	case OC_OPENBAR:	// ��OWM ControlBar
		{
			if(wParam == NULL)
				return FALSE;

			CPlugIn* pOWM = theApp.GetDynamicOWM((LPCTSTR)wParam);
			if(pOWM)
			{
				if(lParam == 1)
					pOWM->ShowOwmControlBar(TRUE);
				else
					pOWM->NewOwmControlBar();
				return TRUE;
			}

			return FALSE;
		}

	case OC_CLOSEBAR:	// �ر�OWM ControlBar
		{
			if(wParam == NULL)
				return FALSE;

			CPlugIn* pOWM = theApp.GetDynamicOWM((LPCTSTR)wParam);
			if(pOWM)
			{
				if(lParam == 1)
					pOWM->ShowOwmControlBar(FALSE);
				else
					pOWM->CloseOwmControlBar();
				return TRUE;
			}

			return FALSE;
		}

	case OC_GETTCLEXKEYWORD:	// ��ȡTCL��չ�����б�ָ��
		{
			return NULL;
		}

	case OC_DYNCLOSE:	// ĳ����չģ��Ķ�̬ж������
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			return pMainFrame->PostMessage(WM_PLAT_DYNCLOSE, wParam, lParam);
		}

	case OC_CREATEOUTPUT:	// �����������
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

			// ���ʹ��������������͡��ҿ��Ĵ���
			CString strName = (LPCTSTR)wParam;
			int nPos = strName.Find("##");
			int nType = OUTPUT_RICH;
			int nAttachPane = 0;
			int nPaneID = 0;
			if(nPos != -1)
			{
				CString strType = strName;
				strType.Delete(0, nPos+2);
				if(strType.Find("GRID") == 0)
				{
					nType = OUTPUT_GRID;
					strType.Delete(0, 4);
				}else
				if(strType.Find("TREE") == 0)
				{
					nType = OUTPUT_TREE;
					strType.Delete(0, 4);
				}
				strName = strName.Left(nPos);
				int nPos1 = strType.Find("##");
				if(nPos1 == 0)
				{
					// ��ʾҪͣ����ָ�������ұ�,##����Ϊͣ���Ĵ�����
					strType.Delete(0, 2);
					COutputWndCtrl* pOutputCtrl = pMainFrame->GetOutputCtrl(strType);
					if(pOutputCtrl)
					{
						nAttachPane = pOutputCtrl->nID;
					}else
					{
						nAttachPane = -1;
					}
				}else
				{
					nPos1 = strType.Find("**");
					if(nPos1 == 0)
					{
						// ��ʾָ��Pane ID
						strType.Delete(0, 2);
						nPaneID = atoi(strType);
					}
				}
			}

			int nID = *((int*)lParam);
			if(nID == 0)
			{
				nID = nPaneID;
			}

			if(nID == 0)
			{	// =0��ʾ�Զ�����ID
				COutputWndCtrl* pOutputWndCtrl = pMainFrame->CreateOutputWnd(strName, nType, -1, nAttachPane);
				*((int*)lParam) = pOutputWndCtrl->nID - IDR_PANE_OUTPUT;	// ��2���������ڷ���ID
			}else
			{	// !=0��ʾʹ��ָ��ID
				COutputWndCtrl* pOutputWndCtrl = pMainFrame->CreateOutputWnd(strName, nType, nID);
			}
			return nID;
		}

	case OC_CLOSEOUTPUT:	// �ر��������
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			//pMainFrame->GetPaneManager()->ClosePane((int)wParam + IDR_PANE_OUTPUT);
			pMainFrame->GetPaneManager()->DestroyPane((int)wParam + IDR_PANE_OUTPUT);
			pMainFrame->DeleteOutputCtrl((int)wParam + IDR_PANE_OUTPUT);
			return TRUE;
		}

	case OC_OUTPUTSET:	// �����������
		{
			return OUTPUT_SET_ID((int)wParam, (LPCTSTR)lParam);
		}

	case OC_OUTPUTGET:	//  ��ȡ���������Ϣ
		{
			if((int)wParam == -1)	// wParam=-1��ʾ���ݴ�������ȡ����ID,lParam=������
			{
				CString strTmp = "-1";
				CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
				COutputWndCtrl* pOutputCtrl = pMainFrame->GetOutputCtrl((LPCTSTR)lParam);
				if(pOutputCtrl)
				{
					strTmp.Format("%d", pOutputCtrl->nID - IDR_PANE_OUTPUT);
				}
				CString* pStr = new CString(strTmp);
				return (int)pStr;
			}else
			if((int)wParam == -2)	// wParam=-2��ʾ�жϴ����Ƿ����,lParam=����ID
			{
				CString strTmp = "-1";
				CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
				COutputWndCtrl* pOutputCtrl = pMainFrame->GetOutputCtrl((int)lParam);
				if(pOutputCtrl)
				{
					strTmp.Format("%d", pOutputCtrl->nID - IDR_PANE_OUTPUT);
				}
				CString* pStr = new CString(strTmp);
				return (int)pStr;
			}
			CString* pStr = new CString(OUTPUT_GET_ID((int)wParam, (LPCTSTR)lParam));
			return (int)pStr;
			//return (int)(LPCTSTR)OUTPUT_GET_ID((int)wParam, (LPCTSTR)lParam);
		}

	case OC_OUTPUT:	// ������������
		{
			//return OutputText((int)wParam, (LPCTSTR)lParam, RGB(0,0,0));
			return OUTPUT_ID_DIRECT((int)wParam, (LPCTSTR)lParam);
		}

	case OC_ACTIVEOUTPUT:	// �����
		{
			return OUTPUT_ACTIVE_ID((int)wParam);
		}

	case OC_CLEAROUTPUT:	// �����������
		{
			OUTPUT_CLEAR((int)wParam);
			return TRUE;
		}

	case OC_TREEOUTPUT:	// �������������
		{
			return OUTPUT_TREEITEMS_ID((int)wParam, (LPVOID)lParam);
		}

	case OC_ADDPANE:	// ���Pane
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->AddPane((UINT)wParam);
			pMainFrame->SetPaneWidth((UINT)wParam, (int)lParam);
			return TRUE;
		}

	case OC_REMOVEPANE:	// ɾ��Pane
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->ClosePaneItem((UINT)wParam);
			return TRUE;
		}

	case OC_SETPANETEXT:	// ����Pane����
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneText((UINT)wParam, (LPCTSTR)lParam);
			return TRUE;
		}

	case OC_SETPANECONTROL:	// ����Pane�ؼ�
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneControl((UINT)wParam, (CWnd*)lParam);
			return TRUE;
		}

	case OC_SETPANEIMAGE:	// ����Paneͼ����
		{
			//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			//pMainFrame->SetPaneImage((UINT)wParam, lParam);
			return TRUE;
		}

	case OC_SETPANEIMAGEITEM:	// ����Paneͼ����
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneImageItem((UINT)wParam, lParam);
			return TRUE;
		}

	case OC_SETPANEWIDTH:	// ����Pane���
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneWidth((UINT)wParam, lParam);
			return TRUE;
		}

	case OC_ATTACHPANEPROGRESS:	// ����Pane�ͽ������Ĺ���
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			UINT nPaneID = LOWORD(wParam);
			UINT nMaxValue = HIWORD(wParam);
			pMainFrame->AttachPaneProgress(nPaneID, (LPCTSTR)lParam, nMaxValue);
			return TRUE;
		}

	case OC_SHOWPANEPROGRESS:	// ��ʾ/����Pane������
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->ShowPaneProgress((UINT)wParam, lParam);
			return TRUE;
		}

	case OC_SETPANEPROGRESSPOS:	// ����Pane����������
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneProgressPos((UINT)wParam, (int)lParam);
			return TRUE;
		}

	case OC_SETPANEBUTTON:	// ����Pane��ť
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneButton((UINT)wParam, (CExtMenuInfo*)lParam);
			return TRUE;
		}

	case OC_SETPANETOOLTIP:	// ����Pane��ʾ��Ϣ
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetPaneTooltip((UINT)wParam, (LPCTSTR)lParam);
			return TRUE;
		}

	case OC_OPENPROJECT:	// �򿪹���
		{
			if(wParam == 0)
			{
				theApp.OnProjectOpen();
			}else
			{
				theApp.OpenProjectFile(NULL, (LPCTSTR)wParam);
			}
			return TRUE;
		}

	case OC_CLOSEPROJECT:	// �رչ���
		{
			theApp.OnProjectClose();
			return TRUE;
		}

	case OC_EXECOPTION:	// ִ������ҳ����
		{
			if((wParam == 1) && (theApp.IsProjectOpen()))	// ��������ҳ
			{
				theApp.OnProjectProperty();
			}
			return TRUE;
		}

	case OC_EXECTREEOPTION:	// ִ��ͨ����״���ԶԻ���
		{
			CTreeOptionsDlg option;
			option.SetTOptArrayPoint((CTOptArray*)lParam);
			return option.DoModal();
		}

	case OC_OPENDOCUMENTFILE:	// ���ļ�
		{
			return (int)(theApp.OpenDocumentFileWithOwm((LPCTSTR)wParam, (LPCTSTR)lParam));
		}

	case OC_SETDOCICON:	// �����ĵ�ͼ��
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetDocumentIcon((LPCTSTR)wParam, (HWND)lParam);
			return TRUE;
		}

	case OC_SETDOCTITLE:	// �����ĵ�����
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetDocumentTitle((LPCTSTR)wParam, 0, (HWND)lParam);
			return TRUE;
		}

	case OC_MODIFYDOCTITLE:	// �޸��ĵ�����
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetDocumentTitle("", wParam, (HWND)lParam);
			return TRUE;
		}

	case OC_SETCOMMANDICON:	// ����ĳ������ID��ͼ��
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			return pMainFrame->ImageManagerSetIcon(wParam, (LPCTSTR)lParam);
		}

	case OC_SETCOMMANDICONS:	// ���ö������ID��ͼ��
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			return pMainFrame->ImageManagerSetIcons((CUIntArray*)wParam, (LPCTSTR)lParam);
		}

	case OC_SETOWMICONS:	// ����ĳ��OWM��ͼ��
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if(pMainFrame)
			{
				CPlugIn* pOwmEdit = theApp.FindPlugIn((LPCTSTR)wParam);
				if(pOwmEdit)
				{
					CString strImagePath = pOwmEdit->m_strPath;
					strImagePath += "\\image";
					return pMainFrame->LoadImageStyleIcons(strImagePath);
				}
			}
			return FALSE;
		}

	case OC_REFRESHDEBUGWIN:	// ˢ�µ��Դ���
		{
			IInterp* pInterp = (IInterp*)wParam;
			if(pInterp)
			{
				CString strRes;
				pInterp->RefreshDebugWindow("all", FALSE, strRes);
			}
			return TRUE;
		}

	case OC_CHECK_ACTIVEOWM:	// �ж�ָ��OWM�����Ƿ�Ϊ��ǰ�����
		{
			return theApp.CheckActiveOWM((LPCTSTR)wParam, (LPCTSTR)lParam);
		}

	case OC_SHOWABOUTBOX:	// ��ʾָ��ģ���About�Ի���
		{
			CString strPluginId = (LPCTSTR)wParam;
			theApp.ShowAboutBox(strPluginId);
			return TRUE;
		}

	case OC_SENDVCICOMMAND:	// ����VCI������(ProcessMessage����)
		{
			if((wParam == 0) || (lParam == 0))
			{
				return FALSE;
			}
			CString strVciInstance = (LPCTSTR)wParam;
			CVciMessage* pMsg = (CVciMessage*)lParam;
			if(strVciInstance.Find("owm:") == 0)
			{
				// ����OWM����
				CString strOwmId = strVciInstance;
				strOwmId.Delete(0, 4);
				CPlugIn* pPlugIn = theApp.GetOWM(strOwmId);
				if(pPlugIn == NULL)
				{
					return FALSE;
				}
				IVciOwm* pIVciOwm = pPlugIn->GetIVciOwm();
				if(pIVciOwm == NULL)
				{
					// ���OWM�����δ����,���ȼ���
					pPlugIn->LoadOwmPlugIn();
					pIVciOwm = pPlugIn->GetIVciOwm();
				}
				IVciControl* pIVciControl = (IVciControl*)(pPlugIn->GetObjectControlPtrByInstancePtr(pIVciOwm));
				if(pIVciControl != NULL)
				{
					return pIVciControl->ProcessMessage(pMsg, NULL);
				}
			}else
			{
				// ���͵�ָ��ʵ������VCI�������
				LPVOID pVciObj = theApp.GetObjectByInstanceName(strVciInstance);
				IVciControl* pIVciControl = (IVciControl*)(theApp.GetObjectControlPtrByInstancePtr(pVciObj));
				if(pIVciControl != NULL)
				{
					return pIVciControl->ProcessMessage(pMsg, NULL);
				}
			}
			return FALSE;
		}

	case OC_PATHSELECT_DIALOG:	// ·��ѡ��Ի���
		{
			CXTBrowseDialog dlg(AfxGetMainWnd());
			// ���ñ���
			CString strTitle = (LPCTSTR)wParam;
			dlg.SetTitle(strTitle.GetBuffer(0));
			// ���ó�ʼ·��
			if(lParam != NULL)
			{
				CString strPath = (LPCTSTR)lParam;
				dlg.SetSelPath(strPath.GetBuffer(0));
			}

			if (dlg.DoModal() == IDOK)
			{
				CString strPath = dlg.GetSelPath();
				CString* pStr = new CString(strPath);
				return (int)pStr;
			}

			return NULL;
		}

	// ͨ�ý��ȶԻ������
	case OC_PROGDLG_CREATE:	// ����ͨ�ý��ȶԻ���
		{
			if(g_ProgressDlg == NULL)
			{
				CString strCaption = "";
				CString strStatus = "";
				if(wParam)
				{
					strCaption = (LPCTSTR)wParam;
					int nPos = strCaption.Find('\n');
					if(nPos != -1)
					{
						strStatus = strCaption.Right(strCaption.GetLength()-nPos-1);
						strCaption = strCaption.Left(nPos);
					}
				}
				g_ProgressDlg = new CProgressDlg(strCaption);
				g_ProgressDlg->Create();
				g_ProgressDlg->SetStatus(strStatus);
				if(lParam != 0)
				{
					g_ProgressDlg->SetRange(0, lParam);
				}
				//int nResult = g_ProgressDlg->LoopDialog();
				//delete g_ProgressDlg;
				//g_ProgressDlg = NULL;
				// 1=��������,0=�û�cancel
				//return nResult;
				return 0;
			}
			// �Ѿ����������еĶԻ���
			return -1;
		}

	case OC_PROGDLG_SETPOS:	// ���ý���
		{
			if(g_ProgressDlg != NULL)
			{
				g_ProgressDlg->SetPos(wParam);
			}

			MSG msg;
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{															
				TranslateMessage(&msg);										
				DispatchMessage(&msg);
			}

			return 0;
		}

	case OC_PROGDLG_SETSTATUS:	// ����״̬��Ϣ
		{
			if(g_ProgressDlg != NULL)
			{
				g_ProgressDlg->SetStatus(LPCTSTR(wParam));
			}

			MSG msg;
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{															
				TranslateMessage(&msg);										
				DispatchMessage(&msg);
			}

			return 0;
		}

	case OC_PROGDLG_END:	// �������ȶԻ���
		{
			if(g_ProgressDlg != NULL)
			{
				g_ProgressDlg->EndDialog();
				delete g_ProgressDlg;
				g_ProgressDlg = NULL;
			}
			return 0;
		}

	case OC_PROGDLG_CANCEL:	// cancel���ȶԻ���
		{
			if(g_ProgressDlg != NULL)
			{
				// ִ��cancel�ű�
				CString strScript = g_ProgressDlg->GetCancelScript();
				if(strScript != "")
				{
					CString strInterpInstance = _T("");

					// ������������(֧��������ʽ�Ľ�����)
					if(strScript.Find("<<") == 0)
					{
						int nPos = strScript.Find(">>");
						if(nPos != -1)
						{
							strScript.Delete(0, 2);
							strInterpInstance = strScript.Left(nPos-2);
							strScript.Delete(0, nPos);
						}
					}

					CString strInterpName = "";	// ��������
					if(strScript.Find("<") == 0)
					{
						strScript.Delete(0, 1);
						int nPos = strScript.Find(">");
						if(nPos != -1)
						{
							strInterpName = strScript.Left(nPos);
							if(strInterpName.Find("org.interp") != 0)
							{
								strInterpName = "org.interp." + strInterpName;
							}
							strScript.Delete(0, nPos+1);
						}		
					}

					BOOL bCancel = TRUE;
					// ������������ִ��
					if(strInterpName.GetLength() > 0)
					{
						IInterp* pInterp = (IInterp*)(theApp.CreateVciObject(strInterpName, strInterpInstance));
						if(pInterp)
						{
							pInterp->RunScriptCommand(strScript);
							if(pInterp->GetResult() == "0")
								bCancel = FALSE;
							theApp.ReleaseObject(pInterp);
						}
					}

					if(!bCancel)
						return -1;
				}

				// �����Ի���
				g_ProgressDlg->EndDialog();
				delete g_ProgressDlg;
				g_ProgressDlg = NULL;
			}
			return 0;
		}

	case OC_PROGDLG_CANCELSCRIPT:	// ����cancel�ű�
		{
			if(g_ProgressDlg != NULL)
			{
				g_ProgressDlg->SetCancelScript((LPCTSTR)wParam);
			}
			return 0;
		}

	case OC_TASK_DIALOG:	// ͨ������Ի���
		{
			if(wParam == 0)	// ִ��TaskDialog
			{
				return theApp.DoTaskDialog((void*)lParam);
			}else
			if(wParam == 1)	// ˢ��TaskDialog
			{
				return theApp.RefreshTaskDialog((void*)lParam);
			}else
			if(wParam == 2)	// ��XML���ز�ִ��TaskDialog
			{
				return theApp.DoXmlTaskDialog((void*)lParam);
			}else
			if(wParam == 3)	// ɾ��������ҳ��
			{
				theApp.ClearTaskDialogPage();
			}else
			if(wParam == 4)	// �����ҳ��
			{
				return theApp.AddTaskDialogPage((void*)lParam);
			}else
			if(wParam == 5)	// ִ����
			{
				return theApp.DoTaskDialogSheets();
			}
			return 0;
		}

	case OC_POPUP_CONTROL:	// ��ʾPopup����
		{
			theApp.ShowPopupControl((TPopupControlInfo*)lParam);
			return 0;
		}

	default:
		break;
	}

	return 0;
}



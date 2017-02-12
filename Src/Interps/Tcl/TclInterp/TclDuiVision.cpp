////////////////////////////////////////////////////////////////////////////
//	File:		TclDuiVision.cpp
//	Version:	1.0.0.0
//	Created:	2017-01-21
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	implementation of Tcl DuiVision cmds
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CTclInterp.h"
#include "TclDuiVision.h"
#include "IDuiVisionApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static IDuiVisionApp* g_pIDuiVisionApp = NULL;		// ƽ̨DuiVision�ӿ�ָ��

/////////////////////////////////////////////////////////////////////////////
// ��ȡIDuiVisionApp�ӿ�
/////////////////////////////////////////////////////////////////////////////
IDuiVisionApp* GetIDuiVisionApp(Tcl_Interp *interp)
{
	if(g_pIDuiVisionApp == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
		if(pTclInterp)
		{
			g_pIDuiVisionApp = (IDuiVisionApp*)(pTclInterp->getPlatInterface(PLAT_INTERFACE_DUIVISIONAPP));
		}
	}
	return g_pIDuiVisionApp;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPtrList	g_TclDuiDialogObjList;	// DUI�Ի�������б�

CTclDuiDialogObj::CTclDuiDialogObj(Tcl_Interp* interp)
{
	m_pDialogObj	= NULL;
	m_Interp		= interp;
	m_strName		= "";
	m_strErrorInfo	= "";
	g_TclDuiDialogObjList.AddTail(this);
}

CTclDuiDialogObj::~CTclDuiDialogObj()
{
	// ɾ���Ի���
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(m_Interp);
	if(pIDuiVisionApp)
	{
		pIDuiVisionApp->RemoveDuiDialog((CDlgBase*)(m_pDialogObj));
	}

	m_pDialogObj = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ����һ��DUI�Ի���
/////////////////////////////////////////////////////////////////////////////
void* CTclDuiDialogObj::CreateDuiDialog(CString strXmlTemplate, CString strDlgName, BOOL bModule)
{
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(m_Interp);
	if(pIDuiVisionApp == NULL)
	{
		m_strErrorInfo = _T("Get duivisionapp interface failed.");
		return FALSE;
	}

	CDlgBase* pDlg = pIDuiVisionApp->CreateDuiDialog(strXmlTemplate, NULL, strDlgName, bModule, TRUE);

	m_pDialogObj = (void*)pDlg;
	m_strName = strDlgName;


	if(m_pDialogObj == NULL)
	{
		m_strErrorInfo = _T("Create duivision dialog failed.");
	}

	return m_pDialogObj;
}

/////////////////////////////////////////////////////////////////////////////
// ����һ��DUI�Ի���,��ע���¼��������
/////////////////////////////////////////////////////////////////////////////
void* CTclDuiDialogObj::CreateDuiDialogWithHandler(CString strXmlTemplate, CString strDlgName, BOOL bModule,
				CTclDuiHandler* pDuiHandler, CString strTDuiHandlerObj, CString strControlName, BOOL bInit)
{
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(m_Interp);
	if(pIDuiVisionApp == NULL)
	{
		m_strErrorInfo = _T("Get duivisionapp interface failed.");
		return FALSE;
	}

	CDlgBase* pDlg = pIDuiVisionApp->CreateDuiDialog(strXmlTemplate, NULL, strDlgName, bModule, TRUE);

	m_pDialogObj = (void*)pDlg;
	m_strName = strDlgName;

	if(m_pDialogObj == NULL)
	{
		m_strErrorInfo = _T("Create duivision dialog failed.");
		return NULL;
	}

	// �����������¼��������
	CTclDuiHandler::RegisterDuiHandler(pDlg, pDuiHandler,
					strTDuiHandlerObj, strControlName, bInit, m_Interp);

	// ����iTcl Handler���������DuiVision����ָ�뺯��
	if(!strTDuiHandlerObj.IsEmpty())
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		if(pTclInterp)
		{
			CString strTemp;
			strTemp.Format("%s SetDuiObject %d", strTDuiHandlerObj, m_pDialogObj);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return NULL;
			}
		}
	}

	// �����ģ̬�Ի���,��ֱ����ʾ�Ի���
	if(bModule)
	{
		pDlg->DoModal();
	}

	return m_pDialogObj;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡDUI�Ի������
/////////////////////////////////////////////////////////////////////////////
CTclDuiDialogObj* CTclDuiDialogObj::GetDuiDialog(void* pDialogObj)
{
	// ��ȡ�����б��е���
	int count = g_TclDuiDialogObjList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclDuiDialogObjList.FindIndex(i);
		CTclDuiDialogObj* pTclDialogObj = (CTclDuiDialogObj*)g_TclDuiDialogObjList.GetAt(pos);
		if(pDialogObj == pTclDialogObj->GetDialogObj()) 
		{
			return pTclDialogObj;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ���DUI�Ի������
/////////////////////////////////////////////////////////////////////////////
void CTclDuiDialogObj::AddDuiDialog(CString strDlgName, void* pDialogObj, Tcl_Interp* interp)
{
	CTclDuiDialogObj* pTclDialogObj = CTclDuiDialogObj::GetDuiDialog(pDialogObj);
	if(pTclDialogObj != NULL)
	{
		return;
	}

	pTclDialogObj = new CTclDuiDialogObj(interp);
	pTclDialogObj->SetDialogObj(pDialogObj);
	if(!strDlgName.IsEmpty())
	{
		pTclDialogObj->SetName(strDlgName);
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp)
	{
		pIDuiVisionApp->AddDuiDialog((CDlgBase*)pDialogObj);
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�DUI�Ի������
/////////////////////////////////////////////////////////////////////////////
void CTclDuiDialogObj::RemoveDuiDialog(void* pDialogObj, Tcl_Interp* interp)
{
	// ɾ���Ի���
	int count = g_TclDuiDialogObjList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclDuiDialogObjList.FindIndex(i);
		CTclDuiDialogObj* pTclDialogObj = (CTclDuiDialogObj*)g_TclDuiDialogObjList.GetAt(pos);
		if(pDialogObj == pTclDialogObj->GetDialogObj()) 
		{
			g_TclDuiDialogObjList.RemoveAt(pos);
			delete pTclDialogObj;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPtrList	g_TclDuiHandlerList;	// DUI Handler�����б�

CTclDuiHandler::CTclDuiHandler(Tcl_Interp* interp) : CDuiHandler()
{
	m_pDlg = NULL;
	m_Interp = interp;
	m_strTDuiHandlerObj = "";
	m_strErrorInfo	= "";
	g_TclDuiHandlerList.AddTail(this);
}

CTclDuiHandler::~CTclDuiHandler(void)
{
}

// ��ʼ��
void CTclDuiHandler::OnInit()
{
	// ����iTcl�����OnInit����
	if(!m_strTDuiHandlerObj.IsEmpty())
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		if(pTclInterp)
		{
			CString strTemp;
			strTemp.Format("%s OnInit", m_strTDuiHandlerObj);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return;
			}
		}
	}
}

// DUI�¼�����
LRESULT CTclDuiHandler::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����iTcl�����OnInit����
	if(!m_strTDuiHandlerObj.IsEmpty())
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		if(pTclInterp)
		{
			CString strTemp;
			strTemp.Format("%s OnMessage %s %d %d %d",
					m_strTDuiHandlerObj, strName, Msg, wParam, lParam);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				//AfxMessageBox(pTclInterp->GetResult());
				return FALSE;
			}
		}
	}

    return 0;
}

// DUI��ʱ���¼�����
void CTclDuiHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
	// ���strTimerName�ǿգ������iTcl�����OnTimer����
	if(!strTimerName.IsEmpty())
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		if(pTclInterp)
		{
			CString strTemp;
			strTemp.Format("%s OnTimer %s",
					m_strTDuiHandlerObj, strTimerName);
			if(!pTclInterp->RunCommandEx(strTemp, TCL_EVAL_DIRECT))
			{
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����һ��DUI Handler����,������ע��
/////////////////////////////////////////////////////////////////////////////
CTclDuiHandler* CTclDuiHandler::RegisterDuiHandler(CDuiObject* pDuiObject, CTclDuiHandler* pDuiHandler, CString strTDuiHandlerObj, CString strControlName, BOOL bInit, Tcl_Interp* interp)
{
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		return NULL;
	}

	CTclDuiHandler* pTclDuiHandler = NULL;
	if(pDuiHandler != NULL)
	{
		pTclDuiHandler = pDuiHandler;
	}else
	{
		pTclDuiHandler = new CTclDuiHandler(interp);
	}
	pTclDuiHandler->SetTDuiHandlerObj(strTDuiHandlerObj);
	BOOL bRet = pIDuiVisionApp->RegisterDuiHandler(pDuiObject, pTclDuiHandler, strControlName, bInit);

	if(!bRet)
	{
		pTclDuiHandler->SetErrorInfo(_T("Register duivision handler failed."));
	}

	return pTclDuiHandler;
}

/////////////////////////////////////////////////////////////////////////////
// ����DUI Handler�����Ƿ����
/////////////////////////////////////////////////////////////////////////////
BOOL CTclDuiHandler::IsDuiHandlerExist(CTclDuiHandler* pTclDuiHandler)
{
	// ���Ҷ����б��е���
	int count = g_TclDuiHandlerList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclDuiHandlerList.FindIndex(i);
		CTclDuiHandler* _pTclDuiHandler = (CTclDuiHandler*)g_TclDuiHandlerList.GetAt(pos);
		if(_pTclDuiHandler == pTclDuiHandler) 
		{
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ���һ��DUI Handler����
/////////////////////////////////////////////////////////////////////////////
CTclDuiHandler* CTclDuiHandler::AddDuiHandler(CTclDuiHandler* pTclDuiHandler, Tcl_Interp* interp)
{
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		return NULL;
	}

	pIDuiVisionApp->AddDuiHandler(pTclDuiHandler);

	return pTclDuiHandler;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ��һ��DUI Handler����
/////////////////////////////////////////////////////////////////////////////
void CTclDuiHandler::RemoveDuiHandler(CTclDuiHandler* pTclDuiHandler, Tcl_Interp* interp)
{
	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		return;
	}

	pIDuiVisionApp->RemoveDuiHandler(pTclDuiHandler);
}

/////////////////////////////////////////////////////////////////////////////
// �ر�ָ������������������DUI����
/////////////////////////////////////////////////////////////////////////////
void CloseInterpAllDuiObject(Tcl_Interp* interp)
{
	// ɾ��ָ������������Tcl�ű�������Dui�Ի���
	int count = g_TclDuiDialogObjList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclDuiDialogObjList.FindIndex(i);
		CTclDuiDialogObj* pTclDialogObj = (CTclDuiDialogObj*)g_TclDuiDialogObjList.GetAt(pos);
		if((pTclDialogObj != NULL) && (pTclDialogObj->GetInterp() == interp))
		{
			g_TclDuiDialogObjList.RemoveAt(pos);
			delete pTclDialogObj;
		}
	}

	// ɾ������Tcl�ű�������DuiHandler����
	count = g_TclDuiHandlerList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclDuiHandlerList.FindIndex(i);
		CTclDuiHandler* pTclDuiHandler = (CTclDuiHandler*)g_TclDuiHandlerList.GetAt(pos);
		if((pTclDuiHandler != NULL) && (pTclDuiHandler->GetInterp() == interp))
		{
			g_TclDuiHandlerList.RemoveAt(pos);
			// �������ƽ̨��ɾ���ӿڽ���ɾ��
			CTclDuiHandler::RemoveDuiHandler(pTclDuiHandler, interp);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ر�����DUI����
/////////////////////////////////////////////////////////////////////////////
void CloseAllDuiObject()
{
	// ɾ������Tcl�ű�������Dui�Ի���
	int count = g_TclDuiDialogObjList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclDuiDialogObjList.FindIndex(i);
		CTclDuiDialogObj* pTclDialogObj = (CTclDuiDialogObj*)g_TclDuiDialogObjList.GetAt(pos);
		if(pTclDialogObj != NULL)
		{
			g_TclDuiDialogObjList.RemoveAt(pos);
			delete pTclDialogObj;
		}
	}

	// ɾ������Tcl�ű�������DuiHandler����
	count = g_TclDuiHandlerList.GetCount();
	for(int i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclDuiHandlerList.FindIndex(i);
		CTclDuiHandler* pTclDuiHandler = (CTclDuiHandler*)g_TclDuiHandlerList.GetAt(pos);
		if(pTclDuiHandler != NULL) 
		{
			g_TclDuiHandlerList.RemoveAt(pos);
			// �������ƽ̨��ɾ���ӿڽ���ɾ��
			CTclDuiHandler::RemoveDuiHandler(pTclDuiHandler, pTclDuiHandler->GetInterp());
		}
	}

	// �ͷŴ˽������д�����DuiSystemʵ��
	DuiSystem::Release();
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::resource
// load xmlFile
// getconfig name
// getxmlfile name
// getskin name
// getstring name
// setstring name value
// parsestring value
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_Resource(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));

	if(strOpr == "load")	// ������Դ�ļ�
	{
		CString strXmlFile = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strStyle = _T("");
		if(objc > 3)
		{
			strStyle = ConvertUTFParam(Tcl_GetString(objv[3]));
		}
		if(!pIDuiVisionApp->DuiLoadResourceXml(strXmlFile, strStyle))
		{
			Tcl_AppendResult(interp, "Load resource file fail.", (char *) NULL);
			return TCL_ERROR;
		}
	}else
	if(strOpr == "getconfig")	// ��ȡϵͳ������Ϣ
	{
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = pIDuiVisionApp->DuiGetConfig(strName);
		Tcl_AppendResult(interp, strValue, (char *)NULL);
	}else
	if(strOpr == "getxmlfile")	// ��ȡXML�ļ�
	{
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = pIDuiVisionApp->DuiGetXmlFile(strName);
		Tcl_AppendResult(interp, strValue, (char *)NULL);
	}else
	if(strOpr == "getskin")	// ��ȡSkin
	{
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = pIDuiVisionApp->DuiGetSkin(strName);
		Tcl_AppendResult(interp, strValue, (char *)NULL);
	}else
	if(strOpr == "getstring")	// ��ȡ�ַ���ֵ
	{
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = pIDuiVisionApp->DuiGetString(strName);
		Tcl_AppendResult(interp, strValue, (char *)NULL);
	}else
	if(strOpr == "setstring")	// �����ַ���ֵ
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "setstring name value");
			return TCL_ERROR;
		}
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = ConvertUTFParam(Tcl_GetString(objv[3]));
		pIDuiVisionApp->DuiSetString(strName, strValue);
	}else
	if(strOpr == "parsestring")	// �����ַ������滻���е��滻����
	{
		// ��ȡtcl������
		CString strVarName = ConvertUTFParam(Tcl_GetString(objv[2]));
		// ��ȡ����ֵ
		CString strValue = ConvertUTFParam(Tcl_GetVar(interp, strVarName.GetBuffer(strVarName.GetLength()), 0));
		pIDuiVisionApp->ParseDuiString(strValue);
		strVarName.ReleaseBuffer();

		strValue = ConvertStringToUTF(strValue);
		char* szVar = strVarName.GetBuffer(strVarName.GetLength());
		char* szValue = strValue.GetBuffer(strValue.GetLength());
		Tcl_SetVar(interp, szVar, szValue, TCL_LEAVE_ERR_MSG);
		strVarName.ReleaseBuffer();
		strValue.ReleaseBuffer();
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::control
// getcontrol controlName ?-parentDialog parentDialog?
// getcontrol controlName ?-parentPopup parentPopup?
// getcontrol controlName ?-parent parentControl?
// getcontrol controlName -dialog dialogName
// getvalue controlObj
// setvalue controlObj value
// setattribute controlObj attrName attrValue
// getposstr controlObj
// setposstr controlObj pos
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_Control(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));

	if(strOpr == "getcontrol")	// ��ȡDUI�ؼ�������
	{
		CString strControlName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CControlBase* pControl = NULL;
		CString strType = _T("");
		if(objc > 3)
		{
			strType = ConvertUTFParam(Tcl_GetString(objv[3]));
			strType.MakeUpper();
		}
		if(strType == _T("-PARENT"))
		{
			CString strParentControl = ConvertUTFParam(Tcl_GetString(objv[4]));
			CDuiObject* pDuiObject = (CDuiObject*)(atoi(strParentControl));
			pControl = pIDuiVisionApp->DuiGetControl(pDuiObject, strControlName);
		}else
		if(strType == _T("-PARENTDIALOG"))
		{
			CString strParentDialog = ConvertUTFParam(Tcl_GetString(objv[4]));
			CDlgBase* pDuiDlg = (CDlgBase*)(atoi(strParentDialog));
			pControl = pIDuiVisionApp->DuiGetControl(pDuiDlg, strControlName);
		}else
		if(strType == _T("-PARENTPOPUP"))
		{
			CString strParentPopup = ConvertUTFParam(Tcl_GetString(objv[4]));
			CDlgPopup* pDuiPopup = (CDlgPopup*)(atoi(strParentPopup));
			pControl = pIDuiVisionApp->DuiGetControl(pDuiPopup, strControlName);
		}else
		if(strType == _T("-DIALOG"))
		{
			CString strDialogName = ConvertUTFParam(Tcl_GetString(objv[4]));
			pControl = pIDuiVisionApp->DuiGetDialogControl(strDialogName, strControlName);
		}else
		{
			pControl = pIDuiVisionApp->DuiGetControl(NULL, strControlName);
		}

		CString strResult;
		strResult.Format("%d", (int)pControl);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "getvalue")	// ��ȡDUI�ؼ�ֵ
	{
		CString strControlObj = ConvertUTFParam(Tcl_GetString(objv[2]));
		CControlBase* pControl = (CControlBase*)(atoi(strControlObj));
		if(pControl == NULL)
		{
			Tcl_AppendResult(interp, "Control object is null!", (char *) NULL);
			return TCL_ERROR;
		}

		CString strResult = _T("");
		if(pControl->IsClass(CControlBaseFont::GetClassName()))
		{
			strResult = ((CControlBaseFont*)pControl)->GetTitle();
			Tcl_AppendResult(interp, strResult, (char *)NULL);
		}else
		if(pControl->IsClass(CDuiCheckButton::GetClassName()))
		{
			strResult = ((CDuiCheckButton*)pControl)->GetCheck() ? _T("1") : _T("0");
			Tcl_AppendResult(interp, strResult, (char *)NULL);
		}else
		if(pControl->IsClass(CDuiRadioButton::GetClassName()))
		{
			strResult = ((CDuiRadioButton*)pControl)->GetValue();
			Tcl_AppendResult(interp, strResult, (char *)NULL);
		}
	}else
	if(strOpr == "setvalue")	// ����DUI�ؼ�ֵ
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "setvalue controlObj value ?-update?");
			return TCL_ERROR;
		}

		CString strControlObj = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strValue = ConvertUTFParam(Tcl_GetString(objv[3]));
		BOOL bUpdateControl = FALSE;
		if(objc > 4)
		{
			CString strUpdate = ConvertUTFParam(Tcl_GetString(objv[4]));
			if(strUpdate == "-update")
			{
				bUpdateControl = TRUE;
			}
		}

		CControlBase* pControl = (CControlBase*)(atoi(strControlObj));
		if(pControl == NULL)
		{
			Tcl_AppendResult(interp, "Control object is null!", (char *) NULL);
			return TCL_ERROR;
		}

		if(pControl->IsClass(CControlBaseFont::GetClassName()))
		{
			((CControlBaseFont*)pControl)->SetTitle(strValue);
		}else
		if(pControl->IsClass(CDuiCheckButton::GetClassName()))
		{
			((CDuiCheckButton*)pControl)->SetCheck((strValue == _T("1")) ? TRUE : FALSE);
		}else
		if(pControl->IsClass(CDuiRadioButton::GetClassName()))
		{
			((CDuiRadioButton*)pControl)->SetCheck((strValue == _T("1")) ? TRUE : FALSE);
		}else
		{
			Tcl_AppendResult(interp, "Control is not support get value!", (char *) NULL);
			return TCL_ERROR;
		}

		// ˢ�¿ؼ�
		if(bUpdateControl)
		{
			pControl->UpdateControl(true);
		}
	}else
	if(strOpr == "setattribute")	// ����DUI�ؼ�����ֵ
	{
		if(objc < 5)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "setattribute controlObj attrName attrValue ?-update?");
			return TCL_ERROR;
		}

		CString strControlObj = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strAttrName = ConvertUTFParam(Tcl_GetString(objv[3]));
		CString strAttrValue = ConvertUTFParam(Tcl_GetString(objv[4]));
		BOOL bUpdateControl = FALSE;
		if(objc > 5)
		{
			CString strUpdate = ConvertUTFParam(Tcl_GetString(objv[5]));
			if(strUpdate == "-update")
			{
				bUpdateControl = TRUE;
			}
		}

		CControlBase* pControl = (CControlBase*)(atoi(strControlObj));
		if(pControl == NULL)
		{
			Tcl_AppendResult(interp, "Control object is null!", (char *) NULL);
			return TCL_ERROR;
		}

		// ��Ҫͨ���ؼ�����ˢ�µ�һЩ����ֵ��Ҫ���⴦��,������ֱ�ӵ��ÿؼ�����������ֵ����
		if(strAttrName == _T("show"))
		{
			pControl->SetVisible(_ttoi(strAttrValue));
		}else
		if(strAttrName == _T("title"))
		{
			((CControlBaseFont*)pControl)->SetTitle(strAttrValue);
		}else
		if(strAttrName == _T("value"))
		{
			if(pControl->IsClass(CDuiComboBox::GetClassName()))
			{
				((CDuiComboBox*)pControl)->SetComboValue(strAttrValue);
			}else
			{
				pControl->SetAttribute(strAttrName, strAttrValue, TRUE);
			}
		}else
		{
			pControl->SetAttribute(strAttrName, strAttrValue, TRUE);
		}

		// ˢ�¿ؼ�
		if(bUpdateControl)
		{
			pControl->UpdateControl(true);
		}
	}else
	if(strOpr == "getposstr")	// ��ȡDUI�ؼ�ֵ��λ���ַ���
	{
		CString strControlObj = ConvertUTFParam(Tcl_GetString(objv[2]));
		CControlBase* pControl = (CControlBase*)(atoi(strControlObj));
		if(pControl == NULL)
		{
			Tcl_AppendResult(interp, "Control object is null!", (char *) NULL);
			return TCL_ERROR;
		}

		Tcl_AppendResult(interp, pControl->GetPosStr(), (char *)NULL);
	}else
	if(strOpr == "setposstr")	// ����DUI�ؼ�ֵ��λ���ַ���
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "setposstr controlObj pos");
			return TCL_ERROR;
		}

		CString strControlObj = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strPos = ConvertUTFParam(Tcl_GetString(objv[3]));
		CControlBase* pControl = (CControlBase*)(atoi(strControlObj));
		if(pControl == NULL)
		{
			Tcl_AppendResult(interp, "Control object is null!", (char *) NULL);
			return TCL_ERROR;
		}

		pControl->SetPosStr(strPos);
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

// GridCtrl��Ԫ����Ϣ
struct _GridItemInfo
{
	CString strTitle;		// ����
	CString strContent;		// ����
	int		nImageIndex;	// ͼƬ����
	CString strImage;		// ͼƬ�ļ�
	Color	clrText;		// ������ɫ
	CString strLink;		// ���ӵ�����
	CString strLinkAction;	// ���ӵĶ���
	BOOL	bUseTitleFont;	// �Ƿ�ʹ�ñ���������ʾ����
	BOOL	bLink;			// �Ƿ������ӵ�Ԫ��
};

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::gridctrl
// addrow controlObj row ?-id id? ?-image image? ?-rightImage rightImage? ?-check check? ?-textColor textColor? ?-backColor backColor? -items itemList
// deleterow controlObj row
// addcontrol controlObj row item controlClass controlName ?-dialog dialogObj?
// getrowcount controlObj
// getrowcheck controlObj row
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_GridCtrl(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	// ��ʼ��DuiSystem����Դ�ļ���Ŀ¼
	DuiSystem::SetRootPath(pIDuiVisionApp->GetDuiResourcePath());

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));
	CString strDuiObject = ConvertUTFParam(Tcl_GetString(objv[2]));
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)(atoi(strDuiObject));
	if(pGridCtrl == NULL)
	{
		Tcl_AppendResult(interp, "Get gridctrl object fail!", (char *) NULL);
		return TCL_ERROR;
	}

	if(strOpr == "addrow")	// �����
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "addrow controlObj row ?-image image? ?-check check? -items itemList");
			return TCL_ERROR;
		}

		int nRow = -1;
		CString strRow = ConvertUTFParam(Tcl_GetString(objv[3]));
		if(!strRow.IsEmpty())
		{
			nRow = atoi(strRow);
		}

		CString strId = "";
		int nImageIndex = -1;
		CString strImage = "";
		int nRightImageIndex = -1;
		CString strRightImage = "";
		Color clrText = Color(0,0,0,0);
		Color clrBack = Color(0,0,0,0);
		int nCheck = -1;
		vector<_GridItemInfo> vecItemInfo;
		for(int i = 4; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			if(strParam == "ID")	// ����id����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost id param");
					return TCL_ERROR;
				}
				strId = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "IMAGE")	// ����image����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost image param");
					return TCL_ERROR;
				}
				CString strImageTmp = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				if((strImageTmp.Find(_T("skin:")) == 0) || (strImageTmp.Find(_T(".")) != -1))
				{
					strImage = strImageTmp;
				}else
				{
					nImageIndex = atoi(strImageTmp);
				}
				i++;
			}else
			if(strParam == "RIGHTIMAGE")	// ����right-image����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost right-image param");
					return TCL_ERROR;
				}
				CString strImageTmp = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				if((strImageTmp.Find(_T("skin:")) == 0) || (strImageTmp.Find(_T(".")) != -1))
				{
					strRightImage = strImageTmp;
				}else
				{
					nRightImageIndex = atoi(strImageTmp);
				}
				i++;
			}else
			if(strParam == "CHECK")	// ����check����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost check param");
					return TCL_ERROR;
				}
				CString strCheck = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				if(!strCheck.IsEmpty())
				{
					nCheck = atoi(strCheck);
				}
				i++;
			}else
			if(strParam == "TEXTCOLOR")	// ����text-color����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost text-color param");
					return TCL_ERROR;
				}
				clrText = CDuiObject::StringToColor(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "BACKCOLOR")	// ����back-color����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost back-color param");
					return TCL_ERROR;
				}
				clrBack = CDuiObject::StringToColor(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "ITEMS")	// ����items����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost itemList param");
					return TCL_ERROR;
				}
				
				// ��ȡitem�б�
				int nListCount = 0;
				Tcl_Obj** paList;
				if(TCL_ERROR == Tcl_ListObjGetElements(interp, objv[i+1], &nListCount, &paList))
				{
					return TCL_ERROR;
				}

				if(0 == nListCount)
				{
					return TCL_OK;
				}

				for(int j=0; j<nListCount; j++)
				{
					Tcl_Obj* listItem = (paList[j]);
					// ��ȡÿ��item�������б�
					int nListCountAttr = 0;
					Tcl_Obj** paListAttr;
					if(TCL_ERROR == Tcl_ListObjGetElements(interp, listItem, &nListCountAttr, &paListAttr))
					{
						return TCL_ERROR;
					}

					if(nListCountAttr < 1)
					{
						return TCL_OK;
					}

					_GridItemInfo itemInfo;
					itemInfo.bLink = FALSE;
					itemInfo.strTitle = ConvertUTFParam(paListAttr[0]->bytes);
					itemInfo.strContent = "";
					itemInfo.bUseTitleFont = 0;
					itemInfo.nImageIndex = -1;
					itemInfo.clrText = Color(0,0,0,0);
					itemInfo.strImage = "";
					itemInfo.strLink = "";
					itemInfo.strLinkAction = "";
					if(nListCountAttr > 1)
					{
						itemInfo.strContent = ConvertUTFParam(paListAttr[1]->bytes);
					}
					if(nListCountAttr > 2)
					{
						CString strTemp = ConvertUTFParam(paListAttr[2]->bytes);
						if(strTemp == "-link")
						{
							// ��ʾ�����ӵ�Ԫ��
							itemInfo.bLink = TRUE;
							itemInfo.strLink = itemInfo.strTitle;
							itemInfo.strLinkAction = itemInfo.strContent;
						}else
						{
							itemInfo.bUseTitleFont = atoi(ConvertUTFParam(paListAttr[2]->bytes));
						}
					}
					if(nListCountAttr > 3)
					{
						itemInfo.nImageIndex = atoi(ConvertUTFParam(paListAttr[3]->bytes));
					}
					if(nListCountAttr > 4)
					{
						itemInfo.clrText = CDuiObject::StringToColor(ConvertUTFParam(paListAttr[4]->bytes));
					}
					if(nListCountAttr > 5)
					{
						itemInfo.strImage = ConvertUTFParam(paListAttr[5]->bytes);
					}

					vecItemInfo.push_back(itemInfo);
				}

				i++;
			}
		}

		// �����
		nRow = pGridCtrl->InsertRow(nRow, strId, nImageIndex, clrText, strImage,
					nRightImageIndex, strRightImage, nCheck, clrBack);
		// ����еĵ�Ԫ������
		for(size_t i = 0; i < vecItemInfo.size(); i++)
		{
			_GridItemInfo &itemInfo = vecItemInfo.at(i);
			if(itemInfo.bLink)
			{
				pGridCtrl->SetSubItemLink(nRow, i,
					itemInfo.strLink, itemInfo.strLinkAction,
					itemInfo.nImageIndex,
					itemInfo.clrText, itemInfo.strImage);
			}else
			{
				pGridCtrl->SetSubItem(nRow, i,
					itemInfo.strTitle, itemInfo.strContent,
					itemInfo.bUseTitleFont, itemInfo.nImageIndex,
					itemInfo.clrText, itemInfo.strImage);
			}
		}

		CString strResult;
		strResult.Format("%d", nRow);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "deleterow")	// ɾ����
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "deleterow controlObj row");
			return TCL_ERROR;
		}

		int nRow = -1;
		CString strRow = ConvertUTFParam(Tcl_GetString(objv[3]));
		if(!strRow.IsEmpty())
		{
			nRow = atoi(strRow);
		}

		BOOL bRet = pGridCtrl->DeleteRow(nRow);

		CString strResult;
		strResult.Format("%d", bRet);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "addcontrol")	// ��ӵ�Ԫ���ӿؼ�
	{
		if(objc < 8)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "addcontrol controlObj row item controlClass controlName pos ?-dialog dialogObj?");
			return TCL_ERROR;
		}

		int nRow = atoi(ConvertUTFParam(Tcl_GetString(objv[3])));
		int nItem = atoi(ConvertUTFParam(Tcl_GetString(objv[4])));
		CString strControlClass = ConvertUTFParam(Tcl_GetString(objv[5]));
		CString strControlName = ConvertUTFParam(Tcl_GetString(objv[6]));
		CString strControlPos = ConvertUTFParam(Tcl_GetString(objv[7]));
		HWND hWnd = NULL;
		if(objc > 9)
		{
			CString strDialog = ConvertUTFParam(Tcl_GetString(objv[8]));
			if(strDialog == "-dialog")
			{
				CString strDialogObj = ConvertUTFParam(Tcl_GetString(objv[9]));
				CDlgBase* pDlg = (CDlgBase*)(atoi(strDialogObj));
				if(pDlg)
				{
					hWnd = pDlg->GetSafeHwnd();
				}
			}
		}

		CControlBase* pControl = pIDuiVisionApp->DuiCreateControlByName(strControlClass, hWnd, NULL);
		if(pControl)
		{
			pControl->SetName(strControlName);
			pControl->SetPosStr(strControlPos);
			BOOL bRet = pGridCtrl->AddSubItemControl(nRow, nItem, pControl);
			if(!bRet)
			{
				delete pControl;
				Tcl_AppendResult(interp, "Create control " + strControlClass + " faild.", (char *) NULL);
				return TCL_ERROR;
			}

			CString strResult;
			strResult.Format("%d", pControl);
			Tcl_AppendResult(interp, strResult, (char *)NULL);
		}
	}else
	if(strOpr == "getrowcount")	// ��ȡ����
	{
		int nRowCount = pGridCtrl->GetRowCount();

		CString strResult;
		strResult.Format("%d", nRowCount);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "getrowcheck")	// ��ȡ�еļ���״̬
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "deleterow controlObj row");
			return TCL_ERROR;
		}

		int nRow = -1;
		CString strRow = ConvertUTFParam(Tcl_GetString(objv[3]));
		if(!strRow.IsEmpty())
		{
			nRow = atoi(strRow);
		}

		int nCheck = pGridCtrl->GetRowCheck(nRow);

		CString strResult;
		strResult.Format("%d", nCheck);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::handler
// duiObj ?-handler handlerObj? ?-handlerName handlerTclObjName? ?-control controlName? ?-init?
// add handlerObj
// remove handlerObj
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_Handler(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));

	if(strOpr == "register")	// ע���¼��������
	{
		CString strDuiObject = ConvertUTFParam(Tcl_GetString(objv[2]));
		CDuiObject* pDuiObject = (CDuiObject*)(atoi(strDuiObject));

		CTclDuiHandler* pTclDuiHandler = NULL;
		CString strTDuiHandlerObj = "";
		CString strControlName = "";
		BOOL bInit = FALSE;
		for(int i = 3; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			if(strParam == "HANDLER")	// ָ��Handler����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost handlerObj param");
					return TCL_ERROR;
				}
				pTclDuiHandler = (CTclDuiHandler*)(atoi(ConvertUTFParam(Tcl_GetString(objv[i+1]))));
				i++;
			}else
			if(strParam == "HANDLERNAME")	// ָ��Tcl�¼����������
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost handlerName param");
					return TCL_ERROR;
				}
				strTDuiHandlerObj = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "CONTROL")	// ָ��ControlName
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost controlName param");
					return TCL_ERROR;
				}
				strControlName = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "INIT")
			{
				bInit = TRUE;
			}
		}

		pTclDuiHandler = CTclDuiHandler::RegisterDuiHandler(pDuiObject, pTclDuiHandler,
							strTDuiHandlerObj, strControlName, bInit, interp);

		CString strResult;
		strResult.Format("%d", pTclDuiHandler);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "add")	// ����¼��������
	{
		CString strHandler = ConvertUTFParam(Tcl_GetString(objv[2]));
		CTclDuiHandler::AddDuiHandler((CTclDuiHandler*)(atoi(strHandler)), interp);
	}else
	if(strOpr == "remove")	// ɾ���¼��������
	{
		CString strHandler = ConvertUTFParam(Tcl_GetString(objv[2]));
		CTclDuiHandler::RemoveDuiHandler((CTclDuiHandler*)(atoi(strHandler)), interp);
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::dialog
// create create xmlTemplate ?-name dlgName? ?-modal 1|0? ?-handler handlerObj? ?-handlerName handlerTclObjName? ?-control controlName? ?-init?
// show xmlTemplate dlgName isModule
// messagebox text caption ?type? ?-width width? ?-height height?
// showwindow dlgObj show|hide
// notifymsg msg -caption caption ?-delay delayTime? ?-width width? ?-height height? ?type?
// add dlgObj
// remove dlgObj
// get dlgName
// close dlgObj ?-ok|cancel|close|yes|no?
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_Dialog(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));

	if(strOpr == "create")	// �����Ի���
	{
		CString strXmlTemplate = ConvertUTFParam(Tcl_GetString(objv[2]));

		CString strDlgName = "";
		BOOL bModal = TRUE;
		CTclDuiHandler* pTclDuiHandler = NULL;
		CString strTDuiHandlerObj = "";
		CString strControlName = "";
		BOOL bInit = FALSE;
		for(int i = 3; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			if(strParam == "NAME")	// ָ��Name
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost name param");
					return TCL_ERROR;
				}
				strDlgName = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "MODAL")	// �Ƿ�ģ̬�Ի���
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost isModal param");
					return TCL_ERROR;
				}
				bModal = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "HANDLER")	// ָ��Handler����
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost handlerObj param");
					return TCL_ERROR;
				}
				pTclDuiHandler = (CTclDuiHandler*)(atoi(ConvertUTFParam(Tcl_GetString(objv[i+1]))));
				i++;
			}else
			if(strParam == "HANDLERNAME")	// ָ��Tcl�¼����������
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost handlerName param");
					return TCL_ERROR;
				}
				strTDuiHandlerObj = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "CONTROL")	// ָ��ControlName
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost controlName param");
					return TCL_ERROR;
				}
				strControlName = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "INIT")
			{
				bInit = TRUE;
			}
		}

		CTclDuiDialogObj* pTclDuiDialogObj = new CTclDuiDialogObj(interp);
		void* pDlg = NULL;
		if(!bModal)
		{
			pDlg = pTclDuiDialogObj->CreateDuiDialog(strXmlTemplate, strDlgName, bModal);
		}else
		{
			pDlg = pTclDuiDialogObj->CreateDuiDialogWithHandler(strXmlTemplate, strDlgName, bModal,
				pTclDuiHandler, strTDuiHandlerObj, strControlName, bInit);
		}

		CString strResult;
		strResult.Format("%d", pDlg);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "show")	// ��������ʾ�Ի���
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "show xmlTemplate dlgName isModule");
			return TCL_ERROR;
		}

		CString strXmlTemplate = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strDlgName = ConvertUTFParam(Tcl_GetString(objv[3]));
		BOOL bModule = TRUE;
		if(objc > 4)
		{
			bModule = atoi(ConvertUTFParam(Tcl_GetString(objv[4])));
		}

		int nResult = pIDuiVisionApp->ShowDuiDialog(strXmlTemplate, NULL, strDlgName, bModule);

		// ����Ƿ�ģ̬�Ի���,����ӵ��Ի�������б���
		if(!bModule)
		{
			CDlgBase* pDlg = pIDuiVisionApp->GetDuiDialog(strDlgName);
			CTclDuiDialogObj::AddDuiDialog("", (void*)pDlg, interp);
		}

		CString strResult;
		strResult.Format("%d", nResult);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "messagebox")	// ��ʾ��Ϣ�Ի���
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "messagebox text caption ?type? ?-width width? ?-height height?");
			return TCL_ERROR;
		}

		CString strText = ConvertUTFParam(Tcl_GetString(objv[2]));
		CString strCaption = ConvertUTFParam(Tcl_GetString(objv[3]));
		UINT uType = 0;
		int nWidth = 0;
		int nHeight = 0;
		for(int i = 4; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			if(strParam == "MB_OK")	// ѡ��
			{
				uType |= MB_OK;
			}else
			if(strParam == "MB_OKCANCEL")
			{
				uType |= MB_OKCANCEL;
			}else
			if(strParam == "MB_RETRYCANCEL")
			{
				uType |= MB_RETRYCANCEL;
			}else
			if(strParam == "MB_YESNO")
			{
				uType |= MB_YESNO;
			}else
			if(strParam == "MB_ICONWARNING")
			{
				uType |= MB_ICONWARNING;
			}else
			if(strParam == "MB_ICONINFORMATION")
			{
				uType |= MB_ICONINFORMATION;
			}else
			if(strParam == "MB_ICONQUESTION")
			{
				uType |= MB_ICONQUESTION;
			}else
			if(strParam == "MB_ICONERROR")
			{
				uType |= MB_ICONERROR;
			}else
			if(strParam == "WIDTH")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost width param");
					return TCL_ERROR;
				}
				nWidth = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "HEIGHT")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost height param");
					return TCL_ERROR;
				}
				nHeight = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}
		}

		int nResult = pIDuiVisionApp->DuiMessageBox(NULL, strText, strCaption, uType, nWidth, nHeight);
		CString strResult;
		strResult.Format("%d", nResult);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "showwindow")	// ��ʾ�Ի���
	{
		if(objc < 3)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "showwindow dlgObj show|hide");
			return TCL_ERROR;
		}

		CString strDlg = ConvertUTFParam(Tcl_GetString(objv[2]));
		CDlgBase* pDlg = (CDlgBase*)atoi(strDlg);
		if(pDlg == NULL)
		{
			Tcl_AppendResult(interp, "Dialog point is null!", (char *) NULL);
			return TCL_ERROR;
		}

		CString strShow = ConvertUTFParam(Tcl_GetString(objv[3]));
		if(strShow == _T("show"))
		{
			pDlg->ShowWindow(SW_SHOW);
		}else
		if(strShow == _T("hide"))
		{
			pDlg->ShowWindow(SW_HIDE);
		}else
		{
			Tcl_AppendResult(interp, "Unknown operation " + strShow, (char *) NULL);
			return TCL_ERROR;
		}
	}else
	if(strOpr == "notifymsg")	// ��ʾ��ʾ��Ϣ��
	{
		CString strMsg = ConvertUTFParam(Tcl_GetString(objv[2]));
		
		CString strCaption = "";
		UINT uNotifyType = 2;
		UINT uIconType = 0;
		int nDelayTime = 0;
		int nWidth = 0;
		int nHeight = 0;
		for(int i = 3; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			if(strParam == "MB_ICONINFORMATION")	// ͼ��
			{
				uIconType |= MB_ICONINFORMATION;
			}else
			if(strParam == "MB_ICONQUESTION")
			{
				uIconType |= MB_ICONQUESTION;
			}else
			if(strParam == "MB_ICONWARNING")
			{
				uIconType |= MB_ICONWARNING;
			}else
			if(strParam == "MB_ICONERROR")
			{
				uIconType |= MB_ICONERROR;
			}else
			if(strParam == "TYPE")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost type param");
					return TCL_ERROR;
				}
				uNotifyType = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "CAPTION")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost caption param");
					return TCL_ERROR;
				}
				strCaption = ConvertUTFParam(Tcl_GetString(objv[i+1]));
				i++;
			}else
			if(strParam == "DELAY")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost delayTime param");
					return TCL_ERROR;
				}
				nDelayTime = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "WIDTH")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost width param");
					return TCL_ERROR;
				}
				nWidth = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}else
			if(strParam == "HEIGHT")
			{
				if((objc - i) < 2)
				{
					Tcl_WrongNumArgs(interp, 1, objv, "wrong # args: lost height param");
					return TCL_ERROR;
				}
				nHeight = atoi(ConvertUTFParam(Tcl_GetString(objv[i+1])));
				i++;
			}
		}

		pIDuiVisionApp->AddDuiNotifyMsgTask(strMsg, uIconType | uNotifyType, strCaption, nDelayTime, nWidth, nHeight);
	}else
	if(strOpr == "add")	// ��ӶԻ���
	{
		CString strDlg = ConvertUTFParam(Tcl_GetString(objv[2]));
		CTclDuiDialogObj::AddDuiDialog("", (void*)(atoi(strDlg)), interp);
	}else
	if(strOpr == "remove")	// ɾ���Ի���
	{
		CString strDlg = ConvertUTFParam(Tcl_GetString(objv[2]));
		CTclDuiDialogObj::RemoveDuiDialog((void*)(atoi(strDlg)), interp);
	}else
	if(strOpr == "get")	// ��ȡ�Ի���
	{
		CString strName = ConvertUTFParam(Tcl_GetString(objv[2]));
		CDlgBase* pDlg = pIDuiVisionApp->GetDuiDialog(strName);
		CString strResult;
		strResult.Format("%d", pDlg);
		Tcl_AppendResult(interp, strResult, (char *)NULL);
	}else
	if(strOpr == "close")	// �رնԻ���
	{
		CString strDlg = ConvertUTFParam(Tcl_GetString(objv[2]));
		CDlgBase* pDlg = (CDlgBase*)atoi(strDlg);
		if(pDlg == NULL)
		{
			Tcl_AppendResult(interp, "Dialog point is null!", (char *) NULL);
			return TCL_ERROR;
		}

		CString strCloseType = "CLOSE";
		for(int i = 3; i < objc; i++)
		{
			CString strParam = ConvertUTFParam(Tcl_GetString(objv[i]));
			if((strParam.GetLength() > 0) && (strParam[0] == _T('-')))
				strParam.Delete(0, 1);
			strParam.MakeUpper();

			strCloseType = strParam;
		}

		if(strCloseType == "OK")
		{
			pDlg->DoOK();
		}else
		if(strCloseType == "CANCEL")
		{
			pDlg->DoCancel();
		}else
		if(strCloseType == "CLOSE")
		{
			pDlg->DoClose();
		}else
		if(strCloseType == "YES")
		{
			pDlg->DoYes();
		}else
		if(strCloseType == "NO")
		{
			pDlg->DoNo();
		}
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:dui::timer
// add timerName time
// stop timerName
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DUI_Timer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "operate ...");
		return TCL_ERROR;
	}

	IDuiVisionApp* pIDuiVisionApp = GetIDuiVisionApp(interp);
	if(pIDuiVisionApp == NULL)
	{
		Tcl_AppendResult(interp, "Get duivisionapp interface fail!", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));

	if(strOpr == "add")	// ��Ӷ�ʱ��
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "add timerName time");
			return TCL_ERROR;
		}
		CString strTimername = ConvertUTFParam(Tcl_GetString(objv[2]));
		UINT uTimerResolution = (UINT)atoi(ConvertUTFParam(Tcl_GetString(objv[3])));
		pIDuiVisionApp->AddDuiTimer(uTimerResolution, strTimername);
	}else
	if(strOpr == "stop")	// ֹͣ��ʱ��
	{
		CString strTimername = ConvertUTFParam(Tcl_GetString(objv[2]));
		pIDuiVisionApp->RemoveDuiTimer(strTimername);
	}else
	{
		Tcl_AppendResult(interp, "Unknown operation " + strOpr, (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

//TCL������������
int TclDuiVision_Init(Tcl_Interp *interp)
{
	// ע������

	// DUI��Դ����
	Tcl_CreateObjCommand(interp, "dui::resource", Tcl_Cmd_DUI_Resource, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	
	// DUI�ؼ�����
	Tcl_CreateObjCommand(interp, "dui::control", Tcl_Cmd_DUI_Control, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// DUI GridCtrl�ؼ�����
	Tcl_CreateObjCommand(interp, "dui::gridctrl", Tcl_Cmd_DUI_GridCtrl, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// DUI�¼�����������
	Tcl_CreateObjCommand(interp, "dui::handler", Tcl_Cmd_DUI_Handler, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// DUI�Ի������
	Tcl_CreateObjCommand(interp, "dui::dialog", Tcl_Cmd_DUI_Dialog, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// DUI��ʱ������
	Tcl_CreateObjCommand(interp, "dui::timer", Tcl_Cmd_DUI_Timer, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    return TCL_OK;
}

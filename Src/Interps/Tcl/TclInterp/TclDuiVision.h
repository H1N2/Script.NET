////////////////////////////////////////////////////////////////////////////
//	File:		TclDuiVision.h
//	Version:	1.0.0.0
//	Created:	2017-01-28
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of TclDuiVision.
////////////////////////////////////////////////////////////////////////////
#ifndef __TCLDUIVISION_H_
#define __TCLDUIVISION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DuiVision.h"

class CTclDuiHandler;

// ��װDUI�Ի������
class CTclDuiDialogObj
{
public:
	CTclDuiDialogObj(Tcl_Interp* interp);
	virtual ~CTclDuiDialogObj();

// function
public:
	void* GetDialogObj() { return m_pDialogObj; }
	void SetDialogObj(void* pDialogObj) { m_pDialogObj = pDialogObj; }
	CString GetName() { return m_strName; }
	void SetName(CString strName) { m_strName = strName; }
	CString GetErrorInfo() { return m_strErrorInfo; }
	void SetErrorInfo(CString strErrorInfo) { m_strErrorInfo = strErrorInfo; }

	void* CreateDuiDialog(CString strXmlTemplate, CString strDlgName, BOOL bModule);
	void* CreateDuiDialogWithHandler(CString strXmlTemplate, CString strDlgName, BOOL bModule,
			CTclDuiHandler* pDuiHandler, CString strTDuiHandlerObj, CString strControlName, BOOL bInit);
	static CTclDuiDialogObj* GetDuiDialog(void* pDialogObj);
	static void AddDuiDialog(CString strDlgName, void* pDialogObj, Tcl_Interp* interp);
	static void RemoveDuiDialog(void* pDialogObj, Tcl_Interp* interp);
	void SetInterp(Tcl_Interp* interp) {m_Interp = interp;}
	Tcl_Interp* GetInterp() {return m_Interp;}

// attribute
private:
	void*			m_pDialogObj;	// DUI�Ի������ָ��
	Tcl_Interp*		m_Interp;		// ������ָ��
	CString			m_strName;		// �Ի�����
	CString			m_strErrorInfo;	// ������Ϣ
};

typedef CArray<CTclDuiDialogObj, CTclDuiDialogObj&> CTclDuiDialogObjArray;


// Tcl DUI�¼�������
class CTclDuiHandler : public CDuiHandler
{
public:
	CTclDuiHandler(Tcl_Interp* interp);
	virtual ~CTclDuiHandler(void);

	void SetDialog(CDlgBase* pDlg) { m_pDlg = pDlg; }
	CDlgBase* GetDialog() { return m_pDlg; }
	void SetInterp(Tcl_Interp* interp) {m_Interp = interp;}
	Tcl_Interp* GetInterp() {return m_Interp;}
	void SetTDuiHandlerObj(CString strTDuiHandlerObj) { m_strTDuiHandlerObj = strTDuiHandlerObj; }
	CString GetTDuiHandlerObj() { return m_strTDuiHandlerObj; }
	void SetErrorInfo(CString strErrorInfo) { m_strErrorInfo = strErrorInfo; }
	CString GetErrorInfo() { return m_strErrorInfo; }

	virtual void OnInit();	
	virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	static CTclDuiHandler* RegisterDuiHandler(CDuiObject* pDuiObject, CTclDuiHandler* pDuiHandler,
			CString strTDuiHandlerObj, CString strControlName, BOOL bInit, Tcl_Interp* interp);
	static BOOL IsDuiHandlerExist(CTclDuiHandler* pTclDuiHandler);
	static CTclDuiHandler* AddDuiHandler(CTclDuiHandler* pTclDuiHandler, Tcl_Interp* interp);
	static void RemoveDuiHandler(CTclDuiHandler* pTclDuiHandler, Tcl_Interp* interp);

// attribute
private:
	CDlgBase*		m_pDlg;			// �����ĶԻ������
	Tcl_Interp*		m_Interp;		// ������ָ��
	CString			m_strTDuiHandlerObj;// Tcl�¼����������
	CString			m_strErrorInfo;	// ������Ϣ
};


// �ر�ָ������������������DUI����
void CloseInterpAllDuiObject(Tcl_Interp* interp);

// �ر�����DUI����
void CloseAllDuiObject();

// TclDuiVision��չ����
int TclDuiVision_Init(Tcl_Interp *interp);

#endif	// __TCLDUIVISION_H_
////////////////////////////////////////////////////////////////////////////
//	File:		VciOwm.h
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	VCI OWM Interface
////////////////////////////////////////////////////////////////////////////
#ifndef __VCIOWM_H_
#define __VCIOWM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IID_IVciOwm "IVciOwm"

interface IVciOwm : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	// ��ȡģ����
	virtual int __stdcall GetOwmName(CString& strOwmName) = 0;
	// ��ȡģ������
	virtual int __stdcall GetOwmDesc(CString& strOwmDesc) = 0;
	// ��ȡģ��汾
	virtual int __stdcall GetOwmVersion(int& nVersion) = 0;
	// ��ȡģ������
	virtual int __stdcall GetOwmType(UINT& nType) = 0;
	// ��ȡģ��ID
	virtual int __stdcall GetOwmID(UINT& nID) = 0;
	// ����ƽ̨UI�ӿ�ָ��
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI) = 0;
	// �����ĵ�ģ��ָ��
	virtual int __stdcall SetDocTemplate(CMultiDocTemplate* pDocTemplate) = 0;
	// ��ȡ�ĵ�ģ��ָ��
	virtual int __stdcall GetDocTemplate(CMultiDocTemplate** ppDocTemplate) = 0;
	// ��ȡָ������ʱ��ָ��
	virtual int __stdcall GetRuntimeClass(int nType, CRuntimeClass** ppRuntimeClass) = 0;
	// ��ʾDockingPane����ʱ���õĺ���
	virtual CWnd* __stdcall OnShowDockingPane(int nID, CWnd* pParentWnd, LPCTSTR lpszParam) = 0;
	// OWM�����
	virtual int __stdcall Command(int nCmd, WPARAM wParam, LPARAM lParam) = 0;
	// ע��˵�
	virtual int __stdcall RegisterClientMenu(CMenu* pMenu, COWMMenuArray& aOWMMenus, int nOWMIdStart, CDocument* pDoc) = 0;
	// ж�ز˵�
	virtual int __stdcall UnregisterClientMenu(COWMMenuArray& aOWMMenus) = 0;
	// ע��ToolBar
	virtual int __stdcall RegisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart, int nIndex) = 0;
	// ж��ToolBar
	virtual int __stdcall UnregisterClientToolBar(CWnd* pBar, COWMMenuArray& aOWMMenus, int nOWMIdStart) = 0;
	// ע��OWM����ҳ
	virtual int __stdcall RegisterOption(COWMOptionArray& aOWMOption) = 0;
	// ע���ӡҳ����������ҳ
	virtual int __stdcall RegisterPrintPageOption(COWMOptionArray& aOWMOption) = 0;
	// �����ĵ�Session
	virtual int __stdcall LoadDocumentSession(CDocument* pDoc, CStringArray& asSessionLine) = 0;
	// �����ĵ�Session
	virtual int __stdcall SaveDocumentSession(CDocument* pDoc, CStringArray& asSessionLine) = 0;
	// �����ĵ�
	virtual int __stdcall BackupDocument(CDocument* pDoc, LPCTSTR lpszBackupFile, int nBackupType) = 0;
	// ��ȡ��������
	virtual int __stdcall GetImportTypeList(CStringArray& asImportType) = 0;
	// ��ȡ��������
	virtual int __stdcall GetExportTypeList(CStringArray& asExportType) = 0;
	// �����ĵ�
	virtual int __stdcall ImportDocument(CDocument* pDoc, LPCTSTR lpszImportSource, int nImportType) = 0;
	// �����ĵ�
	virtual int __stdcall ExportDocument(CDocument* pDoc, LPCTSTR lpszExportDest, int nExportType) = 0;
	// ����(pDocΪ�ձ�ʾ�������ĵ�)
	virtual int __stdcall Zoom(float fZoomFactor, CDocument* pDoc) = 0;
	// ��ȡ��ǰ��������(pDocΪ�ձ�ʾ�������ĵ�)
	virtual int __stdcall GetZoomFactor(float& fZoomFactor, CDocument* pDoc) = 0;
	// ��ȡ�����б�
	virtual int __stdcall GetZoomFactorRange(CStringArray& asZoomFactor, float& fZoomMin, float& fZoomMax) = 0;
};


#endif // __VCIOWM_H_
#if !defined(__VCI_INTERFACE_CProjectManager_H_INCLUDED__)
#define __VCI_INTERFACE_CProjectManager_H_INCLUDED__


#include "vcicomm.h"
#include "IProjectManager.h"

#define INF_IProjectManager \
{\
	"IProjectManager",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"{6F20ED82-A398-44CF-A45B-1E1906DA2C6A}",\
	"Project manager",\
	"blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IProjectManager = INF_IProjectManager;

class CProjectManager : public CVisualComponent
{
public:
	CProjectManager();
	~CProjectManager();

//////////////////////////////////////////////////////////////////////////
// �ṩ���ӿڵĹ��ܺ���
//////////////////////////////////////////////////////////////////////////
public:
	//{{VCI_IMPLEMENT_DEFINE_BEGIN
	// �����½�������
	virtual int __stdcall NewProjectWizard(CString& strPrjVci, CString& strPrjFile);
	// �����½��ļ���
	virtual int __stdcall NewFileWizard();
	//}}VCI_IMPLEMENT_DEFINE_END

	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData);
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler);
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut);

//////////////////////////////////////////////////////////////////////////
// Ҫ����Ĺ��ܽӿ�
//////////////////////////////////////////////////////////////////////////
protected:
	// �ӿڶ���
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

	BEGIN_INTERFACE_PART(ProjectManager, IProjectManager)
		//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
		// ��ȡ�ӿڵĶ�����Ϣ
		virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData);
		//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	//{{VCI_INTERFACE_PART_BEGIN
	// �����½�������
	virtual int __stdcall NewProjectWizard(CString& strPrjVci, CString& strPrjFile);
	// �����½��ļ���
	virtual int __stdcall NewFileWizard();
	//}}VCI_INTERFACE_PART_END
	END_INTERFACE_PART(ProjectManager)
	EXPORT_INTERFACE_PART(ProjectManager)
};


#endif // !defined(__VCI_INTERFACE_CProjectManager_H_INCLUDED__)

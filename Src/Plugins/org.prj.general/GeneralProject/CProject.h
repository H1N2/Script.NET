#if !defined(__VCI_INTERFACE_CProject_H_INCLUDED__)
#define __VCI_INTERFACE_CProject_H_INCLUDED__


#include "vcicomm.h"
#include "IProject.h"

#define INF_IProject \
{\
	"IProject",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"{6F20ED82-A398-44CF-A45B-1E1906DA2C6A}",\
	"General project interface",\
	"blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IProject = INF_IProject;

class CProject : public CVisualComponent
{
public:
	CProject();
	~CProject();

public:
	CXmlParser		m_xmlProject;		// �����ļ�
	CString			m_strProjectName;
	CString			m_strProjectDesc;
	CString			m_strProjectFile;
	CString			m_strProjectPath;

//////////////////////////////////////////////////////////////////////////
// �ṩ���ӿڵĹ��ܺ���
//////////////////////////////////////////////////////////////////////////
public:
	//{{VCI_IMPLEMENT_DEFINE_BEGIN
		PROJECT_INTERFACE_DEFINE
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

	BEGIN_INTERFACE_PART(Project, IProject)
		//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
		// ��ȡ�ӿڵĶ�����Ϣ
		virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData);
		//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	//{{VCI_INTERFACE_PART_BEGIN
		PROJECT_INTERFACE_DEFINE
	//}}VCI_INTERFACE_PART_END
	END_INTERFACE_PART(Project)
	EXPORT_INTERFACE_PART(Project)
};


#endif // !defined(__VCI_INTERFACE_CProject_H_INCLUDED__)

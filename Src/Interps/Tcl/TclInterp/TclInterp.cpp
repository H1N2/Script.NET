// TclInterp.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "CTclInterp.h"
#include "TclPcom.h"
#include "TclXml.h"
#include "wait.h"
#include "resource.h"
#include "TpsTclInterp.h"
#include "TpsTclDebug.h"
#include "TpsTclCompile.h"
#include "TpsTclBuild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE TclInterpDLL = { NULL, NULL };

HINSTANCE GetDllInstance()
{
	return TclInterpDLL.hModule;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("TCLINTERP.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(TclInterpDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(TclInterpDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		CloseAllVciObject();

		CloseAllTclXmlFile();

		DeleteAllWaitObject();

		TRACE0("TCLINTERP.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(TclInterpDLL);
	}
	return 1;   // ok
}

/////////////////////////////////////////////////////////////////////////////
// ��Դ�л�����
// get_dll_resource   �л���DLL����Դ
// reset_dll_resource �ָ���ǰ����Դ
/////////////////////////////////////////////////////////////////////////////
static int resource_counter = 0;		// ��Դ����
static HINSTANCE save_hInstance = NULL;	// ��Դ����

void get_dll_resource(void)
{
   if (resource_counter == 0)
   {
	   save_hInstance = AfxGetResourceHandle();
	   AfxSetResourceHandle(GetDllInstance());
   }

   resource_counter++;
}

void reset_dll_resource(void)
{
   if (resource_counter > 0)
      resource_counter--;

   if (resource_counter == 0)
      AfxSetResourceHandle(save_hInstance);
}

//////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
const TInterfaceInfo INTERFACE_LIST[] =
{
	INF_IInterp,
};


//////////////////////////////////////////////////////////////////////////
// �� DLL ����ĺ���
//////////////////////////////////////////////////////////////////////////
//
// ��ѯ�� DLL ����Ľӿ��б����ؽӿ���Ŀ
//
extern "C" __declspec(dllexport)
int GetInterfaceList(const TInterfaceInfo **pInfo)
{
	*pInfo = INTERFACE_LIST;
	return (sizeof(INTERFACE_LIST)/sizeof(INTERFACE_LIST[0]));
}

//
// ע������ҳ
//
extern "C" __declspec(dllexport)
int gRegisterPropertyPage(CVciPropertyPageArray& aPropertyPage)
{
	// �ڴ��������ҳ
	DLL_LOADSTRING(strTpsTclInterp, IDS_TPS_TCL_INTERP);	// Tcl������
	DLL_LOADSTRING(strTpsAboutTclInterp, IDS_TPS_ABOUT_TCL_INTERP);	// ����Tcl������...
	VCI_REGISTER_PROPERTYPAGE("", strTpsTclInterp, strTpsAboutTclInterp, IDD_TPS_TCLINTERP, CTpsTclInterp, 0)

	// ���procomp.exe�ļ�,���û��,��ʾδ��װ������Tcl������,����ԡ����롢����������ҳ������ʾ
	CString strProCompFile	= GetInterpRootPath();
	strProCompFile	+= "bin\\procomp.exe";
	if(GetFileAttributes(strProCompFile) != 0xFFFFFFFF)
	{
		DLL_LOADSTRING(strTpsTclDebug, IDS_TPS_TCL_DEBUG);	// ���Բ���
		DLL_LOADSTRING(strTpsAboutTclDebug, IDS_TPS_ABOUT_TCL_DEBUG);	// ����Tcl���Բ���...
		VCI_REGISTER_PROPERTYPAGE(strTpsTclInterp, strTpsTclDebug, strTpsAboutTclDebug, IDD_TPS_TCL_DEBUG, CTpsTclDebug, 0)
		DLL_LOADSTRING(strTpsTclComplie, IDS_TPS_TCL_COMPILE);	// ����ѡ��
		DLL_LOADSTRING(strTpsAboutTclCompile, IDS_TPS_ABOUT_TCL_COMPILE);	// ����Tcl����ѡ��...
		VCI_REGISTER_PROPERTYPAGE(strTpsTclInterp, strTpsTclComplie, strTpsAboutTclCompile, IDD_TPS_TCL_COMPILE, CTpsTclCompile, 0)
		DLL_LOADSTRING(strTpsTclBuild, IDS_TPS_TCL_BUILD);	// ����ѡ��
		DLL_LOADSTRING(strTpsAboutTclBuild, IDS_TPS_ABOUT_TCL_BUILD);	// ����Tcl����ѡ��...
		VCI_REGISTER_PROPERTYPAGE(strTpsTclInterp, strTpsTclBuild, strTpsAboutTclBuild, IDD_TPS_TCL_BUILD, CTpsTclBuild, 0)
	}

	return aPropertyPage.GetSize();
}

//
// �������󲢷��ض���Ľӿ�
//
extern "C" __declspec(dllexport)
LPVOID CreateObject(LPCSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData)
{
	TRACE("CreateObject(\"%s\")\n", lpcsInterfaceName);

	CVisualComponent *pObject;
	if(strcmp(lpcsInterfaceName, IID_IInterp) == 0)
	{
		pObject = new CTclInterp;
	}
	else
	{
		TRACE("Can't find Object: %s, CreateObject failed!\n", lpcsInterfaceName);
		return NULL;
	}

	// Ϊ�豸��װ������Ϣ����ص�����
	if(lpInitData)
	{
		TInitData_DebugOut *pInitData_DebugOut = (TInitData_DebugOut *)lpInitData;
		pObject->InstallHandler(DEBUG_OUT_EVENT, pInitData_DebugOut->lpfnDebugOut, pInitData_DebugOut->nDeviceID);
	}

	*lppVciControl = pObject->VciControlQueryInterface();

	return pObject->ExternalQueryInterface();
}

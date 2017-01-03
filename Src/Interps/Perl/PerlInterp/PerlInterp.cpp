// PerlInterp.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "CPerlInterp.h"
#include "TpsPerlInterp.h"
#include "TpsPerlDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE PerlInterpDLL = { NULL, NULL };

HINSTANCE GetDllInstance()
{
	return PerlInterpDLL.hModule;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PERLINTERP.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(PerlInterpDLL, hInstance))
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

		new CDynLinkLibrary(PerlInterpDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PERLINTERP.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(PerlInterpDLL);
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
	DLL_LOADSTRING(strTpsPerlInterp, IDS_TPS_PERL_INTERP);	// Perl������
	DLL_LOADSTRING(strTpsAboutPerlInterp, IDS_TPS_ABOUT_PERL_INTERP);	// ����Perl������...
	VCI_REGISTER_PROPERTYPAGE("", strTpsPerlInterp, strTpsAboutPerlInterp, IDD_TPS_PERLINTERP, CTpsPerlInterp, 0);

	DLL_LOADSTRING(strTpsPerlDebug, IDS_TPS_PERL_DEBUG);	// ���Բ���
	DLL_LOADSTRING(strTpsAboutPerlDebug, IDS_TPS_ABOUT_PERL_DEBUG);	// ����Perl���Բ���...
	VCI_REGISTER_PROPERTYPAGE(strTpsPerlInterp, strTpsPerlDebug, strTpsAboutPerlDebug, IDD_TPS_PERL_DEBUG, CTpsPerlDebug, 0)

	//DLL_LOADSTRING(strTpsPerlBuild, IDS_TPS_PERL_BUILD);	// ����ѡ��
	//DLL_LOADSTRING(strTpsAboutPerlBuild, IDS_TPS_ABOUT_PERL_BUILD);	// ����Perl����ѡ��...
	//VCI_REGISTER_PROPERTYPAGE(strTpsPerlInterp, strTpsPerlBuild, strTpsAboutPerlBuild, IDD_TPS_PERL_BUILD, CTpsPerlBuild, 0)

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
		pObject = new CPerlInterp;
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

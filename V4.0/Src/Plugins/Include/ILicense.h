////////////////////////////////////////////////////////////////////////////
//	File:		ILicense.h
//	Version:	2.0.0.0
//	Created:	2000-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	license interface define
////////////////////////////////////////////////////////////////////////////
#if !defined(__VCI_INTERFACE_ILicense_H_INCLUDED__)
#define __VCI_INTERFACE_ILicense_H_INCLUDED__


#define IID_ILicense "ILicense"

namespace License {
// License����ֵ����
enum ETJResult
{
    trOk             = 0,                       //ִ�гɹ�
	trLicenseFail	 = 100,						//LicenseУ��ʧ��
	trLicenseExpire		,						//License����
	trLicenseDriver		,						//û����Ӧ������License��Ȩ
	trLicenseUpdateExpire,						//License����������,����ʹ�õ�ǰ�汾,������ʹ���ϰ汾
	
	trUnknownErr    	,	                    //δ֪����
    //����ǰ����������ķ�����
    trLast                                     //���ķ���ֵ
};

// License�����������
enum{
	FUNC_ACTION_LOAD	= 0,	// �������
	FUNC_ACTION_CHECKLICENSE,	// ���License��Ч��
	FUNC_ACTION_RUNFILE,		// ִ���ļ�
	FUNC_ACTION_RUNCMD,			// ִ������
	FUNC_ACTION_DEBUG,			// ����
	FUNC_ACTION_COMPILE,		// ����
	FUNC_ACTION_BUILD,			// ����
	FUNC_ACTION_TEST,			// ����
	FUNC_ACTION_REFACTOR,		// �ع�
	FUNC_ACTION_UIDESIGN,		// ������ӻ����
	FUNC_ACTION_MODEL,			// ��ģ
	FUNC_ACTION_WEBDEV,			// WEB����
};


// License���Ͷ���
enum{
	LICENSE_NORMAL,				// ��ʽLicense
	LICENSE_TRIAL,				// ����License
	LICENSE_ERROR,				// ����License
	LICENSE_FREE,				// Free License
};
};

interface ILicense : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
	// Ӧ�ó�����ڶԻ���
	virtual int __stdcall ApplicationAbout() = 0;
	// ����Ĺ��ڶԻ���
	virtual int __stdcall ComponentAbout(LPCTSTR lpszVciId) = 0;
	// У��License���
	virtual int __stdcall VerifyLicenseComponent(int nCmd, LPCTSTR lpszParamIn, CString& strParamOut) = 0;
	// �����LicenseУ��
	virtual int __stdcall VerifyPluginLicense(LPCTSTR lpszVciId, int nAction) = 0;
	// У���ƷLicense
	virtual int __stdcall VerifyProductLicense() = 0;
	// У�鹦��License
	virtual int __stdcall VerifyFunctionLicense(int nFunction, int nValue) = 0;
	// ��ȡ��Ʒ���к�
	virtual int __stdcall GetProductNo(CString& strProductNo) = 0;
	// ��ȡ���������ַ���
	virtual int __stdcall GetUpgradeRequestString(CString& strRequest) = 0;
	// ����License�ļ�
	virtual int __stdcall UpgradeLicenseFile(LPCTSTR lpszUpgradeFile) = 0;
	// ��ȡӲ��Ψһ���к�
	virtual int __stdcall GetESN(CString& strESN) = 0;
	// ����License ID
	virtual int __stdcall GenerateLicenseID(LPCTSTR lpszESN, LPCTSTR lpszMaskCode, CString& strLicenseID) = 0;
	// ����License ID
	virtual int __stdcall ImportLicenseID(LPCTSTR lpszLicenseID) = 0;
	// У��License ID
	virtual int __stdcall VerifyLicenseID(LPCTSTR lpszFunction) = 0;
	// ��ȡLicense ID
	virtual int __stdcall GetLicenseID(CString& strLicenseID) = 0;
	// ��ȡ�����License��Ϣ
	virtual int __stdcall GetPluginLicenseInfo(LPCTSTR lpszVciId, int& nLicenseType, CTime& tUpdateLimit, int& nUserCount, CTime& tTrialTime) = 0;
	// ��ȡ��Ȩ�û���Ϣ
	virtual int __stdcall GetCustomerInfo(CString& strCustomerInfo) = 0;
	// ��ȡ������Ϣ
	virtual int __stdcall GetOrderInfo(CString& strOrderInfo) = 0;
	//}}VCI_INTERFACE_END
};


#endif // !defined(__VCI_INTERFACE_ILicense_H_INCLUDED__)

#if !defined(__PCOM_INTERFACE_IRemoteClient_H_INCLUDED__)
#define __PCOM_INTERFACE_IRemoteClient_H_INCLUDED__

#include "RcResult.h"

#define IID_IRemoteClient     "IRemoteClient"

#define    RC_SOCKET_SERVER              1116 // Զ�̷���˿�

interface IRemoteClient : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//����������紦��ʱ��
	virtual STDMETHODIMP_(void) SetMaxProcTime(DWORD dwMaxProcTime) = 0;

	//���Ӳ���¼������
	virtual STDMETHODIMP_(ERCResult) ConnectAndLogin(CString strServerAddr, CString strUser, CString strPassword, DWORD dwPort = RC_SOCKET_SERVER) = 0;
	//�Ͽ�����������
	virtual STDMETHODIMP_(ERCResult) DisconnectServer(void) = 0;

	//��ȡƽ̨�汾
	virtual STDMETHODIMP_(ERCResult) ProcessGetPlatVersion(CString& strReturn) = 0;
	//����ƽ̨�����°汾
	virtual STDMETHODIMP_(ERCResult) ProcessSetPlatNewVersion(CString strVer, CString strSubVer) = 0;
	//ִ��TCL����
	virtual STDMETHODIMP_(ERCResult) ProcessTclCommand(CString strCommand, CString& strReturn) = 0;
	//����������
	virtual STDMETHODIMP_(ERCResult) ProcessOpenInterpreter(CString strName, int& nInterpreterID) = 0;
	//�رսű�������
	virtual STDMETHODIMP_(ERCResult) ProcessCloseInterpreter(int nInterpreterID) = 0;
	//��ȡ������ID
	virtual STDMETHODIMP_(ERCResult) ProcessGetInterpreterID(CString strName, int& nInterpreterID) = 0;
	//ִ�нű��ļ�
	virtual STDMETHODIMP_(ERCResult) ProcessEvalFile(int nInterpreterID, CString strFileName, CString& strReturn) = 0;
	//ִ�нű�����
	virtual STDMETHODIMP_(ERCResult) ProcessEvalCommand(int nInterpreterID, CString strCommand, CString& strReturn) = 0;
	//��ȡ�����к�
	virtual STDMETHODIMP_(ERCResult) ProcessGetErrorLine(int nInterpreterID, int& nErrorLine) = 0;

};

#endif // !defined(__PCOM_INTERFACE_IRemoteClient_H_INCLUDED__)

#pragma once
#include <atlstr.h>
#include "sysobj.h"

// ��Ϣ���ջص�
typedef int (*RECV_CALLBACK) (void *pData);

// �¼�����ص�
typedef int (*EVENT_CALLBACK) (int nEventCode);

// ��־�ص�
typedef void (*PRINT_LOG_CALLBACK) (LPCSTR lpData);

enum EVENT_CODE
{
	TCP_CONNECT = 0x00, // ���ӳɹ�
	TCP_DISCONNECT,     // �����ر�����
	TCP_ERROR,          // �����쳣�ж� 
};

#define DEFAULT_BUFLEN 1024 // Ĭ�Ͻ������ݰ��Ĵ�С

class CDebugSocket
{
public:
	~CDebugSocket(void);

	static CDebugSocket *Instance();

	// socket�����
	// ��������
	int StartServer(unsigned short uPort = 17664);

	// ֹͣ����
	BOOL StopServer();

	// ���������߳�
	int StartServerThread(unsigned short uPort = 17664);

	// ������Ϣ��Socket�ͻ���
	int SendCmdToClient(const char *pMsg);

	// socket�ͻ���
	// ��������
	int Connect(const char *pszIP, unsigned short uPort);

	// �ر�����
	int Disconnect();

	// ������Ϣ
	int SendMsg(const char *pData);

	 // ��ȡ��ǰ���ӷ�����IP��ַ
	CString GetLocalIP();

	CString GetServerAddr();

	// ע����Ϣ���ջص�
	void RegRecvMsgCallBack(RECV_CALLBACK pRecv);

	// ע��״̬ͨ��ص�
	void RegEventCallBack(EVENT_CALLBACK pEvent);

	// ע����־��ӡ�ӿ�
	void RegPrintLog(PRINT_LOG_CALLBACK pLog);

	bool IsConnect() 
	{
		CAutoLockWrap<CCSWrap> guard(m_lockConnect);
		return m_bConnct;
	};

	void Destroy();

private:
	void SetConnctState(bool bState) 
	{
		CAutoLockWrap<CCSWrap> guard(m_lockConnect);
		m_bConnct = bState;
	};

	// �����߳�
	static DWORD ServerThreadProc(LPVOID lpParam);

	// ���ݽ����߳�
	static DWORD RecvThreadProc(LPVOID lpParam);

	CString GetStrByINT(int nData);

	void GetLocalIP(const char *pszIP, unsigned short uPort);

	void PrintLog(LPCSTR lpData);
	void SetCallbackEvent(int nEventCode);

	CString GetErrorCode(DWORD dwErrorCode);

private:
	CDebugSocket(void);

	static CDebugSocket		m_Instance;

	SOCKET					m_Socket;	// ����˵ļ���socket
	SOCKET					m_SockConn;	// ����˵�����socket
	bool					m_bConnct;
    CCSWrap					m_lockConnect;
	CCSWrap					m_lockSend;
 
	bool					m_bRecvThreadRun;

	HANDLE					m_ServerThreadHandle;
	HANDLE					m_RecvThreadHandle;

	RECV_CALLBACK			m_lpfnRecvMsg;		// ������Ϣ�ص�
	EVENT_CALLBACK			m_lpfnEvent;		// �¼��ص�
	PRINT_LOG_CALLBACK		m_lpfnLogPrint;		// ��־��ӡ

	CString					m_strServerAddr;	// ��������ַ
	CString					m_strLocalIP;		// ��ȡ����IP 
};


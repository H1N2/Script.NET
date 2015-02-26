#include "StdAfx.h"
#include "DbgSocket.h"
#include <list>
#include <string>
#include <vector>

CDebugSocket CDebugSocket::m_Instance;

#pragma comment(lib, "Ws2_32.lib")

CDebugSocket::CDebugSocket(void)
{
	m_lpfnRecvMsg = NULL;
	m_lpfnEvent   = NULL;
	m_lpfnLogPrint = NULL;
	m_Socket = INVALID_SOCKET;
	m_SockConn = INVALID_SOCKET;
	m_bConnct = false;
	m_bRecvThreadRun = true;
	m_ServerThreadHandle = NULL;
	m_RecvThreadHandle = NULL;
	// ��ʼ��socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
}

CDebugSocket::~CDebugSocket(void)
{

}

CDebugSocket *CDebugSocket::Instance()
{
	return &m_Instance;
}

void CDebugSocket::Destroy()
{
	try
	{
		StopServer();	// ֹͣ�����߳�
		Disconnect();	// �Ͽ�socket�ͻ�������
		m_bRecvThreadRun = false;

		DWORD dwExitCode = 0;
		if (m_RecvThreadHandle)
		{
			TerminateThread(m_RecvThreadHandle, dwExitCode);
			WaitForSingleObject(m_RecvThreadHandle,INFINITE);
			CloseHandle(m_RecvThreadHandle);
		}

		WSACleanup();
	}
	catch (...)
	{
	}
}

void CDebugSocket::RegRecvMsgCallBack(RECV_CALLBACK pRecv)
{
	m_lpfnRecvMsg = pRecv;
}

void CDebugSocket::RegEventCallBack(EVENT_CALLBACK pEvent)
{
	m_lpfnEvent = pEvent;
}

void CDebugSocket::RegPrintLog(PRINT_LOG_CALLBACK pLog)
{
	m_lpfnLogPrint = pLog;
}

// ��������
int CDebugSocket::StartServer(unsigned short uPort)
{
	// ��ʼ��Socket
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET) 
	{
		return -1;
	}

	// ���ü�����ַ�Ͷ˿�
	SOCKADDR_IN serverAddr;
	ZeroMemory((char *)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(uPort);           // ���ؼ����˿�
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //���ؼ���IP

	// �󶨼���socket
	bind(m_Socket,(struct sockaddr *)&serverAddr,sizeof(serverAddr));

	// ��ʼ����
	listen(m_Socket,1);

	PrintLog("CDebugSocket::StartServer start listen");

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	m_SockConn = accept(m_Socket,(SOCKADDR*)&addrClient,&len);
	if(m_SockConn)
	{
		SetConnctState(true);
		//SetCallbackEvent(TCP_CONNECT);
		PrintLog("CDebugSocket::StartServer accept socket conn succ");
	}else
	{
		PrintLog("CDebugSocket::StartServer accept socket conn failed");
		return -2;
	}

	char szRecvbuf[DEFAULT_BUFLEN] = {0x00};
	int nRecvbuflen = DEFAULT_BUFLEN;

	char szTmp[DEFAULT_BUFLEN * 2] = {0x00}; // ���ݻ�����
	int  nTmpLen = 0;                        // ���ݻ��������Ƿ�������δ����
	bool bIsNewMsg = true;                   // ��ʶ�Ƿ���һ���µ����ݰ�

	unsigned char *pMsgData = NULL;			// ����

	while(m_SockConn != INVALID_SOCKET)
	{
		// ��������
		memset(szRecvbuf, 0x00, DEFAULT_BUFLEN);
		nRecvbuflen = DEFAULT_BUFLEN;
		int iResult = recv(m_SockConn, szRecvbuf, nRecvbuflen, 0);
		if ( iResult < 0)
		{
			CString strLog = "recv error, errorcode:";
			strLog += GetErrorCode(GetLastError());
			PrintLog(strLog.GetBuffer(0));

			// ���ӷ�������
			DWORD dwError = GetLastError();
			SetCallbackEvent(TCP_ERROR);
			closesocket(m_SockConn);
			m_SockConn = INVALID_SOCKET;

			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			
			SetConnctState(false);
			nTmpLen = 0; 
			memset(szTmp, 0x00, DEFAULT_BUFLEN * 2);
			bIsNewMsg = true;

			if (NULL != pMsgData)
			{
				delete []pMsgData;
				pMsgData = NULL;
			}

			return -2;
		}

		// ���ճɹ�,���ݴ���
		if (m_lpfnRecvMsg && (iResult > 0))
		{
			m_lpfnRecvMsg(szRecvbuf); 
		}

		Sleep(10);
	}

	return 0;
}

// ֹͣ����
BOOL CDebugSocket::StopServer()
{
	PrintLog("CDebugSocket::StopServer");

	// �ر�����socket
	if (m_SockConn != INVALID_SOCKET)
	{
		closesocket(m_SockConn);
		m_SockConn = INVALID_SOCKET;
	}

	// �رռ���socket
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	// �޸�����״̬��Ϣ
	SetConnctState(false);
	SetCallbackEvent(TCP_DISCONNECT);

	return TRUE;
}

static bool bServerThread = false;	// �����߳��Ƿ��Ѵ���
// �����߳�
DWORD CDebugSocket::ServerThreadProc(LPVOID lpParam)
{
	// ������Ч�Լ��
	if (NULL == lpParam)
	{
		return 0;
	}

	CDebugSocket *pDbgSocket = (CDebugSocket*)lpParam;
	pDbgSocket->StartServer();
	bServerThread = false;

	return 1;
}

// ����socket�����߳�
int CDebugSocket::StartServerThread(unsigned short uPort)
{
	if (!bServerThread)
	{
		bServerThread = true;
		DWORD dwThreadId = 0;
		// ����socket server�߳�
		PrintLog("Create Server Thread...");
		m_ServerThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CDebugSocket::ServerThreadProc,
			(LPVOID) this,0, &dwThreadId);
		if (NULL == m_ServerThreadHandle)
		{
			return -1;
		}
	}
	return 0;
}

// ���������Socket�ͻ���
int CDebugSocket::SendCmdToClient(const char *pMsg)
{
	// ����Ҫ���͵�����
	if (m_SockConn != INVALID_SOCKET)
	{
		unsigned char *pData = NULL;
		int nDataLen = strlen(pMsg);
		pData = new BYTE[nDataLen+1];
		memset(pData, 0x00, nDataLen+1);
		memcpy(pData, pMsg, nDataLen);

		int iResult = 0;
		{
			CAutoLockWrap<CCSWrap> guard(m_lockSend);
			iResult = send( m_SockConn, (char*)pData, nDataLen, 0 );
		}

		if (pData != NULL)
		{
			delete [] pData;
			pData = NULL;
		}

		if (iResult == SOCKET_ERROR) 
		{
			SetCallbackEvent(TCP_ERROR);
			closesocket(m_SockConn);
			m_SockConn = INVALID_SOCKET;

			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			SetConnctState(false);
			return -1;
		}

		return 0;
	}
	else
	{
		return -1;
	}
}

// �����µ�����
int CDebugSocket::Connect(const char *pszIP, unsigned short uPort)
{
	static bool bRecvThread = false;
	if (!bRecvThread)
	{
		bRecvThread = true;
		DWORD dwThreadId = 0;
		// �������ݽ����߳�

		PrintLog("Create Recv Thread...");
		m_RecvThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CDebugSocket::RecvThreadProc,
			(LPVOID) this,0, &dwThreadId);
		if (NULL == m_RecvThreadHandle)
		{
			return -1;
		}
	}
	// �ȹرվɵ����ӣ�����һ���ͻ��˽����������
	Disconnect();

	// ��ʼ��Socket
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET) 
	{
		return -1;
	}

	m_strServerAddr = pszIP;

	struct sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( pszIP );
	clientService.sin_port = htons( uPort );

	// ��������
	int iResult = connect( m_Socket, (SOCKADDR*) &clientService, sizeof(clientService) );
	if (iResult == SOCKET_ERROR) 
	{
		CString strLog = "Connect fail,errorCode:";
		strLog += GetErrorCode(GetLastError());
		PrintLog(strLog.GetBuffer(0));
		closesocket(m_Socket);
		return -1;
	}

	//���ӽ����ɹ�����SocketΪ����ģʽ
	u_long iMode = 0;
	ioctlsocket(m_Socket, FIONBIO, &iMode);

	GetLocalIP(pszIP, uPort);

	SetConnctState(true);
	SetCallbackEvent(TCP_CONNECT);
	return 0;
}

// �ر�����
int CDebugSocket::Disconnect()
{
	if (IsConnect()) // �����ǰ��������״̬��ر����ӣ�����������
	{
		PrintLog("Start close connect.");
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		SetConnctState(false);
		SetCallbackEvent(TCP_DISCONNECT);
	}

	return 0;
}

CString CDebugSocket::GetStrByINT(int nData)
{
	char szBuf[10] = {0x00};
	itoa(nData, szBuf, 10);

	CString strData = szBuf;
	return strData;
}

// ������Ϣ
int CDebugSocket::SendMsg(const char *pMsg)
{
	// ����Ҫ���͵�����
	if (IsConnect())
	{
		unsigned char *pData = NULL;
		int nDataLen = 0;
		//CreateSendData(pMsg, &pData, nDataLen);

		int iResult = 0;
		{
			CAutoLockWrap<CCSWrap> guard(m_lockSend);
			iResult = send( m_Socket, (char*)pData, nDataLen, 0 );
		}

		if (pData != NULL)
		{
			delete [] pData;
			pData = NULL;
		}

		if (iResult == SOCKET_ERROR) 
		{
			SetCallbackEvent(TCP_ERROR);
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			SetConnctState(false);
			return -1;
		}

		return 0;
	}
	else
	{
		return -1;
	}
}

// ��Ϊ�ͻ�������µĽ��������߳�
DWORD CDebugSocket::RecvThreadProc(LPVOID lpParam)
{
	// ������Ч�Լ��
	if (NULL == lpParam)
	{
		return 0;
	}

	CDebugSocket *pDbgSocket = (CDebugSocket*)lpParam;

	char szRecvbuf[DEFAULT_BUFLEN] = {0x00};
	int nRecvbuflen = DEFAULT_BUFLEN;

	char szTmp[DEFAULT_BUFLEN * 2] = {0x00}; // ���ݻ�����
	int  nTmpLen = 0;                        // ���ݻ��������Ƿ�������δ����
	bool bIsNewMsg = true;                   // ��ʶ�Ƿ���һ���µ����ݰ�

	unsigned char *pMsgData;				// ����

	// ��ʼ��������
	while(pDbgSocket->m_bRecvThreadRun)
	{
		if (pDbgSocket->IsConnect())
		{
			// ��������
			memset(szRecvbuf, 0x00, DEFAULT_BUFLEN);
			nRecvbuflen = DEFAULT_BUFLEN;
			int iResult = recv(pDbgSocket->m_Socket, szRecvbuf, nRecvbuflen, 0);
			if ( iResult <= 0)
			{
				CString strLog = "recv error, errorcode:";
				strLog += pDbgSocket->GetErrorCode(GetLastError());
				pDbgSocket->PrintLog(strLog.GetBuffer(0));

				// ���ӷ�������
				DWORD dwError = GetLastError();
				pDbgSocket->SetCallbackEvent(TCP_ERROR);
				closesocket(pDbgSocket->m_Socket);
				pDbgSocket->m_Socket = INVALID_SOCKET;
				pDbgSocket->SetConnctState(false);
				nTmpLen = 0; 
				memset(szTmp, 0x00, DEFAULT_BUFLEN * 2);
				bIsNewMsg = true;

				if (NULL != pMsgData)
				{
					delete []pMsgData;
					pMsgData = NULL;
				}
			}
			else
			{
				// ���ճɹ�,���ݴ���
				
			}
		}
		else
		{
			bIsNewMsg = true;
			nTmpLen = 0; 
			memset(szTmp, 0x00, DEFAULT_BUFLEN * 2);

			if (NULL != pMsgData)
			{
				delete []pMsgData;
				pMsgData = NULL;
			}

			// ��ǰ����δ����״̬�����ߵȵ����ӳɹ�
			Sleep(10);
		}
	}

	return 1;
}

// ��ȡ���ӳɹ��ı��ص�ַ
CString CDebugSocket::GetLocalIP()
{
	return m_strLocalIP;
}

CString CDebugSocket::GetServerAddr()
{
	return m_strServerAddr;
}

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void CDebugSocket::GetLocalIP(const char *pszIP, unsigned short uPort)
{
	// Declare and initialize variables
	PMIB_TCPTABLE pTcpTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	char szLocalAddr[128] = {0x00};
	char szRemoteAddr[128] = {0x00};

	struct in_addr IpAddr;

	int i;

	pTcpTable = (MIB_TCPTABLE *) MALLOC(sizeof (MIB_TCPTABLE));
	if (pTcpTable == NULL) {
		return;
	}

	dwSize = sizeof (MIB_TCPTABLE);
	// Make an initial call to GetTcpTable to
	// get the necessary size into the dwSize variable
	if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) ==
		ERROR_INSUFFICIENT_BUFFER) {
			FREE(pTcpTable);
			pTcpTable = (MIB_TCPTABLE *) MALLOC(dwSize);
			if (pTcpTable == NULL) {
				return;
			}
	}
	// Make a second call to GetTcpTable to get
	// the actual data we require
	if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) 
	{
		printf("\tNumber of entries: %d\n", (int) pTcpTable->dwNumEntries);
		for (i = 0; i < (int) pTcpTable->dwNumEntries; i++) 
		{
			if (MIB_TCP_STATE_ESTAB == pTcpTable->table[i].dwState)
			{
				IpAddr.S_un.S_addr = (u_long) pTcpTable->table[i].dwRemoteAddr;
				strcpy_s(szRemoteAddr, sizeof (szRemoteAddr), inet_ntoa(IpAddr));

				if (ntohs((u_short)pTcpTable->table[i].dwRemotePort) == uPort &&
					stricmp(szRemoteAddr, pszIP) == 0)
				{
					IpAddr.S_un.S_addr = (u_long) pTcpTable->table[i].dwLocalAddr;
					strcpy_s(szLocalAddr, sizeof (szLocalAddr), inet_ntoa(IpAddr));
					m_strLocalIP = szLocalAddr;
					break;
				}

			}
		}
	}

	FREE(pTcpTable);

	CString strLog = "Get local ip:";
	strLog += m_strLocalIP;
	PrintLog(strLog.GetBuffer(0));
}

void CDebugSocket::PrintLog(LPCSTR lpData)
{
	if (m_lpfnLogPrint)
	{
		m_lpfnLogPrint(lpData);
	}
}

void CDebugSocket::SetCallbackEvent(int nEventCode)
{
	if (m_lpfnEvent)
	{
		m_lpfnEvent(nEventCode);
	}
}

CString CDebugSocket::GetErrorCode(DWORD dwErrorCode)
{
	char szBuf[MAX_PATH] = {0x00};
	_sntprintf_s(szBuf, MAX_PATH, _TRUNCATE, "%u", dwErrorCode);
	CString strError = szBuf;
	return strError;
}
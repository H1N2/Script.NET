////////////////////////////////////////////////////////////////////////////
//	File:		vcimessage.h
//	Version:	1.0.0.0
//	Created:	2005-08-23
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of VCI Message.
////////////////////////////////////////////////////////////////////////////
#ifndef __VCIMESSAGE_H_
#define __VCIMESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �������ݼ�������
enum {
	VME_NONE,		// ������
	VME_BASE64,		// Base64����
	VME_NOT,		// ȡ������
	VME_XOR,		// ������
	VME_DES,		// DES����
};

// ������Ϣ���ȼ�
#define VMP_HIGH	1		// �����ȼ�
#define VMP_NORMAL	3		// ��ͨ���ȼ�
#define VMP_LOW		5		// �����ȼ�

// ����������
#define VMH_NULL	0x0000	// �վ��

// ������Ϣ����
enum {
	VMT_NORMAL,		// ��ͨ��Ϣ
	VMT_BROADCAST,	// �㲥��Ϣ
	VMT_MULTICAST,	// �鲥��Ϣ
	VMT_FILTER,		// ������Ϣ
};

// ��������/Ӧ������
enum {
	VMT_REQUEST,	// ����
	VMT_RESPONSE,	// Ӧ��
};

// ����ͬ��/�첽��Ϣ
enum {
	VMT_SYNC,		// ͬ����Ϣ
	VMT_ASYNC,		// �첽��Ϣ
};

// ������Ϣ���
enum {
	VMR_OK,			// ִ�гɹ�
	VMR_FAIL,		// ִ��ʧ��
};

// ������������
enum {
	VMD_NULL,		// ����������
	VMD_DATA,		// ������������
	VMD_STRING,		// �ַ�������
	VMD_XML,		// XML����
};

// ������Ϣ���ݵ���󳤶�
#define VMD_MAX_SIZE			0x100000

// ����������
// ������������
#define VMC_NULL				0x0000	// ������
#define VMC_CHECK_LINK			0x0001	// �������
#define VMC_RELINK				0x0002	// ����
#define VMC_CONNECT				0x0003	// ����
#define VMC_CONNECT_CONFIRM		0x0004	// ����֤ʵ
#define VMC_CLOSE				0x0005	// �ر�
#define VMC_CLOSE_CONFIRM		0x0006	// �ر�֤ʵ

// ������
#define VMC_CHECKPWD			0x0010	// ������֤����
#define VMC_CHECKPWD_ACK		0x0011	// ������֤Ӧ��
#define VMC_CHECKPWD_CONFIRM	0x0012	// ������֤֤ʵ
#define VMC_RIGHT_SETRIGHT		0x0015	// ����Զ���û�Ȩ��
#define VMC_RIGHT_GETRIGHT		0x0016	// ��ȡԶ���û�Ȩ��

// ��������
#define VMC_RUNCOMMAND			0x0020	// ִ������
#define VMC_RUNSCRIPT			0x0021	// ִ�нű�
#define VMC_SENDOWMCMD			0x0022	// ����OWM����
#define VMC_SHOWMESSAGE			0x0023	// ��ʾ��Ϣ
#define VMC_SENDDATA			0x0024	// ��������
#define VMC_GETCONFIG			0x0025	// ��ȡ������Ϣ
#define VMC_CONFIG				0x0026	// ����������Ϣ

// ��������
#define VMC_GETVERSION			0x0030	// ��ȡƽ̨�汾
#define VMC_SETNEWVER			0x0031	// �����°汾��
#define VMC_DOWMLOAD_PLAT		0x0032	// ����ƽ̨��װ��
#define VMC_DOWNLOAD_PLUGIN		0x0033	// ���ز��

// �ű�����
#define VMC_INTERP_OPEN			0x0040	// �򿪽ű�������
#define VMC_INTERP_CLOSE		0x0041	// �رսű�������
#define VMC_INTERP_EVALFILE		0x0042	// ִ�нű��ļ�
#define VMC_INTERP_EVALCOMMAND	0x0043	// ִ�нű�����
#define VMC_INTERP_GETERRORLINE	0x0044	// ��ȡ�����к�
#define VMC_INTERP_GETINTERPID	0x0045	// ��ȡ������ID

// ��Ϣ��������
#define VMC_MSGQUEUE_EXIT		0x0050	// ������Ϣ����
#define VMC_MSGQUEUE_ABNORMAL	0x0051	// ��Ϣ���з����쳣

class CVciMessage
{
// attribuite
public:
	int		nEncrypt;	// ���ݼ�������
	int		nPriority;	// ���ȼ�
	CString	szMsgId;	// ��ϢID
	CString	szFrom;		// ��Դ(���ID)
	CString	szDest;		// Ŀ�ĵ�(���ID)
	CString	szUserFrom;	// ��Դ�û�
	CString	szUserTo;	// Ŀ���û�
	int		nCmdCode;	// ������
	int		nHandle;	// Ψһ���
	int		nMsgType;	// ��Ϣ����(��ͨ/�㲥)
	int		nRequest;	// ����/Ӧ����Ϣ
	int		nSync;		// ͬ��/�첽��Ϣ
	int		nTimeOut;	// ��ʱʱ��(����)
	int		nResult;	// ִ�н��
	int		nDataType;	// ��������
	int		nDataLen;	// ���ݳ���
	BYTE*	pData;		// ����ָ��
	CString	szContent;	// ��Ϣ��(XML��)

// Operation
public:
	CVciMessage()
	{
		nEncrypt	= VME_NONE;
		nPriority	= VMP_NORMAL;
		szMsgId		= "";
		szFrom		= "";
		szDest		= "";
		szUserFrom	= "";
		szUserTo	= "";
		nCmdCode	= VMC_NULL;
		nHandle		= VMH_NULL;
		nMsgType	= VMT_NORMAL;
		nRequest	= VMT_REQUEST;
		nSync		= VMT_SYNC;
		nTimeOut	= 100;	// Ĭ��100����
		nResult		= VMR_OK;
		nDataType	= VMD_NULL;
		nDataLen	= 0;
		pData		= NULL;
		szContent	= "";
	}

	CVciMessage(int _nCmdCode, int _nHandle = VMH_NULL)
	{
		nEncrypt	= VME_NONE;
		nPriority	= VMP_NORMAL;
		szMsgId		= "";
		szFrom		= "";
		szDest		= "";
		szUserFrom	= "";
		szUserTo	= "";
		nCmdCode	= _nCmdCode;
		nHandle		= _nHandle;
		nMsgType	= VMT_NORMAL;
		nRequest	= VMT_REQUEST;
		nSync		= VMT_SYNC;
		nTimeOut	= 100;	// Ĭ��100����
		nResult		= VMR_OK;
		nDataType	= VMD_NULL;
		nDataLen	= 0;
		pData		= NULL;
		szContent	= "";
	}

	CVciMessage(LPCTSTR lpszMsgId, LPCTSTR lpszFrom, LPCTSTR lpszDest)
	{
		nEncrypt	= VME_NONE;
		nPriority	= VMP_NORMAL;
		szMsgId		= lpszMsgId;
		szFrom		= lpszFrom;
		szDest		= lpszDest;
		szUserFrom	= "";
		szUserTo	= "";
		nCmdCode	= VMC_NULL;
		nHandle		= VMH_NULL;
		nMsgType	= VMT_NORMAL;
		nRequest	= VMT_REQUEST;
		nSync		= VMT_SYNC;
		nTimeOut	= 100;	// Ĭ��100����
		nResult		= VMR_OK;
		nDataType	= VMD_NULL;
		nDataLen	= 0;
		pData		= NULL;
		szContent	= "";
	}

	virtual ~CVciMessage()
	{
		if(pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}

	CVciMessage(const CVciMessage& other)
	{
		nEncrypt	= other.nEncrypt;
		nPriority	= other.nPriority;
		szMsgId		= other.szMsgId;
		szFrom		= other.szFrom;
		szDest		= other.szDest;
		szUserFrom	= other.szUserFrom;
		szUserTo	= other.szUserTo;
		nCmdCode	= other.nCmdCode;
		nHandle		= other.nHandle;
		nMsgType	= other.nMsgType;
		nRequest	= other.nRequest;
		nSync		= other.nSync;
		nTimeOut	= other.nTimeOut;
		nResult		= other.nResult;
		nDataType	= other.nDataType;
		nDataLen	= other.nDataLen;
		pData = NULL;
		if(nDataLen > 0)
		{
			pData = new BYTE[nDataLen];
			for(int i=0; i<nDataLen; i++)
				pData[i] = other.pData[i];
		}
		szContent	= other.szContent;
	}

	CVciMessage& operator = (const CVciMessage& other)
	{
		nEncrypt	= other.nEncrypt;
		nPriority	= other.nPriority;
		szMsgId		= other.szMsgId;
		szFrom		= other.szFrom;
		szDest		= other.szDest;
		szUserFrom	= other.szUserFrom;
		szUserTo	= other.szUserTo;
		nCmdCode	= other.nCmdCode;
		nHandle		= other.nHandle;
		nMsgType	= other.nMsgType;
		nRequest	= other.nRequest;
		nSync		= other.nSync;
		nTimeOut	= other.nTimeOut;
		nResult		= other.nResult;
		nDataType	= other.nDataType;
		nDataLen	= other.nDataLen;
		if(pData != NULL)
			delete pData;
		pData = NULL;
		if(nDataLen > 0)
		{
			pData = new BYTE[nDataLen];
			for(int i=0; i<nDataLen; i++)
				pData[i] = other.pData[i];
		}
		szContent	= other.szContent;
		return *this;
	}

	// ����������Ϣ����Ӧ����Ϣ�Ļ�������
	void MakeResponceMessage(CVciMessage* pInMsg)
	{
		if(pInMsg)
		{
			nPriority	= pInMsg->nPriority;
			szMsgId		= pInMsg->szMsgId;
			szFrom		= pInMsg->szDest;
			szDest		= pInMsg->szFrom;
			szUserFrom	= pInMsg->szUserTo;
			szUserTo	= pInMsg->szUserFrom;
			nCmdCode	= pInMsg->nCmdCode;
			nHandle		= pInMsg->nHandle;
			nRequest	= VMT_RESPONSE;
		}
	}

	int GetEncrypt() { return nEncrypt; }
	void SetEncrypt(int _nEncrypt) { nEncrypt = _nEncrypt; }

	int GetPriority() { return nPriority; }
	void SetPriority(int _nPriority) { nPriority = _nPriority; }

	CString GetMsgId() { return szMsgId; }
	void SetMsgId(LPCTSTR lpszMsgId) { szMsgId = lpszMsgId; }

	CString GetFrom() { return szFrom; }
	void SetFrom(LPCTSTR lpszFrom) { szFrom = lpszFrom; }

	CString GetDest() { return szDest; }
	void SetDest(LPCTSTR lpszDest) { szDest = lpszDest; }

	CString GetFirstNodeId()	// ��ȡ��һ��·�ɽڵ�ID
	{
		CString strNodeId = szDest;
		int nPos = strNodeId.Find("|");
		if(nPos != -1)
		{
			strNodeId = strNodeId.Left(nPos);
		}
		nPos = strNodeId.Find(":");
		if(nPos != -1)
		{
			strNodeId = strNodeId.Left(nPos);
		}
		return strNodeId;
	}
	CString GetFirstNode()	// ��ȡ��һ��·�ɽڵ�
	{
		CString strNode = szDest;
		int nPos = strNode.Find("|");
		if(nPos != -1)
		{
			strNode = strNode.Left(nPos);
		}
		return strNode;
	}
	CString GetLastNode()	// ��ȡ���һ��·�ɽڵ�(Ŀ�Ľڵ�)
	{
		CStringArray asNode;
		if(GetNodeList(asNode) > 0)
		{
			return asNode[asNode.GetSize()-1];
		}else
		{
			return "";
		}
	}
	int GetNodeList(CStringArray& asNode)	// ��ȡ·�ɽڵ��б�
	{
		CString strNodes = szDest;
		int nPos = -1;
		while((nPos = strNodes.Find("|")) != -1)
		{
			if(strNodes.Left(nPos) != "")
			{
				asNode.Add(strNodes.Left(nPos));
			}
			strNodes.Delete(0, nPos+1);
		}
		if(strNodes != "")
		{
			asNode.Add(strNodes);
		}
		return asNode.GetSize();
	}

	CString GetUserFrom() { return szUserFrom; }
	void SetUserFrom(LPCTSTR lpszUserFrom) { szUserFrom = lpszUserFrom; }

	CString GetUserTo() { return szUserTo; }
	void SetUserTo(LPCTSTR lpszUserTo) { szUserTo = lpszUserTo; }

	int GetCmdCode() { return nCmdCode; }
	void SetCmdCode(int _nCmdCode) { nCmdCode = _nCmdCode; }

	int GetHandle() { return nHandle; }
	void SetHandle(int _nHandle) { nHandle = _nHandle; }

	int GetMsgType() { return nMsgType; }
	void SetMsgType(int _nMsgType) { nMsgType = _nMsgType; }

	int GetRequest() { return nRequest; }
	void SetRequest(int _nRequest) { nRequest = _nRequest; }

	int GetSync() { return nSync; }
	void SetSync(int _nSync) { nSync = _nSync; }

	int GetTimeOut() { return nTimeOut; }
	void SetTimeOut(int _nTimeOut) { nTimeOut = _nTimeOut; }

	int GetResult() { return nResult; }
	void SetResult(int _nResult) { nResult = _nResult; }

	int GetDataType() { return nDataType; }
	void SetDataType(int _nDataType) { nDataType = _nDataType; }

	int GetDataLen() { return nDataLen; }

	void SetData(BYTE* _pData, int nLen)
	{
		if(pData != NULL)
		{
			delete pData;
		}
		pData		= NULL;
		nDataLen	= nLen;
		if(nDataLen > 0)
		{
			pData = new BYTE[nDataLen];
			memcpy(pData, _pData, nDataLen);
		}
	}
	BYTE* GetData() { return pData; }

	CString GetContent() { return szContent; }
	void SetContent(CString _szContent) { szContent = _szContent; }

	void SetString(CString str)
	{
		nDataType	= VMD_STRING;
		szContent	= str;
	}
	CString GetString()
	{
		if(nDataType == VMD_STRING)
		{
			return szContent;
		}
		return "";
	}

	void SetXML(CString str)
	{
		nDataType	= VMD_XML;
		szContent	= str;
	}
	CString GetXML()
	{
		if(nDataType == VMD_XML)
		{
			return szContent;
		}
		return "";
	}

	void SetOwmCmd(int nOwmCmdCode, int wParam, int lParam)
	{
		if(pData != NULL)
		{
			delete pData;
		}
		pData		= NULL;
		nDataLen	= 3 * sizeof(int);
		pData = new BYTE[nDataLen];
		int* ppData = (int*)pData;
		ppData[0] = nOwmCmdCode;
		ppData[1] = wParam;
		ppData[2] = lParam;
	}
	BOOL GetOwmCmd(int& nOwmCmdCode, int& wParam, int& lParam)
	{
		if((nCmdCode == VMC_SENDOWMCMD) && (pData != NULL))
		{
			int* ppData = (int*)pData;
			nOwmCmdCode = ppData[0];
			wParam = ppData[1];
			lParam = ppData[2];
			return TRUE;
		}
		return FALSE;
	}

	// Serialize from buffer
	virtual BOOL SerializeIn(BYTE* pBuffer, int nBufLen)
	{
		int nLen = 0;
		CString strSign;
		if(!SerialReadString(pBuffer, nBufLen, nLen, strSign)) return FALSE;
		if(strSign != "vcimessage") return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nEncrypt)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nPriority)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szMsgId)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szFrom)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szDest)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szUserFrom)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szUserTo)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nCmdCode)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nHandle)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nMsgType)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nRequest)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nSync)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nTimeOut)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nResult)) return FALSE;
		if(!SerialReadInt(pBuffer, nBufLen, nLen, nDataType)) return FALSE;
		if(!SerialReadData(pBuffer, nBufLen, nLen)) return FALSE;
		if(!SerialReadString(pBuffer, nBufLen, nLen, szContent)) return FALSE;

		return TRUE;
	}

	// Serialize to buffer
	virtual BYTE* SerializeOut(int& nBufLen)
	{
		int nMaxLen = 10240;
		BYTE* pBuffer = new BYTE[nMaxLen];
		BYTE* pBufStart = pBuffer;

		nBufLen = 0;

		CString strSign = "vcimessage";
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, strSign)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nEncrypt)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nPriority)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szMsgId)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szFrom)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szDest)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szUserFrom)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szUserTo)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nCmdCode)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nHandle)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nMsgType)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nRequest)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nSync)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nTimeOut)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nResult)) { delete pBuffer; return NULL; }
		if(!SerialWriteInt(pBuffer, nMaxLen, nBufLen, nDataType)) { delete pBuffer; return NULL; }
		if(!SerialWriteData(pBuffer, nMaxLen, nBufLen)) { delete pBuffer; return NULL; }
		if(!SerialWriteString(pBuffer, nMaxLen, nBufLen, szContent)) { delete pBuffer; return NULL; }

		return pBufStart;
	}

	BOOL SerialReadInt(BYTE*& pBuffer, int& nBufLen, int& nLen, int& nValue)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		nValue = *((int*)pBuffer);
		pBuffer += sizeof(int);
		nLen += sizeof(int);

		return TRUE;
	}

	BOOL SerialWriteInt(BYTE*& pBuffer, int& nBufLen, int& nLen, int& nValue)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		*((int*)pBuffer) = nValue;
		pBuffer += sizeof(int);
		nLen += sizeof(int);

		return TRUE;
	}

	BOOL SerialReadString(BYTE*& pBuffer, int& nBufLen, int& nLen, CString& strValue)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		int nStrLen = *((int*)pBuffer);
		pBuffer += sizeof(int);
		nLen += sizeof(int);
		
		if((nLen + nStrLen) > nBufLen)
		{
			return FALSE;
		}

		if(nStrLen > 0)
		{
			char* szBuf = new char[nStrLen+1];
			memset(szBuf, 0, nStrLen+1);
			memcpy(szBuf, pBuffer, nStrLen);
			strValue = szBuf;
			delete szBuf;
			pBuffer += nStrLen;
			nLen += nStrLen;
		}

		return TRUE;
	}

	BOOL SerialWriteString(BYTE*& pBuffer, int& nBufLen, int& nLen, CString& strValue)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		int nStrLen = strValue.GetLength();
		*((int*)pBuffer) = nStrLen;
		pBuffer += sizeof(int);
		nLen += sizeof(int);
		
		if((nLen + nStrLen) > nBufLen)
		{
			return FALSE;
		}

		if(nStrLen > 0)
		{
			memset(pBuffer, 0, nStrLen);
			memcpy(pBuffer, strValue.GetBuffer(0), nStrLen);
			strValue.ReleaseBuffer();
			pBuffer += nStrLen;
			nLen += nStrLen;
		}

		return TRUE;
	}

	BOOL SerialReadData(BYTE*& pBuffer, int& nBufLen, int& nLen)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		nDataLen = *((int*)pBuffer);
		pBuffer += sizeof(int);
		nLen += sizeof(int);

		if((nLen + nDataLen) > nBufLen)
		{
			return FALSE;
		}

		if(pData != NULL)
		{
			delete pData;
			pData = NULL;
		}

		if(nDataLen > 0)
		{
			pData = new BYTE[nDataLen];
			memcpy(pData, pBuffer, nDataLen);
			pBuffer += nDataLen;
			nLen += nDataLen;
		}

		return TRUE;
	}

	BOOL SerialWriteData(BYTE*& pBuffer, int& nBufLen, int& nLen)
	{
		if((nLen + (int)sizeof(int)) > nBufLen)
		{
			return FALSE;
		}

		*((int*)pBuffer) = nDataLen;
		pBuffer += sizeof(int);
		nLen += sizeof(int);

		if((nLen + nDataLen) > nBufLen)
		{
			return FALSE;
		}

		if((nDataLen > 0) && (pData != NULL))
		{
			memcpy(pBuffer, pData, nDataLen);
			pBuffer += nDataLen;
			nLen += nDataLen;
		}

		return TRUE;
	}
/*
	virtual void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar << nEncrypt;
			ar << szMsgId;
			ar << szFrom;
			ar << szDest;
			ar << szUserFrom;
			ar << szUserTo;
			ar << nCmdCode;
			ar << nHandle;
			ar << nMsgType;
			ar << nRequest;
			ar << nSync;
			ar << nResult;
			ar << nDataType;
			ar << nDataLen;
			for(int i=0; i<nDataLen; i++)
				ar << (BYTE)pData[i];
			ar << szContent;
		}
		else
		{
			ar >> nEncrypt;
			ar >> szMsgId;
			ar >> szFrom;
			ar >> szDest;
			ar >> szUserFrom;
			ar >> szUserTo;
			ar >> nCmdCode;
			ar >> nHandle;
			ar >> nMsgType;
			ar >> nRequest;
			ar >> nSync;
			ar >> nResult;
			ar >> nDataType;
			ar >> nDataLen;
			if(pData != NULL)
				delete pData;
			pData = NULL;
			if(nDataLen > 0)
			{
				pData = new BYTE[nDataLen];
				for(int i=0; i<nDataLen; i++)
					ar >> pData[i];
			}
		}
	}

#ifdef _DEBUG
	virtual void AssertValid() const
	{
		CObject::AssertValid();
	}
	virtual void Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);
	}
#endif*/

#ifdef _DEBUG
	virtual CString DumpInfo()
	{
		CString strDump;
		strDump.Format("CVciMessage:\r\n MsgId=%s, From=%s, Dest=%s, UserFrom=%s, UserTo=%s\r\n CmdCode=%d, Handle=%d, MsgType=%d, Request=%d, Sync=%d\r\n Result=%d, DataType=%d\r\n Data=%s\r\n",
				szMsgId, szFrom, szDest, szUserFrom, szUserTo, nCmdCode, nHandle, nMsgType, nRequest, nSync, nResult, nDataType, szContent);
		return strDump;
	}
#endif
};

// ��ȡ������Ϣ��ָ��
#define GET_RESPONSE_MSG(msg_rep)	\
	CVciMessage msg_rep;	\
	CVciMessage* p##msg_rep;	\
	if(ppOut != NULL)	\
	{	\
		p##msg_rep = ppOut;	\
	}else	\
	{	\
		p##msg_rep = &msg_rep;	\
		msg_rep.SetSync(VMT_ASYNC);	\
	}	\
	p##msg_rep->MakeResponceMessage(pIn);	\


// ���ͷ�����Ϣ
#define SEND_RESPONSE_MSG(msg_rep)	\
	if(ppOut == NULL)	\
	{	\
		SendMessage(&msg_rep, NULL);	\
	}	\


// ��Ϣ�����ඨ��
class CMsgFilter : public CVciMessage
{
// attribute
public:
	CString	szName;			// ����������
	CString	szFilterDest;	// ������Ŀ�����

// Operation
public:
	CMsgFilter()
	{
		CVciMessage();
		szName = _T("");
		szFilterDest = _T("");
	}

	CMsgFilter(const CMsgFilter& other)
	{
		szName		= other.szName;
		szDest		= other.szDest;
		szFilterDest= other.szFilterDest;
	}

	CMsgFilter& operator = (const CMsgFilter& other)
	{
		szName		= other.szName;
		szDest		= other.szDest;
		szFilterDest= other.szFilterDest;
		return *this;
	}

	CString GetName() { return szName; }
	void SetName(LPCTSTR lpszName) { szName = lpszName; }

	CString GetFilterDest() { return szFilterDest; }
	void SetFilterDest(LPCTSTR lpszFilterDest) { szFilterDest = lpszFilterDest; }
};

typedef CArray<CMsgFilter, CMsgFilter&> CMsgFilterArray;

#endif // !defined(__VCIMESSAGE_H_)
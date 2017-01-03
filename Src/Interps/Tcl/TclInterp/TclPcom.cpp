////////////////////////////////////////////////////////////////////////////
//	File:		TclPcom.cpp
//	Version:	1.0.0.0
//	Created:	2002-04-03
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	implementation of use PCOM in TCL script.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TclPcom.h"
#include "CTclInterp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define VCI_NAME_PLATUI _T("org.blueui.platui")

// ��֤TCL��չ����Ȩ�޺�
#define VERIFY_TCLCMD_RIGHT(right)	\
/*	if(right < theApp.m_CurUser.nUserRight)	\
	{	\
		interp->result = "This tcl command request higher user rights.";	\
		return TCL_ERROR;	\
	}else	\
	if(interp == theApp.m_pRCTclInterp && right < theApp.m_nRCUserRights)	\
	{	\
		interp->result = "This tcl command request higher rights for remote user.";	\
		return TCL_ERROR;	\
	}	\
*/

static IPlatUI* g_pIPlatUI = NULL;		// ƽ̨�ӿ�ָ��
static CTclPcomObj* g_pTclPcomObjPlatUI = NULL;	// ƽ̨�ӿ�TclVci����
static CPtrList g_TclPcomCallBackList;	// PCOM�ص������б�

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTclPcomParam::CTclPcomParam()
{
	m_strParamName	= "";
	m_nType			= PTYPE_INT;
	m_nMode			= PMODE_DIRECT;
	m_strDefault	= "";
	//
	m_strRefVar		= "";
	m_nCommVar		= 0;
	m_chChar		= 0;
	m_byByte		= 0;
	m_sShort		= 0;
	m_wWord			= 0;
	m_nInt			= 0;
	m_uiUint		= 0;
	m_tTime			= CTime(0);
	m_ptTime		= NULL;
	m_lpVoid		= 0;
	m_fFloat		= 0;
	m_lpTSTR		= NULL;
	m_strString		= "";
	m_psString		= NULL;
	m_pasString		= NULL;
	m_pauUInt		= NULL;
}

CTclPcomParam::~CTclPcomParam()
{
}

CTclPcomParam::CTclPcomParam(const CTclPcomParam& other)
{
	m_strParamName	= other.m_strParamName;
	m_nType			= other.m_nType;
	m_nMode			= other.m_nMode;
	m_strDefault	= other.m_strDefault;
	//
	m_strRefVar		= other.m_strRefVar;
	m_nCommVar		= other.m_nCommVar;
	m_chChar		= other.m_chChar;
	m_byByte		= other.m_byByte;
	m_sShort		= other.m_sShort;
	m_wWord			= other.m_wWord;
	m_nInt			= other.m_nInt;
	m_uiUint		= other.m_uiUint;
	m_tTime			= other.m_tTime;
	m_ptTime = NULL;
	if(other.m_ptTime != NULL)
	{
		m_ptTime		= new CTime(*(other.m_ptTime));
	}
	m_lpVoid		= other.m_lpVoid;
	m_fFloat		= other.m_fFloat;
	m_lpTSTR		= NULL;
	if(other.m_lpTSTR != NULL)
	{
		m_lpTSTR		= new char[strlen(other.m_lpTSTR)];
		strcpy(m_lpTSTR, other.m_lpTSTR);
	}
	m_strString		= other.m_strString;
	m_psString = NULL;
	if(other.m_psString != NULL)
	{
		m_psString		= new CString(*(other.m_psString));
	}
	m_asString.RemoveAll();
	int i;
	for(i=0; i<other.m_asString.GetSize(); i++)
	{
		m_asString.Add(other.m_asString[i]);
	}
	m_pasString = NULL;
	if(other.m_pasString != NULL)
	{
		m_pasString = new CStringArray();
		for(i=0; i<other.m_pasString->GetSize(); i++)
		{
			m_pasString->Add(other.m_pasString->GetAt(i));
		}
	}
	m_auUInt.RemoveAll();
	for(i=0; i<other.m_auUInt.GetSize(); i++)
	{
		m_auUInt.Add(other.m_auUInt[i]);
	}
	m_pauUInt = NULL;
	if(other.m_pauUInt != NULL)
	{
		m_pauUInt = new CUIntArray();
		for(i=0; i<other.m_pauUInt->GetSize(); i++)
		{
			m_pauUInt->Add(other.m_pauUInt->GetAt(i));
		}
	}
}

//////////////////////////////////////////////////////////////////////
// ���ظ�ֵ������
//////////////////////////////////////////////////////////////////////
CTclPcomParam& CTclPcomParam::operator = (const CTclPcomParam& other)
{
	m_strParamName	= other.m_strParamName;
	m_nType			= other.m_nType;
	m_nMode			= other.m_nMode;
	m_strDefault	= other.m_strDefault;
	//
	m_strRefVar		= other.m_strRefVar;
	m_nCommVar		= other.m_nCommVar;
	m_chChar		= other.m_chChar;
	m_byByte		= other.m_byByte;
	m_sShort		= other.m_sShort;
	m_wWord			= other.m_wWord;
	m_nInt			= other.m_nInt;
	m_uiUint		= other.m_uiUint;
	m_tTime			= other.m_tTime;
	m_ptTime = NULL;
	if(other.m_ptTime != NULL)
	{
		m_ptTime		= new CTime(*(other.m_ptTime));
	}
	m_lpVoid		= other.m_lpVoid;
	m_fFloat		= other.m_fFloat;
	if(other.m_lpTSTR != NULL)
	{
		m_lpTSTR		= new char[strlen(other.m_lpTSTR)];
		strcpy(m_lpTSTR, other.m_lpTSTR);
	}
	m_strString		= other.m_strString;
	if(other.m_psString != NULL)
	{
		m_psString		= new CString(*(other.m_psString));
	}
	m_asString.RemoveAll();
	int i;
	for(i=0; i<other.m_asString.GetSize(); i++)
	{
		m_asString.Add(other.m_asString[i]);
	}
	if(other.m_pasString != NULL)
	{
		m_pasString = new CStringArray();
		for(i=0; i<other.m_pasString->GetSize(); i++)
		{
			m_pasString->Add(other.m_pasString->GetAt(i));
		}
	}
	m_auUInt.RemoveAll();
	for(i=0; i<other.m_auUInt.GetSize(); i++)
	{
		m_auUInt.Add(other.m_auUInt[i]);
	}
	m_pauUInt = NULL;
	if(other.m_pauUInt != NULL)
	{
		m_pauUInt = new CUIntArray();
		for(i=0; i<other.m_pauUInt->GetSize(); i++)
		{
			m_pauUInt->Add(other.m_pauUInt->GetAt(i));
		}
	}

	return *this;
};

//////////////////////////////////////////////////////////////////////
// ��ȡ����ռ�ÿռ�
//////////////////////////////////////////////////////////////////////
int CTclPcomParam::GetParamSize()
{
	switch(m_nType)
	{
	case PTYPE_VOID: return 0;
	case PTYPE_CHAR: return sizeof(char);
	case PTYPE_BYTE: return sizeof(BYTE);
	case PTYPE_SHORT: return sizeof(short);
	case PTYPE_WORD: return sizeof(WORD);
	case PTYPE_INT: return sizeof(int);
	case PTYPE_UINT: return sizeof(UINT);
	case PTYPE_CTIME: return sizeof(LPVOID);
	case PTYPE_POINTER: return sizeof(LPVOID);
	case PTYEP_FLOAT: return sizeof(float);
	case PTYPE_STRING: return sizeof(LPCTSTR);
	case PTYPE_CSTRING: return sizeof(LPVOID);
	case PTYPE_CSTRINGARRAY: return sizeof(LPVOID);
	case PTYPE_UINTARRAY: return sizeof(LPVOID);
	default: return 0;
	}
};

//////////////////////////////////////////////////////////////////////
// �ж��Ƿ���Ҫ���ڻش�������TCL������
//////////////////////////////////////////////////////////////////////
BOOL CTclPcomParam::NeedRefVar()
{
	// �������ͱ���Ҫ,ָ�����Ϳ�����Ҫ
	return (PMODE_DIRECT != m_nMode);
}

//////////////////////////////////////////////////////////////////////
// ��������ı�����Ϣ��ʼ����Ӧ����
//////////////////////////////////////////////////////////////////////
void CTclPcomParam::VarInit(CString strVar, CString strValue)
{
	// �����ȱʡ����,����δ��ֵ,����ȱʡ������ֵ
	if((strValue == "") && (m_strDefault != ""))
	{
		strValue = m_strDefault;
	}

	// ��������û�ָ�뷽ʽ,��Ҫ����TCL������,���ڻش�����
	if(NeedRefVar())
	{
		m_strRefVar = strVar;
	}else
	{
		m_strRefVar = "";
	}

	// ����ֵ
	switch(m_nType)
	{
	case PTYPE_CHAR:
		if(strValue.GetLength() > 0)
			m_chChar = (char)(strValue[0]);
		else
			m_chChar = '\0';
		m_nCommVar = m_chChar;
		break;
	case PTYPE_BYTE:
		m_byByte = (BYTE)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_byByte);	
		else
			m_nCommVar = m_byByte;
		break;
	case PTYPE_SHORT:
		m_sShort = (short)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_sShort);	
		else
			m_nCommVar = m_sShort;
		break;
	case PTYPE_WORD:
		m_wWord = (WORD)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_wWord);	
		else
			m_nCommVar = m_wWord;
		break;
	case PTYPE_INT:
		m_nInt = (int)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_nInt);			
		else
			m_nCommVar = m_nInt;
		break;
	case PTYPE_UINT:
		m_uiUint = (UINT)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_uiUint);	
		else
			m_nCommVar = m_uiUint;
		break;
	case PTYPE_CTIME:
		{
			if(NeedRefVar())
			{
				m_tTime = CTime((time_t)(atoi(strValue)));
				m_nCommVar = (int)(void*)(&m_tTime);
			}else
			{
				m_ptTime = new CTime((time_t)(atoi(strValue)));
				m_nCommVar = (int)(m_ptTime->GetTime());
			}
		}
		break;
	case PTYPE_POINTER:
		m_lpVoid = (LPVOID)(atoi(strValue));
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_lpVoid);	
		else
			m_nCommVar = (int)m_lpVoid;
		break;
	case PTYEP_FLOAT:
		m_fFloat = atof(strValue);
		if(NeedRefVar())
			m_nCommVar = (int)(void*)(&m_fFloat);	
		else
			m_nCommVar = m_fFloat;
		break;
	case PTYPE_STRING:
		m_lpTSTR = new char[strValue.GetLength()+1];
		strcpy(m_lpTSTR, strValue);
		m_nCommVar = (int)(void*)m_lpTSTR;
		break;
	case PTYPE_CSTRING:
		{
			if(NeedRefVar())
			{
				m_strString = strValue;
				m_nCommVar = (int)(void*)(&m_strString);
			}else
			{
				m_psString = new CString(strValue);
				//m_nCommVar = (int)(void*)m_psString;
				m_nCommVar = (int)(LPCTSTR)(*m_psString);
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// ��������ı�����Ϣ��ʼ����Ӧ����
//////////////////////////////////////////////////////////////////////
void CTclPcomParam::VarInitArray(CString strVar, CStringArray* pasValue)
{
	// ��������û�ָ�뷽ʽ,��Ҫ����TCL������,���ڻش�����
	if(NeedRefVar())
	{
		m_strRefVar = strVar;
	}else
	{
		m_strRefVar = "";
	}
	if(pasValue == NULL)
	{
		return;
	}

	// ����ֵ
	if(PTYPE_CSTRINGARRAY == m_nType)
	{
		if(NeedRefVar())
		{
			for(int i=0; i<pasValue->GetSize(); i++)
			{
				m_asString.Add(pasValue->GetAt(i));
			}
			m_nCommVar = (int)(void*)(&m_asString);
		}else
		{
			m_pasString = new CStringArray();
			for(int i=0; i<pasValue->GetSize(); i++)
			{
				m_pasString->Add(pasValue->GetAt(i));
			}
			m_nCommVar = (int)(void*)m_pasString;
		}
	}else
	if(PTYPE_UINTARRAY == m_nType)
	{
		if(NeedRefVar())
		{
			for(int i=0; i<pasValue->GetSize(); i++)
			{
				m_auUInt.Add(atoi(pasValue->GetAt(i)));
			}
			m_nCommVar = (int)(void*)(&m_auUInt);
		}else
		{
			m_pauUInt = new CUIntArray();
			for(int i=0; i<pasValue->GetSize(); i++)
			{
				m_pauUInt->Add(atoi(pasValue->GetAt(i)));
			}
			m_nCommVar = (int)(void*)m_pauUInt;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// �ͷű���(��Ҫ�Ƕ�̬�����һЩ����),
// �������û�ָ�����ͻ᷵�ر仯��ı���(ת��Ϊ�ַ���)
//////////////////////////////////////////////////////////////////////
CString CTclPcomParam::VarRelease()
{
	CString strRet = "";

	// ת��Ϊ�ַ���,��ɾ����̬����Ŀռ�
	switch(m_nType)
	{
	case PTYPE_CTIME:
		{
			if(NeedRefVar())
			{
				strRet.Format("%d", m_tTime.GetTime());
			}else
			{
				delete (void*)m_ptTime;
			}
		}
		break;
	case PTYPE_STRING:
		if(NeedRefVar())
			strRet = m_lpTSTR;
		delete m_lpTSTR;
		break;
	case PTYPE_CSTRING:
		{
			if(NeedRefVar())
			{
				strRet = m_strString;
			}else
			{
				delete (void*)m_psString;
			}
		}
		break;
	case PTYPE_CSTRINGARRAY:
		{
			if(!NeedRefVar())
			{
				delete (void*)m_pasString;
			}
		}
		break;
	case PTYPE_UINTARRAY:
		{
			if(!NeedRefVar())
			{
				delete (void*)m_pauUInt;
			}
		}
		break;
	}

	// �ж��Ƿ���Ҫ�ش�����
	if(!NeedRefVar())
	{
		return strRet;
	}

	// ת��Ϊ�ַ���,��ɾ����̬����Ŀռ�
	switch(m_nType)
	{
	case PTYPE_CHAR:
		strRet = m_chChar;
		break;
	case PTYPE_BYTE:
		strRet.Format("%d", m_byByte);
		break;
	case PTYPE_SHORT:
		strRet.Format("%d", m_sShort);
		break;
	case PTYPE_WORD:
		strRet.Format("%d", m_wWord);
		break;
	case PTYPE_INT:
		strRet.Format("%d", m_nInt);
		break;
	case PTYPE_UINT:
		strRet.Format("%u", m_uiUint);
		break;
	case PTYPE_POINTER:
		strRet.Format("%d", m_lpVoid);
		break;
	case PTYEP_FLOAT:
		if(m_fFloat < 1)
		{
			// С��0.3�ĸ�����ֱ��ת��Ϊ�ַ�����ʧ��,��������Ϊ%.6f�ſ���
			strRet.Format("%.6f", m_fFloat);
		}else
		{
			strRet.Format("%f", m_fFloat);
		}
		break;
	}

	return strRet;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTclPcomFunc::CTclPcomFunc()
{
	m_strFuncName	= "";
	m_strFuncDef	= "";
	m_nLeastParams	= 0;
	m_bEnableExec	= TRUE;
}

CTclPcomFunc::~CTclPcomFunc()
{
}

//////////////////////////////////////////////////////////////////////
// ���ظ�ֵ������
//////////////////////////////////////////////////////////////////////
CTclPcomFunc::CTclPcomFunc(CTclPcomFunc& other)
{
	m_strFuncName	= other.m_strFuncName;
	m_strFuncDef	= other.m_strFuncDef;
	m_nLeastParams	= other.m_nLeastParams;
	m_bEnableExec	= other.m_bEnableExec;
	m_Ret			= other.m_Ret;
	m_arParams.RemoveAll();
	for(int i=0; i<other.m_arParams.GetSize(); i++)
	{
		m_arParams.Add(other.m_arParams[i]);
	}
};

//////////////////////////////////////////////////////////////////////
// ���ظ�ֵ������
//////////////////////////////////////////////////////////////////////
CTclPcomFunc& CTclPcomFunc::operator = (CTclPcomFunc& other)
{
	m_strFuncName	= other.m_strFuncName;
	m_strFuncDef	= other.m_strFuncDef;
	m_nLeastParams	= other.m_nLeastParams;
	m_bEnableExec	= other.m_bEnableExec;
	m_Ret			= other.m_Ret;
	m_arParams.RemoveAll();
	for(int i=0; i<other.m_arParams.GetSize(); i++)
	{
		m_arParams.Add(other.m_arParams[i]);
	}

	return *this;
};

//////////////////////////////////////////////////////////////////////
// ����������,�������ͺʹ��ݷ�ʽ
// ����ֵ: ��������
//////////////////////////////////////////////////////////////////////
int	CTclPcomFunc::ParseParamType(CString strParam, CString& strParamName, int& nType, int& nMode, CString& strDefault)
{
	strDefault = "";
	
	strParamName = "";
	// ȥ�����ָ���
	strParam.Replace("\t", " ");	// �滻Tab��
	strParam.Replace("\r", "");		// �滻\r
	strParam.Replace("\n", "");		// �滻\n
	strParam.TrimLeft();
	strParam.TrimRight();

	// �����Ƿ���ȱʡֵ
	int nPos = strParam.Find("=");
	if(nPos != -1)
	{
		strDefault = strParam.Right(strParam.GetLength()-nPos-1);
		strDefault.TrimLeft();
		strParam = strParam.Left(nPos);
		strParam.TrimRight();
	}

	// ���������ͺͲ������ֿ�
	nPos = strParam.ReverseFind(' ');
	if(nPos != -1)
	{
		strParamName = strParam.Right(strParam.GetLength() - nPos -1);
		strParamName.TrimLeft();
		strParam = strParam.Left(nPos);
		while(strParamName.FindOneOf("*& ") == 0)
		{
			if(('*' == strParamName[0]) || ('&' == strParamName[0]))
			{
				strParam += strParamName[0];
			}
			strParamName.Delete(0, 1);
		}
	}

	// ����const�ؼ���,Ŀǰ��ȥ��const�ؼ���
	if(strParam.Find("const ") == 0)
	{
		strParam.Delete(0, 6);
		strParam.TrimLeft();
	}
	nPos = strParam.Find(" const");
	if((nPos > 0) && (nPos == (strParam.GetLength()-6)))
	{
		strParam = strParam.Left(strParam.GetLength()-6);
		strParam.TrimRight();
	}

	// ������������
	CString strMode = "";
	while(strParam.FindOneOf("*&") == (strParam.GetLength() - 1))
	{
		strMode += strParam[strParam.GetLength() - 1];
		strParam = strParam.Left(strParam.GetLength()-1);
	}

	// ������������
	if(strMode == "")
	{
		nMode = PMODE_DIRECT;
	}else
	if(strMode == "&")
	{
		nMode = PMODE_REFERENCE;
	}else
	if(strMode == "*")
	{
		nMode = PMODE_POINTER;
	}else
	if(strMode == "**")
	{
		nMode = PMODE_POINTER2;
	}

	// ��������
	if(strParam == "void")
	{
		if(nMode == PMODE_POINTER)
		{
			// ���Ϊvoid*���ͬ��LPVOID
			nType = PTYPE_POINTER;
			nMode = PMODE_DIRECT;
		}else
		{
			nType = PTYPE_VOID;
		}
	}else
	if(strParam == "char" || strParam == "CHAR" || strParam == "bool")
	{
		nType = PTYPE_CHAR;
	}else
	if(strParam == "BYTE" || strParam == "unsigned char")
	{
		nType = PTYPE_BYTE;
	}else
	if(strParam == "short")
	{
		nType = PTYPE_SHORT;
	}else
	if(strParam == "WORD" || strParam == "unsigned short")
	{
		nType = PTYPE_WORD;
	}else
	if(strParam == "int" || strParam == "BOOL" || strParam == "long" || strParam == "time_t"
		|| strParam == "CDocument")
	{
		// ע: CDocument�����⴦��,�������ο���
		nType = PTYPE_INT;
	}else
	if(strParam == "UINT" || strParam == "unsigned int" || strParam == "DWORD" || strParam == "ULONG" || strParam == "unsigned long" || strParam == "HANDLE" || strParam == "WPARAM" || strParam == "LPARAM")
	{
		nType = PTYPE_UINT;
	}else
	if(strParam == "CTime")
	{
		nType = PTYPE_CTIME;
	}else
	if(strParam == "LPVOID")
	{
		nType = PTYPE_POINTER;
	}else
	if(strParam == "float")
	{
		nType = PTYEP_FLOAT;
	}else
	if(strParam == "LPCSTR" || strParam == "LPCTSTR" || strParam == "LPSTR" || strParam == "PTSTR")
	{
		nType = PTYPE_STRING;
	}else
	if(strParam == "CString")
	{
		nType = PTYPE_CSTRING;
	}else
	if(strParam == "CStringArray")
	{
		nType = PTYPE_CSTRINGARRAY;
	}else
	if((strParam == "CUIntArray") || (strParam == "CPtrArray"))
	{
		nType = PTYPE_UINTARRAY;
	}else
	{
		nType = PTYPE_UNKNOWN;
	}

	return nType;
}

//////////////////////////////////////////////////////////////////////
// ���㺯���Ĳ����ܳ���
//////////////////////////////////////////////////////////////////////
int	CTclPcomFunc::CalcTotalParamSize()
{
	int nSize = 0;
	for(int i=0; i<m_arParams.GetSize(); i++)
	{
		nSize += m_arParams[i].GetParamSize();
	}
	return nSize;
}

//////////////////////////////////////////////////////////////////////
// ���һ������
// ����ֵ: Ŀǰ�Ĳ�������
//////////////////////////////////////////////////////////////////////
int	CTclPcomFunc::AddParam(CString strParamName, int nType, int nMode, CString strDefault)
{
	CTclPcomParam PcomParam;
	PcomParam.m_strParamName	= strParamName;
	PcomParam.m_nType			= nType;
	PcomParam.m_nMode			= nMode;
	PcomParam.m_strDefault		= strDefault;
	m_arParams.Add(PcomParam);
	return m_arParams.GetSize();
}

//////////////////////////////////////////////////////////////////////
// ���Ҳ���
//////////////////////////////////////////////////////////////////////
CTclPcomParam* CTclPcomFunc::FindParam(CString strParam)
{
	for(int i=0; i<m_arParams.GetSize(); i++)
	{
		if(strParam == m_arParams[i].m_strParamName)
		{
			return &(m_arParams[i]);
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPtrList	g_TclPcomClassList;	// PCOM�����б�

CTclPcomClass::CTclPcomClass()
{
	m_dwRefCount	= 0;
	m_strVciName		= "";
}

CTclPcomClass::~CTclPcomClass()
{
}

//////////////////////////////////////////////////////////////////////
// ���Լ���ͷ�ļ�
//////////////////////////////////////////////////////////////////////
BOOL CTclPcomClass::LoadHeadFile()
{
	if(g_pIPlatUI == NULL)
	{
		return FALSE;
	}

	CString strPlatPath = g_pIPlatUI->GetPlatPath();
	CString strVciType = g_pIPlatUI->GetVciType(m_strVciName);
	CString strVciClass = g_pIPlatUI->GetVciClass(m_strVciName);
	CString strVciInterfaceFile = g_pIPlatUI->GetVciInterfaceFile(m_strVciName);

	// ����ƽ̨��VCI\IncludeĿ¼,���ܷ��ҵ�ͷ�ļ�
	CString strPath;
	if(m_strVciName == VCI_NAME_PLATUI)
	{
		strVciType = "plat";
		strPath = strPlatPath + "Plugins\\Include\\IPlatUI.h";
		strVciClass = "IPlatUI";
	}else
	if(strVciType == "owm")
	{
		strPath = strPlatPath + "Plugins\\Include\\vciowm.h";
	}else
	if(strVciType == "interp")
	{
		strPath = strPlatPath + "Plugins\\Include\\IInterp.h";
	}else
	if(strVciType == "project")
	{
		strPath = strPlatPath + "Plugins\\Include\\IProject.h";
	}else
	{
		strPath = strVciInterfaceFile;
	}
/*
	if(g_strDevicePath == "")
	{
		strPath = GetPlatRootPath();
		strPath += "VCI\\Include\\";
		strPath += m_strVciName;
		strPath += ".h";
	}else
	{
		strPath = g_strDevicePath;
		strPath.Replace("/", "\\");
		if(strPath.Right(1) != '\\')
		{
			strPath += "\\";
			strPath += "Include\\";
			strPath += m_strVciName;
			strPath += ".h";
		}
	}
*/
	if(GetFileAttributes(strPath) == 0xFFFFFFFF)
	{
		// δ�ҵ�ͷ�ļ�,����ʧ��
		TRACE("TclPcomClass: not find device head file %s!\n", strPath);
		return FALSE;
	}

	// ͷ�ļ���������:��������ʽƥ�������麯��
	strPath.Replace("\\", "/");
	char loadHeadFileScript[500];
	sprintf(loadHeadFileScript, "\n\
		set fi [open \"%s\" r];\n\
		set buf [read $fi];\n\
		close $fi;\n\
		regexp {interface\\s+%s\\s+.*?\\{(.*?)\\}\\s*;} $buf total all;\n\
		regsub -all -line {//.*$} $all {} all1;\n\
		regsub -all -line {/\\*.*\\*/} $all1 {} all;\n\
		set all;\n\
		", strPath, strVciClass);

	CTclInterp interp;
	if(!interp.RunScriptCommand(loadHeadFileScript))
	{
		return FALSE;
	}

	// ��ȡ�����麯���Ĵ�(ֻ�ǽ�)
	CString strFuncs = interp.GetResult();
	int nPos = 0;
	int nPosStart = 0;
	int nPosEnd = 0;
	int nPosParam = 0;
	int nPosRet = 0;
	while(TRUE)
	{
		// ����ͷ
		nPos = strFuncs.Find("virtual ", nPosStart);
		if(nPos == -1)
			break;

		// ����β
		nPosEnd = strFuncs.Find(") = 0;", nPos);
		if(nPosEnd == -1)
			break;

		// ������ʼλ��
		nPosParam = strFuncs.Find("(", nPos);
		if(nPosParam == -1)
			break;

		// ����ֵ����
		nPosRet = strFuncs.Find("__stdcall", nPos);
		if(nPosRet == -1)
			break;
		CString strRetType = strFuncs.Mid(nPos+8, nPosRet-nPos-8);
		CString strParamName = "";	// ������
		int nType = -1;				// ��������
		int nMode = PMODE_DIRECT;	// �������ݷ�ʽ
		CString strDefault = "";	// ����ȱʡֵ

		CTclPcomFunc PcomFunc;
		PcomFunc.ParseParamType(strRetType, strParamName, nType, nMode, strDefault);
		PcomFunc.m_Ret.m_nType	= nType;
		PcomFunc.m_Ret.m_nMode	= nMode;

		// ��������ԭ��
		CString strFuncDef = strFuncs.Mid(nPos+8, nPosEnd-nPos-7);
		strFuncDef.Delete(nPosRet-nPos-8, 10);
		strFuncDef.TrimLeft();
		strFuncDef += ";";
		PcomFunc.SetFuncDef(strFuncDef);

		// ����������
		CString strFuncName = strFuncs.Mid(nPosRet+9, nPosParam-nPosRet-9);
		strFuncName.TrimLeft();
		strFuncName.TrimRight();
		PcomFunc.SetFuncName(strFuncName);
		//TRACE(">>>%s\n", strFuncName);

		// ������������
		CString strParams = strFuncs.Mid(nPosParam+1, nPosEnd-nPosParam-1);
		strParams.TrimLeft();
		strParams.TrimRight();
		int nPosP = -1;
		while((nPosP = strParams.Find(",")) != -1)
		{	// ǰ�����ɸ�����
			CString strParam = strParams.Left(nPosP);
			strParams.Delete(0, nPosP+1);
			PcomFunc.ParseParamType(strParam, strParamName, nType, nMode, strDefault);
			PcomFunc.AddParam(strParamName, nType, nMode, strDefault);
			//TRACE("...%s(type=%d,mode=%d,default=%s)\n", strParamName, nType, nMode, strDefault);
			// ��δ֪���Ͳ����򲻿�ִ��
			if(PTYPE_UNKNOWN == nType)
				PcomFunc.m_bEnableExec = FALSE;
		}
		strParams.TrimLeft();
		if(strParams != "")	// ���һ������
		{
			PcomFunc.ParseParamType(strParams, strParamName, nType, nMode, strDefault);
			PcomFunc.AddParam(strParamName, nType, nMode, strDefault);
			//TRACE("...%s(type=%d,mode=%d,default=%s)\n", strParamName, nType, nMode, strDefault);
			// ��δ֪���Ͳ����򲻿�ִ��
			if(PTYPE_UNKNOWN == nType)
				PcomFunc.m_bEnableExec = FALSE;
		}

		// ����̫��Ҳ����ִ��
		if(PcomFunc.m_arParams.GetSize() > TCLPCOM_MAXPARAM)
		{
			PcomFunc.m_bEnableExec = FALSE;
		}

		// �����ȥȱʡ����������ٲ�������
		PcomFunc.m_nLeastParams = PcomFunc.m_arParams.GetSize();
		for(int i=0; i<PcomFunc.m_arParams.GetSize(); i++)
		{
			if(PcomFunc.m_arParams[i].m_strDefault != "")
			{
				PcomFunc.m_nLeastParams = i+1;
				break;
			}
		}

		m_arFuncs.Add(PcomFunc);

		// ��һ������
		nPosStart = nPosEnd;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// ���Һ���
//////////////////////////////////////////////////////////////////////
CTclPcomFunc* CTclPcomClass::FindFunction(CString strFunc)
{
	// ���������ĺ���,ͨ���ں��������"(No)"����ʾ,
	// ����Send(2)��ʾ��2��������Send�ĺ���
	int nFuncNo = 1;	// ���ں�������ͬ�ĺ��������
	int nPos = strFunc.Find("(");
	if(nPos != -1)
	{
		int nPos1 = strFunc.Find(")");
		if(nPos1 > nPos)
		{
			CString strFuncNo = strFunc.Mid(nPos+1, nPos1-nPos-1);
			nFuncNo = atoi(strFuncNo);
			if(nFuncNo <= 0)
				nFuncNo = 1;
			strFunc = strFunc.Left(nPos);
		}
	}

	for(int i=0; i<m_arFuncs.GetSize(); i++)
	{
		if(strFunc == m_arFuncs[i].GetFuncName())
		{
			nFuncNo--;
			if(nFuncNo <= 0)
			{
				return &(m_arFuncs[i]);
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ����������
//////////////////////////////////////////////////////////////////////
int CTclPcomClass::GetFunctionIndex(CString strFunc)
{
	// ���������ĺ���,ͨ���ں��������"(No)"����ʾ,
	// ����Send(2)��ʾ��2��������Send�ĺ���
	int nFuncNo = 1;	// ���ں�������ͬ�ĺ��������
	int nPos = strFunc.Find("(");
	if(nPos != -1)
	{
		int nPos1 = strFunc.Find(")");
		if(nPos1 > nPos)
		{
			CString strFuncNo = strFunc.Mid(nPos+1, nPos1-nPos-1);
			nFuncNo = atoi(strFuncNo);
			if(nFuncNo <= 0)
				nFuncNo = 1;
			strFunc = strFunc.Left(nPos);
		}
	}

	for(int i=0; i<m_arFuncs.GetSize(); i++)
	{
		if(strFunc == m_arFuncs[i].GetFuncName())
		{
			nFuncNo--;
			if(nFuncNo <= 0)
			{
				return i;
			}
		}
	}
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// ��������������������
/////////////////////////////////////////////////////////////////////////////
CTclPcomClass* GetTclPcomClass(CString strClassName)
{
	int count = g_TclPcomClassList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomClassList.FindIndex(i);
		CTclPcomClass* pTclPcomClass = (CTclPcomClass*)g_TclPcomClassList.GetAt(pos);
		if(strClassName == pTclPcomClass->GetVciName()) 
		{
			return pTclPcomClass;
			break;
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPtrList	g_TclPcomObjList;	// PCOM�����б�

CTclPcomObj::CTclPcomObj()
{
	m_pClass	= NULL;
	m_pPcomObj	= NULL;
	m_Interp	= NULL;
	m_strVciName= "";
	m_strShareName = "";
	m_strErrorInfo = "";
	m_nRefCount	= 1;
	g_TclPcomObjList.AddTail(this);
}

CTclPcomObj::~CTclPcomObj()
{
	// �ر����
	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		if(pTclInterp)
		{
			g_pIPlatUI = pTclInterp->pIPlatUI;
		}
	}
	if(g_pIPlatUI && (m_strShareName == ""))	// sharename�ǿյ���CTclPcomObj::CloseVciObject�����Ѿ�����ƽ̨���ͷź���
	{
		g_pIPlatUI->ReleaseVciObject(m_pPcomObj, FALSE);
	}

	m_pPcomObj = NULL;

	// �ͷ�PcomClass����
	if(m_pClass != NULL)
	{
		// �ж����ü���
		if(m_pClass->ReleaseRef() == 0)
		{
			// ɾ���б��е����ݺͶ���
			int count = g_TclPcomClassList.GetCount();
			for(int i = 0; i < count; i ++)
			{
				POSITION pos = g_TclPcomClassList.FindIndex(i);
				CTclPcomClass* pTclPcomClass = (CTclPcomClass*)g_TclPcomClassList.GetAt(pos);
				if(m_pClass == pTclPcomClass) 
				{
					g_TclPcomClassList.RemoveAt(pos);
					delete m_pClass;
					m_pClass = NULL;
					break;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ж�PCOM�����Ƿ��Ѿ���
// ����1: �����
// ����2: ��Ҫ�İ汾
// ����3: ������
/////////////////////////////////////////////////////////////////////////////
BOOL CTclPcomObj::IsOpen(CString strVciName, CString strVer, CString strShareName)
{
	if((strVciName == m_strVciName) && (strShareName != "") && (strShareName == m_strShareName))
	{
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��һ��PCOM���
// ����1: �����
// ����2: ��Ҫ�İ汾
// ����3: ������
// ����4: ָ��PcomObj����ָ��,Ĭ��Ϊ��,���ָ����ʹ��ָ����ָ��,���ô����µĶ���
/////////////////////////////////////////////////////////////////////////////
BOOL CTclPcomObj::OpenVciObject(CString strVciName, CString strVer, CString strShareName, LPVOID pPcomObj)
{
	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)m_Interp);
		IPlatUI* pIPlatUI = NULL;
		if(pTclInterp)
		{
			pIPlatUI = pTclInterp->pIPlatUI;
			g_pIPlatUI = pIPlatUI;
		}
	}
	if(g_pIPlatUI == NULL)
	{
		m_strErrorInfo = _T("Get platform interface failed.");
		return FALSE;
	}

	// 1.���Ȳ����Ƿ���ڶ�Ӧ��PcomClass����
	int count = g_TclPcomClassList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomClassList.FindIndex(i);
		CTclPcomClass* pTclPcomClass = (CTclPcomClass*)g_TclPcomClassList.GetAt(pos);
		if(strVciName == pTclPcomClass->GetVciName()) 
		{
			m_pClass = pTclPcomClass;
			pTclPcomClass->AddRef();
			break;
		}
	}

	// 2.δ�ҵ��򴴽�һ���µ�
	if(m_pClass == NULL)
	{
		m_pClass = new CTclPcomClass;
		g_TclPcomClassList.AddTail(m_pClass);
		m_pClass->AddRef();
		m_pClass->SetVciName(strVciName);
		// ��PcomClass�����Լ���ͷ�ļ�
		m_pClass->LoadHeadFile();
	}

	// 3.����ƽ̨�ӿڴ����
	m_pPcomObj = NULL;

	if(m_pPcomObj == NULL)
	{
		// ����ƽ̨�ӿڴ����
		if(strVciName == VCI_NAME_PLATUI)
		{
			m_pPcomObj = g_pIPlatUI;
			m_strVciName = VCI_NAME_PLATUI;
		}else
		{
			if(pPcomObj != NULL)
			{
				// ������ݵĲ�������PcomObjָ��,��ֱ��ʹ��ָ����ָ��
				m_pPcomObj = pPcomObj;
			}else
			{
				// ���򴴽�����
				m_pPcomObj = g_pIPlatUI->CreateVciObject(strVciName, strShareName);
			}
			m_strVciName = strVciName;
			m_strShareName = strShareName;
		}
		if(m_pPcomObj == NULL)
		{
			m_strErrorInfo = _T("Not find device manager interface or not find this device.");
			return FALSE;
		}
	}else
	if(strShareName != "")	// ���sharename�ǿ�,������VCI��������ü���
	{
		if(strVciName != VCI_NAME_PLATUI)
		{
			m_pPcomObj = g_pIPlatUI->CreateVciObject(strVciName, strShareName);
			m_nRefCount++;	// �������ü���
		}
	}

	if(m_pPcomObj == NULL)
	{
		m_strErrorInfo = _T("Open device failed.");
	}

	return (m_pPcomObj != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// �ر�PCOM�������
/////////////////////////////////////////////////////////////////////////////
void CTclPcomObj::CloseVciObject(CTclPcomObj* pPcomObj)
{
	// ���sharename�ǿ�,���ж����ü����Ƿ�Ϊ0
	if(pPcomObj->m_strShareName != "")
	{
		if(g_pIPlatUI == NULL)
		{
			CTclInterp* pTclInterp = GetTclInterp((LPVOID)(pPcomObj->m_Interp));
			if(pTclInterp)
			{
				g_pIPlatUI = pTclInterp->pIPlatUI;
			}
		}
		if(g_pIPlatUI)
		{
			pPcomObj->m_nRefCount--;
			if(g_pIPlatUI->ReleaseVciObject(pPcomObj->m_pPcomObj, FALSE) && (pPcomObj->m_nRefCount > 0))
			{
				// ����TRUE��ʾ��VCI�������ü�������Ϊ0,���ͬʱTclPcomObj�����ü���Ҳ����0,����ɾ���˶���
				return;
			}
		}
	}

	// ж�ػص�����
	int count = g_TclPcomCallBackList.GetCount();
	int i;
	for(i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(pPcomObj==pTclPcomCallBack->m_pPcomObject) 
		{
			CTclPcomCallBack::UnInstallCallBack(pPcomObj, pTclPcomCallBack->m_nEventID);
			break;
		}
	}

	// ɾ�������������б��е���
	count = g_TclPcomObjList.GetCount();
	for(i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomObjList.FindIndex(i);
		CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
		if(pPcomObj == pTclPcomObj) 
		{
			g_TclPcomObjList.RemoveAt(pos);
			delete pTclPcomObj;
			break;
		}
	}

	// ɾ�������Ļص�����
	count = g_TclPcomCallBackList.GetCount();
	for(i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(pPcomObj == pTclPcomCallBack->m_pPcomObject) 
		{
			delete pTclPcomCallBack;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ͷ�VCI����
/////////////////////////////////////////////////////////////////////////////
void CTclPcomObj::ReleaseVciObject(BOOL bForce)
{
	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)(m_Interp));
		if(pTclInterp)
		{
			g_pIPlatUI = pTclInterp->pIPlatUI;
		}
	}
	if(g_pIPlatUI)
	{
		m_nRefCount--;
		if(bForce)
		{
			m_nRefCount = 0;
		}
		g_pIPlatUI->ReleaseVciObject(m_pPcomObj, bForce);
	}
}

/////////////////////////////////////////////////////////////////////////////
// �ر�����PCOM�������
/////////////////////////////////////////////////////////////////////////////
void CloseAllVciObject()
{
	// ɾ�������������б��е���
	int count = g_TclPcomObjList.GetCount();
	int i;
	for(i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclPcomObjList.FindIndex(i);
		CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
		if(pTclPcomObj != NULL)
		{
			pTclPcomObj->ReleaseVciObject(TRUE);
			g_TclPcomObjList.RemoveAt(pos);
			delete pTclPcomObj;
		}
	}

	// ɾ�����лص�����
	count = g_TclPcomCallBackList.GetCount();
	for(i = count-1; i >= 0; i --)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(pTclPcomCallBack != NULL) 
		{
			delete pTclPcomCallBack;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����������ָ���Ƿ�Ϸ�
/////////////////////////////////////////////////////////////////////////////
BOOL IsValidTclPcomObj(CTclPcomObj* pObj)
{
	int count = g_TclPcomObjList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomObjList.FindIndex(i);
		CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
		if(pObj == pTclPcomObj)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ����ƽ̨�ӿڵ�TclVci����
/////////////////////////////////////////////////////////////////////////////
BOOL CreatePlatUITclPcomObj()
{
	if(g_pTclPcomObjPlatUI)
	{
		return TRUE;
	}

	g_pTclPcomObjPlatUI = new CTclPcomObj();
	return g_pTclPcomObjPlatUI->OpenVciObject(VCI_NAME_PLATUI, "2.0.0.2", "");
}

/////////////////////////////////////////////////////////////////////////////
// ����ص�����,����ͨ���ű���callback
/////////////////////////////////////////////////////////////////////////////
// ִ��TCL�ű��ĵ���
int TclPcomCallBack(CUIntArray& arParams, LPVOID pCallBackFunc)
{
	int count = g_TclPcomCallBackList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(pCallBackFunc==pTclPcomCallBack->m_pCallBackFunc)
		{
			CTclInterp* pTclInterp = GetTclInterp(pTclPcomCallBack->m_pInterp);
			if(pTclInterp == NULL)
				return 0;
			CString strScript = pTclPcomCallBack->m_strCallBackScript;
			for(int j=0; j<arParams.GetSize(); j++)
			{
				CString strParam;
				strParam.Format(" %d", arParams[j]);
				strScript += strParam;
			}
			//TRACE("%s\n", strScript);
			BOOL bRes = pTclInterp->RunScriptCommand(strScript.GetBuffer(strScript.GetLength()));
			strScript.ReleaseBuffer();
			if(bRes)
			{
				return atoi(pTclInterp->GetResult());
			}else
			{
				TRACE("TclCallBack error:%s\n", pTclInterp->GetResult());
				return 0;
			}
		}
	}
	return 0;
}

// ���������Ļص�
int PcomCallBack0()
{
	CUIntArray arParams;
	return TclPcomCallBack(arParams, PcomCallBack0);
}

// ��1�������Ļص�
int PcomCallBack1(int nParam1)
{
	CUIntArray arParams;
	arParams.Add(nParam1);
	return TclPcomCallBack(arParams, PcomCallBack1);
}

// ��2�������Ļص�
int PcomCallBack2(int nParam1, int nParam2)
{
	CUIntArray arParams;
	arParams.Add(nParam1);
	arParams.Add(nParam2);
	return TclPcomCallBack(arParams, PcomCallBack2);
}

// ��3�������Ļص�
int PcomCallBack3(int nParam1, int nParam2, int nParam3)
{
	CUIntArray arParams;
	arParams.Add(nParam1);
	arParams.Add(nParam2);
	arParams.Add(nParam3);
	return TclPcomCallBack(arParams, PcomCallBack3);
}

// ��4�������Ļص�
int PcomCallBack4(int nParam1, int nParam2, int nParam3, int nParam4)
{
	CUIntArray arParams;
	arParams.Add(nParam1);
	arParams.Add(nParam2);
	arParams.Add(nParam3);
	arParams.Add(nParam4);
	return TclPcomCallBack(arParams, PcomCallBack4);
}

// ��5�������Ļص�
int PcomCallBack5(int nParam1, int nParam2, int nParam3, int nParam4, int nParam5)
{
	CUIntArray arParams;
	arParams.Add(nParam1);
	arParams.Add(nParam2);
	arParams.Add(nParam3);
	arParams.Add(nParam4);
	arParams.Add(nParam5);
	return TclPcomCallBack(arParams, PcomCallBack5);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTclPcomCallBack::CTclPcomCallBack()
{
	m_pInterp			= NULL;
	m_strCallBackScript	= "";
	m_nParamCount		= -1;
	m_pCallBackFunc		= NULL;
	m_pPcomObject		= NULL;
	m_nEventID			= 0;
	g_TclPcomCallBackList.AddTail(this);
}

CTclPcomCallBack::~CTclPcomCallBack()
{
	// ɾ���б��е����ݺͶ���
	int count = g_TclPcomCallBackList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(this == pTclPcomCallBack) 
		{
			g_TclPcomCallBackList.RemoveAt(pos);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// ��װ�ص�����
//////////////////////////////////////////////////////////////////////
CTclPcomCallBack* CTclPcomCallBack::InstallCallBack(LPVOID pInterp, CString strScript, int nParamCount, CTclPcomObj* pObj, int nEventID)
{
	if(nParamCount<0 || nParamCount>5)
	{
		return NULL;
	}

	int count = g_TclPcomCallBackList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if(nParamCount == pTclPcomCallBack->m_nParamCount) 
		{
			if(strScript == pTclPcomCallBack->m_strCallBackScript)
			{
				// ����Ѿ���װ��һ��ͬ����,��ֱ�ӷ��ذ�װ���Ļص�ָ��
				return pTclPcomCallBack;
			}

			// ÿ�ֲ�������ֻ����һ���ص�����
			return NULL;
		}
	}

	// �����µĻص�����
	CTclPcomCallBack* pCallBack = new CTclPcomCallBack();
	pCallBack->m_pInterp		= pInterp;
	pCallBack->m_strCallBackScript= strScript;
	pCallBack->m_nParamCount	= nParamCount;
	pCallBack->m_pPcomObject	= pObj;
	pCallBack->m_nEventID		= nEventID;
	switch(nParamCount)
	{
	case 0: pCallBack->m_pCallBackFunc = PcomCallBack0; break;
	case 1: pCallBack->m_pCallBackFunc = PcomCallBack1; break;
	case 2: pCallBack->m_pCallBackFunc = PcomCallBack2; break;
	case 3: pCallBack->m_pCallBackFunc = PcomCallBack3; break;
	case 4: pCallBack->m_pCallBackFunc = PcomCallBack4; break;
	case 5: pCallBack->m_pCallBackFunc = PcomCallBack5; break;
	}
	
	return pCallBack;
}

//////////////////////////////////////////////////////////////////////
// ж�ػص�����
//////////////////////////////////////////////////////////////////////
BOOL CTclPcomCallBack::UnInstallCallBack(CTclPcomObj* pObj, int nEventID)
{
	int count = g_TclPcomCallBackList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if((pObj==pTclPcomCallBack->m_pPcomObject) && (nEventID==pTclPcomCallBack->m_nEventID)) 
		{
			delete pTclPcomCallBack;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ�ص���������
//////////////////////////////////////////////////////////////////////
CTclPcomCallBack* CTclPcomCallBack::GetCallBack(CTclPcomObj* pObj, int nEventID)
{
	int count = g_TclPcomCallBackList.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_TclPcomCallBackList.FindIndex(i);
		CTclPcomCallBack* pTclPcomCallBack = (CTclPcomCallBack*)g_TclPcomCallBackList.GetAt(pos);
		if((pObj==pTclPcomCallBack->m_pPcomObject) && (nEventID==pTclPcomCallBack->m_nEventID)) 
		{
			return pTclPcomCallBack;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// ����������õ�TCL��չ����

/////////////////////////////////////////////////////////////////////////////
// ��չ����:��һ��VCI���
// ��  ��: vci::open name ?sharename? ?ver?
// ����ֵ: ������
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DeviceOpen(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *CONST objv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(objc < 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "name ?sharename? ?ver?");
		return TCL_ERROR;
	}

	// ��ȡ����
	CString strName = ConvertUTFParam(Tcl_GetString(objv[1]));
	CString strVer = "";
	CString strShareName = "";
	if(objc >= 3)
	{
		strShareName = ConvertUTFParam(Tcl_GetString(objv[2]));
	}
	if(objc >= 4)
	{
		strVer = ConvertUTFParam(Tcl_GetString(objv[3]));
	}

	// �����Ƿ���id,ver,sharename��ͬ�Ķ���,����оͲ����ٴ�,ֱ�ӷ��ض���ָ��
	if(strShareName != "")
	{
		int count = g_TclPcomObjList.GetCount();
		for(int i = 0; i < count; i ++)
		{
			POSITION pos = g_TclPcomObjList.FindIndex(i);
			CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
			if((pTclPcomObj != NULL) && pTclPcomObj->IsOpen(strName, strVer, strShareName))
			{
				pTclPcomObj->OpenVciObject(strName, strVer, strShareName);
				sprintf(interp->result, "%d", pTclPcomObj);
				return TCL_OK;
			}
		}
	}

	CTclPcomObj* pTclPcomObj = new CTclPcomObj;
	pTclPcomObj->SetInterp(interp);
	if(!pTclPcomObj->OpenVciObject(strName, strVer, strShareName))
	{
		sprintf(interp->result, "%s", pTclPcomObj->GetErrorInfo());
		CTclPcomObj::CloseVciObject(pTclPcomObj);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}else
	{
		sprintf(interp->result, "%d", pTclPcomObj);
		return TCL_OK;
	}	
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:�ر�һ��VCI���
// ��  ��: vci::close handle
// ����ֵ: �Ƿ�ɹ�
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DeviceClose(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *CONST objv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(objc < 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "handle");
		return TCL_ERROR;
	}

	// ��ȡ����
	CString strpDev = ConvertUTFParam(Tcl_GetString(objv[1]));
	CTclPcomObj* pTclPcomObj = (CTclPcomObj*)(atoi(strpDev));

	// ���ָ��ĺϷ���
	if(!IsValidTclPcomObj(pTclPcomObj))
	{
		Tcl_AppendResult(interp, "Invalid device handle: ", strpDev, (char *) NULL);
		return TCL_ERROR;
	}

	CTclPcomObj::CloseVciObject(pTclPcomObj);

	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:VCI����ĺ�������
// ��  ��: vci::info -list | -add | -remove
// -listclass			: �е�ǰ����VCI��
// -listobject ?vciid?	: �д򿪵�������������ĳһ�ֶ���
// -getvcihandle tclvciobj	: ��ȡָ��TclVci�����VCI���
// -gettclvciobj vciid vcihandle	: ��ȡָ��VCI�����TclVci��װ������
// -listinstalled ?vciid? ?verlist|version? : �а�װ�������Ϣ
// -listdef vciid		: �к�������
// -listfunc vciid		: �к�����
// -listparam vciid func ?param? : �к���������ϸ��Ϣ
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DeviceInfo(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *CONST objv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(objc < 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "-opr(listdef|listfunc|listparam|listclass|listobject|listinstalled|getvcihandle|gettclvciobj) ...");
		return TCL_ERROR;
	}

	CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
	if(pTclInterp)
	{
		g_pIPlatUI = pTclInterp->pIPlatUI;
	}
	if(g_pIPlatUI == NULL)
	{
		Tcl_AppendResult(interp, "Get platform interface fail.", (char *) NULL);
		return TCL_ERROR;
	}

	// ��ȡ����
	CString strOpr = ConvertUTFParam(Tcl_GetString(objv[1]));
	CString strVciName = "";
	CTclPcomClass* pTclPcomClass = NULL;
	if(objc >= 3)
	{
		strVciName = ConvertUTFParam(Tcl_GetString(objv[2]));
		pTclPcomClass = GetTclPcomClass(strVciName);
	}

	if(strOpr == "-listclass")	// �е�ǰ����VCI��
	{
		int count = g_TclPcomClassList.GetCount();
		for(int i = 0; i < count; i ++)
		{
			POSITION pos = g_TclPcomClassList.FindIndex(i);
			CTclPcomClass* pTclPcomClass = (CTclPcomClass*)g_TclPcomClassList.GetAt(pos);
			Tcl_AppendElement(interp, pTclPcomClass->GetVciName());
		}

		ConvertResultToUTF(interp);
		return TCL_OK;
	}else
	if(strOpr == "-listobject")	// �е�ǰ�򿪵�VCI����
	{
		if(strVciName == "")	// ������VCI������
		{
			int count = g_TclPcomObjList.GetCount();
			for(int i = 0; i < count; i ++)
			{
				POSITION pos = g_TclPcomObjList.FindIndex(i);
				CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
				if(pTclPcomObj != NULL)
				{
					CString strHandle;
					strHandle.Format("%d", pTclPcomObj);
					Tcl_AppendElement(interp, strHandle);
				}
			}
		}else	// ��ָ����������ж���ʵ�����
		{
			if(!pTclPcomClass)
			{
				Tcl_AppendResult(interp, "Vci class ", strVciName, " not find.", (char *) NULL);
				return TCL_ERROR;
			}

			int count = g_TclPcomObjList.GetCount();
			for(int i = 0; i < count; i ++)
			{
				POSITION pos = g_TclPcomObjList.FindIndex(i);
				CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
				if(pTclPcomObj->GetTclPcomClass() == pTclPcomClass)
				{
					CString strHandle;
					strHandle.Format("%d", pTclPcomObj);
					Tcl_AppendElement(interp, strHandle);
				}
			}
		}

		ConvertResultToUTF(interp);
		return TCL_OK;
	}else
	if(strOpr == "-getvcihandle")	// ��ȡָ��TclVci�����VCI���
	{
		if(objc < 3)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "-getvcihandle tclvciobj");
			return TCL_ERROR;
		}

		CTclPcomObj* pTclPcomObj = (CTclPcomObj*)(atoi(strVciName));

		int count = g_TclPcomObjList.GetCount();
		for(int i = 0; i < count; i ++)
		{
			POSITION pos = g_TclPcomObjList.FindIndex(i);
			CTclPcomObj* pTclPcomObjTmp = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
			if(pTclPcomObjTmp == pTclPcomObj)
			{
				CString strHandle;
				strHandle.Format("%d", pTclPcomObj->GetPcomObj());
				Tcl_AppendElement(interp, strHandle);
				return TCL_OK;
			}
		}

		ConvertResultToUTF(interp);
		return TCL_OK;
	}else
	if(strOpr == "-gettclvciobj")	// ��ȡָ��VCI�����TclVci��װ������,�����װ�����ھʹ���
	{
		if(objc < 3)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "-gettclvciobj vciid vcihandle");
			return TCL_ERROR;
		}

		void* pPcomObj = (void*)(atoi(ConvertUTFParam(Tcl_GetString(objv[3]))));

		// ����ҵ�,�򷵻�ָ��
		if(pTclPcomClass)
		{
			int count = g_TclPcomObjList.GetCount();
			for(int i = 0; i < count; i ++)
			{
				POSITION pos = g_TclPcomObjList.FindIndex(i);
				CTclPcomObj* pTclPcomObj = (CTclPcomObj*)g_TclPcomObjList.GetAt(pos);
				if( (pTclPcomObj->GetTclPcomClass() == pTclPcomClass) &&
					(pTclPcomObj->GetPcomObj() == pPcomObj) )
				{
					CString strHandle;
					strHandle.Format("%d", pTclPcomObj);
					Tcl_AppendElement(interp, strHandle);
					return TCL_OK;
				}
			}
		}

		// δ�ҵ��򴴽�TclPcom����
		CString strInstanceName = g_pIPlatUI->GetInstanceNameByObject(strVciName, pPcomObj);
		CTclPcomObj* pTclPcomObj = new CTclPcomObj;
		pTclPcomObj->SetInterp(interp);
		if(!pTclPcomObj->OpenVciObject(strVciName, "", strInstanceName, pPcomObj))
		{
			sprintf(interp->result, "%s", pTclPcomObj->GetErrorInfo());
			CTclPcomObj::CloseVciObject(pTclPcomObj);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}

		CString strHandle;
		strHandle.Format("%d", pTclPcomObj);
		Tcl_AppendElement(interp, strHandle);
		ConvertResultToUTF(interp);
		return TCL_OK;
	}else
	if(strOpr == "-listinstalled")	// �е�ǰ�����Ѿ���װ������ӿ�
	{
		CStringArray asVciId;
		g_pIPlatUI->GetInstalledVci(asVciId);

		if(strVciName == "")	// û�нӿ�������,��ʾ����������ӿ�
		{
			for(int i = 0; i < asVciId.GetSize(); i ++)
			{
				Tcl_AppendElement(interp, asVciId[i]);
			}
		}else
		{
			// ���Ҵ�����Ƿ�װ
			int i;
			for(i = 0; i < asVciId.GetSize(); i ++)
			{
				if(strVciName == asVciId[i])
					break;
			}
			if(i >= asVciId.GetSize())
			{
				Tcl_AppendResult(interp, "Not find vci component ", strVciName, ".", (char *) NULL);
				ConvertResultToUTF(interp);
				return TCL_ERROR;
			}

			Tcl_AppendElement(interp, g_pIPlatUI->GetVciVersion(strVciName));
			Tcl_AppendElement(interp, g_pIPlatUI->GetVciPath(strVciName));
			Tcl_AppendElement(interp, g_pIPlatUI->GetVciProvider(strVciName));
			Tcl_AppendElement(interp, g_pIPlatUI->GetVciType(strVciName));
			Tcl_AppendElement(interp, g_pIPlatUI->GetVciInterfaceFile(strVciName));
			Tcl_AppendElement(interp, g_pIPlatUI->GetVciAboutPage(strVciName));
			ConvertResultToUTF(interp);
			return TCL_OK;
		}

		ConvertResultToUTF(interp);
		return TCL_OK;
	}

	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "-opr(listdef|listfunc|listparam) vciid");
		return TCL_ERROR;
	}
	if(!pTclPcomClass)
	{
		Tcl_AppendResult(interp, "Vci class ", strVciName, " not find.", (char *) NULL);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}

	if(strOpr == "-listdef")	// �к�������
	{
		for(int i=0; i<pTclPcomClass->m_arFuncs.GetSize(); i++)
		{
			CString strDef = pTclPcomClass->m_arFuncs[i].GetFuncDef();
			Tcl_AppendElement(interp, strDef);
		}
	}else	
	if(strOpr == "-listfunc")	// �к�����
	{
		for(int i=0; i<pTclPcomClass->m_arFuncs.GetSize(); i++)
		{
			CString strName = pTclPcomClass->m_arFuncs[i].GetFuncName();
			Tcl_AppendElement(interp, strName);
		}
	}else
	if(strOpr == "-listparam")	// �к�������
	{
		if(objc < 4)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "-listparam vciid func ?param?");
			return TCL_ERROR;
		}

		CString strFunc = ConvertUTFParam(Tcl_GetString(objv[3]));
		// ���Һ����Ƿ����
		// ����������ͬ�ĺ���,����ͨ���ں��������"(No)"����ʾ
		CTclPcomFunc* pTclPcomFunc = pTclPcomClass->FindFunction(strFunc);
		if(!pTclPcomFunc)
		{
			Tcl_AppendResult(interp, "Not find function ", strFunc, " in vci ", pTclPcomClass->GetVciName(), ".", (char *) NULL);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}

		if(objc == 4)	// �в�����
		{
			for(int i=0; i<pTclPcomFunc->m_arParams.GetSize(); i++)
			{
				CString strParamName = pTclPcomFunc->m_arParams[i].m_strParamName;
				Tcl_AppendElement(interp, strParamName);
			}
			ConvertResultToUTF(interp);
			return TCL_OK;
		}

		// ��ȡ��������ϸ���
		CString strParam = ConvertUTFParam(Tcl_GetString(objv[4]));
		// ���Ҳ����Ƿ����
		CTclPcomParam* pTclPcomParam = pTclPcomFunc->FindParam(strParam);
		if(!pTclPcomParam)
		{
			Tcl_AppendResult(interp, "Not find param ", strParam, " in func ", strFunc, ".", (char *) NULL);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}

		CString strDetail;
		strDetail.Format("%d", pTclPcomParam->m_nType);		// ����
		Tcl_AppendElement(interp, strDetail);
		strDetail.Format("%d", pTclPcomParam->m_nMode);		// ���÷�ʽ
		Tcl_AppendElement(interp, strDetail);
		Tcl_AppendElement(interp, pTclPcomParam->m_strDefault);	// ȱʡֵ

	}else
	{
		Tcl_AppendResult(interp, "operater must be -listclass, -listobject, -listinstalled, -getvcihandle, -gettclvciobj, -listdef, -listfunc or -listparam.",  (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:����VCI�������ĺ���
// ��  ��: vci::exec handle func params...
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DeviceExec(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *CONST objv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(objc < 3)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "handle func params...");
		return TCL_ERROR;
	}

	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
		if(pTclInterp)
		{
			g_pIPlatUI = pTclInterp->pIPlatUI;
		}
	}

	// ��ȡ����
	CString strpDev = ConvertUTFParam(Tcl_GetString(objv[1]));
	CString strFunc = ConvertUTFParam(Tcl_GetString(objv[2]));
	CTclPcomObj* pTclPcomObj = NULL;
	if(strpDev == "plat")
	{
		// ��ʾ����ƽ̨�ӿ�
		CreatePlatUITclPcomObj();
		pTclPcomObj = g_pTclPcomObjPlatUI;
	}else
	{
		pTclPcomObj = (CTclPcomObj*)(atoi(strpDev));
	}
	// ���ָ��ĺϷ���
	if(!IsValidTclPcomObj(pTclPcomObj))
	{
		Tcl_AppendResult(interp, "Invalid vci object handle: ", strpDev, (char *) NULL);
		return TCL_ERROR;
	}

	// ���PCOM���װ�����Ƿ����
	CTclPcomClass* pTclPcomClass = pTclPcomObj->GetTclPcomClass();
	if(!pTclPcomClass)
	{
		Tcl_AppendResult(interp, "Not find vci define object.", (char *) NULL);
		return TCL_ERROR;
	}

	// ���Һ����Ƿ����
	// ����������ͬ�ĺ���,����ͨ���ں��������"(No)"����ʾ
	CTclPcomFunc* pTclPcomFunc = pTclPcomClass->FindFunction(strFunc);
	if(!pTclPcomFunc)
	{
		Tcl_AppendResult(interp, "Not find function ", strFunc, " in vci ", pTclPcomClass->GetVciName(), ".", (char *) NULL);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}

	// �˺����Ƿ��ִ��
	if(!pTclPcomFunc->EnableExec())
	{	// ��δ֪���Ͳ���,��������ִ��
		Tcl_AppendResult(interp, "Can't exec function ", strFunc, ", because have some unknown type paramter, or too many paramters.", (char *) NULL);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}

	// �жϲ��������Ƿ�(ȥ��ȱʡ����������ٲ�������)
	int nLeastParams = pTclPcomFunc->m_nLeastParams;
	if(nLeastParams > (objc-3))
	{
		CString strArgNum;
		strArgNum.Format("%d", nLeastParams);
		Tcl_AppendResult(interp, "wrong number of args: need at least ", strArgNum, " parameters of function ", strFunc, ".", (char *) NULL);
		ConvertResultToUTF(interp);
		return TCL_ERROR;
	}

	// ��ʼִ�к���

	// 1.�ڶ�ӦTclPcomClass�����в��Ҵ˺�������Ӧ������
	// ע:��PCOM��װ��,ǰ�����������صĺ���,���Ҫ��3
	int nFuncIndex = pTclPcomClass->GetFunctionIndex(strFunc) + 3;

	// 2.�������
	void* pPcomObj = pTclPcomObj->GetPcomObj();

	// ����һ����ʱ�Ĳ�����������,���ڱ�����Ϣ
	CTclPcomParamArray arLocalParams;
	int i;
	for(i=0; i<pTclPcomFunc->m_arParams.GetSize(); i++)
	{
		arLocalParams.Add(pTclPcomFunc->m_arParams[i]);
	}

	// ���ڲ������ݵĻ�����,���ȼ������в������η��ڴ˻�������
	char bufParam[TCLPCOM_MAXPARAMSIZE];
	memset(bufParam, 0, TCLPCOM_MAXPARAMSIZE);

	// stdcallΪ���ҵ����ѹջ����
	int nParamCount = arLocalParams.GetSize();
	int nPos = 0;
	for(i=nParamCount-1; i >= 0; i--)
	{
		// �������
		CString strVar = "";
		CString strValue = "";
		CStringArray asValue;

		if( (PTYPE_CSTRINGARRAY == arLocalParams[i].GetParamType()) ||
			(PTYPE_UINTARRAY == arLocalParams[i].GetParamType()) )
		{
			Tcl_Obj* listObj = NULL;

			if(arLocalParams[i].NeedRefVar())
			{
				if((3+i) < objc)
				{
					strValue = ConvertUTFParam(Tcl_GetString(objv[3+i]));
				}
				// ��ʾ����Ĳ�����һ���б���,��ȡ�б�����,��ӵ�����
				strVar = strValue;
				listObj = Tcl_GetVar2Ex(interp, strVar.GetBuffer(strVar.GetLength()), NULL, TCL_LIST_ELEMENT);
				strVar.ReleaseBuffer();
				
			}else
			{
				if((3+i) < objc)
				{
					listObj = objv[3+i];
				}
			}

			if(listObj != NULL)
			{
				int nListCount = 0;
				Tcl_Obj** paList;
				if(TCL_ERROR == Tcl_ListObjGetElements(interp, listObj, &nListCount, &paList))
				{
					Tcl_AppendResult(interp, "get list elements from param error.",  (char *) NULL);
					return TCL_ERROR;
				}
				for(int k=0; k<nListCount; k++)
				{
					Tcl_Obj* list = (paList[k]);
					asValue.Add(ConvertUTFParam(list->bytes));
				}
			}

		}else
		{

			if((3+i) < objc)
			{
				strValue = ConvertUTFParam(Tcl_GetString(objv[3+i]));
			}

			if(arLocalParams[i].NeedRefVar())
			{
				// ��ʾ����Ĳ�����һ��������,��ȡ�������ĳ�ֵ
				strVar = strValue;
				CString szValue	= Tcl_GetVar(interp, strVar.GetBuffer(strVar.GetLength()), TCL_LEAVE_ERR_MSG);
				strVar.ReleaseBuffer();
				strValue = szValue;
			}
		}

		// ����ǰ�װ��ж�ػص�����������,�����2�����ô����,��Ӧ���ô�������Ӧ��C���Ժ���ָ��
		if((strFunc=="InstallHandler" || strFunc=="UninstallHandler") && (i==1))
		{
			if(strFunc=="InstallHandler")	// ��װ
			{
				// ���ýű���ȡ��������
				char szScript[64];
				sprintf(szScript, "llength [info args %s]", strValue);
				if(TCL_OK != Tcl_Eval(interp, szScript))
				{
					// ��ѯ�����б����,������Ϊָ���Ļص��ű�����������
					Tcl_AppendResult(interp, "Get device callback func ", strValue, " args list error.", (char *) NULL);
					ConvertResultToUTF(interp);
					return TCL_ERROR;
				}			

				int nArgsCount = atoi(Tcl_GetStringResult(interp));
				Tcl_ResetResult(interp);

				CTclPcomCallBack* pCallBack = 
					CTclPcomCallBack::InstallCallBack(interp, strValue, nArgsCount,
								pTclPcomObj, arLocalParams[0].m_uiUint);
				if(pCallBack == NULL)
				{
					// ��װ�ص�ʧ��,��������Ϊ�ص���Դ�Ѿ���ռ��
					Tcl_AppendResult(interp, "Tcl vci callback func ", strValue, " install failed.", (char *) NULL);
					ConvertResultToUTF(interp);
					return TCL_ERROR;
				}else
				{
					// ����strValueΪʵ�ʵĺ�����ַ
					strValue.Format("%d", pCallBack->m_pCallBackFunc);
				}
			}else	// ж��
			{
				CTclPcomCallBack* pCallBack = CTclPcomCallBack::GetCallBack(pTclPcomObj, arLocalParams[0].m_uiUint);
				if(pCallBack == NULL)
				{
					// ж��ʧ��,δ�ҵ��ص�����
					Tcl_AppendResult(interp, "Tcl vci callback func ", strValue, " uninstall failed.", (char *) NULL);
					ConvertResultToUTF(interp);
					return TCL_ERROR;
				}else
				{
					// ����strValueΪʵ�ʵĺ�����ַ
					strValue.Format("%d", pCallBack->m_pCallBackFunc);
					CTclPcomCallBack::UnInstallCallBack(pTclPcomObj, arLocalParams[0].m_uiUint);
				}
			}
		}

		// ������ʼ��(VarInit�ڲ����Դ���ȱʡ����)
		// ע:Ŀǰ��ȱʡ�����Ĵ���������,����ȱʡֵ�Ǻ궨���,������Ч����
		if( (PTYPE_CSTRINGARRAY == arLocalParams[i].GetParamType()) ||
			(PTYPE_UINTARRAY == arLocalParams[i].GetParamType()) )
		{
			arLocalParams[i].VarInitArray(strVar, &asValue);
		}else
		{
			arLocalParams[i].VarInit(strVar, strValue);
		}

		// �Ȱ�����򵥵�,��Ϊÿ��������ռ4���ֽڵķ���ʵ��,�Ժ�����ǿ
		// �������ڰ��ձ�����С/����ֵ�����ɶԷ���
		//(int*) pSize = (int*)((char*)bufParam+nPos);
		//memcpy(bufParam+nPos, ,arLocalParams[i].GetParamSize());
		//nPos += arLocalParams[i].GetParamSize();
		memcpy(bufParam+nPos, &(arLocalParams[i].m_nCommVar), sizeof(int));
		nPos += sizeof(int);
	}

	// �������ֵΪCString����,ʵ���ϴ��ݲ���ʱ��Ҫ�ഫ��һ��
	int nParam_RetCount = nParamCount;

	// ��ʼ������ֵ����
	int nTmpRet = 0; // ���ڴ洢����ֵ����ʱ����,ֻ���ڷ���CString�����,
					 // ��Ϊ��������·��ص�eaxû���ô�,��Ҫ�Ҹ���ʱ�����洢,
					 // ���⸲�ǵ���ȷ�Ľ��
	void* pTmpRet = &nTmpRet;
	if(PTYPE_CSTRING == pTclPcomFunc->m_Ret.m_nType)
	{
		// ����ֵΪCString�����,��Ҫ�ഫ��һ������,��һ����ʱCString�����ĵ�ַ
		pTclPcomFunc->m_Ret.m_nMode	= PMODE_REFERENCE;
		pTclPcomFunc->m_Ret.VarInit("", "");
		memcpy(bufParam+nPos, &(pTclPcomFunc->m_Ret.m_nCommVar), sizeof(int));
		nParam_RetCount++;
		nPos += sizeof(int);
		memcpy(bufParam+nPos, &(pTmpRet), sizeof(int));
	}else
	if(PTYPE_CTIME == pTclPcomFunc->m_Ret.m_nType)
	{
		// ����ֵΪCTime�����,��Ҫ�ഫ��һ������,��һ����ʱCTime�����ĵ�ַ
		pTclPcomFunc->m_Ret.m_nMode	= PMODE_REFERENCE;
		pTclPcomFunc->m_Ret.VarInit("", "");
		memcpy(bufParam+nPos, &(pTclPcomFunc->m_Ret.m_nCommVar), sizeof(int));
		nParam_RetCount++;
		nPos += sizeof(int);
		memcpy(bufParam+nPos, &(pTmpRet), sizeof(int));
	}else
	{
		pTclPcomFunc->m_Ret.m_nMode	= PMODE_REFERENCE;
		pTclPcomFunc->m_Ret.VarInit("", "");
		memcpy(bufParam+nPos, &(pTclPcomFunc->m_Ret.m_nCommVar), sizeof(int));
	}

	int nRetTypeIsFloat = 0;	// ����ֵ�����Ƿ�float�ı�־
	if(PTYEP_FLOAT == pTclPcomFunc->m_Ret.m_nType)
	{
		nRetTypeIsFloat = 1;
	}

	// 3.ͨ����������÷�װ
	//int nRet;
	//void* pRet = (void*)(&nRet);
	try
	{
		_asm
		{
			// ���滷���Ĵ���
			push	eax;
			push	edx;
			push	ecx;

			// �Ӳ�����������ѭ��ȡ������ѹջ
			mov		ecx, nParam_RetCount;

			// �жϲ�������,���Ϊ0����ִ�������ѹջ����
			cmp		ecx, 0;
			jz		next;

			lea		edx, bufParam;
	again:	mov		eax, [edx];
			push	eax;
			add		edx, 4;
			dec		ecx
			jecxz	next;
			jmp		again;

			// �����׵�ַѹջ
	next:	mov		eax, pPcomObj;
			push	eax;

			// ���ݺ�������ֵ���㺯�����麯�����е�λ��
			mov		edx, [eax];
			mov		eax, nFuncIndex;
			shl		eax, 2;
			// �����麯�����ж�Ӧλ�õĺ���
			call	dword ptr [edx+eax];

			// ���ݷ���ֵ
			lea		edx, bufParam;
			mov		ecx, nParam_RetCount;
			shl		ecx, 2;
			add		edx, ecx;

			// �жϷ���ֵ�����Ƿ�float,����Ǿʹ�Э������ջ��ȡ����,�����Ǵ�eaxȡ
			mov		ecx, nRetTypeIsFloat;
			jecxz	reteax;
			mov		ecx, dword ptr [edx];
			fst		dword ptr [ecx];
			jmp		end;

	reteax:	mov		ecx, dword ptr [edx];	// �����Ļ�ȡ����ֵ����(ȡeax��ֵ)
			mov		dword ptr [ecx], eax;

//			lea		edx, nRet;				// ���÷���ֵ(ȡ�����ַ���)
//			mov		dword ptr [edx], eax;

			// �ָ������Ĵ���
	end:	pop		ecx;
			pop		edx;
			pop		eax;
		}

		// 4.�ش�����
		for(i=0; i<nParamCount; i++)
		{
			CString strVarRet = arLocalParams[i].VarRelease();
			if(arLocalParams[i].NeedRefVar())
			{
				if(PTYPE_CSTRINGARRAY == arLocalParams[i].GetParamType())
				{
					CString strVar = arLocalParams[i].m_strRefVar;
					Tcl_Obj* listObj = Tcl_GetVar2Ex(interp, strVar.GetBuffer(strVar.GetLength()), NULL, TCL_LIST_ELEMENT);
					strVar.ReleaseBuffer();
					int nLength = 0;
					int nResult = Tcl_ListObjLength(interp, listObj, &nLength);
					if(nLength == 0)
					{
						// ����б���Ϊ0,��Ҫ�´���һ���б�,���������Ӳ��������쳣
						listObj = Tcl_NewListObj(0, NULL);
						listObj = Tcl_SetVar2Ex(interp, strVar.GetBuffer(strVar.GetLength()), NULL, listObj, TCL_LIST_ELEMENT);
						strVar.ReleaseBuffer();
					}

					for(int el = 0; el < arLocalParams[i].m_asString.GetSize(); el++)
					{
						CString strElement = arLocalParams[i].m_asString[el];
						strElement = ConvertStringToUTF(strElement);
						Tcl_Obj* objPtr = Tcl_NewStringObj(strElement.GetBuffer(strElement.GetLength()), strElement.GetLength());
						strElement.ReleaseBuffer();
						Tcl_ListObjAppendElement(interp, listObj, objPtr);
					}
				}else
				if(PTYPE_UINTARRAY == arLocalParams[i].GetParamType())
				{
					CString strVar = arLocalParams[i].m_strRefVar;
					Tcl_Obj* listObj = Tcl_GetVar2Ex(interp, strVar.GetBuffer(strVar.GetLength()), NULL, TCL_LIST_ELEMENT);
					strVar.ReleaseBuffer();
					int nLength = 0;
					int nResult = Tcl_ListObjLength(interp, listObj, &nLength);
					if(nLength == 0)
					{
						// ����б���Ϊ0,��Ҫ�´���һ���б�,���������Ӳ��������쳣
						listObj = Tcl_NewListObj(0, NULL);
						listObj = Tcl_SetVar2Ex(interp, strVar.GetBuffer(strVar.GetLength()), NULL, listObj, TCL_LIST_ELEMENT);
						strVar.ReleaseBuffer();
					}

					for(int el = 0; el < arLocalParams[i].m_auUInt.GetSize(); el++)
					{
						CString strElement;
						strElement.Format("%u", arLocalParams[i].m_auUInt[el]);
						strElement = ConvertStringToUTF(strElement);
						Tcl_Obj* objPtr = Tcl_NewStringObj(strElement.GetBuffer(strElement.GetLength()), strElement.GetLength());
						strElement.ReleaseBuffer();
						Tcl_ListObjAppendElement(interp, listObj, objPtr);
					}
				}else
				{
					strVarRet = ConvertStringToUTF(strVarRet);
					CString strVar = arLocalParams[i].m_strRefVar;
					char* szVar = strVar.GetBuffer(strVar.GetLength());
					char* szVarRet = strVarRet.GetBuffer(strVarRet.GetLength());
					Tcl_SetVar(interp, szVar, szVarRet, TCL_LEAVE_ERR_MSG);
					strVar.ReleaseBuffer();
					strVarRet.ReleaseBuffer();
				}
			}
		}

		// 5.���÷���ֵ
		if( (PTYPE_VOID != pTclPcomFunc->m_Ret.m_nType) &&
			(PTYPE_UNKNOWN != pTclPcomFunc->m_Ret.m_nType) )
		{
			Tcl_ResetResult(interp);
			CString strRet = pTclPcomFunc->m_Ret.VarRelease();
			Tcl_AppendResult(interp, strRet, (char *) NULL);
		}
	}
	catch(...)
	{
		Tcl_AppendResult(interp, "function exec failed, catch a unknown exception.", (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:����VCI�������ĺ���
// ��  ��: 
//	vci::callback handle install eventId command
//	vci::callback handle uninstall eventId command
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_DeviceCallBack(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj *CONST objv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(objc < 5)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "handle install|uninstall eventId command");
		return TCL_ERROR;
	}

	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
		if(pTclInterp)
		{
			g_pIPlatUI = pTclInterp->pIPlatUI;
		}
	}

	// ��ȡ����
	CString strpDev = ConvertUTFParam(Tcl_GetString(objv[1]));
	CString strOperate = ConvertUTFParam(Tcl_GetString(objv[2]));
	int nEventId = atoi(ConvertUTFParam(Tcl_GetString(objv[3])));
	CString strCommand = ConvertUTFParam(Tcl_GetString(objv[4]));
	CTclPcomObj* pTclPcomObj = (CTclPcomObj*)(atoi(strpDev));
	// ���ָ��ĺϷ���
	if(!IsValidTclPcomObj(pTclPcomObj))
	{
		Tcl_AppendResult(interp, "Invalid vci object handle: ", strpDev, (char *) NULL);
		return TCL_ERROR;
	}

	// ���PCOM���װ�����Ƿ����
	CTclPcomClass* pTclPcomClass = pTclPcomObj->GetTclPcomClass();
	if(!pTclPcomClass)
	{
		Tcl_AppendResult(interp, "Not find vci define object.", (char *) NULL);
		return TCL_ERROR;
	}

	// ��ȡIVciControl�ӿ�ָ��
	IVciControl* pVciControl = (IVciControl*)(g_pIPlatUI->GetObjectControlPtrByInstancePtr(pTclPcomObj->GetPcomObj()));
	if(pVciControl == NULL)
	{
		Tcl_AppendResult(interp, "Invalid vci control interface.", (char *) NULL);
		return TCL_ERROR;
	}

	try
	{
	///////////////////////////////////////////////////////

	if(strOperate=="install")	// ��װ
	{
		// ���ýű���ȡ��������
		char szScript[64];
		sprintf(szScript, "llength [info args %s]", strCommand);
		if(TCL_OK != Tcl_Eval(interp, szScript))
		{
			// ��ѯ�����б����,������Ϊָ���Ļص��ű�����������
			Tcl_AppendResult(interp, "Get device callback func ", strCommand, " args list error.", (char *) NULL);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}			

		int nArgsCount = atoi(Tcl_GetStringResult(interp));
		Tcl_ResetResult(interp);

		CTclPcomCallBack* pCallBack = 
			CTclPcomCallBack::InstallCallBack(interp, strCommand, nArgsCount,
						pTclPcomObj, nEventId);
		if(pCallBack == NULL)
		{
			// ��װ�ص�ʧ��,��������Ϊ�ص���Դ�Ѿ���ռ��
			Tcl_AppendResult(interp, "Tcl vci callback func ", strCommand, " install failed.", (char *) NULL);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}else
		{
			pVciControl->InstallHandler(nEventId, pCallBack->m_pCallBackFunc, 0);
		}
	}else
	if(strOperate=="uninstall")	// ж��
	{
		CTclPcomCallBack* pCallBack = CTclPcomCallBack::GetCallBack(pTclPcomObj, nEventId);
		if(pCallBack == NULL)
		{
			// ж��ʧ��,δ�ҵ��ص�����
			Tcl_AppendResult(interp, "Tcl vci callback func ", strCommand, " uninstall failed.", (char *) NULL);
			ConvertResultToUTF(interp);
			return TCL_ERROR;
		}else
		{
			pVciControl->UninstallHandler(nEventId, pCallBack->m_pCallBackFunc);
			CTclPcomCallBack::UnInstallCallBack(pTclPcomObj, nEventId);
		}
	}else
	{
		Tcl_AppendResult(interp, "operater must be install or uninstall.",  (char *) NULL);
		return TCL_ERROR;
	}

	//////////////////////////////////////////////////////////
	}
	catch(...)
	{
		Tcl_AppendResult(interp, "install/uninstall callback failed, catch a unknown exception.", (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ��չ����:OWM��չ����
// owm showview owmid ?active?							- ��ʾOWM View
// owm closeview owmid									- �ر�OWM View
// owm sendcmd owmid CmdCode wParam lParam ?returntype?	- ����OWM����
/////////////////////////////////////////////////////////////////////////////
int Tcl_Cmd_OWM(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])
{
	VERIFY_TCLCMD_RIGHT(USER_OPERATOR);

	if(argc < 2)
	{
		interp->result = "wrong # args: should be \"owm operate params\"";
		return TCL_ERROR;
	}

	if(g_pIPlatUI == NULL)
	{
		CTclInterp* pTclInterp = GetTclInterp((LPVOID)interp);
		if(pTclInterp)
		{
			g_pIPlatUI = pTclInterp->pIPlatUI;
		}
	}
	if(g_pIPlatUI == NULL)
	{
		Tcl_AppendResult(interp, "Get platform interface fail.", (char *) NULL);
		return TCL_ERROR;
	}

	CString strOption = ConvertUTFParam(argv[1]);
	if(strOption[0] == '-')
		strOption.Delete(0, 1);
	strOption.MakeUpper();

	if(strOption == "SHOWVIEW")	// ��ʾOWM View
	{
		if(argc < 3)
		{
			interp->result = "wrong # args: should be \"owm showview owmid ?active?\"";
			return TCL_ERROR;
		}
		CString strOwmId = ConvertUTFParam(argv[2]);

		BOOL bActive = FALSE;
		if(argc > 3)
		{
			bActive = (ConvertUTFParam(argv[3]) == "active");
		}

		int nRet;
		g_pIPlatUI->SendOwmCmd("", OC_OPENVIEW, (WPARAM)(LPCTSTR)strOwmId, bActive, &nRet);

	}else
	if(strOption == "CLOSEVIEW")	// �ر�OWM View
	{
		if(argc < 3)
		{
			interp->result = "wrong # args: should be \"owm closeview owmid\"";
			return TCL_ERROR;
		}
		CString strOwmId = ConvertUTFParam(argv[2]);

		int nRet;
		g_pIPlatUI->SendOwmCmd("", OC_CLOSEVIEW, (WPARAM)(LPCTSTR)strOwmId, 0, &nRet);

	}else
	if(strOption == "SENDCMD")	// ����OWM����
	{
		if(argc < 6)
		{
			interp->result = "wrong # args: should be \"owm sendcmd owmid cmdcode wparam lparam ?returntype?\"";
			return TCL_ERROR;
		}

		CString strReturnType = "";

		CString strOwmId = ConvertUTFParam(argv[2]);
		int nCmdCode = atoi(ConvertUTFParam(argv[3]));
		WPARAM wParam = 0;
		LPARAM lParam = 0;
		CString strwParam = ConvertUTFParam(argv[4]);

		CUIntArray auwParam;
		CUIntArray aulParam;
		BOOL bwUIntArray = FALSE;	// ���������־
		BOOL blUIntArray = FALSE;
		int  nwRef;					// �����������͵ı���
		int  nlRef;
		BOOL bwIRef	= FALSE;		// �������ͱ�־
		BOOL blIRef	= FALSE;

		if(strwParam.Find("<string>") == 0)	// �ַ�������
		{
			strwParam.Delete(0, 8);
			wParam = (WPARAM)(LPCTSTR)strwParam;
		}else
		if(strwParam.Find("<uintarray>") == 0)	// �����������(���Իش���Ϣ,�������Ϊ�б����)
		{
			strwParam.Delete(0, 11);
			// ��ȡ�б�����,��ӵ�����
			Tcl_Obj* listObj = Tcl_GetVar2Ex(interp, strwParam.GetBuffer(strwParam.GetLength()), NULL, TCL_LIST_ELEMENT);
			if(listObj != NULL)
			{
				int nListCount = 0;
				Tcl_Obj** paList;
				if(TCL_ERROR == Tcl_ListObjGetElements(interp, listObj, &nListCount, &paList))
				{
					Tcl_AppendResult(interp, "get list elements from wparam error.",  (char *) NULL);
					return TCL_ERROR;
				}
				for(int k=0; k<nListCount; k++)
				{
					Tcl_Obj* list = (paList[k]);
					auwParam.Add(atoi(ConvertUTFParam(list->bytes)));
				}
			}
			wParam = (WPARAM)(&auwParam);
			bwUIntArray = TRUE;
		}else
		if(strwParam.Find("<&int>") == 0)	// �������͵�����
		{
			strwParam.Delete(0, 6);
			CString szVar = Tcl_GetVar(interp, strwParam.GetBuffer(strwParam.GetLength()), 0);
			nwRef = atoi(szVar);
			wParam = (WPARAM)(&nwRef);
			bwIRef = TRUE;
		}else
		{
			wParam = atoi(ConvertUTFParam(argv[4]));
		}
		CString strlParam = ConvertUTFParam(argv[5]);
		if(strlParam.Find("<string>") == 0)
		{
			strlParam.Delete(0, 8);
			lParam = (WPARAM)(LPCTSTR)strlParam;
		}else
		if(strlParam.Find("<uintarray>") == 0)	// �����������(���Իش���Ϣ,�������Ϊ�б����)
		{
			strlParam.Delete(0, 11);
			// ��ȡ�б�����,��ӵ�����
			Tcl_Obj* listObj = Tcl_GetVar2Ex(interp, strlParam.GetBuffer(strlParam.GetLength()), NULL, TCL_LIST_ELEMENT);
			if(listObj != NULL)
			{
				int nListCount = 0;
				Tcl_Obj** paList;
				if(TCL_ERROR == Tcl_ListObjGetElements(interp, listObj, &nListCount, &paList))
				{
					Tcl_AppendResult(interp, "get list elements from lparam error.",  (char *) NULL);
					return TCL_ERROR;
				}
				for(int k=0; k<nListCount; k++)
				{
					Tcl_Obj* list = (paList[k]);
					aulParam.Add(atoi(ConvertUTFParam(list->bytes)));
				}
			}
			lParam = (LPARAM)(&aulParam);
			blUIntArray = TRUE;
		}else
		if(strlParam.Find("<&int>") == 0)	// �������͵�����
		{
			strlParam.Delete(0, 6);
			CString szVar = Tcl_GetVar(interp, strlParam.GetBuffer(strlParam.GetLength()), 0);
			nlRef = atoi(szVar);
			lParam = (WPARAM)(&nlRef);
			blIRef = TRUE;
		}else
		{
			lParam = atoi(ConvertUTFParam(argv[5]));
		}

		if(argc > 6)
		{
			strReturnType = ConvertUTFParam(argv[6]);
		}

		int nRet;
		g_pIPlatUI->SendOwmCmd(strOwmId, nCmdCode, wParam, lParam, &nRet);

		// ������CUIntArrayʱ��ķ����б��������
		if(bwUIntArray && (strwParam.GetLength() > 0))
		{
			CString strCmd;
			strCmd.Format("set %s [list];", strwParam);
			Tcl_Eval(interp, strCmd.GetBuffer(0));
			strCmd.ReleaseBuffer();
			for(int i=0; i<auwParam.GetSize(); i++)
			{
				strCmd.Format("lappend %s %d;", strwParam, auwParam[i]);
				Tcl_Eval(interp, strCmd.GetBuffer(0));
				strCmd.ReleaseBuffer();
			}
		}
		if(blUIntArray && (strlParam.GetLength() > 0))
		{
			CString strCmd;
			strCmd.Format("set %s [list];", strlParam);
			Tcl_Eval(interp, strCmd.GetBuffer(0));
			strCmd.ReleaseBuffer();
			for(int i=0; i<aulParam.GetSize(); i++)
			{
				strCmd.Format("lappend %s %d;", strlParam, aulParam[i]);
				Tcl_Eval(interp, strCmd.GetBuffer(0));
				strCmd.ReleaseBuffer();
			}
		}
		// ������������������ʱ�ķ���ֵ���õ�Tcl������
		if(bwIRef)
		{
			CString strTmp;
			strTmp.Format("%d", nwRef);
			Tcl_SetVar(interp, strwParam.GetBuffer(0), strTmp.GetBuffer(0), TCL_LEAVE_ERR_MSG);
			strwParam.ReleaseBuffer();
			strTmp.ReleaseBuffer();
		}
		if(blIRef)
		{
			CString strTmp;
			strTmp.Format("%d", nlRef);
			Tcl_SetVar(interp, strlParam.GetBuffer(0), strTmp.GetBuffer(0), TCL_LEAVE_ERR_MSG);
			strlParam.ReleaseBuffer();
			strTmp.ReleaseBuffer();
		}

		CString strRet;
		if(strReturnType == "<string>")
		{
			strRet = (LPCTSTR)nRet;
		}else
		{
			strRet.Format("%d", nRet);
		}
		Tcl_AppendResult(interp, strRet, (char *) NULL);
	}else
	{
		Tcl_AppendResult(interp, "bad argument \"", argv[1], "\" : must be showview, closeview or sendcmd", (char *) NULL);
		return TCL_ERROR;
	}

	ConvertResultToUTF(interp);
	return TCL_OK;
}

//TCL������������
int Tclvci_Init(Tcl_Interp *interp)
{
	// ע��VCI�������������
	// ��һ��VCI���
	Tcl_CreateObjCommand(interp, "vci_open", Tcl_Cmd_DeviceOpen, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// �ر�һ��VCI���
	Tcl_CreateObjCommand(interp, "vci_close", Tcl_Cmd_DeviceClose, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// VCI����ĺ�������
	Tcl_CreateObjCommand(interp, "vci_info", Tcl_Cmd_DeviceInfo, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ����VCI�������ĺ���
	Tcl_CreateObjCommand(interp, "vci_exec", Tcl_Cmd_DeviceExec, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// ��װ/ж��VCI�������Ļص�����
	Tcl_CreateObjCommand(interp, "vci_callback", Tcl_Cmd_DeviceCallBack, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	// OWM��չ����
	Tcl_CreateCommand(interp, "owm", Tcl_Cmd_OWM, 
			(ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	
    return TCL_OK;
}

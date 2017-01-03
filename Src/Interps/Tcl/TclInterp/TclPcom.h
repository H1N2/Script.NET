////////////////////////////////////////////////////////////////////////////
//	File:		TclPcom.h
//	Version:	1.0.0.0
//	Created:	2002-03-04
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of TclPcom.
////////////////////////////////////////////////////////////////////////////
#ifndef __TCLPCOM_H_
#define __TCLPCOM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ���峣�õ�����
enum{
	PTYPE_VOID,		// ��
	PTYPE_CHAR,		// �з��ŵ��ֽ�
	PTYPE_BYTE,		// �޷��ŵ��ֽ�
	PTYPE_SHORT,	// �з���˫�ֽ�
	PTYPE_WORD,		// �޷���˫�ֽ�
	PTYPE_INT,		// �з������ֽ�
	PTYPE_UINT,		// �޷������ֽ�
	PTYPE_CTIME,	// CTime����
	PTYPE_POINTER,	// ָ��
	PTYEP_FLOAT,	// ����
	PTYPE_STRING,	// �ַ���
	PTYPE_CSTRING,	// CString����
	PTYPE_CSTRINGARRAY,	// CStringArray����
	PTYPE_UINTARRAY,	// CUIntArray����
};

#define PTYPE_UNKNOWN	-1	// δ֪����

// ����������ݷ�ʽ
enum{
	PMODE_DIRECT,	// ֱ�Ӵ���
	PMODE_REFERENCE,// ����&
	PMODE_POINTER,	// ָ��*
	PMODE_POINTER2,	// ˫��ָ��**
	PMODE_CONST,	// ����const
};

// �����������ռ�
#define	TCLPCOM_MAXPARAM		15	// ����������
#define TCLPCOM_MAXPARAMSIZE	64	// ռ�ÿռ�


// ��װPCOM����
class CTclPcomParam
{
public:
	CTclPcomParam();
	~CTclPcomParam();

	// �������캯��
	CTclPcomParam(const CTclPcomParam& other);
	// ���ظ�ֵ�����
	CTclPcomParam& operator = (const CTclPcomParam& other);

	int	GetParamSize();
	int	GetParamType() {return m_nType;}
	BOOL NeedRefVar();
	void VarInit(CString strVar, CString strValue);
	void VarInitArray(CString strVar, CStringArray* pasValue);
	CString VarRelease();

// function
public:

// attribute
public:
	CString		m_strParamName;	// ������
	int			m_nType;		// ��������
	int			m_nMode;		// ������������
	CString		m_strDefault;	// �˲�����ȱʡֵ

	// ��������ִ��ʱ�����ʱ����,���ں���˵��û���ô�
	CString		m_strRefVar;	// ���������õ������¼�����TCL������
	int			m_nCommVar;		// ��������(��ʱ�����ּ򵥷���)
	char		m_chChar;
	BYTE		m_byByte;
	short		m_sShort;
	WORD		m_wWord;
	int			m_nInt;
	UINT		m_uiUint;
	CTime		m_tTime;
	CTime*		m_ptTime;
	LPVOID		m_lpVoid;
	float		m_fFloat;
	LPTSTR		m_lpTSTR;
	CString		m_strString;	// ��ͨ��ָ�����͵�CString
	CString*	m_psString;		// �������͵�CString
	CStringArray	m_asString;	// �������͵�CStringArray
	CStringArray*	m_pasString;	// ָ�����͵�CStringArray
	CUIntArray	m_auUInt;		// �������͵�CUIntArray
	CUIntArray*	m_pauUInt;		// ָ�����͵�CUIntArray
};

typedef CArray<CTclPcomParam, CTclPcomParam&> CTclPcomParamArray;


// ��װPCOM���еĺ���
class CTclPcomFunc
{
public:
	CTclPcomFunc();
	~CTclPcomFunc();

	// �������캯��
	CTclPcomFunc(CTclPcomFunc& other);
	// ���ظ�ֵ�����
	CTclPcomFunc& operator = (CTclPcomFunc& other);

// function
public:
	int	ParseParamType(CString strParam, CString& strParamName, int& nType, int& nMode, CString& strDefault);
	int	CalcTotalParamSize();
	int	AddParam(CString strParamName, int nType, int nMode, CString strDefault);
	CTclPcomParam* FindParam(CString strParam);

	void SetFuncName(CString strName) { m_strFuncName = strName; }
	CString	GetFuncName() { return m_strFuncName; }
	void SetFuncDef(CString strDef) { m_strFuncDef = strDef; }
	CString	GetFuncDef() { return m_strFuncDef; }
	BOOL EnableExec() { return m_bEnableExec; }

// attribute
public:
	CTclPcomParamArray	m_arParams;		// �����б�
	CTclPcomParam		m_Ret;			// ����ֵ
	CString				m_strFuncName;	// ������
	CString				m_strFuncDef;	// ����ԭ�Ͷ���
	int					m_nLeastParams;	// ȥ��ȱʡ����������ٲ�������
	BOOL				m_bEnableExec;	// �˺����Ƿ����ȷִ��(��δ֪���Ͳ����򲻿�ִ��)
};

typedef CArray<CTclPcomFunc, CTclPcomFunc&> CTclPcomFuncArray;


// ��װPCOM��
class CTclPcomClass
{
public:
	CTclPcomClass();
	~CTclPcomClass();

// function
public:
	DWORD GetRefCount() { return m_dwRefCount; }
	DWORD AddRef() { return ++m_dwRefCount; }
	DWORD ReleaseRef() { return --m_dwRefCount; }
	CString	GetVciName() { return m_strVciName; }
	void  SetVciName(CString strVciName) { m_strVciName = strVciName; }

	BOOL LoadHeadFile();
	CTclPcomFunc* FindFunction(CString strFunc);
	int  GetFunctionIndex(CString strFunc);

// attribute
public:
	DWORD			m_dwRefCount;	// ���ü���,��ʾ��ǰ������PcomObjʹ��
	CString			m_strVciName;	// �������
	CTclPcomFuncArray	m_arFuncs;	// �����б�
};

typedef CArray<CTclPcomClass, CTclPcomClass&> CTclPcomClassArray;


class CTclPcomObj;
// ����ص�������Ϣ��
class CTclPcomCallBack
{
public:
	CTclPcomCallBack();
	~CTclPcomCallBack();

// function
public:
	static CTclPcomCallBack* InstallCallBack(LPVOID pInterp, CString strScript, int nParamCount, CTclPcomObj* pObj, int nEventID);
	static BOOL UnInstallCallBack(CTclPcomObj* pObj, int nEventID);
	static CTclPcomCallBack* GetCallBack(CTclPcomObj* pObj, int nEventID);

// attribute
public:
	LPVOID		m_pInterp;			// ������ָ��
	CString		m_strCallBackScript;// �ص��Ľű�������
	int			m_nParamCount;		// �ص������Ĳ�������
	LPVOID		m_pCallBackFunc;	// �ص�����ָ��(Ŀǰֻ��֧��һ���ص�����)
	CTclPcomObj* m_pPcomObject;		// �����Ӧ��ʵ������ָ��
	int			m_nEventID;			// �ص������¼�ID
};

typedef CArray<CTclPcomCallBack, CTclPcomCallBack&> CTclPcomCallBackArray;


// ��װPCOM����
class CTclPcomObj
{
public:
	CTclPcomObj();
	virtual ~CTclPcomObj();

// function
public:
	void* GetPcomObj() { return m_pPcomObj; }
	CTclPcomClass* GetTclPcomClass() { return m_pClass; }
	CString GetErrorInfo() { return m_strErrorInfo; }

	BOOL IsOpen(CString strVciName, CString strVer, CString strShareName);
	BOOL OpenVciObject(CString strVciName, CString strVer, CString strShareName, LPVOID pPcomObj = NULL);
	static void CloseVciObject(CTclPcomObj* pPcomObj);
	void ReleaseVciObject(BOOL bForce);
//	BOOL CallPcomFunction(CString strFuncName, CStringArray& arParams);
	void SetInterp(Tcl_Interp* interp) {m_Interp = interp;}
	Tcl_Interp* GetInterp() {return m_Interp;}

// attribute
private:
	CTclPcomClass*	m_pClass;		// ��Ӧ�������
	void*			m_pPcomObj;		// ����ָ��
	Tcl_Interp*		m_Interp;		// ������ָ��
	CString			m_strVciName;	// �������
	CString			m_strShareName;	// ���������
	CString			m_strErrorInfo;	// ������Ϣ
	int				m_nRefCount;	// ���ü���
};

typedef CArray<CTclPcomObj, CTclPcomObj&> CTclPcomObjArray;


void CloseAllVciObject();

int Tclvci_Init(Tcl_Interp *interp);

#endif	// __TCLPCOM_H_
////////////////////////////////////////////////////////////////////////////
//	File:		CommLogin.h
//	Version:	1.0.0.0
//	Created:	2002-07-04
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of platform comm login dialog.
////////////////////////////////////////////////////////////////////////////
#ifndef __PLAT_COMMLOGIN_H_
#define __PLAT_COMMLOGIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ͨ�õ�¼��Ϣ�ṹ
struct TCommLoginInfo
{
	CString	strAddress;		// ��¼��ַ
	CString	strUserName;	// �û���
	CString	strPassword;	// ����
	CString	strDomain;		// ��¼��
	CString	strDomainList;	// ��¼���б�
	CString	strTitle;		// ����
	CString	strInfo;		// ��ʾ��Ϣ
	CString	strLAddress;	// ��ַ�����ǩ
	CString	strLUserName;	// �û������ǩ
	CString	strLPassword;	// ���������ǩ
	CString	strLDomain;		// �������ǩ
	CString	strCheck;		// ��������
	BOOL	bCheck;			// ����ֵ
	BOOL	bUseAddress;	// ʹ�õ�ַ�ֶ�
	BOOL	bUseUserName;	// ʹ���û����ֶ�
	BOOL	bUsePassword;	// ʹ�ÿ����ֶ�
	BOOL	bUseDomain;		// ʹ�õ�¼���ֶ�
	CString	varAddress;		// ��¼��ַTCL����
	CString	varUserName;	// �û���TCL����
	CString	varPassword;	// ����TCL����
	CString	varDomain;		// ��¼��TCL����
	CString	varCheck;		// ����TCL����
	CString	strFocus;		// ���뽹��

public:
	TCommLoginInfo()
	{
		strAddress		= "";
		strUserName		= "";
		strPassword		= "";
		strDomain		= "";
		strDomainList	= "";
		strTitle		= "";
		strInfo			= "";
		strLAddress		= "";
		strLUserName	= "";
		strLPassword	= "";
		strLDomain		= "";
		strCheck		= "";
		bCheck			= FALSE;
		bUseAddress		= FALSE;
		bUseUserName	= FALSE;
		bUsePassword	= FALSE;
		bUseDomain		= FALSE;
		varAddress		= "";
		varUserName		= "";
		varPassword		= "";
		varDomain		= "";
		varCheck		= "";
		strFocus		= "";
	};
};


#endif // !defined(__PLAT_COMMLOGIN_H_)
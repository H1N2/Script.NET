////////////////////////////////////////////////////////////////////////////
//	File:		OptTree.h
//	Version:	1.0.0.0
//	Created:	2002-07-04
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of platform optiontree dialog.
////////////////////////////////////////////////////////////////////////////
#ifndef __PLAT_OPTTREE_H_
#define __PLAT_OPTTREE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CTreeOptionsDlg dialog
typedef struct _TOPTITEM
{
	HTREEITEM	hItem;		// ���ڵ���(�ڲ�ʹ��)
	INT			iType;		// �ڵ�����
	CString		lpszText;	// ��Ϣ
	CString		lpszEdit;	// �ɱ༭�ַ���
	CString		lpszList;	// Combo�б�
	BOOL		bCheck;		// �����
	INT			iImage;		// ͼ��
	INT			iParent;	// ���ڵ�����
	CString		strTclVar;	// TCL�б���������
public:
	_TOPTITEM& operator = (const _TOPTITEM& other)
	{
		hItem	= other.hItem;
		iType	= other.iType;
		lpszText= other.lpszText;
		lpszEdit= other.lpszEdit;
		lpszList= other.lpszList;
		bCheck	= other.bCheck;
		iImage	= other.iImage;
		iParent	= other.iParent;
		strTclVar=other.strTclVar;
		return *this;
	};
} TOPTITEM, FAR* LPTOPTITEM;

typedef CArray<TOPTITEM, TOPTITEM&> CTOptArray;

// ��������������
enum {
	TOPT_MSG,			// ���ñ������Ϣ
	TOPT_HELP,			// ���ð����ļ�
	TOPT_ITEM,			// �����
	TOPT_GROUP,			// �����
	TOPT_CHECKBOX,		// ��Ӽ���
	TOPT_RADIOBUTTON,	// ��ӹ㲥��ť
	TOPT_EDIT,			// ��ӱ༭��
	TOPT_COMBO,			// �����Ͽ�
};

// ����������ͼ��
enum {	
	TOPTI_FOLDER,		// �ļ���
	TOPTI_GROUP,		// ��
	TOPTI_EDIT,			// �༭��
	TOPTI_COMBO,		// ��Ͽ�
	TOPTI_M_FOLDER,		// �ļ���(������)
	TOPTI_M_GROUP,		// ��(������)
	TOPTI_M_EDIT,		// �༭��(������)
	TOPTI_M_COMBO,		// ��Ͽ�(������)
	TOPTI_INFO,			// ��Ϣ
	TOPTI_PROJECT,		// ����
	TOPTI_FILE,			// �ļ�
	TOPTI_VERSION,		// �汾
	TOPTI_INTERFACE,	// �ӿ�
	TOPTI_ITEM,			// ��Ŀ
	TOPTI_HELP,			// ����
	TOPTI_BOARD,		// ����
	TOPTI_WORNING,		// �澯
	TOPTI_CONNECT,		// ��������
	TOPTI_USER,			// �û�
};


#endif // !defined(__PLAT_OPTTREE_H_)
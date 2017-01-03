////////////////////////////////////////////////////////////////////////////
//	File:		WorkTree.h
//	Version:	1.0.0.0
//	Created:	2000-07-01
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of Work Tree.
////////////////////////////////////////////////////////////////////////////
#ifndef __WORKTREE_H_
#define __WORKTREE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
// WorkTree define

// ����WorkSpace����
#define OC_WS_ADDTREE			200	// ���������
#define OC_WS_ADDTREEEX			201	// ���������(��չ��ʽ,��������ͼ���)
#define OC_WS_CLOSETREE			202	// �ر�������
#define OC_WS_GETTREEINDEX		203	// �������ֵõ�������
#define OC_WS_SETACTIVETREE		204	// ���û��
#define OC_WS_ACTIVECB			205	// ��װ���ڼ���Ļص�����
#define OC_WS_TREEEXIST			206	// �ж�ָ���������Ƿ��Ѿ�����
#define OC_WS_SETTREETYPE		207	// ���ô�������
#define OC_WS_SETTREEIMAGE		208	// ��������ͼ����
#define OC_WS_ADDMENU			210	// ��Ӳ˵�
#define OC_WS_ADDMENUITEM		211	// ��Ӳ˵���
#define OC_WS_ADDTREEITEM		212	// ��ӽڵ�
#define OC_WS_DELTREEITEM		213	// ɾ���ڵ�
#define OC_WS_SETITEMTYPE		214	// ���ýڵ�����
#define OC_WS_GETITEMTYPE		215	// ȡ�ýڵ�����
#define OC_WS_SETITEMSTATE		216	// ���ýڵ�״̬
#define OC_WS_GETITEMSTATE		217	// ȡ�ýڵ�״̬
#define OC_WS_SETITEMMENU		218	// ���ýڵ�˵�
#define OC_WS_GETITEMMENU		219	// ȡ�ýڵ�˵�
#define OC_WS_SETITEMOBJECT		220	// ���ýڵ����
#define OC_WS_GETITEMOBJECT		221	// ȡ�ýڵ����
#define OC_WS_SETITEMID			222	// ���ýڵ�ID
#define OC_WS_GETITEMID			223	// ȡ�ýڵ�ID
#define OC_WS_SETITEMNAME		224	// ���ýڵ���
#define OC_WS_GETITEMNAME		225	// ȡ�ýڵ���
#define OC_WS_SETITEMIMAGE		226	// ���ýڵ�ͼ��
#define OC_WS_GETITEMIMAGE		227	// ȡ�ýڵ�ͼ��
#define OC_WS_EXPANDITEM		228	// չ���ڵ�
#define OC_WS_COLLAPSEITEM		229	// �����ڵ�
#define OC_WS_SETDCLICKSCRIPT	230	// ���ýڵ�˫���Ľű�
#define OC_WS_SETITEMINFO		231	// ���ýڵ���Ϣ
#define OC_WS_GETITEMINFO		232	// ��ȡ�ڵ���Ϣ
#define	OC_WS_SEARCHITEM		233	// ���ҽڵ�
#define OC_WS_GETCHILDITEM		234	// ��ȡ��һ���ӽڵ�
#define OC_WS_GETNEXTITEM		235	// ��ȡ��һ���ڵ�
#define OC_WS_SORTCHILDREN		236	// ���ӽڵ�����

#define OC_WS_SETTASKTHEME		240	// ����TaskPanel��Theme
#define OC_WS_ADDTASKITEM		241	// ���һ��TaskItem
#define OC_WS_DELTASKITEM		242	// ɾ��ָ��TaskItems
#define OC_WS_SETTASKICON		243	// ����TaskPanel��ͼ���ͼ����
#define OC_WS_CREATTASKMENU		244	// ��ָ���˵�����TaskPanel

#define OC_WS_SETPAGEMENU		250	// ����ҳ��˵�


// ������ڵ�ID
#define UNITID_ALL				-1

typedef void (TYPEOF_ViewChange)();	// �л������ڻص�

// ����ȱʡ�ļ���ҳ��ID
#define TREE_ID_NAVIGATER	10005	// ������
#define TREE_ID_FILE		10006	// �ļ���

// ����ҳ��ͼ��(����)
enum{
	PGI_FILE,		// �ļ�����ҳ��
	PGI_TEST,		// ������ҳ��
	PGI_CUSTOM,		// �Զ���ҳ��
	PGI_TASKPANEL,	// TaskPanelҳ��
};

// �ڵ�״̬(���Խڵ�)
enum{
	WTS_NORMAL,		// ����
	WTS_TESTING,	// ����
	WTS_SUCC,		// ͨ��
	WTS_FAIL,		// ʧ��
	WTS_DISABLE,	// ��ֹ
};

// �ڵ�ͼ��(�ļ��ڵ�)
enum{
	WTI_FILE_ALL,			// ���й���
	WTI_FILE_PRJ,			// ����
	WTI_FILE_SOURCE,		// Դ�ļ�
	WTI_FOLDER_OPEN,		// �򿪵İ�
	WTI_FOLDER,				// ��
	WTI_RES_ACCEL,			// ��ݼ�
	WTI_RES_DLG,			// �Ի���
	WTI_RES_ICON,			// ͼ��
	WTI_RES_MENU,			// �˵�
	WTI_RES_STRING,			// �ַ���
	WTI_RES_TOOLBAR,		// ������
	WTI_RES_VERSION,		// �汾
	WTI_CLASS,				// ��
	WTI_CLASS_PRIVFUNC,		// ˽�з���
	WTI_CLASS_FUNC,			// ����
	WTI_CLASS_PRIVVAR,		// ˽�б���
	WTI_CLASS_VAR,			// ����
	WTI_RESOURCE,			// ��Դ
	WTI_DOCUMENT,			// �ĵ�
	WTI_DEVICE,				// ����
	WTI_RES_MODULE,			// ģ��
	WTI_TCL_LIB,			// TCL���ؿ�
	WTI_TCL_INTERP,			// TCL������
	WTI_RES_ITEM,			// Ԫ��
	WTI_DOC_HELP,			// �����ĵ�
	WTI_RES_BOARD,			// ����
	WTI_RES_SYSTEM,			// ����
	WTI_RES_PROJECT,		// ����
	WTI_RES_INFO,			// ��Ϣ
	WTI_DATABASE,			// ���ݿ�
	WTI_DB_LINK,			// �����ӵ����ݿ�
	WTI_NET_DISCONNECT,		// ����Ͽ�
	WTI_NET_CONNECT,		// ��������
	WTI_USER,				// �û�
	WTI_USER_LOGIN,			// �û��ѵ�¼
	WTI_SELF_CHECKOUT,		// �Լ�checkout���ļ�
	WTI_OTHER_CHECKOUT,		// �����û�checkout���ļ�
	WTI_THIS,				// this
	WTI_PACKAGE_MULTI,		// �����
	WTI_PACKAGE_SINGLE,		// ������
};

#define WTT_NULL	255		// ������

// ���ڵ�
struct HWORKTREEITEM
{
	DWORD		dwMask;		// ����
	HTREEITEM	hItem;		// �ڵ���
	HTREEITEM	hParent;	// ���ڵ���
	int			nParentID;	// ���ڵ�ID
	int			nID;		// �ڵ�ID
	CString		lpszName;	// �ڵ���
	BYTE		nImage;		// �ڵ�ͼ��
	BYTE		byType;		// �ڵ�����
	BYTE		byState;	// �ڵ�״̬
	CMenu*		pMenu;		// �ڵ�˵�
	CObject*	pObject;	// �ڵ����
	CString		strInfo;	// �ڵ���Ϣ
	CString		strDClick;	// �ڵ�˫���ű�
	COLORREF	crColor;	// �ڵ���ɫ
	LOGFONT		logFont;	// �ڵ�����
};

// ��������
#define TIM_ITEM		0x00000001	// �ڵ���
#define TIM_PARENT		0x00000002	// ���ڵ���
#define TIM_PARENTID	0x00000004	// ���ڵ���
#define TIM_ID			0x00000008	// �ڵ�ID
#define TIM_NAME		0x00000010	// �ڵ���
#define TIM_IMAGE		0x00000020	// �ڵ�ͼ��
#define TIM_TYPE		0x00000040	// �ڵ�����
#define TIM_STATE		0x00000080	// �ڵ�״̬
#define TIM_MENU		0x00000100	// �ڵ�˵�
#define TIM_OBJECT		0x00000200	// �ڵ����
#define	TIM_INFO		0x00000400	// �ڵ���Ϣ��
#define TIM_DCLICK		0x00000800	// �ڵ�˫���ű�
#define TIM_COLOR		0x01000000	// �ڵ���ɫ
#define TIM_BOLD		0x02000000	// �ڵ��Ƿ����
#define TIM_FONT		0x04000000	// �ڵ�����
#define TIM_SPECIAL		0x08000000	// ����ڵ����
#define TIM_TRAVERSAL	0x00001000	// �Ƿ�ݹ����
#define TIM_UPTRAVERSAL	0x00002000	// ���ϱ���,�������ø��ڵ�״̬
#define TIM_FRESHALL	0x00004000	// �Ƿ�λ���нڵ�(FALSE=����λ��ֹ�ڵ�)
#define TIM_CHILDONLY	0x00008000	// �����ӽڵ����


// ����˵�����
typedef void (TYPEOF_Proc)(CObject* pThis);
typedef void (TYPEOF_UpdateProc)(CCmdUI* pCmdUI, CObject* pThis);

// �˵���ID�봦�����ṹ
struct HWORKMENUITEM
{
	BYTE				byType;		// �ڵ�����(��HWORKTREEDATA��Ӧ)
	UINT				nID;		// �˵���ID
	CString				strItem;	// �˵���ʾ�ַ���
	CString				strCommand;	// �˵��ű�
	TYPEOF_Proc			*Proc;		// �˵�����
	TYPEOF_UpdateProc	*UpdateProc;// �˵�ˢ�º���
};

typedef CArray<HWORKMENUITEM, HWORKMENUITEM> CMenuItemArray;

/////////////////////////////////////////////////////////////////
// TaskPanel define

// TaskPanel Item����
enum{
	WTT_GROUP,
	WTT_TEXT,
	WTT_LINK,
	WTT_MASHUP,
};

// TaskPanel��
struct HWORKTASKITEM
{
	DWORD		dwMask;		// ����
	int			nType;		// ����(��/����/����/Mashup)
	int			nID;		// ID
	int			nImage;		// ͼ��
	int			nParent;	// ���ڵ�ID
	CString		strCaption;	// ����
	CString		strTooltip;	// Tooltip
	CString		strInfo;	// ��Ϣ
	CString		strDClick;	// ˫���ű�
	LPVOID		lpItem;		// PanelItem����
	COLORREF	crColor;	// ��ɫ
	LOGFONT		logFont;	// ����
};

typedef CArray<HWORKTASKITEM, HWORKTASKITEM> CTaskPanelItemArray;


// ������������ṹ
struct STRU_ADDTREEEX
{
	CString	strTreeName;	// ������
	CString	strIconFile;	// ͼ���ļ�
	int		nTreeType;		// ����
	int		nAttachPaneId;	// �������ĸ����ں���
	CString	strBmpFile;		// ͼƬ�ļ���
	int		nPaneId;		// ָ���Ĵ���PandeId
};

#endif // !defined(__WORKTREE_H_)
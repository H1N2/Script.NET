// WorkTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WorkSpace.h"
#include "WorkTreeCtrl.h"
#include "WorkSpaceView.h"
//#include "WorkSpaceBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDM_OWMCMD   40000

/////////////////////////////////////////////////////////////////////////////
// CWorkTreeCtrl

CWorkTreeCtrl::CWorkTreeCtrl()
{
	m_nImageCount = 0;
	m_nTreeType = 0;
	m_nTreeID = 0;
	m_nMenuItemID = IDM_OWMCMD;
	m_strBmpFile = "";
	m_strDClickScript = "";
	m_aDatas.SetSize(0, 40);
}

CWorkTreeCtrl::~CWorkTreeCtrl()
{
	for(int i=0; i<m_aMenus.GetSize(); i++)
	{
		CMenu* pMenu = (CMenu*)(m_aMenus[i]);
		delete pMenu;
	}
	m_aMenus.RemoveAll();
	for(int i=0; i<m_aDatas.GetSize(); i++)
	{
		HWORKTREEDATA* pData = (HWORKTREEDATA*)(m_aDatas[i]);
		delete pData;
	}
	m_aDatas.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// �����б��Ƿ��ʼ��
/////////////////////////////////////////////////////////////////////////////
BOOL CWorkTreeCtrl::IsWsInit()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���������б��ʼ����־
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetWsInit(BOOL bInit)
{
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::InitTree()
{
	get_dll_resource();

	// ��ʼ�����ڵ�ͼ��
	if((m_strBmpFile == "") && (GetFileAttributes(m_strBmpFile) == 0xFFFFFFFF))
	{
		CBitmap bitmap1, bitmap2, bitmap3, bitmap4;
		if(PGI_TEST == m_nTreeType)
		{
			m_ImageList.Create(IDB_TREE_NORMAL, 16, 1, RGB( 0, 128, 128));
			bitmap1.LoadBitmap(IDB_TREE_TESTING);
			bitmap2.LoadBitmap(IDB_TREE_SUCC);
			bitmap3.LoadBitmap(IDB_TREE_FAIL);
			bitmap4.LoadBitmap(IDB_TREE_DISABLE);
			m_ImageList.Add(&bitmap1, RGB( 0, 128, 128));
			m_ImageList.Add(&bitmap2, RGB( 0, 128, 128));
			m_ImageList.Add(&bitmap3, RGB( 0, 128, 128));
			m_ImageList.Add(&bitmap4, RGB( 0, 128, 128));
			m_nImageCount = 6;
		}else
		{
			m_ImageList.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1);

			CBitmap bmp;
			VERIFY(bmp.LoadBitmap(IDB_IL_FILE_EX));
			m_ImageList.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));
			m_nImageCount = 57;
		}
	}else
	{
		// ʹ���Զ���ͼƬ
		SetTreeImageList(m_strBmpFile);
		/*
		CBitmap bmp;

		HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetApp()->m_hInstance,
			m_strBmpFile,
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		if(hBitmap != NULL)
		{
			// ����ͼƬ,������16*16/ͼ��
			BITMAP bmpInfo;
			::GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

			bmp.Attach(hBitmap);

			UINT nFlag = ILC_COLOR4;
			switch(bmpInfo.bmBitsPixel)
			{
			case 4:nFlag = ILC_COLOR4;break;
			case 8:nFlag = ILC_COLOR8;break;
			case 16:nFlag = ILC_COLOR16;break;
			case 24:nFlag = ILC_COLOR24;break;
			case 32:nFlag = ILC_COLOR32;break;
			}
			m_ImageList.Create(16, 16, ILC_MASK|nFlag, 0, 1);
			m_ImageList.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));

			// ͼ������Ϊ��ȳ��߶�
			m_nImageCount = bmpInfo.bmWidth / bmpInfo.bmHeight;
		}*/
	}

	SetImageList(&m_ImageList, TVSIL_NORMAL);

	// ��ʼ�������˵�
	if(PGI_TEST == m_nTreeType)
	{
		CMenu* pMenu = new CMenu();
		pMenu->LoadMenu(IDR_TESTTREE);
		AddTreeMenu(pMenu);
	}else
	{
		CMenu* pMenu = new CMenu();
		pMenu->LoadMenu(IDR_MENU_BASE);
		AddTreeMenu(pMenu);
	}

	reset_dll_resource();

	// �����ֹ��ѡ,�������˫������չ���ڵ�
	EnableMultiSelect(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// ��������ͼ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetTreeImageList(CString strBmpFile)
{
	if(GetFileAttributes(strBmpFile) != 0xFFFFFFFF)
	{
		m_strBmpFile = strBmpFile;

		m_ImageList.DeleteImageList();

		// ʹ���Զ���ͼƬ
		CBitmap bmp;

		HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetApp()->m_hInstance,
			m_strBmpFile,
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		if(hBitmap != NULL)
		{
			// ����ͼƬ,������16*16/ͼ��
			BITMAP bmpInfo;
			::GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

			bmp.Attach(hBitmap);

			UINT nFlag = ILC_COLOR4;
			switch(bmpInfo.bmBitsPixel)
			{
			case 4:nFlag = ILC_COLOR4;break;
			case 8:nFlag = ILC_COLOR8;break;
			case 16:nFlag = ILC_COLOR16;break;
			case 24:nFlag = ILC_COLOR24;break;
			case 32:nFlag = ILC_COLOR32;break;
			}
			m_ImageList.Create(16, 16, ILC_MASK|nFlag, 0, 1);
			m_ImageList.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));

			// ͼ������Ϊ��ȳ��߶�
			m_nImageCount = bmpInfo.bmWidth / bmpInfo.bmHeight;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӳ˵�
/////////////////////////////////////////////////////////////////////////////
int CWorkTreeCtrl::AddTreeMenu(CMenu* pMenu)
{
	return m_aMenus.Add(pMenu);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ò˵�
/////////////////////////////////////////////////////////////////////////////
CMenu* CWorkTreeCtrl::GetTreeMenu(int nIndex)
{
	if(nIndex >= m_aMenus.GetSize())
		return NULL;
	else
		return (CMenu*)(m_aMenus[nIndex]);
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӳ˵���
/////////////////////////////////////////////////////////////////////////////
int CWorkTreeCtrl::AddMenuItem(BYTE byType, UINT nID, CString strItem, CString strCommand, TYPEOF_Proc* Proc, TYPEOF_UpdateProc* UpdateProc)
{
	for(int i=0; i<m_aMenuItems.GetSize(); i++)
	{
		if((byType == m_aMenuItems[i].byType) &&
			((nID == m_aMenuItems[i].nID) || (strItem == m_aMenuItems[i].strItem)) )
		{
			return m_aMenuItems.GetSize();
		}
	}

	if(m_nMenuItemID > (IDM_OWMCMD + 1000))
	{
		m_nMenuItemID = IDM_OWMCMD;
	}

	HWORKMENUITEM item;
	item.byType		= byType;
	item.nID		= (nID == 0) ? m_nMenuItemID++ : nID;
	item.strItem	= strItem;
	item.strCommand	= strCommand;
	item.Proc		= Proc;
	item.UpdateProc	= UpdateProc;
	return m_aMenuItems.Add(item);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ò˵������
/////////////////////////////////////////////////////////////////////////////
BOOL CWorkTreeCtrl::GetMenuItem(BYTE byType, UINT nID, CString& strItem, CString& strCommand, TYPEOF_Proc** pProc, TYPEOF_UpdateProc** pUpdateProc)
{
	for(int i=0; i<m_aMenuItems.GetSize(); i++)
	{
		if((byType == m_aMenuItems[i].byType) &&
			(nID == m_aMenuItems[i].nID))
		{
			strItem = m_aMenuItems[i].strItem;
			strCommand = m_aMenuItems[i].strCommand;
			*pProc = m_aMenuItems[i].Proc;
			*pUpdateProc = m_aMenuItems[i].UpdateProc;
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ӽڵ�
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CWorkTreeCtrl::AddTreeItem(HTREEITEM hParent, CString strText,
				    int iImage, HTREEITEM hInsAfter)
{
	HTREEITEM hItem;       // return value
	TV_ITEM tvI;           // item structure
	TV_INSERTSTRUCT tvIns; // item insert structure

	DWORD dwStyle = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	
	tvI.mask			= dwStyle;
	tvI.pszText			= strText.GetBuffer(1);
	tvI.cchTextMax		= strText.GetLength();
	tvI.iImage			= iImage;
	tvI.iSelectedImage	= iImage;

	// allow one more level down
	tvI.cChildren = 1;
	
	// do the insert
	tvIns.item			= tvI;
	tvIns.hInsertAfter	= hInsAfter;
	tvIns.hParent		= hParent;
	hItem				= InsertItem(&tvIns);

	HWORKTREEDATA* pData= new HWORKTREEDATA;
	pData->byType		= WTT_NULL;
	pData->byState		= WTS_NORMAL;
	pData->pMenu		= GetTreeMenu(0);
	pData->pObject		= NULL;
	pData->hItem		= hItem;
	pData->strInfo		= "";
	SetItemData(hItem, (DWORD)pData);
	m_aDatas.Add(pData);

//	SetItemColor(hItem, RGB(0,0,255));
//	SetItemBold(hItem, TRUE);

	return( hItem ); // return (add error trap here)
}

/////////////////////////////////////////////////////////////////////////////
// ɾ���ڵ�
// ע������bChildOnly���ΪTRUE,��ʾֻɾ�������ӽڵ�
/////////////////////////////////////////////////////////////////////////////
BOOL CWorkTreeCtrl::DelTreeItem(HTREEITEM hItem, BOOL bChildOnly)
{
	if(hItem == NULL)
	{
		return FALSE;
	}

	if(!ItemHasChildren(hItem))
	{
		if(bChildOnly)
			return TRUE;

		DelTreeItemData(hItem);
		DeleteItem(hItem);
		return TRUE;
	}

	HTREEITEM hChildItem = NULL;
	while((hChildItem = GetChildItem(hItem)) != NULL)
	{
		DelTreeItem(hChildItem, FALSE);
	}

	if(bChildOnly)
	{
		return TRUE;
	}
/*
	HTREEITEM hChildItem = GetChildItem(hItem);
	if(hChildItem != NULL)
	{
		DelTreeItem(hChildItem);

		HTREEITEM hNextItem;
		while((hNextItem = GetNextSiblingItem(hChildItem)) != NULL)
		{
			DelTreeItem(hNextItem);
			hChildItem = hNextItem;
		}
	}
*/
	DelTreeItemData(hItem);
	DeleteItem(hItem);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ɾ���ڵ�����
/////////////////////////////////////////////////////////////////////////////
BOOL CWorkTreeCtrl::DelTreeItemData(HTREEITEM hItem)
{
	HWORKTREEDATA* pData= (HWORKTREEDATA*)GetItemData(hItem);
	if(pData == NULL)
	{
		return FALSE;
	}

	for(int i=0; i<m_aDatas.GetSize(); i++)
	{
		if(pData == m_aDatas[i])
		{
			delete pData;
			m_aDatas.RemoveAt(i);
			return TRUE;
		}
	}

	delete pData;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�˵�
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemMenu(HTREEITEM hItem, CMenu* pMenu)
{
	//if(pMenu == NULL)
	//	pMenu = GetTreeMenu(0);

	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->pMenu	= pMenu;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->pMenu = pMenu;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�˵�
/////////////////////////////////////////////////////////////////////////////
CMenu* CWorkTreeCtrl::GetItemMenu(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return NULL;
	return pData->pMenu;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemType(HTREEITEM hItem, BYTE byType)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->byType	= byType;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->byType = byType;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�����
/////////////////////////////////////////////////////////////////////////////
BYTE CWorkTreeCtrl::GetItemType(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return NULL;
	return pData->byType;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�״̬
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemState1(HTREEITEM hItem, BYTE byState)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		if(byState == pData->byState)
			return;
		pData->byState	= byState;
		SetItemData(hItem, (DWORD)pData);
		int nImage, nSelImage;
		GetItemImage(hItem, nImage, nSelImage);
		int nIndex = 0;
		if(m_nImageCount > 0)
			nIndex = nImage % m_nImageCount;
		nImage = m_nImageCount*byState+nIndex;
		SetItemImage(hItem, nImage, nImage);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->byState = byState;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�״̬(���µݹ����)
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::TraversalSetItemState(HTREEITEM hItem, BYTE byState, BOOL bFreshAll)
{
	if(hItem == NULL)
	{
		return;
	}

	if(!ItemHasChildren(hItem))
	{
		// ��������������ýڵ�״̬
		// 1.��TIM_FRESHALL����,��ʾǿ�ƻָ�����״̬
		// 2.��ǰ�ڵ�״̬���ǽ�ֹ״̬(����ǽ�ֹ״̬,һ������Ҫ������)
		if(bFreshAll || (GetItemState1(hItem) != WTS_DISABLE))
		{
			SetItemState1(hItem, byState);
		}
		return;
	}

	HTREEITEM hChildItem = GetChildItem(hItem);
	if(hChildItem != NULL)
	{
		TraversalSetItemState(hChildItem, byState, bFreshAll);

		HTREEITEM hNextItem;
		while((hNextItem = GetNextSiblingItem(hChildItem)) != NULL)
		{
			TraversalSetItemState(hNextItem, byState, bFreshAll);
			hChildItem = hNextItem;
		}
	}

	SetItemState1(hItem, byState);
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�״̬(���ϱ�����ʽ)
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::UpTraversalSetItemState(HTREEITEM hItem, BYTE byState)
{
	if(hItem == NULL)
	{
		return;
	}

	SetItemState1(hItem, byState);

	if(PGI_TEST != m_nTreeType)
	{	// ֻ�в���ҳ��������ϱ�������״̬
		return;
	}

	HTREEITEM hParentItem = GetParentItem(hItem);
	if(hParentItem == NULL)
	{
		return;
	}

	if(WTS_TESTING == byState)
	{	// ��������ڲ���,��ֱ�����ø��ڵ�Ϊ���ڲ���
		UpTraversalSetItemState(hParentItem, WTS_TESTING);
		return;
	}

	// ��ʼ��״̬�ڵ������,�ֱ��ʾ5�ֽڵ�ĸ���
	int aCount[5];
	for(int i=0; i<5; i++)
	{
		aCount[i] = 0;
	}

	int nCount = 0;	// �ܽڵ����
	HTREEITEM hChildItem = GetChildItem(hParentItem);
	if(hChildItem != NULL)
	{
		BYTE byState1 = GetItemState1(hChildItem);
		if((byState1>=0) && (byState1<5))
		{
			aCount[byState1]++;
			nCount++;
		}

		HTREEITEM hNextItem;
		while((hNextItem = GetNextSiblingItem(hChildItem)) != NULL)
		{
			byState1 = GetItemState1(hNextItem);
			if((byState1>=0) && (byState1<5))
			{
				aCount[byState1]++;
				nCount++;
			}
			hChildItem = hNextItem;
		}

		for(int i=0; i<5; i++)
		{
			if(nCount == aCount[i])	
			{	// �������ĳ��״̬,�򸸽ڵ���Ϊ����״̬
				UpTraversalSetItemState(hParentItem, i);
				return;
			}
		}

		nCount -= aCount[WTS_DISABLE];	// ȥ����ֹ��

		for(int i=0; i<5; i++)
		{
			if(nCount == aCount[i])	
			{	// ʣ����������ĳ��״̬,�򸸽ڵ���Ϊ����״̬
				UpTraversalSetItemState(hParentItem, i);
				return;
			}
		}

		if(aCount[WTS_FAIL] > 0)
		{	// ���ʧ�������0,�򸸽ڵ���Ϊʧ��
			UpTraversalSetItemState(hParentItem, WTS_FAIL);
			return;
		}

		// ��������Ϊδ����״̬
		UpTraversalSetItemState(hParentItem, WTS_NORMAL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�״̬
/////////////////////////////////////////////////////////////////////////////
BYTE CWorkTreeCtrl::GetItemState1(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return NULL;
	return pData->byState;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemObject(HTREEITEM hItem, CObject* pObject)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->pObject	= pObject;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->pObject = pObject;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ����
/////////////////////////////////////////////////////////////////////////////
CObject* CWorkTreeCtrl::GetItemObject(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return NULL;
	return pData->pObject;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�ID
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemID(HTREEITEM hItem, int nID)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->nID		= nID;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->nID = nID;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�ID
/////////////////////////////////////////////////////////////////////////////
int CWorkTreeCtrl::GetItemID(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return NULL;
	if(GetParentItem(hItem) == NULL)
		return -1;
	return pData->nID;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�ͼ��
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemImage1(HTREEITEM hItem, int iImage)
{
	int nImage, nSelImage;
	GetItemImage(hItem, nImage, nSelImage);
	int nState = 0;
	if(m_nImageCount > 0)
		nState = nImage / m_nImageCount;
	nImage = m_nImageCount*nState+iImage;
	SetItemImage(hItem, nImage, nImage);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�ͼ��
/////////////////////////////////////////////////////////////////////////////
int CWorkTreeCtrl::GetItemImage1(HTREEITEM hItem)
{
	int nImage, nSelImage;
	GetItemImage(hItem, nImage, nSelImage);
	int nIndex = 0;
	if(m_nImageCount > 0)
		nIndex = nImage % m_nImageCount;
	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ���Ϣ
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemInfo(HTREEITEM hItem, CString strInfo)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->strInfo	= strInfo;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->strInfo	= strInfo;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ���Ϣ
/////////////////////////////////////////////////////////////////////////////
CString CWorkTreeCtrl::GetItemInfo(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return _T("");
	return pData->strInfo;
}

/////////////////////////////////////////////////////////////////////////////
// ���ýڵ�˫���ű�
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::SetItemDClick(HTREEITEM hItem, CString strDClick)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(pData)
	{
		pData->strDClick	= strDClick;
		SetItemData(hItem, (DWORD)pData);
		return;
	}

	pData = new HWORKTREEDATA;
	pData->strDClick	= strDClick;

	SetItemData(hItem, (DWORD)pData);
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�ýڵ�˫���ű�
/////////////////////////////////////////////////////////////////////////////
CString CWorkTreeCtrl::GetItemDClick(HTREEITEM hItem)
{
	HWORKTREEDATA* pData = (HWORKTREEDATA*)GetItemData(hItem);
	if(!pData)	return _T("");
	return pData->strDClick;
}

/////////////////////////////////////////////////////////////////////////////
// ȡ�������б�ID
/////////////////////////////////////////////////////////////////////////////
int CWorkTreeCtrl::GetWsID()
{
	return m_nWsNo;
}

/////////////////////////////////////////////////////////////////////////////
// ��ѯ�ڵ�(ͨ��ID��ѯHTREEITEM)
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CWorkTreeCtrl::GetItemFromID(int nID)
{
	for(int i=0; i<m_aDatas.GetSize(); i++)
	{
		HWORKTREEDATA* pData = (HWORKTREEDATA*)(m_aDatas[i]);
		if(pData != NULL)
		{
			if(nID == pData->nID)
			{
				return pData->hItem;
			}
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ѯ�ڵ�(ͨ��Info��ѯHTREEITEM)
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CWorkTreeCtrl::GetItemFromInfo(CString strInfo)
{
	for(int i=0; i<m_aDatas.GetSize(); i++)
	{
		HWORKTREEDATA* pData = (HWORKTREEDATA*)(m_aDatas[i]);
		if(pData != NULL)
		{
			if(strInfo == pData->strInfo)
			{
				return pData->hItem;
			}
		}
	}

	return NULL;
}


BEGIN_MESSAGE_MAP(CWorkTreeCtrl, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CWorkTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_TEST_INIT, OnTestInit)
	ON_COMMAND(ID_TEST_START, OnTestStart)
	ON_COMMAND(ID_TEST_STOP, OnTestStop)
	ON_COMMAND(ID_TEST_PAUSE, OnTestPause)
	ON_COMMAND(ID_TEST_CONTINUE, OnTestContinue)
	ON_COMMAND(ID_TEST_DISABLE, OnTestDisable)
	ON_COMMAND(ID_TEST_FRESH, OnTestFresh)
	ON_COMMAND(ID_TEST_PROPERTY, OnTestProperty)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(ID_RSM_PROPERTY, OnRsmProperty)
	ON_UPDATE_COMMAND_UI(ID_RSM_PROPERTY, OnUpdateRsmProperty)
	ON_COMMAND(ID_RSM_REFRESH, OnRsmRefresh)
	ON_UPDATE_COMMAND_UI(ID_RSM_REFRESH, OnUpdateRsmRefresh)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(1, 65535, OnWorkTreeCmd)
	ON_UPDATE_COMMAND_UI_RANGE(1, 65535, OnUpdateWorkTreeCmd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkTreeCtrl message handlers


/////////////////////////////////////////////////////////////////////////////
// ���ڵ�˫����Ϣ
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ASSERT(m_hWnd==pNMHDR->hwndFrom);

	HTREEITEM		hTarget = NULL;
	//TV_HITTESTINFO	tvht;
	//CPoint			m_point;

	//GetCursorPos(&m_point);
	//tvht.pt = m_point;
	//ScreenToClient(&tvht.pt);
	hTarget = GetFirstSelectedItem();
	//if ((hTarget = HitTest(&tvht)) != NULL)
	if(hTarget)
	{
		int nID = GetItemID(hTarget);
		if(nID >= 0)
		{
			theApp.m_ol.CommandPlat(OC_GOTOITEM, GetWsID(), nID);
		}

		// ִ��˫���ű�
		m_strCurDClickScript = GetItemDClick(hTarget);
		if(m_strCurDClickScript != "")
		{
			if(m_strCurDClickScript.Find("html:") == 0)
			{
				m_strCurDClickScript.Delete(0, 5);
				theApp.m_pIPlatUI->SendOwmCmd("org.owm.htmlview", OC_VIEWHTMLPAGE, ((WPARAM)(LPCTSTR)m_strCurDClickScript), 0, NULL);
			}else
			{
				ExecScript(m_strCurDClickScript);
			}
		}else
		if(m_strDClickScript != "")
		{
			// ��Ҫ������������,�ֱ�Ϊ�ڵ����ͽڵ�ID
			CString strScript;
			strScript.Format("%s %d %d %d", m_strDClickScript, m_nTreeID, hTarget, nID);
			ExecScript(strScript);
		}
	}

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Command Routing for Popup Menus
// Usage Example:
// CmdRouteMenu(pWnd,pSubMenu);
// pSubMenu->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON | 
//                         TPM_LEFTALIGN,point.x,point.y,pWnd,NULL);
/////////////////////////////////////////////////////////////////////////////
void CmdRouteMenu(CWnd* pWnd,CMenu* pPopupMenu)
{
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	state.m_pParentMenu = pPopupMenu;
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();

	for (state.m_nIndex = 0; 
	     state.m_nIndex < state.m_nIndexMax; 
	     state.m_nIndex++) 
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);

		// menu separator or invalid cmd - ignore it
		if (state.m_nID == 0) continue; 

		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to child menu if so
			CMenu* pSub=pPopupMenu->GetSubMenu(state.m_nIndex);
			if(pSub) CmdRouteMenu(pWnd,pSub);
		}
		else 
		{
			// normal menu item, Auto disable if command is 
			// _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �����Ҽ��˵�
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM		hTarget = NULL;
	//TV_HITTESTINFO	tvht;
	CPoint			m_point;

	GetCursorPos(&m_point);
	//tvht.pt = m_point;
	//ScreenToClient(&tvht.pt);
	hTarget = GetFirstSelectedItem();
	//if ((hTarget = HitTest(&tvht)) != NULL)
	if(hTarget)
	{
		m_hCurTreeItem = hTarget;
		CMenu* pMenu = (CMenu*)(GetItemMenu(hTarget));
		if(pMenu)
		{
			CMenu* pmenu = (CMenu*)(pMenu->GetSubMenu(0));
			::SetMenuDefaultItem(pmenu->m_hMenu, 0, TRUE);
		    SelectItem(hTarget);

			BYTE byType = GetItemType(m_hCurTreeItem);
			for(int i=0; i<m_aMenuItems.GetSize(); i++)
			{
				if(byType == m_aMenuItems[i].byType)
				{
					BOOL bExist = FALSE;
					int nMenuItemCount = pmenu->GetMenuItemCount();
					for(int j=0; j<nMenuItemCount; j++)
					{
						UINT nID = pmenu->GetMenuItemID(j);
						if(m_aMenuItems[i].nID == nID)
						{
							bExist = TRUE;
						}
					}

					if(!bExist)
					{
						pmenu->AppendMenu(MF_STRING, m_aMenuItems[i].nID, m_aMenuItems[i].strItem);
					}
				}else
				{
					// �����ڴ����ͽڵ�Ĳ˵�ɾ��
					pmenu->RemoveMenu(m_aMenuItems[i].nID, MF_BYCOMMAND);
				}
			}

			// ���ò˵���״̬
			pmenu->EnableMenuItem(ID_TEST_INIT, EnableTestInit() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_START, EnableTestStart() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_STOP, EnableTestStop() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_PAUSE, EnableTestPause() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_CONTINUE, EnableTestContinue() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_DISABLE, EnableTestDisable() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_FRESH, EnableTestFresh() ? MF_ENABLED : MF_GRAYED);
			pmenu->EnableMenuItem(ID_TEST_PROPERTY, EnableTestProperty() ? MF_ENABLED : MF_GRAYED);

			if(pmenu->GetMenuItemID(0) == 0)
			{
				// �����һ���˵�����SEPARATOR��ɾ��
				pmenu->RemoveMenu(0, MF_BYPOSITION);
				if(pmenu->GetMenuItemCount() == 0)
				{
					// ����˵�����Ϊ0���˳�
					*pResult = 0;
					return;
				}
			}

			// ִ�в˵�
			CmdRouteMenu(this, pmenu);
    		pmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					m_point.x, m_point.y, this);
		}
	}
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// �������
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnWorkTreeCmd(UINT nID)
{
	TRACE("WorkTreeCtrl::OnWorkTreeCmd %d\n", nID);
	//AfxMessageBox("ffff");
	BYTE byType = GetItemType(m_hCurTreeItem);
	CString strItem;
	CString strCommand;
	TYPEOF_Proc*		Proc = NULL;
	TYPEOF_UpdateProc*	UpdateProc = NULL;
	if(GetMenuItem(byType, nID, strItem, strCommand, &Proc, &UpdateProc))
	{
		CObject* pObject = GetItemObject(m_hCurTreeItem);
		if(Proc)
		{
			Proc(pObject);
		}else
		if(strCommand != "")
		{
			ExecScript(strCommand);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����ˢ�º���
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnUpdateWorkTreeCmd(CCmdUI* pCmdUI)
{
	UINT nID = pCmdUI->m_nID;
	TRACE("WorkTreeCtrl::OnUpdateWorkTreeCmd %d\n", nID);
	BYTE byType = GetItemType(m_hCurTreeItem);
	CString strItem;
	CString strCommand;
	TYPEOF_Proc*		Proc = NULL;
	TYPEOF_UpdateProc*	UpdateProc = NULL;
	if(GetMenuItem(byType, nID, strItem, strCommand, &Proc, &UpdateProc))
	{
		if(UpdateProc)
		{
			CObject* pObject = GetItemObject(m_hCurTreeItem);
			UpdateProc(pCmdUI, pObject);
			return;
		}else
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ʼ��
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestInit() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	if(theApp.m_ol.CommandPlat(OC_TESTINIT, GetWsID(), nOpID))
	{
		SetWsInit(TRUE);
	}else
		SetWsInit(FALSE);
}

BOOL CWorkTreeCtrl::EnableTestInit()
{
	BOOL bEnable = (GetParentItem(m_hCurTreeItem) == NULL);
	return bEnable;	// ֻ�и��ڵ����ִ�г�ʼ������
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ʼ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestStart() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTSTART, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestStart()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && ((byState == WTS_NORMAL) || (byState == WTS_FAIL)));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ֹ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestStop() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTSTOP, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestStop()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && (byState == WTS_TESTING));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ͣ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestPause() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTHOLD, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestPause()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && (byState == WTS_TESTING));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:�ָ�����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestContinue() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTRESUME, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestContinue()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && (byState == WTS_TESTING));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ֹ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestDisable() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTDISABLE, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestDisable()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && (byState != WTS_TESTING));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��λ����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestFresh() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTRESET, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestFresh()
{
	BYTE byState = GetItemState1(m_hCurTreeItem);
	return (IsWsInit() && (byState != WTS_TESTING));
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnTestProperty() 
{
	int nOpID = GetItemID(m_hCurTreeItem);
	theApp.m_ol.CommandPlat(OC_TESTPROPERTY, GetWsID(), nOpID);	
}

BOOL CWorkTreeCtrl::EnableTestProperty()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:ˢ�½ڵ�
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnRsmRefresh() 
{
	theApp.m_ol.CommandPlat(OC_NAVTREE_REFRESH, 0, GetItemID(m_hCurTreeItem));
}

void CWorkTreeCtrl::OnUpdateRsmRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// �˵�����:��ʾ�ڵ�����
/////////////////////////////////////////////////////////////////////////////
void CWorkTreeCtrl::OnRsmProperty() 
{
	CTOptArray aopt;	// ����������Ԫ������
	TOPTITEM item;		// ������Ԫ��
	item.hItem		= NULL;
	item.iType		= TOPT_GROUP;
	CString strTemp;
	//LOADRCSTRING( strTemp, IDS_LOCAL_TOTAL)
	item.lpszText	= "property";
	item.lpszEdit	= "";
	item.lpszList	= "";
	item.bCheck		= FALSE;
	item.iImage		= TOPTI_FOLDER;
	item.iParent	= -1;
	aopt.Add(item);		// ��Ӹ��ڵ�

	item.iParent	= 0;
	item.iType		= TOPT_EDIT;
	item.iImage		= TOPTI_INFO;

	item.lpszText	= "ID";
	item.lpszEdit.Format("%d", GetItemID(m_hCurTreeItem));
	aopt.Add(item);

	item.lpszText	= "Name";
	item.lpszEdit	= GetItemText(m_hCurTreeItem);
	aopt.Add(item);

	item.lpszText	= "Type";
	item.lpszEdit.Format("%d", GetItemType(m_hCurTreeItem));
	aopt.Add(item);

	item.lpszText	= "State";
	item.lpszEdit.Format("%d", GetItemState1(m_hCurTreeItem));
	aopt.Add(item);

	item.lpszText	= "Image";
	item.lpszEdit.Format("%d", GetItemImage1(m_hCurTreeItem));
	aopt.Add(item);

	item.lpszText	= "Info";
	item.lpszEdit	= GetItemInfo(m_hCurTreeItem);
	aopt.Add(item);

	item.lpszText	= "DClick";
	item.lpszEdit	= GetItemDClick(m_hCurTreeItem);
	aopt.Add(item);

	// ��ӱ������ʾ��Ϣ
	item.iType		= TOPT_MSG;
	//LOADRCSTRING( strTemp, IDS_LOCAL_LIST)
	item.lpszText	= "Tree Item Property";
	item.lpszEdit	= "You can modify these property.";
	item.iParent	= -1;
	aopt.Add(item);

	// ִ��������
	if(theApp.m_ol.CommandPlat(OC_EXECTREEOPTION, 0, (LPARAM)(&aopt)) == IDOK)
	{
		SetItemID(m_hCurTreeItem, atoi(aopt[1].lpszEdit));
		SetItemText(m_hCurTreeItem, aopt[2].lpszEdit);
		SetItemType(m_hCurTreeItem, atoi(aopt[3].lpszEdit));
		SetItemState1(m_hCurTreeItem, atoi(aopt[4].lpszEdit));
		SetItemImage1(m_hCurTreeItem, atoi(aopt[5].lpszEdit));
		SetItemInfo(m_hCurTreeItem, aopt[6].lpszEdit);
		SetItemDClick(m_hCurTreeItem, aopt[7].lpszEdit);
	}
}

void CWorkTreeCtrl::OnUpdateRsmProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

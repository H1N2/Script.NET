// TreeOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "TreeOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTreeOptionsDlg dialog


CTreeOptionsDlg::CTreeOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeOptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	PLAT_LOADSTRING(strTitle, IDS_TITLE_TREEOPTDLG); // ����
	m_strTitle = strTitle;
	m_strMsg = "";
	m_paTOpts = NULL;
}


void CTreeOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeOptionsDlg)
	DDX_Control(pDX, IDC_STATIC_MSG, m_staticMsg);
	DDX_Control(pDX, IDHELP, m_btnHelp);
	DDX_Control(pDX, IDCANCLE, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_REPORT, m_rcCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeOptionsDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeOptionsDlg message handlers

BOOL CTreeOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ilOptions.Create(IDB_OPTIONS, 16, 1, RGB(255,0,255));

	//m_ilMarks.Create(IDB_OPTIONMARKS, 16, 1, RGB(255, 0, 255));

	m_ilMarks.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1);
	CBitmap bmp;
	VERIFY(bmp.LoadBitmap(IDB_OPTIONMARKS));
	m_ilMarks.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));


	m_rcCtrl.SetImageList(&m_ilOptions, &m_ilMarks);
	//m_rcCtrl.ModifyStyle(0, RVS_SHOWHGRID);
	// �ı�ȱʡ�ķָ��,��Ϊȱʡ����"|",�������г�ͻ
	m_rcCtrl.ModifyProperty(RVP_SEPARATOR, 0x01);
	
	// ��������
	//m_rcCtrl.SetSelfFont("FixedSys", 16);

	if(m_paTOpts != NULL)
	{	// ��������
		for(int j=0; j<m_paTOpts->GetSize(); j++)
		{
			m_aTOpts.Add((*m_paTOpts)[j]);
		}
	}

	for(int i=0; i<m_aTOpts.GetSize(); i++)
	{
		InsertTOptItem(&(m_aTOpts[i]));
	}

	// չ�����ڵ�
	HTREEITEM hRootItem = GetItem(0);
	if(hRootItem != NULL)
	{
		m_rcCtrl.Expand(hRootItem, TVE_EXPAND);
	}

	// ���ñ������Ϣ
	SetWindowText(m_strTitle);
	//GetDlgItem(IDC_STATIC_MSG)->SetWindowText(m_strMsg);
	m_staticMsg.SetBkColor(RGB(255,255,255));
	m_staticMsg.SetTextColor(RGB(0,0,255));
	m_staticMsg.SetText(m_strMsg);

	// ������ť��ʼ��
	GetDlgItem(IDHELP)->EnableWindow(m_strHelpFile.GetLength()>0);

	//m_btnOK.SetXButtonStyle(BS_XT_HILITEPRESSED | BS_XT_SHOWFOCUS | BS_XT_XPFLAT);
	//m_btnCancel.SetXButtonStyle(BS_XT_HILITEPRESSED | BS_XT_SHOWFOCUS | BS_XT_XPFLAT);
	//m_btnHelp.SetXButtonStyle(BS_XT_HILITEPRESSED | BS_XT_SHOWFOCUS | BS_XT_XPFLAT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////
void CTreeOptionsDlg::ResetAllItem()
{
	m_aTOpts.RemoveAll();
}

BOOL CTreeOptionsDlg::IsGroup(HTREEITEM hItem)
{
	return m_rcCtrl.IsGroup(hItem);
}

BOOL CTreeOptionsDlg::IsCheckBox(HTREEITEM hItem)
{
	return m_rcCtrl.IsCheckBox(hItem);
}

BOOL CTreeOptionsDlg::IsRadioButton(HTREEITEM hItem)
{
	return m_rcCtrl.IsRadioButton(hItem);
}

BOOL CTreeOptionsDlg::IsEditable(HTREEITEM hItem)
{
	return m_rcCtrl.IsEditable(hItem);
}

BOOL CTreeOptionsDlg::IsDisabled(HTREEITEM hItem)
{
	return m_rcCtrl.IsDisabled(hItem);
}

BOOL CTreeOptionsDlg::SetCheckBox(HTREEITEM hItem, BOOL bCheck)
{
	return m_rcCtrl.SetCheckBox(hItem, bCheck);
}

BOOL CTreeOptionsDlg::GetCheckBox(HTREEITEM hItem, BOOL& bCheck)
{
	return m_rcCtrl.GetCheckBox(hItem, bCheck);
}

BOOL CTreeOptionsDlg::SetRadioButton(HTREEITEM hParent, INT iIndex)
{
	return m_rcCtrl.SetRadioButton(hParent, iIndex);
}

BOOL CTreeOptionsDlg::SetRadioButton(HTREEITEM hItem)
{
	return m_rcCtrl.SetRadioButton(hItem);
}

BOOL CTreeOptionsDlg::GetRadioButton(HTREEITEM hParent, INT& iIndex, HTREEITEM& hCheckItem)
{
	return m_rcCtrl.GetRadioButton(hParent, iIndex, hCheckItem);
}

BOOL CTreeOptionsDlg::GetRadioButton(HTREEITEM hItem, BOOL& bCheck)
{
	return m_rcCtrl.GetRadioButton(hItem, bCheck);
}

BOOL CTreeOptionsDlg::SetGroupEnable(HTREEITEM hParent, BOOL bEnable)
{
	return m_rcCtrl.SetGroupEnable(hParent, bEnable);
}

BOOL CTreeOptionsDlg::GetGroupEnable(HTREEITEM hParent, BOOL& bEnable)
{
	return m_rcCtrl.GetGroupEnable(hParent, bEnable);
}

BOOL CTreeOptionsDlg::SetCheckBoxEnable(HTREEITEM hItem, BOOL bEnable)
{
	return m_rcCtrl.SetCheckBoxEnable(hItem, bEnable);
}

BOOL CTreeOptionsDlg::GetCheckBoxEnable(HTREEITEM hItem, BOOL& bEnable)
{
	return m_rcCtrl.GetCheckBoxEnable(hItem, bEnable);
}

BOOL CTreeOptionsDlg::SetRadioButtonEnable(HTREEITEM hItem, BOOL bEnable)
{
	return m_rcCtrl.SetRadioButtonEnable(hItem, bEnable);
}

BOOL CTreeOptionsDlg::GetRadioButtonEnable(HTREEITEM hItem, BOOL& bEnable)
{
	return m_rcCtrl.GetRadioButtonEnable(hItem, bEnable);
}

CString CTreeOptionsDlg::GetEditText(HTREEITEM hItem)
{
	return m_rcCtrl.GetEditText(hItem);
}

void CTreeOptionsDlg::SetEditText(HTREEITEM hItem, LPCTSTR lpszText)
{
	m_rcCtrl.SetEditText(hItem, lpszText);
}

CString CTreeOptionsDlg::GetComboText(HTREEITEM hItem)
{
	return m_rcCtrl.GetComboText(hItem);
}

void CTreeOptionsDlg::SetComboText(HTREEITEM hItem, LPCTSTR lpszText)
{
	m_rcCtrl.SetComboText(hItem, lpszText);
}

CString CTreeOptionsDlg::GetDateTimeText(HTREEITEM hItem)
{
	return m_rcCtrl.GetDateTimeText(hItem);
}

void CTreeOptionsDlg::SetDateTimeText(HTREEITEM hItem, LPCTSTR lpszText)
{
	m_rcCtrl.SetDateTimeText(hItem, lpszText);
}

/////////////////////////////////////////////////////////////////////////////
// CReportOptionsCtrl operations

HTREEITEM CTreeOptionsDlg::InsertItem(LPCTSTR lpszItem, INT iImage, HTREEITEM hParent)
{
	return m_rcCtrl.InsertItem(lpszItem, iImage, hParent);
}

HTREEITEM CTreeOptionsDlg::InsertGroup(LPCTSTR lpszItem, INT iImage, HTREEITEM hParent)
{
	return m_rcCtrl.InsertItem(lpszItem, iImage, hParent);
}

HTREEITEM CTreeOptionsDlg::InsertCheckBox(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck)
{
	return m_rcCtrl.InsertCheckBox(lpszItem, hParent, bCheck);
}

HTREEITEM CTreeOptionsDlg::InsertRadioButton(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck)
{
	return m_rcCtrl.InsertRadioButton(lpszItem, hParent, bCheck);
}

BOOL CTreeOptionsDlg::AddEditBox(HTREEITEM hItem, LPCTSTR lpszText, CRuntimeClass* lpRuntimeClass)
{
	return m_rcCtrl.AddEditBox(hItem, lpszText, lpRuntimeClass);
}

BOOL CTreeOptionsDlg::AddComboBoxEx(HTREEITEM hItem, LPCTSTR lpszText, CString strTextList, CRuntimeClass* lpRuntimeClass)
{
	return m_rcCtrl.AddComboBoxEx(hItem, lpszText, strTextList, lpRuntimeClass);
}

BOOL CTreeOptionsDlg::AddDateTime(HTREEITEM hItem, LPCTSTR lpszText,  CString strTextList, CRuntimeClass* lpRuntimeClass)
{
	return m_rcCtrl.AddDateTime(hItem, lpszText, strTextList, lpRuntimeClass);
}

void CTreeOptionsDlg::DeleteItem(HTREEITEM hItem)
{
	m_rcCtrl.DeleteItem(hItem);
}

void CTreeOptionsDlg::SelectItem(HTREEITEM hItem)
{
	m_rcCtrl.SelectItem(hItem);
}

/////////////////////////////////////////////////////////////////////////////
// �������ⲿ����
/////////////////////////////////////////////////////////////////////////////
void CTreeOptionsDlg::SetTOptArrayPoint(CTOptArray* paOpts)
{
	m_paTOpts = paOpts;
}
/////////////////////////////////////////////////////////////////////////////
// �����
/////////////////////////////////////////////////////////////////////////////
int CTreeOptionsDlg::AddTOptItem(int iType, LPCTSTR lpszText, LPCTSTR lpszEdit, LPCTSTR lpszList, BOOL bCheck, int iImage, int iParent)
{
	TOPTITEM item;
	item.hItem		= NULL;
	item.iType		= iType;
	item.lpszText	= lpszText;
	item.lpszEdit	= lpszEdit;
	item.lpszList	= lpszList;
	item.bCheck		= bCheck;
	item.iImage		= iImage;
	item.iParent	= iParent;
	m_aTOpts.Add(item);
	return (m_aTOpts.GetSize()-1);
}

/////////////////////////////////////////////////////////////////////////////
// ���ñ������Ϣ
/////////////////////////////////////////////////////////////////////////////
int CTreeOptionsDlg::AddTOptMsg(LPCTSTR lpszTitle, LPCTSTR lpszMsg)
{
	TOPTITEM item;
	item.hItem		= NULL;
	item.iType		= TOPT_MSG;
	item.lpszText	= lpszTitle;
	item.lpszEdit	= lpszMsg;
	item.lpszList	= "";
	item.bCheck		= FALSE;
	item.iImage		= 0;
	item.iParent	= -1;
	m_aTOpts.Add(item);
	return (m_aTOpts.GetSize()-1);
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӱ����ĵ���Ϣ
/////////////////////////////////////////////////////////////////////////////
int CTreeOptionsDlg::AddTOptHelp(LPCTSTR lpszHelpFile)
{
	TOPTITEM item;
	item.hItem		= NULL;
	item.iType		= TOPT_HELP;
	item.lpszText	= lpszHelpFile;
	item.lpszEdit	= "";
	item.lpszList	= "";
	item.bCheck		= FALSE;
	item.iImage		= 0;
	item.iParent	= -1;
	m_aTOpts.Add(item);
	return (m_aTOpts.GetSize()-1);
}

/////////////////////////////////////////////////////////////////////////////
// ��Ӽ����㲥��ť
/////////////////////////////////////////////////////////////////////////////
int CTreeOptionsDlg::AddTOptItemCluster(int iType, LPCTSTR lpszText, BOOL bCheck, int iImage, int iParent)
{
	TOPTITEM item;
	item.hItem		= NULL;
	item.iType		= iType;
	item.lpszText	= lpszText;
	item.lpszEdit	= "";
	item.lpszList	= "";
	item.bCheck		= bCheck;
	item.iImage		= iImage;
	item.iParent	= iParent;
	m_aTOpts.Add(item);
	return (m_aTOpts.GetSize()-1);
}

/////////////////////////////////////////////////////////////////////////////
// ��ӱ༭�����Ͽ�
/////////////////////////////////////////////////////////////////////////////
int CTreeOptionsDlg::AddTOptItemEdit(int iType, LPCTSTR lpszText, LPCTSTR lpszEdit, LPCTSTR lpszList, int iImage, int iParent)
{
	TOPTITEM item;
	item.hItem		= NULL;
	item.iType		= iType;
	item.lpszText	= lpszText;
	item.lpszEdit	= lpszEdit;
	item.lpszList	= lpszList;
	item.bCheck		= FALSE;
	if(iImage == -1)
		item.iImage	= (iType==TOPT_COMBO) ? 3 : 2;
	else
		item.iImage	= iImage;
	item.iParent	= iParent;
	m_aTOpts.Add(item);
	return (m_aTOpts.GetSize()-1);
}

/////////////////////////////////////////////////////////////////////////////
// ����������ȡ���
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CTreeOptionsDlg::GetItem(int nItem)
{
	if((nItem < 0) || (nItem >= m_aTOpts.GetSize()))
		return RVTI_ROOT;
	return m_aTOpts[nItem].hItem;
}

/////////////////////////////////////////////////////////////////////////////
// ������
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CTreeOptionsDlg::InsertTOptItem(LPTOPTITEM lpItem)
{
	switch(lpItem->iType)
	{
	case TOPT_MSG:
		m_strTitle = lpItem->lpszText;
		m_strMsg = lpItem->lpszEdit;
		break;
	case TOPT_HELP:
		m_strHelpFile = lpItem->lpszText;
		break;
	case TOPT_ITEM:
		lpItem->hItem = InsertItem(lpItem->lpszText, lpItem->iImage, GetItem(lpItem->iParent));
		break;
	case TOPT_GROUP:
		lpItem->hItem = InsertGroup(lpItem->lpszText, lpItem->iImage, GetItem(lpItem->iParent));
		break;
	case TOPT_CHECKBOX:
		lpItem->hItem = InsertCheckBox(lpItem->lpszText, GetItem(lpItem->iParent), lpItem->bCheck);
		break;
	case TOPT_RADIOBUTTON:
		lpItem->hItem = InsertRadioButton(lpItem->lpszText, GetItem(lpItem->iParent), lpItem->bCheck);
		break;
	case TOPT_EDIT:
		lpItem->hItem = InsertItem(lpItem->lpszText, lpItem->iImage, GetItem(lpItem->iParent));
		AddEditBox(lpItem->hItem, lpItem->lpszEdit);
		break;
	case TOPT_COMBO:
		lpItem->hItem = InsertItem(lpItem->lpszText, lpItem->iImage, GetItem(lpItem->iParent));
		AddComboBoxEx(lpItem->hItem, lpItem->lpszEdit, lpItem->lpszList);
		break;
	case TOPT_TIME:
		lpItem->hItem = InsertItem(lpItem->lpszText, lpItem->iImage, GetItem(lpItem->iParent));
		AddDateTime(lpItem->hItem, lpItem->lpszEdit, lpItem->lpszList);
		break;
	}
	return lpItem->hItem;
}

/////////////////////////////////////
// get functions
/////////////////////////////////////////////////////////////////////////////
// ��ȡ�ڵ���
/////////////////////////////////////////////////////////////////////////////
LPTOPTITEM CTreeOptionsDlg::GetItemHandle(LPCTSTR lpszItem)
{
	for(int i=0; i<m_aTOpts.GetSize(); i++)
	{
		if(strcmp(lpszItem, m_aTOpts[i].lpszText) == 0)
		{
			return &(m_aTOpts[i]);
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ������
/////////////////////////////////////////////////////////////////////////////
BOOL CTreeOptionsDlg::GetCheckBox(LPCTSTR lpszItem)
{
	LPTOPTITEM lpItem = GetItemHandle(lpszItem);
	if(lpItem)
		return lpItem->bCheck;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�㲥��ť���
/////////////////////////////////////////////////////////////////////////////
BOOL CTreeOptionsDlg::GetRadioButton(LPCTSTR lpszItem)
{
	LPTOPTITEM lpItem = GetItemHandle(lpszItem);
	if(lpItem)
		return lpItem->bCheck;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ�༭����
/////////////////////////////////////////////////////////////////////////////
CString CTreeOptionsDlg::GetEditText(LPCTSTR lpszItem)
{
	LPTOPTITEM lpItem = GetItemHandle(lpszItem);
	if(lpItem)
		return lpItem->lpszEdit;
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ��Ͽ���
/////////////////////////////////////////////////////////////////////////////
CString CTreeOptionsDlg::GetComboText(LPCTSTR lpszItem)
{
	LPTOPTITEM lpItem = GetItemHandle(lpszItem);
	if(lpItem)
		return lpItem->lpszEdit;
	return "";
}

/////////////////////////////////////////////////////////////////////////////
// ������Ľ��
/////////////////////////////////////////////////////////////////////////////
void CTreeOptionsDlg::SetTOptItem(LPTOPTITEM lpItem)
{
	switch(lpItem->iType)
	{
	case TOPT_CHECKBOX:
		if(lpItem->hItem)
			m_rcCtrl.GetCheckBox(lpItem->hItem, lpItem->bCheck);
		break;
	case TOPT_RADIOBUTTON:
		if(lpItem->hItem)
			m_rcCtrl.GetRadioButton(lpItem->hItem, lpItem->bCheck);
		break;
	case TOPT_EDIT:
		if(lpItem->hItem)
			lpItem->lpszEdit = m_rcCtrl.GetEditText(lpItem->hItem);
		break;
	case TOPT_COMBO:
		if(lpItem->hItem)
			lpItem->lpszEdit = m_rcCtrl.GetComboText(lpItem->hItem);
		break;
	case TOPT_TIME:
		if(lpItem->hItem)
			lpItem->lpszEdit = m_rcCtrl.GetDateTimeText(lpItem->hItem);
		break;
	}
}

void CTreeOptionsDlg::OnOK() 
{
	// �ش����
	for(int i=0; i<m_aTOpts.GetSize(); i++)
	{
		SetTOptItem(&(m_aTOpts[i]));
		if(m_paTOpts != NULL)
		{	// ��������
			(*m_paTOpts)[i] = m_aTOpts[i];
		}
		int nImage = m_aTOpts[i].iImage;
		if(TOPTI_M_FOLDER==nImage || TOPTI_M_GROUP==nImage ||
			TOPTI_M_EDIT==nImage || TOPTI_M_COMBO==nImage)
		{	// ������Ŀ,��Ҫ�ж��Ƿ��Ѿ�����
			int nType = m_aTOpts[i].iType;
			if(TOPT_EDIT==nType || TOPT_COMBO==nType)
			{	// ֻ��������EDIT��COMBO�Ĳ���Ҫ�ж�
				if(m_aTOpts[i].lpszEdit.GetLength() == 0)
				{	// �ؼ��ֶ�(��ɫ��ǵ���Ŀ)����Ϊ��\n��������д!
					AfxMessageBox(IDS_INFO_TREEOPTDLG_MUSTINPUT);
					return;
				}
			}
		}
	}

	// ѡ�е���
	INT nSel = m_rcCtrl.GetFirstSelectedItem();
	if((m_paTOpts != NULL) && (m_paTOpts->GetSize() > 0))
	{	// ���÷��ؽ����Ԫ��0��hItemΪ��ѡ��ĵ�һ���ڵ�����
		(*m_paTOpts)[0].hItem = (HTREEITEM)nSel;
	}
	
	CDialog::OnOK();
}

void CTreeOptionsDlg::OnHelp() 
{
	ShellExecute(GetSafeHwnd(), NULL, "hh.exe", m_strHelpFile, NULL, SW_SHOW);
}

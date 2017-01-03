// TreeOutCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "TreeOutCtrl.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeOutCtrl

CTreeOutCtrl::CTreeOutCtrl()
{
	m_hLastItem = NULL;
	m_strSeparator = _T(",");
	m_pParentBar = NULL;
}

CTreeOutCtrl::~CTreeOutCtrl()
{
	// ɾ���������ڵ����
	DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CTreeOutCtrl, CReportCtrl)
	//{{AFX_MSG_MAP(CTreeOutCtrl)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_OEDIT_CLEAR, OnOeditClear)
	ON_COMMAND(ID_OUTCTRL_DELROW, OnOutctrlDelrow)
	ON_COMMAND(ID_OUTCTRL_SCRIPT, OnOutctrlScript)
	ON_UPDATE_COMMAND_UI(ID_OUTCTRL_SCRIPT, OnUpdateOutctrlScript)
	ON_UPDATE_COMMAND_UI(ID_OEDIT_CLEAR, OnUpdateOeditClear)
	ON_UPDATE_COMMAND_UI(ID_OUTCTRL_DELROW, OnUpdateOutctrlDelrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTreeOutCtrl message handlers

BOOL CTreeOutCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= RVS_TREEVIEW;	// ����ReportCtrl�ؼ�Ϊ������
	
	return CReportCtrl::PreCreateWindow(cs);
}

int CTreeOutCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CReportCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, RVS_SHOWHGRID|RVS_SHOWVGRID|RVS_EXPANDSUBITEMS);
	//ModifyProperty(RVP_SEPARATOR, 0x01);

	// ��ʼ����ɫ��
	InsertColor(0, RGB(255,255,225));	// ����
	InsertColor(1, RGB(0,0,0));			// ��
	InsertColor(2, RGB(255,255,255));	// ��
	InsertColor(3, RGB(255,0,0));		// ��
	InsertColor(4, RGB(0,255,0));		// ��
	InsertColor(5, RGB(0,0,255));		// ��
	InsertColor(6, RGB(128,128,128));	// ��
	InsertColor(7, RGB(128,0,0));		// ���
	InsertColor(8, RGB(0,128,0));		// ����
	InsertColor(9, RGB(0,0,128));		// ����

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ��
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::Init()
{
	// Create the image list.
	m_ilTree.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1);

	CBitmap bmp;
	VERIFY(bmp.LoadBitmap(IDB_REPORT_CTRL));
	m_ilTree.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));

	SetImageList(&m_ilTree);
}

/////////////////////////////////////////////////////////////////////////////
// �رմ���ʱִ�еĽű�
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::DoCloseScript() 
{
	// ɾ���������ڵ����
	DeleteAllItems();

	if(m_strScriptClose.GetLength() == 0)
		return;

	// ִ�д��ڹر�ǰ�Ľű�
	CString strInterpInstance = _T("");

	// ������������(֧��������ʽ�Ľ�����)
	if(m_strScriptClose.Find("<<") == 0)
	{
		int nPos = m_strScriptClose.Find(">>");
		if(nPos != -1)
		{
			m_strScriptClose.Delete(0, 2);
			strInterpInstance = m_strScriptClose.Left(nPos-2);
			m_strScriptClose.Delete(0, nPos);
		}
	}

	CString strInterpName = "";	// ��������
	if(m_strScriptClose.Find("<") == 0)
	{
		m_strScriptClose.Delete(0, 1);
		int nPos = m_strScriptClose.Find(">");
		if(nPos != -1)
		{
			strInterpName = m_strScriptClose.Left(nPos);
			if(strInterpName.Find("org.interp") != 0)
			{
				strInterpName = "org.interp." + strInterpName;
			}
			m_strScriptClose.Delete(0, nPos+1);
		}		
	}

	// ������������ִ��
	if(strInterpName.GetLength() > 0)
	{
		IInterp* pInterp = (IInterp*)(theApp.CreateVciObject(strInterpName, strInterpInstance));
		if(pInterp)
		{
			pInterp->RunScriptCommand(m_strScriptClose);
			theApp.ReleaseObject(pInterp);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ֳ�������
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::ParseLines(CString strSet, CStringArray& aLines)
{
	int pos;
	while((pos = strSet.Find(";")) >= 0)
	{
		aLines.Add(strSet.Left(pos));
		int len = strSet.GetLength() - pos -1;
		strSet = strSet.Right(len);
	}
	if(strSet.GetLength() > 0)
	{
		aLines.Add(strSet);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����style
/////////////////////////////////////////////////////////////////////////////
int CTreeOutCtrl::ParseStyle(CString strStyle)
{
	if(strStyle == _T("SINGLESELECT"))
		return RVS_SINGLESELECT;
	else if(strStyle == _T("SHOWSELALWAYS"))
		return RVS_SHOWSELALWAYS;
	else if(strStyle == _T("SHOWCOLORALWAYS"))
		return RVS_SHOWCOLORALWAYS;
	else if(strStyle == _T("SHOWCOLORALTERNATE"))	// �任����ɫ,ע����Ҫ����Colors����
		return RVS_SHOWCOLORALTERNATE;
	else if(strStyle == _T("SHOWHGRID"))
		return RVS_SHOWHGRID;
	else if(strStyle == _T("SHOWVGRID"))
		return RVS_SHOWVGRID;
	else if(strStyle == _T("NOHEADER"))
		return RVS_NOHEADER;
	else if(strStyle == _T("NOSORT"))
		return RVS_NOSORT;
	else if(strStyle == _T("ALLOWCOLUMNREMOVAL"))
		return RVS_ALLOWCOLUMNREMOVAL;
	else if(strStyle == _T("SHOWHGRIDEX"))	// ��ʾ����ı����
		return RVS_SHOWHGRIDEX;
	else if(strStyle == _T("FOCUSSUBITEMS"))
		return RVS_FOCUSSUBITEMS;
	else if(strStyle == _T("EXPANDSUBITEMS"))
		return RVS_EXPANDSUBITEMS;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ����������
// �﷨����
// 1.���������ɸ��������";"����
// 2.�����: ADDCOLUMN name,width,format
// 3.ɾ����: DELCOLUMN nCol|all
// 4.ɾ����: DELITEM nItem|all
// 5.���÷ָ��: SEPARATOR=???
// 6.���������е���ɫ: SETROWCOLORS cText cTextBk
// 7.������ɫ���ĳ����ɫ: SETCOLORTABLE index color
// 8.�޸Ŀؼ����: MODIFYSTYLE ADD|REMOVE style
// 9.��ע����ȡ����Ϣ: READPROFILE Section, Entry
// 10.��ע���д������Ϣ: WRITEPROFILE Section, Entry
// 11.���ô��ڹر�ʱִ�еĽű�: SETCLOSESCRIPT script
// 12.��������: SETFONT font size
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::ParseALine(CString strLine)
{
	CString strSet = strLine;
	strSet.MakeUpper();
	int pos;
	if((pos = strSet.Find("ADDCOLUMN ")) == 0)	// �����
	{
		CString strColName;
		CString strFormat = _T("LEFT");
		strSet = strLine.Right(strLine.GetLength()-10);
		int nPos = strSet.Find(",");
		strColName = strSet.Left(nPos);
		strSet = strSet.Right(strSet.GetLength()-nPos-1);

		// �����п��
		nPos = strSet.Find(",");
		if(nPos != -1)
		{
			strFormat = strSet.Right(strSet.GetLength()-nPos-1);
			strFormat.MakeUpper();
			strSet = strSet.Left(nPos);
		}
		int nWidth = atoi(strSet);

		RVSUBITEM rvs;
		rvs.nFormat = RVCF_TEXT|RVCF_EX_TOOLTIP|RVCF_EX_PERSISTENT;
		if(strFormat == _T("CENTER"))
		{
			rvs.nFormat |= RVCF_CENTER;
		}else
		if(strFormat == _T("RIGHT"))
		{
			rvs.nFormat |= RVCF_RIGHT;
		}
		rvs.lpszText = strColName.GetBuffer(strColName.GetLength()+1);
		rvs.iWidth = nWidth;
		int nSubItem = GetActiveSubItemCount();
		DefineSubItem(nSubItem, &rvs, TRUE);
		ActivateSubItem(nSubItem, nSubItem);
		strColName.ReleaseBuffer();
	}else
	if((pos = strSet.Find("DELCOLUMN ")) == 0)	// ɾ����
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		if(strSet == _T("ALL"))
		{
			UndefineAllSubItems();
			m_asItemScript.RemoveAll();
		}else
		{
			int nCol = atoi(strSet);
			UndefineSubItem(nCol);
			if(GetActiveSubItemCount() == 0)
			{
				m_asItemScript.RemoveAll();
			}
		}
	}else
	if((pos = strSet.Find("DELITEM ")) == 0)	// ɾ����
	{
		strSet = strSet.Right(strSet.GetLength()-8);
		if(strSet == _T("ALL"))
		{
			DeleteAllItems();
			m_asItemScript.RemoveAll();
		}else
		{
			int nItem = atoi(strSet);
			HTREEITEM hItem = GetItemHandle(nItem);
			DeleteItem(hItem);
			if(GetItemCount() == 0)
			{
				m_asItemScript.RemoveAll();
			}
		}
	}else
	if((pos = strSet.Find("SEPARATOR=")) == 0)	// ���÷ָ��
	{
		m_strSeparator = strLine.Right(strLine.GetLength()-10);
	}else
	if((pos = strSet.Find("SETROWCOLORS ")) == 0)	// ������������ɫ
	{
		strSet = strSet.Right(strSet.GetLength()-13);
		CString strBkColor = _T("");
		int nPos = strSet.Find(" ");
		if(nPos != -1)
		{
			strBkColor = strSet.Right(strSet.GetLength()-nPos-1);
			strSet = strSet.Left(nPos);
		}
		COLORREF cText = RGB(0,0,0);
		COLORREF cTextBk = RGB(255,255,255);
		int nPos1;
		if((nPos1 = strSet.Find("RGB(")) == 0)
		{
			strSet = strSet.Right(strSet.GetLength()-pos-4);
			strSet.Delete(strSet.GetLength()-1, 1);
			nPos1 = strSet.Find(",");
			CString strTmp = strSet.Left(nPos1);
			int nr = atoi(strTmp);
			strSet = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			nPos1 = strSet.Find(",");
			strTmp = strSet.Left(nPos1);
			int ng = atoi(strTmp);
			strTmp = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			int nb = atoi(strTmp);
			cText = RGB(nr, ng, nb);
		}
		strSet = strBkColor;
		if((nPos1 = strSet.Find("RGB(")) == 0)
		{
			strSet = strSet.Right(strSet.GetLength()-pos-4);
			strSet.Delete(strSet.GetLength()-1, 1);
			nPos1 = strSet.Find(",");
			CString strTmp = strSet.Left(nPos1);
			int nr = atoi(strTmp);
			strSet = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			nPos1 = strSet.Find(",");
			strTmp = strSet.Left(nPos1);
			int ng = atoi(strTmp);
			strTmp = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			int nb = atoi(strTmp);
			cTextBk = RGB(nr, ng, nb);
		}
		SetRowColors(cText, cTextBk);
	}else
	if((pos = strSet.Find("SETCOLORTABLE ")) == 0)	// ������ɫ���ĳ����ɫ
	{
		strSet = strSet.Right(strSet.GetLength()-14);
		int nIndex = 0;
		int nPos = strSet.Find(" ");
		if(nPos != -1)
		{
			CString strIndex = strSet.Left(nPos);
			nIndex = atoi(strIndex);
			if(nIndex<0 || nIndex>9)
				return;
			strSet.Delete(0, nPos+1);
		}
		if(strSet.Find("RGB(") == 0)
		{
			strSet = strSet.Right(strSet.GetLength()-pos-4);
			strSet.Delete(strSet.GetLength()-1, 1);
			int nPos1 = strSet.Find(",");
			CString strTmp = strSet.Left(nPos1);
			int nr = atoi(strTmp);
			strSet = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			nPos1 = strSet.Find(",");
			strTmp = strSet.Left(nPos1);
			int ng = atoi(strTmp);
			strTmp = strSet.Mid(nPos1 + 1, strSet.GetLength() - nPos1 -1);
			int nb = atoi(strTmp);
			COLORREF cColor = RGB(nr, ng, nb);
			SetColor(nIndex, cColor);
		}
	}else
	if((pos = strSet.Find("MODIFYSTYLE ")) == 0)	// �޸Ŀؼ����
	{
		strSet = strSet.Right(strSet.GetLength()-12);
		BOOL bAdd = TRUE;
		if(strSet.Find("REMOVE(") == 0)
		{
			bAdd = FALSE;
			strSet.Delete(0, 7);
		}else
		{
			strSet.Delete(0, 4);
		}
		int nPos = strSet.Find(")");
		if(nPos != -1)
		{
			strSet = strSet.Left(nPos);
		}
		int nStyle = 0;
		while((nPos=strSet.Find("|")) != -1)
		{
			CString strStyle = strSet.Left(nPos);
			nStyle |= ParseStyle(strStyle);
			strSet.Delete(0, nPos+1);
		}
		nStyle |= ParseStyle(strSet);
		ModifyStyle(bAdd ? 0 : nStyle, bAdd ? nStyle : 0);
	}else
	if(strSet.Find("READPROFILE ") == 0)	// ��ע����ȡ����Ϣ
	{
		strLine = strLine.Right(strLine.GetLength()-12);
		int nPos = strLine.Find(",");
		if(nPos != -1)
		{
			CString strSection = strLine.Left(nPos);
			CString strEntry = strLine.Right(strLine.GetLength()-nPos-1);
			GetProfile(strSection, strEntry);
		}
	}else
	if(strSet.Find("WRITEPROFILE ") == 0)	// ��ע���д������Ϣ
	{
		strLine = strLine.Right(strLine.GetLength()-13);
		int nPos = strLine.Find(",");
		if(nPos != -1)
		{
			CString strSection = strLine.Left(nPos);
			CString strEntry = strLine.Right(strLine.GetLength()-nPos-1);
			WriteProfile(strSection, strEntry);
		}
	}else
	if(strSet.Find("SETCLOSESCRIPT ") == 0)	// ���ô��ڹر�ʱִ�еĽű�
	{
		m_strScriptClose = strLine.Right(strLine.GetLength()-15);
	}else
	if(strSet.Find("SETFONT ") == 0)	// ��������
	{
		strLine = strLine.Right(strLine.GetLength()-8);
		CString strFont = strLine;
		int nSize = 0;
		int nPos = strLine.Find(",");
		if(nPos != -1)
		{
			strFont = strLine.Left(nPos);
			CString strSize = strLine.Right(strLine.GetLength()-nPos-1);
			nSize = atoi(strSize);
		}
		SetSelfFont(strFont, nSize);
	}
}

//////////////////////////////////////////////////////////////////
// ���������ʽ
//////////////////////////////////////////////////////////////////
void CTreeOutCtrl::SetCtrlFormat(CString strSet)
{
	CStringArray aLines;
	// ��ֳ�������
	ParseLines(strSet, aLines);
	for(int i=0; i<aLines.GetSize(); i++)
	{
		CString strLine = aLines[i];
		ParseALine(strLine);	// ������
	}
}

//////////////////////////////////////////////////////////////////
// �����Ϣ
// �﷨����
// 1.���õ�������: <I>SET ITEM<nItem,nSubItem> TEXT XXXX
// 2.���õ���ͼ��: <I>SET ITEM<nItem,nSubItem> IMAGE nImage
// 3.���õ�������: <I>SET ITEM<nItem,nSubItem> CHECK nCheck
// 4.����������ɫ: <I>SET ITEM<nItem,nSubItem> COLOR colorIndex
// 5.��������״̬: <I>SET ITEM<nItem,nSubItem> STATE ?BOLD?
// 6.������˫���¼��ű�: <I>SET SCRIPT<nRow> <nIndex>|script
// 7.�����չ�˵�: <I>ADDEXMENU menu script
// 8.�����չ�˵�: <I>CLEAREXMENU
// 9.�����: ?<hParentItem,image>?XXX,XXX,XXX,XXX
// 10.չ�����ڵ�: <T>EXPANDITEM<hItem>
//////////////////////////////////////////////////////////////////
void CTreeOutCtrl::Output(CString strOutput)
{
	if(strOutput.Find("<I>") == 0)	// �������
	{
		strOutput = strOutput.Right(strOutput.GetLength()-3);
		strOutput.TrimLeft();
		CString strSet = strOutput;
		strSet.MakeUpper();
		if(strSet.Find("SET ITEM<") == 0)	// ���õ�������
		{
			int nPos = strSet.Find("> ");
			if(nPos == -1)
				return;
			strOutput = strOutput.Right(strOutput.GetLength()-nPos-2);
			strSet = strSet.Mid(9, nPos-9);
			nPos = strSet.Find(",");
			if(nPos == -1)
				return;
			CString strItem = strSet.Left(nPos);
			int nItem = atoi(strItem);
			if(nItem<0 || nItem>=GetItemCount())
				return;
			strSet = strSet.Right(strSet.GetLength()-nPos-1);
			int nSubItem = atoi(strSet);
			if(nSubItem >= GetActiveSubItemCount())
				return;
			strSet = strOutput;
			strSet.MakeUpper();
			if(strSet.Find("TEXT ") == 0)	// ���õ�������
			{
				strOutput = strOutput.Right(strOutput.GetLength()-5);
				SetItemText(nItem, nSubItem, strOutput);
			}else
			if(strSet.Find("IMAGE ") == 0)	// ���õ���ͼ��
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				int nImage = atoi(strOutput);
				SetItemImage(nItem, nSubItem, nImage);
			}else
			if(strSet.Find("CHECK ") == 0)	// ���õ�������
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				int nCheck = atoi(strOutput);
				SetItemCheck(nItem, nSubItem, nCheck);
			}else
			if(strSet.Find("COLOR ") == 0)	// ����������ɫ
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				RVITEM rvi;
				rvi.nMask = RVIM_TEXTCOLOR;
				rvi.iItem = nItem;
				rvi.iSubItem = nSubItem;
				rvi.iTextColor = atoi(strOutput);	// ��ɫ�������
				if(nSubItem == -1)	// -1��ʾ�������е���ɫ
				{
					for(int i=0; i<GetActiveSubItemCount(); i++)
					{
						rvi.iSubItem = i;
						SetItem(&rvi);
					}
				}else
				{
					SetItem(&rvi);
				}
			}else
			if(strSet.Find("STATE ") == 0)	// ��������״̬
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				strOutput.MakeUpper();
				RVITEM rvi;
				rvi.nMask = RVIM_STATE;
				rvi.iItem = nItem;
				rvi.iSubItem = nSubItem;
				rvi.nState = 0;
				if(strOutput == "BOLD")
					rvi.nState = RVIS_BOLD;
				if(nSubItem == -1)	// -1��ʾ�������е���ɫ
				{
					for(int i=0; i<GetActiveSubItemCount(); i++)
					{
						rvi.iSubItem = i;
						SetItem(&rvi);
					}
				}else
				{
					SetItem(&rvi);
				}
			}
		}else
		if(strSet.Find("SET SCRIPT<") == 0)	// �����е�˫���¼��ű�
		{
			strSet = strOutput.Right(strOutput.GetLength()-11);
			// ��ȡ�к�
			int nPos = strSet.Find("> ");
			if(nPos == -1)
				return;
			CString strItem = strSet.Left(nPos);
			strSet = strSet.Right(strSet.GetLength()-nPos-2);
			if((strSet.Find("<")==0) && (strSet.Find("<<")!=0))	// ʹ����ǰ�����ĳ���ű�
			{
				strSet.Delete(0, 1);
				int nPos = strSet.Find(">");
				if(nPos == -1)
					return;
				strSet = strSet.Left(nPos);
				int nItem = atoi(strItem);
				if(strItem.Find("#") == 0)	// #��ͷ��ʾ�Ǿ��,����������
				{
					nItem = GetItemIndex((HTREEITEM)(atoi(strItem)));
				}
				if(nItem < GetItemCount())
					SetItemData(nItem, atoi(strSet));
				return;
			}
			// ��ӽű�,��������(ע:���õ��к������Ǵ�1��ʼ������,��ΪItemDataȱʡΪ0)
			m_asItemScript.Add(strSet);
			int nItem = atoi(strItem);
			if(strItem.Find("#") == 0)	// #��ͷ��ʾ�Ǿ��,����������
			{
				nItem = GetItemIndex((HTREEITEM)(atoi(strItem)));
			}
			if(nItem>=0 && nItem<GetItemCount())
			{
				SetItemData(nItem, m_asItemScript.GetSize());
			}
		}else
		if(strSet.Find("ADDEXMENU ") == 0)	// �����չ�˵�
		{
			strOutput = strOutput.Right(strOutput.GetLength()-10);
			int nPos = strOutput.Find(" ");
			if(nPos != -1)
			{
				CString strScript = strOutput.Right(strOutput.GetLength()-nPos-1);
				strOutput = strOutput.Left(nPos);
				m_asExMenu.Add(strOutput);
				m_asExMenuScript.Add(strScript);
			}
		}else
		if(strSet.Find("CLEAREXMENU") == 0)	// �����չ�˵�
		{
			m_asExMenu.RemoveAll();
			m_asExMenuScript.RemoveAll();
		}
	}else
	if(strOutput.Find("<T>") == 0)	// ������
	{
		strOutput = strOutput.Right(strOutput.GetLength()-3);
		strOutput.TrimLeft();
		CString strSet = strOutput;
		strSet.MakeUpper();
		if(strSet.Find("EXPANDITEM<") == 0)	// չ�����ڵ�
		{
			int nPos = strSet.Find(">");
			if(nPos == -1)
				return;
			strSet = strSet.Mid(11, nPos-11);
			HTREEITEM hItem = (HTREEITEM)(atoi(strSet));
			if(strSet == "ROOT")
				hItem = RVTI_ROOT;
			if(hItem == NULL)
				return;
			Expand(hItem, RVE_EXPAND);
		}
	}else	// �������
	{
		CString strItem = _T("");
		int nSepLen = m_strSeparator.GetLength();
		int nPos;
		int nImage = -1;
		HTREEITEM hParentItem = RVTI_ROOT;
		// �������ڵ��ͼ���ֶ�
		if(strOutput.Find("<") == 0)
		{
			nPos = strOutput.Find(">");
			if(nPos > 1)
			{
				CString strParam = strOutput.Mid(1, nPos-1);
				strOutput = strOutput.Right(strOutput.GetLength()-nPos-1);
				nPos = strParam.Find(",");
				if(nPos != -1)
				{
					CString strImage = strParam.Right(strParam.GetLength()-nPos-1);
					nImage=  atoi(strImage);
					strParam = strParam.Left(nPos);
				}
				hParentItem = (HTREEITEM)(atoi(strParam));
				if(strParam == "ROOT")
					hParentItem = RVTI_ROOT;
				if(hParentItem == NULL)
					return;				
			}
		}
		// ����������
		nPos = strOutput.Find(m_strSeparator);
		if(nPos != -1)
		{
			strItem = strOutput.Left(nPos);
			strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
		}else
		{
			strItem = strOutput;
		}
		m_hLastItem = InsertItem(strItem, nImage, -1, -1, hParentItem, RVTI_LAST);//, 0);
		int nIndex = GetItemIndex(m_hLastItem);
		if(nPos != -1)
		{
			int nCount = 1;
			while((nPos = strOutput.Find(m_strSeparator)) != -1)
			{
				strItem = strOutput.Left(nPos);
				strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
				if(nCount >= GetActiveSubItemCount())
					return;
				SetItemText(nIndex, nCount++, strItem);
			}
			if(nCount >= GetActiveSubItemCount())
				return;
			SetItemText(nIndex, nCount++, strOutput);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��ʽ�����
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::vsWrite(LPCTSTR lpszFormat, va_list argList)
{
	ASSERT(AfxIsValidString(lpszFormat, FALSE));

	va_list argListSave = argList;
	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCTSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _tcsinc(lpsz)) {

		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%') {

			nMaxLen += _tclen(lpsz);
			continue;
		}

		int nItemLen = 0;
		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz)) {

			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0) {

			// width indicated by
			nWidth = _ttoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
				;
		}
		ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.') {

			// skip past '.' separator (width.precision)
			lpsz = _tcsinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*') {

				nPrecision = va_arg(argList, int);
				lpsz = _tcsinc(lpsz);
			}
			else {

				nPrecision = _ttoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
					;
			}
			ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		switch (*lpsz) {

		// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz = _tcsinc(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = _tcsinc(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _tcsinc(lpsz);
				break;
		}

		// now should be on specifier
		switch (*lpsz | nModifier) {

			// single characters
			case 'c':
			case 'C':
				nItemLen = 2;
				va_arg(argList, TCHAR_ARG);
				break;
			case 'c'|FORCE_ANSI:
			case 'C'|FORCE_ANSI:
				nItemLen = 2;
				va_arg(argList, CHAR_ARG);
				break;
			case 'c'|FORCE_UNICODE:
			case 'C'|FORCE_UNICODE:
				nItemLen = 2;
				va_arg(argList, WCHAR_ARG);
				break;

			// strings
			case 's': {

				LPCTSTR pstrNextArg = va_arg(argList, LPCTSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else {

				   nItemLen = lstrlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}

			case 'S': {

	#ifndef _UNICODE
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else {

				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
	#else
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else {

				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
	#endif
				break;
			}

			case 's'|FORCE_ANSI:
			case 'S'|FORCE_ANSI:
			{

				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{

				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}

	#ifndef _MAC
			case 's'|FORCE_UNICODE:
			case 'S'|FORCE_UNICODE: {

				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{

				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}
	#endif
		}

			// adjust nItemLen for strings
		if (nItemLen != 0) {

			nItemLen = max(nItemLen, nWidth);
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
		}
		else{

			switch (*lpsz) {

				// integers
				case 'd':
				case 'i':
				case 'u':
				case 'x':
				case 'X':
				case 'o':
					va_arg(argList, int);
					nItemLen = 32;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				case 'e':
				case 'f':
				case 'g':
				case 'G':
					va_arg(argList, DOUBLE_ARG);
					nItemLen = 128;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				case 'p':
					va_arg(argList, void*);
					nItemLen = 32;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				// no output
				case 'n':
					va_arg(argList, int*);
					break;

				default:
					//ASSERT(FALSE);  // unknown formatting option
					nMaxLen += 2;
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;	// add for blueant
	}

	char  *p_ssData;
	p_ssData = new char[nMaxLen+1];
	VERIFY(_vstprintf(p_ssData, lpszFormat, argListSave) <= nMaxLen+1);

	Output(p_ssData);	// ���

	delete[] p_ssData;

	va_end(argListSave);
}

//////////////////////////////////////////////////////////////////
// ��ȡ���������Ϣ
// �﷨����:
// 1.��ȡ�ȵ��к�: HOTITEM
// 2.��ȡѡ�������: SELITEMCOUNT
// 3.��ȡѡ�����: SELITEM ?nItem?
// 4.����ĳ��Ԫ�ص��к�: FINDITEM<nSubItem> item
// 5.��ȡ��Ԫ����: <nItem,nSubItem>
// 6.��ȡ���һ�β��������ڵ���: LASTITEM
// 7.��ȡĳ���ڵ������(�к�): ITEMINDEX
// 8.��ȡĳ���ڵ�ľ��: ITEMHANDLE
// 9.��ȡĳ���ڵ���ӽڵ����: ITEMCOUNT or CHILDCOUNT
// 10.��ȡĳ���ڵ�ĸ��ڵ���: ITEMPARENT
// 11.��ȡ����: COLUMNCOUNT
//////////////////////////////////////////////////////////////////
CString CTreeOutCtrl::GetInfo(CString strParam)
{
	CString strRet = _T("");
	CString strSet = strParam;
	strSet.MakeUpper();
	if(strSet.Find("HOTITEM") == 0)	// ��ȡ�ȵ��к�
	{
		strRet.Format("%d", GetFirstSelectedItem());
	}else
	if(strSet.Find("SELITEMCOUNT") == 0)	// ��ȡѡ����к�
	{
		int nCount = 0;
		int nItem = RVI_INVALID;
		while((nItem=GetNextSelectedItem(nItem)) != RVI_INVALID)
		{
			nCount++;
		}
		strRet.Format("%d", nCount);
	}else
	if(strSet.Find("SELITEM") == 0)	// ��ȡѡ����к�
	{
		strSet = strSet.Right(strSet.GetLength()-7);
		int nItem = -1;
		if(strSet.GetLength() > 0)
			nItem = atoi(strSet);
		if(nItem == -1)
			strRet.Format("%d", GetFirstSelectedItem());
		else
			strRet.Format("%d", GetNextSelectedItem(nItem));
	}else
	if(strSet.Find("FINDITEM<") == 0)	// ����ĳ��Ԫ�ص��к�
	{
		int nPos = strSet.Find("> ");
		if(nPos == -1)
			return _T("-1");
		strSet = strSet.Mid(9, nPos-9);
		int nSubItem = atoi(strSet);
		if(nSubItem<0)
			return _T("-1");
		strParam = strParam.Right(strParam.GetLength()-nPos-2);
		for(int i=0; i<GetItemCount(); i++)
		{
			if(GetItemText(i, nSubItem) == strParam)
			{
				strRet.Format("%d", i);
				return strRet;
			}
		}
		return _T("-1");
	}else
	if(strSet.Find("<") == 0)
	{
		int nPos = strSet.Find(">");
		if(nPos == -1)
			return _T("");
		strSet = strSet.Mid(1, nPos-1);
		nPos = strSet.Find(",");
		if(nPos == -1)
			return _T("");
		CString strItem = strSet.Left(nPos);
		int nItem = atoi(strItem);
		strSet = strSet.Right(strSet.GetLength()-nPos-1);
		int nSubItem = atoi(strSet);
		if(nItem>=0 && nItem<GetItemCount())
			strRet = GetItemText(nItem, nSubItem);
	}else
	if(strSet.Find("LASTITEM") == 0)	// ��ȡ���һ����ӵ����ڵ���
	{
		strRet.Format("%d", m_hLastItem);
	}else
	if(strSet.Find("ITEMINDEX ") == 0)	// ��ȡĳ���ڵ������(�к�)
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		HTREEITEM hItem = (HTREEITEM)(atoi(strSet));
		if(strSet == "ROOT")
			hItem = RVTI_ROOT;
		if(hItem != NULL)
		{
			strRet.Format("%d", GetItemIndex(hItem));
		}
	}else
	if(strSet.Find("ITEMHANDLE ") == 0)	// ��ȡĳ���ڵ�ľ��
	{
		strSet = strSet.Right(strSet.GetLength()-11);
		int nItem = atoi(strSet);
		strRet.Format("%d", GetItemHandle(nItem));
	}else
	if(strSet.Find("ITEMCOUNT ") == 0 || strSet.Find("CHILDCOUNT ") == 0)	// ��ȡĳ���ڵ���ӽڵ����
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		HTREEITEM hItem = (HTREEITEM)(atoi(strSet));
		if(strSet == "ROOT")
			hItem = RVTI_ROOT;
		if(hItem != NULL)
		{
			strRet.Format("%d", GetItemCount(hItem));
		}
	}else
	if(strSet.Find("ITEMPARENT ") == 0)	// ��ȡĳ���ڵ�ĸ��ڵ���
	{
		strSet = strSet.Right(strSet.GetLength()-11);
		HTREEITEM hItem = (HTREEITEM)(atoi(strSet));
		HTREEITEM hParentItem = (HTREEITEM)(GetNextItem(hItem, RVGN_PARENT));
		if(hParentItem == NULL)
			strRet = "ROOT";
		else
			strRet.Format("%d", hParentItem);
	}else
	if(strSet.Find("COLUMNCOUNT") == 0)	// ��ȡ����
	{
		strRet.Format("%d", GetActiveSubItemCount());
	}

	return strRet;
}

/////////////////////////////////////////////////////////////////////////////
// �������������
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::AddTreeItems(CTTreeItemArray*& pItems)
{
	if(pItems == NULL)
	{
		return;
	}

	// ����ɾ��������
	DeleteAllItems();
	m_asItemScript.RemoveAll();

	int nSepLen = m_strSeparator.GetLength();
	HTREEITEM hParentItem = RVTI_ROOT;

	for(int i=0; i<pItems->GetSize(); i++)
	{
		LPTTREEITEM pItem = &(pItems->GetAt(i));
		if(pItem == NULL)
		{
			continue;
		}

		if(pItem->nParent < 0)
		{
			hParentItem = RVTI_ROOT;
		}else
		if(pItem->nParent < pItems->GetSize())
		{
			LPTTREEITEM pParentItem = &(pItems->GetAt(pItem->nParent));
			hParentItem = (HTREEITEM)(pParentItem->hItem);
		}

		// ����������
		CString strItem = "";
		CString strOutput = pItem->strLine;
		int nPos = strOutput.Find(m_strSeparator);
		if(nPos != -1)
		{
			strItem = strOutput.Left(nPos);
			strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
		}else
		{
			strItem = strOutput;
		}
		pItem->hItem = InsertItem(strItem, pItem->nImage, -1, -1, hParentItem, RVTI_LAST);
		(*pItems)[i].hItem = pItem->hItem;
		if((*pItems)[i].hItem == NULL)
		{
			continue;
		}

		int nIndex = GetItemIndex((HTREEITEM)(pItem->hItem));
		if(nPos != -1)
		{
			int nCount = 1;
			while((nPos = strOutput.Find(m_strSeparator)) != -1)
			{
				strItem = strOutput.Left(nPos);
				strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
				if(nCount >= GetActiveSubItemCount())
					return;
				SetItemText(nIndex, nCount++, strItem);
			}
			if(nCount >= GetActiveSubItemCount())
				return;
			SetItemText(nIndex, nCount++, strOutput);
		}

		if(pItem->nClickFunc != 0)
		{
			SetItemData(nIndex, pItem->nClickFunc);
		}

		if(pItem->nExpand == 1)
		{
			// չ���ڵ�
			Expand((HTREEITEM)((*pItems)[i].hItem), RVE_EXPAND);
		}
	}

	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// �Ҽ��˵�
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CReportCtrl::OnRButtonDown(nFlags, point);

	// ��װCoolMenu
	CMenu* pMenu = new CMenu();
	pMenu->LoadMenu(IDR_OUTPUT_GRID);
	CMenu* pmenu = (CMenu*)pMenu->GetSubMenu(0);
	ClientToScreen(&point);
	BOOL bEnable = (GetItemCount() > 0);
	pmenu->EnableMenuItem(ID_OEDIT_CLEAR, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OUTCTRL_DELROW, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OFILE_SAVE, bEnable ? MF_ENABLED : MF_GRAYED);
	CXTPCommandBars::TrackPopupMenu(pmenu, 0, point.x, point.y, this);
	delete pMenu;
}

typedef void (CALLBACK_ITEM_CLICK)(LPVOID lpObj, LPCTSTR lpszParams);

/////////////////////////////////////////////////////////////////////////////
// ˫������
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CReportCtrl::OnLButtonDblClk(nFlags, point);

	int nIndex = -1;
	if(GetFirstSelectedItem() != RVI_INVALID)
	{
		nIndex = GetItemData(GetFirstSelectedItem());
	}
	if((nIndex > 0) && (nIndex <= m_asItemScript.GetSize()))
	{	// ִ��˫�������ű�
		CString strScriptCommand = m_asItemScript[nIndex-1];
		CString strInterpInstance = _T("");

		// ������������(֧��������ʽ�Ľ�����)
		if(strScriptCommand.Find("<<") == 0)
		{
			int nPos = strScriptCommand.Find(">>");
			if(nPos != -1)
			{
				strScriptCommand.Delete(0, 2);
				strInterpInstance = strScriptCommand.Left(nPos-2);
				strScriptCommand.Delete(0, nPos);
			}
		}

		if(strInterpInstance.Find("cfunction:") == 0)
		{
			// C�ص�����
			strInterpInstance.Delete(0, 10);
			CALLBACK_ITEM_CLICK* callback_item_click = (CALLBACK_ITEM_CLICK*)(atoi(strInterpInstance));
			int nPos1 = strScriptCommand.Find(",");
			if(nPos1 != -1)
			{
				LPVOID lpObj = (LPVOID)(atoi(strScriptCommand.Left(nPos1)));
				strScriptCommand.Delete(0, nPos1+1);
				CString strTmp;
				strTmp.Format("%d,", GetFirstSelectedItem());
				strScriptCommand = strTmp + strScriptCommand;
				(*callback_item_click)(lpObj, (LPCTSTR)strScriptCommand);
			}
		}else
		{
			IInterp* pInterp = (IInterp*)(theApp.CreateVciObject("org.interp.tcl", strInterpInstance));
			if(pInterp)
			{
				pInterp->RunScriptCommand(strScriptCommand);
				//CString strRes = pInterp->GetResult();
				theApp.ReleaseObject(pInterp);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ȫ�����
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::OnOeditClear() 
{
	DeleteAllItems();
}

void CTreeOutCtrl::OnUpdateOeditClear(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetItemCount() > 0);
	pCmdUI->Enable(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::OnOutctrlDelrow() 
{
	int nItem = GetFirstSelectedItem();
	if(nItem != RVI_INVALID)
	{
		HTREEITEM hItem = GetItemHandle(nItem);
		DeleteItem(hItem);
	}
}

void CTreeOutCtrl::OnUpdateOutctrlDelrow(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetFirstSelectedItem() != RVI_INVALID);
	pCmdUI->Enable(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// �鿴�ű�
/////////////////////////////////////////////////////////////////////////////
void CTreeOutCtrl::OnOutctrlScript() 
{
	int nIndex = -1;
	if(GetFirstSelectedItem() != RVI_INVALID)
	{
		nIndex = GetItemData(GetFirstSelectedItem());
	}
	if((nIndex > 0) && (nIndex <= m_asItemScript.GetSize()))
	{
		CString strScriptCommand = m_asItemScript[nIndex-1];
		CString strInterp = _T("");
		// ������������(֧��������ʽ�Ľ�����)
		if(strScriptCommand.Find("<<") == 0)
		{
			int nPos = strScriptCommand.Find(">>");
			if(nPos != -1)
			{
				strScriptCommand.Delete(0, 2);
				strInterp = strScriptCommand.Left(nPos-2);
				strScriptCommand.Delete(0, nPos);
			}
		}

		CString strMsg;
		PLAT_LOADSTRING(strInfoOutputScript, IDS_INFO_OUTPUT_SCRIPT); // ��������:%s\n�ű�:\n%s
		PLAT_LOADSTRING(strInfoOutputScriptTitle, IDS_INFO_OUTPUT_SCRIPT_TITLE); // ��Ԫ�ű�
		//CString strInfoOutputScript = "��������:%s\n�ű�:\n%s";
		//CString strInfoOutputScriptTitle = "��Ԫ�ű�";
		strMsg.Format(strInfoOutputScript, strInterp, strScriptCommand);
		::MessageBox(AfxGetMainWnd()->m_hWnd, strMsg, strInfoOutputScriptTitle, MB_OK|MB_ICONINFORMATION);
		/*
		TPopupControlInfo info;
		info.strCaption = strInfoOutputScriptTitle;
		info.strText = strMsg;
		info.strIcon = "info";
		info.nWidth = 600;
		info.nAddHight = 60;
		info.nShowDelay = 0;
		info.asButtonText.Add("OK");
		COM_CMD("", OC_POPUP_CONTROL, 0, (LPARAM)(&info));*/
	}
}

void CTreeOutCtrl::OnUpdateOutctrlScript(CCmdUI* pCmdUI) 
{
	int nIndex = -1;
	if(GetFirstSelectedItem() != RVI_INVALID)
	{
		nIndex = GetItemData(GetFirstSelectedItem());
	}

	pCmdUI->Enable((nIndex > 0) && (nIndex <= m_asItemScript.GetSize()));
}

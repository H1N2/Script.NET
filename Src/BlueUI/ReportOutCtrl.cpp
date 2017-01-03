// TreeOutCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "ReportOutCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportOutCtrl

CReportOutCtrl::CReportOutCtrl()
{
	m_pLastRecord = NULL;
	m_strSeparator = _T(",");
	m_bSetColor = FALSE;
}

CReportOutCtrl::~CReportOutCtrl()
{
}


BEGIN_MESSAGE_MAP(CReportOutCtrl, CXTPReportControl)
	//{{AFX_MSG_MAP(CReportOutCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_OEDIT_CLEAR, OnOeditClear)
	ON_COMMAND(ID_OUTCTRL_DELROW, OnOutctrlDelrow)
	ON_COMMAND(ID_OUTCTRL_SCRIPT, OnOutctrlScript)
	ON_UPDATE_COMMAND_UI(ID_OUTCTRL_SCRIPT, OnUpdateOutctrlScript)
	ON_UPDATE_COMMAND_UI(ID_OEDIT_CLEAR, OnUpdateOeditClear)
	ON_UPDATE_COMMAND_UI(ID_OUTCTRL_DELROW, OnUpdateOutctrlDelrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CReportOutCtrl message handlers

BOOL CReportOutCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	//dwStyle |= (ES_AUTOVSCROLL | ES_MULTILINE| ES_AUTOHSCROLL | ES_READONLY 
	//	| WS_BORDER | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL);
	dwStyle |= (ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_TABSTOP | WS_BORDER); 
	if(!CXTPReportControl::Create( dwStyle, rect, pParentWnd, nID))
		return FALSE;

	SetGridStyle(FALSE, (XTPReportGridStyle)xtpReportGridSmallDots);
	SetGridStyle(TRUE, (XTPReportGridStyle)xtpReportGridSmallDots);

	//GetPaintManager()->m_columnStyle = xtpReportColumnOffice2003;
	GetPaintManager()->SetColumnStyle(xtpReportColumnOffice2007);

	m_ilIcons.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1);

	CBitmap bmp;
	VERIFY(bmp.LoadBitmap(IDB_REPORT_CTRL));
	m_ilIcons.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));

	SetImageList(&m_ilIcons);

	// ��ʼ����ɫ��
	AppendColor(RGB(255,255,225));	// ����
	AppendColor(RGB(0,0,0));		// ��
	AppendColor(RGB(255,255,255));	// ��
	AppendColor(RGB(255,0,0));		// ��
	AppendColor(RGB(0,255,0));		// ��
	AppendColor(RGB(0,0,255));		// ��
	AppendColor(RGB(128,128,128));	// ��
	AppendColor(RGB(128,0,0));		// ���
	AppendColor(RGB(0,128,0));		// ����
	AppendColor(RGB(0,0,128));		// ����

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �����ɫ��
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::AppendColor(COLORREF color)
{
	m_aColorTable.Add(color);
}

/////////////////////////////////////////////////////////////////////////////
// ������ɫ��
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::SetColor(int nIndex, COLORREF color)
{
	if(nIndex < m_aColorTable.GetSize())
	{
		m_aColorTable.SetAt(nIndex, color);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ������ɫ��
/////////////////////////////////////////////////////////////////////////////
COLORREF CReportOutCtrl::GetColor(int nIndex)
{
	if(nIndex < m_aColorTable.GetSize())
	{
		return m_aColorTable.GetAt(nIndex);
	}
	return RGB(0,0,0);
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����������
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::Clear()
{
	// ɾ���������ڵ����
	GetRecords()->RemoveAll();
	GetColumns()->Clear();
	m_bSetColor = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// �رմ���ʱִ�еĽű�
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::DoCloseScript() 
{
	// ɾ���������ڵ����
	GetRecords()->RemoveAll();
	GetColumns()->Clear();

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
void CReportOutCtrl::ParseLines(CString strSet, CStringArray& aLines)
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
XTPReportGridStyle CReportOutCtrl::ParseStyle(CString strStyle)
{
	if(strStyle == _T("NOLINES"))
		return xtpReportGridNoLines;
	else if(strStyle == _T("SMALLDOTS"))
		return xtpReportGridSmallDots;
	else if(strStyle == _T("LARGEDOTS"))
		return xtpReportGridLargeDots;
	else if(strStyle == _T("DASHES"))
		return xtpReportGridDashes;
	else if(strStyle == _T("SOLID"))
		return xtpReportGridSolid;
	return xtpReportGridSolid;
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
// 8.�޸Ŀؼ����: MODIFYSTYLE vertical|horizontal style
// 9.��ע����ȡ����Ϣ: READPROFILE Section, Entry
// 10.��ע���д������Ϣ: WRITEPROFILE Section, Entry
// 11.���ô��ڹر�ʱִ�еĽű�: SETCLOSESCRIPT script
// 12.��������: SETFONT font size
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::ParseALine(CString strLine)
{
	CString strSet = strLine;
	strSet.MakeUpper();
	int pos;
	if((pos = strSet.Find("ADDCOLUMN ")) == 0)	// �����
	{
		CString strColName;
		CString strFormat = _T("");
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

		// ����AutoSize
		BOOL bAutoSize = (strFormat.Find("AUTOSIZE") == 0);
		if(bAutoSize)
		{
			strFormat.Delete(0, 8);
			if((strFormat.GetLength() > 0) && (strFormat.Find(",") == 0))
			{
				strFormat.Delete(0, 1);
			}
		}

		// �������뷽ʽ��ͼ��
		CString strImage = _T("");
		nPos = strFormat.Find(",");
		if(nPos != -1)
		{
			strImage = strFormat.Right(strFormat.GetLength()-nPos-1);
			strImage.MakeUpper();
			strFormat = strFormat.Left(nPos);
		}

		int nAlignment = DT_LEFT;
		if(strFormat == _T("CENTER"))
		{
			nAlignment = DT_CENTER;
		}else
		if(strFormat == _T("RIGHT"))
		{
			nAlignment = DT_RIGHT;
		}

		int nImage = XTP_REPORT_NOICON;
		if(strImage.GetLength() > 0)
		{
			nImage = atoi(strImage);
		}

		CXTPReportColumn* pColumn = AddColumn(new CXTPReportColumn(GetColumns()->GetCount(), strColName, nWidth, bAutoSize, nImage));
		if(pColumn)
		{
			pColumn->EnableResize(TRUE);
			pColumn->SetHeaderAlignment(nAlignment);
		}
	}else
	if((pos = strSet.Find("DELCOLUMN ")) == 0)	// ɾ����
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		if(strSet == _T("ALL"))
		{
			GetColumns()->Clear();
			m_asItemScript.RemoveAll();
		}else
		{
			int nCol = atoi(strSet);
			GetColumns()->Remove(GetColumns()->GetAt(nCol));
			if(GetColumns()->GetCount() == 0)
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
			GetRecords()->RemoveAll();
			m_asItemScript.RemoveAll();
		}else
		{
			int nItem = atoi(strSet);
			GetRecords()->RemoveAt(nItem);
			if(GetRecords()->GetCount() == 0)
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
		m_clText = cText;
		m_clBkGnd = cTextBk;
		m_bSetColor = TRUE;
		/*
		BeginUpdate();
		for(int i=0; i<GetRows()->GetCount(); i++)
		{
			CXTPReportRow* pRow = GetRows()->GetAt(i);
			CXTPReportRecord* pRecord = pRow->GetRecord();
			for(int j=0; j<pRecord->GetItemCount(); j++)
			{
				CXTPReportRecordItem* pItem = pRecord->GetItem(j);
				if(pItem)
				{
					pItem->SetTextColor(cText);
					pItem->SetBackgroundColor(cTextBk);
				}
			}
		}
		EndUpdate();
		*/
	}else
	if((pos = strSet.Find("SETGRIDCOLOR ")) == 0)	// ���ñ������ɫ
	{
		strSet = strSet.Right(strSet.GetLength()-13);
		COLORREF cGrid = RGB(0,0,0);
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
			cGrid = RGB(nr, ng, nb);
		}
		SetGridColor(cGrid);
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
		BOOL bVertical = TRUE;
		if(strSet.Find("VERTICAL ") == 0)
		{
			strSet.Delete(0, 9);
		}else
		if(strSet.Find("HORIZONTAL ") == 0)
		{
			bVertical = FALSE;
			strSet.Delete(0, 11);
		}
		SetGridStyle(bVertical, ParseStyle(strSet));
	}else
	if((pos = strSet.Find("SETTREECOLUMN ")) == 0)	// ����������
	{
		strSet = strSet.Right(strSet.GetLength()-14);
		CXTPReportColumn* pColumn = GetColumns()->Find(atoi(strSet));
		if(pColumn)
		{
			pColumn->SetTreeColumn(TRUE);
		}
	}else
		/*
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
	}else*/
	if(strSet.Find("SETCLOSESCRIPT ") == 0)	// ���ô��ڹر�ʱִ�еĽű�
	{
		m_strScriptClose = strLine.Right(strLine.GetLength()-15);
	}
}

//////////////////////////////////////////////////////////////////
// ���������ʽ
//////////////////////////////////////////////////////////////////
void CReportOutCtrl::SetCtrlFormat(CString strSet)
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
// ��ȡָ�����Record����ָ��
// itemǰ��#��ʾ���������ָ��
//////////////////////////////////////////////////////////////////
CXTPReportRecord* CReportOutCtrl::GetItemRecord(CString strItem)
{
	if(strItem.Find("#") == 0)
	{
		strItem.Delete(0, 1);
		return (CXTPReportRecord*)(atoi(strItem));
	}else
	{
		int nItem = atoi(strItem);
		if((nItem >= 0) && (nItem < GetRows()->GetCount()))
		{
			CXTPReportRecord* pRecord = GetRows()->GetAt(nItem)->GetRecord();
			return pRecord;
		}
	}
	return NULL;
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
void CReportOutCtrl::Output(CString strOutput)
{
	if(strOutput == "\r\n")
	{
		return;
	}

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
			CXTPReportRecord* pRecord = GetItemRecord(strItem);
			if(!pRecord)
				return;

			strSet = strSet.Right(strSet.GetLength()-nPos-1);
			int nSubItem = atoi(strSet);
			if(nSubItem >= GetColumns()->GetVisibleColumnsCount())
				return;

			while(pRecord->GetItemCount() < (nSubItem+1))
			{
				pRecord->AddItem(new CXTPReportRecordItemText(""));
			}

			strSet = strOutput;
			strSet.MakeUpper();
			if(strSet.Find("TEXT ") == 0)	// ���õ�������
			{
				strOutput = strOutput.Right(strOutput.GetLength()-5);
				CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
				pItem->SetCaption(strOutput);
				if(m_bSetColor)
				{
					pItem->SetTextColor(m_clText);
					pItem->SetBackgroundColor(m_clBkGnd);
				}
			}else
			if(strSet.Find("IMAGE ") == 0)	// ���õ���ͼ��
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				int nImage = atoi(strOutput);
				CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
				if(pItem)
				{
					pItem->SetIconIndex(nImage);
				}
			}else
			if(strSet.Find("CHECK ") == 0)	// ���õ�������
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				int nCheck = atoi(strOutput);
				CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
				if(pItem)
				{
					pItem->HasCheckbox(TRUE);
					pItem->SetChecked(nCheck);
				}
			}else
			if(strSet.Find("COLOR ") == 0)	// ����������ɫ
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				int nColor = atoi(strOutput);	// ��ɫ�������

				if(nSubItem == -1)	// -1��ʾ�������е���ɫ
				{
					for(int i=0; i<pRecord->GetItemCount(); i++)
					{
						CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
						if(pItem)
						{
							pItem->SetTextColor(GetColor(nColor));
						}
					}
				}else
				{
					CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
					if(pItem)
					{
						pItem->SetTextColor(GetColor(nColor));
					}
				}
			}else
			if(strSet.Find("STATE ") == 0)	// ��������״̬
			{
				strOutput = strOutput.Right(strOutput.GetLength()-6);
				strOutput.MakeUpper();
				BOOL bBold = FALSE;
				if(strOutput == "BOLD")
					bBold = TRUE;
				if(nSubItem == -1)	// -1��ʾ�������е�״̬
				{
					for(int i=0; i<pRecord->GetItemCount(); i++)
					{
						CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
						if(pItem)
						{
							pItem->SetBold(bBold);
						}
					}
				}else
				{
					CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
					if(pItem)
					{
						pItem->SetBold(bBold);
					}
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
				CXTPReportRecord* pRecord = GetItemRecord(strItem);
				if(pRecord)
				{
					CXTPReportRecordItem* pItem = pRecord->GetItem(0);
					if(pItem)
					{
						pItem->SetItemData(atoi(strSet));
					}
				}
				
				return;
			}
			// ��ӽű�,��������(ע:���õ��к������Ǵ�1��ʼ������,��ΪItemDataȱʡΪ0)
			m_asItemScript.Add(strSet);
			CXTPReportRecord* pRecord = GetItemRecord(strItem);
			if(pRecord)
			{
				CXTPReportRecordItem* pItem = pRecord->GetItem(0);
				if(pItem)
				{
					pItem->SetItemData(m_asItemScript.GetSize());
				}
			}
		}/*else
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
		}*/
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
			if(strSet == "ROOT")
			{
				ExpandAll();
			}else
			{
				CXTPReportRecord* pRecord = GetItemRecord(strSet);
				if(pRecord)
				{
					pRecord->SetExpanded(TRUE);
				}
			}
		}
	}else	// �������
	{
		CString strItem = _T("");
		int nSepLen = m_strSeparator.GetLength();
		int nPos;
		int nImage = -1;
		CXTPReportRecord* pParentRecord = NULL;
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
				if(strParam != "ROOT")
				{
					pParentRecord = (CXTPReportRecord*)(atoi(strParam));
				}
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
		m_pLastRecord = new CXTPReportRecord();
		m_pLastRecord->AddItem(new CXTPReportRecordItemText(strItem));
		if(m_bSetColor)
		{
			m_pLastRecord->GetItem(0)->SetTextColor(m_clText);
			m_pLastRecord->GetItem(0)->SetBackgroundColor(m_clBkGnd);
		}
		if(nImage != -1)
		{
			m_pLastRecord->GetItem(0)->SetIconIndex(nImage);
		}
		
		if(nPos != -1)
		{
			int nCount = 1;
			while((nPos = strOutput.Find(m_strSeparator)) != -1)
			{
				strItem = strOutput.Left(nPos);
				strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
				if(nCount >= GetColumns()->GetCount())
					break;
				m_pLastRecord->AddItem(new CXTPReportRecordItemText(strItem));
				if(m_bSetColor)
				{
					m_pLastRecord->GetItem(nCount)->SetTextColor(m_clText);
					m_pLastRecord->GetItem(nCount)->SetBackgroundColor(m_clBkGnd);
				}
				nCount++;
			}
			if(nCount < GetColumns()->GetCount())
			{
				m_pLastRecord->AddItem(new CXTPReportRecordItemText(strOutput));
				if(m_bSetColor)
				{
					m_pLastRecord->GetItem(nCount)->SetTextColor(m_clText);
					m_pLastRecord->GetItem(nCount)->SetBackgroundColor(m_clBkGnd);
				}
			}
		}

		if(pParentRecord == NULL)
		{
			AddRecord(m_pLastRecord);
		}else
		{
			pParentRecord->GetChilds()->Add(m_pLastRecord);
		}
	}

	// Populate all data on control
	Populate();
}

/////////////////////////////////////////////////////////////////////////////
// ��ʽ�����
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::vsWrite(LPCTSTR lpszFormat, va_list argList)
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
// 1.��ȡ�ȵ��о��: HOTITEM
// 2.��ȡѡ�������: SELITEMCOUNT
// 3.��ȡѡ�����: SELITEM ?nItem?
// 4.����ĳ��Ԫ�ص��к�: FINDITEM<nSubItem> item
// 5.��ȡ��Ԫ����: <nItem,nSubItem>
// 6.��ȡ���һ�β��������ڵ��о��(Record����): LASTITEM
// 7.��ȡĳ���ڵ������(�к�): ITEMINDEX
// 8.��ȡĳ���ڵ�ľ��: ITEMHANDLE
// 9.��ȡĳ���ڵ���ӽڵ����: ITEMCOUNT
// 10.��ȡĳ���ڵ�ĸ��ڵ���: ITEMPARENT
// 11.��ȡ����: COLUMNCOUNT
//////////////////////////////////////////////////////////////////
CString CReportOutCtrl::GetInfo(CString strParam)
{
	CString strRet = _T("");
	CString strSet = strParam;
	strSet.MakeUpper();
	if(strSet.Find("HOTITEM") == 0)	// ��ȡ�ȵ��о��
	{
		CXTPReportRow* pRow = GetFocusedRow();
		/*int nIndex = -1;
		if(pRow)
		{
			nIndex = pRow->GetIndex();
		}
		strRet.Format("%d", nIndex);*/
		if(pRow)
		{
			CXTPReportRecord* pRecord = pRow->GetRecord();
			strRet.Format("%d", pRecord);
		}
	}else
	if(strSet.Find("SELITEMCOUNT") == 0)	// ��ȡѡ�������
	{
		int nCount = GetSelectedRows()->GetCount();
		strRet.Format("%d", nCount);
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
		strSet = strSet.Right(strSet.GetLength()-nPos-1);
		int nSubItem = atoi(strSet);
		CXTPReportRecord* pRecord = GetItemRecord(strItem);
		if(pRecord)
		{
			CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
			if(pItem)
			{
				strRet = pItem->GetCaption(GetColumns()->GetAt(nSubItem));
			}
		}
	}else
	if(strSet.Find("PARENTROW ") == 0)	// ��ȡĳ���ڵ�ĸ��ڵ��к�
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		int nItem = atoi(strSet);
		if(nItem>=0 && nItem<GetRows()->GetCount())
		{
			CXTPReportRow* pRow = GetRows()->GetAt(nItem);
			if(pRow && pRow->GetParentRow())
			{
				strRet.Format("%d", pRow->GetParentRow()->GetIndex());
			}
		}
	}else
	if(strSet.Find("CHILDCOUNT ") == 0)	// ��ȡĳ���ڵ���ӽڵ����
	{
		strRet = "0";
		strSet = strSet.Right(strSet.GetLength()-11);
		CXTPReportRecord* pRecord = (CXTPReportRecord*)(atoi(strSet));
		if(pRecord)
		{
			strRet.Format("%d", pRecord->GetChilds()->GetCount());
		}
	}else
	if(strSet.Find("LASTITEM") == 0)	// ��ȡ���һ����ӵ����ڵ���Record����
	{
		if(m_pLastRecord)
		{
			strRet.Format("%d", m_pLastRecord);
		}
	}else
	if(strSet.Find("SELITEM") == 0)	// ��ȡѡ����о��
	{
		strSet = strSet.Right(strSet.GetLength()-7);
		int nItem = 0;
		if(strSet.GetLength() > 0)
			nItem = atoi(strSet);
		CXTPReportSelectedRows* pSelectedRows = GetSelectedRows();
		if(pSelectedRows && (pSelectedRows->GetCount() > 0))
		{
			CXTPReportRow* pFocusedRow = pSelectedRows->GetAt(nItem);
			if(pFocusedRow)
			{
				strRet.Format("%d", pFocusedRow->GetRecord());
			}
		}
	}else
	if(strSet.Find("ITEMHANDLE ") == 0)	// ��ȡĳ���ڵ��еľ��
	{
		strSet = strSet.Right(strSet.GetLength()-11);
		int nIndex = atoi(strSet);
		CXTPReportRow* pRow = GetRows()->GetAt(nIndex);
		if(pRow)
		{
			CXTPReportRecord* pRecord = pRow->GetRecord();
			strRet.Format("%d", pRecord);
		}
	}else
	if(strSet.Find("ITEMINDEX ") == 0)	// ��ȡĳ���ڵ������(�к�)
	{
		strSet = strSet.Right(strSet.GetLength()-10);
		CXTPReportRecord* pRecord = (CXTPReportRecord*)(atoi(strSet));
		for(int i=0; i<GetRows()->GetCount(); i++)
		{
			if(GetRows()->GetAt(i)->GetRecord() == pRecord)
			{
				strRet.Format("%d", i);
			}
		}
	}else
	if(strSet.Find("ITEMPARENT ") == 0)	// ��ȡĳ���ڵ�ĸ��ڵ���
	{
		strSet = strSet.Right(strSet.GetLength()-11);
		CXTPReportRecord* pRecord = (CXTPReportRecord*)(atoi(strSet));
		if(pRecord)
		{
			CXTPReportRecord* pParentRecord = pRecord->GetRecords()->GetOwnerRecord();
			if(pParentRecord)
			{
				strRet.Format("%d", pParentRecord);
			}else
			{
				strRet = "ROOT";
			}
		}
	}else
	if(strSet.Find("FINDITEM<") == 0)	// ����ĳ��Ԫ�ص��к�
	{
		int nPos = strSet.Find("> ");
		if(nPos == -1)
			return _T("");
		strSet = strSet.Mid(9, nPos-9);
		int nSubItem = atoi(strSet);
		if(nSubItem<0)
			return _T("");
		strParam = strParam.Right(strParam.GetLength()-nPos-2);
		for(int i=0; i<GetRows()->GetCount(); i++)
		{
			CXTPReportRow* pRow = GetRows()->GetAt(i);
			if(pRow)
			{
				CXTPReportRecord* pRecord = pRow->GetRecord();
				if(pRecord)
				{
					CXTPReportRecordItem* pItem = pRecord->GetItem(nSubItem);
					if(pItem)
					{
						if(pItem->GetCaption(GetColumns()->GetAt(nSubItem)) == strParam)
						{
							strRet.Format("%d", i);
							return strRet;
						}
					}
				}
			}
		}
		return _T("");
	}else
	if(strSet.Find("COLUMNCOUNT") == 0)	// ��ȡ����
	{
		strRet.Format("%d", GetColumns()->GetCount());
	}

	return strRet;
}

/////////////////////////////////////////////////////////////////////////////
// �������������
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::AddTreeItems(CTTreeItemArray*& pItems)
{
	if(pItems == NULL)
	{
		return;
	}

	// ����ɾ��������
	GetRecords()->RemoveAll();
	m_asItemScript.RemoveAll();

	int nSepLen = m_strSeparator.GetLength();
	CXTPReportRecord* pParentRecord = NULL;

	for(int i=0; i<pItems->GetSize(); i++)
	{
		LPTTREEITEM pItem = &(pItems->GetAt(i));
		if(pItem == NULL)
		{
			continue;
		}

		if(pItem->nParent < 0)
		{
			pParentRecord = NULL;
		}else
		if((pItem->nParent < pItems->GetSize()) && (pItem->nParent < i))
		{
			pParentRecord = (CXTPReportRecord*)((*pItems)[pItem->nParent].hItem);
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
		CXTPReportRecord* pItemRow = new CXTPReportRecord();
		pItemRow->AddItem(new CXTPReportRecordItemText(strItem));
		pItem->hItem = pItemRow;
		(*pItems)[i].hItem = pItem->hItem;	// ���ṹ��hItem�����ӵ�Record
		if((*pItems)[i].hItem == NULL)
		{
			continue;
		}
		/*if(m_bSetColor)
		{
			pItemRow->GetItem(0)->SetTextColor(m_clText);
			pItemRow->GetItem(0)->SetBackgroundColor(m_clBkGnd);
		}*/
		if(pItem->nImage != -1)
		{
			pItemRow->GetItem(0)->SetIconIndex(pItem->nImage);
		}
		if(pItem->nClickFunc != 0)
		{
			CXTPReportRecordItem* pRecordItem = pItemRow->GetItem(0);
			if(pRecordItem)
			{
				pRecordItem->SetItemData(pItem->nClickFunc);
			}
		}

		if(nPos != -1)
		{
			int nCount = 1;
			while((nPos = strOutput.Find(m_strSeparator)) != -1)
			{
				strItem = strOutput.Left(nPos);
				strOutput = strOutput.Right(strOutput.GetLength()-nPos-nSepLen);
				if(nCount >= GetColumns()->GetCount())
					break;
				pItemRow->AddItem(new CXTPReportRecordItemText(strItem));
				if(m_bSetColor)
				{
					pItemRow->GetItem(nCount)->SetTextColor(m_clText);
					pItemRow->GetItem(nCount)->SetBackgroundColor(m_clBkGnd);
				}
				nCount++;
			}
			if(nCount < GetColumns()->GetCount())
			{
				pItemRow->AddItem(new CXTPReportRecordItemText(strOutput));
				/*if(m_bSetColor)
				{
					m_pLastRecord->GetItem(nCount)->SetTextColor(m_clText);
					m_pLastRecord->GetItem(nCount)->SetBackgroundColor(m_clBkGnd);
				}*/
			}
		}

		if(pParentRecord == NULL)
		{
			AddRecord(pItemRow);
		}else
		{
			pParentRecord->GetChilds()->Add(pItemRow);
			pParentRecord->SetExpanded(TRUE);
		}
	}

	// ��������ʾ����
	Populate();
}

typedef void (CALLBACK_ITEM_CLICK)(LPVOID lpObj, LPCTSTR lpszParams);

//////////////////////////////////////////////////////////////////
// ���˫���¼�����
//////////////////////////////////////////////////////////////////
void CReportOutCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CXTPReportControl::OnLButtonDblClk(nFlags, point);

	CXTPReportRow* pRow = HitTest(point);
	if(pRow == NULL)
	{
		return;
	}

	CXTPReportRecord* pActiveRecord = pRow->GetRecord();
	if(pActiveRecord == NULL)
	{
		return;
	}

	CXTPReportRecordItem* pItem = pActiveRecord->GetItem(0);
	if(pItem == NULL)
	{
		return;
	}

	int nIndex = pItem->GetItemData();
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
				strTmp.Format("%d,", pRow->GetIndex());
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
				//TRACE(strRes);
				theApp.ReleaseObject(pInterp);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// �Ҽ��˵�
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// ��װCoolMenu
	CMenu* pMenu = new CMenu();
	pMenu->LoadMenu(IDR_OUTPUT_GRID);
	CMenu* pmenu = (CMenu*)pMenu->GetSubMenu(0);
	ClientToScreen(&point);
	BOOL bEnable = (GetRecords()->GetCount() > 0);
	pmenu->EnableMenuItem(ID_OEDIT_CLEAR, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OUTCTRL_DELROW, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OFILE_SAVE, bEnable ? MF_ENABLED : MF_GRAYED);
	//pmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
	//		point.x, point.y, this);
	CXTPCommandBars::TrackPopupMenu(pmenu, 0, point.x, point.y, this);
	delete pMenu;
}

/////////////////////////////////////////////////////////////////////////////
// ȫ�����
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::OnOeditClear() 
{
	BeginUpdate();
	GetRows()->Clear();
	EndUpdate();
}

void CReportOutCtrl::OnUpdateOeditClear(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetRows()->GetCount() > 0);
	pCmdUI->Enable(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// ɾ����
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::OnOutctrlDelrow() 
{
	CXTPReportRow* pRow = GetFocusedRow();
	int nIndex = -1;
	if(pRow)
	{
		BeginUpdate();
		nIndex = pRow->GetIndex();
		GetRows()->RemoveAt(nIndex);
		EndUpdate();
	}
}

void CReportOutCtrl::OnUpdateOutctrlDelrow(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetRows()->GetCount() > 0);
	pCmdUI->Enable(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// �鿴�ű�
/////////////////////////////////////////////////////////////////////////////
void CReportOutCtrl::OnOutctrlScript() 
{
	CXTPReportRow* pRow = GetFocusedRow();
	int nIndex = -1;
	if(pRow)
	{
		CXTPReportRecord* pRecord = pRow->GetRecord();
		if(pRecord)
		{
			CXTPReportRecordItem* pItem = pRecord->GetItem(0);
			if(pItem)
			{
				nIndex = pItem->GetItemData();
			}
		}
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
		IInterp* pInterp = (IInterp*)(theApp.CreateVciObject("org.interp.tcl", ""));
		if(pInterp)
		{
			CString strCommand;
			strCommand.Format("proc viewscript {} {\n%s\n}", strScriptCommand);
			pInterp->RunScriptCommand(strCommand);
			CString strFile = theApp.GetPlatRootPath() + "\\conf\\menu\\viewscript.tcl";
			pInterp->RunScriptFile(strFile);
			theApp.ReleaseObject(pInterp);
		}
*/
	}
}

void CReportOutCtrl::OnUpdateOutctrlScript(CCmdUI* pCmdUI) 
{
	CXTPReportRow* pRow = GetFocusedRow();
	int nIndex = -1;
	if(pRow)
	{
		CXTPReportRecord* pRecord = pRow->GetRecord();
		if(pRecord)
		{
			CXTPReportRecordItem* pItem = pRecord->GetItem(0);
			if(pItem)
			{
				nIndex = pItem->GetItemData();
			}
		}
	}

	pCmdUI->Enable((nIndex > 0) && (nIndex <= m_asItemScript.GetSize()));
}

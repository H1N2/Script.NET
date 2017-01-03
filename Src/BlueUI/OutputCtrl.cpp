// OutputCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BlueUI.h"
#include "OutputCtrl.h"
#include "TreeOutCtrl.h"
#include "ReportOutCtrl.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputCtrl

COutputCtrl::COutputCtrl()
{
	dwMaxLine	= 10000;
	m_Color		= RGB(0, 0, 0);
	m_Effect	= 0;
	m_Height	= 200;
	strcpy(m_FaceName, "\0");
	m_pParentBar= NULL;
}

COutputCtrl::~COutputCtrl()
{
}

IMPLEMENT_DYNAMIC(COutputCtrl, CRichEditCtrl)

BEGIN_MESSAGE_MAP(COutputCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(COutputCtrl)
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_OEDIT_COPY, OnOeditCopy)
	ON_COMMAND(ID_OEDIT_CLEAR, OnOeditClear)
	ON_COMMAND(ID_OFILE_SAVE, OnOfileSave)
	ON_COMMAND(ID_OEDIT_SELECTALL, OnOeditSelectall)
	ON_UPDATE_COMMAND_UI(ID_OEDIT_COPY, OnUpdateOeditCopy)
	ON_UPDATE_COMMAND_UI(ID_OEDIT_CLEAR, OnUpdateOeditClear)
	ON_UPDATE_COMMAND_UI(ID_OFILE_SAVE, OnUpdateOfileSave)
	ON_UPDATE_COMMAND_UI(ID_OEDIT_SELECTALL, OnUpdateOeditSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputCtrl message handlers
BOOL COutputCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= (ES_AUTOVSCROLL | ES_MULTILINE| ES_AUTOHSCROLL | ES_READONLY 
		| WS_BORDER | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL);
	if(!CRichEditCtrl::Create( dwStyle, rect, pParentWnd, nID))
		return FALSE;

	GetDefaultCharFormat(m_fmtCur);	// ��ȡȱʡ������Ϣ

	ResetFormat();					// ��ʼ������

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ����������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::Clear()
{
	int len = GetWindowTextLength();
	SetSel(0,len);
	ReplaceSel("");
}

/////////////////////////////////////////////////////////////////////////////
// ���ָ����
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::ClearLineFrom(INT dlineFrom, INT dlineTo)
{
	int Num1 = LineIndex(dlineFrom);
	int Num2 = LineIndex(dlineTo+1);
	SetSel(Num1,Num2);
	ReplaceSel("");
}

/////////////////////////////////////////////////////////////////////////////
// ���������������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetMaxLine(DWORD dwLine)
{
	dwMaxLine = dwLine;
	if( dwMaxLine < 100)
		dwMaxLine = 100;
	ArrangeLine();
}

/////////////////////////////////////////////////////////////////////////////
// ����������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::ArrangeLine()
{
	INT TotalLine;
	TotalLine = GetLineCount();
	if( TotalLine > (INT)dwMaxLine ) {

		TotalLine -= dwMaxLine;
		ClearLineFrom( 0 , TotalLine-1 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// ���
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::vsWrite(LPCTSTR lpszFormat, va_list argList)
{
	ASSERT(AfxIsValidString(lpszFormat, FALSE));

	if(!::IsWindow(m_hWnd))
	{
		return;
	}

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

	SetSel(-1,-1);			// ѡ�����
	SetCurFormat();			// ��������
	ReplaceSel(p_ssData);	// ��ѡ�񲿷ֽ����滻

	delete[] p_ssData;

	UpdateWindow();

	va_end(argListSave);
	
	ArrangeLine();			// ɾ���������

	// �Զ�����
	CDC* pDC =  GetWindowDC( );
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	RECT  sRect;
	GetRect( &sRect );
	ReleaseDC( pDC );
	int nWndLns = (sRect.bottom - sRect.top) / tm.tmAscent;
	int nCurLine= GetFirstVisibleLine( );
	long lScrLns = GetLineCount( ) - nCurLine;
	if(lScrLns>nWndLns)
		LineScroll( lScrLns-nWndLns );  //added by wpr to scroll to the newest line
}

/////////////////////////////////////////////////////////////////////////////
// ֱ�����,�޸�ʽ��
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::WriteDirect(LPCTSTR lpszOutput)
{
	SetSel(-1,-1);			// ѡ�����
	SetCurFormat();			// ��������
	ReplaceSel(lpszOutput);	// ��ѡ�񲿷ֽ����滻

	UpdateWindow();
	
	ArrangeLine();			// ɾ���������

	// �Զ�����
	CDC* pDC =  GetWindowDC( );
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	RECT  sRect;
	GetRect( &sRect );
	ReleaseDC( pDC );
	int nWndLns = (sRect.bottom - sRect.top) / tm.tmAscent;
	int nCurLine= GetFirstVisibleLine( );
	long lScrLns = GetLineCount( ) - nCurLine;
	if(lScrLns>nWndLns)
		LineScroll( lScrLns-nWndLns );  //added by wpr to scroll to the newest line
}

/*
/////////////////////////////////////////////////////////////////////////////
// �޸�һ�е���ɫ
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetLineFormat(int nFmt)
{
	long nCharNumber  = 0, nLineIndex = 0;

	for (int i = 0; i < nLineNO - 1; i++)
	{
		//�õ�ÿ�е�������
		nLineIndex = pEditCtrl->LineIndex(i); 

		//�ۼ��ַ���
		nCharNumber = 2 + nCharNumber + pEditCtrl->LineLength(nLineIndex);
	}

	//��λ��굽ָ����
	SetSel(nCharNumber, nCharNumber);

	//�����ɿ�ʼ��ָ�������׵��ַ���
	long nStartChar = LineIndex(-1);;

	//�����ɿ�ʼ��ָ������β���ַ���
	long nEndChar = nStartChar + LineLength(-1);

	HideSelection(TRUE, FALSE);
	//�����ɿ�ʼ��ָ����������β���ַ���ѡ������
	SetSel(nStartChar,nEndChar);

	//����ѡ���е��ַ�Ϊ��ɫ
	CHARFORMAT cf;
	GetSelectionCharFormat(cf);

	cf.crTextColor = RGB(255,0,0);
	cf.dwMask = CFM_COLOR;
	SetSelectionCharFormat(cf);

	SetCursorLineNO(nOldCursorLine);
	HideSelection(FALSE, FALSE);
}
*/
/////////////////////////////////////////////////////////////////////////////
// ������ɫ����
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetFormat(int nFmt)
{
	m_fmtCur.dwMask |= CFM_COLOR;
	m_fmtCur.dwEffects &= ~CFE_AUTOCOLOR ;
	switch(nFmt)
	{
	case 0:m_fmtCur.crTextColor = RGB( 0 , 0, 0 );break;
	case 1:m_fmtCur.crTextColor = RGB( 0 , 0, 255 );break;
	case 2:m_fmtCur.crTextColor = RGB( 255 , 0, 0 );break;
	case 3:m_fmtCur.crTextColor = RGB( 0 , 255, 0 );break;
	case 4:m_fmtCur.crTextColor = RGB( 128 , 128, 128 );break;
	default:m_fmtCur.crTextColor = RGB( 0 , 0, 0 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// ����������ɫ
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetTextColor(COLORREF color)
{
	m_fmtCur.dwMask |= CFM_COLOR;
	m_fmtCur.dwEffects &= ~CFE_AUTOCOLOR ;
	m_fmtCur.crTextColor = color;
}

/////////////////////////////////////////////////////////////////////////////
// ��������Ϊ��ǰ����
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetCurFormat()
{
	SetSelectionCharFormat(m_fmtCur);
}

/////////////////////////////////////////////////////////////////////////////
// ��ʼ������(����Ϊȱʡֵ)
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::ResetFormat()
{
	CString strSubKey;
	strSubKey.Format("%s\\%s", REG_FORMAT_SUBKEY, REG_FMT_OUTPUT);
	// ������
	CString strFont = AfxGetApp()->GetProfileString(strSubKey, REG_FMFONT, "FixedSys");
	// �����С
	int nSize = AfxGetApp()->GetProfileInt(strSubKey, REG_FMSIZE, 16);
	// ǰ��ɫ
	COLORREF colorFg = AfxGetApp()->GetProfileInt(strSubKey, REG_FMCOLORFG, RGB(0,0,0));
	// ����ɫ
	COLORREF colorBk = AfxGetApp()->GetProfileInt(strSubKey, REG_FMCOLORBK, RGB(255,255,255));
	// �Ƿ����
	BOOL bBold = AfxGetApp()->GetProfileInt(strSubKey, REG_FMBOLD, FALSE);
	// �Ƿ�б��
	BOOL bItalic = AfxGetApp()->GetProfileInt(strSubKey, REG_FMITALIC, FALSE);
	// �Ƿ��»���
	BOOL bUnderLine = AfxGetApp()->GetProfileInt(strSubKey, REG_FMUNDERLINE, FALSE);
	// �Ƿ�ɾ����
	BOOL bStrikeOut = AfxGetApp()->GetProfileInt(strSubKey, REG_FMSTRIKEOUT, FALSE);
	// �����������ô�
	CString strFmt;
	strFmt.Format("FONT=%s;HEIGHT=%d;COLOR=(%d,%d,%d);", strFont, nSize*12,
				GetRValue(colorFg), GetGValue(colorFg), GetBValue(colorFg));
	CString strEffect = "EFFECT=";
	if(bBold)	strEffect += "BOLD,";
	if(bItalic)	strEffect += "ITALIC,";
	if(bUnderLine)	strEffect += "UNDERLINE,";
	if(bStrikeOut)	strEffect += "STRIKEOUT";
	strFmt += strEffect;
	// �������嵽��ǰ���������
	SetCtrlFormat(strFmt);
}

/////////////////////////////////////////////////////////////////////////////
// �������ô�����������Ϣ����ǰ���������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SetCtrlFormat(LPCTSTR lpszSet)
{
	CString strSet = lpszSet;
	CStringArray aLines;
	// ��ֳ�������
	ParseLines(strSet, aLines);
	for(int i=0; i<aLines.GetSize(); i++)
	{
		CString strLine = aLines[i];
		strLine.MakeUpper();
		ParseALine(strLine);	// ������
	}

	// ���õ���ǰ���������
	m_fmtCur.dwMask |= CFM_CHARSET;	// char set
	m_fmtCur.dwMask |= CFM_COLOR;
	m_fmtCur.dwMask |= CFM_SIZE;
	m_fmtCur.dwMask |= CFM_FACE;
	m_fmtCur.dwMask |= CFM_BOLD;
	m_fmtCur.dwMask |= CFM_ITALIC;
	m_fmtCur.dwMask |= CFM_STRIKEOUT;
	m_fmtCur.dwMask |= CFM_UNDERLINE;
	m_fmtCur.dwEffects = 0;
	m_fmtCur.dwEffects &= ~CFE_AUTOCOLOR;
	m_fmtCur.dwEffects |= m_Effect;
	m_fmtCur.crTextColor = m_Color;
	m_fmtCur.yHeight = m_Height;
	if(m_FaceName[0] != 0)
	{
		strcpy(m_fmtCur.szFaceName, m_FaceName);
	}
	m_fmtCur.bCharSet = GB2312_CHARSET;	// char set
}

/////////////////////////////////////////////////////////////////////////////
// ��ֳ�������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::ParseLines(CString strSet, CStringArray& aLines)
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
// ����������
// �﷨����
// 1.���������ɸ��������";"����
// 2.������ɫ���﷨��: COLOR=R,G,B
// 3.���ø߶ȵ��﷨��: HEIGHT=�߶�
// 4.����Ч�����﷨������: EFFECT=Ч��1,Ч��2,...
//                         Ч��������: BOLD       - ����
//                                     ITALIC     - б��
//                                     STRIKEOUT  - ɾ����
//                                     UNDERLINE  - �»���
//                                     NOEFFECT   - û��Ч��
// 5.����������﷨������: FONT=������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::ParseALine(CString strLine)
{
	CString strSet, strTmp;
	int pos;
	if((pos = strLine.Find("COLOR=")) == 0)
	{
		strSet = strLine.Right(strLine.GetLength()-pos-6);
		int nPos = strSet.Find(",");
		strTmp = strSet.Left(nPos);
		int nr = atoi(strTmp);
		strSet = strSet.Mid(nPos + 1, strSet.GetLength() - nPos -1);
		nPos = strSet.Find(",");
		strTmp = strSet.Left(nPos);
		int ng = atoi(strTmp);
		strTmp = strSet.Mid(nPos + 1, strSet.GetLength() - nPos -1);
		int nb = atoi(strTmp);
		m_Color = RGB(nr, ng, nb);
		return;
	}

	if((pos = strLine.Find("HEIGHT=")) == 0)
	{
		strSet = strLine.Right(strLine.GetLength()-pos-7);
		m_Height = atoi(strSet);
		return;
	}

	if((pos = strLine.Find("EFFECT=")) == 0)
	{
		strSet = strLine.Right(strLine.GetLength()-pos-7);

		int nPos;
		while((nPos = strSet.Find(",")) >= 0)
		{
			strTmp = strSet.Left(nPos);
			if(strcmp(strTmp, "BOLD") == 0)
			{
				m_Effect |= CFE_BOLD;
			}else
			if(strcmp(strTmp, "ITALIC") == 0)
			{
				m_Effect |= CFE_ITALIC;
			}else
			if(strcmp(strTmp, "STRIKEOUT") == 0)
			{
				m_Effect |= CFE_STRIKEOUT;
			}else
			if(strcmp(strTmp, "UNDERLINE") == 0)
			{
				m_Effect |= CFE_UNDERLINE;
			}else
			if(strcmp(strTmp, "NOEFFECT") == 0)
			{
				m_Effect = 0;
			}
			strSet = strSet.Mid(nPos + 1, strSet.GetLength() - nPos -1);
		}
		if(strSet.GetLength() > 0)
		{
			strTmp = strSet;
			if(strcmp(strTmp, "BOLD") == 0)
			{
				m_Effect |= CFE_BOLD;
			}else
			if(strcmp(strTmp, "ITALIC") == 0)
			{
				m_Effect |= CFE_ITALIC;
			}else
			if(strcmp(strTmp, "STRIKEOUT") == 0)
			{
				m_Effect |= CFE_STRIKEOUT;
			}else
			if(strcmp(strTmp, "UNDERLINE") == 0)
			{
				m_Effect |= CFE_UNDERLINE;
			}else
			if(strcmp(strTmp, "NOEFFECT") == 0)
			{
				m_Effect = 0;
			}
		}
		return;
	}

	if((pos = strLine.Find("FONT=")) == 0)
	{
		strSet = strLine.Right(strLine.GetLength()-pos-5);
		if(strcmp(strSet, "DEFAULT") == 0)
		{
			CHARFORMAT fmt;
			GetDefaultCharFormat(fmt);
			strcpy(m_FaceName, fmt.szFaceName);
		}else
			strcpy(m_FaceName, strSet);
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ָ����ɫ���
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::OutputFormat(int nColor, LPCTSTR format, ...) 
{
	SetFormat(nColor);
	va_list apList;
	va_start( apList, format );     /* Initialize variable arguments. */
	vsWrite( format, apList);
	va_end( apList ) ;
}

/////////////////////////////////////////////////////////////////////////////
// ���
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::Output(LPCTSTR format, ...) 
{
	va_list apList;
	va_start( apList, format );     /* Initialize variable arguments. */
	vsWrite( format, apList);
	va_end( apList ) ;
}

/////////////////////////////////////////////////////////////////////////////
// �Ҽ��˵�
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// ��װCoolMenu
	CMenu* pMenu = new CMenu();
	pMenu->LoadMenu(IDR_OUTPUT);
	CMenu* pmenu = (CMenu*)pMenu->GetSubMenu(0);
	ClientToScreen(&point);
	BOOL bEnable = (GetTextLength() > 0);
	pmenu->EnableMenuItem(ID_OEDIT_CLEAR, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OEDIT_SELECTALL, bEnable ? MF_ENABLED : MF_GRAYED);
	pmenu->EnableMenuItem(ID_OFILE_SAVE, bEnable ? MF_ENABLED : MF_GRAYED);
	//pmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
	//		point.x, point.y, this);
	CXTPCommandBars::TrackPopupMenu(pmenu, 0, point.x, point.y, this);
	delete pMenu;
}

/////////////////////////////////////////////////////////////////////////////
// RichEdit �������ص�����
/////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK COutputCtrl::EditStreamInCallback(DWORD dwCookie, 
    LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	CFile* pFile = (CFile*) dwCookie;

	*pcb = pFile->Read(pbBuff, cb);

	return 0;
}

DWORD CALLBACK COutputCtrl::EditStreamOutCallback(DWORD dwCookie, 
    LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	CFile* pFile = (CFile*) dwCookie;

	pFile->Write(pbBuff, cb);
	*pcb = cb;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ������ļ�
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::SaveToFile(CString strFile, int nFormat) 
{
	EDITSTREAM es;
	CFile rtfFile(TEXT(strFile), CFile::modeCreate|CFile::modeWrite);
	es.dwCookie = (DWORD) &rtfFile;
	es.pfnCallback = EditStreamOutCallback;
	StreamOut(nFormat, es);	// SF_TEXT
	rtfFile.Close();
}

/////////////////////////////////////////////////////////////////////////////
// ����������
/////////////////////////////////////////////////////////////////////////////
void COutputCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// �������������ϰ�ESC�쳣������,��ESC���ε�
	if(VK_ESCAPE == nChar)
	{
		return;
	}

	if((GetKeyState(VK_CONTROL) < 0) && GetAsyncKeyState('C'))
	{
		Copy();
	}
	
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////
// ����ѡ�񲿷�
//////////////////////////////////////////////////////////////////
void COutputCtrl::OnOeditCopy() 
{
	Copy();
}

void COutputCtrl::OnUpdateOeditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

//////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////
void COutputCtrl::OnOeditClear() 
{
	SetSel(0, -1);
	Clear();
}

void COutputCtrl::OnUpdateOeditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTextLength() > 0);
}

//////////////////////////////////////////////////////////////////
// �����������ȫѡ
//////////////////////////////////////////////////////////////////
void COutputCtrl::OnOeditSelectall() 
{
	int nLen = GetWindowTextLength();
	SetSel(0, nLen);
}

void COutputCtrl::OnUpdateOeditSelectall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTextLength() > 0);
}

//////////////////////////////////////////////////////////////////
// ���������������
//////////////////////////////////////////////////////////////////
void COutputCtrl::OnOfileSave() 
{
	CString	strFilePath = "";

	CString strFileFilterOutput = _T("�ı��ļ�(*.txt)|*.txt|RTF�ĵ�(*.rtf)|*.rtf||");
	CFileDialog dialog(FALSE, "*.txt", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFileFilterOutput);

	if(IDOK == dialog.DoModal())
	{
		strFilePath = dialog.GetPathName();

		// ������չ����������Ϊ�������͵��ĵ�
		CString strExt = dialog.GetFileExt();
		strExt.MakeLower();
		if(strExt == "txt")	// ��ͨ�ı�
			SaveToFile(strFilePath, SF_TEXT);
		else				// RTF�ĵ�
			SaveToFile(strFilePath, SF_RTF);
	}
}

void COutputCtrl::OnUpdateOfileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetTextLength() > 0);
}

//////////////////////////////////////////////////////////////////
// ����������
//////////////////////////////////////////////////////////////////
void OUTPUT_CLEAR(int nID)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return;
	}

	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return;
	}
	
	if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
	{
		COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->SetSel(0, -1);
			pOutCtrl->Clear();
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->Clear();
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->DeleteAllItems();
		}
	}
}

//////////////////////////////////////////////////////////////////
// ����ĳ������
//////////////////////////////////////////////////////////////////
int OUTPUT_ACTIVE_ID(int nID)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	pMainFrm->GetPaneManager()->ShowPane(nID+IDR_PANE_OUTPUT);

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ����ĳ�����������ʽ����ɫ
//////////////////////////////////////////////////////////////////
int OUTPUT_SET_ID(int nID, CString strFormat)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
	{
		COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->SetCtrlFormat(strFormat);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->SetCtrlFormat(strFormat);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->SetCtrlFormat(strFormat);
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��ȡĳ�����������Ϣ
//////////////////////////////////////////////////////////////////
CString OUTPUT_GET_ID(int nID, CString strParam)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return "";
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return "";
	}

	if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
	{
		COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			return "";
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			return pOutCtrl->GetInfo(strParam);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			return pOutCtrl->GetInfo(strParam);
		}
	}

	return "";
}

//////////////////////////////////////////////////////////////////
// �������������
//////////////////////////////////////////////////////////////////
int OUTPUT_TREEITEMS_ID(int nID, LPVOID pItems)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->AddTreeItems((CTTreeItemArray*&)pItems);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->AddTreeItems((CTTreeItemArray*&)pItems);
		}
	}else
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��Trace���������Ϣ,���Ըı��ʽ����ɫ
//////////////////////////////////////////////////////////////////
int OUTPUT_COLOR(COLORREF color, LPCTSTR format, ...)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
	if(pOutCtrl && pOutCtrl->m_hWnd)
	{
		pOutCtrl->SetTextColor(color);
		va_list apList;
		va_start( apList, format );     /* Initialize variable arguments. */
		pOutCtrl->vsWrite( format, apList);
		va_end( apList ) ;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��ĳ�����������Ϣ,���Ըı��ʽ����ɫ
//////////////////////////////////////////////////////////////////
int OUTPUT_COLOR_ID(int nID, COLORREF color, LPCTSTR format, ...)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
	if(pOutCtrl && pOutCtrl->m_hWnd)
	{
		pOutCtrl->SetTextColor(color);
		va_list apList;
		va_start( apList, format );     /* Initialize variable arguments. */
		pOutCtrl->vsWrite( format, apList);
		va_end( apList ) ;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��ָ�����������Ϣ,���Ըı���ɫ
//////////////////////////////////////////////////////////////////
int OutputText(int nID, CString strText, COLORREF color)
{
	return OUTPUT_COLOR_ID(nID, color, strText);
}

//////////////////////////////////////////////////////////////////
// ��Trace���������Ϣ,���Ըı��ʽ
//////////////////////////////////////////////////////////////////
int OUTPUT(int nFmt, LPCTSTR format, ...)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
	if(pOutCtrl && pOutCtrl->m_hWnd)
	{
		pOutCtrl->SetFormat(nFmt);
		va_list apList;
		va_start( apList, format );     /* Initialize variable arguments. */
		pOutCtrl->vsWrite( format, apList);
		va_end( apList ) ;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��ĳ�����������Ϣ
//////////////////////////////////////////////////////////////////
int OUTPUT_ID(int nID, LPCTSTR format, ...)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
	{
		COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			va_list apList;
			va_start( apList, format );     /* Initialize variable arguments. */
			pOutCtrl->vsWrite( format, apList);
			va_end( apList ) ;
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			va_list apList;
			va_start( apList, format );     /* Initialize variable arguments. */
			pOutCtrl->vsWrite( format, apList);
			va_end( apList ) ;
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			va_list apList;
			va_start( apList, format );     /* Initialize variable arguments. */
			pOutCtrl->vsWrite( format, apList);
			va_end( apList ) ;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��ĳ�����������Ϣ(���ý���ת���)
//////////////////////////////////////////////////////////////////
int OUTPUT_ID_DIRECT(int nID, LPCTSTR strOut)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(nID+IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	if(pOutputCtrl->nCtrlType == OUTPUT_RICH)
	{
		COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->WriteDirect(strOut);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_GRID)
	{
		CReportOutCtrl* pOutCtrl = (CReportOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->Output(strOut);
		}
	}else
	if(pOutputCtrl->nCtrlType == OUTPUT_TREE)
	{
		CTreeOutCtrl* pOutCtrl = (CTreeOutCtrl*)(pOutputCtrl->pWnd);
		if(pOutCtrl && pOutCtrl->m_hWnd)
		{
			pOutCtrl->Output(strOut);
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// ��Trace���������Ϣ,��У���ַ����Ƚϣ���һ�µ�λ��ʾΪ��ɫ
//////////////////////////////////////////////////////////////////
BOOL OUTPUT_VERIFY(CString strActual, CString strVerify)
{
	CMainFrame* pMainFrm = (CMainFrame*)(theApp.m_pMainWnd);
	if(!pMainFrm)
	{
		return FALSE;
	}
	COutputWndCtrl* pOutputCtrl = pMainFrm->GetOutputCtrl(IDR_PANE_OUTPUT);
	if(pOutputCtrl == NULL)
	{
		return FALSE;
	}

	COutputCtrl* pOutCtrl = (COutputCtrl*)(pOutputCtrl->pWnd);
	if(!pOutCtrl || !pOutCtrl->m_hWnd)
	{
		return FALSE;
	}

	BOOL bValid = TRUE;
	int nLenVerify = strVerify.GetLength();
	CString strTemp;
	for(int i=0; i<strActual.GetLength(); i++)
	{
		strTemp = strActual[i];
		if((i < nLenVerify) && (strVerify[i] != 'X') && (strVerify[i] != 'x') && (strActual[i] != strVerify[i]))
		{
			pOutCtrl->SetFormat(COLOR_ERROR);
			bValid = FALSE;
		} else
		{
			pOutCtrl->SetFormat(COLOR_NORMAL);
		}
		pOutCtrl->WriteDirect(strTemp);
	}

	return bValid;
}

//////////////////////////////////////////////////////////////////
// �������Ϣ,ֻ�ǱȽ�,��У���ַ����Ƚ�
//////////////////////////////////////////////////////////////////
BOOL STRING_VERIFY(CString strActual, CString strVerify)
{
	BOOL bValid = TRUE;
	int nLenVerify = strVerify.GetLength();
	CString strTemp;
	for(int i=0; i<strActual.GetLength(); i++)
	{
		strTemp = strActual[i];
		if((i < nLenVerify) && (strVerify[i] != 'X') && (strVerify[i] != 'x') && (strActual[i] != strVerify[i]))
		{
			bValid = FALSE;
		}
	}

	return bValid;
}

//////////////////////////////////////////////////////////////////
// ���ַ������ո�ָ������
//////////////////////////////////////////////////////////////////
void StringFillSpace(CString& strLine, int nLen)
{
	int nOldLen = strLine.GetLength();
	for(int i=nOldLen; i<nLen; i++)
	{
		strLine += _T(" ");
	}
}

//////////////////////////////////////////////////////////////////
// ��ת�ַ������������
//////////////////////////////////////////////////////////////////
CString ReverseString(CString strInput)
{
	CString strOutput = "";
	int nLen = strInput.GetLength();
	for(int i=nLen-1; i>=0; i--)
	{
		strOutput += strInput[i];
	}
	return strOutput;
}

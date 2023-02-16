// GetValueDlg.cpp : implementation file
//
#include "stdafx.h"
#include "GetTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetTextDlg dialog


CGetTextDlg::CGetTextDlg(int maxchar, TCHAR* getnum, TCHAR* title,CWnd* pParent, BOOL pwd, BOOL btndisable)
	: CDialog(CGetTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetTextDlg)
	//}}AFX_DATA_INIT
	m_intMaxChar = maxchar;
	//m_strGetVal = getnum;

	m_strGetVal.Format(_T("%S"), getnum);
	if(m_strGetVal.GetLength() < 1)
	{
		m_strGetVal.Empty();
	}
	m_strWndText = title;
	m_bpwd = pwd;
	m_bbtndisable = btndisable;
}


void CGetTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetTextDlg)
	DDX_Control(pDX, IDC_DISP_VAL, m_sDispValue);
	DDX_Control(pDX, IDB_KEY_DOT, m_bDot);
	DDX_Control(pDX, IDB_KEY_EQUAL, m_bEqual);
	DDX_Control(pDX, IDB_KEY_COLON, m_bColon);
	DDX_Control(pDX, IDB_KEY_WON, m_bWon);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetTextDlg, CDialog)
	//{{AFX_MSG_MAP(CGetTextDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetTextDlg message handlers

BEGIN_EVENTSINK_MAP(CGetTextDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CGetTextDlg)
	ON_EVENT(CGetTextDlg, IDB_KEY_OK, -600 /* Click */, OnKeyOk, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_ESC, -600 /* Click */, OnKeyEsc, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_BACK, -600 /* Click */, OnKeyBack, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_CLEAR, -600 /* Click */, OnKeyClear, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_1, -600 /* Click */, OnKey1, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_2, -600 /* Click */, OnKey2, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_3, -600 /* Click */, OnKey3, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_4, -600 /* Click */, OnKey4, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_5, -600 /* Click */, OnKey5, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_6, -600 /* Click */, OnKey6, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_7, -600 /* Click */, OnKey7, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_8, -600 /* Click */, OnKey8, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_9, -600 /* Click */, OnKey9, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_0, -600 /* Click */, OnKey0, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_Q, -600 /* Click */, OnKeyQ, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_W, -600 /* Click */, OnKeyW, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_E, -600 /* Click */, OnKeyE, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_R, -600 /* Click */, OnKeyR, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_T, -600 /* Click */, OnKeyT, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_Y, -600 /* Click */, OnKeyY, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_U, -600 /* Click */, OnKeyU, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_I, -600 /* Click */, OnKeyI, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_O, -600 /* Click */, OnKeyO, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_P, -600 /* Click */, OnKeyP, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_A, -600 /* Click */, OnKeyA, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_S, -600 /* Click */, OnKeyS, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_D, -600 /* Click */, OnKeyD, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_F, -600 /* Click */, OnKeyF, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_G, -600 /* Click */, OnKeyG, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_H, -600 /* Click */, OnKeyH, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_J, -600 /* Click */, OnKeyJ, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_K, -600 /* Click */, OnKeyK, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_L, -600 /* Click */, OnKeyL, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_Z, -600 /* Click */, OnKeyZ, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_X, -600 /* Click */, OnKeyX, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_C, -600 /* Click */, OnKeyC, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_V, -600 /* Click */, OnKeyV, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_B, -600 /* Click */, OnKeyB, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_N, -600 /* Click */, OnKeyN, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_M, -600 /* Click */, OnKeyM, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_EQUAL, -600 /* Click */, OnKeyEqual, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_WON, -600 /* Click */, OnKeyWon, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_COLON, -600 /* Click */, OnKeyColon, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_DOT, -600 /* Click */, OnKeyDot, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_DASH, -600 /* Click */, OnKeyDash, VTS_NONE)
	ON_EVENT(CGetTextDlg, IDB_KEY_UNDER_BAR, -600 /* Click */, OnKeyUnderBar, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGetTextDlg::BtnDisable()
{
	// TODO: Add your control notification handler code here
	m_bDot.EnableWindow(FALSE);
	m_bColon.EnableWindow(FALSE);
	m_bWon.EnableWindow(FALSE);
	m_bEqual.EnableWindow(FALSE);
}

void CGetTextDlg::OnKeyOk() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CGetTextDlg::OnKeyEsc() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CGetTextDlg::OnKeyBack() 
{
	// TODO: Add your control notification handler code here
	int len = m_strGetVal.GetLength();
	if( len )
	{
		m_strGetVal.SetAt(len-1,' ');
		m_strGetVal.TrimRight();
		UpdateDisplay();
	}
}

void CGetTextDlg::OnKeyClear() 
{
	// TODO: Add your control notification handler code here
	m_strGetVal.Format(_T(""));
	UpdateDisplay();
}

void CGetTextDlg::OnKey1() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "1";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey2() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "2";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey3() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "3";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey4() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "4";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey5() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "5";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey6() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "6";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey7() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "7";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey8() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "8";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey9() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "9";	
	UpdateDisplay();
}

void CGetTextDlg::OnKey0() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "0";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyQ() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "Q";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyW() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "W";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyE() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "E";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyR() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "R";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyT() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "T";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyY() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "Y";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyU() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "U";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyI() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "I";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyO() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "O";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyP() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "P";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyA() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "A";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyS() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "S";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyD() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "D";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyF() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "F";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyG() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "G";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyH() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "H";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyJ() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "J";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyK() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "K";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyL() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "L";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyZ() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "Z";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyX() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "X";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyC() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "C";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyV() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "V";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyB() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "B";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyN() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "N";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyM() 
{
	// TODO: Add your control notification handler code here
	if( !VerifyMaxChar() ) return;

	m_strGetVal += "M";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyEqual() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += "=";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyWon() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += "\\";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyColon() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += ":";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyDot() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += ".";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyDash() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += "-";	
	UpdateDisplay();
}

void CGetTextDlg::OnKeyUnderBar() 
{
	// TODO: Add your control notification handler code here
	if( m_bpwd ) return;

	if( !VerifyMaxChar() ) return;

	m_strGetVal += " ";	
	UpdateDisplay();
}

void CGetTextDlg::UpdateDisplay(COLORREF bkcolor)
{
/*    CClientDC dc (this);
    dc.DrawEdge(m_rect, EDGE_SUNKEN, BF_RECT);

    CFont* pOldFont = dc.SelectObject( GetFont() );
    CSize size = dc.GetTextExtent(m_strGetVal);

    CRect rect = m_rect;
    rect.InflateRect(-2, -2);
//    int x = rect.right - size.cx - m_cxChar;		// Right Align
    int x = rect.left + 2;							// Left Align
    int y = rect.top + ((rect.Height() - m_cyChar) / 2);

	dc.SetBkColor(bkcolor);

    dc.ExtTextOut(x, y, ETO_OPAQUE, rect, m_strGetVal, NULL);
    dc.SelectObject(pOldFont);
*/
	int len, i;
	CString bufstr;
	bufstr.Format(_T(""));

	if( m_bpwd )
	{
		len = m_strGetVal.GetLength();
		if( len <= 0 ) 
		{
			bufstr = "";
			bufstr.Format(_T(""));
			bufstr.Empty();
			m_sDispValue.SetCaption(bufstr);
			return;
		}
		for(i=0;i<len;i++) bufstr += "*";
		m_sDispValue.SetCaption(bufstr);
	}
	else
	{
		m_sDispValue.SetCaption(m_strGetVal);
		//m_sDispValue.SetCaption(_T(" "));
	}
		
}

BOOL CGetTextDlg::VerifyMaxChar()
{
	if( m_strGetVal.GetLength() >= m_intMaxChar ) return FALSE;
	else return TRUE;
}

void CGetTextDlg::InitStaticDispWnd()
{
    CStatic* pRect = (CStatic*)GetDlgItem(IDC_DISP_VAL);
    pRect->GetWindowRect(&m_rect);
    pRect->DestroyWindow();
    ScreenToClient(&m_rect);

    TEXTMETRIC tm;
    CClientDC dc(this);
    dc.GetTextMetrics(&tm);
    m_cxChar = tm.tmAveCharWidth;
    m_cyChar = tm.tmHeight - tm.tmDescent;
}

CString CGetTextDlg::GetStringValue()
{
	return m_strGetVal;
}

void CGetTextDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	UpdateDisplay();
	SetWindowText(m_strWndText);
	
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CGetTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_intMaxChar <= 0) 
		m_intMaxChar = 1;

	if( m_bbtndisable ) 
		BtnDisable();

	m_sDispValue.SetCaption(m_strGetVal);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGetTextDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
			return TRUE;

		m_sDispValue.SetFocus();
		if(pMsg->wParam == VK_RETURN)
		{
			OnKeyOk();
		}
		if(pMsg->wParam == 0x08)
		{
			OnKeyBack();
		}
		else if(pMsg->wParam == 0xDC)
		{
			OnKeyWon();
		}
		else if(pMsg->wParam == 0xBA)
		{
			OnKeyColon();
		}
		else if(pMsg->wParam == 0xBE)
		{
			OnKeyDot();
		}
		else if(pMsg->wParam == 0xBD)
		{
			OnKeyDash();
		}
		else if(pMsg->wParam == 0x20)
		{
			OnKeyUnderBar();
		}
		else if(pMsg->wParam == 0xBB)
		{
			OnKeyEqual();
		}
		else if(pMsg->wParam == 0x30)
		{
			OnKey0();
		}
		else if(pMsg->wParam == 0x31)
		{
			OnKey1();
		}
		else if(pMsg->wParam == 0x32)
		{
			OnKey2();
		}
		else if(pMsg->wParam == 0x33)
		{
			OnKey3();
		}
		else if(pMsg->wParam == 0x34)
		{
			OnKey4();
		}
		else if(pMsg->wParam == 0x35)
		{
			OnKey5();
		}
		else if(pMsg->wParam == 0x36)
		{
			OnKey6();
		}
		else if(pMsg->wParam == 0x37)
		{
			OnKey7();
		}
		else if(pMsg->wParam == 0x38)
		{
			OnKey8();
		}
		else if(pMsg->wParam == 0x39)
		{
			OnKey9();
		}
		else if(pMsg->wParam == 0x41)
		{
			OnKeyA();
		}
		else if(pMsg->wParam == 0x42)
		{
			OnKeyB();
		}
		else if(pMsg->wParam == 0x43)
		{
			OnKeyC();
		}
		else if(pMsg->wParam == 0x44)
		{
			OnKeyD();
		}
		else if(pMsg->wParam == 0x45)
		{
			OnKeyE();
		}
		else if(pMsg->wParam == 0x46)
		{
			OnKeyF();
		}
		else if(pMsg->wParam == 0x47)
		{
			OnKeyG();
		}
		else if(pMsg->wParam == 0x48)
		{
			OnKeyH();
		}
		else if(pMsg->wParam == 0x49)
		{
			OnKeyI();
		}
		else if(pMsg->wParam == 0x4A)
		{
			OnKeyJ();
		}
		else if(pMsg->wParam == 0x4B)
		{
			OnKeyK();
		}
		else if(pMsg->wParam == 0x4C)
		{
			OnKeyL();
		}
		else if(pMsg->wParam == 0x4D)
		{
			OnKeyM();
		}
		else if(pMsg->wParam == 0x4E)
		{
			OnKeyN();
		}
		else if(pMsg->wParam == 0x4F)
		{
			OnKeyO();
		}
		else if(pMsg->wParam == 0x50)
		{
			OnKeyP();
		}
		else if(pMsg->wParam == 0x51)
		{
			OnKeyQ();
		}
		else if(pMsg->wParam == 0x52)
		{
			OnKeyR();
		}
		else if(pMsg->wParam == 0x53)
		{
			OnKeyS();
		}
		else if(pMsg->wParam == 0x54)
		{
			OnKeyT();
		}
		else if(pMsg->wParam == 0x55)
		{
			OnKeyU();
		}
		else if(pMsg->wParam == 0x56)
		{
			OnKeyV();
		}
		else if(pMsg->wParam == 0x57)
		{
			OnKeyW();
		}
		else if(pMsg->wParam == 0x58)
		{
			OnKeyX();
		}
		else if(pMsg->wParam == 0x59)
		{
			OnKeyY();
		}
		else if(pMsg->wParam == 0x5A)
		{
			OnKeyZ();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

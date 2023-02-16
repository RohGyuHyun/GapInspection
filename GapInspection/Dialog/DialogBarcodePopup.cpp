// DialogBarcodePopup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogBarcodePopup.h"
#include "afxdialogex.h"


// CDialogBarcodePopup 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogBarcodePopup, CDialogEx)

CDialogBarcodePopup::CDialogBarcodePopup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogBarcodePopup::IDD, pParent)
{
	m_bInputBarcode = FALSE;
}

CDialogBarcodePopup::~CDialogBarcodePopup()
{
	KillTimer(BARCODE_TIMER_BARCODE_READER);
}

void CDialogBarcodePopup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BARCODE_POPUP_STRING, m_edit_Barcode);
	DDX_Control(pDX, IDC_BTNENHCTRL_BARCODE_STRING, m_ctrBarcode);
}


BEGIN_MESSAGE_MAP(CDialogBarcodePopup, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_BARCODE_POPUP_STRING, &CDialogBarcodePopup::OnEnChangeEditBarcodePopupString)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogBarcodePopup 메시지 처리기입니다.


void CDialogBarcodePopup::OnEnChangeEditBarcodePopupString()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bInputBarcode = FALSE;
	CString strTemp, strText;
	UpdateData(FALSE);
	m_edit_Barcode.GetWindowTextW(strTemp);
	if(!strTemp.IsEmpty() && strTemp.GetLength() >= 15)
	{
		m_edit_Barcode.SetWindowTextW(_T(""));
		strText.Format(_T("%s"), strTemp.Left(15));
		m_strBarcode.Format(_T("%s"), strText);
		UpdateData(TRUE);
		m_strWaitBarcode.Format(_T(""));
		if(!m_bNoBarcode)
		{
			if(m_bSeqProgress)
			{
				m_strWaitBarcode.Format(_T("%s"), m_strBarcode);
				m_strBarcode.Format(_T(""));
				strText.Format(_T("%s%s"), m_strBarCap, m_strWaitBarcode);
				m_ctrBarcode.SetWindowTextW(strText);
				m_bWaitBarcode = TRUE;
				UpdateData(TRUE);
			}
			else
			{
				ShowWindow(SW_HIDE);
				m_bInputBarcode = TRUE;
			}
		}
		else
		{
			ShowWindow(SW_HIDE);
			m_bInputBarcode = TRUE;
		}
	}
}

void CDialogBarcodePopup::SetShowRect(CRect *rect)
{
	m_ShowRect.left = rect->left;
	m_ShowRect.top = rect->top;
	m_ShowRect.right = rect->right;
	m_ShowRect.bottom = rect->bottom;
}


BOOL CDialogBarcodePopup::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_bWaitBarcode = FALSE;

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifdef NO_BARCODE
	m_bNoBarcode = TRUE;
#else
	m_bNoBarcode = FALSE;
#endif
	m_strBarCap.Format(_T("Barcode Input Wait"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogBarcodePopup::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogBarcodePopup::SetMainTapIdx(int nTapIdx)
{
	m_nMainTapIdx = nTapIdx;
	if(m_nMainTapIdx > 0)
	{
		KillTimer(BARCODE_TIMER_BARCODE_READER);
	}
}

void CDialogBarcodePopup::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
		MoveWindow(m_ShowRect.left, m_ShowRect.top, m_ShowRect.Width(), m_ShowRect.Height());
		//GetDlgItem(IDC_BTNENHCTRL_BARCODE_STRING)->MoveWindow(0, 0, m_ShowRect.Width(), m_ShowRect.Height());
		//m_ctrBarcode.SetWindowTextW(m_strBarCap);
		m_bWaitBarcode = FALSE;
		//m_edit_Barcode.SetWindowTextW(_T(""));
		UpdateData(TRUE);
		//SetTimer(BARCODE_TIMER_BARCODE_READER, 100, NULL);
	}
	else
	{
		//KillTimer(BARCODE_TIMER_BARCODE_READER);
	}
}


void CDialogBarcodePopup::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nEvent = (int)nIDEvent;

	switch(nEvent)
	{
	case BARCODE_TIMER_BARCODE_READER:
		if(this->IsWindowVisible() && m_nMainTapIdx == 0 && this->IsWindowEnabled())
		{
			//m_edit_Barcode.SetFocus();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CDialogBarcodePopup::SetSeqProgress(BOOL isProgress)
{
	m_bSeqProgress = isProgress;

	if(m_bSeqProgress)
	{
		m_strBarCap.Format(_T("Wait Barcode : "));
		m_ctrBarcode.SetWindowTextW(m_strBarCap);
		
	}
	else
	{
		m_strBarCap.Format(_T("Barcode Input Wait"));
		m_ctrBarcode.SetWindowTextW(m_strBarCap);
	}
}

void CDialogBarcodePopup::SetNoBarcode(CString strBarcode)
{
	m_edit_Barcode.SetWindowTextW(strBarcode);
	//OnEnChangeEditBarcodePopupString();
}
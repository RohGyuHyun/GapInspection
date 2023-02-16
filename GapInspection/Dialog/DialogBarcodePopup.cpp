// DialogBarcodePopup.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogBarcodePopup.h"
#include "afxdialogex.h"


// CDialogBarcodePopup ��ȭ �����Դϴ�.

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


// CDialogBarcodePopup �޽��� ó�����Դϴ�.


void CDialogBarcodePopup::OnEnChangeEditBarcodePopupString()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialogEx::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
#ifdef NO_BARCODE
	m_bNoBarcode = TRUE;
#else
	m_bNoBarcode = FALSE;
#endif
	m_strBarCap.Format(_T("Barcode Input Wait"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CDialogBarcodePopup::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
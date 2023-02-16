// DialogZoomView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogZoomView.h"
#include "afxdialogex.h"


// CDialogZoomView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogZoomView, CDialogEx)

CDialogZoomView::CDialogZoomView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogZoomView::IDD, pParent)
{
	m_nSelectInspPoint = 0;
	m_bLive[0] = FALSE;
	m_bLive[1] = FALSE;
	m_nCamIdx = 0;
	m_bInitDisplay = FALSE;
}

CDialogZoomView::~CDialogZoomView()
{
	if (m_pImage)		cvReleaseImage(&m_pImage);
}

void CDialogZoomView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMG, m_MatDisplay);
	for(int i = 0; i < (MAX_CAM_CNT / 2); i++)
	{
		DDX_Control(pDX, IDC_BTNENHCTRL_ZOOM_VIEW_CAM_0 + i, m_ctrCamIdx[i]);
	}

	for(int i = 0; i < MAX_GRAB_CNT; i++)
	{
		DDX_Control(pDX, IDC_BTNENHCTRL_ZOOM_VIEW_GRAB_0 + i, m_ctrGrabIdx[i]);
	}
	
	DDX_Control(pDX, IDC_BTNENHCTRL_CUR_POS, m_ctrCurserPos);
	DDX_Control(pDX, IDC_BTNENHCTRL_CUR_VAL, m_ctrCurVal);

	DDX_Control(pDX, IDC_BTNENHCTRL_LIVE, m_ctrLive);
}


BEGIN_MESSAGE_MAP(CDialogZoomView, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDialogZoomView 메시지 처리기입니다.

BOOL CDialogZoomView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_MatDisplay.SetDlgCtrlID(IDC_STATIC_IMG);
	m_MatDisplay.SetUseTracker(TRUE);
	m_nDisplayId = IDC_STATIC_IMG;
	InitImage(CAM_WIDTH, CAM_HEIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogZoomView::InitImage(long width, long height)
{
	m_pImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
}

void CDialogZoomView::UpdateDisplay()
{
	m_MatDisplay.UpdateDisplay();
}

void CDialogZoomView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strText;
	if(bShow)
	{
		CRect wndRect;

		SystemParametersInfo(SPI_GETWORKAREA, NULL, &wndRect, NULL);

		//int nShow = this->ShowWindow(SW_SHOW);
		int nWidth = wndRect.right - wndRect.left;
		int nHeight = wndRect.bottom - wndRect.top;

		this->SetWindowPos(&wndTopMost, (wndRect.right - nWidth), (wndRect.bottom - nHeight), 0, 0, SWP_NOSIZE);

		if(m_bInitDisplay)
		{
			SetLive(0, FALSE);
			SetLive(1, FALSE);
		}
		
		if(m_pImage)
		{
			//SetTimer(ZOOM_VIEW_TIMER_WINDOWS_VISIBLE, 100, NULL);
			m_MatDisplay.SetImage(m_pImage);
			m_MatDisplay.Fit();
			//SelectCamGrabImageView();

			for(int i = 0; i < MAX_CAM_CNT / 2; i++)
			{
				m_ctrCamIdx[i].SetValue(FALSE);
				if(i == m_nCamIdx)
				{
					m_ctrCamIdx[i].SetValue(TRUE);
					m_ctrGrabIdx[0].SetValue(TRUE);
					m_ctrGrabIdx[1].SetValue(FALSE);
					m_nGrabIdx = 0;
				}
			}

			switch(m_nChannel)
			{
			case 0:
				if(m_nCamIdx == 0)
				{
					strText.Format(_T("No.1 Channel - Right Camera"));
					m_nCamIdx = 0;
				}
				else
				{
					strText.Format(_T("No.1 Channel - Left Camera"));
					m_nCamIdx = 1;
				}
				break;
			case 1:
				if(m_nCamIdx == 0)
				{
					strText.Format(_T("No.2 Channel - Right Camera"));
					m_nCamIdx = 0;
				}
				else
				{
					strText.Format(_T("No.2 Channel - Left Camera"));
					m_nCamIdx = 1;
				}
				break;
			}
			
			this->SetWindowTextW(strText);

			ActiveTracker(m_nSelectInspPoint);
		}
	}
	else
	{
		if(m_bInitDisplay)
		{
			if(m_bLive[0])
			{
				m_bLive[0] = FALSE;
				AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, 0);
			}
			if(m_bLive[1])
			{
				m_bLive[1] = FALSE;
				AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, 1);
			}
		}
		
	}
}

void CDialogZoomView::SetTrackerRect(int nTrackerIdx, CRect rect)
{
	m_TrackRect[nTrackerIdx] = rect;
}

CRect CDialogZoomView::GetTrackerRect(int nTrackerIdx)
{
	m_MatDisplay.GetTracker(nTrackerIdx, &m_TrackRect[nTrackerIdx]);
	return m_TrackRect[nTrackerIdx];
}

void CDialogZoomView::ActiveTracker(int nInspPoint)
{
	m_MatDisplay.SetActiveImageRect(TRUE);
	m_MatDisplay.DeleteAll();
	char cTrackerCap[128];
	int nLen = 0;
	if(nInspPoint == (INSP_POINT1 - 1) || nInspPoint == (INSP_POINT4 - 1))
	{
		sprintf_s(cTrackerCap, INSP_CENTER_TRACKER_IDX_LABEL_0.GetLength() + 1, "%S", INSP_CENTER_TRACKER_IDX_LABEL_0);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_CENTER_TRACKER_IDX_UB_SEARCH], RGB(0, 200, 0), cTrackerCap);
		sprintf_s(cTrackerCap, INSP_CENTER_TRACKER_IDX_LABEL_1.GetLength() + 1, "%S", INSP_CENTER_TRACKER_IDX_LABEL_1);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_CENTER_TRACKER_IDX_BRACKET_VERTICALITY], RGB(200, 200, 0), cTrackerCap);
		sprintf_s(cTrackerCap, INSP_CENTER_TRACKER_IDX_LABEL_2.GetLength() + 1, "%S", INSP_CENTER_TRACKER_IDX_LABEL_2);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_CENTER_TRACKER_IDX_BRACKET_HORIZONTALITY_0], RGB(0, 200, 200), cTrackerCap);
		sprintf_s(cTrackerCap, INSP_CENTER_TRACKER_IDX_LABEL_3.GetLength() + 1, "%S", INSP_CENTER_TRACKER_IDX_LABEL_3);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_CENTER_TRACKER_IDX_BRACKET_HORIZONTALITY_1], RGB(200, 0, 200), cTrackerCap);
	}
	else
	{
		sprintf_s(cTrackerCap, INSP_LEFT_TRACKER_IDX_LABEL_0.GetLength() + 1, "%S", INSP_LEFT_TRACKER_IDX_LABEL_0);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_LEFT_RIGHT_TRACKER_IDX_BRACKET_SEARCH], RGB(0, 200, 0), cTrackerCap);
		sprintf_s(cTrackerCap, INSP_LEFT_TRACKER_IDX_LABEL_1.GetLength() + 1, "%S", INSP_LEFT_TRACKER_IDX_LABEL_1);
		m_MatDisplay.AddTracker(0, m_TrackRect[INSP_LEFT_RIGHT_TRACKER_IDX_PROJECTION], RGB(200, 200, 0), cTrackerCap);
	}
}

BEGIN_EVENTSINK_MAP(CDialogZoomView, CDialogEx)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_GRAB, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlGrab, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_INSPECTION, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlInspection, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_LIVE, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlLive, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_EXIT, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlExit, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_ZOOM_VIEW_CAM_0, DISPID_CLICK, CDialogZoomView::ZoomViewChange, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_ZOOM_VIEW_CAM_1, DISPID_CLICK, CDialogZoomView::ZoomViewChange, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_IMAGE_SAVE, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlImageSave, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_IMAGE_LOAD, DISPID_CLICK, CDialogZoomView::ClickBtnenhctrlImageLoad, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_ZOOM_VIEW_GRAB_0, DISPID_CLICK, CDialogZoomView::SelectCamGrabImageView, VTS_NONE)
	ON_EVENT(CDialogZoomView, IDC_BTNENHCTRL_ZOOM_VIEW_GRAB_1, DISPID_CLICK, CDialogZoomView::SelectCamGrabImageView, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogZoomView::ZoomViewChange()
{
	int nDialogID;
	int nIdx;
	CString strText;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();
	nIdx = 0;

	switch(nDialogID)
	{
	case IDC_BTNENHCTRL_ZOOM_VIEW_CAM_0:
		strText.Format(_T("Right Camera"));
		nIdx = 0;
		break;
	case IDC_BTNENHCTRL_ZOOM_VIEW_CAM_1:
		strText.Format(_T("Left Camera"));
		nIdx = 1;
		break;
	}

	if(m_nCamIdx == nIdx)
	{
		return;
	}

	BOOL bGrabIdx[2];
	bGrabIdx[0] = m_ctrGrabIdx[0].GetValue();
	bGrabIdx[1] = m_ctrGrabIdx[1].GetValue();

	if(bGrabIdx[0])
	{
		m_nGrabIdx = 0;
	}
	else
	{
		m_nGrabIdx = 1;
	}
	m_ctrGrabIdx[m_nGrabIdx].SetValue(TRUE);

	/*if(m_ctrGrabIdx[0].GetValue())
	{
		m_nGrabIdx = 0;
	}
	else
	{
		m_nGrabIdx = 1;
	}*/

	m_ctrLive.SetValue(FALSE);

	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_CAM_CHANGE, nIdx);
	m_nCamIdx = nIdx;
	switch(m_nChannel)
	{
	case 0:
		if(m_nCamIdx == 0)
		{
			strText.Format(_T("No.1 Channel - Right Camera"));
			m_nCamIdx = 0;
		}
		else
		{
			strText.Format(_T("No.1 Channel - Left Camera"));
			m_nCamIdx = 1;
		}
		break;
	case 1:
		if(m_nCamIdx == 0)
		{
			strText.Format(_T("No.2 Channel - Right Camera"));
			m_nCamIdx = 0;
		}
		else
		{
			strText.Format(_T("No.2 Channel - Left Camera"));
			m_nCamIdx = 1;
		}
		break;
	}
	this->SetWindowTextW(strText);
	
}

void CDialogZoomView::ClickBtnenhctrlGrab()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_ctrGrabIdx[0].GetValue())
	{
		m_nGrabIdx = 0;
	}
	else
	{
		m_nGrabIdx = 1;
	}
	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_GRAB_MSG, m_nCamIdx);
}


void CDialogZoomView::ClickBtnenhctrlInspection()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_INSPECTION_MSG, m_nCamIdx);
}


void CDialogZoomView::ClickBtnenhctrlLive()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_ctrGrabIdx[0].GetValue())
	{
		m_nGrabIdx = 0;
	}
	else
	{
		m_nGrabIdx = 1;
	}

	if(!m_bLive[m_nCamIdx])
	{
		AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_START_MSG, m_nCamIdx);
	}else
	{
		AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, m_nCamIdx);
	}
}

void CDialogZoomView::SetLive(int nCamIdx, BOOL isLive)
{
	m_bLive[nCamIdx] = isLive;
	
	if(m_bLive[nCamIdx])
	{
		m_ctrLive.SetValue(TRUE);
	}
	else
	{
		m_ctrLive.SetValue(FALSE);
	}
}


void CDialogZoomView::ClickBtnenhctrlExit()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrLive.SetValue(FALSE);
	//AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, m_nCamIdx);
	this->ShowWindow(SW_HIDE);
}

void CDialogZoomView::SetCamIdx(int nChannel, int nCamIdx)
{
	m_nCamIdx = nCamIdx;
	m_nChannel = nChannel;
	//if(this->IsWindowVisible())
	{
		for(int i = 0; i < MAX_CAM_CNT / 2; i++)
		{
			m_ctrCamIdx[i].SetValue(FALSE);
		}

		m_ctrCamIdx[m_nCamIdx].SetValue(TRUE);

		CString strText;
		switch(nChannel)
		{
		case 0:
			if(nCamIdx == 0)
			{
				strText.Format(_T("No.1 Channel - Right Camera"));
				m_nCamIdx = 0;
			}
			else
			{
				strText.Format(_T("No.1 Channel - Left Camera"));
				m_nCamIdx = 1;
			}
			break;
		case 1:
			if(nCamIdx == 0)
			{
				strText.Format(_T("No.2 Channel - Right Camera"));
				m_nCamIdx = 0;
			}
			else
			{
				strText.Format(_T("No.2 Channel - Left Camera"));
				m_nCamIdx = 1;
			}
			break;
		}
		this->SetWindowTextW(strText);
	}
}


BOOL CDialogZoomView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogZoomView::SetImage(IplImage* pImage)
{
	if(pImage == NULL)
	{
		return;
	}
	if(pImage->nChannels == 1)
	{
		cvCvtColor(pImage, m_pImage, CV_GRAY2BGR);
	}
	else
	{
		cvCopy(pImage, m_pImage);
	}
	m_MatDisplay.SetImage(m_pImage);
}

void CDialogZoomView::ClickBtnenhctrlImageSave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_IMAGE_SAVE_MSG, m_nCamIdx);
}


void CDialogZoomView::ClickBtnenhctrlImageLoad()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_IMAGE_LOAD_MSG, m_nCamIdx);
}

void CDialogZoomView::SetRsltVal(double dRslt)
{
	CString strText;
	m_dRslt = dRslt;
	strText.Format(_T("%0.3f"), m_dRslt);
	GetDlgItem(IDC_BTNENHCTRL_RSLT_VAL)->SetWindowTextW(strText);
}



void CDialogZoomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect, rect1;
	BYTE byCurVal = 0;
	CPoint curPoint, tempPoint;
	CString strText;

	GetDlgItem(IDC_STATIC_IMG)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_IMG)->GetWindowRect(rect1);

	if(rect.left + ZOOM_VIEW_IMAGE_X_OFFSET <= point.x && point.x <= rect.right + ZOOM_VIEW_IMAGE_X_OFFSET &&
		rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET <= point.y && point.y <= rect.bottom + ZOOM_VIEW_IMAGE_Y_OFFSET)
	{
		tempPoint.x = point.x - (rect.left + ZOOM_VIEW_IMAGE_X_OFFSET);
		tempPoint.y = point.y - (rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET);
		m_MatDisplay.OnMouseMove(nFlags, tempPoint);
	}

	curPoint = m_MatDisplay.GetCurImagePoint();
	strText.Format(_T("X:%04d,Y:%04d"), curPoint.x, curPoint.y);
	m_ctrCurserPos.SetCaption(strText);

	byCurVal = m_MatDisplay.GetCurImageVal();
	strText.Format(_T("Val:%d"), byCurVal);
	m_ctrCurVal.SetCaption(strText);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CDialogZoomView::SelectCamGrabImageView()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_ctrCamIdx[0].GetValue())
	{
		m_nCamIdx = 0;
	}
	else
	{
		m_nCamIdx = 1;
	}

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	int nDialogID = pBtnEnh->GetDlgCtrlID();

	switch(nDialogID)
	{
	case 1011:
		m_nGrabIdx = 0;
		break;
	case 1012:
		m_nGrabIdx = 1;
		break;
	}


	AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_IMAGE_CHANGE, m_nCamIdx);
}


void CDialogZoomView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	CPoint tempPoint;
	GetDlgItem(IDC_STATIC_IMG)->GetClientRect(rect);

	if(rect.left + ZOOM_VIEW_IMAGE_X_OFFSET <= point.x && point.x <= rect.right + ZOOM_VIEW_IMAGE_X_OFFSET &&
		rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET <= point.y && point.y <= rect.bottom + ZOOM_VIEW_IMAGE_Y_OFFSET)
	{
		tempPoint.x = point.x - (rect.left + ZOOM_VIEW_IMAGE_X_OFFSET);
		tempPoint.y = point.y - (rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET);
		m_MatDisplay.OnLButtonDblClk(nFlags, tempPoint);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CDialogZoomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	CPoint tempPoint;
	GetDlgItem(IDC_STATIC_IMG)->GetClientRect(rect);

	if(rect.left + ZOOM_VIEW_IMAGE_X_OFFSET <= point.x && point.x <= rect.right + ZOOM_VIEW_IMAGE_X_OFFSET &&
		rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET <= point.y && point.y <= rect.bottom + ZOOM_VIEW_IMAGE_Y_OFFSET)
	{
		tempPoint.x = point.x - (rect.left + ZOOM_VIEW_IMAGE_X_OFFSET);
		tempPoint.y = point.y - (rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET);
		m_MatDisplay.OnLButtonDown(nFlags, tempPoint);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDialogZoomView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rect;
	CPoint tempPoint;
	GetDlgItem(IDC_STATIC_IMG)->GetClientRect(rect);

	if(rect.left + ZOOM_VIEW_IMAGE_X_OFFSET <= point.x && point.x <= rect.right + ZOOM_VIEW_IMAGE_X_OFFSET &&
		rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET <= point.y && point.y <= rect.bottom + ZOOM_VIEW_IMAGE_Y_OFFSET)
	{
		tempPoint.x = point.x - (rect.left + ZOOM_VIEW_IMAGE_X_OFFSET);
		tempPoint.y = point.y - (rect.top + ZOOM_VIEW_IMAGE_Y_OFFSET);
		m_MatDisplay.OnLButtonUp(nFlags, tempPoint);

		AfxGetMainWnd()->SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_SET_TRACKER_RECT, m_nCamIdx);
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CDialogZoomView::SetInitDisplay()
{
	SetTimer(ZOOM_VIEW_TIMER_DIALOG_INIT, 100, NULL);
}

void CDialogZoomView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int nEvent = (int)nIDEvent;

	switch(nEvent)
	{
	case ZOOM_VIEW_TIMER_DIALOG_INIT:
		if(this->IsWindowVisible())
		{
			KillTimer(ZOOM_VIEW_TIMER_DIALOG_INIT);
			this->ShowWindow(SW_HIDE);
			SetTimer(ZOOM_VIEW_TIMER_WINDOWS_VISIBLE, 100, NULL);
		}
		break;
	case ZOOM_VIEW_TIMER_WINDOWS_VISIBLE:
		if(!this->IsWindowVisible())
		{
			KillTimer(ZOOM_VIEW_TIMER_WINDOWS_VISIBLE);
			m_bInitDisplay = TRUE;
		}
		break;
	default:

		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CDialogZoomView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	
}

CWnd *CDialogZoomView::GetDisplayHWND()
{
	return GetDlgItem(m_nDisplayId);
}
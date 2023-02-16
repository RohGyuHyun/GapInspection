// DialogInspRslt.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogInspRslt.h"
#include "afxdialogex.h"


// CDialogInspRslt 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogInspRslt, CDialogEx)

CDialogInspRslt::CDialogInspRslt(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogInspRslt::IDD, pParent)
{

}

CDialogInspRslt::~CDialogInspRslt()
{

}

void CDialogInspRslt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RLST_GAP_VAL, m_ctrlInspGapVal);
	DDX_Control(pDX, IDC_BTNENHCTRL_RSLT, m_btn_Rslt);
}


BEGIN_MESSAGE_MAP(CDialogInspRslt, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_MEASUREITEM()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RLST_GAP_VAL, &CDialogInspRslt::OnNMDblclkListRlstGapVal)
END_MESSAGE_MAP()


// CDialogInspRslt 메시지 처리기입니다.


void CDialogInspRslt::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		
	}
	else
	{

	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CDialogInspRslt::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFont m_font; 
	m_font.CreatePointFont(50, _T("Arial"));
	m_ctrlInspGapVal.SetFont(&m_font, TRUE);

	m_ctrlInspGapVal.ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	m_ctrlInspGapVal.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlInspGapVal.m_bEditDbClick = FALSE;

	InitRsltList();

	InitRslt();
	m_nGapRsltIdx = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogInspRslt::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDialogInspRslt::SetAllRsltView()
{
	WriteRsltValList();
}

void CDialogInspRslt::SetInspRsltView(int nInspPoint, BOOL *bRslt, double *dRslt)
{
	m_AllRslt[nInspPoint].nInspPoint = nInspPoint + 1;
	m_AllRslt[nInspPoint].bGapRslt = *bRslt;
	m_AllRslt[nInspPoint].dGapRslt = *dRslt;
}

double CDialogInspRslt::GetPointRslt(int nInspPoint)
{
	return m_AllRslt[nInspPoint].dGapRslt;
}

void CDialogInspRslt::WriteRsltValList()
{
	CString strText;
	double dVal = 0.;
	BOOL bRslt, bRslt2;

	strText.Format(_T("%d"), m_nGapRsltIdx + 1);
	m_ctrlInspGapVal.InsertItem(0, strText);

	strText.Format(_T("%04d.%02d.%02d-%02d:%02d:%02d"), m_cStart.GetYear(), m_cStart.GetMonth(), m_cStart.GetDay(), m_cStart.GetHour(), m_cStart.GetMinute(), m_cStart.GetSecond());
	m_ctrlInspGapVal.SetItemText(0, 1, strText);

	strText.Format(_T("%s"), m_strBarcode);
	m_ctrlInspGapVal.SetItemText(0, 2, strText);

	bRslt = TRUE;
	TypeInspRslt InspRslt;
	int nRsltDirection[MAX_RSLT_POINT_CNT];
	int nRsltPoint[2];
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		InspRslt.dGapRslt[i] = GetRslt(i, &InspRslt.bGapRslt[i], &bRslt2, &nRsltDirection[i]);
		
#ifdef MODEL_BLOOM
		if(i == RSLT_POINT1)
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Right -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Left -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
		else
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("CAM -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("USB -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
#else
		if(i == RSLT_POINT1)
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Top -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Bot -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
		else
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Right -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Left -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
#endif

		switch(i)
		{
		case RSLT_POINT1:
			nRsltPoint[0] = 0;
			nRsltPoint[1] = 3;
			break;
		case RSLT_POINT2:
			nRsltPoint[0] = 1;
			nRsltPoint[1] = 2;
			break;
		case RSLT_POINT3:
			nRsltPoint[0] = 4;
			nRsltPoint[1] = 5;
			break;
		}
		
	/*	if(!InspRslt.bGapRslt[i])
		{
			if(m_bInspPointRslt[nRsltPoint[0]] && m_bInspPointRslt[nRsltPoint[1]])
			{
				InspRslt.bGapRslt[i] = TRUE;
			}
		}
*/
		if(!InspRslt.bGapRslt[i])
			bRslt = FALSE;
	}

	/*switch(m_nLR)
	{
	case -1:
		strText.Format(_T(""));
		break;
	case 0:
		strText.Format(_T("Left"));
		break;
	case 1:
		strText.Format(_T("Right"));
		break;
	}
	m_ctrlInspGapVal.SetItemText(0, 3, strText);*/

	if(bRslt)
	{
		strText.Format(_T("OK"));
	}
	else
	{
		strText.Format(_T("NG"));
	}
	
	m_ctrlInspGapVal.SetItemText(0, 3, strText);

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
#ifdef MODEL_BLOOM
		if(i == RSLT_POINT1)
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Right -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Left -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
		else
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("CAM -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("USB -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
#else
		if(i == RSLT_POINT1)
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Top -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Bot -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
		else
		{
			if(InspRslt.dGapRslt[i] == 0.)
			{
				strText.Format(_T("Cen -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else if(InspRslt.dGapRslt[i] > 0.)
			{
				strText.Format(_T("Right -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
			else
			{
				strText.Format(_T("Left -> %0.3f"), abs(InspRslt.dGapRslt[i]));
			}
		}
#endif
		m_ctrlInspGapVal.SetItemText(0, 4 + i, strText);
	}

	if(bRslt)
	{
		m_btn_Rslt.SetBackColor(RGB(0, 200, 0));
		strText.Format(_T("OK"));
	}
	else
	{
		m_btn_Rslt.SetBackColor(RGB(200, 0, 0));
		strText.Format(_T("NG"));
	}
	m_btn_Rslt.SetWindowTextW(strText);

	m_vAllRslt = InspRslt;
	m_vAllRslt.bRslt = bRslt;
	m_vAllRslt.nRsltDirection[0] = nRsltDirection[0];
	m_vAllRslt.nRsltDirection[1] = nRsltDirection[1];
	m_vAllRslt.nRsltDirection[2] = nRsltDirection[2];

	m_nGapRsltIdx++;
}

void CDialogInspRslt::SetStartTime(CTime *cStart)
{
	m_cStart = *cStart;
}

void CDialogInspRslt::InitRslt()
{
	m_btn_Rslt.SetBackColor(RGB(200, 200, 200));
	m_btn_Rslt.SetWindowTextW(_T("-"));
	InitRsltArr();
}

void CDialogInspRslt::InitRsltArr()
{
	{
		for(int j = 0; j < MAX_INSP_POINT_CNT; j++)
		{
			m_AllRslt[j].bGapRslt = FALSE;
			m_AllRslt[j].dGapRslt = 0.;
			m_AllRslt[j].nInspPoint = 0;
		}
	}
}

void CDialogInspRslt::SetSpec(TypeSpecPara *spec)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		m_Spec[i].dMinSpec = spec[i].dMinSpec;
		m_Spec[i].dMaxSpec = spec[i].dMaxSpec;
		m_Spec[i].dOffset = spec[i].dOffset;
	}
}

BOOL CDialogInspRslt::InitRsltList()
{
	BOOL rslt = TRUE;
	DWORD dwStyle;
	CString strText(_T(""));
	LV_COLUMN lvcolumn; 
#ifdef MODEL_BLOOM
	TCHAR *sStepSideList[RSLT_LIST_SIDE_MAX_COL_COUNT] = {_T("Index"), _T("Start Time"), _T("Barcode"),  _T("RSLT"), _T("Y (1 - 4)"), _T("X Right (2 - 3)"), _T("X Left (5 - 6)"),};
#else
	TCHAR *sStepSideList[RSLT_LIST_SIDE_MAX_COL_COUNT] = {_T("Index"), _T("Start Time"), _T("Barcode"), _T("Port"), _T("RSLT"), _T("Y (1 - 4)"), _T("CAM X (2 - 3)"), _T("USB X (5 - 6)"),};
#endif

	int iStepSideWidth[RSLT_LIST_SIDE_MAX_COL_COUNT] = {50, 150, 170, 50, 115, 115, 115};

	dwStyle = m_ctrlInspGapVal.GetExtendedStyle(); 
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EDITLABELS ;// | LVS_EX_FULLROWSELECT; 
	m_ctrlInspGapVal.SetExtendedStyle(dwStyle); 	

	for(int i = 0; i < RSLT_LIST_SIDE_MAX_COL_COUNT; i++)
	{ 
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
		lvcolumn.fmt = LVCFMT_RIGHT | LVCFMT_FIXED_WIDTH; 
		if(i > 0)
		{
			lvcolumn.fmt = LVCFMT_CENTER | LVCFMT_FIXED_WIDTH; 
		}
		
		
		lvcolumn.pszText = sStepSideList[i]; 
		lvcolumn.iSubItem = i; 
		lvcolumn.cx = iStepSideWidth[i]; 
		m_ctrlInspGapVal.InsertColumn(i, &lvcolumn); 
	}
		
	UpdateData(FALSE);

	return rslt;
}

BEGIN_EVENTSINK_MAP(CDialogInspRslt, CDialogEx)
END_EVENTSINK_MAP()


void CDialogInspRslt::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(IDC_LIST_RLST_GAP_VAL == nIDCtl)
	{
		lpMeasureItemStruct->itemHeight += 10;
	}

	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void CDialogInspRslt::OnNMDblclkListRlstGapVal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.



	if(pNMItemActivate->iItem != -1)
	{
		CString strText, strDay, strTime, strTemp, strRslt;
		int nRsltIdx = 0;

		strTemp = m_ctrlInspGapVal.GetItemText(pNMItemActivate->iItem, 1);

		strDay.Format(_T("%s\\%s\\%s"), strTemp.Mid(0,4), strTemp.Mid(5, 2), strTemp.Mid(8, 2));
		strTime.Format(_T("%s%s%s"), strTemp.Mid(11,2), strTemp.Mid(14, 2), strTemp.Mid(17, 2));
		
		strTemp = m_ctrlInspGapVal.GetItemText(pNMItemActivate->iItem, 3);
		strRslt.Format(_T("%s"), strTemp);

		if(m_bOKImageOpen && m_bNGImageOpen)
		{

		}
		else if(m_bOKImageOpen && !m_bNGImageOpen)
		{
			if(strRslt.Compare(_T("OK")))
				return;
		}
		else if(!m_bOKImageOpen && m_bNGImageOpen)
		{
			if(strRslt.Compare(_T("NG")))
				return;
		}

		strTemp = m_ctrlInspGapVal.GetItemText(pNMItemActivate->iItem, 2);

		strText.Format(_T("%s%s%s\\%s\\Ch_%d\\%s_%s\\"), m_strDrive, RSLT_IMAGE_FILE_PATH, strDay, strRslt, m_nChannelIdx + 1, strTemp, strTime);

		if(FolderSearch(strText))
		{
			AfxGetMainWnd()->SendMessage(USE_MSG_ISNP_RSLT_TO_MAIN_DIALOG, INSP_RSLT_TO_MAIN_DB_CLICK, 0);
			ShellExecute(NULL, _T("open"), _T("explorer.exe"), strText, NULL, SW_SHOW);
		}
		else
		{
			CString strLog;
			strLog.Format(_T("Is Not Directory !!\n%s"), strText);
			AfxMessageBox(strLog);
		}
	}

	*pResult = 0;
}

double CDialogInspRslt::GetRslt(int nRsltPoint, BOOL *bReturnRslt, BOOL *bReturnPointRslt, int *nRsltDirection)
{
	double dRslt = 0.;
	*bReturnRslt = FALSE;
	*nRsltDirection = NON_DIRECTION;
	*bReturnPointRslt = FALSE;

	switch(nRsltPoint)
	{
	case RSLT_POINT1:
		if(m_AllRslt[INSP_Y_CAM].dGapRslt > 0. && m_AllRslt[INSP_Y_USB].dGapRslt > 0.)
		{
			*bReturnPointRslt = TRUE;
			dRslt = (m_AllRslt[INSP_Y_CAM].dGapRslt - m_AllRslt[INSP_Y_USB].dGapRslt) / 2.;
			if(dRslt == 0.)
			{
				*bReturnRslt = TRUE;
			}
		}
		break;
	case RSLT_POINT2:
		if(m_AllRslt[INSP_X_CAM_R].dGapRslt > 0. && m_AllRslt[INSP_X_CAM_L].dGapRslt > 0.)
		{
			*bReturnPointRslt = TRUE;
			dRslt = (m_AllRslt[INSP_X_CAM_R].dGapRslt - m_AllRslt[INSP_X_CAM_L].dGapRslt) / 2.;
			if(dRslt == 0.)
			{
				*bReturnRslt = TRUE;
			}
		}
		break;
	case RSLT_POINT3:
		if(m_AllRslt[INSP_X_USB_R].dGapRslt > 0. && m_AllRslt[INSP_X_USB_L].dGapRslt > 0.)
		{
			*bReturnPointRslt = TRUE;
			dRslt = (m_AllRslt[INSP_X_USB_R].dGapRslt - m_AllRslt[INSP_X_USB_L].dGapRslt) / 2.;
			if(dRslt == 0.)
			{
				*bReturnRslt = TRUE;
			}
		}
		break;
	}
	
	if(*bReturnPointRslt)
	{
		if(m_Spec[nRsltPoint].dMinSpec < dRslt && dRslt < m_Spec[nRsltPoint].dMaxSpec)
		{
			*bReturnRslt = TRUE;
		}
	}

	if(dRslt >= 0.)
	{
		*nRsltDirection = RIGHT_OR_TOP_DIRECTION;
	}
	else
	{
		*nRsltDirection = LEFT_OR_BOT_DIRECTION;
	}

	return dRslt;
}

BOOL CDialogInspRslt::GetRslt(double *dReturnRsltArr)
{
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		dReturnRsltArr[i] = m_vAllRslt.dGapRslt[i];
	}

	return m_vAllRslt.bRslt;
}

void CDialogInspRslt::SetPointRslt(BOOL *rslt)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_bInspPointRslt[i] = rslt[i];
	}
}
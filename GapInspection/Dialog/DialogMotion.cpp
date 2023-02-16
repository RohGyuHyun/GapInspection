// DialogMotion.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "DialogMotion.h"
#include "DialogAlarmMsg.h"
#include "Motion\SequenceModule.h"
#include "GapInspectionDlg.h"

#ifndef TWO_CHANNEL_MODE
extern CSequenceModule g_SeqModule;

// CDialogMotion 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogMotion, CDialogEx)

CDialogMotion::CDialogMotion(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogMotion::IDD, pParent)
	, m_bDebugBypass(FALSE)
	, m_chk_AllAxisHome(FALSE)
{
	memset(m_bInputState, NULL, sizeof(m_bInputState));
	memset(m_bOutputState, NULL, sizeof(m_bOutputState));

	m_dwIndexInput[0] = INPUT_000_EMO_BUTTON;
	m_dwIndexInput[1] = INPUT_001_LEFT_START_BUTTON;				
	m_dwIndexInput[2] =  INPUT_002_RIGHT_START_BUTTON;
	m_dwIndexInput[3] =  INPUT_003_AREA_SENSOR;						
	m_dwIndexInput[4] =  INPUT_004_Z_CYLINDER_LEFT_UP;
	m_dwIndexInput[5] =  INPUT_005_Z_CYLINDER_LEFT_DN;				
	m_dwIndexInput[6] =  INPUT_006_Z_CYLINDER_RIGHT_UP;				
	m_dwIndexInput[7] =  INPUT_007_Z_CYLINDER_RIGHT_DN;
	m_dwIndexInput[8] =  INPUT_008_X_CYLINDER_LEFT_FW;				
	m_dwIndexInput[9] =  INPUT_009_X_CYLINDER_LEFT_BK;				
	m_dwIndexInput[10] =  INPUT_010_X_CYLINDER_RIGHT_FW;				
	m_dwIndexInput[11] =  INPUT_011_X_CYLINDER_RIGHT_BK;		

	m_dwIndexOutput[0] = OUTPUT_000_TOWER_LAMP_RED;
	m_dwIndexOutput[1] = OUTPUT_001_TOWER_LAMP_YELLOW;
	m_dwIndexOutput[2] = OUTPUT_002_TOWER_LAMP_GREEN;
	m_dwIndexOutput[3] = OUTPUT_003_TOWER_LAMP_BUZZER;
	m_dwIndexOutput[4] = OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP;
	m_dwIndexOutput[5] = OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN;
	m_dwIndexOutput[6] = OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP;
	m_dwIndexOutput[7] = OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN;
	m_dwIndexOutput[8] = OUTPUT_008_SOL_X_CYLINDER_LEFT_FW;
	m_dwIndexOutput[9] = OUTPUT_009_SOL_X_CYLINDER_LEFT_BK;
	m_dwIndexOutput[10] = OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW;
	m_dwIndexOutput[11] = OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK;
	
	m_edit_dStepVal = 1.0;
	m_nProcessMoveIdx = 0;
	m_nRadioAxisName = 0;
	m_nRadioMoveType = 0;

	m_lAxisVelocity[AXIS_Y] = 10000;
	m_lAxisVelocity[AXIS_X] = 10000;
	m_lAxisVelocity[AXIS_Z] = 10000;
}

CDialogMotion::~CDialogMotion()
{
}

void CDialogMotion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IO_INPUT, m_listctrl_IO_Input);
	DDX_Control(pDX, IDC_LIST_IO_OUTPUT, m_listctrl_IO_Output);
	DDX_Text(pDX, IDC_EDIT_STEP_VALUE, m_edit_dStepVal);

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_NAME_X + i, m_CBtnAxisName[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_CUR_POS_X + i, m_CBtnAxisPulse[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_LIMIT_P_X + i, m_CBtnAxisSensorP[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_LIMIT_N_X + i, m_CBtnAxisSensorN[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_SERVO_STATUS_X + i, m_CBtnAxisServo[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_VELOCITY_X + i, m_CBtnAxis_Velocity[i]);
	}

	DDX_Control(pDX, IDC_BTNENHCTRL_RADIO_JOG, m_CBtnJog);
	DDX_Control(pDX, IDC_BTNENHCTRL_PROGRESS_HOME, m_ctrl_progress);
	DDX_Check(pDX, IDC_CHECK_DEBUG_BYPASS, m_bDebugBypass);
	DDX_Check(pDX, IDC_CHECK_ALL_HOME, m_chk_AllAxisHome);
}


BEGIN_MESSAGE_MAP(CDialogMotion, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_LIST_IO_OUTPUT, &CDialogMotion::OnNMClickListIoOutput)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_IO_TEST, &CDialogMotion::OnBnClickedButtonIoTest)
	ON_BN_CLICKED(IDC_BUTTON_IO_TEST2, &CDialogMotion::OnBnClickedButtonIoTest2)
	ON_BN_CLICKED(IDC_BUTTON_IO_TEST3, &CDialogMotion::OnBnClickedButtonIoTest3)
	ON_BN_CLICKED(IDC_BUTTON_IO_TEST4, &CDialogMotion::OnBnClickedButtonIoTest4)
END_MESSAGE_MAP()


// CDialogMotion 메시지 처리기입니다.
BOOL CDialogMotion::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		if (g_SeqModule.m_Axis.GetAmpFaultState(i)){
			if (g_SeqModule.m_Axis.AxisIsHomeFinished(i))
			{
				m_CBtnAxisName[i].SetBackColor(GREEN);
			}
			else
			{
				m_CBtnAxisName[i].SetBackColor(RED);
			}
		}
		else {
			m_CBtnAxisName[i].SetBackColor(RED);
		}
	}

	m_CBtnAxisName[AXIS_Y].SetValue(TRUE);	
	m_CBtnJog.SetValue(TRUE);

	m_ctrl_progress.SetProgressBarMin(0);
	m_ctrl_progress.SetProgressBarMax(100);
	m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
	m_ctrl_progress.SetCaption(_T(" "));
	m_ctrl_progress.ShowWindow(FALSE);

	SetListIO();

	LoadPara();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogMotion::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow)
	{
#ifdef MOTION
		g_SeqModule.m_Axis.SetReadIOStart(TRUE);
		SetTimer(TIMER_IO_READ, 100, NULL);
#endif
		LoadPara();
	}
	else
	{
#ifdef MOTION
		KillTimer(TIMER_IO_READ);
		g_SeqModule.m_Axis.SetReadIOStart(FALSE);
#endif
	}
}

void CDialogMotion::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int rslt = 0, axis_cnt = 0;
	int i = 0;

	switch(nIDEvent)
	{
	case TIMER_IO_READ:
		{
			DisplayReadIO();
			DispCurPos();
			DispAxisState();
		}
		break;
	case TIMER_ORIGIN_DLG:
		{
			if (IsDlgButtonChecked(IDC_CHECK_ALL_HOME))
				axis_cnt = MAX_AXIS_NUM;
			else
				axis_cnt = 1;

			for (i = 0; i < axis_cnt; i++)
			{
				if (1 == axis_cnt)
				{
					i = m_nRadioAxisName;
				}

				if (!g_SeqModule.m_Axis.AxisIsHomeFinished(i))
				{
					rslt = g_SeqModule.m_Axis.MotReadOriginResult(i);

					//원점 완료
					if(ORIGIN_OK == rslt)
					{
						g_SeqModule.m_Axis.SetAxisHomeFinished(i, TRUE);

						if (m_CBtnAxisName[i].GetBackColor() != GREEN)
							m_CBtnAxisName[i].SetBackColor(GREEN);

						if (1 == axis_cnt)
						{
							KillTimer(TIMER_ORIGIN_DLG);

							m_nProcessMoveIdx = 0;
							m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
							m_ctrl_progress.ShowWindow(FALSE);

							AfxMessageBox(_T("원점 완료"));
						}
					}
					//원점 동작 중..
					else if(ORIGIN_SEARCHING == rslt)
					{
						if (m_nProcessMoveIdx >= 100)
							m_nProcessMoveIdx = 0;

						m_nProcessMoveIdx++;
						m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					}
				}
			}

			if (g_SeqModule.m_Axis.AxisIsHomeFinished(AXIS_Y) && g_SeqModule.m_Axis.AxisIsHomeFinished(AXIS_X) && g_SeqModule.m_Axis.AxisIsHomeFinished(AXIS_Z))
			{
				KillTimer(TIMER_ORIGIN_DLG);

				m_nProcessMoveIdx = 0;
				m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
				m_ctrl_progress.ShowWindow(FALSE);

				if(this->IsWindowVisible())
				{
					AfxMessageBox(_T("모든 축 원점 완료"));
				}
			}
		}
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}

void CDialogMotion::DispCurPos()
{
	CString strPos = _T("");
	double dCurPos = 0;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		//dCurPos = g_SeqModule.m_Axis.GetCurPos(i);

		dCurPos = g_SeqModule.m_Axis.GetCurrentPos(i);
		strPos.Format(_T("%0.3f"), dCurPos * g_SeqModule.m_Axis.GetMm4Pulse(i));

		if(strPos.Compare(m_CBtnAxisPulse[i].GetCaption()))
			m_CBtnAxisPulse[i].SetCaption(strPos);
	}
}

void CDialogMotion::DispAxisState()
{
	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		//리미트 P
		if(g_SeqModule.m_Axis.GetPosSensor(i))
		{
			if (m_CBtnAxisSensorP[i].GetBackColor() != RED)
				m_CBtnAxisSensorP[i].SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorP[i].GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorP[i].SetBackColor(LTYELLOW);
		}

		//리미트 N
		if(g_SeqModule.m_Axis.GetNegSensor(i))
		{
			if (m_CBtnAxisSensorN[i].GetBackColor() != RED)
				m_CBtnAxisSensorN[i].SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorN[i].GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorN[i].SetBackColor(LTYELLOW);
		}

		//서보
		if(g_SeqModule.m_Axis.GetAmpFaultState(i))
		{
			if (m_CBtnAxisServo[i].GetBackColor() != GREEN)
			{
				m_CBtnAxisServo[i].SetCaption(_T("ON"));
				m_CBtnAxisServo[i].SetBackColor(GREEN);
				m_CBtnAxisServo[i].SetForeColor(BLACK);
			}
		}
		else
		{
			if (m_CBtnAxisServo[i].GetBackColor() != LTYELLOW)
			{
				m_CBtnAxisServo[i].SetCaption(_T("OFF"));
				m_CBtnAxisServo[i].SetBackColor(LTYELLOW);
				m_CBtnAxisServo[i].SetForeColor(RED);
			}
		}

		//HOME
		if(g_SeqModule.m_Axis.AxisIsHomeFinished(i))
		{
			if (m_CBtnAxisName[i].GetBackColor() != GREEN)
			{
				m_CBtnAxisName[i].SetBackColor(GREEN);
			}
		}
		else
		{
			if (m_CBtnAxisName[i].GetBackColor() != RED)
			{
				m_CBtnAxisName[i].SetBackColor(RED);
			}
		}
	}
}

BOOL CDialogMotion::LoadIOLabelPara()
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i;
	CString file_path, pat_path, extention, master_path;
	
	file_path.Format(_T("%s%s"), SYSTEM_FILE_PATH, IO_STR_PARA_FILENAME);
	
	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
		
		AfxMessageBox(_T("Not found I/O Label !"));
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < MAX_INPUT_NUM; i++)
		{
			key.Empty();
			key.Format(_T("X%03d"), i);
			GetPrivateProfileString(_T("DIGITAL INPUT LABEL"), key, _T(" "), m_sDILabel[i], sizeof(m_sDILabel), file_path);
		}

		for(i = 0; i < MAX_OUTPUT_NUM; i++)
		{
			key.Empty();
			key.Format(_T("Y%03d"), i);
			GetPrivateProfileString(_T("DIGITAL OUTPUT LABEL"), key, _T(" "), m_sDOLabel[i], sizeof(m_sDOLabel), file_path);
		}
	}
	fd=0;
	
	return rslt;
}

void CDialogMotion::DisplayReadIO()
{
	int i;
	LV_ITEM lvitem;

	CString str;
	BOOL state;

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	for(i = 0; i < MAX_OUTPUT_NUM; i++)
	{
		state = g_SeqModule.m_Axis.GetOutput(i);

		if(m_bOutputState[i] == state)
			continue;

		m_bOutputState[i] = state;

		m_listctrl_IO_Output.GetItem(&lvitem);
		str.Format(_T("Y%03d"), i);

		lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem=i;
		lvitem.iSubItem=0;
		lvitem.iImage = (state) ? 1:0;
		lvitem.pszText=(LPWSTR)(LPCWSTR)str;
		m_listctrl_IO_Output.SetItem(&lvitem);
	}

	for(i = 0; i < MAX_INPUT_NUM; i++)
	{
		state = g_SeqModule.m_Axis.GetInput(i);

		if(m_bInputState[i] == state)
			continue;

		m_bInputState[i] = state;

		m_listctrl_IO_Input.GetItem(&lvitem);
		str.Format(_T("X%03d"), i);

		lvitem.mask=LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem=i;
		lvitem.iSubItem=0;
		lvitem.iImage = (state) ? 1:0;
		lvitem.pszText=(LPWSTR)(LPCWSTR)str;
		m_listctrl_IO_Input.SetItem(&lvitem);
	}
}

void CDialogMotion::SetListIO()
{
	LV_COLUMN lvColumn;
	//CFont font;
	CImageList *img_list_slct;
	CBitmap bitmap;
	CString str;
	int i;

	LoadIOLabelPara();

	img_list_slct = new CImageList;
	img_list_slct->Create(16, 16, ILC_COLOR4, 3, 3);
	
	bitmap.LoadBitmap(IDB_BITMAP_IO_OFF);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_IO_ON);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	m_listctrl_IO_Input.SetImageList(img_list_slct, LVSIL_SMALL);
	m_listctrl_IO_Output.SetImageList(img_list_slct, LVSIL_SMALL);
	
	lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt=LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	lvColumn.pszText=_T("INPUT NO.");
	lvColumn.iSubItem=0;
	lvColumn.cx=110;
	m_listctrl_IO_Input.InsertColumn(0, &lvColumn);
	lvColumn.pszText=_T("OUTPUT NO.");
	m_listctrl_IO_Output.InsertColumn(0, &lvColumn);

	lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt=LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	lvColumn.pszText=_T("NAME");
	lvColumn.iSubItem=0;
	lvColumn.cx=200 ;
	m_listctrl_IO_Input.InsertColumn(1, &lvColumn);
	m_listctrl_IO_Output.InsertColumn(1, &lvColumn);

	m_listctrl_IO_Input.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listctrl_IO_Output.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	

	for(i = 0; i < MAX_INPUT_NUM; i++)
	{
		str.Format(_T("X%03d"), i);
		m_listctrl_IO_Input.InsertItem(i, str);

		m_listctrl_IO_Input.SetItemText(i, 1, m_sDILabel[i]);
	}

	for(i = 0; i < MAX_OUTPUT_NUM; i++)
	{
		str.Format(_T("Y%03d"), i);
		m_listctrl_IO_Output.InsertItem(i, str);

		m_listctrl_IO_Output.SetItemText(i, 1, m_sDOLabel[i]);
	}
}

void CDialogMotion::OnNMClickListIoOutput(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int idx = pNMListView->iItem;

	if(idx >= MAX_OUTPUT_NUM)
		return;
	
	int slave = (idx <= 3) ? AXIS_Y : AXIS_X;

	if(m_bOutputState[idx])
	{
		g_SeqModule.m_Axis.WriteOutput((BYTE)slave, m_dwIndexOutput[idx], FALSE);
	}
	else
	{
		g_SeqModule.m_Axis.WriteOutput((BYTE)slave, m_dwIndexOutput[idx], TRUE);
	}

	*pResult = 0;
}
BEGIN_EVENTSINK_MAP(CDialogMotion, CDialogEx)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_HOME, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonHome, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME_X, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisNameX, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME_Y, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisNameY, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME_Z, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisNameZ, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_SERVO_ON, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonServoOn, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_SERVO_OFF, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonServoOff, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_EMO_STOP, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonEmoStop, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_X_MINUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_X_PLUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_Y_MINUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_Y_PLUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_Z_MINUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVE_Z_PLUS, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_RADIO_JOG, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlRadioJog, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_RADIO_STEP, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlRadioStep, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOTION_PARAMETER, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonMotionParameter, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_X, DISPID_DBLCLICK, CDialogMotion::DblClickBtnenhctrlLabelVelocityX, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_Y, DISPID_DBLCLICK, CDialogMotion::DblClickBtnenhctrlLabelVelocityY, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_Z, DISPID_DBLCLICK, CDialogMotion::DblClickBtnenhctrlLabelVelocityZ, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_X, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelVelocityX, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_Y, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelVelocityY, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_VELOCITY_Z, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelVelocityZ, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_RE_CONNECT, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonReConnect, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_ALARM_RESET, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonAlarmReset, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogMotion::SetAllHome()
{
	m_chk_AllAxisHome = TRUE;
	UpdateData(FALSE);
	ClickBtnenhctrlButtonHome();
};

void CDialogMotion::ClickBtnenhctrlButtonHome()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strTemp;
	CString strMsg;
	int i;
	m_bHomming = FALSE;
	if (IsDlgButtonChecked(IDC_CHECK_ALL_HOME))
	{
		//모든 축 원점
		strTemp.Format(_T("ALL Axis, Start to Origin?"));

		//원점 설정
		if (AfxMessageBox(strTemp, MB_YESNO) == IDNO)
			return;

		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			if (!g_SeqModule.m_Axis.GetAmpFaultState(i))
			{
				strMsg.Format(_T("The %s is Servo off"), g_SeqModule.m_Axis.AxisGetAxisName(i));
				AfxMessageBox(strMsg);
				return;
			}
		}

		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			g_SeqModule.m_Axis.SetAxisHomeFinished(i, FALSE);
			g_SeqModule.m_Axis.SingleHome(i);
		}

		//g_SeqModule.m_Axis.AllHome();

		strTemp.Format(_T("Now homming ALL Axis. Please wait.."));
	}
	else
	{
		//개별 원점
		strTemp.Format(_T("%s Axis, Start to Origin?"), g_SeqModule.m_Axis.AxisGetAxisName(m_nRadioAxisName));

		//원점 설정
		if (AfxMessageBox(strTemp, MB_YESNO) == IDNO)
			return;

		if (!g_SeqModule.m_Axis.GetAmpFaultState(m_nRadioAxisName))
		{
			strMsg.Format(_T("The %s is Servo off"), g_SeqModule.m_Axis.AxisGetAxisName(m_nRadioAxisName));
			AfxMessageBox(strMsg);
			return;
		}

		g_SeqModule.m_Axis.SetAxisHomeFinished(m_nRadioAxisName, FALSE);

		g_SeqModule.m_Axis.SingleHome(m_nRadioAxisName);

		strTemp.Format(_T("Now homming %s Axis. Please wait.."), g_SeqModule.m_Axis.AxisGetAxisName(m_nRadioAxisName));
	}

	//프로그레스 동작
	//20190218 ngh
	if(this->IsWindowVisible())
	{
		m_ctrl_progress.SetCaption(strTemp);
		m_ctrl_progress.ShowWindow(TRUE);
	}

	m_bHomming = TRUE;

	SetTimer(TIMER_ORIGIN_DLG, 300, NULL);
}


void CDialogMotion::ClickBtnenhctrlLabelAxisNameX()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioAxisName = AXIS_Y;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisNameY()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioAxisName = AXIS_X;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisNameZ()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioAxisName = AXIS_Z;
}


void CDialogMotion::ClickBtnenhctrlButtonServoOn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	g_SeqModule.m_Axis.ServoOn(m_nRadioAxisName);
}


void CDialogMotion::ClickBtnenhctrlButtonServoOff()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	g_SeqModule.m_Axis.ServoOff(m_nRadioAxisName);
}


void CDialogMotion::ClickBtnenhctrlButtonEmoStop()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	for(int i = 0; i < MAX_AXIS_NUM; i++)
		g_SeqModule.m_Axis.Stop(i);
}

void CDialogMotion::AxisMove()
{
	UpdateData(TRUE);

	if (m_nRadioMoveType != STEP_MODE)
		return;

	int nDialogID;

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	switch (nDialogID)
	{
	case IDC_BTNENHCTRL_BUTTON_MOVE_X_MINUS:		//Y -
		if (!g_SeqModule.m_Axis.Incmove(AXIS_Y, (long)((m_edit_dStepVal * AXIS_Y_PULSE4MM) * -1), m_lAxisVelocity[AXIS_Y]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_MOVE_X_PLUS:		//Y +
		if(!g_SeqModule.m_Axis.Incmove(AXIS_Y, (long)(m_edit_dStepVal * AXIS_Y_PULSE4MM), m_lAxisVelocity[AXIS_Y]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_MOVE_Y_MINUS:		//X -
		if(!g_SeqModule.m_Axis.Incmove(AXIS_X, (long)((m_edit_dStepVal * AXIS_X_PULSE4MM) * -1), m_lAxisVelocity[AXIS_X]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_MOVE_Y_PLUS:		//X +
		if(!g_SeqModule.m_Axis.Incmove(AXIS_X, (long)(m_edit_dStepVal * AXIS_X_PULSE4MM), m_lAxisVelocity[AXIS_X]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_MOVE_Z_MINUS:		//Z -
		if(!g_SeqModule.m_Axis.Incmove(AXIS_Z, (long)((m_edit_dStepVal * AXIS_Z_PULSE4MM) * -1), m_lAxisVelocity[AXIS_Z]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_MOVE_Z_PLUS:		//Z +
		if(!g_SeqModule.m_Axis.Incmove(AXIS_Z, (long)(m_edit_dStepVal * AXIS_Z_PULSE4MM), m_lAxisVelocity[AXIS_Z]))
		{
			AfxMessageBox(_T("Failed. Run Origin or Check Servo !"));
			return;	
		}
		break;
	}
}

BOOL CDialogMotion::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_X_MINUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_Y, m_lAxisVelocity[AXIS_Y], FALSE);
				//AfxMessageBox(_T("Failed. AXIS Y Run Origin or Check Servo!"));
				//return TRUE;

		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_X_PLUS)->GetSafeHwnd())	
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_Y, m_lAxisVelocity[AXIS_Y], TRUE);
				//AfxMessageBox(_T("Failed. AXIS Y Run Origin or Check Servo!"));
				//return TRUE;
			
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Y_MINUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_X, m_lAxisVelocity[AXIS_X], FALSE);
				//AfxMessageBox(_T("Failed. AXIS X Run Origin or Check Servo!"));
				//return TRUE;
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Y_PLUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_X, m_lAxisVelocity[AXIS_X], TRUE);
				//AfxMessageBox(_T("Failed. AXIS X Run Origin or Check Servo!"));
				//return TRUE;
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Z_MINUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_Z, m_lAxisVelocity[AXIS_Z], FALSE);
				//AfxMessageBox(_T("Failed. AXIS Z Run Origin or Check Servo!"));
				//return TRUE;
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Z_PLUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.JogMove(AXIS_Z, m_lAxisVelocity[AXIS_Z], TRUE);
				//AfxMessageBox(_T("Failed. AXIS Z Run Origin or Check Servo!"));
				//return TRUE;
		}
		break;
	case WM_LBUTTONUP:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_X_MINUS)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_X_PLUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.Stop(AXIS_Y);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Y_MINUS)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Y_PLUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.Stop(AXIS_X);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Z_MINUS)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_MOVE_Z_PLUS)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				g_SeqModule.m_Axis.Stop(AXIS_Z);
		}

		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogMotion::ClickBtnenhctrlRadioJog()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioMoveType = JOG_MODE;
}


void CDialogMotion::ClickBtnenhctrlRadioStep()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_nRadioMoveType = STEP_MODE;
}


void CDialogMotion::ClickBtnenhctrlButtonMotionParameter()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	SavePara();
}

void CDialogMotion::LoadPara()
{
	CString strData, file_path;
	wchar_t sString[512];
	
	file_path.Format(_T("%s%s"), SYSTEM_FILE_PATH, MOT_STR_PARA_FILENAME);
	
	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{		
		AfxMessageBox(_T("Not found MotPara ini!"));
	}
	else
	{
		CloseHandle(fd);
		
		GetPrivateProfileString(_T("VELOCITY"), _T("AXIS_Y"), _T("50000.0"), sString, sizeof(sString), file_path);
		strData.Format(_T("%s"), sString);
		m_CBtnAxis_Velocity[AXIS_Y].SetCaption(strData);
		m_lAxisVelocity[AXIS_Y] = _wtoi(strData);

		GetPrivateProfileString(_T("VELOCITY"), _T("AXIS_X"), _T("100000.0"), sString, sizeof(sString), file_path);
		strData.Format(_T("%s"), sString);
		m_CBtnAxis_Velocity[AXIS_X].SetCaption(strData);
		m_lAxisVelocity[AXIS_X] = _wtoi(strData);

		GetPrivateProfileString(_T("VELOCITY"), _T("AXIS_Z"), _T("10000.0"), sString, sizeof(sString), file_path);
		strData.Format(_T("%s"), sString);
		m_CBtnAxis_Velocity[AXIS_Z].SetCaption(strData);
		m_lAxisVelocity[AXIS_Z] = _wtoi(strData);

	}
	fd=0;
}

void CDialogMotion::SavePara()
{
	CString  file_path;
	
	file_path.Format(_T("%s%s"), SYSTEM_FILE_PATH, MOT_STR_PARA_FILENAME);
	
	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{		
		AfxMessageBox(_T("Not found MotPara ini!"));
	}
	else
	{
		CloseHandle(fd);

		WritePrivateProfileString(_T("VELOCITY"), _T("AXIS_Y"), m_CBtnAxis_Velocity[AXIS_Y].GetCaption(), file_path);
		m_lAxisVelocity[AXIS_Y] = _wtoi(m_CBtnAxis_Velocity[AXIS_Y].GetCaption());

		WritePrivateProfileString(_T("VELOCITY"), _T("AXIS_X"), m_CBtnAxis_Velocity[AXIS_X].GetCaption(), file_path);
		m_lAxisVelocity[AXIS_X] = _wtoi(m_CBtnAxis_Velocity[AXIS_X].GetCaption());

		WritePrivateProfileString(_T("VELOCITY"), _T("AXIS_Z"), m_CBtnAxis_Velocity[AXIS_Z].GetCaption(), file_path);
		m_lAxisVelocity[AXIS_Z] = _wtoi(m_CBtnAxis_Velocity[AXIS_Z].GetCaption());
	}
	fd=0;
}

void CDialogMotion::DblClickBtnenhctrlLabelVelocityX()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}


void CDialogMotion::DblClickBtnenhctrlLabelVelocityY()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}


void CDialogMotion::DblClickBtnenhctrlLabelVelocityZ()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}


void CDialogMotion::OnBnClickedButtonIoTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool flag = false;

	if(!flag)
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, FALSE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, TRUE);

		flag = true;
	}
	else
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, TRUE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, FALSE);

		flag = false;
	}
}


void CDialogMotion::OnBnClickedButtonIoTest2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool flag = false;

	if(!flag)
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, FALSE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, TRUE);

		flag = true;
	}
	else
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, TRUE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, FALSE);

		flag = false;
	}
}


void CDialogMotion::OnBnClickedButtonIoTest3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool flag = false;

	if(!flag)
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, FALSE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, TRUE);

		flag = true;
	}
	else
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, TRUE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, FALSE);

		flag = false;
	}
}


void CDialogMotion::OnBnClickedButtonIoTest4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool flag = false;

	if(!flag)
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, FALSE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, TRUE);

		flag = true;
	}
	else
	{
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, TRUE);
		g_SeqModule.m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, FALSE);

		flag = false;
	}
}


void CDialogMotion::ClickBtnenhctrlLabelVelocityX()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	long dVal = 0;

	AfxMessageBox(_T("Max value is 250000"));

	dVal = _wtoi(m_CBtnAxis_Velocity[AXIS_Y].GetCaption());

	dlg.SetstrNum(dVal);

#ifdef MOTION
	KillTimer(TIMER_IO_READ);
#endif

	if(dlg.DoModal() != IDOK)
	{
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif
		return;
	}
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	if(_wtoi(strText) > 250000)
		return;

	m_CBtnAxis_Velocity[AXIS_Y].SetCaption(strText);
	m_lAxisVelocity[AXIS_Y] = _wtoi(strText);
}


void CDialogMotion::ClickBtnenhctrlLabelVelocityY()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	long dVal = 0;

	dVal = _wtoi(m_CBtnAxis_Velocity[AXIS_X].GetCaption());

	dlg.SetstrNum(dVal);

#ifdef MOTION
	KillTimer(TIMER_IO_READ);
#endif

	if(dlg.DoModal() != IDOK)
	{
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif
		return;
	}
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	m_CBtnAxis_Velocity[AXIS_X].SetCaption(strText);
	m_lAxisVelocity[AXIS_X] = _wtoi(strText);
}


void CDialogMotion::ClickBtnenhctrlLabelVelocityZ()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	long dVal = 0;

	dVal = _wtoi(m_CBtnAxis_Velocity[AXIS_Z].GetCaption());

	dlg.SetstrNum(dVal);

#ifdef MOTION
	KillTimer(TIMER_IO_READ);
#endif

	if(dlg.DoModal() != IDOK)
	{
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif
		return;
	}
#ifdef MOTION
	SetTimer(TIMER_IO_READ, 300, NULL);
#endif

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	m_CBtnAxis_Velocity[AXIS_Z].SetCaption(strText);
	m_lAxisVelocity[AXIS_Z] = _wtoi(strText);
}

BOOL CDialogMotion::GetSeqByPass()
{
	return ::IsDlgButtonChecked(this->m_hWnd, IDC_CHECK_DEBUG_BYPASS);
}

void CDialogMotion::ClickBtnenhctrlButtonReConnect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	g_SeqModule.m_Axis.Release();

	if(!g_SeqModule.m_Axis.Initialize(DEFAULT_PORT_NUMBER))
	{
		AfxMessageBox(_T("Error : Initialize AXIS set fail!"));
	}

}


void CDialogMotion::ClickBtnenhctrlButtonAlarmReset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strTemp;

	strTemp.Format(_T("Axis, Alarm Reset?"));

	//원점 설정
	if (AfxMessageBox(strTemp, MB_YESNO) == IDNO)
		return;
	for(int i = 0; i < MAX_AXIS_NUM; i++)
		g_SeqModule.m_Axis.AlarmReset(i);
}
#endif
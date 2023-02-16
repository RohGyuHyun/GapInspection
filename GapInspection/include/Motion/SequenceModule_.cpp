#include "StdAfx.h"
#include "SequenceModule.h"
#include "DialogAlarmMsg.h"
#include "DialogMotion.h"
#include "GapInspectionDlg.h"

//Multi media Timer
DWORD _TimerCounter;
UINT _TimerID;
TIMECAPS _Caps;
BOOL _TimerPost;

static void CALLBACK TimerReadFunc(UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if (_TimerPost == FALSE)
	{
		_TimerCounter++;
	}
	else
	{
		AfxMessageBox(_T("Error : high resolution timer event loss!"));
	}
}

CMotionData::CMotionData()
{
	memset(&m_Seq, NULL, sizeof(typeMotionSeq) * MOT_STATUS_MAX_NUM);
	memset(&m_ProSts, NULL, sizeof(typeMotionProcessSts) * MOT_STATUS_MAX_NUM * MOT_STATUS_SUB_MAX_NUM);
}


CMotionData::~CMotionData()
{
	ResetSeq();
}

void CMotionData::ResetSeq()
{
	memset(&m_Seq, NULL, sizeof(typeMotionSeq) * MOT_STATUS_MAX_NUM);
	memset(&m_ProSts, NULL, sizeof(typeMotionProcessSts) * MOT_STATUS_MAX_NUM * MOT_STATUS_SUB_MAX_NUM);
}

CSequenceModule::CSequenceModule(void)
{
	m_bInspecting = FALSE;
}


CSequenceModule::~CSequenceModule(void)
{
	if(NULL != m_pMotLog)
	{
		delete m_pMotLog;
		m_pMotLog = NULL;
	}
}

void CSequenceModule::Open(HWND hWnd, int nChannel)
{
	CString strText;

	m_hWndMain = hWnd;

	_TimerPost = FALSE;
	timeGetDevCaps(&_Caps, sizeof(_Caps));
	_Caps.wPeriodMin = 1;
	timeBeginPeriod(_Caps.wPeriodMin);
	_TimerID = timeSetEvent(_Caps.wPeriodMin, _Caps.wPeriodMin, (LPTIMECALLBACK)&TimerReadFunc, 0, (UINT)TIME_PERIODIC);

	if (!_TimerID)
	{
		AfxMessageBox(_T("Error : high resolution timer set fail!"));
	}
#ifdef MOTION
	if(!m_Axis.Initialize(DEFAULT_PORT_NUMBER))
	{
		AfxMessageBox(_T("Error : Initialize AXIS set fail!"));
	}
#endif
	strText.Format(_T("%sMotion\\"), LOG_FILE_PATH);
	m_pMotLog = new CLogFile(strText, _T("Sequence.log"));

	memset(&m_SeqFlag, NULL, sizeof(typeSequenceFlag));

	m_SeqFlag.m_bInitialized = FALSE;
	m_Flag.m_bProcessThread = TRUE;

	SetSeqChannel(nChannel);

	//20190703 ngh
	//SetLamp(COLOR_LAMP_YELLOW);//, FALSE);
	SetLamp(COLOR_LAMP_RED);//, FALSE);
}

void CSequenceModule::Close()
{
	m_Flag.m_bProcessThread = FALSE;

	SetLamp(COLOR_LAMP_OFF);//, FALSE);
#ifdef MOTION	
	m_Axis.Release();
#endif
}

void CSequenceModule::Reset()
{
	int i, j;
	int prc_idx, seq_idx, sub_seq_idx;
	prc_idx = 0;
	seq_idx = 0;
	sub_seq_idx = 0;

	CString strText;

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			if (m_MDat[i].m_Seq[j].m_bAlarm)
			{
				prc_idx = i;
				seq_idx = j;
				sub_seq_idx = m_MDat[i].m_Seq[j].m_iSubSeqIdx;

			}
			m_MDat[i].m_Seq[j].m_bAlarm = FALSE;
		}
	}

	switch (seq_idx)
	{
	case 0:

		break;
	case 1:
		switch(sub_seq_idx)
		{
		case 110:

			break;
		}
		break;
	case 2:
		switch (sub_seq_idx)
		{
		case 5030:
		case 5040:
		case 5050:
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 5000;
			break;
		case 5100:
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 5090;
			break;
		case 8000:	//수평 검사
		case 8010:	//수직 검사 실패시 시퀀스 종료
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 1000;
			break;
		default:
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 1000;	
			break;
		}
		break;
	}

	m_Flag.m_bAlarm = FALSE;
	m_Flag.m_bAutoStart = FALSE;

	strText.Format(_T("Alarm Reset"));		//LOG
	m_pMotLog->WriteText(strText);

	SetLamp(COLOR_LAMP_YELLOW);//, FALSE);
}

BOOL CSequenceModule::Start()
{
	int i, j;
	CString strText;
	BOOL rslt = TRUE;
#ifndef TWO_CHANNEL_MODE
	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		if (!m_Axis.AxisIsHomeFinished(i))
		{
			rslt = FALSE;
			SetLamp(COLOR_LAMP_RED);//, FALSE);
			return rslt;
		}
	}
#endif
	//검사시작
	if (!m_Flag.m_bFirstStart)
		InitSeq();

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			m_Flag.m_bAutoStart = TRUE;
			m_MDat[i].m_Seq[j].m_bAutoStart = m_Flag.m_bAutoStart;
		}
	}

	strText.Format(_T("Ch No %d, AutoRun Start"), m_nSeqChannel + 1);		//LOG
	m_pMotLog->WriteText(strText);

	SetLamp(COLOR_LAMP_GREEN);//, FALSE);

	return rslt;
}
void CSequenceModule::Stop()
{
	int i, j;
	CString strText;

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			m_Flag.m_bAutoStart = FALSE;
			m_MDat[i].m_Seq[j].m_bAutoStart = m_Flag.m_bAutoStart;
		}
	}

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;
	//pDlgMain->SetInitSeqStepPara(m_nSeqChannel);
	pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
	pDlgMain->SetStopInitSeq(m_nSeqChannel);
	m_nGrabStepIndex = 0;
	m_bInspecting = FALSE;

	strText.Format(_T("Ch No %d, AutoRun End"), m_nSeqChannel);		//LOG
	m_pMotLog->WriteText(strText);

	SetLamp(COLOR_LAMP_RED);//, FALSE);
}

void CSequenceModule::SetLamp(int nColor)//, BOOL bBuzz)
{
#ifdef MOTION
	switch(nColor)
	{
	case COLOR_LAMP_RED:
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_000_TOWER_LAMP_RED, TRUE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_001_TOWER_LAMP_YELLOW, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_002_TOWER_LAMP_GREEN, FALSE);
		break;
	case COLOR_LAMP_YELLOW:
		m_Axis.WriteOutput(AXIS_Y, OUTPUT_000_TOWER_LAMP_RED, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_001_TOWER_LAMP_YELLOW, TRUE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_002_TOWER_LAMP_GREEN, FALSE);
		break;
	case COLOR_LAMP_GREEN:
		m_Axis.WriteOutput(AXIS_Y, OUTPUT_000_TOWER_LAMP_RED, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_001_TOWER_LAMP_YELLOW, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_002_TOWER_LAMP_GREEN, TRUE);
		break;
	case COLOR_LAMP_OFF:
		m_Axis.WriteOutput(AXIS_Y, OUTPUT_000_TOWER_LAMP_RED, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_001_TOWER_LAMP_YELLOW, FALSE);
		m_Axis.WriteOutput(AXIS_Y,OUTPUT_002_TOWER_LAMP_GREEN, FALSE);
		break;
	default:
		
		break;
	}
#endif
	//m_Axis.WriteOutput(AXIS_X, OUTPUT_003_TOWER_LAMP_BUZZER, bBuzz);
}

void CSequenceModule::InitSeq()
{
	CString strText;
	strText.Format(_T("Ch No %d, Initialize sequence ?"), m_nSeqChannel + 1);
	//if (AfxMessageBox(strText, MB_YESNO) == IDYES)
	{
		CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

		m_Flag.m_bFirstStart = TRUE;
		m_Flag.m_bAreaSensor = FALSE;
#ifndef TWO_CHANNEL_MODE
		m_Axis.Absmove(AXIS_Y, 0, pDlgMain->GetAxisVel(AXIS_Y));
		m_Axis.Absmove(AXIS_X, 0, pDlgMain->GetAxisVel(AXIS_X));
		m_Axis.Absmove(AXIS_Z, 0, pDlgMain->GetAxisVel(AXIS_Z));
#endif
#ifndef CYLINDER_NOT_USE
		m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, TRUE);
#endif

		m_MDat[0].m_Seq[1].m_iSubSeqIdx = 100;
		m_MDat[0].m_Seq[2].m_iSubSeqIdx = 100;

		int nTimerCnt = 0;
		if(m_nSeqChannel > 0)
			nTimerCnt = 5;
		pDlgMain->KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0 + nTimerCnt);
		m_bSeqProgress = FALSE;			

		strText.Format(_T("Ch No %d, Sequence Initialize"), m_nSeqChannel + 1);		//LOG
		m_pMotLog->WriteText(strText);
	}
}

int CSequenceModule::CheckTimeout(int prc_idx, int seq_idx, int seq_sub_idx, char* status_type)
{

	if (0 == m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx)
	{
		m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx++;
		m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutS = _TimerCounter;
	}
	else
	{
		//Compare timeout
		if (0 <m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeout)
		{
			m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutE = _TimerCounter;
			if (m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeout < (m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutE - m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutS))
			{
				return 0;
			}
		}
	}

	sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_cTypeStatus, "%s", status_type);//축번호

	return 1;
}

//시퀀스 인덱스 넣기(리셋후 다음동작 않함)
void CSequenceModule::SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx)
{
	m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx = 0;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iNextSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_bOpSts = 0;
}

int CSequenceModule::CheckDelay(int prc_idx, int seq_idx, int delay)
{
	if (0 == m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_iNGSeqIdx)
	{
		m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_iNGSeqIdx++;
		m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutS = _TimerCounter;
	}
	else
	{
		//Compare timeout
		if (0 < delay)
		{
			m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutE = _TimerCounter;
			if (delay < (m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutE - m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutS))
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

//정지 상태에서의 동작을 지정
int CSequenceModule::StopModeOperation(int *prc_idx, int *seq_idx)
{	
	BOOL rslt = TRUE;

	*prc_idx = 0;
	*seq_idx = 0;

	if (m_MDat[*prc_idx].m_Seq[*seq_idx].m_bAlarm)
	{
		return 1;
	}

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;
#ifndef TWO_CHANNEL_MODE
	//Safe and limit sensor check
	if (m_Axis.ReadInput(AXIS_Y, INPUT_000_EMO_BUTTON))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_EMO);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_EMO);

		return 0;
	}

	if (!pDlgMain->GetCamConnect(TOP_CAMERA))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_TOPCAM_STATUS);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_TOPCAM_STATUS);

		return 0;
	}

	if (!pDlgMain->GetCamConnect(BOTTOM_CAMERA))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_BOTCAM_STATUS);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_BOTCAM_STATUS);

		return 0;
	}

	if (!m_Axis.AmpPowerState(AXIS_Y))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_AXIS_Y_STATUS);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_AXIS_Y_STATUS);

		return 0;
	}

	if (!m_Axis.AmpPowerState(AXIS_X))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_AXIS_X_STATUS);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_AXIS_X_STATUS);

		return 0;
	}

	if (!m_Axis.AmpPowerState(AXIS_Z))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_AXIS_Z_STATUS);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_MSG_AXIS_Z_STATUS);

		return 0;
	}
#endif
	//스톱 모드에서는 에어리어 센서는 무시한다.
	/*
	if (m_Axis.ReadInput(AXIS_Y, INPUT_003_AREA_SENSOR))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_AREA_SENSOR_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, ALARM_MSG_AREA_SENSOR);
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_AREA_SENSOR);

		return 0;
	}
	*/

	return TRUE;
}

int CSequenceModule::MotionProcess(int prc_idx, int seq_idx)
{
	int rslt = 1;
	int sub_seq_idx = 0;

	long lTotalTime = 0;

	BOOL chk_timeout = TRUE;
	BOOL not_delay = TRUE;

	char status_type[20];
	memset(status_type, NULL, sizeof(status_type));

	CString strText;
	CTime CurTime;
	CurTime = CTime::GetCurrentTime();

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	switch (seq_idx)
	{
		case 0:	//Stop mode check
			chk_timeout = FALSE;
			
			break;

		case 1:	//CASE 1: Safe and limit sensor(inter lock)
			sub_seq_idx = m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx;

			switch (sub_seq_idx)
			{
			case 100:
				SetSeqIdx(prc_idx, seq_idx, 110);
				break;
			case 110:
				SetSeqIdx(prc_idx, seq_idx, 100);
				break;
			}
			break;
		case 2:	//CASE 2: Main sequence
			sub_seq_idx = m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx;
		
			switch (sub_seq_idx)
			{
				case 100:
					//READY INITIALIZE
					m_SeqFlag.m_bInitialized = FALSE;
					m_SeqFlag.m_nMotSubSeqIdx = 100;
					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_READY;
					m_bSeqProgress = FALSE;
					m_bReadyState = TRUE;
					m_bInspecting = FALSE;
					m_bInitSeq = FALSE;
					SetSeqIdx(prc_idx, seq_idx, 110);
					break;
//				case 110://init
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ))
//					{
//						m_TimeCheck.SetCheckTime(5000);
//						m_TimeCheck.StartTimer();
//						m_bReadyState = FALSE;
//						m_nSeqIdx = INIT_SEQ;
//						strText.Format(_T("Ch No %d, Sequence Start"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						strText.Format(_T("Ch No %d, Sequence Init"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						m_nGrabStepIndex = 0;
//						m_bSeqEnd = FALSE;
//						pDlgMain->SetSeqEnd(m_nSeqChannel, FALSE);
//						pDlgMain->SetInitInspection(m_nSeqChannel);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
//						CTime start_time;
//						start_time = CTime::GetCurrentTime();
//						pDlgMain->SetStartTime(m_nSeqChannel, &start_time);
//						SetSeqIdx(prc_idx, seq_idx, 120);
//						m_nDstDeqIdx = 120;
//						m_bInitSeq = TRUE;
//					}
//					break;
//				case 120:
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ))
//					{
//						pDlgMain->SetSendPacket(m_nSeqChannel, INIT_SEQ);
//						pDlgMain->SetInspEndCheck(m_nSeqChannel);
//						SetSeqIdx(prc_idx, seq_idx, 130);
//#ifdef TEST_RUN
//						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
//#endif
//					}
//					break;
//				case 125:
//					m_bInitSeq = FALSE;
//					if(pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ)
//					{
//						SetSeqIdx(prc_idx, seq_idx, 130);
//					}
//					break;
//				case 130://grab
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						m_TimeCheck.SetCheckTime(5000);
//						m_TimeCheck.StartTimer();
//						m_bSeqProgress = TRUE;
//						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
//						m_bInspecting = TRUE;
//						strText.Format(_T("Ch No %d, Sequence 1 Step Grab Start"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqGrab(m_nSeqChannel, 0, 0);
//						SetSeqIdx(prc_idx, seq_idx, 140);
//						m_nDstDeqIdx = 140;
//					}
//					else if(pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ && m_bInitSeq)
//					{
//						//SetSeqIdx(prc_idx, seq_idx, 125);
//					}
//					break;
//				case 140:
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						strText.Format(_T("Ch No %d, Sequence 1 Step Grab End"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
//						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ);
//						pDlgMain->SetInspection(m_nSeqChannel, 0);
//
//						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
//							
//						SetSeqIdx(prc_idx, seq_idx, 150);
//#ifdef TEST_RUN
//						Sleep(1000);
//						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ + 1);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
//#endif
//					}
//					break;
//				case 150://grab
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + 1 && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
//						strText.Format(_T("Ch No %d, Sequence 2 Step Grab Start"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqGrab(m_nSeqChannel, 1, 0);
//						SetSeqIdx(prc_idx, seq_idx, 160);
//						m_nDstDeqIdx = 160;
//					}
//					break;
//				case 160:
//					if((/*pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + 1 &&*/ !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						strText.Format(_T("Ch No %d, Sequence 2 Step Grab End"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
//						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ + 1);
//						pDlgMain->SetInspection(m_nSeqChannel, 1);
//
//						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
//							
//						SetSeqIdx(prc_idx, seq_idx, 170);
//#ifdef TEST_RUN
//						Sleep(1000);
//						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ + 2);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
//#endif
//					}
//					break;
//				case 170://grab
//					if((pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + 2 && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
//						strText.Format(_T("Ch No %d, Sequence 3 Step Grab Start"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqGrab(m_nSeqChannel, 2, 0);
//						SetSeqIdx(prc_idx, seq_idx, 180);
//						m_nDstDeqIdx = 180;
//					}
//					break;
//				case 180:
//					if((/*pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + 2 &&*/ !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
//					{
//						strText.Format(_T("Ch No %d, Sequence 3 Step Grab End"), m_nSeqChannel + 1);
//						m_pMotLog->WriteText(strText);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
//						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ + 2);
//						pDlgMain->SetInspection(m_nSeqChannel, 2);
//
//						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
//							
//						SetSeqIdx(prc_idx, seq_idx, 190);
//						m_bSeqEnd = TRUE;
//						pDlgMain->SetSeqStepIndex(m_nSeqChannel, END_SEQ);
//						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
//						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
//					}
//					break;
//				case 190://end
//					if((pDlgMain->GetSeqStep(m_nSeqChannel) == END_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == END_SEQ))
//					{
//						m_nSeqIdx = END_SEQ;
//						m_bSeqEnd = TRUE;
//						SetSeqIdx(prc_idx, seq_idx, 200);
//					}
//					break;
//				case 200:
//					if((pDlgMain->GetSeqStep(m_nSeqChannel) == END_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == END_SEQ))
//					{
//						pDlgMain->SetSendPacket(m_nSeqChannel, END_SEQ);
//						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
//						m_nGrabStepIndex = 0;
//						SetSeqIdx(prc_idx, seq_idx, 100);
//						strText.Format(_T("Ch No %d, Sequence End -> Tack Time %0.3f sec"), m_nSeqChannel + 1, pDlgMain->GetTeckTime(m_nSeqChannel));
//						m_pMotLog->WriteText(strText);
//						Sleep(1500);
//						pDlgMain->SetReadPlc(m_nSeqChannel);
//					}
//					break;
				case 110://init
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == INIT_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ))
					{
						m_TimeCheck.SetCheckTime(5000);
						m_TimeCheck.StartTimer();
						m_bReadyState = FALSE;
						m_nSeqIdx = INIT_SEQ;
						strText.Format(_T("Ch No %d, Sequence Start"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						strText.Format(_T("Ch No %d, Sequence Init"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						m_nGrabStepIndex = 0;
						m_bSeqEnd = FALSE;
						pDlgMain->SetSeqEnd(m_nSeqChannel, FALSE);
						pDlgMain->SetInitInspection(m_nSeqChannel);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
						CTime start_time;
						start_time = CTime::GetCurrentTime();
						pDlgMain->SetStartTime(m_nSeqChannel, &start_time);
						SetSeqIdx(prc_idx, seq_idx, 1000);
						m_nDstDeqIdx = 120;
					}
					else if(pDlgMain->GetSeqStep(m_nSeqChannel) == SEQ_ERROR)
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
					}
					break;
				case 120:
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == INIT_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == INIT_SEQ))
					{
						pDlgMain->SetSendPacket(m_nSeqChannel, INIT_SEQ);
						pDlgMain->SetReadPlc(m_nSeqChannel);
						pDlgMain->SetInspEndCheck(m_nSeqChannel);
						SetSeqIdx(prc_idx, seq_idx, 130);
#ifdef TEST_RUN
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
#endif
					}
					break;
				case 130://grab
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						m_TimeCheck.SetCheckTime(5000);
						m_TimeCheck.StartTimer();
						m_bSeqProgress = TRUE;
						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
						m_bInspecting = TRUE;
						strText.Format(_T("Ch No %d, Sequence 1 Step Grab Start"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqGrab(m_nSeqChannel, 0, 0);
						SetSeqIdx(prc_idx, seq_idx, 140);
						m_nDstDeqIdx = 140;
					}
					else if(pDlgMain->GetSeqStepIndex(m_nSeqChannel) == SEQ_ERROR)
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
					}
					break;
				case 140:
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						strText.Format(_T("Ch No %d, Sequence 1 Step Grab End"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ);
						pDlgMain->SetReadPlc(m_nSeqChannel);
						pDlgMain->SetInspection(m_nSeqChannel, 0);

						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
							
						SetSeqIdx(prc_idx, seq_idx, 150);
#ifdef TEST_RUN
						Sleep(1000);
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ + 1);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
#endif
					}
					break;
				case 150://grab
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ + 1 && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
						strText.Format(_T("Ch No %d, Sequence 2 Step Grab Start"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqGrab(m_nSeqChannel, 1, 0);
						SetSeqIdx(prc_idx, seq_idx, 160);
						m_nDstDeqIdx = 160;
					}
					else if(pDlgMain->GetSeqStep(m_nSeqChannel) == SEQ_ERROR)
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
					}
					break;
				case 160:
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ + 1 && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						strText.Format(_T("Ch No %d, Sequence 2 Step Grab End"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ + 1);
						pDlgMain->SetReadPlc(m_nSeqChannel);
						pDlgMain->SetInspection(m_nSeqChannel, 1);

						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
							
						SetSeqIdx(prc_idx, seq_idx, 170);
#ifdef TEST_RUN
						Sleep(1000);
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, GRAB_SEQ + 2);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
#endif
					}
					break;
				case 170://grab
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ + 2 && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						m_nSeqIdx = GRAB_SEQ + m_nGrabStepIndex;
						strText.Format(_T("Ch No %d, Sequence 3 Step Grab Start"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqGrab(m_nSeqChannel, 2, 0);
						SetSeqIdx(prc_idx, seq_idx, 180);
						m_nDstDeqIdx = 180;
					}
					else if(pDlgMain->GetSeqStep(m_nSeqChannel) == SEQ_ERROR)
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
					}
					break;
				case 180:
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == GRAB_SEQ + 2 && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == GRAB_SEQ + m_nGrabStepIndex))
					{
						strText.Format(_T("Ch No %d, Sequence 3 Step Grab End"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
						pDlgMain->SetSendPacket(m_nSeqChannel, GRAB_SEQ + 2);
						pDlgMain->SetReadPlc(m_nSeqChannel);
						pDlgMain->SetInspection(m_nSeqChannel, 2);

						pDlgMain->SetReleaseGrabThread(m_nSeqChannel);
							
						SetSeqIdx(prc_idx, seq_idx, 190);
						m_bSeqEnd = TRUE;
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, END_SEQ);
						pDlgMain->SetSeqStepStart(m_nSeqChannel, TRUE);
						pDlgMain->SetSeqStepEnd(m_nSeqChannel, FALSE);
					}
					break;
				case 190://end
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == END_SEQ && pDlgMain->GetSeqStepStart(m_nSeqChannel) && !pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == END_SEQ))
					{
						m_nSeqIdx = END_SEQ;
						//pDlgMain->SetSeqStepStart(m_nSeqChannel, FALSE);
						//pDlgMain->SetSeqStepEnd(m_nSeqChannel, TRUE);
						m_bSeqEnd = TRUE;
						SetSeqIdx(prc_idx, seq_idx, 200);
					}
					else if(pDlgMain->GetSeqStep(m_nSeqChannel) == SEQ_ERROR)
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
					}
					break;
				case 200:
					if((pDlgMain->GetSeqStep(m_nSeqChannel) == END_SEQ && !pDlgMain->GetSeqStepStart(m_nSeqChannel) && pDlgMain->GetSeqStepEnd(m_nSeqChannel)) || (pDlgMain->GetDryRun() && pDlgMain->GetSeqStepIndex(m_nSeqChannel) == END_SEQ))
					{
						pDlgMain->SetSendPacket(m_nSeqChannel, END_SEQ);
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						strText.Format(_T("Ch No %d, Sequence End"), m_nSeqChannel + 1);
						m_pMotLog->WriteText(strText);
						Sleep(1500);
						pDlgMain->SetReadPlc(m_nSeqChannel);
					}
					break;
				case 1000:
					if(!pDlgMain->GetCamConnect((m_nSeqChannel * MAX_CHANNEL_CNT) + TOP_CAMERA))
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
						strText.Format(_T("Ch No %d, TOP Camera Disconnect Error"), m_nSeqChannel+ 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetMainWriteLog(strText);
						pDlgMain->SetAlarmMessage(101 + (MAX_CHANNEL_CNT * m_nSeqChannel));
					}
					else
					{
						SetSeqIdx(prc_idx, seq_idx, 1010);
					}
					break;
				case 1010:
					if(!pDlgMain->GetCamConnect((m_nSeqChannel * MAX_CHANNEL_CNT) + BOTTOM_CAMERA))
					{
						pDlgMain->SetSeqStepIndex(m_nSeqChannel, -1);
						m_nGrabStepIndex = 0;
						SetSeqIdx(prc_idx, seq_idx, 100);
						pDlgMain->SetSendPacket(m_nSeqChannel, m_nSeqIdx, TRUE);
						strText.Format(_T("Ch No %d, BOTTOM Camera Disconnect Error"), m_nSeqChannel+ 1);
						m_pMotLog->WriteText(strText);
						pDlgMain->SetMainWriteLog(strText);
						pDlgMain->SetAlarmMessage(102 + (MAX_CHANNEL_CNT * m_nSeqChannel));
					}
					else
					{
						SetSeqIdx(prc_idx, seq_idx, m_nDstDeqIdx);
					}
					break;
				case 1020:

					break;
			}

			break;
	}

	if (chk_timeout)
		rslt = CheckTimeout(prc_idx, seq_idx, m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx, status_type);

	return rslt;
}

void CSequenceModule::SequenceLoop()
{
	int rslt[MOT_PROCESS_MAX_NUM][MOT_STATUS_MAX_NUM];
	int i, j, prc_idx, seq_idx;

	BOOL chk = FALSE;
	CString strText;

	while (m_Flag.m_bProcessThread)
	{
		//쓰레드 루틴이 돌 때마다 m_bAutoStart를 체크한다.
		chk = TRUE;

		for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
		{
			for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
			{
				if (!m_MDat[i].m_Seq[j].m_bAutoStart)
				{
					chk = FALSE;
				}
			}
		}

		if (chk)		//Auto mode
		{
			//Proccessing
			for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
			{
				for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
				{
					rslt[i][j] = MotionProcess(i, j);
				
					if (1 == rslt[i][j])
					{
						//Auto status
					}
					else if (0 == rslt[i][j])
					{
						Stop();

						m_MDat[i].m_Seq[j].m_bAlarm = TRUE;
						m_Flag.m_bAlarm = TRUE; // Display Status 참조용

						//리셋시 다음번 동작예약
						m_MDat[i].m_Seq[j].m_iSubSeqIdx = m_MDat[i].m_Seq[j].m_iNextSubSeqIdx;

						//Error
						SetLamp(COLOR_LAMP_RED);//, TRUE);

						strText.Format(_T("Ch No %d, Alarm has occurred. (%S)"), m_nSeqChannel, m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGStatus);	//LOG
						m_pMotLog->WriteText(strText);

						//알람처리
						::SendMessage(m_hWndMain, USE_MSG_MOTION_TO_MAIN_DIALOG, 0, 0);
						break;
					}
				}
			}
		}
		else
		{
			//Stop Mode
			if (1 == StopModeOperation(&prc_idx, &seq_idx))
			{

			}
			else
			{
				Stop();	//정지

				//Error
				m_MDat[prc_idx].m_Seq[seq_idx].m_bAlarm = TRUE;
				m_Flag.m_bAlarm = TRUE; // Display Status 참조용

				SetLamp(COLOR_LAMP_RED);//, TRUE);

				strText.Format(_T("Ch No %d, Alarm has occurred. (%S)"), m_nSeqChannel, m_MDat[0].m_ProSts[0][m_MDat[0].m_Seq[0].m_iSubSeqIdx].m_cNGStatus);	//LOG
				m_pMotLog->WriteText(strText);

				//알람처리
				::SendMessage(m_hWndMain, USE_MSG_MOTION_TO_MAIN_DIALOG, 0, 0);
			}
			Sleep(1);
		}

		Sleep(1);
	}
}

BOOL CSequenceModule::GetInspTimeOut()
{
	if(m_TimeCheck.IsTimeOver())
	{
		return TRUE;
	}

	return FALSE;
}
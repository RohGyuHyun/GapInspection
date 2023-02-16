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
	CString strText;
	
	strText.Format(_T("%sMotion\\"), LOG_FILE_PATH);
	m_pMotLog = new CLogFile(strText, _T("Sequence.log"));

	memset(&m_SeqFlag, NULL, sizeof(typeSequenceFlag));
}


CSequenceModule::~CSequenceModule(void)
{
	if(NULL != m_pMotLog)
	{
		delete m_pMotLog;
		m_pMotLog = NULL;
	}
}

void CSequenceModule::Open(HWND hWnd)
{
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

	
	if(!m_Axis.Initialize(DEFAULT_PORT_NUMBER))
	{
		AfxMessageBox(_T("Error : Initialize AXIS set fail!"));
	}

	m_SeqFlag.m_bInitialized = FALSE;
	m_Flag.m_bProcessThread = TRUE;

	m_lStartTime = 0;
	m_lEndTime = 0;

	SetLamp(COLOR_LAMP_YELLOW);//, FALSE);
}

void CSequenceModule::Close()
{
	m_Flag.m_bProcessThread = FALSE;

	SetLamp(COLOR_LAMP_OFF);//, FALSE);
	
	m_Axis.Release();
}

void CSequenceModule::Reset()
{
	int i, j;
	int prc_idx, seq_idx, sub_seq_idx;
	prc_idx = 0;
	seq_idx = 0;
	sub_seq_idx = 0;

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

		break;
	case 2:
		switch (sub_seq_idx)
		{
		case 8000:	//수평 검사
		case 8010:	//수직 검사 실패시 시퀀스 종료
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 1000;
			break;
		case 141:
			//m_pSeqModule.m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 140;		
			break;
		case 180:	//ALIGN #2 그랩 실패
		case 181:
			//m_pSeqModule.m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 180;
			break;
		case 263: //LINESCAN 그랩 실패
			//m_pSeqModule.m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 100;
			break;
		case 480:	//그랩 종료 전송
			//m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 430;		//그랩 시작 위치로 이동
			break;

		default:
			//m_pSeqModule.m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 100;	
			break;
		}
		break;
	}

	CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

	m_Flag.m_bAlarm = FALSE;
	m_Flag.m_bAutoStart = FALSE;

	SetLamp(COLOR_LAMP_YELLOW);//, FALSE);
}

BOOL CSequenceModule::Start()
{
	int i, j;
	BOOL rslt = TRUE;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		if (!m_Axis.AxisIsHomeFinished(i))
		{
			rslt = FALSE;
			return rslt;
		}
	}


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

	SetLamp(COLOR_LAMP_GREEN);//, FALSE);

	return rslt;
}
void CSequenceModule::Stop()
{
	int i, j;

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			m_Flag.m_bAutoStart = FALSE;
			m_MDat[i].m_Seq[j].m_bAutoStart = m_Flag.m_bAutoStart;
		}
	}

	SetLamp(COLOR_LAMP_YELLOW);//, FALSE);
}

void CSequenceModule::SetLamp(int nColor)//, BOOL bBuzz)
{
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

	//m_Axis.WriteOutput(AXIS_X, OUTPUT_003_TOWER_LAMP_BUZZER, bBuzz);
}

void CSequenceModule::InitSeq()
{
	if (AfxMessageBox(_T("Initialize sequence ?"), MB_YESNO) == IDYES)
	{
		CGapInspectionDlg *pDlgMain = (CGapInspectionDlg *)AfxGetApp()->m_pMainWnd;

		m_Flag.m_bFirstStart = TRUE;

		m_Axis.Absmove(AXIS_Y, 0, pDlgMain->GetAxisVel(AXIS_Y));
		m_Axis.Absmove(AXIS_X, 0, pDlgMain->GetAxisVel(AXIS_X));
		m_Axis.Absmove(AXIS_Z, 0, pDlgMain->GetAxisVel(AXIS_Z));

		m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, TRUE);

		m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, FALSE);
		m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, TRUE);

		m_MDat[0].m_Seq[1].m_iSubSeqIdx = 100;
		m_MDat[0].m_Seq[2].m_iSubSeqIdx = 100;
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
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_AXIS_Z_STATUS);

		return 0;
	}

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

	/*
	strText.Format(_T("Init Image Start"));
	m_pMotLog->WriteText(strText);
	*/

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
			case 100:	//OP EMS 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_EMO);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_EMO); 

				if (!m_Axis.ReadInput(AXIS_Y, INPUT_000_EMO_BUTTON))
					SetSeqIdx(prc_idx, seq_idx, 110);
				break;
			case 110:	//AREA SENSOR 체크
				//대기위치에서 빠져나간 후 부터 체크하기 IMSI
				/*
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AREA_SENSOR);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AREA_SENSOR);

				if (!m_Axis.ReadInput(AXIS_Y, INPUT_003_AREA_SENSOR))
					SetSeqIdx(prc_idx, seq_idx, 120);
				*/
				SetSeqIdx(prc_idx, seq_idx, 120);
				break;
			case 120:	//TOP CAMERA 연결상태 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_TOPCAM_STATUS);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_TOPCAM_STATUS);

				if (pDlgMain->GetCamConnect(TOP_CAMERA))
					SetSeqIdx(prc_idx, seq_idx, 130);
				break;
			case 130:	//BOTTOM CAMERA 연결상태 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_BOTCAM_STATUS);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_BOTCAM_STATUS);

				if (pDlgMain->GetCamConnect(BOTTOM_CAMERA))
					SetSeqIdx(prc_idx, seq_idx, 140);
				break;
			case 140:	//Y 모터 서보 상태 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Y_STATUS);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Y_STATUS); 

				if (m_Axis.AmpPowerState(AXIS_Y))
					SetSeqIdx(prc_idx, seq_idx, 150);
				break;
			case 150:	//X 모터 서보 상태 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_X_STATUS);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_X_STATUS); 

				if (m_Axis.AmpPowerState(AXIS_X))
					SetSeqIdx(prc_idx, seq_idx, 160);
				break;
			case 160:	//Y 모터 서보 상태 체크
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Z_STATUS);
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Z_STATUS); 

				if (m_Axis.AmpPowerState(AXIS_Z))
					SetSeqIdx(prc_idx, seq_idx, 200);
				break;
			case 200:
				SetSeqIdx(prc_idx, seq_idx, 100);
				break;
			}
			break;

			/*
			//Up 안정화시간
			not_delay = CheckDelay(prc_idx, seq_idx, 500);
			if(not_delay)

			{
				SetSeqIdx(prc_idx, seq_idx, 130);
			}
			*/

		case 2:	//CASE 2: Main sequence
			sub_seq_idx = m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx;
		
			switch (sub_seq_idx)
			{
				case 100:
					//READY INITIALIZE
					m_SeqFlag.m_bInitialized = FALSE;
					m_SeqFlag.m_nMotSubSeqIdx = 100;
					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_READY;

					m_SeqFlag.m_lAxisVel[AXIS_Y] = pDlgMain->GetAxisVel(AXIS_Y);
					m_SeqFlag.m_lAxisVel[AXIS_X] = pDlgMain->GetAxisVel(AXIS_X);
					m_SeqFlag.m_lAxisVel[AXIS_Z] = pDlgMain->GetAxisVel(AXIS_Z);

					m_SeqFlag.m_bByPass = pDlgMain->GetSeqByPass();
					
					SetSeqIdx(prc_idx, seq_idx, 110);
					break;
				case 110:

					SetSeqIdx(prc_idx, seq_idx, 120);
					break;
				case 120:
					//INPUT START BUTTON 
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = 0;//TIMEOUT_INPUT_BUTTON;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_MISS_START_BUTTON);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_MISS_START_BUTTON);

					//IMSI
					if( (m_Axis.ReadInput(AXIS_Y, INPUT_001_LEFT_START_BUTTON)) && (m_Axis.ReadInput(AXIS_Y, INPUT_002_RIGHT_START_BUTTON)) 
						|| (m_bswStart[0] && m_bswStart[1]) )
					{
						m_bswStart[0] = FALSE;	//IMSI
						m_bswStart[1] = FALSE;
						
						//if(m_SeqFlag.m_bInitialized)
						//if(m_Flag.m_bFirstStart)
						{
							SetSeqIdx(prc_idx, seq_idx, 7000);	//실린더 검사동작으로 이동

							m_lStartTime = GetCurrentTime();	// Start T/T 
						}
						//else
						//{
							//SetSeqIdx(prc_idx, seq_idx, 6000);	//실린더 초기화로 이동
						//}
					}
					break;
				
				case 200:	//Anywhere -> 대기위치
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X] = 0;
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y] = 0;
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1] = 0;
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z2] = 0;

					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_READY;

					SetSeqIdx(prc_idx, seq_idx, 5000);		//모션 이동
					break;
				case 300:	//대기위치 -> STEP#1 위치
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP1, AXIS_POS_STEP_X) * AXIS_X_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP1, AXIS_POS_STEP_Y) * AXIS_Y_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP1, AXIS_POS_STEP_Z1) * AXIS_Z_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z2] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP1, AXIS_POS_STEP_Z2) * AXIS_Z_PULSE4MM);

					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_STEP1;

					SetSeqIdx(prc_idx, seq_idx, 5000);		//모션 이동
					break;
				case 400:	//STEP#1 -> STEP#2 위치
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP2, AXIS_POS_STEP_X) * AXIS_X_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP2, AXIS_POS_STEP_Y) * AXIS_Y_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP2, AXIS_POS_STEP_Z1) * AXIS_Z_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z2] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP2, AXIS_POS_STEP_Z2) * AXIS_Z_PULSE4MM);

					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_STEP2;

					SetSeqIdx(prc_idx, seq_idx, 5000);		//모션 이동
					break;	
				case 500:	//STEP#2 -> STEP#3 위치
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP3, AXIS_POS_STEP_X) * AXIS_X_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP3, AXIS_POS_STEP_Y) * AXIS_Y_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP3, AXIS_POS_STEP_Z1) * AXIS_Z_PULSE4MM);
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z2] = (long)(pDlgMain->GetSeqMotionPara(SEQ_STATUS_STEP3, AXIS_POS_STEP_Z2) * AXIS_Z_PULSE4MM);

					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_STEP3;	//모션 이동

					SetSeqIdx(prc_idx, seq_idx, 5000);
					break;
				case 600:
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X] = 0;
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y] = 0;
					m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1] = 0;

					m_SeqFlag.m_nSeqIndex = SEQ_STATUS_END;

					SetSeqIdx(prc_idx, seq_idx, 5000);
					break;

				case 1000:	//시퀀스 종료
					m_lEndTime = GetCurrentTime();	// Start T/T 

					lTotalTime = m_lEndTime - m_lStartTime;

					SetSeqIdx(prc_idx, seq_idx, 100);
					break;


				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//[case 5000 ~ 5050] X, Y, Z 모션 이동 및 체크 시작
				//
				case 5000://X축 모터 이동
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_STATUS;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_X_STATUS);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_X_STATUS);

					if(m_Axis.Absmove(AXIS_X, m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_X], m_SeqFlag.m_lAxisVel[AXIS_X]))
						SetSeqIdx(prc_idx, seq_idx, 5010);
					break;
				case 5010://Y축 모터 이동
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_STATUS;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Y_STATUS);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Y_STATUS);

					if(m_Axis.Absmove(AXIS_Y, m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Y], m_SeqFlag.m_lAxisVel[AXIS_Y]))
						SetSeqIdx(prc_idx, seq_idx, 5020);
					break;
				case 5020://Z1 축 이동
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_STATUS;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Z_STATUS);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Z_STATUS);

					if(m_Axis.Absmove(AXIS_Z, m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z1], m_SeqFlag.m_lAxisVel[AXIS_Z]))
						SetSeqIdx(prc_idx, seq_idx, 5030);
					break;
				case 5030://X 축 Move done
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_MOVE;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_X_MOVEFAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_X_MOVEFAIL);

					if(m_Axis.AxisMoveDone(AXIS_X))
						SetSeqIdx(prc_idx, seq_idx, 5040);
					break;
				case 5040://Y 축 Move done
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_MOVE;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Y_MOVEFAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Y_MOVEFAIL);

					if(m_Axis.AxisMoveDone(AXIS_Y))
						SetSeqIdx(prc_idx, seq_idx, 5050);
					break;
				case 5050://Z 축 Move done
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_MOVE;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Z_MOVEFAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Z_MOVEFAIL);

					if(m_Axis.AxisMoveDone(AXIS_Z))
						SetSeqIdx(prc_idx, seq_idx, 5055);
					break;
				case 5055:
					//안정화시간
					not_delay = CheckDelay(prc_idx, seq_idx, 300);

					if(not_delay)
						SetSeqIdx(prc_idx, seq_idx, 5060);
					break;
				case 5060:
					switch(m_SeqFlag.m_nSeqIndex)
					{
					case SEQ_STATUS_READY:
						m_SeqFlag.m_bInitialized = TRUE;
						m_SeqFlag.m_nMotSubSeqIdx = 110;	//START BUTTON
						break;
					case SEQ_STATUS_STEP1:
					case SEQ_STATUS_STEP2:
					case SEQ_STATUS_STEP3:
						if(m_SeqFlag.m_bByPass)
							m_SeqFlag.m_nMotSubSeqIdx = 5090;
						else
							m_SeqFlag.m_nMotSubSeqIdx = 5070;	//GRAB
						break;
					case SEQ_STATUS_END:
						m_SeqFlag.m_nMotSubSeqIdx = 6000;
						break;
					}

					SetSeqIdx(prc_idx, seq_idx, m_SeqFlag.m_nMotSubSeqIdx);
					break;
				case 5070://Z1 Grab
					m_bIndexZ = 0;
					::SendMessage(m_hWndMain, USE_MSG_MOTION_TO_MAIN_DIALOG, 3, m_SeqFlag.m_nSeqIndex);

					SetSeqIdx(prc_idx, seq_idx, 5075);
					break;
				case 5075:
					//안정화시간
					not_delay = CheckDelay(prc_idx, seq_idx, 300);

					if(not_delay)
						SetSeqIdx(prc_idx, seq_idx, 5080);
					break;
				case 5080:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_GRAB;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_GRAB_FAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB_FAIL);

					if(pDlgMain->GetGrabEnd(TOP_CAMERA) && pDlgMain->GetGrabEnd(BOTTOM_CAMERA))
						SetSeqIdx(prc_idx, seq_idx, 5090);
					break;
				case 5090:	//Z2 축 이동
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_STATUS;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Z_STATUS);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Z_STATUS);

					if(m_Axis.Absmove(AXIS_Z, m_SeqFlag.m_lAxisPos[AXIS_POS_STEP_Z2], m_SeqFlag.m_lAxisVel[AXIS_Z]))
						SetSeqIdx(prc_idx, seq_idx, 5100);
					break;
				case 5100://Z 축 Move done
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MOTOR_MOVE;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_AXIS_Z_MOVEFAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_AXIS_Z_MOVEFAIL);

					if(m_Axis.AxisMoveDone(AXIS_Z))
					{
						if(m_SeqFlag.m_bByPass)
							SetSeqIdx(prc_idx, seq_idx, 5130);
						else
							SetSeqIdx(prc_idx, seq_idx, 5105);
					}
					break;
				case 5105:
					//안정화시간
					not_delay = CheckDelay(prc_idx, seq_idx, 300);

					if(not_delay)
						SetSeqIdx(prc_idx, seq_idx, 5110);
					break;
				case 5110:	//Z2 Grab
					m_bIndexZ = 1;
					::SendMessage(m_hWndMain, USE_MSG_MOTION_TO_MAIN_DIALOG, 3, m_SeqFlag.m_nSeqIndex);

					SetSeqIdx(prc_idx, seq_idx, 5115);
					break;
				case 5115:
					//안정화시간
					not_delay = CheckDelay(prc_idx, seq_idx, 300);

					if(not_delay)
						SetSeqIdx(prc_idx, seq_idx, 5120);
					break;
				case 5120:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_GRAB;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_GRAB_FAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB_FAIL);

					if(pDlgMain->GetGrabEnd(TOP_CAMERA) && pDlgMain->GetGrabEnd(BOTTOM_CAMERA))
						SetSeqIdx(prc_idx, seq_idx, 5130);
					break;
				case 5130:
					switch(m_SeqFlag.m_nSeqIndex)
					{
					case SEQ_STATUS_STEP1:
						if(m_SeqFlag.m_bByPass)
							m_SeqFlag.m_nMotSubSeqIdx = 400;
						else
							m_SeqFlag.m_nMotSubSeqIdx = 8000;	//수평 수직 검사
						break;
					case SEQ_STATUS_STEP2:
						m_SeqFlag.m_nMotSubSeqIdx = 500;
						break;
					case SEQ_STATUS_STEP3:
						m_SeqFlag.m_nMotSubSeqIdx = 600;
						break;
					}

					SetSeqIdx(prc_idx, seq_idx, m_SeqFlag.m_nMotSubSeqIdx);
					break;

				//
				//[case 5000 ~ 5050] 모션 이동 및 체크 끝
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//[case 6000 ~ 6110] 실린더 대기 위치 이동 및 체크 시작
				//※ 사이사이 안정화 시간 검토
				case 6000://Z 실린더 상승
					m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, TRUE);

					m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, TRUE);

					SetSeqIdx(prc_idx, seq_idx, 6010);
					break;
				case 6010:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_LEFT_DN);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_LEFT_DN);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_005_Z_CYLINDER_LEFT_DN))
						SetSeqIdx(prc_idx, seq_idx, 6020);
					break;
				case 6020:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_LEFT_UP);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_LEFT_UP);

					if(m_Axis.ReadInput(AXIS_X, INPUT_004_Z_CYLINDER_LEFT_UP))
						SetSeqIdx(prc_idx, seq_idx, 6030);
					break;
				case 6030:

					SetSeqIdx(prc_idx, seq_idx, 6040);
					break;
				case 6040:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_RIGHT_DN);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_RIGHT_DN);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_007_Z_CYLINDER_RIGHT_DN))
						SetSeqIdx(prc_idx, seq_idx, 6050);
					break;
				case 6050:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_RIGHT_UP);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_RIGHT_UP);

					if(m_Axis.ReadInput(AXIS_X, INPUT_006_Z_CYLINDER_RIGHT_UP))
						SetSeqIdx(prc_idx, seq_idx, 6060);
					break;
				case 6060://X 실린더 후진
					m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, TRUE);

					m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, TRUE);

					SetSeqIdx(prc_idx, seq_idx, 6070);
					break;
				case 6070:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_LEFT_BK);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_LEFT_BK);

					if(m_Axis.ReadInput(AXIS_X, INPUT_009_X_CYLINDER_LEFT_BK))
						SetSeqIdx(prc_idx, seq_idx, 6080);
					break;
				case 6080:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_LEFT_FW);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_LEFT_FW);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_008_X_CYLINDER_LEFT_FW))
						SetSeqIdx(prc_idx, seq_idx, 6090);
					break;
				case 6090:

					SetSeqIdx(prc_idx, seq_idx, 6100);
					break;
				case 6100:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_RIGHT_BK);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_RIGHT_BK);

					if(m_Axis.ReadInput(AXIS_X, INPUT_011_X_CYLINDER_RIGHT_BK))
						SetSeqIdx(prc_idx, seq_idx, 6110);
					break;
				case 6110:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_RIGHT_FW);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_RIGHT_FW);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_010_X_CYLINDER_RIGHT_FW))
					{
						switch(m_SeqFlag.m_nSeqIndex)
						{
						case SEQ_STATUS_READY:
							SetSeqIdx(prc_idx, seq_idx, 200);
							break;
						case SEQ_STATUS_END:
							SetSeqIdx(prc_idx, seq_idx, 1000);
							break;
						}
					}
					break;
				//
				//[case 6000 ~ 6110] 실린더 대기 위치 이동 및 체크 끝
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//[case 7000 ~ 7110] 실린더 검사 위치 이동 및 체크 시작
				//※ 사이사이 안정화 시간 검토
				case 7000://X 실린더 전진
					m_Axis.WriteOutput(AXIS_X, OUTPUT_009_SOL_X_CYLINDER_LEFT_BK, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_008_SOL_X_CYLINDER_LEFT_FW, TRUE);

					m_Axis.WriteOutput(AXIS_X, OUTPUT_011_SOL_X_CYLINDER_RIGHT_BK, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_010_SOL_X_CYLINDER_RIGHT_FW, TRUE);
					
					SetSeqIdx(prc_idx, seq_idx, 7010);
					break;
				case 7010:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_LEFT_BK);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_LEFT_BK);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_009_X_CYLINDER_LEFT_BK))
						SetSeqIdx(prc_idx, seq_idx, 7020);
					break;
				case 7020:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_LEFT_FW);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_LEFT_FW);

					if(m_Axis.ReadInput(AXIS_X, INPUT_008_X_CYLINDER_LEFT_FW))
						SetSeqIdx(prc_idx, seq_idx, 7030);
					break;
				case 7030:
					
					SetSeqIdx(prc_idx, seq_idx, 7040);
					break;
				case 7040:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_RIGHT_BK);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_RIGHT_BK);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_011_X_CYLINDER_RIGHT_BK))
						SetSeqIdx(prc_idx, seq_idx, 7050);
					break;
				case 7050:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_X_CYLINDER_RIGHT_FW);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_X_CYLINDER_RIGHT_FW);

					if(m_Axis.ReadInput(AXIS_X, INPUT_010_X_CYLINDER_RIGHT_FW))
						SetSeqIdx(prc_idx, seq_idx, 7060);
					break;
				case 7060://좌측 Z 실린더 하강
					m_Axis.WriteOutput(AXIS_X, OUTPUT_004_SOL_Z_CYLINDER_LEFT_UP, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_005_SOL_Z_CYLINDER_LEFT_DN, TRUE);

					m_Axis.WriteOutput(AXIS_X, OUTPUT_006_SOL_Z_CYLINDER_RIGHT_UP, FALSE);
					m_Axis.WriteOutput(AXIS_X, OUTPUT_007_SOL_Z_CYLINDER_RIGHT_DN, TRUE);

					SetSeqIdx(prc_idx, seq_idx, 7070);
					break;
				case 7070:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_LEFT_DN);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_LEFT_DN);

					if(m_Axis.ReadInput(AXIS_X, INPUT_005_Z_CYLINDER_LEFT_DN))
						SetSeqIdx(prc_idx, seq_idx, 7080);
					break;
				case 7080:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_LEFT_UP);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_LEFT_UP);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_004_Z_CYLINDER_LEFT_UP))
						SetSeqIdx(prc_idx, seq_idx, 7090);
					break;
				case 7090:
					
					SetSeqIdx(prc_idx, seq_idx, 7100);
					break;
				case 7100:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_RIGHT_DN);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_RIGHT_DN);

					if(m_Axis.ReadInput(AXIS_X, INPUT_007_Z_CYLINDER_RIGHT_DN))
						SetSeqIdx(prc_idx, seq_idx, 7110);
					break;
				case 7110:
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_CYL_SENSOR;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_Z_CYLINDER_RIGHT_UP);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_Z_CYLINDER_RIGHT_UP);

					if(!m_Axis.ReadInput(AXIS_X, INPUT_006_Z_CYLINDER_RIGHT_UP))	
						SetSeqIdx(prc_idx, seq_idx, 300);			//STEP#1 시작으로 이동
					break;
				//
				//[case 7000 ~ 7110] 실린더 검사 위치 이동 및 체크 끝
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


				case 8000:	//수직 검사
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_INSPECTION;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_VERT_INSP_FAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_VERT_INSP_FAIL);

					if(pDlgMain->GetVerInspectionRslt(0, TOP_CAMERA) && pDlgMain->GetVerInspectionRslt(0, BOTTOM_CAMERA))
						SetSeqIdx(prc_idx, seq_idx, 8010);
					break;
				case 8010:	//수평 검사
					m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_INSPECTION;
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, ALARM_MSG_HORI_INSP_FAIL);	
					sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_HORI_INSP_FAIL);

					if(pDlgMain->GetHorInspectionRslt(0, TOP_CAMERA) && pDlgMain->GetHorInspectionRslt(0, BOTTOM_CAMERA))
						SetSeqIdx(prc_idx, seq_idx, 400);	//STEP#2 Go
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

				//알람처리
				::SendMessage(m_hWndMain, USE_MSG_MOTION_TO_MAIN_DIALOG, 0, 0);
			}
			Sleep(1);
		}

		Sleep(1);
	}
}

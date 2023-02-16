
#include "StdAfx.h"
#include "Axis.h"
#ifndef TWO_CHANNEL_MODE

CAxis::CAxis(void)
{
	m_bConnected = FALSE;
	m_nPortNum = 0;

	for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
	{
		m_bAmp[i] = FALSE;
		m_bHome[i] = FALSE;
	}

	m_strAxisName[AXIS_Y] = _T("AXIS Y");
	m_strAxisName[AXIS_X] = _T("AXIS X");
	m_strAxisName[AXIS_Z] = _T("AXIS Z");

	m_dMm4Pulse[AXIS_Y] = AXIS_Y_MM4PULSE;
	m_dPulse4Mm[AXIS_Y] = AXIS_Y_PULSE4MM;

	m_dMm4Pulse[AXIS_X] = AXIS_X_MM4PULSE;
	m_dPulse4Mm[AXIS_X] = AXIS_X_PULSE4MM;

	m_dMm4Pulse[AXIS_Z] = AXIS_Z_MM4PULSE;
	m_dPulse4Mm[AXIS_Z] = AXIS_Z_PULSE4MM;

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
}

CAxis::~CAxis(void)
{
	Release();
}

BOOL CAxis::Initialize(BYTE nPortNum, DWORD dwBaudRate)
{
	BOOL rslt = TRUE;

	//접속
	if( !m_bConnected )
	{
#ifdef MOTION
		if( !FAS_Connect(nPortNum, dwBaudRate) )
		{
			rslt = FALSE;
		}
		else
		{
			m_nPortNum = nPortNum;
			m_bConnected = TRUE;
			
			//20190212 by ADD
			m_bIOReadThreadStart = FALSE;

			m_pIORead = AfxBeginThread(IOReadThread, this, THREAD_PRIORITY_NORMAL);
			m_pIORead->m_bAutoDelete = TRUE;

			for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
			{
				ServoOn(i);
			}
		}
#else
		m_bConnected = TRUE;
#endif
	}
	else
		rslt = FALSE;

	return rslt;
}


void CAxis::Release()
{
#ifdef MOTION
	m_bIOReadThreadStart = FALSE;
	m_bConnected = FALSE;
	for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
		ServoOff(i);

	FAS_Close(m_nPortNum);
#endif
	m_bConnected = FALSE;
}


void CAxis::IOReadThread()
{
	while(m_bConnected)
	{
		if(m_bIOReadThreadStart)
		{
			OutputAllRead();
			InputAllRead();

			for(int i = 0; i < MOT_STATUS_MAX_NUM; i++) 
			{
				m_bNegSensor[i] = GetNegLimitSensor(i);
				m_bPosSensor[i] = GetPosLimitSensor(i);

				m_lCurPos[i] = GetCurPos(i);
			}
		}	

		Sleep(100);
	}

}

UINT CAxis::IOReadThread(LPVOID pParam)
{
	CAxis *pdlg = (CAxis *)pParam;
	
	pdlg->IOReadThread();

	return 0;
}

BOOL CAxis::OutputAllRead()
{
	BOOL rslt = TRUE;
	int slave;

	for(int i = 0; i < MAX_OUT_PUT_CNT; i++)
	{
		slave = slave = (i <= 3) ? AXIS_Y : AXIS_X;

		m_bOutput[i] = ReadOutput(slave, m_dwIndexOutput[i]);
	}

	return rslt;
}

BOOL CAxis::InputAllRead()
{
	BOOL rslt = TRUE;
	int slave;

	for(int i = 0; i < MAX_IN_PUT_CNT; i++)
	{
		slave = slave = (i <= 3) ? AXIS_Y : AXIS_X;

		m_bInput[i] = ReadInput(slave, m_dwIndexInput[i]);
	}

	return rslt;
}


//Error
/*
	FMM_OK : 명령이 정상적으로 수행되었습니다.
	FMM_NOT_OPEN : 아직 Board 를 연결하기 전 입니다.
	FMM_INVALID_PORT_NUM : 연결한 Port 중에 nPort 는 존재하지 않습니다.
	FMM_INVALID_SLAVE_NUM : 해당 Port 에 iSlaveNo 의 Slave 는 존재하지 않습니다.
*/

//use not
BOOL CAxis::SetAxisVelocity(BYTE nSlaveNUM, DWORD nVel)
{
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

#ifdef MOTION
	nRtn = FAS_SetParameter(m_nPortNum, nSlaveNUM, SERVO_AXISSTARTSPEED, nVel);

	if(nRtn == FMM_OK)
	{

	}
	else
	{
		rslt = FALSE;
	}
#endif

	return rslt;
}

BOOL CAxis::AxisMoveDone(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;

	EZISERVO_AXISSTATUS AxisStatus;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetAxisStatus(m_nPortNum, nSlaveNum, &(AxisStatus.dwValue));

	if(nRtn == FMM_OK)
	{
		if ((AxisStatus.FFLAG_INPOSITION == 1) && (AxisStatus.FFLAG_MOTIONING == 0))
		{
			rslt = TRUE;
		}
	}
	else
	{

	}
#else 
	rslt = TRUE;
#endif

	return rslt;
}

//현재 Motor 의 Actual Position 값을 읽어온다
long CAxis::GetCurPos(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	long lActPos = 0;

#ifdef MOTION
	if (!m_bConnected)
		return -100;

	nRtn = FAS_GetActualPos(m_nPortNum, nSlaveNum, &lActPos);

	if(nRtn != FMM_OK)
	{

	}
#endif

	return lActPos;
}

//절대 이동
BOOL CAxis::Absmove(BYTE nSlaveNum, long nPos, DWORD nVel) 
{
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;
	
#ifdef MOTION
	if (!m_bConnected)
		return FALSE;

	//SERVO 상태 체크
	if(!m_bAmp[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}

	//HOME 상태 체크
	if(!m_bHome[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}

	nRtn = FAS_MoveSingleAxisAbsPos(m_nPortNum, nSlaveNum, nPos, nVel);

	if (nRtn != FMM_OK)
	{
			
	}

OUT_BREAK:
#endif

	return rslt;
}

//상대 이동
BOOL CAxis::Incmove(BYTE nSlaveNum, long nPos, DWORD nVel) 
{
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

#ifdef MOTION
	if (!m_bConnected)
		return FALSE;

	//SERVO 상태 체크
	if(!m_bAmp[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}

	//HOME 상태 체크
	if(!m_bHome[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}
			
	nRtn = FAS_MoveSingleAxisIncPos(m_nPortNum, nSlaveNum, nPos, nVel);

	if (nRtn != FMM_OK)
	{

	}

OUT_BREAK:
#endif

	return rslt;
}

BOOL CAxis::JogMove(BYTE nSlaveNum, long nVel, BOOL bDir)
{
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

#ifdef MOTION
	if (!m_bConnected)
		return FALSE;

	//SERVO 상태 체크
	if(!m_bAmp[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}

	//HOME 상태 체크
	if(!m_bHome[nSlaveNum])
	{
		rslt = FALSE;
		goto OUT_BREAK;
	}

	nRtn = FAS_MoveVelocity(m_nPortNum, nSlaveNum, nVel, bDir);		//bDir :   0:-  1:+

	if (nRtn != FMM_OK)
	{

	}

OUT_BREAK:
#endif

	return nRtn;
}
/*
int CAxis::JogStop(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;

	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}

	return nRtn;
}
*/

int CAxis::ServoOn(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

	EZISERVO_AXISSTATUS AxisStatus;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetAxisStatus(m_nPortNum, nSlaveNum, &(AxisStatus.dwValue));

	if (AxisStatus.FFLAG_SERVOON == SERVO_OFF)
		nRtn = FAS_ServoEnable(m_nPortNum, nSlaveNum, TRUE);

	if(nRtn == FMM_OK)
		m_bAmp[nSlaveNum] = TRUE;

	if (AxisStatus.FFLAG_ERRORALL || AxisStatus.FFLAG_ERROVERCURRENT || AxisStatus.FFLAG_ERROVERLOAD)
		nRtn = FAS_ServoAlarmReset(m_nPortNum, nSlaveNum);
#endif

	return nRtn;
}

int CAxis::ServoOff(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;

	EZISERVO_AXISSTATUS AxisStatus;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_GetAxisStatus(m_nPortNum, nSlaveNum, &(AxisStatus.dwValue));

	if (AxisStatus.FFLAG_SERVOON == SERVO_ON)
		nRtn = FAS_ServoEnable(m_nPortNum, nSlaveNum, FALSE);

	if(nRtn == FMM_OK)
	{
		m_bAmp[nSlaveNum] = FALSE;
		m_bHome[nSlaveNum] = FALSE;
	}
#endif

	return nRtn;
}

//한 축 정지
int CAxis::Stop(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}

//모든 축 정지
int CAxis::AllStop() 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllMoveStop(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}

//Single 원점
int CAxis::SingleHome(BYTE iSlaveNo) 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_MoveOriginSingleAxis(m_nPortNum, iSlaveNo);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}

//모든 축 원점
int CAxis::AllHome() 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllMoveOriginSingleAxis(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}
#else
	for(int i = 0; i < MOT_STATUS_MAX_NUM; i++)
	{
		m_bAmp[i] = TRUE;
		m_bHome[i] = TRUE;
	}
#endif

	return nRtn;
}


int CAxis::MotReadOriginResult(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	EZISERVO_AXISSTATUS AxisStatus;
	int rslt = 0;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, (BYTE)nSlaveNum, &(AxisStatus.dwValue));

	g_AxisSyncIntlock.Unlock();	//20190212 by ADD

	if(nRtn == FMM_OK)
	{
		if(AxisStatus.FFLAG_ORIGINRETOK == 1)
			rslt = 0;
		else if(AxisStatus.FFLAG_ORIGINRETURNING == 1)
			rslt = 1;
	}
	else
	{
		
	}
#endif

	return rslt;
}

//알람 리셋
int CAxis::AlarmReset(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_ServoAlarmReset(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}


//한 축 긴급 정지
int CAxis::Emergency(BYTE nSlaveNum) 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_EmergencyStop(m_nPortNum, nSlaveNum);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}

//모든 축 긴급 정지
int CAxis::AllEmergency() 
{
	int nRtn = FMM_OK;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	nRtn = FAS_AllEmergencyStop(m_nPortNum);

	if (nRtn != FMM_OK)
	{

	}
#endif

	return nRtn;
}

BOOL CAxis::GetPosLimitSensor(BYTE nSlaveNum)
{
	ULONGLONG dwInput = 0;
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

#ifdef MOTION
	g_AxisSyncIntlock.Lock();

	nRtn = FAS_GetIOInput((BYTE)m_nPortNum, nSlaveNum, &dwInput);
	
	g_AxisSyncIntlock.Unlock();
	
	if(nRtn == FMM_OK)
	{
		if(dwInput & SERVO_IN_BITMASK_LIMITP)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{

	}

	g_AxisSyncIntlock.Unlock();
#endif

	return rslt;
}

BOOL CAxis::GetNegLimitSensor(BYTE nSlaveNum)
{
	ULONGLONG dwInput = 0;
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;

#ifdef MOTION
	g_AxisSyncIntlock.Lock();

	nRtn = FAS_GetIOInput((BYTE)m_nPortNum, nSlaveNum, &dwInput);
	
	g_AxisSyncIntlock.Unlock();
	
	if(nRtn == FMM_OK)
	{
		if(dwInput & SERVO_IN_BITMASK_LIMITN)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{

	}
	 
	g_AxisSyncIntlock.Unlock();
#endif

	return rslt;
}


// IO 관련
int CAxis::WriteOutput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal)
{
	int rslt = TRUE;
	DWORD dwIOCLRMask = 0;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();	

	if (dwVal == TRUE)
	{
		if(FAS_SetIOOutput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwNo, dwIOCLRMask) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		if(FAS_SetIOOutput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwIOCLRMask, dwNo) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}		

	g_AxisSyncIntlock.Unlock();	
#endif

	return rslt;
}

int CAxis::ReadOutput(BYTE nSlaveNum, DWORD dwNo)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;
	DWORD dwIOOutput;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();	

	nRtn = FAS_GetIOOutput(m_nPortNum, nSlaveNum, &dwIOOutput);
	
	if(nRtn == FMM_OK)
	{
		if(dwIOOutput & dwNo)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		//rslt = FALSE;
	}

	g_AxisSyncIntlock.Unlock();	
#endif

	return rslt;
}

int CAxis::WriteInput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal)
{
	int rslt = TRUE;
	DWORD dwIOCLRMask = 0;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();	

	if (dwVal == TRUE)
	{
		if(FAS_SetIOInput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwNo, dwIOCLRMask) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{
		if(FAS_SetIOInput((BYTE)m_nPortNum, (BYTE)nSlaveNum, dwIOCLRMask, dwNo) == FMM_OK)
			rslt = TRUE;
		else
			rslt = FALSE;
	}		

	g_AxisSyncIntlock.Unlock();	
#endif

	return rslt;
}

int CAxis::ReadInput(BYTE nSlaveNum, DWORD dwNo)
{
	int nRtn = FMM_OK;
	BOOL rslt = FALSE;
	ULONGLONG dwIOInput;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();	

	nRtn = FAS_GetIOInput(m_nPortNum, nSlaveNum, &dwIOInput);
	
	if(nRtn == FMM_OK)
	{
		if(dwIOInput & dwNo)
			rslt = TRUE;
		else
			rslt = FALSE;
	}
	else
	{

	}

	g_AxisSyncIntlock.Unlock();	
#endif

	return rslt;
}

BOOL CAxis::AmpFaultState(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	EZISERVO_AXISSTATUS state;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, (BYTE)nSlaveNum, &(state.dwValue));

	if(nRtn == FMM_OK)
	{
		if(state.FFLAG_SERVOON == SERVO_ON)
			m_bAmp[nSlaveNum] = TRUE;
		else
			m_bAmp[nSlaveNum] = FALSE;
	}
	else
	{
		
	}

	g_AxisSyncIntlock.Unlock();
#else
	m_bAmp[nSlaveNum] = TRUE;
#endif

	return m_bAmp[nSlaveNum];
}


BOOL CAxis::AmpPowerState(BYTE nSlaveNum)
{
	int nRtn = FMM_OK;
	BOOL rslt = TRUE;
	EZISERVO_AXISSTATUS state;

#ifdef MOTION
	if (!m_bConnected)
		return -1;

	g_AxisSyncIntlock.Lock();

	nRtn = FAS_GetAxisStatus((BYTE)m_nPortNum, (BYTE)nSlaveNum, &(state.dwValue));

	if(nRtn == FMM_OK)
	{
		if(state.FFLAG_ERRMOTORPOWER == 0)
		{
			rslt = TRUE;
		}
		else
		{
			m_bAmp[nSlaveNum] = FALSE;
			m_bHome[nSlaveNum] = FALSE;

			rslt = FALSE;
		}
	}
	else
	{
		m_bAmp[nSlaveNum] = FALSE;
		m_bHome[nSlaveNum] = FALSE;

		rslt = FALSE;
	}

	g_AxisSyncIntlock.Unlock();
#else
	m_bAmp[nSlaveNum] = TRUE;
	m_bHome[nSlaveNum] = TRUE;
#endif

	return rslt;
}


BOOL CAxis::AllAxisIsHome()
{
	BOOL rslt = TRUE;
	
	for(int i = 0; i < MAX_MOTION_CNT; i++)
	{
		if(!m_bHome[i] || !m_bAmp[i])
		{
			rslt = FALSE;
			break;
		}
	}

	return rslt;
}

#endif
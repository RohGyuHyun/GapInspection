
#pragma once
#include "Axis_Io_def.h"
#ifndef TWO_CHANNEL_MODE
#define		MAX_OUT_PUT_CNT	12
#define		MAX_IN_PUT_CNT	12

class CAxis
{

public:
	CAxis(void);
	~CAxis(void);

protected:
	CCriticalSection g_AxisSyncIntlock;

	BOOL m_bAmp[MOT_STATUS_MAX_NUM];
	BOOL m_bHome[MOT_STATUS_MAX_NUM];
	BOOL m_bNegSensor[MOT_STATUS_MAX_NUM];
	BOOL m_bPosSensor[MOT_STATUS_MAX_NUM];
	BOOL m_bConnected;
	BYTE m_nPortNum;

	double m_dMm4Pulse[MOT_STATUS_MAX_NUM];
	double m_dPulse4Mm[MOT_STATUS_MAX_NUM];

	CString m_strAxisName[MOT_STATUS_MAX_NUM];

	CWinThread *m_pIORead;
	BOOL m_bIOReadThreadStart;

	//20190212
	UINT static IOReadThread(LPVOID pParam);
	void IOReadThread();
	BOOL OutputAllRead();
	BOOL InputAllRead();
	BOOL m_bInput[MAX_IN_PUT_CNT];
	BOOL m_bOutput[MAX_OUT_PUT_CNT];
	DWORD m_dwIndexInput[MAX_IN_PUT_CNT];
	DWORD m_dwIndexOutput[MAX_OUT_PUT_CNT];
	long m_lCurPos[MOT_STATUS_MAX_NUM];
		
public:
	//20190212
	BOOL GetNegSensor( int nIdx) { return m_bNegSensor[nIdx]; }
	BOOL GetPosSensor( int nIdx) { return m_bPosSensor[nIdx]; }
	BOOL GetOutput(int nIdx) { return m_bOutput[nIdx]; }
	BOOL GetInput(int nIdx) { return m_bInput[nIdx]; }
	void SetReadIOStart(BOOL bFlag) { m_bIOReadThreadStart = bFlag; }
	long GetCurrentPos(int nIdx) { return m_lCurPos[nIdx]; }

public:
	BOOL Initialize(BYTE nPortNum, DWORD dwBaudRate = DEFAULT_BAUDRATE);
	void Release();

	int ServoOn(BYTE nSlaveNum);								//서보 ON. OFF 상태일 때만 ON
	int ServoOff(BYTE nSlaveNum);								//서보 OFF.
	BOOL Absmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//절대 이동
	BOOL Incmove(BYTE nSlaveNum, long nPos, DWORD nVel) ;		//상대 이동
	BOOL JogMove(BYTE nSlaveNum, long nVel, BOOL bDir);			//조그 이동		0:DIR_DEC	1:DIR_INC
	//int JogStop(BYTE nSlaveNum);								//조그 스탑
	long GetCurPos(BYTE nSlaveNum);								//현재 위치
	BOOL AxisMoveDone(BYTE nSlaveNum);								//이동 완료 체크
	int SetAxisVelocity(BYTE nSlaveNUM, DWORD nVel);			

	int Stop(BYTE nSlaveNum);
	int AllStop();
	int SingleHome(BYTE iSlaveNo);
	int AllHome();
	int AlarmReset(BYTE nSlaveNum);
	int Emergency(BYTE nSlaveNum);
	int AllEmergency() ;
	BOOL AmpFaultState(BYTE nSlaveNum);
	BOOL AmpPowerState(BYTE nSlaveNum);
	int MotReadOriginResult(BYTE nSlaveNum);


	BOOL GetPosLimitSensor(BYTE nSlaveNum);
	BOOL GetNegLimitSensor(BYTE nSlaveNum);

	// IO 관련....
	int WriteOutput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal);
	int ReadOutput(BYTE nSlaveNum, DWORD dwNo);
	int WriteInput(BYTE nSlaveNum, DWORD dwNo, DWORD dwVal);
	int ReadInput(BYTE nSlaveNum, DWORD dwNo);

	double GetPulse4Mm(BYTE nSlaveNum) { return m_dPulse4Mm[nSlaveNum]; }
	double GetMm4Pulse(BYTE nSlaveNum) { return m_dMm4Pulse[nSlaveNum]; }
	BOOL GetAmpFaultState(BYTE nSlaveNum) { return m_bAmp[nSlaveNum]; }
	void SetAxisHomeFinished(BYTE nSlaveNum, BOOL bFlag) { m_bHome[nSlaveNum] = bFlag; }
	BOOL AxisIsHomeFinished(BYTE nSlaveNum) { return m_bHome[nSlaveNum]; }
	CString AxisGetAxisName(BYTE nSlaveNum) { return m_strAxisName[nSlaveNum]; }
	BOOL AllAxisIsHome();
};

#endif
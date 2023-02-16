#pragma once

#include "Axis\Axis.h"
//#include "LogFile\LogFile.h"


#define SEQ_STATUS_STEP1			0
#define SEQ_STATUS_STEP2			1
#define SEQ_STATUS_STEP3			2
#define SEQ_STATUS_STEP4			3
#define SEQ_STATUS_READY			5
#define SEQ_STATUS_END				6

//Motion Process Status///////////////////////////////////////////////////////
typedef struct MotionProcessStatus
{
	//Timeout
	double m_dTimeoutS;//시작 시간
	double m_dTimeoutE;//종료 시간(현재)
	double m_dTimeout;//지정된 시간

	//Status문자(에러 또는 정상종료시 출력할 문자열)
	char m_cNGStatus[MAX_CHAR];
	char m_cNGCode[MAX_CHAR];
	char m_cTypeStatus[MAX_CHAR];

	int m_iNGSeqIdx;

}typeMotionProcessSts;


//Motion Sequence///////////////////////////////////////////////////////
typedef struct MotionSeq
{
	//Sequence
	int m_iSubSeqIdx;
	int m_iNextSubSeqIdx;

	//Flag
	BOOL m_bAutoStart;
	BOOL m_bAlarm;

	//버튼중심의 동작상태
	BOOL m_bOpSts;

}typeMotionSeq;

class CMotionData
{
public:
	CMotionData(void);
	~CMotionData(void);

	//Motion process status
	typeMotionProcessSts	m_ProSts[MOT_STATUS_MAX_NUM][MOT_STATUS_SUB_MAX_NUM];

	//Motion sequence
	typeMotionSeq			m_Seq[MOT_STATUS_MAX_NUM];

	void ResetSeq();
};



//Motion flag///////////////////////////////////////////////////////
typedef struct MotionFlag
{
	BOOL m_bProcessThread;
	BOOL m_bAutoStart;
	BOOL m_bFirstStart;
	BOOL m_bAlarm;
	BOOL m_bAreaSensor;

}typeMotionFlag;



typedef struct SequenceFlag
{
	BOOL					m_bInitialized;
	BOOL					m_bByPass;
	int						m_nSeqIndex;		// 0:대기위치   1:스텝#1   2:스텝#2		3:스텝#3
	int						m_nMotSubSeqIdx;
	
	long					m_lAxisPos[4];		//AXIS_X, AXIS_Y, AXIS_Z1, AXIS_Z2
	long					m_lAxisVel[MOT_STATUS_MAX_NUM];

}typeSequenceFlag;


class CSequenceModule
{
public:
	CSequenceModule(void);
	~CSequenceModule(void);

protected:
	HWND					m_hWndMain;

public:
	typeSequenceFlag		m_SeqFlag;
	typeMotionFlag			m_Flag;
	CMotionData				m_MDat[MOT_PROCESS_MAX_NUM];
#ifndef TWO_CHANNEL_MODE
	CAxis					m_Axis;
#endif
	CLogFile				*m_pMotLog;

	BOOL					m_bIndexZ;		//0: Z1   1: Z2

	BOOL m_bswStart[2];
	BOOL m_bSeqEnd;
	//20190315 ngh
	BOOL m_bSeqProgress;

	int m_nSeqChannel;
	int m_nGrabStepIndex;
	int m_nMaxStepCnt;
	int m_nSeqIdx;
	int m_nDstDeqIdx;
	BOOL m_bReadyState;
	BOOL m_bInspecting;

	CTimerCheck m_TimeCheck;

	BOOL m_bInitSeq;

	int m_nPrc_Idx;
	int m_nSeq_Idx;
	int m_nSeq_Sub_Idx;
public:
	//Motion Sequence
	int MotionProcess(int prc_idx, int seq_idx);
	int StopModeOperation(int *prc_idx, int *seq_idx);
	int CheckTimeout(int prc_idx, int seq_idx, int seq_sub_idx, char* status_type);
	int CheckDelay(int prc_idx, int seq_idx, int delay);
	void SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx);
	void SequenceLoop();
	void Open(HWND hWnd, int nChannel);
	void Close();

	void Reset();
	BOOL Start();
	void Stop();

	void SetLamp(int nColor);//, BOOL bBuzz);

	void InitSeq();

	BOOL GetIndexZ()	{ return m_bIndexZ; }
	void SetSeqFirst(BOOL bFlag) { m_Flag.m_bFirstStart = bFlag; }

	//20190215 ngh
	BOOL GetSeqEnd(){return m_bSeqEnd;};
	void SetSeqEnd(BOOL isEnd){m_bSeqEnd = isEnd;};

	//20190315 ngh
	BOOL GetSeqProgress(){return m_bSeqProgress;};

	void SetSeqChannel(int nChannel){m_nSeqChannel = nChannel;};
	void SetGrabStepIndex(int nIndex){m_nGrabStepIndex = nIndex;};
	int GetGrabStepIndex(){return m_nGrabStepIndex;};
	void SetMaxStepCnt(int nCnt){m_nMaxStepCnt = nCnt;};
	BOOL GetAutoStart(){return m_Flag.m_bAutoStart;};
	BOOL GetReadyState(){return m_bReadyState;};
	BOOL GetInspecting(){return m_bInspecting;};

	BOOL GetInspTimeOut();

	void GetSeqIdx(int *prc_idx, int *seq_idx, int *seq_sub_idx);
	int GetSeqSubIdx(int prc_idx, int seq_idx);
};


/*
	[노트]

	- AUTO / START / RESET / STOP / INITIALIZE  동작 잡기
	
	- DRY RUN 테스트

	- 시퀀스 완성도 올리기

	- 로그 작업


*/
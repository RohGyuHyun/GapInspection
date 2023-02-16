
// GapInspectionDlg.h : 헤더 파일
//

#pragma once

#include "Vision.h"
#include "DialogTeach.h"
#include "DialogZoomView.h"
#include "DialogMotion.h"
#include "DialogInspPoint.h"
#include "DialogInspRslt.h"
#include "DialogAlarmMsg.h"
#include "DialogBarcodePopup.h"
#include "DialogAvgRsltView.h"
#include "Motion\SequenceModule.h"
#include "Light\COMMTHREAD.H"
#include "UserSockClient.h"
#include "afxwin.h"
//#include "Barcode\RsPort.h"

#include <windows.h>
#include <tchar.h>

#include <setupapi.h>
#include <initguid.h>

#include <stdio.h>

// This is the GUID for the USB device class
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
			0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
// (A5DCBF10-6530-11D2-901F-00C04FB951ED)

class CGapInspectionDlg : public CDialogEx
{
	// 생성입니다.
public:
	CGapInspectionDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	enum { IDD = IDD_GAPINSPECTION_DIALOG_2_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
#ifdef LIGHT_TEST
	CLogFile *m_cTestLogFile;
#endif

	CLogFile *m_cLogFile;
	CVision *m_Vision[MAX_CHANNEL_CNT];
	CDialogTeach *m_dlgTeach[MAX_CHANNEL_CNT];
	CDialogZoomView *m_dlgZoomView;
	CDialogInspPoint *m_dlgInspPoint[MAX_CHANNEL_CNT];
	CDialogInspRslt *m_dlgInspRslt[MAX_CHANNEL_CNT];
	CDialogAlarmMsg *m_pDlgAlarm[MAX_CHANNEL_CNT];
	CDialogAvgRsltView *m_dlgAvgRsltView[MAX_CHANNEL_CNT];
	CDialogSystem *m_dlgSystem;
	CAutoDelete *m_cAutoDelete;
	CDialogBarcodePopup *m_dlgBarcode[MAX_CHANNEL_CNT];

	CStaticMatDisplay m_MainDisplay[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	CBtnenhctrl m_btn_Status[MAX_CHANNEL_CNT];
	CBtnenhctrl m_btn_TackTime[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainTime;
	CBtnenhctrl m_ctrMainSystemVer;
	CBtnenhctrl m_ctrMainTotalCnt[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainOkCnt[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainNgCnt[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainOkRatio[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainWorkModel[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainCamConnect[MAX_CAM_CNT];
	CBtnenhctrl m_btn_Auto[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainBarcode[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrInspPointLabel[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	CBtnenhctrl m_ctrMainRslt[MAX_CHANNEL_CNT];
	CBtnenhctrl m_ctrMainPLCState;
	CBtnenhctrl m_ctrBarcodeReset[MAX_CHANNEL_CNT];

	CBtnenhctrl m_ctrMainSpec[MAX_CHANNEL_CNT][MAX_RSLT_POINT_CNT];

	BOOL m_bAutoInsp[MAX_CHANNEL_CNT];
	int m_nMainTapIdx;
	BOOL m_bCamConnect[MAX_CAM_CNT];
	int m_nManualInspCamIdx;

	int m_iCount[MAX_CHANNEL_CNT];	// Total Count
	int m_iOKCount[MAX_CHANNEL_CNT];
	int m_iNGCount[MAX_CHANNEL_CNT];
	int m_iPointNGConunt[MAX_CHANNEL_CNT][MAX_RSLT_POINT_CNT];
	CString m_strUseModelName;
	
	CCameraManager m_CameraManager;

	CString m_strCamSerial[MAX_CAM_CNT];

	char m_szCamName[MAX_CAM_NUM][100];         // 모델 이름
	char m_szSerialNum[MAX_CAM_NUM][100];       // Serial Number
	char m_szInterface[MAX_CAM_NUM][100];       // Inter face 방식
	int m_iCamPosition[MAX_CAM_NUM];           // 연결된 카메라 순서
	int m_nCamIndexBuf[MAX_CAM_NUM];
	int nFrameCount[MAX_CAM_NUM];
	int m_iCamNumber;			                // 연결된 카메라 수
	int m_iCameraIndex;						// 프로그램에서 사용할 카메라 인덱스 넘버 
	int m_error;
	int m_iListIndex;                       // Listcontrol 인덱스
	int m_nInfoIdx;

	LARGE_INTEGER					freq[MAX_CAM_NUM],
									start[MAX_CAM_NUM],
									end[MAX_CAM_NUM];

	bool bStopThread[MAX_CAM_NUM];               // LiveThread flag
	bool bLiveFlag[MAX_CAM_NUM];                 // Live mode flag
	bool m_bLiveFlag[MAX_CAM_NUM];
	BOOL m_bLiveState[MAX_CAM_NUM][MAX_GRAB_CNT];
	int m_nLiveCamIdx;
	int m_nLiveCamGrabIdx;
	BOOL m_bCamLive[MAX_CAM_NUM];
	int m_nCamReconnectCnt[MAX_CAM_NUM];

	CTime m_cCurTime;
	CString m_strCurTime;


	CString m_strStartDate;//20190218 ngh
	CString m_strStartTime[MAX_CHANNEL_CNT];
	CString m_strEndTime[MAX_CHANNEL_CNT];
	CString m_strTackTime[MAX_CHANNEL_CNT];

	CWinThread *m_pThread[MAX_CHANNEL_CNT];		//MOTION Seq Thread

	int m_nChannelIdx;
	int m_nSelectTrackerIdx;;

	vector<TypeSeqLightPara> m_SeqLightPara[MAX_CHANNEL_CNT];

	CString m_strBarcode[MAX_CHANNEL_CNT];

	TypeSystemPara m_SystemPara;
	vector<TypeSeqStepPara> m_SeqStepPara[MAX_CHANNEL_CNT];
	int m_nSeqStepIndex[MAX_CHANNEL_CNT];
	BOOL m_bSeqEnd[MAX_CHANNEL_CNT];

	CTime m_cStart[MAX_CHANNEL_CNT];
	CTime m_cEnd[MAX_CHANNEL_CNT];
	long m_lStartTime[MAX_CHANNEL_CNT];
	long m_lEndTime[MAX_CHANNEL_CNT];
	double m_dTackTime[MAX_CHANNEL_CNT];

	vector<BOOL> m_bInspectionEnd[MAX_CHANNEL_CNT];
	IplImage *m_DispTempImage;
	BOOL m_bInspRslt[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	double m_dInspRslt[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	BOOL m_bInspPointRslt[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	TypeSeqPara m_SeqPara[MAX_CHANNEL_CNT];

	CWinThread* m_pThreadGrab[MAX_CAM_CNT];
	BOOL m_bGrabThreadStart[MAX_CAM_CNT];
	BOOL m_bGrabThreadEnd[MAX_CAM_CNT];
	int m_nGrabIdx[MAX_CAM_CNT];
	BOOL m_bSeqGrabEnd[MAX_CAM_CNT];

	CWinThread* m_pThreadGrabEndCheck[MAX_CHANNEL_CNT];
	BOOL m_bThreadGrabEndCheckEnd[MAX_CHANNEL_CNT];

	CWinThread* m_pThreadInspEndCheck[MAX_CHANNEL_CNT];
	BOOL m_bInspEndCheckThreadStart[MAX_CHANNEL_CNT];
	BOOL m_bInspEndCheckThreadEnd[MAX_CHANNEL_CNT];

	CWinThread* m_pThreadImageSave[MAX_CHANNEL_CNT];
	BOOL m_bInspectionRslt[MAX_CHANNEL_CNT];
	IplImage *m_SaveImage[MAX_CHANNEL_CNT][MAX_INSP_POINT_CNT];
	//IplImage *m_SaveGrabImage[MAX_CAM_CNT][MAX_GRAB_CNT][MAX_INSP_POINT_CNT];

	BOOL m_bInspLogCopy[MAX_CHANNEL_CNT];
	BOOL m_bInspLogCopyWrite[MAX_CHANNEL_CNT];
	BOOL m_bInspLogCopyWritingWait[MAX_CHANNEL_CNT];
	CString m_bSrcInspLogFilePath[MAX_CHANNEL_CNT];
	CString m_bDstInspLogFilePath[MAX_CHANNEL_CNT];

	CClientSock *m_Client;
	BOOL m_bClientConnect;
	BOOL m_bClientRecive;
	BOOL m_bClientConnectFailCheck;
	CString m_strRslt[MAX_CHANNEL_CNT];
	int m_nClientAutoReconnectCnt;

	BOOL m_bSamsungLogCopy[MAX_CHANNEL_CNT];
	BOOL m_bSamsungLogCopyWrite[MAX_CHANNEL_CNT];
	BOOL m_bSamsungLogCopyWritingWait[MAX_CHANNEL_CNT];
	CString m_bSrcSamsungLogFilePath[MAX_CHANNEL_CNT];
	CString m_bDstSamsungLogFilePath[MAX_CHANNEL_CNT];

	vector<typeDGSFilePara> m_DGSFile[MAX_CHANNEL_CNT];
	CWinThread* m_pThreadDGSFileCheck[MAX_CHANNEL_CNT];
	BOOL m_bDGSFileCheckThreadStart[MAX_CHANNEL_CNT];
	BOOL m_bDGSFileCheckThreadEnd[MAX_CHANNEL_CNT];

	BOOL m_bStopInitSeq[MAX_CHANNEL_CNT];
#ifdef TWO_CHANNEL_MODE
	CSerialPort* m_pSerialComm;
#endif

	CTime m_cGrabStart[MAX_CHANNEL_CNT];
	long m_lGrabStart[MAX_CHANNEL_CNT];

	int m_nTestCnt[2];

	int m_nSeqIndex[MAX_CHANNEL_CNT];

	BOOL m_bAutoState[MAX_CHANNEL_CNT];

#ifdef TEST_RUN
	CTimerCheck m_cTimeOut[MAX_CHANNEL_CNT];
#endif

	BOOL m_bReadPlcVal[MAX_CHANNEL_CNT];
	int m_nReadPlcTimeOut[MAX_CHANNEL_CNT];

	CCriticalSection m_RWPlcCritical;
	CCriticalSection m_ReadPlcCritical;
	CCriticalSection m_WritePlcCritical;


	int m_nKeyboardIdx;
	BOOL m_bBarcodeInputEnd[MAX_CHANNEL_CNT];
	CString m_strBarcodeInfo[MAX_CHANNEL_CNT];

	CBarcodeRsPortType *m_pBarcode[MAX_CHANNEL_CNT];

#ifdef ASCII_READER
	BYTE m_byReadBarcode[MAX_CHANNEL_CNT][MAX_PATH];
	int m_nReadLenght[MAX_CHANNEL_CNT];
#endif
#ifdef SERIAL_BARCODE
	CRsPort *m_pBarcodeComm[MAX_CHANNEL_CNT];
	CWinThread* m_pThreadBarcodeRead[MAX_CHANNEL_CNT];
	BOOL m_bThreadBarcodeStart[MAX_CHANNEL_CNT];
	BOOL m_bThreadBarcodeEnd[MAX_CHANNEL_CNT];
#endif

#ifdef NEXT_BARCODE_READ
	CString m_strNextBarcode[MAX_CHANNEL_CNT];
	vector<typeBarcodeString> m_vBarcode[MAX_CHANNEL_CNT];
#endif

#ifdef KEY_BARCODE
	CString m_strKeyBarcode[MAX_CHANNEL_CNT];
#endif


	CCriticalSection m_LightCritical;

public:
	void CamLive();
	BOOL SetGrab(int nChannelIdx, int nSeqIdx, int nCamIdx, int nGrabIdx, BOOL isDisplay = FALSE);
	BOOL SetLightOnof(int nChannelIdx, int nLightIdx, int nLightVal = 0);
	BOOL SetLive(int nChannelIdx, int nCamIdx, int nGrabIdx){return CamLiveSetting(nChannelIdx, nCamIdx, nGrabIdx);};
	void SetInspection(int nChannelIdx, int nSeqIdx, int nCamIdx = -1);

	void SetBarcode(int nChannel, CString strBarcode);
	CString GetBarcode(int nChannel){return m_strBarcode[nChannel];};
	int GetSeqStepIndex(int nChannel){return GetReadPlcVal(nChannel);};
	int GetSeqStep(int nChannel){return m_nSeqStepIndex[nChannel];};
	BOOL GetSeqStepStart(int nChannel);
	BOOL GetSeqStepEnd(int nChannel);
	void SetSeqStepStart(int nChannel, BOOL isVal){if(m_nSeqStepIndex[nChannel] == -1) return; m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = isVal;};
	void SetSeqStepEnd(int nChannel, BOOL isVal){if(m_nSeqStepIndex[nChannel] == -1) return; m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = isVal;};

	void SetStartTime(int nChannel, CTime *cStart);
	void SetEndTIme(int nChannel, CTime *cEnd);
	void SetSeqEnd(int nChannel, BOOL isEnd){m_bSeqEnd[nChannel] = isEnd;};
	BOOL GetSeqEnd(int nChannel){return m_bSeqEnd[nChannel];};
	void SetInitInspection(int nChannel){InitInspection(nChannel);};
	void SetInitSeqStepPara(int nChannel);

	BOOL SetSendPacket(int nChannel, int nSeqStepIdx, BOOL isError = FALSE);
	BOOL GetRecivePacket(int nChannel, CString strRcvPacket);
	void SetSeqGrab(int nChannel, int nSeqIdx, int nGrabIdx);
	BOOL SetSeqLightOnOff(int nChannel, int nSeqIdx, int nCamIdx, int nGrabIdx, BOOL isOn = TRUE);
	int GetGrabCnt(int nChannel, int nCamIdx);
	void SetSeqStepIndex(int nChannel, int nIdx){m_nSeqStepIndex[nChannel] = nIdx;};
	void SetInspEndCheck(int nChannel){InspEndCheck(nChannel);};
	void SetInspRslt(int nChannel, BOOL isOK);
	BOOL GetCamConnect(int nCamIdx);
	void SetMainWriteLog(CString strLog);
	void SetAlarmMessage(int nChannel, int nAlarmCode);
	void SetStopInitSeq(int nChannel){m_bStopInitSeq[nChannel] = TRUE;};

	BOOL m_bReadPlcCheck[MAX_CHANNEL_CNT];

	void SetLightOnOff(int nCh, BOOL isOn);

	void WritePacket(int nCh, BYTE byVal);

	void SetSeqGrabStart(int nChannel, int nSeqIdx, int nGrabIdx);

	BOOL GetDryRun(){return FALSE;};

	BOOL SetReleaseGrabThread(int nChannel);

	int SetAutoStateWrite(int nChannel, BOOL isOnOff);

	int GetReadPlcVal(int nChannel);

	double GetTeckTime(int nChannel){return m_dTackTime[nChannel];};

	void SetInspEndCheckTimer(int nChannel);

	BOOL GetInspecting(int nChannel, int nStepIdx){return m_Vision[nChannel]->GetInspecting(nStepIdx);};

#ifdef SERIAL_BARCODE
	CString GetBarcodeReader(int nChannel);
	void ReadBarcodeThread(int nChannel);
	BOOL GetBarcodeReadEnd(int nChannel){return m_bBarcodeInputEnd[nChannel];};
#endif
	CString GetBarcodeString(int nChannel){return m_strBarcode[nChannel];};
protected:
	BOOL InitDialog();
	BOOL ReleaseDialog();
	BOOL ShowSelectMenu(int nChannel, int nDialogIdx);

	LRESULT OnZoomViewToMainMessage(WPARAM para0, LPARAM para1);
	LRESULT OnTeachToMainMessage(WPARAM para0, LPARAM para1);
	LRESULT OnMotMessage(WPARAM para0, LPARAM para1);
	LRESULT OnSystemToMainMessage(WPARAM para0, LPARAM para1);
	LRESULT OnInspRsltToMainMessage(WPARAM para0, LPARAM para1);
	LRESULT OnMainGrabDisplayCh0Message(WPARAM para0, LPARAM para1);
	LRESULT OnMainGrabDisplayCh1Message(WPARAM para0, LPARAM para1);
	LRESULT OnMainInspEndDisplayMessage(WPARAM para0, LPARAM para1);

	void SetAutoState(int nChannel, BOOL isAuto);

	void CountView(int nChannel);
	void CountOKAdd(int nChannel);
	void CountNGAdd(int nChannel, BOOL *bPointRslt);
	void CountReset(int nChannel);
	int CountLoad(int nChannel);
	void LoadDayCountData(int nChannel, CString strModel);
	void SaveDayCountData(int nChannel, CString strModel);

	BOOL OpenCamera(int iIndex);
	BOOL CloseCamera(int iIndex);
	BOOL ConnectCamera(int iIndex);
	void InitGigaECam(void);
	void GetSerialNumerFromFile(void);
	
	void SelectCamConnect();
	BOOL SetSelectCamClose(int iIndex){return CloseCamera(iIndex);};
	BOOL SetSelectCamConnect(int iIndex){return ConnectCamera(iIndex);};
	
	BOOL GrabSingle(int nChannel, int nCamIdx, int nGrabIdx);
	BOOL CamLiveSetting(int nChannelIdx, int nCamIdx, int nGrabIdx);
	static UINT LiveGrabThreadCam(void *lParam);

	BOOL SavelpImage(int nChannel, int nCamIdx, int nInspIdx);
	BOOL LoadlpImage(int nChannel, int nCamIdx, int nInspIdx);

	UINT static   MotionThread0(LPVOID pParam);
	UINT static   MotionThread1(LPVOID pParam);

	void SetSeqLightPara(int nChannel);

	void DisplayModelName(CString strModel);
	void InitInspection(int nChannel);
	void SetInitBarcode(int nChannel);
	void InitRsltPointDisplay(int nChannel);

	void ThreadGrab(int nCamIdx);
	UINT static GrabThread0(LPVOID pParam);
	UINT static GrabThread1(LPVOID pParam);
	UINT static GrabThread2(LPVOID pParam);
	UINT static GrabThread3(LPVOID pParam);

	void ThreadGrabEndCheck(int nChannel);
	UINT static GrabEndCheckThread0(LPVOID pParam);
	UINT static GrabEndCheckThread1(LPVOID pParam);

	void ReleaseThread(int nChannel);
	BOOL WaitGrabThreadEndCheck(int nCmaIdx);
	BOOL WaitGrabEndCheckThreadEndCheck(int nChannel);

	void InspEndCheck(int nChannel);
	void ThreadInspEndCheck(int nChannel);
	UINT static InspEndCheckThread0(LPVOID pParam);
	UINT static InspEndCheckThread1(LPVOID pParam);

	void SetMainDisplay(int nChannel, int nInspIdx);
	void EachRsltDisplay(int nChannel, int nInspPoint);
	BOOL WaitInspEndCheckThread(int nChannel);
	void InspEndRsltDisplay(int nChannel);
	BOOL RsltDisplay(int nChannel);

	UINT static ImageSaveThread0(LPVOID pParam);
	UINT static ImageSaveThread1(LPVOID pParam);
	void ThreadImageSave(int nChannel, BOOL isInspectionRslt);
	BOOL WaitImageSaveThreadEnd(int nChannel);
	BOOL GetInspectionRslt(int nChannel){return m_bInspectionRslt[nChannel];};
	void SaveInspResultReport(int nChannel, CString sMsg);

	BOOL WriteCsvLog(CStdioFile *pFile, CString strLog);
	BOOL WriteSamsungLog(int nChannel);

	void SetZoomView(int nChannel, int nCamIdx);

	void ThreadWriteDGSFile(int nChannel);
	UINT static WriteDGSFileThread0(LPVOID pParam);
	UINT static WriteDGSFileThread1(LPVOID pParam);
	void WriteDGSFile(int nChannel);
	BOOL WaitWriteDGSFileThreadEnd(int nChannel);

	int ReadPlcVal(int nChannel, int nSeq);
	int WritePlcVal(int nChannel, int nSeq);

	void AllKillTimer();

	void BarcodeReset(int nChannel);

	BOOL BarcodeReaderTimer(int nChannel);

#ifdef NEXT_BARCODE_READ
	void SetNextBarcode(int nChannel, CString strBarcode);
#endif

#ifdef SERIAL_BARCODE
	UINT static BarcodeReadThread0(LPVOID pParam);
	UINT static BarcodeReadThread1(LPVOID pParam);
#endif

	void NGRatioView(BOOL isShow);
	CRect rectTotalCnt[MAX_CHANNEL_CNT][2];
	CRect rectNGRatioCnt[MAX_CHANNEL_CNT][2];
// 구현입니다.
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlMainAuto();
	void ClickBtnenhctrlMainAuto2();
	void ClickBtnenhctrlMainTeach();
	void ClickBtnenhctrlMainTeach2();
	void ClickBtnenhctrlMainSystem();
	void ClickBtnenhctrlMainExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ClickBtnenhctrlZoomView0();
	void ClickBtnenhctrlZoomView2();
	void ClickBtnenhctrlZoomView1();
	void ClickBtnenhctrlZoomView3();
	void ClickBtnenhctrlMainCountReset();
	void ClickBtnenhctrlMainCountReset2();
	void ClickBtnenhctrlMainAvgRsltView();
	void ClickBtnenhctrlMainAvgRsltView2();
	afx_msg LRESULT OnReceive(WPARAM, LPARAM);
	afx_msg LRESULT OnConnect(WPARAM, LPARAM);
	afx_msg LRESULT OnClose(WPARAM, LPARAM);
	void ClickBtnenhctrlCam0Connect();
	void ClickBtnenhctrlCam1Connect();
	void ClickBtnenhctrlCam0Connect2();
	void ClickBtnenhctrlCam1Connect2();
	void ClickBtnenhctrlMainPlcState();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	void ClickBtnenhctrlMainBarcodeReset1();
	void ClickBtnenhctrlMainBarcodeReset2();
	void DblClickBtnenhctrlMainTotalCnt();
	void DblClickBtnenhctrlMainTotalCnt2();
	void DblClickBtnenhctrlMainOkCnt();
	void DblClickBtnenhctrlMainOkCnt2();
	void DblClickBtnenhctrlMainNgCnt();
	void DblClickBtnenhctrlMainNgCnt2();	
	void DblClickBtnenhctrlMainTackTime();
	void DblClickBtnenhctrlMainTackTime2();
	void DblClickBtnenhctrlMainOkRatio();
	void DblClickBtnenhctrlMainOkRatio2();
};
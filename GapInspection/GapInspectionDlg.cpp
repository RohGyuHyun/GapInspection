
// GapInspectionDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "GapInspection.h"
#include "GapInspectionDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSequenceModule g_SeqModule[MAX_CHANNEL_CNT];		//시퀀스 모듈 전역 객체

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CGapInspectionDlg 대화 상자
CGapInspectionDlg::CGapInspectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGapInspectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
#ifdef LIGHT_TEST
	m_cTestLogFile = NULL;
#endif
	m_cLogFile = NULL;
	m_dlgZoomView = NULL;
	m_bClientConnect = FALSE;
	m_bClientConnectFailCheck = FALSE;
	m_nClientAutoReconnectCnt = 0;
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		m_Vision[i] = NULL;
		m_pDlgAlarm[i] = NULL;
		m_dlgTeach[i] = NULL;
		m_dlgInspPoint[i] = NULL;
		m_dlgInspRslt[i] = NULL;
		m_iCount[i] = 0;
		m_iOKCount[i] = 0;
		m_iNGCount[i] = 0;
		m_bAutoInsp[i] = FALSE;

		m_nTestCnt[i] = 0;

		m_pThreadInspEndCheck[i] = NULL;
		m_pThreadImageSave[i] = NULL;
		m_pThreadGrabEndCheck[i] = NULL;
		m_pThread[i] = NULL;

		m_bBarcodeInputEnd[i] = FALSE;
	}
	for(int i = 0; i < MAX_CAM_NUM; i++)
	{
		m_nCamReconnectCnt[i] = 0;
		m_bSeqGrabEnd[i] = FALSE;
		m_pThreadGrab[i] = NULL;
		m_bCamConnect[i] = FALSE;
	}

	m_nKeyboardIdx = 0;
}

void CGapInspectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TIME, m_ctrMainTime);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SYSTEM_VER, m_ctrMainSystemVer);	

	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_RSLT, m_btn_Status[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TACK_TIME, m_btn_TackTime[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TOTAL_CNT, m_ctrMainTotalCnt[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_OK_CNT, m_ctrMainOkCnt[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_NG_CNT, m_ctrMainNgCnt[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_OK_RATIO, m_ctrMainOkRatio[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_CUR_MODEL, m_ctrMainWorkModel[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_CAM_0_CONNECT, m_ctrMainCamConnect[0]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_CAM_1_CONNECT, m_ctrMainCamConnect[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_BARCODE, m_ctrMainBarcode[0]);

	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_RSLT2, m_btn_Status[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TACK_TIME2, m_btn_TackTime[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TOTAL_CNT2, m_ctrMainTotalCnt[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_OK_CNT2, m_ctrMainOkCnt[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_NG_CNT2, m_ctrMainNgCnt[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_OK_RATIO2, m_ctrMainOkRatio[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_CUR_MODEL2, m_ctrMainWorkModel[1]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_CAM_0_CONNECT2, m_ctrMainCamConnect[2]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_CAM_1_CONNECT2, m_ctrMainCamConnect[3]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_BARCODE2, m_ctrMainBarcode[1]);

	DDX_Control(pDX, IDC_BTNENHCTRL_CHANNEL_0_RSLT, m_ctrMainRslt[0]);
	DDX_Control(pDX, IDC_BTNENHCTRL_CHANNEL_1_RSLT, m_ctrMainRslt[1]);

	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_BARCODE_RESET1, m_ctrBarcodeReset[0]);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_BARCODE_RESET2, m_ctrBarcodeReset[1]);

	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_PLC_STATE, m_ctrMainPLCState);	

	int i = 0;
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_4, m_MainDisplay[0][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_0, m_MainDisplay[0][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_2, m_MainDisplay[0][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_5, m_MainDisplay[0][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_1, m_MainDisplay[0][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_3, m_MainDisplay[0][i++]);	

	i = 0;
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_10, m_MainDisplay[1][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_6, m_MainDisplay[1][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_8, m_MainDisplay[1][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_11, m_MainDisplay[1][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_7, m_MainDisplay[1][i++]);	
	DDX_Control(pDX, IDC_STATIC_MAIN_IMAGE_9, m_MainDisplay[1][i++]);	

	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_4, m_ctrInspPointLabel[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_0, m_ctrInspPointLabel[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_1, m_ctrInspPointLabel[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_2, m_ctrInspPointLabel[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_3, m_ctrInspPointLabel[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_5, m_ctrInspPointLabel[0][i++]);	

	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_10, m_ctrInspPointLabel[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_6, m_ctrInspPointLabel[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_7, m_ctrInspPointLabel[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_8, m_ctrInspPointLabel[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_9, m_ctrInspPointLabel[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_INSP_POINT_11, m_ctrInspPointLabel[1][i++]);	

	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_0_0, m_ctrMainSpec[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_0_1, m_ctrMainSpec[0][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_0_2, m_ctrMainSpec[0][i++]);	

	i = 0;
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_1_0, m_ctrMainSpec[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_1_1, m_ctrMainSpec[1][i++]);	
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_SPEC_CHANNEL_1_2, m_ctrMainSpec[1][i++]);		
}

BEGIN_MESSAGE_MAP(CGapInspectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(WM_RECEIVE_DATA, OnReceive)
	ON_MESSAGE(WM_ONCONNECT, OnConnect)
	ON_MESSAGE(WM_ONCLOSE, OnClose)
	ON_MESSAGE(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, OnZoomViewToMainMessage)
	ON_MESSAGE(USE_MSG_TEACH_TO_MAIN_DIALOG, OnTeachToMainMessage)
	ON_MESSAGE(USE_MSG_SYSTEM_TO_MAIN_DIALOG, OnSystemToMainMessage)
	ON_MESSAGE(USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_0, OnMainGrabDisplayCh0Message)
	ON_MESSAGE(USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_1, OnMainGrabDisplayCh1Message)
	ON_MESSAGE(USE_MSG_MAIN_INSP_END_DISPLAY_DIALOG, OnMainInspEndDisplayMessage)
	ON_WM_INPUT()
END_MESSAGE_MAP()

// CGapInspectionDlg 메시지 처리기
BOOL CGapInspectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	if(!InitDialog())
	{
		CString strText;
		strText.Format(_T("InitDialog Error"));
		AfxMessageBox(strText);
		if(m_cLogFile != NULL)
		{
			m_cLogFile->WriteText(strText);
		}

		ReleaseDialog();
	}

	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		CountView(i);
	}

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_AUTO);
	pBtnEnh->SetValue(FALSE);

	pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_AUTO2);
	pBtnEnh->SetValue(FALSE);

	//20190218 ngh
	GetDlgItem(IDC_BTNENHCTRL_MAIN_SYSTEM_VER)->SetWindowTextW(SYSTEM_VER);
	this->SetWindowTextW(_T("UB Tilt Inspection"));

	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("AfxSocketInit Error"));
	}
	else
	{
#ifdef PLC_USE
		m_Client = new CClientSock;
		m_Client->Create();
		m_Client->SetWnd(this->m_hWnd); 
#endif
	}

	SetTimer(MAIN_TIMER_WINDOWS_VISIBLE, 100, NULL);

	RAWINPUTDEVICE ItDevice[2];
	ItDevice[0].usUsagePage = 0x01;
	ItDevice[0].usUsage = 0x06;
	ItDevice[0].dwFlags = RIDEV_INPUTSINK;
	ItDevice[0].hwndTarget = this->GetSafeHwnd();

	if(FALSE == RegisterRawInputDevices(ItDevice, 1, sizeof(RAWINPUTDEVICE)))
	{
		//AfxMessageBox(_T("Error"));
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGapInspectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGapInspectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGapInspectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CGapInspectionDlg::InitDialog()
{
	CString strText;
	strText.Format(_T("%sMain\\"), LOG_FILE_PATH);
#ifdef LIGHT_TEST
	m_cTestLogFile = NULL;
	m_cTestLogFile = new CLogFile(strText, _T("Error.Log"));
#endif
	
	m_cLogFile = NULL;
	m_cLogFile = new CLogFile(strText, _T("Main.log"));

	if(m_cLogFile == NULL)
		return FALSE;

	strText.Format(_T("Program Start"));
	m_cLogFile->WriteText(strText);

	strText.Format(_T("InitDialog Start"));
	m_cLogFile->WriteText(strText);

	m_DispTempImage = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 1);
	memset(m_DispTempImage->imageData, NULL, sizeof(char) * CAM_WIDTH * CAM_HEIGHT);

	m_dlgZoomView = NULL;
	m_dlgZoomView = new CDialogZoomView();
	m_dlgZoomView->Create(IDD_DIALOG_ZOOM_VIEW, this);
	if(m_dlgZoomView == NULL)
	{
		strText.Format(_T("m_dlgZoomView Init Error"));
		m_cLogFile->WriteText(strText);
		return FALSE;
	}

	m_dlgSystem = new CDialogSystem();
#ifdef TWO_CHANNEL_MODE
	m_dlgSystem->Create(IDD_DIALOG_SYSTEM_2_CHANNEL, this);
#else
	m_dlgSystem->Create(IDD_DIALOG_SYSTEM, this);
#endif
	if(m_dlgSystem == NULL)
	{
		strText.Format(_T("m_dlgSystem Init Error"));
		m_cLogFile->WriteText(strText);
		return FALSE;
	}

	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		m_bAutoState[i] = FALSE;
		m_nChannelIdx = i;
		m_bStopInitSeq[i] = FALSE;
		//SetInitSeqStepPara(i);
		m_dlgInspPoint[i] = new CDialogInspPoint();
		m_dlgInspPoint[i]->Create(IDD_DIALOG_INSP_POINT_TOUCH_BLOOM, this);
		if(m_dlgInspPoint[i] == NULL)
		{
			strText.Format(_T("m_dlgInspPoint[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}

		m_dlgInspRslt[i] = new CDialogInspRslt();
		m_dlgInspRslt[i]->Create(IDD_DIALOG_INSP_RSLT, this);
		if(m_dlgInspRslt[i] == NULL)
		{
			strText.Format(_T("m_dlgInspRslt[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}

		m_dlgInspRslt[i]->SetChannedIndex(i);

		m_dlgAvgRsltView[i] = NULL;
		m_dlgAvgRsltView[i] = new CDialogAvgRsltView();
		m_dlgAvgRsltView[i]->Create(IDD_DIALOG_AVG_RSLT_VIEW_BLOOM, this);
		if(m_dlgAvgRsltView[i] == NULL)
		{
			strText.Format(_T("m_dlgAvgRsltView[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}

		m_dlgTeach[i] = NULL;
		m_dlgTeach[i] = new CDialogTeach();
		m_dlgTeach[i]->Create(IDD_DIALOG_TEACH_2_CHANNEL, this);
		if(m_dlgTeach[i] == NULL)
		{
			strText.Format(_T("m_dlgTeach[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}
		m_dlgTeach[i]->SetChannelIdx(i);

		m_Vision[i] = NULL;
		m_Vision[i] = new CVision(i);
		if(m_Vision[i] == NULL)
		{
			strText.Format(_T("m_Vision[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}

		m_pDlgAlarm[i] = NULL;
		m_pDlgAlarm[i] = new CDialogAlarmMsg();
		m_pDlgAlarm[i]->Create(IDD_DIALOG_MSG_ALARM, this);
		if(m_pDlgAlarm == NULL)
		{
			strText.Format(_T("m_pDlgAlarm[%d] Init Error"), i);
			m_cLogFile->WriteText(strText);
			return FALSE;
		}

		g_SeqModule[i].Open(this->m_hWnd, i);
		if(i == 0)
			m_pThread[i] = AfxBeginThread(MotionThread0, this, THREAD_PRIORITY_NORMAL, 0);
		else
			m_pThread[i] = AfxBeginThread(MotionThread1, this, THREAD_PRIORITY_NORMAL, 0);
		
		if (m_pThread[i])
			m_pThread[i]->m_bAutoDelete = TRUE;

		for(int j = 0; j < MAX_INSP_POINT_CNT; j++)
		{
			SetMainDisplay(i, j);
			m_SaveImage[i][j] = NULL;
			m_SaveImage[i][j] = cvCreateImage(cvSize(CAM_WIDTH, CAM_HEIGHT), IPL_DEPTH_8U, 3);
		}

		m_bDGSFileCheckThreadStart[i] = TRUE;
		m_bDGSFileCheckThreadEnd[i] = FALSE;
		if(i == 0)
			m_pThreadDGSFileCheck[i] = AfxBeginThread(WriteDGSFileThread0, this, THREAD_PRIORITY_NORMAL, 0);
		else
			m_pThreadDGSFileCheck[i] = AfxBeginThread(WriteDGSFileThread1, this, THREAD_PRIORITY_NORMAL, 0);

		m_pThreadDGSFileCheck[i]->m_bAutoDelete = FALSE;

		//m_strBarcodeInfo[i].Format(_T("%s"), LoadBarcodeID(BARCODE_INFO_FILE, i));

		m_dlgBarcode[i] = new CDialogBarcodePopup();
		m_dlgBarcode[i]->Create(IDD_DIALOG_BARCODE_POPUP, this);
	}

	InitGigaECam();	

#ifdef TWO_CHANNEL_MODE
	m_pSerialComm = new CSerialPort;

	if(!m_pSerialComm->Open(LIGHT_COM_PORT))
	{
		strText.Format(_T("LIght Com Port %d Open Fail"), LIGHT_COM_PORT);
		m_cLogFile->WriteText(strText);
	}
#endif

	CString strPort;
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		strPort.Format(_T("\\\\.\\COM%d"), BARCODE_COM_PORT + i + 7);
		m_pBarcode[i] = new CBarcodeRsPortType(strPort);

		strText.Format(_T("COM%d Port Connect"), BARCODE_COM_PORT + i + 7);
		if(!m_pBarcode[i]->GetBarcodeConnect())
			strText.Format(_T("COM%d Port Open Fail"), BARCODE_COM_PORT + i + 7);

		m_cLogFile->WriteText(strText);
	}
#ifdef SERIAL_BARCODE
	CString strPort;
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		strPort.Format(_T("\\\\.\\COM%d"), BARCODE_COM_PORT + i);
		m_pBarcodeComm[i] = new CRsPort(strPort);
	}
#endif
	


	strText.Format(_T("InitDialog End"));
	m_cLogFile->WriteText(strText);

	return TRUE;
}

void CGapInspectionDlg::AllKillTimer()
{
	KillTimer(MAIN_TIMER_WINDOWS_VISIBLE							);
	KillTimer(MAIN_TIMER_EVENT_MANUAL_INSPECTION_END_CHECK		);
	KillTimer(MAIN_TIMER_CAMERA_CONNECT_CHECK						);
	KillTimer(MAIN_TIMER_CURRENT_TIME_SHOW						);
	KillTimer(MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_0);	
	KillTimer(MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_1	);
	KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0				);
	KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1				);
	KillTimer(MAIN_TIMER_JOG_CUR_POS								);
	KillTimer(MAIN_TIMER_BARCODE_INPUT							);
	KillTimer(MAIN_TIMER_MENU_SELECT								);
	KillTimer(MAIN_TIMER_MENU_AUTO								);
	KillTimer(MAIN_TIMER_MAIN_ALL_ORIGIN_CHECK			);		
	KillTimer(MAIN_TIMER_BARCODE_WAIT_INPUT						);
	KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_0	);
	KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_1	);
	KillTimer(MAIN_TIMER_SPREAD_ALARM_CHECK						);
	KillTimer(MAIN_TIMER_SERVER_AUTO_CONNECT						);
	KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0		);
	KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_1		);
	KillTimer(MAIN_TIMER_AUTO_CONNECT								);
	KillTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_0				);
	KillTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_1				);
	KillTimer(MAIN_TIMER_PLC_READ_CHECK_CHANNEL_0					);
	KillTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0			);
	KillTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_1			);
	KillTimer(MAIN_TIMER_DISPLAY_DIALOG							);
}

BOOL CGapInspectionDlg::ReleaseDialog()
{
	CString strText;
	strText.Format(_T("ReleaseDialog Start"));
#ifdef SERIAL_BARCODE
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		m_bThreadBarcodeStart[i] = FALSE;
		m_bThreadBarcodeEnd[i] = TRUE;

		while(TRUE)
		{
			if(!m_bThreadBarcodeStart[i] && !m_bThreadBarcodeEnd[i])
				break;

			//Sleep(10);
			::Delay(10);
		}
	}
#endif
	if(m_cLogFile != NULL)
		m_cLogFile->WriteText(strText);

	if (m_DispTempImage)
		cvReleaseImage(&m_DispTempImage);

	if(m_dlgZoomView != NULL)
	{
		delete m_dlgZoomView;
		m_dlgZoomView = NULL;
	}

	if(m_cAutoDelete != NULL)
	{
		delete m_cAutoDelete;
		m_cAutoDelete = NULL;
	}

	if(m_dlgSystem != NULL)
	{
		delete m_dlgSystem;
		m_dlgSystem = NULL;
	}


	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		m_bInspEndCheckThreadEnd[i] = TRUE;
		WaitInspEndCheckThread(i);

		if(m_dlgAvgRsltView[i] != NULL)
		{
			delete m_dlgAvgRsltView[i];
			m_dlgAvgRsltView[i] = NULL;
		}

		if(m_Vision[i] != NULL)
		{
			delete m_Vision[i];
			m_Vision[i] = NULL;
		}

		if(m_pDlgAlarm[i] != NULL)
		{
			delete m_pDlgAlarm[i];
			m_pDlgAlarm[i] = NULL;
		}
		if(m_dlgTeach[i] != NULL)
		{
			delete m_dlgTeach[i];
			m_dlgTeach[i] = NULL;
		}

		if(m_dlgInspPoint[i] != NULL)
		{
			delete m_dlgInspPoint[i];
			m_dlgInspPoint[i] = NULL;
		}

		if(m_dlgInspRslt[i] != NULL)
		{
			delete m_dlgInspRslt[i];
			m_dlgInspRslt[i] = NULL;
		}

		WaitWriteDGSFileThreadEnd(i);
		WaitImageSaveThreadEnd(i);

		for(int j = 0; j < MAX_INSP_POINT_CNT; j++)
		{
			if (m_SaveImage[i][j])
				cvReleaseImage(&m_SaveImage[i][j]);
		}

		if(m_dlgBarcode[i] != NULL)
		{
			delete m_dlgBarcode[i];
			m_dlgBarcode[i] = NULL;
		}
	}

	g_SeqModule[0].Close();
	g_SeqModule[1].Close();

#ifdef TWO_CHANNEL_MODE
#ifndef LIGHT_TEST
	for(int i = 0; i < (MAX_LIGHT_CNT * MAX_CAM_CNT); i++)
	{
		SetLightOnOff(i, FALSE);
	}
#else
	for(int i = 0; i < 2; i++)
	{
		SetLightOnOff(i, FALSE);
	}
#endif

	if (NULL != m_pSerialComm)
	{
		m_pSerialComm->Close();

		delete m_pSerialComm;
		m_pSerialComm = NULL;
	}
#endif
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		delete m_pBarcode[i];
		m_pBarcode[i] = NULL;
	}
#ifdef SERIAL_BARCODE
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		m_pBarcodeComm[i]->~CRsPort();

		delete m_pBarcodeComm[i];
		m_pBarcodeComm[i] = NULL;
	}
#endif

	for(int i = 0; i < MAX_CAM_CNT; i++)
	{
		CloseCamera(i);
	}

	strText.Format(_T("ReleaseDialog End"));
	if(m_cLogFile != NULL)
		m_cLogFile->WriteText(strText);

	strText.Format(_T("Program End"));
	if(m_cLogFile != NULL)
		m_cLogFile->WriteText(strText);

	if(NULL != m_cLogFile)
		delete m_cLogFile;

#ifdef LIGHT_TEST
	if(m_cTestLogFile!= NULL)
		delete m_cTestLogFile;
#endif

	 _CrtDumpMemoryLeaks();

	return TRUE;
}

void CGapInspectionDlg::SetAutoState(int nChannel, BOOL isAuto)
{
	if(isAuto)
	{
		m_btn_Status[nChannel].SetWindowTextW(_T("AUTO"));
		m_btn_Status[nChannel].SetBackColor(RGB(0, 255, 0));
	}
	else
	{
		m_btn_Status[nChannel].SetWindowTextW(_T("WAIT"));
		m_btn_Status[nChannel].SetBackColor(RGB(200, 200, 0));
	}

	m_bAutoInsp[nChannel] = isAuto;
}

int CGapInspectionDlg::CountLoad(int nChannel)		// Product Count
{
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format("%Y") + "\\" + NowTime.Format("%m");// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format("%d");// 일까지,,

	LoadDayCountData(nChannel, strCurtimeday);
	return m_iCount[nChannel];
}

void CGapInspectionDlg::CountOKAdd(int nChannel)		// Product Count
{
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format("%Y") + "\\" + NowTime.Format("%m");// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format("%d");// 일까지,,

	LoadDayCountData(nChannel, strCurtimeday);
	m_iCount[nChannel]++;
	m_iOKCount[nChannel]++;
	SaveDayCountData(nChannel, strCurtimeday);
	CountView(nChannel);
}

void CGapInspectionDlg::CountNGAdd(int nChannel, BOOL *bPointRslt)		// Product Count
{
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format("%Y") + "\\" + NowTime.Format("%m");// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format("%d");// 일까지,,

	LoadDayCountData(nChannel, strCurtimeday);
	m_iCount[nChannel]++;
	m_iNGCount[nChannel]++;

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		if(!bPointRslt[i])
			m_iPointNGConunt[nChannel][i]++;
	}
	SaveDayCountData(nChannel, strCurtimeday);
	CountView(nChannel);
}

void CGapInspectionDlg::CountReset(int nChannel)		// Product Count
{
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format("%Y") + "\\" + NowTime.Format("%m");// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format("%d");// 일까지,,

	LoadDayCountData(nChannel, strCurtimeday);
	m_iCount[nChannel] = 0;
	m_iOKCount[nChannel] = 0;
	m_iNGCount[nChannel] = 0;
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		m_iPointNGConunt[nChannel][i] = 0;

	SaveDayCountData(nChannel, strCurtimeday);
	CountView(nChannel);
}

void CGapInspectionDlg::CountView(int nChannel)
{
	CString strText;
	CountLoad(nChannel);
	strText.Format(_T("%d"), m_iCount[nChannel]);
	m_ctrMainTotalCnt[nChannel].SetWindowTextW(strText);

	strText.Format(_T("%d"), m_iOKCount[nChannel]);
	m_ctrMainOkCnt[nChannel].SetWindowTextW(strText);
	strText.Format(_T("%d"), m_iNGCount[nChannel]);
	m_ctrMainNgCnt[nChannel].SetWindowTextW(strText);

	if(m_iNGCount[nChannel] == 0 && m_iCount[nChannel] == 0)
		strText.Format(_T("0.00%%"));
	else
		strText.Format(_T("%.2f%%"), (double)m_iNGCount[nChannel] / (double)m_iCount[nChannel] * 100.);		// OK Ratio
	m_ctrMainOkRatio[nChannel].SetWindowTextW(strText);
}

void CGapInspectionDlg::LoadDayCountData(int nChannel, CString strDayCount)
{
	CProfileDataIF cDataIf;

	CString strTmp(_T("")), strFileName(_T("")), strNode(_T(""));
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format(_T("%Y")) + _T("\\") + NowTime.Format(_T("%m"));// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format(_T("%d"));// 일까지,,

	strCurtimeday.Format(_T("%s_Ch%d"), strDayCount, nChannel + 1);

	strFileName.Format(_T("%s%s%s.ini"),LOG_COUNT_PATH,strCurtime,strCurtimeday);
	cDataIf.SetFilePath(strFileName);
	strNode.Format(_T("COUNT DATA")); 	
	strTmp.Format(_T("Total Count"));
	m_iCount[nChannel]  = cDataIf.GetInt(strNode, strTmp, 0);
	strTmp.Format(_T("OK Count"));
	m_iOKCount[nChannel]  = cDataIf.GetInt(strNode, strTmp, 0);
	strTmp.Format(_T("NG Count"));
	m_iNGCount[nChannel]  = cDataIf.GetInt(strNode, strTmp, 0);

	strTmp.Format(_T("Y NG Count"));
	m_iPointNGConunt[nChannel][0]  = cDataIf.GetInt(strNode, strTmp, 0);
	strTmp.Format(_T("X Right NG Count"));
	m_iPointNGConunt[nChannel][1]  = cDataIf.GetInt(strNode, strTmp, 0);
	strTmp.Format(_T("X Left NG Count"));
	m_iPointNGConunt[nChannel][2]  = cDataIf.GetInt(strNode, strTmp, 0);

}

void CGapInspectionDlg::SaveDayCountData(int nChannel, CString strDayCount)
{
	CProfileDataIF cDataIf;
	CStdioFile pFile;
	CFileFind finder; 

	CString strTmp(_T("")), strFileName(_T("")), strNode(_T("")); 
	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday,strCount;
	strCurtime  = NowTime.Format(_T("%Y")) + _T("\\") + NowTime.Format(_T("%m"));// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday	= NowTime.Format(_T("%d"));// 일까지,,

	strCurtimeday.Format(_T("%s_Ch%d"), strDayCount, nChannel + 1);

	strFileName.Format(_T("%s%s%s.ini"),LOG_COUNT_PATH,strCurtime,strCurtimeday);
	cDataIf.SetFilePath(strFileName);
	CreateDir(strFileName);//경로에따라 찾고 없으면 만든다...

	strNode.Format(_T("COUNT DATA")); 	
	strTmp.Format(_T("Total Count"));
	cDataIf.SetInt(strNode, strTmp, m_iCount[nChannel]);
	strTmp.Format(_T("OK Count"));
	cDataIf.SetInt(strNode, strTmp, m_iOKCount[nChannel]);
	strTmp.Format(_T("NG Count"));
	cDataIf.SetInt(strNode, strTmp, m_iNGCount[nChannel]);

	strTmp.Format(_T("Y NG Count"));
	cDataIf.SetInt(strNode, strTmp, m_iPointNGConunt[nChannel][0]);
	strTmp.Format(_T("X Right NG Count"));
	cDataIf.SetInt(strNode, strTmp, m_iPointNGConunt[nChannel][1]);
	strTmp.Format(_T("X Left NG Count"));
	cDataIf.SetInt(strNode, strTmp, m_iPointNGConunt[nChannel][2]);
}

BOOL CGapInspectionDlg::GrabSingle(int nChannel, int nCamIdx, int nGrabIdx)
{
	long width, height;

	width = CAM_WIDTH;
	height = CAM_HEIGHT;

	BOOL bGrabEnd = FALSE;

	if(nCamIdx == 1)
	{
		//Sleep(200);
	}

#ifdef CAMERA
//#ifndef DRY_RUN
	if(m_CameraManager.m_bCamConnectFlag[nCamIdx + (nChannel * MAX_CHANNEL_CNT)] == false)
	{
#ifdef LIGHT_TEST
		if(!SetSelectCamConnect(nCamIdx + (nChannel * MAX_CHANNEL_CNT)))
			m_cTestLogFile->WriteText(_T("Caemra ReConnect Error"));
#else
		SetSelectCamConnect(nCamIdx + (nChannel * MAX_CHANNEL_CNT));
#endif
	}

	if(m_CameraManager.m_bCamConnectFlag[nCamIdx + (nChannel * MAX_CHANNEL_CNT)] == true)
	{
		m_CameraManager.ReadEnd(nCamIdx + (nChannel * MAX_CHANNEL_CNT));  // exposure end flag 변경

		if(m_CameraManager.SingleGrab(nCamIdx + (nChannel * MAX_CHANNEL_CNT)) == 0)
		{
			while(1)
			{
				if(m_CameraManager.CheckCaptureEnd(nCamIdx + (nChannel * MAX_CHANNEL_CNT)))
				{
					memcpy(m_Vision[nChannel]->GetGrabImageData(nCamIdx, nGrabIdx), m_CameraManager.pImage8Buffer[nCamIdx + (nChannel * MAX_CHANNEL_CNT)], width * height * sizeof(BYTE));
					if(nCamIdx == TOP_CAMERA)
					{
						FlipImage(m_Vision[nChannel]->GetGrabImage(nCamIdx, nGrabIdx), m_Vision[nChannel]->GetGrabImage(nCamIdx, nGrabIdx));
					}
					bGrabEnd = TRUE;
					break;
				}
				Delay(1, TRUE);
			}
		}
		else
		{
#ifdef LIGHT_TEST
			m_cTestLogFile->WriteText(_T("Single Grab Error"));
#endif
			return FALSE;
		}
	}
	else
	{
#ifdef LIGHT_TEST
		m_cTestLogFile->WriteText(_T("Caemra Connect Error"));
#endif
		return FALSE;
	}

//#endif
#endif
	return TRUE;
}


void CGapInspectionDlg::InitGigaECam(void)
{
	GetSerialNumerFromFile();
	memset(m_bLiveState, FALSE, sizeof(BOOL) * MAX_CAM_NUM);

	int i = 0, j = 0, nCount = 0;
	CString strLog(_T("")), strcamname(_T("")), sMsg(_T("")), strSerialNum(_T(""));
	strLog.Format(_T("InitGigaECam Start"));
	m_cLogFile->WriteText(strLog);
	
#ifdef CAMERA
	m_error = m_CameraManager.FindCamera(m_szCamName, m_szSerialNum, m_szInterface, &m_iCamNumber);

	if(m_error == 0)
	{
		for(i = 0; i < m_iCamNumber; i++)
		{		
			strcamname = (CString)m_szCamName[i];	
			strSerialNum = (CString)m_szSerialNum[i];
			j = 0;
			nCount++;

			while (TRUE)
			{
				if(j > (MAX_CAM_NUM - 1))
					break;

				if(strSerialNum == m_strCamSerial[j])   // 미리 define 한 카메라와 serial number 비교, 프로그램에서는 수행되는 인덱스 이다.
				{
					m_iCamPosition[j]= nCount;     // 카메라가 시스템에서 순서 연결을 의미

					sMsg.Format(_T("%d 번 strSerialNum[%s] -> m_strCamSerial[%d]:%s  m_iCamPosition[%d]:%d -> nCount:%d"),
								i,strSerialNum,j,m_strCamSerial[j],j,m_iCamPosition[j],nCount);
					
					strLog.Format(_T("InitGigaECam, %s"), sMsg);
					m_cLogFile->WriteText(strLog);

					if(OpenCamera(j))
					{
						ConnectCamera(j);
					}
					else
						CloseCamera(j);

					bStopThread[j];
					Delay(100,TRUE);
					break;
				}
				
				j++;
				DoEvents();
				Delay(10,TRUE);
			}

			Delay(100,TRUE);
		}
	}
	else if(m_error == -1)
	{
		strLog.Format(_T("InitGigaECam, 연결된 카메라가 없습니다"));
		m_cLogFile->WriteText(strLog);
	}
	else if(m_error == -2)
	{
		strLog.Format(_T("InitGigaECam, Pylon Function Error"));
		m_cLogFile->WriteText(strLog);
	}
#else
	for(int i = 0; i < MAX_CAM_CNT; i++)
	{
		ConnectCamera(i);
	}
#endif

	strLog.Format(_T("InitGigaECam End"));
	m_cLogFile->WriteText(strLog);
}

void CGapInspectionDlg::GetSerialNumerFromFile(void)
{
	int i = 0;
	CString strNode(_T("CAM1"));
	CProfileDataIF cDataIf;
	CString strLog;
	strLog.Format(_T("GetSerialNumerFromFile Start"));
	m_cLogFile->WriteText(strLog);

	cDataIf.SetFilePath(DATA_CAMERA_INFO_FILE);

	for (i = 0; i < MAX_CAM_NUM; i++)
	{
		strNode.Format(_T("CAM%d"), i+1);
		m_strCamSerial[i] = cDataIf.GetString(strNode, _T("Serial"), _T(""));
	}

	strLog.Format(_T("GetSerialNumerFromFile End"));
	m_cLogFile->WriteText(strLog);
}

BOOL CGapInspectionDlg::OpenCamera(int iIndex)
{
	CString sTempMsg(_T(""));
	BOOL bRet(TRUE);
	CString strLog;
	strLog.Format(_T("OpenCamera Start"));
	m_cLogFile->WriteText(strLog);
#ifdef CAMERA
	if(m_CameraManager.Open_Camera(iIndex, m_iCamPosition[iIndex]) == 0)
		sTempMsg.Format(_T("%d Camera Open_Success"),iIndex);
	else if(m_CameraManager.Open_Camera(iIndex, m_iCamPosition[iIndex]) == -1)
		sTempMsg.Format(_T("%d Camera Alread_Open"),iIndex);
	else
	{
		sTempMsg.Format(_T("%d Camera Open_Fail"),iIndex);
		bRet = FALSE;
	}
	m_cLogFile->WriteText(sTempMsg);

	strLog.Format(_T("OpenCamera, %s"), sTempMsg);
	m_cLogFile->WriteText(strLog);
#endif
	strLog.Format(_T("OpenCamera End"));
	m_cLogFile->WriteText(strLog);

	return bRet;
}

BOOL CGapInspectionDlg::CloseCamera(int iIndex)
{
	BOOL rslt = TRUE;
	CString strLog;
	strLog.Format(_T("CamIndex %d, CloseCamera Start"), iIndex);
	m_cLogFile->WriteText(strLog);
#ifdef CAMERA
	if(m_CameraManager.m_bCamOpenFlag[iIndex] == true)
	{
		if(m_CameraManager.Close_Camera(iIndex)==0)
		{
			strLog.Format(_T("CamIndex %d, CloseCamera Success"), iIndex);
			m_cLogFile->WriteText(strLog);
		}
		else
		{
			strLog.Format(_T("CamIndex %d, CloseCamera Fail"), iIndex);
			m_cLogFile->WriteText(strLog);
			rslt =  FALSE;
		}
	}
#endif
	strLog.Format(_T("CamIndex %d, CloseCamera End"), iIndex);
	m_cLogFile->WriteText(strLog);

	return rslt;
}

void CGapInspectionDlg::SelectCamConnect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int nDialogID;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	switch(nDialogID)
	{
	case IDC_BTNENHCTRL_CAM_0_CONNECT:
#ifdef CAMERA
		if(GetCamConnect(TOP_CAMERA))
		{
			SetSelectCamClose(TOP_CAMERA);
		}
		else
		{
			SetSelectCamConnect(TOP_CAMERA);
		}
#endif
		break;
	case IDC_BTNENHCTRL_CAM_1_CONNECT:
#ifdef CAMERA
		if(GetCamConnect(BOTTOM_CAMERA))
		{
			SetSelectCamClose(BOTTOM_CAMERA);
		}
		else
		{
			SetSelectCamConnect(BOTTOM_CAMERA);
		}
#endif
		break;
	case IDC_BTNENHCTRL_CAM_0_CONNECT2:
#ifdef CAMERA
		if(GetCamConnect(NO_2_CHANNEL + TOP_CAMERA + 1))
		{
			SetSelectCamClose(NO_2_CHANNEL + TOP_CAMERA + 1);
		}
		else
		{
			SetSelectCamConnect(NO_2_CHANNEL + TOP_CAMERA + 1);
		}
#endif
		break;
	case IDC_BTNENHCTRL_CAM_1_CONNECT2:
#ifdef CAMERA
		if(GetCamConnect(NO_2_CHANNEL + BOTTOM_CAMERA + 1))
		{
			SetSelectCamClose(NO_2_CHANNEL + BOTTOM_CAMERA + 1);
		}
		else
		{
			SetSelectCamConnect(NO_2_CHANNEL + BOTTOM_CAMERA + 1);
		}
#endif
		break;
	default:

		break;
	}
}

BOOL CGapInspectionDlg::GetCamConnect(int nCamIdx)
{
#ifdef CAMERA
	return m_CameraManager.m_bCamConnectFlag[nCamIdx];
#else
	return TRUE;
#endif
}
																																																																																																																																																																										
 BOOL CGapInspectionDlg::ConnectCamera(int iIndex)
 {
 	CString sTempMsg(_T(""));
 	BOOL bRet(TRUE);
	CString strLog;
	strLog.Format(_T("CamIndex %d, ConnectCamera Start"), iIndex);
	m_cLogFile->WriteText(strLog);
 #ifdef CAMERA	
 	int iCM_Width(CAM_WIDTH);
 	int iCM_Height(CAM_HEIGHT);

#ifdef TABLET
	m_CameraManager.SetInteger(iIndex,9000,"GevSCPSPacketSize");
#else
	m_CameraManager.SetInteger(iIndex,9000,"GevSCPSPacketSize");
#endif
	//20190422
	//m_CameraManager.SetInteger(iIndex,1500,"GevSCPSPacketSize");

	for(int i = 0; i < MAX_CAM_NUM; i++)
		m_CameraManager.m_bCamOpenFlag[i] = true;

 	if(m_CameraManager.m_bCamOpenFlag[iIndex] == true)
 	{ 
 		if(m_CameraManager.Connect_Camera(iIndex,0,0,iCM_Width,iCM_Height,_T("Mono8"))==0)    //BayerBG8   YUV422Packed  Mono8 , Mono16
 		{
 			sTempMsg.Format(_T("CamIndex %d, Camera Connect_Success"),iIndex);
 		}
 		else
 		{
 			sTempMsg.Format(_T("CamIndex %d, Camera Connect_Fail"),iIndex);
 			bRet = FALSE;
 		}
 	}
 	else
 	{
 		sTempMsg.Format(_T("CamIndex %d, Camera Connect_Fail"),iIndex);
 		bRet = FALSE;
 	}
	m_cLogFile->WriteText(sTempMsg);

	strLog.Format(_T("ConnectCamera, %s"), sTempMsg);
	m_cLogFile->WriteText(strLog);
#else
	m_bCamConnect[iIndex] = TRUE;
	m_CameraManager.m_bCamConnectFlag[iIndex] = true;
#endif
	strLog.Format(_T("CamIndex %d, ConnectCamera End"), iIndex);
	m_cLogFile->WriteText(strLog);
 
 	return bRet;
}

 UINT CGapInspectionDlg::LiveGrabThreadCam(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->CamLive();
	
	return 0;
}

void CGapInspectionDlg::CamLive()
{
	CString Info;
	int nCamIndex = 0, nInspIdx = 0, nChannelIdx = 0;
	CString strLog;
	BOOL bLive = FALSE;
#ifdef CAMERA
	if(m_nLiveCamIdx > 0)
	{
		for(int i = 0; i < MAX_CAM_NUM; i++)
		{
			for(int j = 0; j < MAX_GRAB_CNT; j++)
			{
				if(!m_bLiveState[i][j])
				{
					m_bLiveState[i][j] = TRUE;
					nCamIndex = i;
					bLive = TRUE;
					break;
				}
			}
			if(bLive)
				break;
		}
	}

	if(nCamIndex > 1)
	{
		nChannelIdx = 1;
	}
	else
	{
		nChannelIdx = 0;
	}

	if(m_CameraManager.m_bCamConnectFlag[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)] == false)
	{
		SetSelectCamConnect(nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT));
	}

	QueryPerformanceCounter(&(start[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]));
	nFrameCount[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)] = 0;

	while(bStopThread[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]==true)
	{	
		if(m_CameraManager.m_bRemoveCamera[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]==true)
		{		   		
			m_CameraManager.m_bRemoveCamera[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)] = false;
			Info.Format(_T("%d번 Camera LostConnection"), nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT));	
			strLog.Format(_T("CamLive, %s"), Info);
		}
		else
		{
			if(m_CameraManager.CheckCaptureEnd(nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT))) //exposure end true일때 
			{
				m_CameraManager.ReadEnd(nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT));  // exposure end flag 변경 
				nFrameCount[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]++;
				QueryPerformanceCounter(&(end[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]));

				if(m_Vision[nChannelIdx]->GetGrabImage(nCamIndex, nInspIdx))
				{
					memcpy(m_Vision[nChannelIdx]->GetGrabImageData(nCamIndex, nInspIdx), m_CameraManager.pImage8Buffer[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)], CAM_WIDTH * CAM_HEIGHT * sizeof(BYTE));
				}

				if(nCamIndex == TOP_CAMERA)
				{
					FlipImage(m_Vision[nChannelIdx]->GetGrabImage(nCamIndex, nInspIdx), m_Vision[nChannelIdx]->GetGrabImage(nCamIndex, nInspIdx));
				}

				m_dlgZoomView->SetImage(m_Vision[nChannelIdx]->GetGrabImage(nCamIndex, nInspIdx));
			}
			if(m_CameraManager.m_bCamConnectFlag[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)] == false)
			{
				m_nCamReconnectCnt[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)]++;
				SetSelectCamConnect(nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT));
			}

			if(m_nCamReconnectCnt[nCamIndex + (nChannelIdx * MAX_CHANNEL_CNT)] == MAX_GRAB_RECONNECT_CNT)
			{
				SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, nCamIndex);
				break;
			}
		}
		Delay(250,TRUE);
	}
#endif
}

BOOL CGapInspectionDlg::CamLiveSetting(int nChannelIdx, int nCamIdx, int nGrabIdx)
{
	CString sMsg(_T(""));

	int nCamIndex = nCamIdx + (nChannelIdx * MAX_CHANNEL_CNT);
	m_nLiveCamGrabIdx = nGrabIdx;
#ifdef CAMERA
	if(m_CameraManager.m_bCamConnectFlag[nCamIndex] == true)
	{
		bStopThread[nCamIndex]=(bStopThread[nCamIndex]+1)&0x01;   
		if(bStopThread[nCamIndex])
		{
			m_bLiveFlag[nCamIndex] = true;
			m_CameraManager.GrabLive(nCamIndex, 0);
			//////////////////////////////////////////////////////////////////////////
			AfxBeginThread(LiveGrabThreadCam, this);
			Delay(10,TRUE);
		}
		else
		{
			m_bLiveState[nCamIndex][nGrabIdx] = FALSE;
			m_bLiveFlag[nCamIndex] = false;
			m_CameraManager.LiveStop(nCamIndex, 0);
			return FALSE;
		}	
	}
	else
	{
		sMsg.Format(_T("Camera_%d Connect를 하세요!!"),nCamIndex+1);
		if(OpenCamera(nCamIndex))
		{
			ConnectCamera(nCamIndex);
		}
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CGapInspectionDlg::SavelpImage(int nChannel, int nCamIdx, int nInspIdx)
{
	BOOL rslt = TRUE;
	CFileDialog myFileDlg(FALSE, _T(" "), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Image Files (*.bmp)|*.bmp||"));
	CString strText;

	if(myFileDlg.DoModal() != IDOK)
	{
		return FALSE;
	}
	strText = myFileDlg.GetPathName();
	rslt = SaveIplImage(m_Vision[nChannel]->GetGrabImage(nCamIdx, nInspIdx), strText);

	return rslt;
}

BOOL CGapInspectionDlg::LoadlpImage(int nChannel, int nCamIdx, int nInspIdx)
{
	BOOL rslt = TRUE;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);
	CString strText;

	if(dlg.DoModal() != IDOK)
	{
		return FALSE;
	}
	strText = dlg.GetPathName();
	rslt = LoadIplImage(m_Vision[nChannel]->GetDispImage(nCamIdx), strText, TRUE);
	if(rslt)
	{
		cvCvtColor(m_Vision[nChannel]->GetDispImage(nCamIdx), m_Vision[nChannel]->GetGrabImage(nCamIdx, nInspIdx), CV_BGR2GRAY);
		m_dlgZoomView->SetImage(m_Vision[nChannel]->GetDispImage(nCamIdx));
	}

	return rslt;
}

BEGIN_EVENTSINK_MAP(CGapInspectionDlg, CDialogEx)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_AUTO, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainAuto, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_AUTO2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainAuto2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TEACH, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainTeach, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TEACH2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainTeach2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_SYSTEM, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainSystem, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_EXIT, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainExit, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_ZOOM_VIEW_0, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlZoomView0, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_ZOOM_VIEW_2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlZoomView2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_ZOOM_VIEW_1, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlZoomView1, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_ZOOM_VIEW_3, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlZoomView3, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_COUNT_RESET, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainCountReset, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_COUNT_RESET2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainCountReset2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_AVG_RSLT_VIEW, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainAvgRsltView, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_AVG_RSLT_VIEW2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainAvgRsltView2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_CAM_0_CONNECT, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlCam0Connect, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_CAM_1_CONNECT, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlCam1Connect, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_CAM_0_CONNECT2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlCam0Connect2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_CAM_1_CONNECT2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlCam1Connect2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_PLC_STATE, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainPlcState, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_BARCODE_RESET1, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainBarcodeReset1, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_BARCODE_RESET2, DISPID_CLICK, CGapInspectionDlg::ClickBtnenhctrlMainBarcodeReset2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TOTAL_CNT, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainTotalCnt, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TOTAL_CNT2, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainTotalCnt2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_OK_CNT, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainOkCnt, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_OK_CNT2, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainOkCnt2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_NG_CNT, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainNgCnt, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_NG_CNT2, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainNgCnt2, VTS_NONE)	
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TACK_TIME, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainTackTime, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_TACK_TIME2, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainTackTime2, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_OK_RATIO, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainOkRatio, VTS_NONE)
	ON_EVENT(CGapInspectionDlg, IDC_BTNENHCTRL_MAIN_OK_RATIO2, DISPID_DBLCLICK, CGapInspectionDlg::DblClickBtnenhctrlMainOkRatio2, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CGapInspectionDlg::ShowSelectMenu(int nChannel, int nDialogIdx)
{
	int nTimerVal = 0;
	if(nChannel == 1)
	{
		nTimerVal = 10;
	}

	switch(nDialogIdx)
	{
	case AUTO_CHANNEL_1:
	case AUTO_CHANNEL_2:
		SetInitBarcode(nChannel);
		if(m_vBarcode[nChannel].size() > 0)
		{
			m_vBarcode[nChannel].clear();
		}

		if(m_bAutoInsp[nChannel])
		{
			m_bAutoState[nChannel] = FALSE;
			SetAutoState(nChannel, FALSE);
			g_SeqModule[nChannel].Stop();
			//WritePlcVal(nChannel, 2000);
			//KillTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0 + nTimerVal);
			KillTimer(10000);
#ifdef SERIAL_BARCODE
			m_bThreadBarcodeStart[nChannel] = FALSE;
			m_bThreadBarcodeEnd[nChannel] = FALSE;
#endif
			m_pBarcode[nChannel]->SetBarcodeReadStart(FALSE);
			//SetBarcode(nChannel, m_pBarcode[nChannel]->GetBarcodeString());
			//SetBarcode(nChannel, _T("-"));
			//SetNextBarcode(nChannel, _T(""));
			//m_dlgBarcode[nChannel]->ShowWindow(SW_HIDE);

			if(nChannel == NO_1_CHANNEL)
			{
				KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0);
				KillTimer(MAIN_TIMER_BARCODE_READER_CANNEL_0);
				//GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE0)->ShowWindow(SW_HIDE);
			}
			else
			{
				KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1);
				KillTimer(MAIN_TIMER_BARCODE_READER_CANNEL_1);
				//GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE1)->ShowWindow(SW_HIDE);
			}
			
			InitInspection(nChannel);
			int nCamIdx[2];

			if(nChannel == 0)
			{
				nCamIdx[0] = 0;
				nCamIdx[1] = 1;
			}
			else
			{
				nCamIdx[0] = 2;
				nCamIdx[1] = 3;
			}
			m_bInspEndCheckThreadStart[nChannel] = FALSE;
			m_bInspEndCheckThreadEnd[nChannel] = TRUE;
			m_bGrabThreadStart[nCamIdx[0]] = FALSE;
			m_bGrabThreadEnd[nCamIdx[0]] = TRUE;
			m_bGrabThreadStart[nCamIdx[1]] = FALSE;
			m_bGrabThreadEnd[nCamIdx[1]] = TRUE;
			m_bThreadGrabEndCheckEnd[nChannel] = TRUE;
			SetReleaseGrabThread(nChannel);
		}
		else
		{
			m_bAutoState[nChannel] = TRUE;
			g_SeqModule[nChannel].SetSeqFirst(FALSE);
			m_dlgSystem->SetLoadSystemPara();
			m_SystemPara = m_dlgSystem->GetSystemPara();
			m_Vision[nChannel]->SetSystemPara(m_SystemPara);
			m_dlgTeach[nChannel]->SetModel(m_SystemPara.strUseModelName);
			m_dlgTeach[nChannel]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
			m_SeqPara[nChannel] = m_dlgTeach[nChannel]->GetSeqPara();
			m_Vision[nChannel]->SetSeqPara(m_SeqPara[nChannel]);

#ifdef KEY_BARCODE
			m_strKeyBarcode[nChannel].Format(_T(""));
#endif

			if(nChannel == NO_1_CHANNEL)
			{
				SetTimer(MAIN_TIMER_BARCODE_READER_CANNEL_0, BARCODE_READ_TIME, NULL);
				//GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE0)->ShowWindow(SW_SHOW);
			}
			else
			{
				SetTimer(MAIN_TIMER_BARCODE_READER_CANNEL_1, BARCODE_READ_TIME, NULL);
				//GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE1)->ShowWindow(SW_SHOW);
			}
			
			if(m_bInspectionEnd[nChannel].size() > 0)
				m_bInspectionEnd[nChannel].clear();

			for(int i = 0; i < m_dlgTeach[nChannel]->GetMaxSeqCnt(); i++)
			{
				m_bInspectionEnd[nChannel].push_back(FALSE);
			}
	
			SetSeqLightPara(nChannel);
			SetAutoState(nChannel, TRUE);
			SetInitSeqStepPara(nChannel);
			
			if(m_dlgTeach[nChannel]->IsWindowVisible())
			{
				m_dlgTeach[nChannel]->ShowWindow(SW_HIDE);
			}

			if(m_dlgSystem->IsWindowVisible())
			{
				m_dlgSystem->ShowWindow(SW_HIDE);
			}

			if(m_dlgZoomView->IsWindowVisible())
			{
				m_dlgZoomView->ShowWindow(SW_HIDE);
			}

			if(nChannel == NO_1_CHANNEL)
			{
				CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH);
				pBtnEnh->SetValue(FALSE);
			}
			else
			{
				CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH2);
				pBtnEnh->SetValue(FALSE);
			}

			m_nSeqStepIndex[nChannel] = -1;
			g_SeqModule[nChannel].Start();
			//ReadPlcVal(nChannel, -1);
#ifdef SERIAL_BARCODE
			m_bThreadBarcodeStart[nChannel] = TRUE;
			m_bThreadBarcodeEnd[nChannel] = FALSE;
#endif
			m_pBarcode[nChannel]->SetBarcodeReadStart(TRUE);
		}
		break;
	case TEACH_CHANNEL_1:
	case TEACH_CHANNEL_2:
		if(!m_bAutoInsp[nChannel])
		{
			if(!m_dlgTeach[nChannel]->IsWindowVisible())
			{
				m_nChannelIdx = nChannel;

				if(m_dlgSystem->IsWindowVisible())
					m_dlgSystem->ShowWindow(SW_HIDE);

				m_dlgTeach[nChannel]->SetModel(m_SystemPara.strUseModelName);
				m_dlgTeach[nChannel]->ShowWindow(SW_SHOW);
			}
			else
			{
				m_dlgTeach[nChannel]->ShowWindow(SW_HIDE);
				if(m_dlgZoomView->IsWindowVisible())
				{
					m_dlgZoomView->ShowWindow(SW_HIDE);
				}
				if(nChannel == NO_1_CHANNEL)
				{
					CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH);
					pBtnEnh->SetValue(FALSE);
				}
				else
				{
					CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH2);
					pBtnEnh->SetValue(FALSE);
				}
			}
		}
		else
		{
			if(nChannel == NO_1_CHANNEL)
			{
				CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH);
				pBtnEnh->SetValue(FALSE);
			}
			else
			{
				CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH2);
				pBtnEnh->SetValue(FALSE);
			}
			CString strText;
			strText.Format(_T("Ch No %d, Auto Insp State Not System Dialog View !!"), nChannel + 1);
			if(m_bAutoInsp[nChannel])
			{
				AfxMessageBox(strText);
				m_cLogFile->WriteText(strText);
			}
		}
		break;
	default:
		AfxMessageBox(_T("nDialog Idx Error !!"));
		return FALSE;
		break;
	}

	return TRUE;
}

void CGapInspectionDlg::ClickBtnenhctrlMainAuto()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ShowSelectMenu(NO_1_CHANNEL, AUTO_CHANNEL_1);
}


void CGapInspectionDlg::ClickBtnenhctrlMainAuto2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ShowSelectMenu(NO_2_CHANNEL, AUTO_CHANNEL_2);
}


void CGapInspectionDlg::ClickBtnenhctrlMainTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ShowSelectMenu(NO_1_CHANNEL, TEACH_CHANNEL_1);
}


void CGapInspectionDlg::ClickBtnenhctrlMainTeach2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ShowSelectMenu(NO_2_CHANNEL, TEACH_CHANNEL_2);
}


void CGapInspectionDlg::ClickBtnenhctrlMainSystem()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		CString strText;
		strText.Format(_T("Ch No %d, Auto Insp State Not System Dialog View !!"), i + 1);
		if(m_bAutoInsp[i])
		{
			AfxMessageBox(strText);
			return;
		}
	}

	if(m_dlgSystem->IsWindowVisible())
	{
		m_dlgSystem->ShowWindow(SW_HIDE);
	}
	else
	{
		m_dlgSystem->ShowWindow(SW_SHOW);
	}

	if(m_dlgTeach[0]->IsWindowVisible())
		m_dlgTeach[0]->ShowWindow(SW_HIDE);

	if(m_dlgTeach[1]->IsWindowVisible())
		m_dlgTeach[1]->ShowWindow(SW_HIDE);
}


void CGapInspectionDlg::ClickBtnenhctrlMainExit()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strText;
	for(int i = 0; i < MAX_CHANNEL_CNT; i++)
	{
		strText.Format(_T("Ch No %d, Auto Insp State Not Exit !!"), i + 1);
		if(m_bAutoInsp[i])
		{
			AfxMessageBox(strText);
			return;
		}

		strText.Format(_T("Ch No %d, Teach Dialog View Not Exit !!"), i + 1);
		if(m_dlgTeach[i]->IsWindowVisible())
		{
			AfxMessageBox(strText);
			return;
		}

		strText.Format(_T("Ch No %d, Alarm Dialog View Not Exit !!"), i + 1);
		if(m_pDlgAlarm[i]->IsWindowVisible())
		{
			AfxMessageBox(strText);
			return;
		}
	}
	
	AllKillTimer();

	ReleaseDialog();

	_CrtDumpMemoryLeaks(); 

	EndDialog(0);
}


void CGapInspectionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nEvent = (int)nIDEvent;
	CRect rect1, rect2, rect3, rect4;
	TypeSystemPara SystemPara;
	TypeInspEachRslt InspRslt;
	CBtnenhctrl *pBtnEnh;

	switch(nEvent)
	{
	case MAIN_TIMER_WINDOWS_VISIBLE:
		if(this->IsWindowVisible())
		{
			KillTimer(MAIN_TIMER_WINDOWS_VISIBLE);
			SetTimer(MAIN_TIMER_CURRENT_TIME_SHOW, 1000, NULL);

			GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);
			GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect2);

			m_dlgSystem->MoveWindow(rect2.left + (rect2.Width() / 2) - ((rect1.left - rect2.right) / 2), rect2.top, rect2.Width() + (rect1.left - rect2.right), rect2.Height());
			m_dlgSystem->ShowWindow(SW_SHOW);
			m_dlgSystem->SetTimer(SYSTEM_TIMER_WINDOWS_VISIBLE, 100, NULL);

			m_SystemPara = m_dlgSystem->GetSystemPara();

			//m_strUseModelName.Format(_T("%s"), m_SystemPara.strUseModelName);

			m_dlgTeach[0]->SetModel(m_SystemPara.strUseModelName);
			m_dlgTeach[1]->SetModel(m_SystemPara.strUseModelName);

			m_dlgTeach[0]->MoveWindow(rect1.left, rect1.top, rect1.Width(), rect1.Height());
			m_dlgTeach[0]->ShowWindow(SW_SHOW);

			m_dlgTeach[1]->MoveWindow(rect2.left, rect2.top, rect2.Width(), rect2.Height());
			m_dlgTeach[1]->ShowWindow(SW_SHOW);

			//m_dlgBarcode[0]->MoveWindow(rect2.left + (rect2.Width() / 2) - (rect1.Width() / 2), rect2.top + (rect2.Height() / 2) - (rect1.Height() / 2), rect1.Width(), rect1.Height());
			//m_dlgBarcode[1]->MoveWindow(rect1.left + (rect1.Width() / 2) - (rect2.Width() / 2), rect1.top + (rect1.Height() / 2) - (rect2.Height() / 2), rect2.Width(), rect2.Height());
			//rect4
			GetDlgItem(IDC_BTNENHCTRL_CAM_1_CONNECT)->GetWindowRect(&rect2);
			m_dlgBarcode[0]->GetWindowRect(&rect4);
			rect3.SetRect(rect2.right + 5, rect2.top, rect2.right + 5 + rect4.Width(), rect2.top + rect4.Height());
			m_dlgBarcode[0]->SetShowRect(&rect3);
			m_dlgBarcode[0]->MoveWindow(rect3.left, rect3.top, rect3.Width(), rect3.Height());
			//m_dlgBarcode[0]->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_BTNENHCTRL_CAM_1_CONNECT2)->GetWindowRect(&rect2);
			rect3.SetRect(rect2.right + 5, rect2.top, rect2.right + 5 + rect4.Width(), rect2.top + rect4.Height());
			m_dlgBarcode[1]->SetShowRect(&rect3);
			m_dlgBarcode[1]->MoveWindow(rect3.left, rect3.top, rect3.Width(), rect3.Height());
			//m_dlgBarcode[1]->ShowWindow(SW_SHOW);

			//m_dlgBarcode[0]->ShowWindow(SW_HIDE);
			//m_dlgBarcode[0]->ShowWindow(SW_HIDE);

			//m_ctrBarcodeReset[0].EnableWindow(FALSE);
			//m_ctrBarcodeReset[1].EnableWindow(FALSE);

			GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_5)->GetWindowRect(&rectTotalCnt[0][0]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT)->GetWindowRect(&rectTotalCnt[0][1]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_11)->GetWindowRect(&rectTotalCnt[1][0]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT2)->GetWindowRect(&rectTotalCnt[1][1]);			

			GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_8)->GetWindowRect(&rectNGRatioCnt[0][0]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME)->GetWindowRect(&rectNGRatioCnt[0][1]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_14)->GetWindowRect(&rectNGRatioCnt[1][0]);
			GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME2)->GetWindowRect(&rectNGRatioCnt[1][1]);	

			GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW)->GetWindowRect(&rect1);
			GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW2)->GetWindowRect(&rect2);
			m_dlgInspPoint[0]->GetWindowRect(&rect3);
			m_dlgInspPoint[0]->MoveWindow(rect1.left, rect1.top, rect3.Width(), rect3.Height());
			m_dlgInspPoint[1]->MoveWindow(rect2.left, rect2.top, rect3.Width(), rect3.Height());
			m_dlgInspPoint[0]->ShowWindow(SW_SHOW);
			m_dlgInspPoint[1]->ShowWindow(SW_SHOW);

			m_dlgInspPoint[0]->GetWindowRect(&rect2);
			m_dlgInspRslt[0]->GetWindowRect(&rect3);
			m_dlgInspRslt[0]->MoveWindow(rect1.left + rect2.Width(), rect1.top, rect3.Width(), rect3.Height());
			m_dlgInspRslt[0]->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW2)->GetWindowRect(&rect1);
			m_dlgInspPoint[1]->GetWindowRect(&rect2);
			m_dlgInspRslt[1]->GetWindowRect(&rect3);
			m_dlgInspRslt[1]->MoveWindow(rect1.left + rect2.Width(), rect1.top, rect3.Width(), rect3.Height());
			m_dlgInspRslt[1]->ShowWindow(SW_SHOW);

			m_dlgZoomView->SetInitDisplay();
			m_dlgZoomView->ShowWindow(SW_SHOW);

			this->GetWindowRect(rect1);
			m_dlgTeach[0]->SetMainWindowRect(rect1);
			m_dlgTeach[1]->SetMainWindowRect(rect1);

			m_strUseModelName.Format(_T("%s"), m_SystemPara.strUseModelName);
			m_SystemPara.DirAutoDelPara.bDriveFreeSizeAutoDel = 0;//FREE_SIZE_AUTO_DEL;
			m_SystemPara.DirAutoDelPara.dDelDrivePercent = AUTO_DEL_DRIVE_SIZE_PER;
			m_SystemPara.DirAutoDelPara.bManualDriveFreeSizeAutoDel = FREE_SIZE_AUTO_DEL;
			m_cAutoDelete = new CAutoDelete(m_SystemPara.DirAutoDelPara);

			DisplayModelName(m_strUseModelName);

			m_dlgInspRslt[0]->SetNGImageOpen(m_SystemPara.bRsltNGImageSave);
			m_dlgInspRslt[0]->SetOKImageOpen(m_SystemPara.bRsltImageSave);
			m_dlgInspRslt[1]->SetNGImageOpen(m_SystemPara.bRsltNGImageSave);
			m_dlgInspRslt[1]->SetOKImageOpen(m_SystemPara.bRsltImageSave);

			for(int i = 0; i < MAX_CAM_CNT;i++)
			{
				if(GetCamConnect(i))
				{
					m_ctrMainCamConnect[i].SetBackColor(RGB(0, 220, 0));
					if(i == 0 || i == 2)
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Right Cam Connect"));
					}
					else
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Left Cam Connect"));
					}
				}
				else
				{
					m_ctrMainCamConnect[i].SetBackColor(RGB(220, 0, 0));
					if(i == 0 || i == 2)
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Right Cam Disconnect"));
					}
					else
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Left Cam Disconnect"));
					}
				}
			}

#ifndef LIGHT_TEST
			for(int i = 0; i < (MAX_LIGHT_CNT * MAX_CAM_CNT); i++)
			{
				SetLightOnOff(i, TRUE);
			}
#else
			for(int i = 0; i < 2; i++)
			{
				//SetLightOnOff(i, TRUE);
			}
#endif

			SetTimer(MAIN_TIMER_SERVER_AUTO_CONNECT, AUTO_CONNECT_DELAY, NULL);
			SetTimer(MAIN_TIMER_CAMERA_CONNECT_CHECK, 100, NULL);
			SetTimer(MAIN_TIMER_DISPLAY_DIALOG, 1000, NULL);

			if(m_SystemPara.bVIPMode)
			{
				NGRatioView(FALSE);
			}
			else
			{
				NGRatioView(TRUE);
			}


			for(int i = 0; i < MAX_CHANNEL_CNT; i++)
			{
				TypeSpecPara sSpec[MAX_RSLT_POINT_CNT];
				m_dlgTeach[i]->GetSpec(sSpec);
				for(int j = 0; j < MAX_RSLT_POINT_CNT; j++)
				{
					CString strText;
					strText.Format(_T("%.3f (Min) ~ %.3f (Max)"), sSpec[j].dMinSpec, sSpec[j].dMaxSpec);
					m_ctrMainSpec[i][j].SetWindowTextW(strText);
				}
			}



#ifdef SERIAL_BARCODE
			for(int i = 0; i < MAX_CHANNEL_CNT; i++)
			{
				m_bBarcodeInputEnd[i] = FALSE;

				m_bThreadBarcodeStart[i] = TRUE;
				m_bThreadBarcodeEnd[i] = FALSE;

				if(i == 0)
					m_pThreadBarcodeRead[i] = AfxBeginThread(BarcodeReadThread0, this, THREAD_PRIORITY_NORMAL, 0);
				else
					m_pThreadBarcodeRead[i] = AfxBeginThread(BarcodeReadThread1, this, THREAD_PRIORITY_NORMAL, 0);

				if (m_pThreadBarcodeRead[i])
					m_pThreadBarcodeRead[i]->m_bAutoDelete = TRUE;
			}
#endif
		}
		break;
	case MAIN_TIMER_EVENT_MANUAL_INSPECTION_END_CHECK:
		if(m_Vision[m_nChannelIdx]->GetManualInspectionEnd(m_nManualInspCamIdx))
		{
			KillTimer(MAIN_TIMER_EVENT_MANUAL_INSPECTION_END_CHECK);
			InspRslt.RsltImage = NULL;
			m_Vision[m_nChannelIdx]->GetManualRsltGapVal(m_dlgZoomView->GetCamIdx(), &InspRslt);
			m_dlgZoomView->SetRsltVal(InspRslt.dGapRslt);
			m_dlgZoomView->SetImage(InspRslt.RsltImage);
			cvReleaseImage(&InspRslt.RsltImage);
		}
		break;
	case MAIN_TIMER_CAMERA_CONNECT_CHECK:
		for(int i = 0; i < MAX_CAM_CNT;i++)
		{
			if(m_bCamConnect[i] != GetCamConnect(i))
			{
				m_bCamConnect[i] = GetCamConnect(i);
				if(m_bCamConnect[i])
				{
					m_ctrMainCamConnect[i].SetBackColor(RGB(0, 220, 0));
					if(i == 0 || i == 2)
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Right Cam Connect"));
					}
					else
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Left Cam Connect"));
					}
				}
				else
				{
					m_ctrMainCamConnect[i].SetBackColor(RGB(220, 0, 0));
					if(i == 0 || i == 2)
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Right Cam Disconnect"));
					}
					else
					{
						m_ctrMainCamConnect[i].SetWindowTextW(_T("Left Cam Disconnect"));
					}
				}
			}
		}
		break;
	case MAIN_TIMER_CURRENT_TIME_SHOW:
		m_cCurTime = CTime::GetCurrentTime();
		m_strCurTime = m_cCurTime.Format("[%Y-%m-%d] %H:%M:%S %p");
		m_ctrMainTime.SetWindowTextW(m_strCurTime);
		break;
	case MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_0:
		InspEndRsltDisplay(0);
		break;
	case MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_1:
		InspEndRsltDisplay(1);
		break;
	case MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0:
		{
			int nChannel = 0;
			if(!m_bInspEndCheckThreadStart[0] && m_bInspEndCheckThreadEnd[0])
			{
				KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0);
			
				CString strText;
				SetSeqLightOnOff(nChannel, m_SeqPara[0].InspectionPara.size() - 1, 0, 1, FALSE);
				strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light Off"), nChannel + 1, m_SeqPara[0].InspectionPara.size(), 1, 2);
				m_cLogFile->WriteText(strText);
				SetSeqLightOnOff(nChannel, m_SeqPara[0].InspectionPara.size() - 1, 1, 1, FALSE);
				strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light Off"), nChannel + 1, m_SeqPara[0].InspectionPara.size(), 2, 2);
				m_cLogFile->WriteText(strText);
			
				m_lEndTime[nChannel] = GetCurrentTime();
				m_cEnd[nChannel] = CTime::GetCurrentTime(); 
				m_dTackTime[nChannel] = (float)(m_lEndTime[nChannel] - m_lGrabStart[nChannel]) / 1000.;

				RsltDisplay(nChannel);
				//m_bInspectionRslt[0] = TRUE;

				strText.Format(_T("%0.3f sec"), m_dTackTime[nChannel]);
				m_btn_TackTime[nChannel].SetWindowTextW(strText);

				strText.Format(_T("Ch No %d, SeqIndex End -> Tack Time %0.3f sec"), nChannel + 1, m_dTackTime[nChannel]);
				m_cLogFile->WriteText(strText);
				SetSendPacket(nChannel, END_SEQ);

				m_Vision[nChannel]->ReleaseInspection(TRUE);
				//ReleaseThread(0);
				m_bSeqEnd[nChannel] = TRUE;
				m_nSeqStepIndex[nChannel] = END_SEQ;
				SetSeqStepStart(nChannel, FALSE);
				SetSeqStepEnd(nChannel, TRUE);

				//m_dlgAvgRsltView[0]->SetSpreadAlarmCheck(PORT_BARCODE_EMPTY);
				if(m_dlgAvgRsltView[nChannel]->SpreadAlarmCheck())
				{
					SetAlarmMessage(nChannel, nChannel + 200);	
				}

	#ifdef TEST_RUN
				m_cTimeOut[nChannel].SetCheckTime(2000);
				m_cTimeOut[nChannel].StartTimer();
	#endif
				m_pBarcode[nChannel]->SetBarcodeReadStart(TRUE);
				SetBarcode(nChannel, _T("-"));

				/*if(m_dlgAvgRsltView[0]->GetSpreadAlarm())
				{
					SetAlarmMessage(0, 200);				
				}*/


				/*if(m_dlgAvgRsltView[0]->GetSpreadAlarm())
				{
					AfxMessageBox(_T("Spread Error"));
				}*/
			}
		
			if(m_bStopInitSeq[nChannel])
			{
				m_bStopInitSeq[nChannel] = FALSE;
				ReleaseThread(nChannel);
			}
		}
		break;
	case MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1:
		{
			int nChannel = 1;
			if(!m_bInspEndCheckThreadStart[1] && m_bInspEndCheckThreadEnd[1])
			{
				KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1);

				CString strText;
				SetSeqLightOnOff(nChannel, m_SeqPara[1].InspectionPara.size() - 1, 0, 1, FALSE);
				strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light Off"), nChannel + 1, m_SeqPara[1].InspectionPara.size(), 3, 2);
				m_cLogFile->WriteText(strText);
				SetSeqLightOnOff(nChannel, m_SeqPara[1].InspectionPara.size() - 1, 1, 1, FALSE);
				strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light Off"), nChannel + 1, m_SeqPara[1].InspectionPara.size(), 4, 2);
				m_cLogFile->WriteText(strText);

				m_lEndTime[nChannel] = GetCurrentTime();
				m_cEnd[nChannel] = CTime::GetCurrentTime();
				m_dTackTime[nChannel] = (float)(m_lEndTime[nChannel] - m_lGrabStart[nChannel]) / 1000.;

				RsltDisplay(nChannel);
				//m_bInspectionRslt[nChannel] = TRUE;

				strText.Format(_T("%0.3f sec"), m_dTackTime[nChannel]);
				m_btn_TackTime[nChannel].SetWindowTextW(strText);

				strText.Format(_T("Ch No %d, SeqIndex End -> Tack Time %0.3f sec"), nChannel + 1, m_dTackTime[nChannel]);
				m_cLogFile->WriteText(strText);
				SetSendPacket(nChannel, END_SEQ);

				m_Vision[nChannel]->ReleaseInspection(TRUE);
				//ReleaseThread(1);
				m_bSeqEnd[nChannel] = TRUE;
				m_nSeqStepIndex[nChannel] = END_SEQ;
				SetSeqStepStart(nChannel, FALSE);
				SetSeqStepEnd(nChannel, TRUE);

				if(m_dlgAvgRsltView[nChannel]->SpreadAlarmCheck())
				{
					SetAlarmMessage(nChannel, nChannel + 200);	
				}
				//m_dlgAvgRsltView[nChannel]->SetSpreadAlarmCheck(PORT_BARCODE_EMPTY);

	#ifdef TEST_RUN
				m_cTimeOut[nChannel].SetCheckTime(2000);
				m_cTimeOut[nChannel].StartTimer();
	#endif
				m_pBarcode[nChannel]->SetBarcodeReadStart(TRUE);
				SetBarcode(nChannel, _T("-"));

				/*if(m_dlgAvgRsltView[1]->GetSpreadAlarm())
				{
					SetAlarmMessage(1, 201);
				}*/

			}

			if(m_bStopInitSeq[nChannel])
			{
				m_bStopInitSeq[nChannel] = FALSE;
				ReleaseThread(nChannel);
			}
		}
		break;
	case MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_0:
		if(m_bSamsungLogCopyWritingWait[0] == m_bSamsungLogCopyWrite[0] && m_bSamsungLogCopy[0])
		{
			if(!m_bDstSamsungLogFilePath[0].IsEmpty() && !m_bSrcSamsungLogFilePath[0].IsEmpty())
			{
				if(CopyFile(m_bDstSamsungLogFilePath[0], m_bSrcSamsungLogFilePath[0], FALSE))
				{
					KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_0);
					m_bSamsungLogCopyWritingWait[0] = FALSE;
					m_bSamsungLogCopyWrite[0] = FALSE;
					m_bSamsungLogCopy[0] = FALSE;
					DeleteFile(m_bDstSamsungLogFilePath[0]);
				}
			}
		}
		else if(!m_bSamsungLogCopyWrite[0] && !m_bSamsungLogCopy[0])
		{
			KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_0);
		}
		break;
	case MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_1:
		if(m_bSamsungLogCopyWritingWait[1] == m_bSamsungLogCopyWrite[1] && m_bSamsungLogCopy[1])
		{
			if(!m_bDstSamsungLogFilePath[1].IsEmpty() && !m_bSrcSamsungLogFilePath[1].IsEmpty())
			{
				if(CopyFile(m_bDstSamsungLogFilePath[1], m_bSrcSamsungLogFilePath[1], FALSE))
				{
					KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_1);
					m_bSamsungLogCopyWritingWait[1] = FALSE;
					m_bSamsungLogCopyWrite[1] = FALSE;
					m_bSamsungLogCopy[1] = FALSE;
					DeleteFile(m_bDstSamsungLogFilePath[1]);
				}
			}
		}
		else if(!m_bSamsungLogCopyWrite[1] && !m_bSamsungLogCopy[1])
		{
			KillTimer(MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_1);
		}
		break;
	case MAIN_TIMER_SERVER_AUTO_CONNECT:
		if(!m_bClientConnect)
		{
#ifdef PLC_USE
			m_bClientConnectFailCheck = TRUE;
			m_Client->Connect(m_SystemPara.ServerPara.strIP, m_SystemPara.ServerPara.nPortNum);
			m_nClientAutoReconnectCnt++;
			m_nClientAutoReconnectCnt = 0;
#endif
		}
		break;
	case MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0:
		if(m_bInspLogCopyWritingWait[0] == m_bInspLogCopyWrite[0] && m_bInspLogCopy[0])
		{
			if(!m_bDstInspLogFilePath[0].IsEmpty() && !m_bSrcInspLogFilePath[0].IsEmpty())
			{
				if(CopyFile(m_bDstInspLogFilePath[0], m_bSrcInspLogFilePath[0], FALSE))
				{
					KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0);
					m_bInspLogCopyWritingWait[0] = FALSE;
					m_bInspLogCopyWrite[0] = FALSE;
					m_bInspLogCopy[0] = FALSE;
					DeleteFile(m_bDstInspLogFilePath[0]);
				}
			}
		}
		else if(!m_bInspLogCopyWrite[0] && !m_bInspLogCopy[0])
		{
			KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0);
		}
		break;
	case MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_1:
		if(m_bInspLogCopyWritingWait[1] == m_bInspLogCopyWrite[1] && m_bInspLogCopy[1])
		{
			if(!m_bDstInspLogFilePath[1].IsEmpty() && !m_bSrcInspLogFilePath[1].IsEmpty())
			{
				if(CopyFile(m_bDstInspLogFilePath[1], m_bSrcInspLogFilePath[1], FALSE))
				{
					KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_1);
					m_bInspLogCopyWritingWait[1] = FALSE;
					m_bInspLogCopyWrite[1] = FALSE;
					m_bInspLogCopy[1] = FALSE;
					DeleteFile(m_bDstInspLogFilePath[1]);
				}
			}
		}
		else if(!m_bInspLogCopyWrite[1] && !m_bInspLogCopy[1])
		{
			KillTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_1);
		}
		break;
	case MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_0:
		if(m_pDlgAlarm[0]->IsWindowVisible())
		{
			KillTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_0);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_AUTO);
			pBtnEnh->SetValue(FALSE);
		}
		break;
	case MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_1:
		if(m_pDlgAlarm[1]->IsWindowVisible())
		{
			KillTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_1);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_AUTO2);
			pBtnEnh->SetValue(FALSE);
		}
		break;
	case MAIN_TIMER_PLC_READ_CHECK_CHANNEL_0:
		if(!m_bReadPlcCheck[0])
		{
			SendMessage(WM_RECEIVE_DATA,0,0);
		}
		else
		{
			KillTimer(MAIN_TIMER_PLC_READ_CHECK_CHANNEL_0);
		}
		break;
	case MAIN_TIMEER_PLC_READ_CHANNEL_0:

		break;
	case MAIN_TIMEER_PLC_READ_CHANNEL_1:

		break;
	case MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0:
		//m_WritePlcCritical.Lock();
		WritePlcVal(0, 1000);
		//SetAutoStateWrite(0, m_bAutoState[0]);
		//m_WritePlcCritical.Unlock();
		break;
	case MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_1:
		//m_WritePlcCritical.Lock();
		WritePlcVal(1, 1000);
		//SetAutoStateWrite(1, m_bAutoState[1]);
		//m_WritePlcCritical.Unlock();
		break;
	case 10000:
#ifdef TEST_RUN
		if(m_nTestCnt[0] == 0)
		{
			KillTimer(10000);
		}

		if(m_bAutoInsp[0])// && !g_SeqModule[0].GetInspecting())
		{
			/*if(!m_cTimeOut[0].IsTimeOver())
				break;*/

			KillTimer(10000);
			
			for(int i = 0; i < 1; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				if(m_nSeqStepIndex[i] == -1)
				{
					m_cStart[i] = CTime::GetCurrentTime();
					m_lStartTime[i] = GetCurrentTime();
					m_strBarcode[i].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[i].GetYear(), m_cStart[i].GetMonth(), m_cStart[i].GetDay(), m_cStart[i].GetHour(), m_cStart[i].GetMinute(), m_cStart[i].GetSecond());
					SetBarcode(i, m_strBarcode[i]);
				}
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
			//SetTimer(10000, 1000, NULL);
		}
#endif
		break;
	case 10001:
#ifdef TEST_RUN
		if(m_nTestCnt[1] == 0)
		{
			KillTimer(10001);
		}
		if(m_bAutoInsp[1])// && !g_SeqModule[1].GetInspecting())
		{
			/*if(!m_cTimeOut[1].IsTimeOver())
				break;*/

			KillTimer(10001);

			for(int i = 1; i < MAX_CHANNEL_CNT; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				if(m_nSeqStepIndex[i] == -1)
				{
					m_cStart[i] = CTime::GetCurrentTime();
					m_lStartTime[i] = GetCurrentTime();
					m_strBarcode[i].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[i].GetYear(), m_cStart[i].GetMonth(), m_cStart[i].GetDay(), m_cStart[i].GetHour(), m_cStart[i].GetMinute(), m_cStart[i].GetSecond());
					SetBarcode(i, m_strBarcode[i]);
				}
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
			//SetTimer(10001, 1000, NULL);
		}
#endif
		break;
	case MAIN_TIMER_DISPLAY_DIALOG:
		GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW)->GetWindowRect(&rect1);
		GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW2)->GetWindowRect(&rect2);
		m_dlgInspPoint[0]->GetWindowRect(&rect3);

		if(rect1.left != rect3.left || rect1.top != rect3.top)
		{
			m_dlgInspPoint[0]->MoveWindow(rect1.left, rect1.top, rect3.Width(), rect3.Height());
			m_dlgInspPoint[1]->MoveWindow(rect2.left, rect2.top, rect3.Width(), rect3.Height());

			m_dlgInspPoint[0]->GetWindowRect(&rect2);
			m_dlgInspRslt[0]->GetWindowRect(&rect3);
			m_dlgInspRslt[0]->MoveWindow(rect1.left + rect2.Width(), rect1.top, rect3.Width(), rect3.Height());

			GetDlgItem(IDC_STATIC_MAIN_RSLT_VIEW2)->GetWindowRect(&rect1);
			m_dlgInspPoint[1]->GetWindowRect(&rect2);
			m_dlgInspRslt[1]->GetWindowRect(&rect3);
			m_dlgInspRslt[1]->MoveWindow(rect1.left + rect2.Width(), rect1.top, rect3.Width(), rect3.Height());

			GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);
			GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect2);

			m_dlgSystem->MoveWindow(rect2.left + (rect2.Width() / 2) - ((rect1.left - rect2.right) / 2), rect2.top, rect2.Width() + (rect1.left - rect2.right), rect2.Height());

			m_dlgTeach[0]->MoveWindow(rect1.left, rect1.top, rect1.Width(), rect1.Height());
			m_dlgTeach[1]->MoveWindow(rect2.left, rect2.top, rect2.Width(), rect2.Height());
		}
		break;
	case MAIN_TIMER_BARCODE_READER_CANNEL_0:
		BarcodeReaderTimer(0);
		break;
	case MAIN_TIMER_BARCODE_READER_CANNEL_1:
		BarcodeReaderTimer(1);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CGapInspectionDlg::BarcodeReaderTimer(int nChannel)
{
	BOOL rslt = FALSE;
	if(m_SystemPara.bBarcodeUse)
	{
		if(m_bAutoInsp[nChannel])
		{
			//this->SetFocus();

			CString strBarcode;
			strBarcode.Format(_T("%s"), m_pBarcode[nChannel]->GetBarcodeReader());
			
			if(m_pBarcode[nChannel]->GetReadBarcodeLength() > 0)
			{
				CString strText;
				strText.Format(_T("%d Cnt"), m_pBarcode[nChannel]->GetReadBarcodeLength());
				m_cLogFile->WriteText(strText);
			}

			if(strBarcode.GetLength() >= BARCODE_MAX_STRING_CNT)
			{
				TypeStringBarcode typeBarcode;
				sprintf_s((char*)typeBarcode.byBarcode, strBarcode.GetLength() + 1, "%S", strBarcode);

				BOOL isDel = FALSE;
				for(int i = 0; i < m_vBarcode[!nChannel].size(); i++)
				{
					isDel = FALSE;
					vector<TypeStringBarcode>::iterator it;
					it = m_vBarcode[!nChannel].begin() + i;
					CString strTemp;
					strTemp.Format(_T("%S"), it->byBarcode);

					if(!strBarcode.Compare(strTemp))
					{
						m_vBarcode[!nChannel].erase(it);
						isDel = TRUE;
					}

					if(isDel && m_vBarcode[!nChannel].size() == 0)
					{
						m_strBarcode[!nChannel].Format(_T("-"));
						m_ctrMainBarcode[!nChannel].SetWindowTextW(m_strBarcode[!nChannel]);
					}
					else if(isDel && m_vBarcode[!nChannel].size() > 0)
					{
						m_strBarcode[!nChannel].Format(_T("%S"), m_vBarcode[!nChannel][0].byBarcode);
						m_ctrMainBarcode[!nChannel].SetWindowTextW(m_strBarcode[!nChannel]);
					}
				}

				BOOL isCompare = FALSE;
				for(int i = 0; i < m_vBarcode[nChannel].size(); i++)
				{
					vector<TypeStringBarcode>::iterator it;
					it = m_vBarcode[nChannel].begin() + i;
					CString strTemp;
					strTemp.Format(_T("%S"), it->byBarcode);

					if(!strBarcode.Compare(strTemp))
					{
						m_vBarcode[nChannel].erase(it);
						break;
					}
				}

				typeBarcode.nBarcodeLength = strBarcode.GetLength();
				m_vBarcode[nChannel].push_back(typeBarcode);
				if(m_vBarcode[nChannel].size() > 0)
				{
					CString strTemp;
					strTemp.Format(_T("%S"), m_vBarcode[nChannel][0].byBarcode);
					SetBarcode(nChannel, strTemp);
				}
			}
		}
	}
	
	return rslt;
}

void CGapInspectionDlg::SetNextBarcode(int nChannel, CString strBarcode)
{
	CString strText, strTemp;
	if(strBarcode.IsEmpty())
	{
		strText.Format(_T("Next Barcode : "));
		m_strNextBarcode[nChannel].Format(_T(""));
	}
	else
	{
		strText.Format(_T("Next Barcode : %s"), strBarcode);
		m_strNextBarcode[nChannel].Format(_T("%s"), strBarcode);
	}
	if(nChannel == 0)
	{
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE0)->SetWindowText(strText);
		
	}
	else
	{
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NEXT_BARCODE1)->SetWindowText(strText);
	}
}

void CGapInspectionDlg::ClickBtnenhctrlZoomView0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgZoomView->IsWindowVisible())
		m_dlgZoomView->ShowWindow(SW_HIDE);
	
	if(m_dlgTeach[0]->IsWindowVisible())
	{
		m_nChannelIdx = NO_1_CHANNEL;
		SetZoomView(m_nChannelIdx, TOP_CAMERA);
		//m_dlgZoomView->SetCamIdx(NO_1_CHANNEL + TOP_CAMERA);
		if(!m_dlgZoomView->IsWindowVisible())
			m_dlgZoomView->ShowWindow(SW_SHOW);
	}
}

void CGapInspectionDlg::ClickBtnenhctrlZoomView1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgZoomView->IsWindowVisible())
		m_dlgZoomView->ShowWindow(SW_HIDE);
	
	if(m_dlgTeach[0]->IsWindowVisible())
	{
		m_nChannelIdx = NO_1_CHANNEL;
		SetZoomView(m_nChannelIdx, BOTTOM_CAMERA);
		//m_dlgZoomView->SetCamIdx(NO_1_CHANNEL + BOTTOM_CAMERA);
		if(!m_dlgZoomView->IsWindowVisible())
			m_dlgZoomView->ShowWindow(SW_SHOW);
	}
}

void CGapInspectionDlg::ClickBtnenhctrlZoomView2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgZoomView->IsWindowVisible())
		m_dlgZoomView->ShowWindow(SW_HIDE);
	
	if(m_dlgTeach[1]->IsWindowVisible())
	{
		m_nChannelIdx = NO_2_CHANNEL;
		SetZoomView(NO_2_CHANNEL, TOP_CAMERA);
		//m_dlgZoomView->SetCamIdx(NO_2_CHANNEL + TOP_CAMERA + 1);
		if(!m_dlgZoomView->IsWindowVisible())
			m_dlgZoomView->ShowWindow(SW_SHOW);
	}
}

void CGapInspectionDlg::ClickBtnenhctrlZoomView3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgZoomView->IsWindowVisible())
		m_dlgZoomView->ShowWindow(SW_HIDE);
	
	if(m_dlgTeach[1]->IsWindowVisible())
	{
		m_nChannelIdx = NO_2_CHANNEL;
		SetZoomView(NO_2_CHANNEL, BOTTOM_CAMERA);
		//m_dlgZoomView->SetCamIdx(NO_2_CHANNEL + BOTTOM_CAMERA + 1);
		if(!m_dlgZoomView->IsWindowVisible())
			m_dlgZoomView->ShowWindow(SW_SHOW);
	}
}

void CGapInspectionDlg::SetZoomView(int nChannel, int nCamIdx)
{
	int nIdx = nCamIdx;
	TypeSeqPara SeqPara = m_dlgTeach[nChannel]->GetSeqPara();
	int nSelectSeqIdx = m_dlgTeach[nChannel]->GetSelectSeqIdx();
	int nMaxTracker = MAX_LEFT_RIGHT_INSP_RECT_CNT;

	if(SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nIdx] == INSP_POINT2 || SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nIdx] == INSP_POINT5)
	{
		nMaxTracker = 2;
	}
	else if(SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nIdx] == INSP_SKIP)
	{
		return;
	}

	for(int i = 0; i < nMaxTracker; i++)
	{
		m_dlgZoomView->SetTrackerRect(i, SeqPara.InspectionPara[nSelectSeqIdx].InspPara[nIdx].InspRect[i]);
	}

	m_dlgZoomView->SetInspPoint(SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nIdx] - 1);

	m_dlgZoomView->SetCamIdx(nChannel, nCamIdx);
	m_dlgZoomView->ShowWindow(SW_SHOW);
}

UINT CGapInspectionDlg::MotionThread0(LPVOID pParam)
{
	g_SeqModule[0].SequenceLoop();

	return 0;
}

UINT CGapInspectionDlg::MotionThread1(LPVOID pParam)
{
	g_SeqModule[1].SequenceLoop();

	return 0;
}

BOOL CGapInspectionDlg::SetGrab(int nChannelIdx, int nSeqIdx, int nCamIdx, int nGrabIdx, BOOL isDisplay)
{
	BOOL rslt = TRUE;
	CString strText;
	int nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
	//light on

	if(nCamIdx == BOTTOM_CAMERA)
	{
		nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
		if(m_nChannelIdx == NO_2_CHANNEL)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL + 1);
		}
	}
	else
	{
		nLightStartCh = BOT_CAMERA_LIGHT_START_IDX;
		if(m_nChannelIdx == NO_2_CHANNEL)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL);
		}
	}

	for(int i = 0; i < MAX_LIGHT_CNT; i++)
	{
		if(m_SeqLightPara[nChannelIdx][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
		{
			if(!SetLightOnof(nChannelIdx, nLightStartCh + i, m_SeqLightPara[nChannelIdx][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i]))
			{
				strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
				m_cLogFile->WriteText(strText);
				//AfxMessageBox(strText);
				return FALSE;
			}
		}
	}

	//grab
	rslt = GrabSingle(nChannelIdx, nCamIdx, nGrabIdx);

	//ligh off
 	for(int i = 0; i < MAX_LIGHT_CNT; i++)
	{
		if(m_SeqLightPara[nChannelIdx][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
		{
			if(!SetLightOnof(nChannelIdx, nLightStartCh + i))
			{
				strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
				m_cLogFile->WriteText(strText);
				//AfxMessageBox(strText);
				return FALSE;
			}
		}
	}
	TypeSeqPara SeqPara;
	SeqPara = m_dlgTeach[nChannelIdx]->GetSeqPara();

	if(isDisplay)
	{
		if(SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] != INSP_SKIP)
		{
			cvCvtColor(m_Vision[nChannelIdx]->GetGrabImage(nCamIdx, nGrabIdx), m_Vision[nChannelIdx]->GetDispImage(nCamIdx), CV_GRAY2BGR);
			m_MainDisplay[nChannelIdx][SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] - 1].SetImage(m_Vision[nChannelIdx]->GetDispImage(nCamIdx));
		}
	}

	return rslt;
}

LRESULT CGapInspectionDlg::OnZoomViewToMainMessage(WPARAM para0, LPARAM para1)
{
	int nEvent = (int)para0;
	int nCamIdx = (int)para1;
	int nSeqIdx = 0;
	int nTrackerIdx = (int)para1;
	CRect rect;
	TypeSeqPara SeqPara;
	int nSelectSeqIdx = 0;
	int nMaxTracker = 0;
	CString strText;
	int nLightStartCh;
	CTime cTime = CTime::GetCurrentTime();

	
	switch(nEvent)
	{
	case ZOOM_VIEW_TO_MAIN_GRAB_MSG:
		if(SetGrab(m_nChannelIdx, m_dlgTeach[m_nChannelIdx]->GetSelectSeqIdx(), nCamIdx, m_dlgZoomView->GetGrabIdx()))
		{
			m_dlgZoomView->SetImage(m_Vision[m_nChannelIdx]->GetGrabImage(nCamIdx, m_dlgZoomView->GetGrabIdx()));
		}
		break;
	case ZOOM_VIEW_TO_MAIN_LIVE_START_MSG:
		//Light on
		if(nCamIdx == TOP_CAMERA)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
			if(m_nChannelIdx == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL + 1);
			}
		}
		else
		{
			nLightStartCh = BOT_CAMERA_LIGHT_START_IDX;
			if(m_nChannelIdx == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL);
			}
		}

		m_nCamReconnectCnt[nCamIdx + (m_nChannelIdx * MAX_CHANNEL_CNT)] = 0;
		if(m_SeqLightPara[m_nChannelIdx].size() > 0)
		{
			for(int i = 0; i < MAX_LIGHT_CNT; i++)
			{
				if(m_SeqLightPara[m_nChannelIdx][nSeqIdx].nLightVal[nCamIdx][m_dlgZoomView->GetGrabIdx()][0] > 0)
				{
					if(!SetLightOnof(m_nChannelIdx, nLightStartCh + i, m_SeqLightPara[m_nChannelIdx][nSeqIdx].nLightVal[nCamIdx][m_dlgZoomView->GetGrabIdx()][0]))
					{
						strText.Format(_T("Light ch %d Error"), nLightStartCh + i);
						m_cLogFile->WriteText(strText);
						AfxMessageBox(strText);
						return FALSE;
					}
				}
			}
			///////////////////////////////

			bStopThread[nCamIdx + (m_nChannelIdx * MAX_CHANNEL_CNT)] = false;
			if(!SetLive(m_nChannelIdx, nCamIdx, m_dlgZoomView->GetGrabIdx()))
			{
				m_dlgZoomView->SetLive(nCamIdx, FALSE);
			}

			m_dlgZoomView->SetLive(nCamIdx, m_bLiveFlag[nCamIdx + (m_nChannelIdx * MAX_CHANNEL_CNT)]);
		}
		break;
	case ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG:
		//Light off
		if(nCamIdx == TOP_CAMERA)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
			if(m_nChannelIdx == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL + 1);
			}
		}
		else
		{
			nLightStartCh = BOT_CAMERA_LIGHT_START_IDX;
			if(m_nChannelIdx == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL);
			}
		}

		if(m_SeqLightPara[m_nChannelIdx].size())
		{
			for(int i = 0; i < MAX_LIGHT_CNT; i++)
			{
				if(m_SeqLightPara[m_nChannelIdx][nSeqIdx].nLightVal[nCamIdx][m_dlgZoomView->GetGrabIdx()][0] > 0)
				{
					if(!SetLightOnof(m_nChannelIdx, nLightStartCh + i))
					{
						strText.Format(_T("Light ch %d Error"), nLightStartCh + i);
						m_cLogFile->WriteText(strText);
						AfxMessageBox(strText);
						return FALSE;
					}
				}
			}
			///////////////////////////
			bStopThread[nCamIdx + (m_nChannelIdx * MAX_CHANNEL_CNT)] = true;
			if(!SetLive(m_nChannelIdx, nCamIdx, m_dlgZoomView->GetGrabIdx()))
			{
				m_dlgZoomView->SetLive(nCamIdx, FALSE);
			}

			m_dlgZoomView->SetLive(nCamIdx, m_bLiveFlag[nCamIdx + (m_nChannelIdx * MAX_CHANNEL_CNT)]);
		}
		break;
	case ZOOM_VIEW_TO_MAIN_INSPECTION_MSG:
		//20190218 ngh
		m_Vision[m_nChannelIdx]->SetStartDate(cTime);
		strText.Format(_T("%02d%02d%02d"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
		m_Vision[m_nChannelIdx]->SetBarcode(strText);
		strText.Format(_T("%s%04d\\%02d\\%02d\\%02d%02d%02d\\"), RSLT_IMAGE_FILE_PATH, cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
		if(m_SystemPara.bInspImageSave)
			CreateDir(strText);
		m_Vision[m_nChannelIdx]->SetSeqPara(m_dlgTeach[m_nChannelIdx]->GetSeqPara());
		m_nManualInspCamIdx = nCamIdx;
		m_Vision[m_nChannelIdx]->SetSystemPara(m_dlgSystem->GetSystemPara());
		m_Vision[m_nChannelIdx]->SetManualInspection(m_dlgTeach[m_nChannelIdx]->GetSelectSeqIdx(), nCamIdx, m_dlgZoomView->GetDisplayHWND(), m_dlgZoomView->GetDisplayId());
		SetTimer(MAIN_TIMER_EVENT_MANUAL_INSPECTION_END_CHECK, 10, NULL);
		break;
	case ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_CAM_CHANGE:
		m_dlgZoomView->SetImage(m_Vision[m_nChannelIdx]->GetGrabImage(nCamIdx, m_dlgZoomView->GetGrabIdx()));
		SeqPara = m_dlgTeach[m_nChannelIdx]->GetSeqPara();

		nSelectSeqIdx = m_dlgTeach[m_nChannelIdx]->GetSelectSeqIdx();
		nMaxTracker = MAX_LEFT_RIGHT_INSP_RECT_CNT;

		if(SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nCamIdx] == INSP_POINT2 || SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nCamIdx] == INSP_POINT5)
		{
			nMaxTracker = MAX_CENTER_INSP_RECT_CNT;
		}
		else if(SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] == INSP_SKIP)
		{
			break;
		}

		for(int i = 0; i < nMaxTracker; i++)
		{
			m_dlgZoomView->SetTrackerRect(i, SeqPara.InspectionPara[nSelectSeqIdx].InspPara[nCamIdx].InspRect[i]);
		}

		m_dlgZoomView->SetActiveTracker(SeqPara.InspectionPara[nSelectSeqIdx].nInspPoint[nCamIdx] - 1);

		m_dlgZoomView->UpdateDisplay();

		//SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, 0);
		//SendMessage(USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG, ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG, 1);

		break;
	case ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_IMAGE_CHANGE:
		m_dlgZoomView->SetImage(m_Vision[m_nChannelIdx]->GetGrabImage(nCamIdx, m_dlgZoomView->GetGrabIdx()));
		break;
	case ZOOM_VIEW_TO_MAIN_IMAGE_SAVE_MSG:
		SavelpImage(m_nChannelIdx, nCamIdx, m_dlgZoomView->GetGrabIdx());
		break;
	case ZOOM_VIEW_TO_MAIN_IMAGE_LOAD_MSG:
		LoadlpImage(m_nChannelIdx, nCamIdx, m_dlgZoomView->GetGrabIdx());
		break;
	case ZOOM_VIEW_TO_MAIN_SET_TRACKER_RECT:
		rect = m_dlgZoomView->GetTrackerRect(m_nSelectTrackerIdx);
		m_dlgTeach[m_nChannelIdx]->SetSelectSeqInspRect(nCamIdx, m_dlgZoomView->GetGrabIdx(), m_nSelectTrackerIdx, rect);
		break;
	case ZOOM_VIEW_TO_MAIN_SET_TRACKER_IDX:
		m_nSelectTrackerIdx = nTrackerIdx;
		break;
	default:

		break;
	}

	return 0;
}

//20190128 ngh
BOOL CGapInspectionDlg::SetLightOnof(int nChannelIdx, int nLightIdx, int nLightVal)
{
	BOOL rslt = TRUE;
	
	if((0 <= nLightIdx && nLightIdx < (MAX_LIGHT_CNT * MAX_CAM_CNT)) && (0 <= nLightVal && nLightVal <= MAX_VAL))
	{
		WritePacket(nLightIdx, nLightVal);
	}
	else
	{
		rslt = FALSE;
	}

	return rslt;
}

void CGapInspectionDlg::SetInspEndCheckTimer(int nChannel)
{
	if(nChannel == 0)
	{
		SetTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0, 100, NULL);
	}
	else
	{
		SetTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1, 100, NULL);
	}
}

void CGapInspectionDlg::SetInspection(int nChannelIdx, int nSeqIdx, int nCamIdx)
{
	m_Vision[nChannelIdx]->SetInspStepIdx(nSeqIdx);
	
	if(nCamIdx == -1)
	{
		m_Vision[nChannelIdx]->SetStartDate(m_cStart[nChannelIdx]);
		m_Vision[nChannelIdx]->SetBarcode(m_strBarcode[nChannelIdx]);
		//m_Vision[nChannelIdx]->SetSeqPara(m_SeqPara[nChannelIdx]);
		if(!m_bAutoInsp[nChannelIdx])
			return;

		m_Vision[nChannelIdx]->SetInspection();

		if(nSeqIdx == 0)
		{
			if(nChannelIdx == 0)
			{
				//SetTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0, 100, NULL);
				SetTimer(MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_0, 100, NULL);
			}
			else
			{
				//SetTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1, 100, NULL);
				SetTimer(MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_1, 100, NULL);
			}
		}
	}
	else
	{
		CTime cTime = CTime::GetCurrentTime();
		m_Vision[nChannelIdx]->SetStartDate(cTime);
		m_Vision[nChannelIdx]->SetSeqPara(m_dlgTeach[m_nChannelIdx]->GetSeqPara());
		m_nManualInspCamIdx = nCamIdx;
		m_Vision[nChannelIdx]->SetManualInspection(nSeqIdx, nCamIdx, m_dlgZoomView->GetDisplayHWND(), m_dlgZoomView->GetDisplayId());
	}
}

void CGapInspectionDlg::SetSeqLightPara(int nChannel)
{
	TypeSeqLightPara SeqLight;
	TypeSeqPara SeqPara;
	SeqPara = m_dlgTeach[nChannel]->GetSeqPara();

	if(m_SeqLightPara[nChannel].size() > 0)
		m_SeqLightPara[nChannel].clear();

	for(int i = 0; i < SeqPara.InspectionPara.size(); i++)
	{
		for(int j = 0; j < MAX_CAM_CNT / 2; j++)
		{
			for(int k = 0; k < MAX_GRAB_CNT; k++)
			{
				SeqLight.nLightVal[j][k][0] = SeqPara.InspectionPara[i].nLightVal[j][k][0];
				SeqLight.nLightVal[j][k][1] = SeqPara.InspectionPara[i].nLightVal[j][k][1];
				SeqLight.nLightVal[j][k][2] = SeqPara.InspectionPara[i].nLightVal[j][k][2];
			}
		}
		m_SeqLightPara[nChannel].push_back(SeqLight);
	}
}

LRESULT CGapInspectionDlg::OnTeachToMainMessage(WPARAM para0, LPARAM para1)
{
	int nEvent = (int)para0;
	int nSeqIdx = (int)para1;
	int nCamIdx = 0;
	int nMaxTracker = 0;
	TypeSeqPara SeqPara;
	double dAxisPos = 0.;
	int nAxis = 0;
	int nCylFw = (int)para1;
	int nChannel = (int)para1;

	switch(nEvent)
	{
	case TEACH_TO_MAIN_SELECT_SEQ_CHANGE:
		if(m_dlgZoomView->IsWindowVisible())
		{
			nCamIdx = m_dlgZoomView->GetCamIdx();
			SeqPara = m_dlgTeach[m_nChannelIdx]->GetSeqPara();

			nMaxTracker = MAX_LEFT_RIGHT_INSP_RECT_CNT;

			if(nSeqIdx > -1)
			{
				if(SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] == INSP_POINT2 || SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] == INSP_POINT5)
				{
					nMaxTracker = MAX_CENTER_INSP_RECT_CNT;
				}
				else if(SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] == INSP_SKIP)
				{
					break;
				}

				for(int i = 0; i < nMaxTracker; i++)
				{
					m_dlgZoomView->SetTrackerRect(i, SeqPara.InspectionPara[nSeqIdx].InspPara[nCamIdx].InspRect[i]);
				}

				m_dlgZoomView->SetActiveTracker(SeqPara.InspectionPara[nSeqIdx].nInspPoint[nCamIdx] - 1);
			}

			m_dlgZoomView->UpdateDisplay();
		}
		break;
	case TEACH_TO_MAIN_APPLY_SEQ_PARA:
		if(m_strUseModelName.IsEmpty() || !m_strUseModelName.Compare(_T("0")) || !m_strUseModelName.Compare(_T("-")))
			break;

		SeqPara = m_dlgTeach[m_nChannelIdx]->GetSeqPara();
		m_Vision[m_nChannelIdx]->SetSeqPara(SeqPara);
		//SetSeqMotionPara();
		SetSeqLightPara(m_nChannelIdx);
		m_dlgAvgRsltView[m_nChannelIdx]->SetSpec(SeqPara.Spec);
		m_dlgAvgRsltView[m_nChannelIdx]->SetSpec(SeqPara.SpecPoint, 1);
		m_dlgAvgRsltView[m_nChannelIdx]->SetSpread(SeqPara.Spread);
		m_dlgAvgRsltView[m_nChannelIdx]->SetSpread(SeqPara.SpreadPoint, 1);

		TypeSpecPara sSpec[MAX_RSLT_POINT_CNT];
		m_dlgTeach[m_nChannelIdx]->GetSpec(sSpec);
		for(int j = 0; j < MAX_RSLT_POINT_CNT; j++)
		{
			CString strText;
			strText.Format(_T("%.3f (Min) ~ %.3f (Max)"), sSpec[j].dMinSpec, sSpec[j].dMaxSpec);
			m_ctrMainSpec[m_nChannelIdx][j].SetWindowTextW(strText);
		}

		break;
	case TEACH_TO_MAIN_LOG_WRITE:
		m_cLogFile->WriteText(m_dlgTeach[m_nChannelIdx]->GetLogString());
		break;
	case TEACH_TO_MAIN_DIALOG_CANCEL:
		m_dlgTeach[nChannel]->SetModel(m_SystemPara.strUseModelName);
		m_dlgTeach[nChannel]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
		m_dlgTeach[nChannel]->ShowWindow(SW_HIDE);
		if(m_dlgZoomView->IsWindowVisible())
		{
			m_dlgZoomView->ShowWindow(SW_HIDE);
		}
		if(nChannel == NO_1_CHANNEL)
		{
			CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_MAIN_TEACH2);
			pBtnEnh->SetValue(FALSE);
		}
		break;
	default:

		break;
	}

	return 0;
}

LRESULT CGapInspectionDlg::OnSystemToMainMessage(WPARAM para0, LPARAM para1)
{
	int nEvent = (int)para0;
	TypeSystemPara SystemPara;
	switch(nEvent)
	{
	case SYSTEM_TO_MAIN_APPLY_SYSTEM_PARA:
		m_cAutoDelete->SetAutoDeleteThreadWait(TRUE);
		m_SystemPara = m_dlgSystem->GetSystemPara();
		m_Vision[0]->SetSystemPara(m_SystemPara);
		m_Vision[1]->SetSystemPara(m_SystemPara);
		m_SystemPara.DirAutoDelPara.bDriveFreeSizeAutoDel =0;// FREE_SIZE_AUTO_DEL;
		m_SystemPara.DirAutoDelPara.dDelDrivePercent = AUTO_DEL_DRIVE_SIZE_PER;
		m_SystemPara.DirAutoDelPara.bManualDriveFreeSizeAutoDel = FREE_SIZE_AUTO_DEL;
		m_cAutoDelete->SetAutoDelPara(m_SystemPara.DirAutoDelPara);
		//m_cAutoDelete->SetFirst(TRUE);

		m_dlgTeach[0]->SetModel(m_SystemPara.strUseModelName);
		m_dlgTeach[1]->SetModel(m_SystemPara.strUseModelName);
		m_dlgTeach[0]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
		m_dlgTeach[1]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
		m_dlgInspRslt[0]->SetNGImageOpen(m_SystemPara.bRsltNGImageSave);
		m_dlgInspRslt[0]->SetOKImageOpen(m_SystemPara.bRsltImageSave);
		m_dlgInspRslt[1]->SetNGImageOpen(m_SystemPara.bRsltNGImageSave);
		m_dlgInspRslt[1]->SetOKImageOpen(m_SystemPara.bRsltImageSave);

		m_dlgTeach[0]->SetDGSModelName(m_dlgSystem->GetDGSModelName());
		m_dlgTeach[1]->SetDGSModelName(m_dlgSystem->GetDGSModelName());

		DisplayModelName(m_SystemPara.strUseModelName);
		m_dlgSystem->ShowWindow(SW_HIDE);

		if(m_SystemPara.bVIPMode)
		{
			NGRatioView(FALSE);
		}
		else
		{
			NGRatioView(TRUE);
		}
		break;
	case SYSTEM_TO_MAIN_MODEL_ADD:
		m_dlgTeach[0]->SetModelAdd(m_dlgSystem->GetSelectAddModelName());
		m_dlgTeach[1]->SetModelAdd(m_dlgSystem->GetSelectAddModelName());
		break;
	case SYSTEM_TO_MAIN_MODEL_DELETE:

		break;
	case SYSTEM_TO_MAIN_MODEL_CHANGE:
		m_SystemPara = m_dlgSystem->GetSystemPara();
		m_strUseModelName.Format(_T("%s"), m_dlgSystem->GetUseModel());
		m_dlgTeach[0]->SetModel(m_strUseModelName);
		m_dlgTeach[1]->SetModel(m_strUseModelName);
		m_dlgTeach[0]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
		m_dlgTeach[1]->SetAlgorithmIdx(m_SystemPara.nAlgorithmIdx);
		DisplayModelName(m_strUseModelName);
		break;
	}
	return 0;
}

LRESULT CGapInspectionDlg::OnMainGrabDisplayCh0Message(WPARAM para0, LPARAM para1)
{
	int nChannel = 0;
	int nCameraIdx = (int)para0;
	int nSeqIdx = (int)para1;

	cvCvtColor(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 0), m_Vision[nChannel]->GetDispImage(nCameraIdx), CV_GRAY2BGR);
	m_MainDisplay[nChannel][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1].SetImage(m_Vision[nChannel]->GetDispImage(nCameraIdx));	
	
	return 0;
}

LRESULT CGapInspectionDlg::OnMainGrabDisplayCh1Message(WPARAM para0, LPARAM para1)
{
	int nChannel = 1;
	int nCameraIdx = (int)para0;
	int nSeqIdx = (int)para1;

	cvCvtColor(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 0), m_Vision[nChannel]->GetDispImage(nCameraIdx), CV_GRAY2BGR);
	m_MainDisplay[nChannel][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1].SetImage(m_Vision[nChannel]->GetDispImage(nCameraIdx));	
	
	return 0;
}

LRESULT CGapInspectionDlg::OnMainInspEndDisplayMessage(WPARAM para0, LPARAM para1)
{
	int nChannel = (int)para1;
	//int nStepCamIdx = (int)para1;

	//m_MainDisplay[nChannel][nStepCamIdx].SetImage(m_Vision[nChannel]->GetRsltImage(nStepCamIdx));

	switch((int)para0)
	{
	case 10:
		SetBarcode(nChannel, m_strBarcode[nChannel]);	
		break;
	}

	return 0;
}

void CGapInspectionDlg::DisplayModelName(CString strModel)
{
	m_ctrMainWorkModel[0].SetWindowTextW(strModel);
	m_ctrMainWorkModel[1].SetWindowTextW(strModel);
}

void CGapInspectionDlg::SetBarcode(int nChannel, CString strBarcode)
{
	if(!strBarcode.Compare(_T("-")))
	{
		m_bBarcodeInputEnd[nChannel] = FALSE;
		if(m_vBarcode[nChannel].size() > 0)
		{
			vector<TypeStringBarcode>::iterator it;
			it = m_vBarcode[nChannel].begin();
			m_vBarcode[nChannel].erase(it);

			if(m_vBarcode[nChannel].size() > 0)
			{
				strBarcode.Format(_T("%S"), m_vBarcode[nChannel][0].byBarcode);
			}
		}
	}

	m_strBarcode[nChannel].Format(_T("%s"), strBarcode);
	m_ctrMainBarcode[nChannel].SetWindowTextW(strBarcode);
}

void CGapInspectionDlg::SetInitBarcode(int nChannel)
{
	m_strBarcode[nChannel].Format(_T("-"));
	m_ctrMainBarcode[nChannel].SetWindowTextW(_T("-"));
}

void CGapInspectionDlg::SetStartTime(int nChannel, CTime *cStart)
{
	m_cStart[nChannel] = *cStart;
	m_dlgInspRslt[nChannel]->SetStartTime(&m_cStart[nChannel]);
}

void CGapInspectionDlg::SetEndTIme(int nChannel, CTime *cEnd)
{
	m_cEnd[nChannel] = *cEnd;
}

void CGapInspectionDlg::InitInspection(int nChannel)
{
	CString strText;
	strText.Format(_T("Ch No %d, InitInspection Start"), nChannel + 1);
	m_cLogFile->WriteText(strText);

	m_bSeqEnd[nChannel] = FALSE;

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_bInspRslt[nChannel][i] = FALSE;
		m_dInspRslt[nChannel][i] = 0.;
	}

	for(int i = 0; i < m_dlgTeach[nChannel]->GetMaxSeqCnt(); i++)
	{
		m_bInspectionEnd[nChannel][i] = FALSE;
	}

	int nCamIdx[2];

	if(nChannel == 0)
	{
		nCamIdx[0] = 0;
		nCamIdx[1] = 1;
	}
	else
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
	}
	m_bInspEndCheckThreadStart[nChannel] = FALSE;
	m_bInspEndCheckThreadEnd[nChannel] = TRUE;
	m_bGrabThreadStart[nCamIdx[0]] = FALSE;
	m_bGrabThreadEnd[nCamIdx[0]] = TRUE;
	m_bGrabThreadStart[nCamIdx[1]] = FALSE;
	m_bGrabThreadEnd[nCamIdx[1]] = TRUE;
	m_bThreadGrabEndCheckEnd[nChannel] = TRUE;

	SetReleaseGrabThread(nChannel);

	m_Vision[nChannel]->ReleaseInspection(TRUE);

	m_Vision[nChannel]->SetInitRslt();

	//SetBarcode(nChannel, m_strBarcode[nChannel]);	
	SendMessage(USE_MSG_MAIN_INSP_END_DISPLAY_DIALOG, 10, nChannel);

	g_SeqModule[nChannel].SetMaxStepCnt(m_SeqPara[nChannel].InspectionPara.size());
	m_dlgInspRslt[nChannel]->SetSpec(m_SeqPara[nChannel].Spec);

	m_bGrabThreadStart[nCamIdx[0]] = FALSE;
	m_bGrabThreadEnd[nCamIdx[0]] = FALSE;
	m_bGrabThreadStart[nCamIdx[1]] = FALSE;
	m_bGrabThreadEnd[nCamIdx[1]] = FALSE;
	m_bThreadGrabEndCheckEnd[nChannel] = FALSE;

	if(nChannel == 0)
	{
		m_pThreadGrab[0] = AfxBeginThread(GrabThread0, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrab[0]->m_bAutoDelete = FALSE;

		m_pThreadGrab[1] = AfxBeginThread(GrabThread1, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrab[1]->m_bAutoDelete = FALSE;

		m_pThreadGrabEndCheck[0] = AfxBeginThread(GrabEndCheckThread0, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrabEndCheck[0]->m_bAutoDelete = FALSE;
	}
	else
	{
		m_pThreadGrab[2] = AfxBeginThread(GrabThread2, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrab[2]->m_bAutoDelete = FALSE;

		m_pThreadGrab[3] = AfxBeginThread(GrabThread3, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrab[3]->m_bAutoDelete = FALSE;

		m_pThreadGrabEndCheck[1] = AfxBeginThread(GrabEndCheckThread1, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrabEndCheck[1]->m_bAutoDelete = FALSE;
	}

	strText.Format(_T("Ch No %d, InitInspection End"), nChannel + 1);
	m_cLogFile->WriteText(strText);
}

void CGapInspectionDlg::ReleaseThread(int nChannel)
{
	CString strText;
	strText.Format(_T("Ch No %d, ReleaseThread Start"), nChannel + 1);
	m_cLogFile->WriteText(strText);
	if(nChannel == 0)
	{
		m_bGrabThreadStart[0] = FALSE;
		m_bGrabThreadEnd[0] = TRUE;
		m_bGrabThreadStart[1] = FALSE;
		m_bGrabThreadEnd[1] = TRUE;
		m_bThreadGrabEndCheckEnd[0] = TRUE;
		WaitGrabThreadEndCheck(0);
		WaitGrabThreadEndCheck(1);
		WaitGrabEndCheckThreadEndCheck(0);
	}
	else
	{
		m_bGrabThreadStart[2] = FALSE;
		m_bGrabThreadEnd[2] = TRUE;
		m_bGrabThreadStart[3] = FALSE;
		m_bGrabThreadEnd[3] = TRUE;
		m_bThreadGrabEndCheckEnd[1] = TRUE;
		WaitGrabThreadEndCheck(2);
		WaitGrabThreadEndCheck(3);
		WaitGrabEndCheckThreadEndCheck(1);
	}
	strText.Format(_T("Ch No %d, ReleaseThread End"), nChannel + 1);
	m_cLogFile->WriteText(strText);
}

BOOL CGapInspectionDlg::WaitGrabThreadEndCheck(int nCamIdx)
{
	BOOL rslt = TRUE;

	if(m_pThreadGrab[nCamIdx] != NULL)
	{
		if(!m_bGrabThreadStart[nCamIdx] && m_bGrabThreadEnd[nCamIdx])
		{
			DWORD dwRet = WaitForSingleObject(m_pThreadGrab[nCamIdx]->m_hThread, DEFAULT_TIMEOUT * 2);

			if(dwRet == WAIT_FAILED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_ABANDONED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_TIMEOUT)
			{
				rslt = FALSE;
			}
		}
	}

	if(rslt)
	{
		if(!m_bGrabThreadStart[nCamIdx] && m_bGrabThreadEnd[nCamIdx])
		{
			if(m_pThreadGrab[nCamIdx])
				delete m_pThreadGrab[nCamIdx];

			m_pThreadGrab[nCamIdx] = NULL;
		}
	}

	return rslt;
}

BOOL CGapInspectionDlg::WaitGrabEndCheckThreadEndCheck(int nChannel)
{
	BOOL rslt = TRUE;
	
	if(m_pThreadGrabEndCheck[nChannel] != NULL)
	{
		if(!m_bThreadGrabEndCheckEnd[nChannel])
		{
			DWORD dwRet = WaitForSingleObject(m_pThreadGrabEndCheck[nChannel]->m_hThread, DEFAULT_TIMEOUT * 2);

			if(dwRet == WAIT_FAILED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_ABANDONED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_TIMEOUT)
			{
				rslt = FALSE;
			}
		}
	}

	if(rslt)
	{
		if(!m_bThreadGrabEndCheckEnd[nChannel])
		{
			if(m_pThreadGrabEndCheck[nChannel])
				delete m_pThreadGrabEndCheck[nChannel];

			m_pThreadGrabEndCheck[nChannel] = NULL;
		}
	}

	return rslt;
}

void CGapInspectionDlg::InitRsltPointDisplay(int nChannel)
{
	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_dlgInspPoint[nChannel]->SetInspRsltPointView(i);
		m_MainDisplay[nChannel][i].SetImage(m_DispTempImage);
		m_ctrInspPointLabel[nChannel][i].SetBackColor(RGB(150, 150, 150));;
	}
}

BOOL CGapInspectionDlg::SetSendPacket(int nChannel, int nSeqStepIdx, BOOL isError)
{
	BOOL rslt = FALSE;
	CString strPacket, strTemp, strText, strError;
	strError.Format(_T("A"));
	int nSeqIdx = 0;
	switch(nSeqStepIdx)
	{
	case INIT_SEQ:
		nSeqIdx = 20;
		break;
	case 1://GRAB_SEQ:
	case 2:
	case 3:
		nSeqIdx = nSeqStepIdx;
		break;
	case END_SEQ:
		if(m_bInspectionRslt[nChannel])
		{
			nSeqIdx = 4;
		}
		else
		{
			nSeqIdx = 5;
		}
		break;
	case READY:
		nSeqIdx = 10;
		break;
	}
	if(isError)
	{
		nSeqIdx = 6;
	}

	WritePlcVal(nChannel, nSeqIdx);

	return rslt;
}

BOOL CGapInspectionDlg::GetRecivePacket(int nChannel, CString strRcvPacket)
{
	BOOL rslt = TRUE;
	CString strCommand, strStepIndex;
	int nStepIndex;

	if(m_bAutoInsp[nChannel])
	{
		strCommand.Format(_T("%s"), strRcvPacket.Mid(0, 1));

		if(!strCommand.Compare(_T("S")))
		{
			m_nSeqStepIndex[nChannel] = INIT_SEQ;
			m_cStart[nChannel] = CTime::GetCurrentTime();
			m_lStartTime[nChannel] = GetCurrentTime();
			if(strRcvPacket.GetLength() > 10)
				m_strBarcode[nChannel].Format(_T("%s"), strRcvPacket.Mid(2, strRcvPacket.GetLength() - 2));
			else
				m_strBarcode[nChannel].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[nChannel].GetYear(), m_cStart[nChannel].GetMonth(), m_cStart[nChannel].GetDay(), m_cStart[nChannel].GetHour(), m_cStart[nChannel].GetMinute(), m_cStart[nChannel].GetSecond());

			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = TRUE;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = FALSE;
		}
		else if(!strCommand.Compare(_T("G")))
		{
			strStepIndex.Format(_T("%s"), strRcvPacket.Mid(2, 1));
			nStepIndex = _wtoi(strStepIndex);
			m_nSeqStepIndex[nChannel] = GRAB_SEQ + nStepIndex;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = TRUE;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = FALSE;
		}
		else if(!strCommand.Compare(_T("E")))
		{
			m_nSeqStepIndex[nChannel] = END_SEQ;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = TRUE;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = FALSE;
		}
		else
		{
			rslt = FALSE;
		}

		if(rslt)
		{
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = TRUE;
			m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = FALSE;
		}
	}
	else
	{
		CString strText;
		strText.Format(_T("Ch No %d, Not Auto State !!!"), nChannel + 1);
		m_cLogFile->WriteText(strText);
	}

	return rslt;
}

void CGapInspectionDlg::SetInitSeqStepPara(int nChannel)
{
	TypeSeqStepPara SeqStepPara;
	m_nSeqStepIndex[nChannel] = -1;
	if(m_SeqStepPara[nChannel].size() > 0)
	{
		m_SeqStepPara[nChannel].clear();
	}

	m_SeqPara[nChannel] = m_dlgTeach[nChannel]->GetSeqPara();
	for(int i = 0; i < m_SeqPara[nChannel].InspectionPara.size() + 2; i++)
	{
		SeqStepPara.bStepStart = FALSE;
		SeqStepPara.bStepEnd = FALSE;

		m_SeqStepPara[nChannel].push_back(SeqStepPara);
	}
	m_bStopInitSeq[nChannel] = FALSE;
	int nCamIdx[MAX_CHANNEL_CNT];

	if(nChannel == 0)
	{
		nCamIdx[0] = 0;
		nCamIdx[1] = 1;
		//KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0);
	}
	else
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
		//KillTimer(MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1);
	}

	m_nGrabIdx[nCamIdx[0]] = 0;
	m_nGrabIdx[nCamIdx[1]] = 0;

	m_bInspEndCheckThreadStart[nChannel] = FALSE;
	m_bInspEndCheckThreadEnd[nChannel] = TRUE;

	WaitInspEndCheckThread(nChannel);
}

BOOL CGapInspectionDlg::SetReleaseGrabThread(int nChannel)
{
	BOOL rslt[3];
	int nCamIdx[2];

	if(nChannel == 0)
	{
		nCamIdx[0] = 0;
		nCamIdx[1] = 1;
	}
	else
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
	}

	rslt[0] = WaitGrabThreadEndCheck(nCamIdx[0]);
	rslt[1] = WaitGrabThreadEndCheck(nCamIdx[1]);
	rslt[2] = WaitGrabEndCheckThreadEndCheck(nChannel);

	if(!rslt[0] || !rslt[1] || !rslt[2])
	{
		return TRUE;
	}

	return TRUE;
}

void CGapInspectionDlg::SetSeqGrab(int nChannel, int nSeqIdx, int nGrabIdx)
{
	CString strText;
	if(nSeqIdx == 0)
	{
		InitRsltPointDisplay(nChannel);

		m_ctrMainRslt[nChannel].SetBackColor(RGB(150, 150, 150));
		m_ctrMainRslt[nChannel].SetWindowTextW(_T("-"));

		m_dlgInspRslt[nChannel]->InitRslt();
		m_dlgInspPoint[nChannel]->InitInspPoint();

		m_cGrabStart[nChannel] = CTime::GetCurrentTime();
		m_lGrabStart[nChannel] = GetCurrentTime();

#ifndef PLC_USE
		if(!m_SystemPara.bBarcodeUse)
		{
			m_strBarcode[nChannel].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cGrabStart[nChannel].GetYear(), m_cGrabStart[nChannel].GetMonth(), m_cGrabStart[nChannel].GetDay(), m_cGrabStart[nChannel].GetHour(), m_cGrabStart[nChannel].GetMinute(), m_cStart[nChannel].GetSecond());
			SetBarcode(nChannel, m_strBarcode[nChannel]);
		}
#endif
	}
	int nCamIdx[2];
	if(nChannel == 0)
	{
		nCamIdx[0] = 0;
		nCamIdx[1] = 1;
	}
	else
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
	}
	//Sleep(5000);
	/*m_bGrabThreadEnd[nCamIdx[0]] = TRUE;
	m_bGrabThreadEnd[nCamIdx[1]] = TRUE;*/

	m_nGrabIdx[nCamIdx[0]] = nGrabIdx;
	m_nGrabIdx[nCamIdx[1]] = nGrabIdx;

	SetSeqLightOnOff(nChannel, nSeqIdx, 0, 0);
	strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light On"), nChannel + 1, nSeqIdx + 1, nCamIdx[0] + 1, nGrabIdx + 1);
	m_cLogFile->WriteText(strText);

	SetSeqLightOnOff(nChannel, nSeqIdx, 1, 0);
	strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light On"), nChannel + 1, nSeqIdx + 1, nCamIdx[1] + 1, nGrabIdx + 1);
	m_cLogFile->WriteText(strText);

	//m_bGrabThreadStart[nCamIdx[0]] = TRUE;
	//m_bGrabThreadStart[nCamIdx[1]] = TRUE;

	m_bThreadGrabEndCheckEnd[nChannel] = FALSE;

	m_nSeqIndex[nChannel] = nSeqIdx;

	for(int i = 0; i < MAX_CAM_CNT / 2; i++)
	{
		m_bSeqGrabEnd[nCamIdx[i]] = FALSE;
		m_bGrabThreadStart[nCamIdx[i]] = TRUE;
		m_bGrabThreadEnd[nCamIdx[i]] = FALSE;
	}
}

int CGapInspectionDlg::GetGrabCnt(int nChannel, int nCamIdx)
{
	if(nChannel == 1)
	{
		nCamIdx += 2;
	}
	return m_nGrabIdx[nCamIdx];
}

void CGapInspectionDlg::SetSeqGrabStart(int nChannel, int nSeqIdx, int nGrabIdx)
{
	if(nSeqIdx == 0)
	{
		m_cGrabStart[nChannel] = CTime::GetCurrentTime();
		m_lGrabStart[nChannel] = GetCurrentTime();
	}

	int nCamIdx[2], nCameraIdx = 0;
	nCamIdx[0] = 0;
	nCamIdx[1] = 1;
	if(nChannel == 1)
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
	}

	SetSeqLightOnOff(nChannel, nSeqIdx, nCamIdx[0], nGrabIdx);
	SetSeqLightOnOff(nChannel, nSeqIdx, nCamIdx[1], nGrabIdx);

	if(m_SeqPara[nChannel].InspectionPara[g_SeqModule[nChannel].GetGrabStepIndex()].nInspPoint[0] != INSP_SKIP)
	{
		GrabSingle(nChannel, 0, nGrabIdx);
	}
	if(m_SeqPara[nChannel].InspectionPara[g_SeqModule[nChannel].GetGrabStepIndex()].nInspPoint[1] != INSP_SKIP)
	{
		GrabSingle(nChannel, 1, nGrabIdx);
	}

	SetSeqLightOnOff(nChannel, nSeqIdx, nCamIdx[0], nGrabIdx, FALSE);
	SetSeqLightOnOff(nChannel, nSeqIdx, nCamIdx[1], nGrabIdx, FALSE);

	for(int i = 0; i < MAX_CAM_CNT / 2; i++)
	{
		cvCvtColor(m_Vision[nChannel]->GetGrabImage(i, nGrabIdx), m_Vision[nChannel]->GetDispImage(i), CV_GRAY2BGR);
		m_MainDisplay[nChannel][m_SeqPara[nChannel].InspectionPara[g_SeqModule[nChannel].GetGrabStepIndex()].nInspPoint[i] - 1].SetImage(m_Vision[nChannel]->GetDispImage(i));	
	}

	SetSeqStepStart(nChannel, FALSE);
	SetSeqStepEnd(nChannel, TRUE);
}

void CGapInspectionDlg::ThreadGrab(int nCamIdx)
{
	//int nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
	BOOL bGrabRslt = FALSE;
	CString strText;
	int nChannel = 0, nCameraIdx = 0, nSeqIdx = 0;
	int nMessageIndex = USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_0;
	if(nCamIdx > 1)
	{
		nChannel = 1;
		nMessageIndex = USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_1;
	}

	if(nCamIdx == 1 || nCamIdx == 3)
	{
		nCameraIdx = 1;
	}

	while(TRUE)
	{
		if(m_bGrabThreadStart[nCamIdx])
		{
			m_bGrabThreadStart[nCamIdx] = FALSE;
			m_bGrabThreadEnd[nCamIdx] = FALSE;
			m_bSeqGrabEnd[nCamIdx] = FALSE;

			nSeqIdx = m_nSeqIndex[nChannel];
			if(nSeqIdx > 3 || nSeqIdx < 0)
			{
				continue;
			}

			if(m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] != INSP_SKIP)
			{
				bGrabRslt = GrabSingle(nChannel, nCameraIdx, 0);
				//cvCopy(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 0), m_SaveGrabImage[nCamIdx][0][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1]);
				m_nGrabIdx[nCamIdx]++;
				SetSeqLightOnOff(nChannel, nSeqIdx, nCameraIdx, 0, FALSE);
				SetSeqLightOnOff(nChannel, nSeqIdx, nCameraIdx, 1);
				strText.Format(_T("Ch No %d, SeqIndex %d, CamIndex %d, GrabIndex %d, Light On"), nChannel + 1, nSeqIdx + 1, nCamIdx + 1, 2);
				m_cLogFile->WriteText(strText);

				//SendMessage(nMessageIndex, nCameraIdx, nSeqIdx);
					
				cvCvtColor(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 0), m_Vision[nChannel]->GetDispImage(nCameraIdx), CV_GRAY2BGR);
				m_MainDisplay[nChannel][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1].SetImage(m_Vision[nChannel]->GetDispImage(nCameraIdx));	

				bGrabRslt = GrabSingle(nChannel, nCameraIdx, 1);
				//cvCopy(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 1), m_SaveGrabImage[nCamIdx][1][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1]);
				m_nGrabIdx[nCamIdx]++;
				SetSeqLightOnOff(nChannel, nSeqIdx, nCameraIdx, 1, FALSE);
				//SendMessage(nMessageIndex, nCameraIdx, nSeqIdx);
				cvCvtColor(m_Vision[nChannel]->GetGrabImage(nCameraIdx, 1), m_Vision[nChannel]->GetDispImage(nCameraIdx), CV_GRAY2BGR);
				m_MainDisplay[nChannel][m_SeqPara[nChannel].InspectionPara[nSeqIdx].nInspPoint[nCameraIdx] - 1].SetImage(m_Vision[nChannel]->GetDispImage(nCameraIdx));	

				m_bSeqGrabEnd[nCamIdx] = TRUE;
				//m_bGrabThreadEnd[nCamIdx] = TRUE;
			}
			else
			{
				m_bSeqGrabEnd[nCamIdx] = TRUE;
				//m_bGrabThreadEnd[nCamIdx] = TRUE;
			}
		}

		if(m_bGrabThreadEnd[nCamIdx])
		{
			break;
		}

		Sleep(1);
	}
}

#ifdef SERIAL_BARCODE
void CGapInspectionDlg::ReadBarcodeThread(int nChannel)
{
	while(TRUE)
	{
		//Sleep(100);
		::Delay(100);

		if(m_bThreadBarcodeStart[nChannel])
		{
			int nReadLenght = !nChannel;
			CString strBarcode;
			strBarcode.Format(_T("%s"), GetBarcodeReader(nChannel));
			
			if(strBarcode.GetLength() >= 15)// && !m_bBarcodeInputEnd[nChannel])
			{
				m_strBarcode[nChannel].Format(_T("%s"), strBarcode);
				
				if(!m_strBarcode[nChannel].Compare(m_strBarcode[!nChannel]))
				{
					SetBarcode(!nChannel, _T("-"));
					m_bBarcodeInputEnd[!nChannel] = FALSE;
				}
		
				SetBarcode(nChannel, m_strBarcode[nChannel]);
				m_bBarcodeInputEnd[nChannel] = TRUE;
			}
		}

		if(m_bThreadBarcodeEnd[nChannel])
		{
			m_bThreadBarcodeStart[nChannel] = FALSE;
			m_bThreadBarcodeEnd[nChannel] = FALSE;
			break;
		}
	}
}

UINT CGapInspectionDlg::BarcodeReadThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ReadBarcodeThread(0);

	return 0;
}

UINT CGapInspectionDlg::BarcodeReadThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ReadBarcodeThread(1);

	return 0;
}
#endif

UINT CGapInspectionDlg::GrabThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrab(0);

	return 0;
}

UINT CGapInspectionDlg::GrabThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrab(1);

	return 0;
}

UINT CGapInspectionDlg::GrabThread2(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrab(2);

	return 0;
}

UINT CGapInspectionDlg::GrabThread3(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrab(3);

	return 0;
}

void CGapInspectionDlg::ThreadGrabEndCheck(int nChannel)
{
	int nCamIdx[2];
	if(nChannel == 0)
	{
		nCamIdx[0] = 0;
		nCamIdx[1] = 1;
	}
	else
	{
		nCamIdx[0] = 2;
		nCamIdx[1] = 3;
	}

	while(TRUE)
	{
		if(m_bSeqGrabEnd[nCamIdx[0]] && m_bSeqGrabEnd[nCamIdx[1]])
		{
			SetSeqStepStart(nChannel, FALSE);
			SetSeqStepEnd(nChannel, TRUE);
			m_bSeqGrabEnd[nCamIdx[0]] = FALSE;
			m_bSeqGrabEnd[nCamIdx[1]] = FALSE;
			//m_bThreadGrabEndCheckEnd[nChannel] = TRUE;
		}

		if(m_bThreadGrabEndCheckEnd[nChannel])
			break;

		Sleep(1);
	}
	
}

UINT CGapInspectionDlg::GrabEndCheckThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrabEndCheck(0);

	return 0;
}

UINT CGapInspectionDlg::GrabEndCheckThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadGrabEndCheck(1);

	return 0;
}

BOOL CGapInspectionDlg::SetSeqLightOnOff(int nChannel, int nSeqIdx, int nCamIdx, int nGrabIdx, BOOL isOn)
{
	BOOL rslt = TRUE;
	CString strText, strText2;
	int nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
	m_LightCritical.Lock();
	if(isOn)
		strText2.Format(_T("light on"));
	else
		strText2.Format(_T("light off"));

	strText.Format(_T("ch %d, seq %d, cam %d, grab %d, %s, critical lock"), nChannel + 1, nSeqIdx + 1, nCamIdx + 1, nGrabIdx + 1, strText2);
#ifdef LIGHT_TEST
	this->m_cTestLogFile->WriteText(strText);
#else
	m_cLogFile->WriteText(strText);
#endif
	//light on
	if(isOn)
	{
		if(nCamIdx == BOTTOM_CAMERA)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
			if(nChannel == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL + 1);
			}
		}
		else
		{
			nLightStartCh = BOT_CAMERA_LIGHT_START_IDX;
			if(nChannel == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL);
			}
		}

#ifndef LIGHT_TEST
		for(int i = 0; i < MAX_LIGHT_CNT; i++)
		{
			if(m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
			{
				if(!SetLightOnof(nChannel, nLightStartCh + i, m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i]))
				{
					strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
					m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
					m_cTestLogFile->WriteText(strText);
#endif
					return FALSE;
				}
			}
		}
#else
		for(int i = 0; i < MAX_LIGHT_CNT; i++)
		{
			if(m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
			{
				if(!SetLightOnof(0, nCamIdx, m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i]))
				{
					strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
					m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
					m_cTestLogFile->WriteText(strText);
#endif
					return FALSE;
				}
			}
		}
		

#endif
	}
	else
	{
		if(nCamIdx == BOTTOM_CAMERA)
		{
			nLightStartCh = TOP_CAMERA_LIGHT_START_IDX;
			if(nChannel == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL + 1);
			}
		}
		else
		{
			nLightStartCh = BOT_CAMERA_LIGHT_START_IDX;
			if(nChannel == NO_2_CHANNEL)
			{
				nLightStartCh = TOP_CAMERA_LIGHT_START_IDX * (BOTTOM_CAMERA + NO_2_CHANNEL);
			}
		}
#ifndef LIGHT_TEST
		for(int i = 0; i < MAX_LIGHT_CNT; i++)
		{
			if(m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
			{
				if(!SetLightOnof(nChannel, nLightStartCh + i, 0))
				{
					strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
					m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
					m_cTestLogFile->WriteText(strText);
#endif
					return FALSE;
				}
			}
		}
#else
		for(int i = 0; i < MAX_LIGHT_CNT; i++)
		{
			if(m_SeqLightPara[nChannel][nSeqIdx].nLightVal[nCamIdx][nGrabIdx][i] > 0)
			{
				if(!SetLightOnof(0, nCamIdx))
				{
					strText.Format(_T("SetGrab Light ch %d Error"), nLightStartCh + i);
					m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
					m_cTestLogFile->WriteText(strText);
#endif
					return FALSE;
				}
			}
		}

#endif
	}
	strText.Format(_T("ch %d, seq %d, cam %d, grab %d, %s, critical unlock"), nChannel + 1, nSeqIdx + 1, nCamIdx + 1, nGrabIdx + 1, strText2);
#ifdef LIGHT_TEST
	this->m_cTestLogFile->WriteText(strText);
#else
	m_cLogFile->WriteText(strText);
#endif
	m_LightCritical.Unlock();
	return rslt;
}

BOOL CGapInspectionDlg::GetSeqStepStart(int nChannel)
{
	if(m_nSeqStepIndex[nChannel] == -1)
		return FALSE;

	return m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart;
}

BOOL CGapInspectionDlg::GetSeqStepEnd(int nChannel)
{
	if(m_nSeqStepIndex[nChannel] == -1)
		return FALSE;

	return m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd;
}

int CGapInspectionDlg::GetReadPlcVal(int nChannel)
{
#ifdef PLC_USE
	m_RWPlcCritical.Lock();
	int nRslt = ReadPlcVal(nChannel, 0);
	m_RWPlcCritical.Unlock();
	return nRslt;
#else
	return -1;
#endif
}

int CGapInspectionDlg::ReadPlcVal(int nChannel, int nSeq)
{
	int nTimerVal = 0;
	if(nChannel == 1)
	{
		nTimerVal = 10;
	}

	m_bReadPlcCheck[nChannel] = FALSE;
	BYTE data[256];
	int i = 0;
	data[i++] = 0x50;
	data[i++] = 0x00;

	data[i++] = 0x00;
	data[i++] = 0xFF;

	data[i++] = 0xFF;
	data[i++] = 0x03;
	data[i++] = 0x00;

	data[i++] = 0x0C;
	data[i++] = 0x00;

	data[i++] = 0x10;
	data[i++] = 0x00;

	data[i++] = 0x01;//command2 read:01, write:01
	data[i++] = 0x04;//command1 read:04, write:14
	
	data[i++] = 0x00;
	data[i++] = 0x00;
	
	if(nChannel == 0)
	{
		data[i++] = 0x24;//address2
		data[i++] = 0x13;//address1
	}
	else
	{
		data[i++] = 0x56;//address2
		data[i++] = 0x13;//address1
	}
	data[i++] = 0x00;
	data[i++] = 0xA8;
	
	data[i++] = 0x01;//read, write lenght
	data[i++] = 0x00;

#ifdef PLC_USE
	int a = m_Client->Send(data, i);

	Sleep(PLC_RECIVE_WAIT_TIME);
	//::Delay(PLC_RECIVE_WAIT_TIME);

	memset(data, NULL, 256);
	a = m_Client->Receive(data, 200);

	CString strText, strPacket, strTemp;

	strPacket.Format(_T("%02X"), data[0]);
	for(int j = 1; j < i - 1; j++) 
	{
		strTemp.Format(_T("%s%02X"), strPacket, data[j]);
		strPacket.Format(_T("%s"), strTemp);
	}

	//m_nSeqStepIndex[nChannel] = -1;
	//data[7] = 0x00;
	//data[11] = 0x14;
	
	if(data[7] != 0x0b && m_bAutoState[nChannel])
	{
		//20200114 ngh
		if(data[11] == 0x14 && m_nSeqStepIndex[nChannel] != INIT_SEQ)
		{
			if(m_SystemPara.bBarcodeUse)
			{
				if(m_strBarcode[nChannel].GetLength() >= BARCODE_MAX_STRING_CNT)
				{
					m_nSeqStepIndex[nChannel] = INIT_SEQ;
					m_cStart[nChannel] = CTime::GetCurrentTime();
					m_lStartTime[nChannel] = GetCurrentTime();
					//m_strBarcode[nChannel].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[nChannel].GetYear(), m_cStart[nChannel].GetMonth(), m_cStart[nChannel].GetDay(), m_cStart[nChannel].GetHour(), m_cStart[nChannel].GetMinute(), m_cStart[nChannel].GetSecond());

					m_SeqStepPara[nChannel][INIT_SEQ].bStepStart = TRUE;
					m_SeqStepPara[nChannel][INIT_SEQ].bStepEnd = FALSE;
					m_bReadPlcVal[nChannel] = TRUE;
					strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d, Init Start -> %s"), nChannel + 1, 20, strPacket);
					m_cLogFile->WriteText(strText);
					//m_dlgBarcode[nChannel]->ShowWindow(SW_HIDE);
				}
			}
			else
			{
				m_nSeqStepIndex[nChannel] = INIT_SEQ;
				m_cStart[nChannel] = CTime::GetCurrentTime();
				m_lStartTime[nChannel] = GetCurrentTime();
				m_strBarcode[nChannel].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[nChannel].GetYear(), m_cStart[nChannel].GetMonth(), m_cStart[nChannel].GetDay(), m_cStart[nChannel].GetHour(), m_cStart[nChannel].GetMinute(), m_cStart[nChannel].GetSecond());

				m_SeqStepPara[nChannel][INIT_SEQ].bStepStart = TRUE;
				m_SeqStepPara[nChannel][INIT_SEQ].bStepEnd = FALSE;
				m_bReadPlcVal[nChannel] = TRUE;
				strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d, Init Start -> %s"), nChannel + 1, 20, strPacket);
				m_cLogFile->WriteText(strText);
				//m_dlgBarcode[nChannel]->ShowWindow(SW_HIDE);
			}
		}
		else if(data[11] == 0x01 && m_nSeqStepIndex[nChannel] == INIT_SEQ && !m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart && m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd)
		{
			m_pBarcode[nChannel]->SetBarcodeReadStart(FALSE);
			//m_ctrBarcodeReset[nChannel].EnableWindow(FALSE);
			m_nSeqStepIndex[nChannel] = GRAB_SEQ + 0;
			m_SeqStepPara[nChannel][GRAB_SEQ + 0].bStepStart = TRUE;
			m_SeqStepPara[nChannel][GRAB_SEQ + 0].bStepEnd = FALSE;
			m_bReadPlcVal[nChannel] = TRUE;
			strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d, Step 1 Start-> %s"), nChannel + 1, 1, strPacket);
			m_cLogFile->WriteText(strText);
		}
		else if(data[11] == 0x02 && m_nSeqStepIndex[nChannel] == GRAB_SEQ && !m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart && m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd)
		{
			m_nSeqStepIndex[nChannel] = GRAB_SEQ + 1;
			m_SeqStepPara[nChannel][GRAB_SEQ + 1].bStepStart = TRUE;
			m_SeqStepPara[nChannel][GRAB_SEQ + 1].bStepEnd = FALSE;
			m_bReadPlcVal[nChannel] = TRUE;
			strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d, Step 2 Start -> %s"), nChannel + 1, 2, strPacket);
			m_cLogFile->WriteText(strText);
		}
		else if(data[11] == 0x03 && m_nSeqStepIndex[nChannel] == GRAB_SEQ + 1 && !m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart && m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd)
		{
			m_nSeqStepIndex[nChannel] = GRAB_SEQ + 2;
			m_SeqStepPara[nChannel][GRAB_SEQ + 2].bStepStart = TRUE;
			m_SeqStepPara[nChannel][GRAB_SEQ + 2].bStepEnd = FALSE;
			m_bReadPlcVal[nChannel] = TRUE;
			strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d, Step 3 Start -> %s"), nChannel + 1, 3, strPacket);
			m_cLogFile->WriteText(strText);
		}
		else if(data[11] == 0xE8 && data[12] == 0x03)//error
		{
			m_nSeqStepIndex[nChannel] = SEQ_ERROR;
			m_bReadPlcVal[nChannel] = TRUE;
			strText.Format(_T("Read PLC Value, Ch No %d, Seq Idx %d -> %s"), nChannel + 1, 100, strPacket);
			m_cLogFile->WriteText(strText);
		}
	}
	else
	{
		return -1;
	}
#else
	if(m_nSeqStepIndex[nChannel] > 0)
	{
		m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepStart = TRUE;
		m_SeqStepPara[nChannel][m_nSeqStepIndex[nChannel]].bStepEnd = FALSE;
	}
#endif
	
	return m_nSeqStepIndex[nChannel];
}

int CGapInspectionDlg::SetAutoStateWrite(int nChannel, BOOL isOnOff)
{
	BYTE data[256];
	memset(data, NULL, 256);
	int i = 0, b = 1;
	data[i++] = 0x50;
	data[i++] = 0x00;

	data[i++] = 0x00;

	data[i++] = 0xFF;

	data[i++] = 0xFF;
	data[i++] = 0x03;

	data[i++] = 0x00;

	data[i++] = 0x0E;
	data[i++] = 0x00;

	data[i++] = 0x10;
	data[i++] = 0x00;

	data[i++] = 0x01;//command2 read:01, write:01
	data[i++] = 0x14;//command1 read:04, write:14
	
	data[i++] = 0x00;//sub command2
	data[i++] = 0x00;//sub command1
	
	if(nChannel == 0)
	{
		data[i++] = 0xC1;//address2
		data[i++] = 0x12;//address1
	}
	else
	{
		data[i++] = 0xF3;//address2
		data[i++] = 0x12;//address1
	}
	data[i++] = 0x00;
	data[i++] = 0xA8;
	
	data[i++] = 0x01;//read, write lenght
	data[i++] = 0x00;

	if(isOnOff)
	{
		data[i++] = 0x03;
		data[i++] = 0x00;
	}
	else
	{
		data[i++] = 0x01;
		data[i++] = 0x00;
	}

	m_bAutoState[nChannel] = !isOnOff;

	CString strText, strPacket, strTemp;

	strPacket.Format(_T("%02X"), data[0]);
	for(int j = 1; j < i - 1; j++) 
	{
		strTemp.Format(_T("%s%02X"), strPacket, data[j]);
		strPacket.Format(_T("%s"), strTemp);
	}

#ifdef PLC_USE
	int a = m_Client->Send(data, i);
	Sleep(PLC_RECIVE_WAIT_TIME);
	//::Delay(PLC_RECIVE_WAIT_TIME);
	memset(data, NULL, 256);
	a = m_Client->Receive(data, 200);

	if(data[7] == 0x0b)
	{
		strText.Format(_T("Write Fail, Ch No %d -> %s"), nChannel + 1, strPacket);
		m_cLogFile->WriteText(strText);
		return 1;
	}
#endif
	return 0;
}
	
int CGapInspectionDlg::WritePlcVal(int nChannel, int nSeq)
{
	//m_RWPlcCritical.Lock();
	m_WritePlcCritical.Lock();
	if(nSeq == 1000)
	{
		SetAutoStateWrite(nChannel, m_bAutoState[nChannel]);
	}
	else
	{
		CString strSeqText;
		BYTE data[256];
		memset(data, NULL, 256);
		int i = 0, b = 1, nCamIdx[2];
		data[i++] = 0x50;
		data[i++] = 0x00;

		data[i++] = 0x00;

		data[i++] = 0xFF;

		data[i++] = 0xFF;
		data[i++] = 0x03;

		data[i++] = 0x00;

		data[i++] = 0x0E;
		data[i++] = 0x00;

		data[i++] = 0x10;
		data[i++] = 0x00;

		data[i++] = 0x01;//command2 read:01, write:01
		data[i++] = 0x14;//command1 read:04, write:14
	
		data[i++] = 0x00;//sub command2
		data[i++] = 0x00;//sub command1
	
		if(nChannel == 0)
		{
			if(nSeq == 2000)
			{
				data[i++] = 0xC1;//address2
				data[i++] = 0x12;//address1
			}
			else
			{
				data[i++] = 0xC0;//address2
				data[i++] = 0x12;//address1
			}
			nCamIdx[0] = 0;
			nCamIdx[1] = 1;
		}
		else
		{
			if(nSeq == 2000)
			{
				data[i++] = 0xF3;//address2
				data[i++] = 0x12;//address1
			}
			else
			{
				data[i++] = 0xF2;//address2
				data[i++] = 0x12;//address1
			}
			nCamIdx[0] = 2;
			nCamIdx[1] = 3;
		}
		data[i++] = 0x00;
		data[i++] = 0xA8;
	
		data[i++] = 0x01;//read, write lenght
		data[i++] = 0x00;

		if(nSeq == 2000)
		{
			if(m_bAutoInsp[nChannel])
			{
				data[i++] = 0x03;
				data[i++] = 0x00;
			}
			else
			{
				data[i++] = 0x00;
				data[i++] = 0x00;
			}
			strSeqText.Format(_T("Auto"));
		}
		else if(nSeq == 20)
		{
			data[i++] = 0x14;
			data[i++] = 0x00;
			strSeqText.Format(_T("Init Comp"));
		}
		else if(nSeq == 1)
		{
			data[i++] = 0x01;
			data[i++] = 0x00;
			strSeqText.Format(_T("Step 1 Comp"));
		}
		else if(nSeq == 2)
		{
			data[i++] = 0x02;
			data[i++] = 0x00;
			strSeqText.Format(_T("Step 2 Comp"));
		}
		else if(nSeq == 3)
		{
			data[i++] = 0x03;
			data[i++] = 0x00;
			strSeqText.Format(_T("Step 3 Comp"));
		}
		else if(nSeq == 4)//ok
		{
			data[i++] = 0x0A;
			data[i++] = 0x00;
			strSeqText.Format(_T("End OK Comp"));
		}
		else if(nSeq == 5)//ng
		{
			data[i++] = 0x0B;
			data[i++] = 0x00;
			strSeqText.Format(_T("End NG Comp"));
		}
		else if(nSeq == 6)//에러
		{
			data[i++] = 0xE8;
			data[i++] = 0x03;
		}
		else if(nSeq == 10)//Ready
		{
			data[i++] = 0x00;
			data[i++] = 0x00;
			strSeqText.Format(_T("Seq Ready"));
		}
		m_nGrabIdx[nCamIdx[0]] = 0;
		m_nGrabIdx[nCamIdx[1]] = 0;

		CString strText, strPacket, strTemp;

		strPacket.Format(_T("%02X"), data[0]);
		for(int j = 1; j < i - 1; j++) 
		{
			strTemp.Format(_T("%s%02X"), strPacket, data[j]);
			strPacket.Format(_T("%s"), strTemp);
		}

		BOOL isOK = TRUE;
#ifdef PLC_USE
		if(m_bClientConnect)
		{
			int a = m_Client->Send(data, i);

			Sleep(PLC_RECIVE_WAIT_TIME);
			//::Delay(PLC_RECIVE_WAIT_TIME);
			memset(data, NULL, 256);
			a = m_Client->Receive(data, 200);

			if(data[7] == 0x0b)
			{
				strText.Format(_T("Write Fail, Ch No %d, Seq Idx %d, %s -> %s"), nChannel + 1, nSeq, strSeqText, strPacket);
				m_cLogFile->WriteText(strText);
				isOK = FALSE;
				WritePlcVal(nChannel, nSeq);
			}
		}
		else
		{
			strText.Format(_T("Write Fail, Ch No %d, Seq Idx %d, %s -> %s"), nChannel + 1, nSeq, strSeqText, strPacket);
			m_cLogFile->WriteText(strText);
			isOK = FALSE;
		}
#endif

		if(isOK)
		{
			strText.Format(_T("Write OK, Ch No %d,  Seq Idx %d, %s -> %s"), nChannel + 1, nSeq, strSeqText, strPacket);
			m_cLogFile->WriteText(strText);
		}
	}
	//m_RWPlcCritical.Unlock();
	m_WritePlcCritical.Unlock();
	return 0;
}

void CGapInspectionDlg::ClickBtnenhctrlMainCountReset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
#ifdef TEST_RUN
	if(m_nTestCnt[0] == 0)
	{
		if(m_bAutoInsp[0])
		{
			for(int i = 0; i < 1; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
		}

		//SetTimer(10001, 1000, NULL);
		m_nTestCnt[0] = 1;
	}
	else
	{
		m_nTestCnt[0] = 0;
	}
	//ClickBtnenhctrlMainCountReset2();
#else
	CountReset(0);

	/*TypeStringBarcode a;

	sprintf_s((char*)a.byBarcode, 16, "ABCDEFGHIJKLMNO");
	a.nBarcodeLength = 15;
	m_vBarcode[0].push_back(a);

	sprintf_s((char*)a.byBarcode, 16, "PQRSTUVWXYZABCD");
	a.nBarcodeLength = 15;
	m_vBarcode[0].push_back(a);

	CString strText;
	strText.Format(_T("%S"), m_vBarcode[0][0].byBarcode);
	SetBarcode(0, strText);

	SetTimer(99999, 3000, NULL);*/
#endif
}


void CGapInspectionDlg::ClickBtnenhctrlMainCountReset2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
#ifdef TEST_RUN
	if(m_nTestCnt[1] == 0)
	{
		if(m_bAutoInsp[1])
		{
			for(int i = 1; i < MAX_CHANNEL_CNT; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
		}

		//SetTimer(10001, 1000, NULL);
		m_nTestCnt[1] = 1;
	}
	else
	{
		m_nTestCnt[1] = 0;
	}
#else
	CountReset(1);
#endif
}

void CGapInspectionDlg::InspEndCheck(int nChannel)
{
	m_bInspEndCheckThreadStart[nChannel] = TRUE;
	m_bInspEndCheckThreadEnd[nChannel] = FALSE;

	m_pThreadInspEndCheck[nChannel] = NULL;
	if(nChannel == 0)
	{
		m_pThreadInspEndCheck[nChannel] = AfxBeginThread(InspEndCheckThread0, this, THREAD_PRIORITY_NORMAL);
	}
	else
	{
		m_pThreadInspEndCheck[nChannel] = AfxBeginThread(InspEndCheckThread1, this, THREAD_PRIORITY_NORMAL);
	}

	if(m_pThreadInspEndCheck[nChannel] != NULL)
	{
		m_pThreadInspEndCheck[nChannel]->m_bAutoDelete = FALSE;
		m_pThreadInspEndCheck[nChannel]->ResumeThread();
	}
}

UINT CGapInspectionDlg::InspEndCheckThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadInspEndCheck(0);

	return 0;
}

UINT CGapInspectionDlg::InspEndCheckThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadInspEndCheck(1);

	return 0;
}

void CGapInspectionDlg::ThreadInspEndCheck(int nChannel)
{
	CTimerCheck TimeCheck;
	TypeSeqPara SeqPara;
	int nStepCamIdx = 0;
	BOOL isVerHorRslt = TRUE;
	BOOL *rslt, *InspEndView, allRslt;
	rslt = new BOOL[m_dlgTeach[nChannel]->GetMaxSeqCnt()];
	InspEndView = new BOOL[m_dlgTeach[nChannel]->GetMaxSeqCnt()];

	for(int i = 0; i < m_dlgTeach[nChannel]->GetMaxSeqCnt(); i++)
	{
		rslt[i] = FALSE;
		InspEndView[i] = FALSE;
	}

	m_bInspEndCheckThreadStart[nChannel] = TRUE;
	m_bInspEndCheckThreadEnd[nChannel] = FALSE;
	allRslt = TRUE;
	SeqPara = m_dlgTeach[nChannel]->GetSeqPara();

	TimeCheck.SetCheckTime(INSP_END_TIMECHECK);
	TimeCheck.StartTimer();
	while(TRUE)
	{
		if(m_bInspEndCheckThreadStart)
		{
			allRslt = TRUE;
			for(int i = 0; i < m_bInspectionEnd[nChannel].size(); i++)
			{
				if(!m_bInspectionEnd[nChannel][i])
				{
					allRslt = FALSE;
					break;
				}
			}

			if(allRslt)
			{
				m_bInspEndCheckThreadEnd[nChannel] = TRUE;
				break;
			}

			for(int i = 0; i < m_Vision[nChannel]->GetInspStepIdx() + 1; i++)
			{
				if(!rslt[i])
					rslt[i] = m_Vision[nChannel]->GetInspectionEnd(i);

				if(rslt[i] && !InspEndView[i])
				{
					m_bInspectionEnd[nChannel][i] = TRUE;
					InspEndView[i] = TRUE;

					//rslt Main View
					for(int j = 0; j < MAX_CAM_CNT / 2; j++)
					{
						if(SeqPara.InspectionPara[i].nInspPoint[j] != INSP_SKIP)
						{
							nStepCamIdx = SeqPara.InspectionPara[i].nInspPoint[j] - 1;
							//SendMessage(USE_MSG_MAIN_INSP_END_DISPLAY_DIALOG, nChannel, nStepCamIdx);
							if(m_Vision[nChannel]->GetRsltImage(nStepCamIdx) != NULL)
							{
								m_MainDisplay[nChannel][nStepCamIdx].SetImage(m_Vision[nChannel]->GetRsltImage(nStepCamIdx));
								EachRsltDisplay(nChannel, SeqPara.InspectionPara[i].nInspPoint[j] - 1);
								if(m_SystemPara.bRsltImageSave || m_SystemPara.bRsltNGImageSave)
								{
									cvCopy(m_Vision[nChannel]->GetRsltImage(nStepCamIdx), m_SaveImage[nChannel][nStepCamIdx]);
								}
							}
						}
					}
				}
			}
		}

		if(m_bInspEndCheckThreadEnd[nChannel])
		{
			break;
		}

		Sleep(1);
	}

	m_bInspEndCheckThreadStart[nChannel] = FALSE;
	m_bInspEndCheckThreadEnd[nChannel] = TRUE;

	delete rslt;
	delete InspEndView;
}

BOOL CGapInspectionDlg::WaitInspEndCheckThread(int nChannel)
{
	BOOL rslt = TRUE;

	if(m_pThreadInspEndCheck[nChannel] != NULL)
	{
		if(!m_bInspEndCheckThreadStart[nChannel] && m_bInspEndCheckThreadEnd[nChannel])
		{
			DWORD dwRet = WaitForSingleObject(m_pThreadInspEndCheck[nChannel]->m_hThread, DEFAULT_TIMEOUT * 5);

			if(dwRet == WAIT_FAILED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_ABANDONED)
			{
				rslt = FALSE;
			}
			else if(dwRet == WAIT_TIMEOUT)
			{
				rslt = FALSE;
			}
		}
	}

	if(rslt)
	{
		if(!m_bInspEndCheckThreadStart[nChannel] && m_bInspEndCheckThreadEnd[nChannel])
		{
			if(m_pThreadInspEndCheck[nChannel])
				delete m_pThreadInspEndCheck[nChannel];

			m_pThreadInspEndCheck[nChannel] = NULL;
		}
	}

	return rslt;
}

void CGapInspectionDlg::SetMainDisplay(int nChannel, int nInspIdx)
{
	int nCamIdx = 0;
	if((nInspIdx + 1) == INSP_POINT4 || (nInspIdx + 1) == INSP_POINT5 || (nInspIdx + 1) == INSP_POINT6)
	{
		nCamIdx = 1;
	}

	m_MainDisplay[nChannel][nInspIdx].SetImage(m_Vision[nChannel]->GetDispImage(nCamIdx));
	m_MainDisplay[nChannel][nInspIdx].Fit();
}

void CGapInspectionDlg::EachRsltDisplay(int nChannel, int nInspPoint)
{
	CString strText;
	strText.Format(_T("Ch No %d, %s Insp EachRsltDisplay Start"), nChannel + 1, InspPointToString(nInspPoint + 1));
	m_cLogFile->WriteText(strText);
	//TypeSystemPara SystemPara;
	//SystemPara = m_dlgTeach->GetSystemPara();

	double dRslt = 0.;
	BOOL bRslt = FALSE;
	
	//m_dlgInspPoint->SetInspRsltPointView(nInspPoint);
		
	m_Vision[nChannel]->GetRsltGapVal(nInspPoint, &bRslt, &dRslt);

	m_dlgInspRslt[nChannel]->SetInspRsltView(nInspPoint, &bRslt, &dRslt);
	bRslt = m_Vision[nChannel]->GetPointRslt(nInspPoint);
	m_bInspRslt[nChannel][nInspPoint] = bRslt;
	m_dInspRslt[nChannel][nInspPoint] = dRslt;
	m_bInspPointRslt[nChannel][nInspPoint] = m_Vision[nChannel]->GetPointRslt(nInspPoint);

	strText.Format(_T("Ch No %d, %s Insp EachRsltDisplay End"), nChannel + 1, InspPointToString(nInspPoint + 1));
	m_cLogFile->WriteText(strText);
}

void CGapInspectionDlg::SetInspRslt(int nChannel, BOOL isOK)
{
	if(isOK)
	{
		m_ctrMainRslt[nChannel].SetBackColor(RGB(0, 220, 0));
		m_ctrMainRslt[nChannel].SetWindowTextW(_T("OK"));
	}
	else
	{
		m_ctrMainRslt[nChannel].SetBackColor(RGB(0, 220, 0));
		m_ctrMainRslt[nChannel].SetWindowTextW(_T("NG"));
	}
}

void CGapInspectionDlg::InspEndRsltDisplay(int nChannel)
{
	CString strText;

	if(!m_bInspEndCheckThreadStart[nChannel] && m_bInspEndCheckThreadEnd[nChannel])
	{
		if(WaitInspEndCheckThread(nChannel))
		{
			int nTimerCnt = 0;
			if(nChannel > 0)
				nTimerCnt = 5;
			KillTimer(MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_0 + nTimerCnt);
			//m_Vision[nChannel]->ReleaseInspection();
		}
	}
}

void CGapInspectionDlg::ClickBtnenhctrlMainAvgRsltView()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgAvgRsltView[0]->IsWindowVisible())
		m_dlgAvgRsltView[0]->ShowWindow(SW_HIDE);
	else
	{
		TypeSeqPara SeqPara = m_dlgTeach[0]->GetSeqPara();
		m_dlgAvgRsltView[0]->SetSpec(SeqPara.Spec);
		m_dlgAvgRsltView[0]->SetSpec(SeqPara.SpecPoint, 1);
		m_dlgAvgRsltView[0]->SetSpread(SeqPara.Spread);
		m_dlgAvgRsltView[0]->SetSpread(SeqPara.SpreadPoint, 1);

		double dRatio[MAX_RSLT_POINT_CNT];
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			dRatio[i] = (double)m_iPointNGConunt[0][i] / (double)m_iCount[0] * 100.;
			if(m_iCount[0] == 0)
				dRatio[i] = 0.;
		}
		m_dlgAvgRsltView[0]->SetRatio(dRatio);

		CRect rect1, rect2, rect3;
		GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect1);
		GetDlgItem(IDC_BTNENHCTRL_INSP_POINT_4)->GetWindowRect(&rect2);
		m_dlgAvgRsltView[0]->GetWindowRect(&rect3);
		m_dlgAvgRsltView[0]->MoveWindow(rect1.left, rect2.top, rect3.Width(), rect3.Height());
		m_dlgAvgRsltView[0]->ShowWindow(SW_SHOW);
	}
}


void CGapInspectionDlg::ClickBtnenhctrlMainAvgRsltView2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_dlgAvgRsltView[1]->IsWindowVisible())
		m_dlgAvgRsltView[1]->ShowWindow(SW_HIDE);
	else
	{
		TypeSeqPara SeqPara = m_dlgTeach[1]->GetSeqPara();
		m_dlgAvgRsltView[1]->SetSpec(SeqPara.Spec);
		m_dlgAvgRsltView[1]->SetSpec(SeqPara.SpecPoint, 1);
		m_dlgAvgRsltView[1]->SetSpread(SeqPara.Spread);
		m_dlgAvgRsltView[1]->SetSpread(SeqPara.SpreadPoint, 1);

		double dRatio[MAX_RSLT_POINT_CNT];
		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			dRatio[i] = (double)m_iPointNGConunt[1][i] / (double)m_iCount[1] * 100.;
			if(m_iCount[1] == 0)
				dRatio[i] = 0.;
		}
		m_dlgAvgRsltView[1]->SetRatio(dRatio);

		CRect rect1, rect2, rect3;
		GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);
		GetDlgItem(IDC_BTNENHCTRL_INSP_POINT_10)->GetWindowRect(&rect2);
		m_dlgAvgRsltView[1]->GetWindowRect(&rect3);
		m_dlgAvgRsltView[1]->MoveWindow(rect1.left, rect2.top, rect3.Width(), rect3.Height());
		m_dlgAvgRsltView[1]->ShowWindow(SW_SHOW);
	}
}

BOOL CGapInspectionDlg::RsltDisplay(int nChannel)
{
	BOOL rslt = TRUE;
	CString strText, strVal1, strVal2, strSavePath, strSaveFileName;

	strText.Format(_T("Ch No %d, RsltDisplay Start"), nChannel + 1);
	m_cLogFile->WriteText(strText);

	TypeSpecPara Spec[MAX_RSLT_POINT_CNT];
	BOOL bRslt, bRslt2;
	BOOL bPointRslt[MAX_RSLT_POINT_CNT] = {FALSE,};
	double dRslt[MAX_RSLT_POINT_CNT] = {0.,};

	int nRsltPoint[2] = {0,};
	int nRsltDirection[MAX_RSLT_POINT_CNT] = {0,};
	BOOL bAllRslt = TRUE;

	TypeSeqPara SeqPara;
	TypeSystemPara SystemPara;
	TypeAvgRslt PointRslt;

	SeqPara = m_dlgTeach[nChannel]->GetSeqPara();
	SystemPara = m_dlgSystem->GetSystemPara();
	m_dlgTeach[nChannel]->GetSpec(Spec);

	m_dlgInspRslt[nChannel]->SetPointRslt(m_bInspPointRslt[nChannel]);

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		dRslt[i] = m_dlgInspRslt[nChannel]->GetRslt(i, &bRslt, &bRslt2, &nRsltDirection[i]);
		
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

		PointRslt.dGapRslt[i] = dRslt[i];
		bPointRslt[i] = bRslt;

		if(bRslt)
		{
			m_dlgInspPoint[nChannel]->SetInspRsltPointView(nRsltPoint[0], TRUE);
			m_dlgInspPoint[nChannel]->SetInspRsltPointView(nRsltPoint[1], TRUE);

			m_ctrInspPointLabel[nChannel][nRsltPoint[0]].SetBackColor(RGB(0, 255, 0));
			m_ctrInspPointLabel[nChannel][nRsltPoint[1]].SetBackColor(RGB(0, 255, 0));
		}
		else
		{
			m_dlgInspPoint[nChannel]->SetInspRsltPointView(nRsltPoint[0], FALSE);
			m_dlgInspPoint[nChannel]->SetInspRsltPointView(nRsltPoint[1], FALSE);

			m_ctrInspPointLabel[nChannel][nRsltPoint[0]].SetBackColor(RGB(255, 0, 0));
			m_ctrInspPointLabel[nChannel][nRsltPoint[1]].SetBackColor(RGB(255, 0, 0));
		}

		double dPointRslt[2];
		dPointRslt[0] = m_dlgInspRslt[nChannel]->GetPointRslt(nRsltPoint[0]);
		dPointRslt[1] = m_dlgInspRslt[nChannel]->GetPointRslt(nRsltPoint[1]);

		if(dPointRslt[0] < dPointRslt[1])
		{
			//m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[0], 1);
			m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[1], 0);
		}
		else if(dPointRslt[0] > dPointRslt[1])
		{
			m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[0], 0);
			//m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[1], 1);
		}
		else
		{
			//m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[0], 1);
			//m_dlgInspPoint[nChannel]->SetEachRsltPoint(nRsltPoint[1], 1);
		}
	}

	m_dlgInspRslt[nChannel]->SetPointRslt(m_bInspPointRslt[nChannel]);

	strText.Empty();

	m_strStartTime[nChannel].Empty();
	m_strEndTime[nChannel].Empty();
	m_strTackTime[nChannel].Empty();
	
	//20190218 ngh
	m_strStartTime[nChannel].Format(_T("%02d:%02d:%02d"), m_cStart[nChannel].GetHour(), m_cStart[nChannel].GetMinute(), m_cStart[nChannel].GetSecond());
	m_strEndTime[nChannel].Format(_T("%02d:%02d:%02d"), m_cEnd[nChannel].GetHour(), m_cEnd[nChannel].GetMinute(), m_cEnd[nChannel].GetSecond());
	m_strTackTime[nChannel].Format(_T("%0.3f"), m_dTackTime[nChannel]);

	strText += (m_strBarcode[nChannel] + _T(","));
	switch(nChannel)
	{
	case 0:
		strVal1.Format(_T("CH_1"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			PointRslt.dGapPointRslt[i] = m_dInspRslt[nChannel][i];
		}
		break;
	case 1:
		strVal1.Format(_T("CH_2"));
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			PointRslt.dGapPointRslt[i] = m_dInspRslt[nChannel][i];
		}
		break;
	}

	m_dlgAvgRsltView[nChannel]->SetSpec(SeqPara.Spec);
	m_dlgAvgRsltView[nChannel]->SetSpec(SeqPara.SpecPoint, 1);
	m_dlgAvgRsltView[nChannel]->SetSpread(SeqPara.Spread);
	m_dlgAvgRsltView[nChannel]->SetSpread(SeqPara.SpreadPoint, 1);

	m_dlgAvgRsltView[nChannel]->SetSpreadAlarmCnt(SystemPara.nSpreadAlarmCheckCnt);
	m_dlgAvgRsltView[nChannel]->SetAvgRsltCnt(SystemPara.nAvgRsltCnt);
	m_dlgAvgRsltView[nChannel]->SetAvgRsltVector(PointRslt, -1);
	
	strText += (strVal1 + _T(","));
	strText += (m_strUseModelName + _T(","));
	strText += (m_strStartTime[nChannel] + _T(","));
	strText += (m_strEndTime[nChannel] + _T(","));
	strText += (m_strTackTime[nChannel] + _T(","));

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		if(!bPointRslt[i])
		{
			bAllRslt = FALSE;
			break;
		}
	}
	
	if(bAllRslt)
	{
		strText += _T("OK,");
		CountOKAdd(nChannel);
		m_bInspectionRslt[nChannel] = TRUE;
		m_strRslt[nChannel].Format(_T("OK"));
		m_ctrMainRslt[nChannel].SetBackColor(RGB(0, 255, 0));
	}
	else
	{
		strText += _T("NG,");
		CountNGAdd(nChannel, bPointRslt);
		m_bInspectionRslt[nChannel] = FALSE;
		m_strRslt[nChannel].Format(_T("NG"));
		m_ctrMainRslt[nChannel].SetBackColor(RGB(255, 0, 0));
	}
	m_ctrMainRslt[nChannel].SetWindowTextW(m_strRslt[nChannel]);
	CountView(nChannel);

	double dRatio[MAX_RSLT_POINT_CNT];
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		dRatio[i] = (double)m_iPointNGConunt[nChannel][i] / (double)m_iCount[nChannel] * 100.;

	m_dlgAvgRsltView[nChannel]->SetRatio(dRatio);

	if(m_SystemPara.bRsltImageSave || m_SystemPara.bRsltNGImageSave)
	{
		if(WaitImageSaveThreadEnd(nChannel))
		{
			if(nChannel == 0)
				m_pThreadImageSave[nChannel] = AfxBeginThread(ImageSaveThread0, this, THREAD_PRIORITY_NORMAL);
			else
				m_pThreadImageSave[nChannel] = AfxBeginThread(ImageSaveThread1, this, THREAD_PRIORITY_NORMAL);

			if(m_pThreadImageSave != NULL)
			{
				m_pThreadImageSave[nChannel]->m_bAutoDelete = FALSE;
				m_pThreadImageSave[nChannel]->ResumeThread();
			}
		}
	}

	int nInspPointMax = 1;

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		strVal1.Format(_T("%0.3f,"), dRslt[i]);
		strText += strVal1;
	}

	strVal1.Empty();
	strVal2.Empty();
	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		strVal1.Format(_T("%0.3f,%0.3f,"), SeqPara.Spec[i].dMinSpec, SeqPara.Spec[i].dMaxSpec);
		strText += strVal1;
	}

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		strVal2.Empty();
		strVal1.Format(_T("%0.3f,"), m_dInspRslt[nChannel][i]);
		
		strVal2 += strVal1;
		strText += strVal2;
	}

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		strVal1.Format(_T("%0.3f,%0.3f"), SeqPara.SpecPoint[i].dMinSpec, SeqPara.SpecPoint[i].dMaxSpec);
		if(i < MAX_INSP_POINT_CNT - 1)
		{
			strVal1 += _T(",");
		}
		strText += strVal1;

	}

	SaveInspResultReport(nChannel, strText);

	m_dlgInspRslt[nChannel]->SetLR(-1);
	m_dlgInspRslt[nChannel]->SetDriveName(SystemPara.strDrive);
	m_dlgInspRslt[nChannel]->SetBarcode(m_strBarcode[nChannel]);
	m_dlgInspRslt[nChannel]->SetAllRsltView();

	WriteSamsungLog(nChannel);

	strText.Format(_T("Ch No %d, RsltDisplay End"), nChannel + 1);
	m_cLogFile->WriteText(strText);

	return rslt;
}

UINT CGapInspectionDlg::ImageSaveThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadImageSave(0, pdlg->GetInspectionRslt(0));

	return 0;
}

UINT CGapInspectionDlg::ImageSaveThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadImageSave(1, pdlg->GetInspectionRslt(1));

	return 0;
}

void CGapInspectionDlg::ThreadImageSave(int nChannel, BOOL isInspectionRslt)
{
	CString strText, strSavePath, strSaveFileName;
	TypeSystemPara SystemPara = m_dlgSystem->GetSystemPara();;
	CTime cStart = m_cStart[nChannel];

	if(isInspectionRslt)
	{
		if(SystemPara.bRsltImageSave && DriveUseSearch(SystemPara.strDrive))
		{
			strText.Format(_T("%02d%02d%02d"), cStart.GetHour(), cStart.GetMinute(), cStart.GetSecond());
			strSavePath.Format(_T("%s%s%04d\\%02d\\%02d\\OK\\Ch_%d\\%s_%s\\"), SystemPara.strDrive, RSLT_IMAGE_FILE_PATH, cStart.GetYear(), cStart.GetMonth(), cStart.GetDay(), nChannel + 1, m_strBarcode[nChannel], strText);
			MakeDirectories(strSavePath);
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strSaveFileName.Format(_T("%s%s_RSLT.jpg"), strSavePath, InspPointToString(i + 1));
				SaveIplImage(m_SaveImage[nChannel][i], strSaveFileName);
			}

			/*for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
			{
				for(int j = 0; j < MAX_GRAB_CNT; j++)
				{
					for(int k = 0; k < MAX_INSP_POINT_CNT; k++)
					{
						strSaveFileName.Format(_T("%sCMA%d_GRAB%d_POINT%d_%s.jpg"), strSavePath, i + 1, j + 1, k + 1, InspPointToString(i + 1));
						SaveIplImage(m_SaveGrabImage[i + (nChannel * MAX_CHANNEL_CNT)][j][k], strSaveFileName);
					}
				}
			}*/
		}
	}
	else
	{
		if(SystemPara.bRsltNGImageSave && DriveUseSearch(SystemPara.strDrive))
		{
			strText.Format(_T("%02d%02d%02d"), cStart.GetHour(), cStart.GetMinute(), cStart.GetSecond());
			strSavePath.Format(_T("%s%s%04d\\%02d\\%02d\\NG\\Ch_%d\\%s_%s\\"), SystemPara.strDrive, RSLT_IMAGE_FILE_PATH, cStart.GetYear(), cStart.GetMonth(), cStart.GetDay(), nChannel + 1, m_strBarcode[nChannel], strText);
			MakeDirectories(strSavePath);
			for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
			{
				strSaveFileName.Format(_T("%s%s_RSLT.jpg"), strSavePath, InspPointToString(i + 1));
				SaveIplImage(m_SaveImage[nChannel][i], strSaveFileName);
			}

			/*for(int i = 0; i < ONE_CHANNEL_MAX_CAM_CNT; i++)
			{
				for(int j = 0; j < MAX_GRAB_CNT; j++)
				{
					for(int k = 0; k < MAX_INSP_POINT_CNT; k++)
					{
						strSaveFileName.Format(_T("%sCMA%d_GRAB%d_POINT%d_%s.jpg"), strSavePath, i + 1, j + 1, k + 1, InspPointToString(i + 1));
						SaveIplImage(m_SaveGrabImage[i + (nChannel * MAX_CHANNEL_CNT)][j][k], strSaveFileName);
					}
				}
			}*/
		}
	}
}

BOOL CGapInspectionDlg::WaitImageSaveThreadEnd(int nChannel)
{
	BOOL rslt = TRUE;

	if(m_pThreadImageSave[nChannel] != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadImageSave[nChannel]->m_hThread, AUTO_DELETE_THREAD_TIME_OUT);

		if(dwRet == WAIT_FAILED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			rslt = FALSE;
		}
	}

	return rslt;
}

void CGapInspectionDlg::SaveInspResultReport(int nChannel, CString sMsg)
{
	m_bInspLogCopy[nChannel] = FALSE;
	m_bInspLogCopyWrite[nChannel] = FALSE;
	CStdioFile pFile;
	CString strResultData,tempStr,strDefectType;
	TypeSystemPara SystemPara;
	int nTimerCnt = 0;
	SystemPara = m_dlgSystem->GetSystemPara();

	if(nChannel == 1)
	{
		nTimerCnt = 5;
	}

	CTime NowTime=CTime::GetCurrentTime();
	CString strCurtime , strCurtimeday;
	strCurtime  = NowTime.Format(_T("%Y")) + "\\" + NowTime.Format(_T("%m"));
	strCurtimeday	= NowTime.Format(_T("%d"));
	CString strPath,tStr, strText, strFileName, strCopyPath;

	strText.Format(_T("Ch No %d, SaveInspResultReport Start"), nChannel + 1);
	m_cLogFile->WriteText(strText);


	tStr.Format(_T("%s%s%s\\Ch%d_InspReport_%s_day.csv"), SystemPara.strDrive, RSLT_REPORT_FILE_PATH, strCurtime, nChannel + 1, strCurtimeday);
	strPath.Format(_T("%s%s%s"), SystemPara.strDrive, RSLT_REPORT_FILE_PATH, strCurtime);
	strFileName.Format(_T("Ch%d_InspReport_%s_day.csv"), nChannel + 1, strCurtimeday);

	if(DriveUseSearch(SystemPara.strDrive))
	{
		CreateDir(strPath);

		if(GetFileAttributes(strPath) != FILE_ATTRIBUTE_DIRECTORY)
		{	
			if(CreateDirectory(strPath, NULL) == 0) return;
		}
	
		CFileFind finder; 
		BOOL bWorking = finder.FindFile(tStr);
	
		BOOL bOpen;
		if(!bWorking)
		{
			if(bOpen = pFile.Open(tStr, CFile::modeCreate |CFile::modeWrite))
			{
				pFile.WriteString(RSLT_FILE_LABEL);
				tempStr.Format(_T("%s\n"), sMsg);
				pFile.Seek(0L,CFile::end);		
				pFile.WriteString(tempStr);
				pFile.Close();	
			}
		}
		else
		{
			bOpen = pFile.Open(tStr, CFile::modeNoInherit |CFile::modeWrite);

			if(bOpen)
			{
				tempStr.Format(_T("%s\n"), sMsg);
				pFile.Seek(0L,CFile::end);		
				pFile.WriteString(tempStr);
				pFile.Close();	
			}
			else
			{
				MakeDirectories(BACKUP_FILE_PATH);
				strCopyPath.Format(_T("%s%s"), BACKUP_FILE_PATH, strFileName);

				bWorking = finder.FindFile(strCopyPath);

				m_bSrcInspLogFilePath[nChannel].Format(_T("%s"), tStr);
				m_bDstInspLogFilePath[nChannel].Format(_T("%s"), strCopyPath);

				if(!bWorking)
				{
					if(CopyFile(tStr, strCopyPath, FALSE))
					{
						bOpen = pFile.Open(strCopyPath, CFile::modeNoInherit | CFile::modeWrite);
						if(bOpen)
						{
							m_bInspLogCopy[nChannel] = TRUE;
							m_bInspLogCopyWritingWait[nChannel] = TRUE;
							SetTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0 + nTimerCnt, 100, NULL);
							tempStr.Format(_T("%s\n"), sMsg);
							pFile.Seek(0L,CFile::end);		
							pFile.WriteString(tempStr);
							pFile.Close();	
						}
					}
				}
				else
				{
					bOpen = pFile.Open(strCopyPath, CFile::modeNoInherit | CFile::modeWrite);
					if(bOpen)
					{
						m_bInspLogCopy[nChannel] = TRUE;
						m_bInspLogCopyWritingWait[nChannel] = TRUE;
						SetTimer(MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0 + nTimerCnt, 100, NULL);
						tempStr.Format(_T("%s\n"), sMsg);
						pFile.Seek(0L,CFile::end);		
						pFile.WriteString(tempStr);
						pFile.Close();	
					}
				}
			}
		}
		m_bInspLogCopyWrite[nChannel] = TRUE;
	}

	strText.Format(_T("Ch No %d, SaveInspResultReport End"), nChannel + 1);
	m_cLogFile->WriteText(strText);
}

LRESULT CGapInspectionDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}

LRESULT CGapInspectionDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	CString strCap;
	
	m_bClientRecive = FALSE;
	m_bClientConnect = FALSE;

	strCap.Format(_T("PLC Disconnect"));
	m_ctrMainPLCState.SetWindowTextW(strCap);
	m_ctrMainPLCState.SetBackColor(RGB(255, 0, 0));
	
	delete m_Client;
	m_Client = NULL;

	m_Client = new CClientSock();
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd);
	SetTimer(MAIN_TIMER_SERVER_AUTO_CONNECT, AUTO_CONNECT_DELAY, NULL);

	KillTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0);
	KillTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_1);

	strCap.Format(_T("PLC <-> PC Disconnect : IP : %s  Port : %d"), m_SystemPara.ServerPara.strIP, m_SystemPara.ServerPara.nPortNum);
	m_cLogFile->WriteText(strCap);

	return 0;
}

LRESULT CGapInspectionDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
	int err = (int)wParam;
	CString strCap;
	int nTime = PLC_AUTO_DEFAULT_STATE_TIME;

	switch(err)
	{
	case NULL:				//연결 성공
		KillTimer(MAIN_TIMER_SERVER_AUTO_CONNECT);
		m_bClientConnect = TRUE;
		m_bClientConnectFailCheck = FALSE;
		strCap.Format(_T("PLC Connect"));
		m_ctrMainPLCState.SetWindowTextW(strCap);
		m_ctrMainPLCState.SetBackColor(RGB(0, 255, 0));
		strCap.Format(_T("PLC <-> PC Connect : IP : %s  Port : %d"), m_SystemPara.ServerPara.strIP, m_SystemPara.ServerPara.nPortNum);
		m_cLogFile->WriteText(strCap);
		WritePlcVal(0, 2000);
		WritePlcVal(1, 2000);
		
		if(PLC_RECIVE_WAIT_TIME + PLC_AUTO_STATE_TIME > PLC_AUTO_DEFAULT_STATE_TIME)
		{
			nTime = (PLC_AUTO_DEFAULT_STATE_TIME - PLC_RECIVE_WAIT_TIME) + PLC_RECIVE_WAIT_TIME;
		}

		SetTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0, nTime, NULL);
		SetTimer(MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_1, nTime, NULL);
		break;
	case WSAECONNREFUSED:	//연결 시도가 강제로 거부되었습니다.
		if(!m_bClientConnectFailCheck)
		{
			SetTimer(MAIN_TIMER_SERVER_AUTO_CONNECT, AUTO_CONNECT_DELAY, NULL);
		}
		break;
	}

	return 0;
}

BOOL CGapInspectionDlg::WriteCsvLog(CStdioFile *pFile, CString strLog)
{
	BOOL rslt = TRUE;
	CString tempStr;

	tempStr.Format(_T("%s\n"), strLog);
	pFile->Seek(0L,CFile::end);		
	pFile->WriteString(tempStr);

	return rslt;
}

void CGapInspectionDlg::WriteDGSFile(int nChannel)
{
	typeDGSFilePara DGSFile = m_DGSFile[nChannel].front();
	CString strLog, strSrcDGSPath, strDstDGSPath, strDGSFileName;
	CStdioFile pFile;
	BOOL bOpen;

	strDGSFileName.Format(_T("%S"), DGSFile.cDGSFileName);
	strSrcDGSPath.Format(_T("%s%s"), DGS_FILE_PATH, strDGSFileName);
	strDstDGSPath.Format(_T("%s%s"), BACKUP_FILE_PATH, strDGSFileName);
	MakeDirectories(DGS_FILE_PATH);
	MakeDirectories(BACKUP_FILE_PATH);

	if(FileSearch(BACKUP_FILE_PATH, strDGSFileName))
	{
		bOpen = pFile.Open(strDstDGSPath, CFile::modeNoInherit | CFile::modeWrite);
	}
	else if(FileSearch(DGS_FILE_PATH, strDGSFileName))
	{
		CopyFile(strSrcDGSPath, strDstDGSPath, FALSE);
		bOpen = pFile.Open(strDstDGSPath, CFile::modeNoInherit | CFile::modeWrite);
	}
	else
	{
		bOpen = pFile.Open(strDstDGSPath, CFile::modeCreate | CFile::modeWrite);
	}
	
	if(bOpen)
	{
		strLog.Format(_T(""));
		WriteCsvLog(&pFile, strLog);

		strLog.Format(_T("#INIT"));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("MODEL : %S"), DGSFile.cModel);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("P/N : %S"), DGSFile.cBarcode);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("S/W : "));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("DATE : %S"), DGSFile.cDate);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("TIME : %S"), DGSFile.cStartTime);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("TESTCODE : %s"), DGS_TOP_CODE);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("INSTRUMENT : "));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("JIG : %d"), DGSFile.nChannel);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("PROGRAM : %S"), DGSFile.cExeFileName);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("INIFILE : %S"), DGSFile.cModelFileName);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("HW : "));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T(""));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("#TEST"));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("/*================================================================================"));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("Test Conditions, Measured Value, Lower Limit, Upper Limit, P/F, Sec, Code, Code Lsl, Code Usl, Meas Fine, Code Fine "));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("================================================================================*/"));
		WriteCsvLog(&pFile, strLog);

		for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
		{
			strLog.Format(_T("%S"), DGSFile.cRsltData[i]);
			WriteCsvLog(&pFile, strLog);
		}

		//측정 결과 Point 별 출력
		for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
		{
			strLog.Format(_T("%S"), DGSFile.cPointRsltData[i]);
			WriteCsvLog(&pFile, strLog);
		}

		strLog.Format(_T(""));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("#END"));
		WriteCsvLog(&pFile, strLog);

		strLog.Format(_T("RESULT : FAIL"));
		if(DGSFile.isRslt)
			strLog.Format(_T("RESULT : PASS"));
		
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("TEST-TIME : %.2f"), DGSFile.dTackTime);
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("ERROR-CODE : "));
		WriteCsvLog(&pFile, strLog);

		if(DGSFile.isRslt)
			strLog.Format(_T("FAILITEM : "));
		else
			strLog.Format(_T("FAILITEM : %S"), DGSFile.cFailItem);

		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T(""));
		WriteCsvLog(&pFile, strLog);
		strLog.Format(_T("%c"), 0x0c);
		WriteCsvLog(&pFile, strLog);
		pFile.Close();
		m_DGSFile[nChannel].pop_back();
	
		if(CopyFile(strDstDGSPath, strSrcDGSPath, FALSE))
		{
			if(m_DGSFile[nChannel].size() == 0)
			{
				if(FileSearch(BACKUP_FILE_PATH, strDGSFileName))
				{
					DeleteFile(strDstDGSPath);
				}
			}
		}
	}
}

void CGapInspectionDlg::ThreadWriteDGSFile(int nChannel)
{
	while(TRUE)
	{
		if(m_bDGSFileCheckThreadStart[nChannel] && !m_bDGSFileCheckThreadEnd[nChannel])
		{
			if(m_DGSFile[nChannel].size() > 0)
			{
				WriteDGSFile(nChannel);
			}
		}

		if(m_DGSFile[nChannel].size() == 0 && !m_bDGSFileCheckThreadStart[nChannel] && m_bDGSFileCheckThreadEnd[nChannel])
		{
			break;
		}

		//Sleep(10);
		::Delay(10);
	}
}

UINT CGapInspectionDlg::WriteDGSFileThread0(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadWriteDGSFile(0);

	return 0;
}

UINT CGapInspectionDlg::WriteDGSFileThread1(LPVOID pParam)
{
	CGapInspectionDlg *pdlg = (CGapInspectionDlg *)pParam;

	pdlg->ThreadWriteDGSFile(1);

	return 0;
}

BOOL CGapInspectionDlg::WaitWriteDGSFileThreadEnd(int nChannel)
{
	BOOL rslt = TRUE;
	m_bDGSFileCheckThreadStart[nChannel] = FALSE;
	m_bDGSFileCheckThreadEnd[nChannel] = TRUE;

	if(m_pThreadDGSFileCheck[nChannel] != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadDGSFileCheck[nChannel]->m_hThread, AUTO_DELETE_THREAD_TIME_OUT);

		if(dwRet == WAIT_FAILED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			rslt = FALSE;
		}
	}

	return rslt;
}

BOOL CGapInspectionDlg::WriteSamsungLog(int nChannel)
{
	BOOL rslt = TRUE;
	CString strExeFileName, strModelFileName, strTemp;
	double dRslt[MAX_INSP_POINT_CNT];
	BOOL bPointRslt, bFailItemSearch = FALSE, bRsltPoint[MAX_INSP_POINT_CNT], bRslt2, bInspRslt;
	int nRsltDirection[MAX_RSLT_POINT_CNT];
	typeDGSFilePara DGSFile;
	char cPassFail[2];
	TypeSpecPara Spec[MAX_RSLT_POINT_CNT];
	TypeSystemPara SystemPara;
	m_dlgTeach[nChannel]->GetSpec(Spec);
	SystemPara = m_dlgSystem->GetSystemPara();
	this->GetWindowTextW(strExeFileName);
	SYSTEMTIME WriteTime;
	strModelFileName.Format(_T("%s%s_Ch%d"), MODEL_FILE_PATH, m_strUseModelName, nChannel + 1);
	GetWriteFileTime(strModelFileName, &WriteTime);
	strModelFileName.Format(_T("%s_Ch%d_Ver-%02d.%02d.%02d.01(R)"), m_strUseModelName, nChannel + 1, WriteTime.wYear - 2000, WriteTime.wMonth, WriteTime.wDay);

	strTemp.Format(_T("%s_%s_%04d%02d%02d_%02d.csv"), m_dlgSystem->GetDGSModelName(), DGS_TOP_CODE, m_cStart[nChannel].GetYear(), m_cStart[nChannel].GetMonth(), m_cStart[nChannel].GetDay(), nChannel + 1);
	sprintf_s(DGSFile.cDGSFileName, strTemp.GetLength() + 1, "%S", strTemp);

	strTemp.Format(_T("%s"), m_strUseModelName);
	sprintf_s(DGSFile.cModel, strTemp.GetLength() + 1, "%S", m_dlgSystem->GetDGSModelName());//strTemp);

	strTemp.Format(_T("%s"), m_strBarcode[nChannel]);;
	sprintf_s(DGSFile.cBarcode, strTemp.GetLength() + 1, "%S", strTemp);

	strTemp.Format(_T("%04d/%02d/%02d"), m_cStart[nChannel].GetYear(), m_cStart[nChannel].GetMonth(), m_cStart[nChannel].GetDay());;
	sprintf_s(DGSFile.cDate, strTemp.GetLength() + 1, "%S", strTemp);

	strTemp.Format(_T("%s"), m_strStartTime[nChannel]);
	sprintf_s(DGSFile.cStartTime, strTemp.GetLength() + 1, "%S", strTemp);

	DGSFile.nChannel = nChannel + 1;
	strTemp.Format(_T("%s_%s"), strExeFileName, SYSTEM_VER);
	sprintf_s(DGSFile.cExeFileName, strTemp.GetLength() + 1, "%S", strTemp);

	strTemp.Format(_T("%s"), strModelFileName);
	sprintf_s(DGSFile.cModelFileName, strTemp.GetLength() + 1, "%S", strTemp);

	for(int i = 0; i < MAX_RSLT_POINT_CNT; i++)
	{
		dRslt[i] = m_dlgInspRslt[nChannel]->GetRslt(i, &bPointRslt, &bRslt2, &nRsltDirection[i]);

		DGSFile.isRslt = TRUE;
		sprintf(cPassFail, "P");
		if(!bPointRslt)
		{
			DGSFile.isRslt = FALSE;
			sprintf(cPassFail, "F");
		}

		switch(i)
		{
		case 0:
			strTemp.Format(_T("Y,%0.3f,%0.3f,%0.3f,%S,%0.2f,,,,,"), dRslt[i], Spec[i].dMinSpec, Spec[i].dMaxSpec, cPassFail, m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 1:
			strTemp.Format(_T("X RIGHT,%0.3f,%0.3f,%0.3f,%S,%0.2f,,,,,"), dRslt[i], Spec[i].dMinSpec, Spec[i].dMaxSpec, cPassFail, m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 2:
			strTemp.Format(_T("X LEFT,%0.3f,%0.3f,%0.3f,%S,%0.2f,,,,,"), dRslt[i], Spec[i].dMinSpec, Spec[i].dMaxSpec, cPassFail, m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		}

		if(!bPointRslt && !bFailItemSearch)
		{
			bFailItemSearch = TRUE;
			bInspRslt = FALSE;
			switch(i)
			{
			case 0:
				strTemp.Format(_T("Y"));
				sprintf_s(DGSFile.cFailItem, strTemp.GetLength() + 1, "%S", strTemp);
				break;
			case 1:
				strTemp.Format(_T("X RIGHT"));
				sprintf_s(DGSFile.cFailItem, strTemp.GetLength() + 1, "%S", strTemp);
				break;
			case 2:
				strTemp.Format(_T("X LEFT"));
				sprintf_s(DGSFile.cFailItem, strTemp.GetLength() + 1, "%S", strTemp);
				break;
			}
		}
	}

	for(int i = 0; i < MAX_INSP_POINT_CNT; i++)
	{
		m_Vision[nChannel]->GetRsltGapVal(i, &bRsltPoint[i], &dRslt[i]);
		switch(i)
		{
		case 0:
			strTemp.Format(_T("Y RIGHT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 1:
			strTemp.Format(_T("X CAM RIGHT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 2:
			strTemp.Format(_T("X USB RIGHT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 3:
			strTemp.Format(_T("Y LEFT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 4:
			strTemp.Format(_T("X CAM LEFT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		case 5:
			strTemp.Format(_T("X USB LEFT,%0.3f,%0.3f,%0.3f,P,%0.2f,,,,,"), dRslt[i], dRslt[i], dRslt[i], m_dTackTime[nChannel]);
			sprintf_s(DGSFile.cPointRsltData[i], strTemp.GetLength() + 1, "%S", strTemp);
			break;
		}
	}

	DGSFile.dTackTime = m_dTackTime[nChannel];

	m_DGSFile[nChannel].push_back(DGSFile);

	return rslt;
}


void CGapInspectionDlg::ClickBtnenhctrlCam0Connect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(GetCamConnect(0))
	{
		SetSelectCamClose(0);
	}
	else
	{
		SetSelectCamConnect(0);
	}
}


void CGapInspectionDlg::ClickBtnenhctrlCam1Connect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(GetCamConnect(1))
	{
		SetSelectCamClose(1);
	}
	else
	{
		SetSelectCamConnect(1);
	}
}


void CGapInspectionDlg::ClickBtnenhctrlCam0Connect2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(GetCamConnect(2))
	{
		SetSelectCamClose(2);
	}
	else
	{
		SetSelectCamConnect(2);
	}
}


void CGapInspectionDlg::ClickBtnenhctrlCam1Connect2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(GetCamConnect(3))
	{
		SetSelectCamClose(3);
	}
	else
	{
		SetSelectCamConnect(3);
	}
}

void CGapInspectionDlg::SetMainWriteLog(CString strLog)
{
	m_cLogFile->WriteText(strLog);
}

void CGapInspectionDlg::SetAlarmMessage(int nChannel, int nAlarmCode)
{
	CString strText;
	char *cText;
	//int nChannel = 0;
	CRect rect1, rect2, rect3;
	
	switch(nAlarmCode)
	{
	case 101:
		strText.Format(_T("Ch No 1, Top Camera Disconnect"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[0]->SetMessage(ALARM_CODE_TOP_CAM_STATUS_CHANNEL_0, cText);
		nChannel = 0;
		GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect1);
		delete cText;
		break;
	case 102:
		strText.Format(_T("Ch No 1, Bottom Camera Disconnect"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[0]->SetMessage(ALARM_CODE_BOT_CAM_STATUS_CHANNEL_0, cText);
		nChannel = 0;
		GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect1);
		delete cText;
		break;
	case 103:
		strText.Format(_T("Ch No 2, Top Camera Disconnect"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[1]->SetMessage(ALARM_CODE_TOP_CAM_STATUS_CHANNEL_1, cText);
		nChannel = 1;
		GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);
		delete cText;
		break;
	case 104:
		strText.Format(_T("Ch No 2, Bottom Camera Disconnect"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[1]->SetMessage(ALARM_CODE_BOT_CAM_STATUS_CHANNEL_1, cText);
		nChannel = 1;
		GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);
		delete cText;
		break;
	case 105:
		nChannel = 0;
		strText.Format(_T("Ch No 1, Inspection Time Out"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[0]->SetMessage(ALARM_CODE_INSP_TIME_OUT_CHANNEL_0, cText);
		delete cText;
		break;
	case 106:
		nChannel = 1;
		strText.Format(_T("Ch No 2, Inspection Time Out"));
		cText = new char[strText.GetLength() + 1];
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		m_pDlgAlarm[1]->SetMessage(ALARM_CODE_INSP_TIME_OUT_CHANNEL_1, cText);
		delete cText;
		break;
	case 200:
	case 201:
		m_pDlgAlarm[nChannel]->SetMessage("Alarm0200", m_dlgAvgRsltView[nChannel]->GetSpreadAlarmData().cAlarmString);

		if(nChannel == 0)
			GetDlgItem(IDC_STATIC_NO_1)->GetWindowRect(&rect1);
		else
			GetDlgItem(IDC_STATIC_NO_2)->GetWindowRect(&rect1);

		break;
	}

	if(nAlarmCode < 200)
	{
		if(nChannel == 0)
		{
			SetTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_0, 100, NULL);
		}
		else
		{
			SetTimer(MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_1, 100, NULL);
		}
	}

	m_pDlgAlarm[nChannel]->GetWindowRect(&rect2);
	m_pDlgAlarm[nChannel]->MoveWindow(rect1.left + (rect1.Width() / 2) - (rect2.Width() / 2), rect1.top + (rect1.Height() / 2) - (rect2.Height() / 2), rect2.Width(), rect2.Height());

	m_pDlgAlarm[nChannel]->ShowWindow(SW_SHOW);
	if(nAlarmCode < 200) 
	{
		SetAutoState(nChannel, FALSE);
		g_SeqModule[nChannel].Stop();
	}
}

void CGapInspectionDlg::SetLightOnOff(int nCh, BOOL isOn)
{
	char sPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(char) * MAX_PATH);
	CString strText, strCh;
	
	if(nCh < 9)
	{
		if(isOn)
		{
			sprintf_s(sPacket, "H%dON%c%c\0", nCh + 1, 0x0D,0x0A);	//ON
			strCh.Format(_T("H%dON"), nCh + 1);
		}
		else
		{
			sprintf_s(sPacket, "H%dOF%c%c\0", nCh + 1, 0x0D,0x0A);	//OFF
			strCh.Format(_T("H%dOF"), nCh + 1);
		}
	}
	else
	{
		if(isOn)
		{
			switch(nCh)
			{
			case 9:
				sprintf_s(sPacket, "HAON%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HAON"), nCh + 1);
				break;
			case 10:
				sprintf_s(sPacket, "HBON%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HBON"), nCh + 1);
				break;
			case 11:
				sprintf_s(sPacket, "HCON%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HCON"), nCh + 1);
				break;
			}
		}
		else
		{
			switch(nCh)
			{
			case 9:
				sprintf_s(sPacket, "HAOF%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HAOF"), nCh + 1);
				break;
			case 10:
				sprintf_s(sPacket, "HBOF%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HBOF"), nCh + 1);
				break;
			case 11:
				sprintf_s(sPacket, "HCOF%c%c\0", 0x0D, 0x0A);
				strCh.Format(_T("HCOF"), nCh + 1);
				break;
			}
		}
	}
#ifdef TWO_CHANNEL_MODE
	//m_LightCritical.Lock();
	if(m_pSerialComm->Write(sPacket, (DWORD)strlen(sPacket)) == 0)
	{
		strText.Format(_T("Light Packet Write Fail, Packet : %s"), strCh);
		m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
		m_cTestLogFile->WriteText(strText);
#endif
	}
	//m_LightCritical.Unlock();
#endif
	//Sleep(m_SystemPara.nLightOnOffWaitTime);
	::Delay(m_SystemPara.nLightOnOffWaitTime);
}

#ifdef SERIAL_BARCODE
CString CGapInspectionDlg::GetBarcodeReader(int nChannel)
{
	CString strRslt, strTemp, strTemp1;
	BYTE sPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(BYTE) * MAX_PATH);
	memset(m_byReadBarcode[nChannel], 0, sizeof(BYTE) * MAX_PATH);
	strRslt.Format(_T(""));

	if(m_pBarcodeComm[nChannel]->m_Connect)
	{
		m_pBarcodeComm[nChannel]->ReadCommPort(sPacket, 100);

#ifdef ASCII_READER
		strTemp.Format(_T(""));
		strTemp1.Format(_T(""));

		for(int i = 0; i < MAX_PATH; i++)
		{
			if(sPacket[i] != END_BARCODE_READER_CHAR)
			{
				strTemp.Format(_T("%c"), sPacket[i]);
				strRslt.Format(_T("%s%s"), strTemp1, strTemp);
				strTemp1.Format(_T("%s"), strRslt);
				m_byReadBarcode[nChannel][i] = sPacket[i];
			}
			else
			{
				m_nReadLenght[nChannel] = i + 1;
				break;
			}
		}
#else
		strTemp1.Format(_T("%S"), sPacket);
#endif

		strRslt.Format(_T("%s"), strTemp1.Mid(0, m_nReadLenght[nChannel]));
	}

	return strRslt;
}
#endif

void CGapInspectionDlg::WritePacket(int nCh, BYTE byVal)
{
	CString strText;
	char sPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(char) * MAX_PATH);

	if(nCh > 12)
	{
		return;
	}

	if(nCh >= 9)
	{
		switch(nCh)
		{
		case 9:
			sprintf_s(sPacket, "CA%02X%c%c\0", byVal, 0x0D, 0x0A);
			break;
		case 10:
			sprintf_s(sPacket, "CB%02X%c%c\0", byVal, 0x0D, 0x0A);
			break;
		case 11:
			sprintf_s(sPacket, "CC%02X%c%c\0", byVal, 0x0D, 0x0A);
			break;
		}
	}
	else
	{
		sprintf_s(sPacket, "C%d%02X%c%c\0", nCh + 1, byVal, 0x0D, 0x0A);
	}

	//m_LightCritical.Lock();
	if(m_pSerialComm->Write(sPacket, (DWORD)strlen(sPacket)) == 0)
	{
		strText.Format(_T("Light Packet Write Fail, Packet : C%d,%02X"), nCh + 1, byVal);
		m_cLogFile->WriteText(strText);
#ifdef LIGHT_TEST
		m_cTestLogFile->WriteText(strText);
#endif
	}
	CString strTemp;
	strTemp.Format(_T("%S"), sPacket);
	strText.Format(_T("Light Packet Write, Packet : C%d,%s"), nCh + 1, strTemp.Left(4));
#ifdef LIGHT_TEST
	m_cTestLogFile->WriteText(strText);
#endif

	//Sleep(m_SystemPara.nLightOnOffWaitTime);
	::Delay(m_SystemPara.nLightOnOffWaitTime);

	//m_LightCritical.Unlock();
}

void CGapInspectionDlg::ClickBtnenhctrlMainPlcState()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
#ifdef TEST_RUN
	if(m_nTestCnt[0] == 0)
	{
		if(m_bAutoInsp[0])
		{
			for(int i = 0; i < 1; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				if(m_nSeqStepIndex[i] == -1)
				{
					m_cStart[i] = CTime::GetCurrentTime();
					m_lStartTime[i] = GetCurrentTime();
					m_strBarcode[i].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[i].GetYear(), m_cStart[i].GetMonth(), m_cStart[i].GetDay(), m_cStart[i].GetHour(), m_cStart[i].GetMinute(), m_cStart[i].GetSecond());
					SetBarcode(i, m_strBarcode[i]);
				}
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
		}

		SetTimer(10000, 10000, NULL);
		m_nTestCnt[0] = 1;
	}
	else
	{
		m_nTestCnt[0] = 0;
	}

	if(m_nTestCnt[1] == 0)
	{
		if(m_bAutoInsp[1])
		{
			for(int i = 1; i < MAX_CHANNEL_CNT; i++)
			{
				m_nSeqIndex[i] = 0;
				m_nSeqStepIndex[i] = -1;
				if(m_nSeqStepIndex[i] == -1)
				{
					m_cStart[i] = CTime::GetCurrentTime();
					m_lStartTime[i] = GetCurrentTime();
					m_strBarcode[i].Format(_T("%04d%02d%02d_%02d%02d%02d"), m_cStart[i].GetYear(), m_cStart[i].GetMonth(), m_cStart[i].GetDay(), m_cStart[i].GetHour(), m_cStart[i].GetMinute(), m_cStart[i].GetSecond());
					SetBarcode(i, m_strBarcode[i]);
				}
				m_nSeqStepIndex[i]++;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepStart = TRUE;
				m_SeqStepPara[i][m_nSeqStepIndex[i]].bStepEnd = FALSE;
			}
		}

		SetTimer(10001, 10000, NULL);
		m_nTestCnt[1] = 1;
	}
	else
	{
		m_nTestCnt[1] = 0;
	}
#else
	CountReset(1);
#endif
}


BOOL CGapInspectionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if(m_nKeyboardIdx == -1)
			return TRUE;

		if (pMsg->wParam == VK_RETURN)
		{
#ifdef KEY_BARCODE
			m_bBarcodeInputEnd[m_nKeyboardIdx] = TRUE;
#endif
			return TRUE;
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
#ifdef KEY_BARCODE
		if(m_bAutoInsp[m_nKeyboardIdx])
		{
			m_bBarcodeInputEnd[m_nKeyboardIdx] = FALSE;
			CString strTemp1, strTemp2;
			strTemp1.Format(_T("%c"), pMsg->wParam);
			strTemp2.Format(_T(""));

			strTemp2.Format(_T("%s"), m_strKeyBarcode[m_nKeyboardIdx]);
			m_strKeyBarcode[m_nKeyboardIdx].Format(_T("%s%s"), strTemp2, strTemp1);
		}
#endif
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CGapInspectionDlg::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
#ifdef KEY_BARCODE
	int nDeviceCnt = 0;
	HANDLE hKeyHandle[20];
	BOOL rslt = SearchUSBDevice(hRawInput, hKeyHandle, &nDeviceCnt); 

	if(rslt)
	{
		m_nKeyboardIdx = -1;
		for(int i = 0; i < MAX_CHANNEL_CNT; i++)
		{
			if(CompareUSBDevice(hRawInput, hKeyHandle, nDeviceCnt, m_strBarcodeInfo[i]))
			{
				if(m_bAutoInsp[i])
				{
					m_bBarcodeInputEnd[m_nKeyboardIdx] = FALSE;
					m_nKeyboardIdx = i;
				}
				break;
			}
		}
	}
#endif
	CDialogEx::OnRawInput(nInputcode, hRawInput);
}


void CGapInspectionDlg::ClickBtnenhctrlMainBarcodeReset1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	BarcodeReset(0);
}


void CGapInspectionDlg::ClickBtnenhctrlMainBarcodeReset2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	BarcodeReset(1);
}

void CGapInspectionDlg::BarcodeReset(int nChannel)
{
	m_pBarcode[nChannel]->SetInitBarcode();
	SetInitBarcode(nChannel);
	//m_dlgBarcode[nChannel]->ShowWindow(SW_SHOW);
	g_SeqModule[nChannel].Stop();
	g_SeqModule[nChannel].Start();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainTotalCnt()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainTotalCnt2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainOkCnt()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainOkCnt2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainNgCnt()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainNgCnt2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainTackTime()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainTackTime2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainOkRatio()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}

void CGapInspectionDlg::DblClickBtnenhctrlMainOkRatio2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//NGRatioView();
}


void CGapInspectionDlg::NGRatioView(BOOL isShow)
{
	if(isShow)
	{
		CRect rect;
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_6)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_5)->MoveWindow(rectTotalCnt[0][0].left, rectTotalCnt[0][0].top, rect.Width(), rect.Height());
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT)->MoveWindow(rectTotalCnt[0][1].left, rectTotalCnt[0][1].top, rect.Width(), rect.Height());

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_15)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_11)->MoveWindow(rectTotalCnt[1][0].left, rectTotalCnt[1][0].top, rect.Width(), rect.Height());
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT2)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT2)->MoveWindow(rectTotalCnt[1][1].left, rectTotalCnt[1][1].top, rect.Width(), rect.Height());

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_7)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_8)->MoveWindow(rectNGRatioCnt[0][0].left, rectNGRatioCnt[0][0].top, rectNGRatioCnt[0][0].Width(), rect.Height());
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME)->MoveWindow(rectNGRatioCnt[0][1].left, rectNGRatioCnt[0][1].top, rectNGRatioCnt[0][1].Width(), rect.Height());

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_13)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_14)->MoveWindow(rectNGRatioCnt[1][0].left, rectNGRatioCnt[1][0].top, rectNGRatioCnt[1][0].Width(), rect.Height());
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME2)->MoveWindow(rectNGRatioCnt[1][1].left, rectNGRatioCnt[1][1].top, rectNGRatioCnt[1][1].Width(), rect.Height());

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_CNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_10)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_12)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_CNT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_15)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT2)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_7)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_RATIO)->ShowWindow(SW_SHOW);
		
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_13)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_RATIO2)->ShowWindow(SW_SHOW);
	}
	else
	{
		CRect rect;
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_10)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_5)->MoveWindow(rectTotalCnt[0][0].left, rectTotalCnt[0][0].top, rect.Width(), rect.bottom - rectTotalCnt[0][0].top);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT)->MoveWindow(rectTotalCnt[0][1].left, rectTotalCnt[0][1].top, rect.Width(), rect.bottom - rectTotalCnt[0][1].top);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_15)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_11)->MoveWindow(rectTotalCnt[1][0].left, rectTotalCnt[1][0].top, rect.Width(), rect.bottom - rectTotalCnt[1][0].top);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT2)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TOTAL_CNT2)->MoveWindow(rectTotalCnt[1][1].left, rectTotalCnt[1][1].top, rect.Width(), rect.bottom - rectTotalCnt[1][1].top);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_7)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_8)->MoveWindow(rectNGRatioCnt[0][0].left, rectNGRatioCnt[0][0].top, rect.Width(), rect.bottom - rectNGRatioCnt[0][0].top);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME)->MoveWindow(rectNGRatioCnt[0][1].left, rectNGRatioCnt[0][1].top, rectNGRatioCnt[0][1].Width(), rect.bottom - rectNGRatioCnt[0][1].top);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_13)->GetWindowRect(&rect);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_14)->MoveWindow(rectNGRatioCnt[1][0].left, rectNGRatioCnt[1][0].top, rect.Width(), rect.bottom - rectNGRatioCnt[1][0].top);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_TACK_TIME2)->MoveWindow(rectNGRatioCnt[1][1].left, rectNGRatioCnt[1][1].top, rectNGRatioCnt[1][1].Width(), rect.bottom - rectNGRatioCnt[1][1].top);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_CNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_CNT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_15)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_NG_CNT2)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_RATIO)->ShowWindow(SW_HIDE);
		
		GetDlgItem(IDC_BTNENHCTRL_MAIN_CAP_13)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNENHCTRL_MAIN_OK_RATIO2)->ShowWindow(SW_HIDE);
	}
}
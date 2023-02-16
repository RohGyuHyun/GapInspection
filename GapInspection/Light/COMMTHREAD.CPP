#include "stdafx.h"
#include <stdio.h>
#include "CommThread.h"

//	Message를 받을 Window Handle
//	이 크래스를 사용한는 윈도에서 HWND hCommWnd = this->m_hWnd로 설정해 준다 
//extern HWND hCommWnd;

CQueue::CQueue()
{
	buff = new BYTE[BUFF_SIZE];
	InitializeCriticalSection(&csLock);
	Clear();
}

CQueue::~CQueue()
{
	DeleteCriticalSection(&csLock);
	delete [] buff;
}

//	Queue Buffer에 들어온 데이터의 개수를 가리킬 Position
//	을 초기화하고 Queue사용할 Buffer를 0으로 초기화 한다 
//	memset()함수는 buff배열을 BUFF_SIZE크기만큼 0으로 채운다

void CQueue::Clear()
{
	EnterCriticalSection(&csLock);
	m_iHead = m_iTail = 0;
	memset(buff,0,BUFF_SIZE);
	LeaveCriticalSection(&csLock);
}

//	Queue에 들어 있는 자료 갯수를 Return한다 
//	BUFF_SIZE % BUFF_SIZE에 의해 BUFF_SIZE값은 제거 되고 
//	m_intHead - mintTail 결과 값만 return하게 된다.
//	즉, 정해진 크기 범위에서 변화된 데이터 갯수만 알수 있다 

int CQueue::GetSize()
{	
	EnterCriticalSection(&csLock);
	int nSize = (m_iHead - m_iTail + BUFF_SIZE) % BUFF_SIZE;
	LeaveCriticalSection(&csLock);
	return nSize;
}

//	Queue에 1 byte를 넣는다 
//	[1]	처음에 Queue Buffer에 데이터가  꽉 찼는지 검사
//	[2] Queue Buffer에 1 byte를 대입하고 m_iHead Position을 증가시킨다
//	[3] 나머지 연산자에 의해 m_intHead 크기가 BUFF_SIZE보다 크면 0으로
//		Clear되며 BUFF_SIZE크기를 넘을때까지는 변화가 없다 

BOOL CQueue::PutByte(BYTE byte)
{
	if(GetSize() == (BUFF_SIZE-1)) return FALSE;

	EnterCriticalSection(&csLock);
	buff[m_iHead++] = byte;
	m_iHead %= BUFF_SIZE;
	LeaveCriticalSection(&csLock);

	return TRUE;
}

//	Queue에 1 byte를 꺼낸다 
//	[1]	처음에 Queue Buffer에 데이터가  꽉 찼는지 검사
//	[2] Queue Buffer에 1 byte를 포인트 매개값이 가리키는 곳에 대입하고
//		m_iTail Position을 증가시킨다
//	[3] 나머지 연산자에 의해 m_intTail 크기가 BUFF_SIZE보다 크면 0으로
//		Clear되며 BUFF_SIZE크기를 넘을때까지는 변화가 없다 

BOOL CQueue::GetByte(BYTE* pbyte)
{
	if(GetSize() == 0) return FALSE;

	EnterCriticalSection(&csLock);
	*pbyte = buff[m_iTail++];
	m_iTail %= BUFF_SIZE;
	LeaveCriticalSection(&csLock);

	return TRUE;
}












//#############################################################################
// 포트 sPortName을 dwBaud 속도로 연다.
// ThreadWatchComm 함수에서 포트에 무언가 읽혔을 때 MainWnd에 알리기
// 위해 WM_COMM_READ메시지를 보낼때 같이 보낼 wPortID값을 전달 받는다.
//#############################################################################

CCommThread::CCommThread()
{
	m_intInQueue = 4096;
	m_intOutQueue = 4096;

	m_hComm = NULL;

	m_hWnd = NULL;
	m_bPortOpen = FALSE;
}

//#############################################################################

//								Comm 1 Open

//#############################################################################

BOOL CCommThread::OpenPort(HWND hWnd, CString strPortName, DWORD dwBaud, WORD wPortID)
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;
	m_bPortOpen = FALSE;
	m_wPortID	= wPortID;

	m_hWnd = hWnd;

/******************************************************************************
	overlapped structure 변수 초기화.

	typedef struct_OVERLAPPED{
		DWORD	Internal;		
		DWORD	InternalHigh;
		DWORD	Offset;			- File에서 읽기 시작할 위치의 하위 32비트값 
		DWORD	OffsetHigh;
		HANDLE	hEvent;
	}OVERLAPPED;

	Internal...운영 체체가 사용하도록 예약되어 있다
	InternalHigh...운영 체체가 사용하도록 예약되어 있다 	
	Offset...File에서 읽기 시작할 위치의 하위 32비트값. 호출 프로세스는 ReadFile
			 또는 WriteFile 함수를 호출하기 전해 이 멤버의 값을 정해주어야 한다
			 네임드 파이프와 통신 장치로부터 읽거난 이들에 쓸때는 이멤버의 값이 
			 0 이 되어야 한다 
	Offset...File에서 읽기 시작할 위치의 상위 32비트값. 호출 프로세스는 ReadFile
			 또는 WriteFile 함수를 호출하기 전해 이 멤버의 값을 정해주어야 한다
			 네임드 파이프와 통신 장치로부터 읽거난 이들에 쓸때는 이멤버의 값이 
			 0 이 되어야 한다 	
	hEvent...읽어서 전송이 종료되었을 때 신호를 받는 상태로 지정해 주어야 할 
			 Event의 Handle. 호출 프로세스는 ReadFile, WriteFile, ConnectNamePipe
			 또는 TransactNamedPipe 함수를 호출하기 전에 이 멤버의 값을 정해 
			 주어야 한다 
*******************************************************************************
	CreateEvent()

	SetEvent함수를 사용해서 프로그램의 의한 신호 또는 비신호 상태가 될수 있다
	응용프로그램은 이벤트를 사용해서 활동중인 쓰레드와 쓰레드의 동기화를 통지한다 

******************************************************************************/
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	if (!(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))	
		return FALSE;

	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if (!(m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		return FALSE;
/******************************************************************************
	[1]	포트를 열자

  	m_sPortName...열고자하는 Device Port String으로 결정
	GENERIC_READ | GENERIC_WRITE...Device에서 Access할 모드설정,
	읽고 쓰기 모두 결정
	0...File의 Only Read 및 Read/Write 결정 Device File에서 의미 없음
	NULL...File의 보안을 설정 Device File에서는 사용 안함
	OPEN_EXISTING...File을 생성할때 새로,기존것에 +,이미존재할경우만 생성 
	을 결정 Device File에서는 존재지 않으면 생성 못하므로 OPEN_EXISTING
	FILE_ATTRIBUTE_NORMAL...일반 File
	FILE_FLAG_OVERLAPPED...Device Buffer에서 읽기나 쓰기가 끝남을 알려준다
	읽고쓰는 함수에서 OVERAPPED structure를 이용하여 Overapped Read/Write 해준다 
	NULL...NT에서나 유용함 
******************************************************************************/
	//strcpy_s(m_szPortName, strPortName);	// <<---- COM1,COM2
	m_strPortName = strPortName;
#ifdef _UNICODE
	m_hComm = CreateFile(	
				m_strPortName, 
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
				NULL
			  );
#else
	m_hComm = CreateFile(	
				(LPCSTR)m_strPortName, 
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
				NULL
			  );
#endif
	// OK...Handle return, NO...INVALID_HANDLE_VALUE(-1) return
	if (m_hComm == (HANDLE)INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_osRead.hEvent);
		CloseHandle(m_osWrite.hEvent);
		return FALSE;
	}
/******************************************************************************
	[2] 포트 상태를 설정하자

	사용하고자 하는 Event들을 조합하고 0 일경우 모든 Event가 취소된다 

  	EV_BREAK - A break was detected on input. 
	EV_CTS	- The CTS (clear-to-send) signal changed state. 
	EV_DSR	- The DSR (data-set-ready) signal changed state. 
	EV_ERR	- A line-status error occurred.
			  Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
	EV_RING	- A ring indicator was detected. 
	EV_RLSD	- The RLSD (receive-line-signal-detect) signal changed state. 
	EV_RXCHAR - A character was received and placed in the input buffer. 
	EV_RXFLAG - The event character was received and placed in the input buffer.
				The event character is specified in the device’s DCB structure,
				which is applied to a serial port by using the SetCommState function. 
	EV_TXEMPTY - The last character in the output buffer was sent. 
******************************************************************************/
	SetCommMask( 
		m_hComm,EV_RXCHAR
	);	
/******************************************************************************
	[3] 입출력 Queue의 크기를 결정자
	
	입력되는 Buffer의 크기는 주고 받는 데이터의 양에 따라 결정되므로 DialogBox
	에서 설정 할수 있도록 int형 변수를 사용한다
	일반적으로 4096(80*25*4)의 크기를 가지는게 좋다 
******************************************************************************/
	SetupComm( 
		m_hComm,
		m_intInQueue,
		m_intOutQueue
	);	
/******************************************************************************
	[4] 입출력 Buffer를 깨끗이 초기화 하자

	처음 Buffer를 생성하면 Serial Port에 대기하고 있던 모든 입출력 데이터를
	제거하고 전송 및 수신 준비를 해야한다 

	PURGE_TXABORT - Overapped 전송작업을 취소 
	PURGE_RXABORT - Overapped 전송작업을 취소
	PURGE_TXCLEAR - 출력 Buffer를 Clear한다
	PURGE_RXCLEAR - 입력 Buffer를 Clear한다
******************************************************************************/
	PurgeComm( 
		m_hComm,					
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR
	);
/******************************************************************************
	[5] Buffer에서 읽고 쓰는 시간 timeout 설정한다

	이제 읽기 쓰기 Overapped I/O를 위한 Timeout값들을 설정해 주어야 할 단계입니다
	MFC에서 Serial Port의 Timeout값을 설정해주기 위해 사용하는 구조체는
	COMMTIMEOUT입니다.

	typedef	struct_COMMTIMEOUTS{
				DWORD	ReadIntervalTimeout;			[1]
				DWORD	ReadTotalTimeoutMultiplier;		[2]
				DWORD	ReadTotalTimeoutConstant;		[3]
				DWORD	WriteTotalTimeoutMultiplier;	[4]
				DWORD	WriteTotalTimeoutConstant;		[5]
			}COMMTIMEOUTS,*LPCOMMTIMEOUTS;

[1]	Data가  들어올 때 두 바이트의 입력이 이루어지는 시간으로, 이 이상의 시간만큼 
	다음 바이트의 입력이 없을 경우에는 입력값을 Return하고 RX_CHAR Message를 
	발생시킵니다

[2][3]	ReadTotalTimeoutMultiplier*ReadTotalTimeoutConstant 
		읽기 동작에 걸리는 총시간으로 이 값이 0이면 사용하지 않는다는 뜻입니다

[3][4]	WriteTotalTimeoutMultiplier*ReadTotalTimeoutConstant 
		쓰기 동작에 걸리는 총시간으로 이 값이 0이면 사용하지 않는다는 뜻입니다

위의 값들을 설정한 후 SetCommTimeouts()함수를 호출하여 Timeout값을 설정합니다

 BOOL		SetCommTimeouts(
			HANDLE			hFile,
			LPCOMMTIMEOUSTS	lpCommTimeouts
		);

즉,  ReadIntervalTimeout만큼 기다리다 다음 Data가 없으면 읽은 Data를 Return하고,  
ReadTotalTimeoutMultiplier  *ReadTotalTimeoutConstant의 시간만큼 기다려도 읽기로
한 Data 사이즈만큼 읽지 못하거나 WriteTotalTimeout Multiplier  *WriteTotalTimeoutConstant
시간 안에 전송을 못 할경우 Error event가 발생되도록 설정하는 것입니다.

******************************************************************************/
	timeouts.ReadIntervalTimeout		= 1;//0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant	= 1;
	timeouts.WriteTotalTimeoutMultiplier= 1;//2*CBR_115200 / dwBaud;	// <<-- 9600
	timeouts.WriteTotalTimeoutConstant	= 1;

	SetCommTimeouts( m_hComm, &timeouts);
/******************************************************************************
	[6] dcb 설정
	
	다음은 Port의 전송 속도, 데이터 비트수, 패리티, 스톱 비트수, XON / XOFF Flow
	Control을 설정해줄 차례입니다. MFC에서는 이런 설정을 위해 DCB라는 구조체를
	제공합니다.

typedef	  struct_DCB{
	DWORD	DCBlength;				[1] sizeof(DCB)
	DWORD	BaudRate;				[2] baud rate
	DWORD	fBinary: 1;				[3] binary mode, EOF를 Chack하지 않음
	DWORD	fParity: 1;				[4] Parity를 Chack함
	DWORD	fOutxCtsFlow: 1;		[5] CTS output flow control을 함
	DWORD	fOutxDsrFlow: 1;		[6] DSR output flow control을 함 
	DWORD	fDtrControl: 2;			[7] DTR flow control
	DWORD	fDsrSensitivity: 1;		[8] DSR 감지
 	DWORD	fTXContinueOnXoff: 1;	[9] XOFF라도 전송을 계속함
	DWORD	fOutX: 1;				[10]XON / XOFF 전송 흐름 제어 
	DWORD	fInX: 1;				[11] XON / XOFF 입력 흐름 제어
	DWORD	fErrorChar: 1;			[12] 오류 수정
	DWORD	fNull: 1;				[13] 입력받은 데이터 중 NULL을 없엠
	DWORD	fRtsControl: 2;			[14] RTS flow control
	DWORD	fAbortOnError: 1;		[15] Error 발생시 입출력 취소
	DWORD	fDummy2: 17;			[16] reserved
	WORD	wReserved;				[17] 현재 사용 안함
	WORD	XonLim;					[18] 전송 XON 제한치
	WORD	XoffLim;				[19] 전송 XOFF 제한치
	BYTE	ByteSize;				[20] 한 Byte당 비트수
	BYTE	Parity					[21] 0 ~ 4 = no, odd, even, mark, space
	BYTE	StopBits;				[22] 0, 1, 2 = 1, 1.5, 2
	char 	XonChar;				[23] XON으로 사용할 문자 
	char 	XoffChar;				[24] XOFF로 사용할 문자 
	char	ErrorChar;				[25] 오류시 대체할 문자
	char	EofChar;				[26] EOF 문자
	char	EvtChar;				[27] 수신 이벤트 문자
	WORD	wReserved1;				[28] reserved
}DCB;
	
	위의 값들은 모두 일일이 설정해도 되지만 편의를 위해 GetCommState() 함수를
	이용할 수도 있습니다. 이 함수는 시스템에서 이미 사용중인 설정을 읽어 옵니다
	
	BOOL	GetCommjState( HANDLE hFile, LPDCB lpDCB  );

	이렇게 읽어온 설정 중 원하는 설정을 바꾸어 주고 SetCommState() 함수를 이용해서
	다시 설정해 줍니다.
	
	BOOL	SetCommjState( HANDLE hFile, LPDCB lpDCB  );

******************************************************************************/

	dcb.DCBlength = sizeof(DCB);	// [1]

	GetCommState( m_hComm, &dcb);	

	dcb.fNull   = 0;
	dcb.BaudRate		= dwBaud;	// [2]	9600
	dcb.ByteSize		= 8;		// [20] 8 bit
	dcb.Parity			= 0;		// [21] None
	dcb.StopBits		= 0;		// [22] 1 stop
	
	if (!SetCommState(m_hComm, &dcb))
	{
//		AfxMessageBox("통신포트의 통신설정에 실패했습니다\r\n장치를 닫습니다");
		ClosePort();
		return FALSE;
	}

	/******************************************************************************
	[7] 포트 감시 쓰레드 생성.

  HANDLE	CreateThread(
			LPSECURITY_ATTRIBUTES	lpThreadAttributes,	[1]
			DWORD				dwStackSize,			[2]
			LPTHREAD_START_ROUTINE	lpStartAddress,		[3]
			LPVOID				lpParameter,			[4]
			DWORD				dwCreationFlagz,		[5]
			LPDWORD				lpThreadld				[6]
		);


[1] 생성된 스레드가 부모의 특성을 물려받을지를 결정, 이경우 우리는 NULL로 설정
	하여 특성을 물려받지 않아도 지장이 없습니다.

[2]	생성되는 스레드에서 사용할 스택의 크기, 이것을 0으로 설정하여 주 프로세스의 
	크기에 따라 자동으로 설정되도록 해놓으면 스레드가 종료될 경우에도 자동으로 
	해지 됩니다. 여기서는 0으로 설정하여 이러한 특징을 이용하였습니다. 만약 여기	
	서 설정한 스택 사이즈만큼의 메모리를 할당받지 못하면 스레드가 생성되지 않습
	니다.

[3]	스레드로 수행할 함수의 포인터를 설정합니다.

[4] 스레드에 전달할 파라미터가 있을 경우에 사용합니다. 우리의 경우 부모 프로세스
	의 Window Handle을 이용하여 전달함으로써 부모 프로세스에 메시지를 전달하는데
	사용합니다

[5] 이값을 CREATE_SUSPENDED로 해두면 ResumeThread()를 이용하여 활성화시키기 
	전에는 스레드가 정지해 있습니다. 우리는 스레드가 즉각 작동해야 하므로 0으로
	설정합니다.

[6] 스레드를 식벌하기 위한 핸들의 포인터입니다.

******************************************************************************/	  

	m_hThreadWatchComm = CreateThread(
							NULL,
							0, 
							(LPTHREAD_START_ROUTINE)ThreadWatchComm,
							this,
							0,
							&dwThreadID		// <<--- wPortID
						 );
	if (!m_hThreadWatchComm){
		ClosePort();
		return FALSE;
	}
	m_bPortOpen = TRUE;
	return TRUE;
}

/******************************************************************************

스레드로 수행할 함수입니다 

포트를 감시하고, Serial Port에서 메시지가 발생하면 입력 메시지인가 검사하고 
내용이 있으면 m_ReadData에 저장한 뒤에 MainWnd에 메시지를 보내어 Buffer의 
내용을 읽어가라고 WM_COMM_READ 메세지를 보낸다 

BYTE	buff[2048];	// InQueue의 데이터를 읽어오기 위한 Input Buffer
DWORD	dwBufferCounter;	// 읽은 바이트수
// 포트를 감시하는 루프	
while(pComm->m_bPortOpen){	}	
// 포트 메세지가 발생할때까지 기다린다
WaitCommEvent(pComm->m_hComm, &dwEvent, NULL);
******************************************************************************/
DWORD	ThreadWatchComm(CCommThread* pComm)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;		 
	BYTE		buff[2048];	
	DWORD		dwBufferCounter;		
	char		cEnd[2];
	cEnd[0] = 0x0d;
	cEnd[1] = 0x0a;

	// Event, Overap Structure 설정.
	memset( &os, 0, sizeof(OVERLAPPED));
	if (!(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL))) bOk = FALSE;
	if (! SetCommMask( pComm->m_hComm, EV_RXCHAR)) bOk = FALSE;
	if (!bOk)
	{
		char szMsg[300];
#ifdef _UNICODE
		sprintf_s(szMsg, "Error while creating ThreadWatchComm, %s", pComm->m_strPortName.GetBuffer(pComm->m_strPortName.GetLength()));
		::MessageBox(NULL, (LPCWSTR)szMsg, _T("Error"), MB_OK);
#else
		sprintf(szMsg, "Error while creating ThreadWatchComm, %s", pComm->m_strPortName.GetBuffer(pComm->m_strPortName.GetLength()));
		::MessageBox(NULL, (LPCSTR)szMsg, _T("Error"), MB_OK);
#endif
		return FALSE;
	}

	while (pComm->m_bPortOpen){

		dwEvent = 0;
//=============================================================================		

		WaitCommEvent(pComm->m_hComm, &dwEvent, NULL);

//=============================================================================
		if((dwEvent & EV_RXCHAR) == EV_RXCHAR){
			// 포트에서 읽을 수 있는 만큼 읽는다.

			do{
				dwBufferCounter = pComm->ReadComm(buff, 2048);
				// ReadComm으로부터 받은 데이터 개수를 가지고 읽은 데이터를 
				// 한 바이트씩 Queue에 집어 넣는다
				if (BUFF_SIZE - pComm->m_QueueRead.GetSize() > (int)dwBufferCounter)
				{
					for ( WORD i = 0; i < dwBufferCounter; i++)
					{
						if(buff[0] == 0x1)
						{
							buff[0] = 0x4c;
						}
						else if(buff[0] == 0x00)
						{
							buff[0] = 0x52;
						}

						if(buff[i] == cEnd[0])
						{
							if(buff[i + 1] == cEnd[1])
							{
								break;
							}
						}

						pComm->m_QueueRead.PutByte(buff[i]);
					}
				}
				else
				{
					// 수신큐가 저장 가능한 공간을 초과 하였음.
					char szStr[300];
					buff[dwBufferCounter] = 0;
					sprintf_s(szStr, "M_QueueRead FULL\r\nQueue:%d, Read=%d, Wait=%s\r\n"
					"Head=%d, Tail=%d", 
						pComm->m_QueueRead.GetSize(), dwBufferCounter, buff
						, pComm->m_QueueRead.m_iHead, pComm->m_QueueRead.m_iTail);
#ifdef _UNICODE
					::MessageBox( NULL, (LPCWSTR)szStr, NULL, MB_OK);
#else
					::MessageBox( NULL, (LPCSTR)szStr, NULL, MB_OK);
#endif
				}
			  
			}while (dwBufferCounter);
			//	읽어 가도록 메시지를 보낸다.

			if (pComm->m_hWnd && pComm->m_QueueRead.GetSize())
				::PostMessage(pComm->m_hWnd, WM_COMM_READ, pComm->m_wPortID, 0);
		}
	}// 주 프로세스에서 이 값을 FALSE로 바꿔주면 while은 끝난다 

	// 포트가 ClosePort에 의해 닫히면 m_bPortOpen 가 FALSE가 되어 종료.
	CloseHandle(os.hEvent);
	pComm->m_hThreadWatchComm = NULL;
	return TRUE;
}


/******************************************************************************
	포트를 닫는다
	Event발생을 모두 정지시키고 Buffer을 모두 깨끗이 지운다 
******************************************************************************/
void CCommThread::ClosePort()
{
	m_bPortOpen = FALSE;
	if (m_hComm)
	{
		SetCommMask(m_hComm, 0);
		PurgeComm( 
			m_hComm,
			PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR
		);
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
}


// 포트에 pBuff의 내용을 nToWrite만큼 쓴다.
// 실제로 쓰여진 Byte수를 리턴한다.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten;
	DWORD	dwError;
	DWORD	dwErrorFlags;

	COMSTAT	comstat;

	if (!WriteFile(m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite)){
		// Error Code가 무엇인지 알아본다 
		if (GetLastError() == ERROR_IO_PENDING){
			//	읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			//	특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//	timeouts에 정해준 시간만큼 기다려준다.
			while (!GetOverlappedResult(m_hComm, &m_osWrite, &dwWritten, TRUE)){
				// 마지막으로 발생한 ErrorMessage
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE){
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
					Delay(1,TRUE);
				}
			}
		}
		else{
			dwWritten = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
	return dwWritten;
}
/*****************************************************************************
	포트로부터 pBuff에 nToRead만큼 읽는다.
	실제로 읽혀진 Byte수를 리턴한다.

	BOOL	ReadFile(
				HANDLE			hFile,					[1]
				LPVOID			lpBuffer,				[2]
				DWORD			nNumberOfBytesToRead,	[3]
				LPDWORD			lpNumberOfBytesRead,	[4]
				LPOVERLAPPED	lpOverlapped			[5]
			);

[1] Device File의 handle
[2] Data를 저장한 Buffer의 포인터
[3] 읽을 바이트 수
[4] 읽을 바이트수를 저장할 변수의 포인터
[5] Overapped structure에로의 포인터
******************************************************************************/
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead;
	DWORD	dwError;
	DWORD	dwErrorFlags;
	COMSTAT	comstat;

	ClearCommError( m_hComm, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	
	if (dwRead > 0){
		if (! ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead)){
			if (GetLastError() == ERROR_IO_PENDING){
				// timeouts에 정해준 시간만큼 기다려준다.
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE)){
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE){
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
						Delay(1,TRUE);
					}
				}
			}
			else{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}

	return dwRead;
}


void CCommThread::SetHWnd(HWND hWnd)
{
	this->m_hWnd = hWnd;
}

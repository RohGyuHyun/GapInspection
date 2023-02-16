#ifndef		__COMM_THREAD_H__
#define		__COMM_THREAD_H__

#include <string>
using namespace std;

// Thread와 부모 프로세스가 데이터를 주고 받을 Buffer크기 
#define	BUFF_SIZE		163840				
#define	WM_COMM_READ	(WM_USER+100)		
#define	ASCII_LF		0x0a
#define	ASCII_CR		0x0d
#define	ASCII_XON		0x11
#define	ASCII_XOFF		0x13
//#define	ON				TRUE;
//#define	OFF				FALSE;

class CQueue
{
	CRITICAL_SECTION	csLock;
public:
	BYTE	*buff;		// Queue Buffer
	int		m_iHead;				// Queue Head 및 Tail Position
	int		m_iTail;
	int		GetSize();				// Queue에 들어있는 데이터의 길이를 Return한다 

	BOOL	PutByte(BYTE byte);		// Queue에 1 byte 넣기
	BOOL	GetByte(BYTE *pbyte);	// Queue에 1 byte 꺼내기

	CQueue();			
	~CQueue();			
	void	Clear();				// Queue의 내용을 지운다 
};

//	통신 클래스	CCommThread 
//	포트 열기 : OpenPort("COM1", CBR_9600);
//	포트 읽기 : 포트를 연 후에 포트에 자료가 도착하면 WM_COMM_READ 메시지가
//				메인 윈도우에 전달된다. ON_MESSAGE 매크로를 이용, 함수를 
//				연결하고 m_ReadData String에 저장된 데이터를 이용해서 읽는다
//	포트 쓰기 : WriteComm(buff, 30)과 같이 버퍼와 그 크기를 건네면 된다.


class	CCommThread
{
public:
	// 메시지를 받는 윈도를 변경할때 이용한다.
	void SetHWnd(HWND hWnd);

	CCommThread();

	HANDLE		m_hRead;			// Watch 스레드에서 Read 완료 때까지 대기하기 위한 Event핸들 추가. 
	HWND		m_hWnd;				// 수신 메시지를 받을 윈도의 핸들.
	HANDLE		m_hComm;			// 통신 포트 파일 핸들
//	char		m_szPortName[100];	// 포트 이름 (COM1 ..)
	CString		m_strPortName;
	BOOL		m_bPortOpen;		// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead;			// 포트 파일 Overlapped structure
	OVERLAPPED	m_osWrite;
	HANDLE		m_hThreadWatchComm;	// Watch함수 Thread 핸들.
	WORD		m_wPortID;			// WM_COMM_READ와 함께 보내는 인수.

	CQueue	m_QueueRead;
	string m_strRead;

//=============================================================================
	//BOOL	OpenPort(HWND hWnd, char *psPortName, DWORD dwBaud, WORD wParam);
	BOOL	OpenPort(HWND hWnd, CString strPortName, DWORD dwBaud, WORD wParam);
//=============================================================================
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);

	int		m_intInQueue;
	int		m_intOutQueue;

	void ClosePort();
};

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);



#endif



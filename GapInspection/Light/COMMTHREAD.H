#ifndef		__COMM_THREAD_H__
#define		__COMM_THREAD_H__

#include <string>
using namespace std;

// Thread�� �θ� ���μ����� �����͸� �ְ� ���� Bufferũ�� 
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
	int		m_iHead;				// Queue Head �� Tail Position
	int		m_iTail;
	int		GetSize();				// Queue�� ����ִ� �������� ���̸� Return�Ѵ� 

	BOOL	PutByte(BYTE byte);		// Queue�� 1 byte �ֱ�
	BOOL	GetByte(BYTE *pbyte);	// Queue�� 1 byte ������

	CQueue();			
	~CQueue();			
	void	Clear();				// Queue�� ������ ����� 
};

//	��� Ŭ����	CCommThread 
//	��Ʈ ���� : OpenPort("COM1", CBR_9600);
//	��Ʈ �б� : ��Ʈ�� �� �Ŀ� ��Ʈ�� �ڷᰡ �����ϸ� WM_COMM_READ �޽�����
//				���� �����쿡 ���޵ȴ�. ON_MESSAGE ��ũ�θ� �̿�, �Լ��� 
//				�����ϰ� m_ReadData String�� ����� �����͸� �̿��ؼ� �д´�
//	��Ʈ ���� : WriteComm(buff, 30)�� ���� ���ۿ� �� ũ�⸦ �ǳ׸� �ȴ�.


class	CCommThread
{
public:
	// �޽����� �޴� ������ �����Ҷ� �̿��Ѵ�.
	void SetHWnd(HWND hWnd);

	CCommThread();

	HANDLE		m_hRead;			// Watch �����忡�� Read �Ϸ� ������ ����ϱ� ���� Event�ڵ� �߰�. 
	HWND		m_hWnd;				// ���� �޽����� ���� ������ �ڵ�.
	HANDLE		m_hComm;			// ��� ��Ʈ ���� �ڵ�
//	char		m_szPortName[100];	// ��Ʈ �̸� (COM1 ..)
	CString		m_strPortName;
	BOOL		m_bPortOpen;		// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED	m_osRead;			// ��Ʈ ���� Overlapped structure
	OVERLAPPED	m_osWrite;
	HANDLE		m_hThreadWatchComm;	// Watch�Լ� Thread �ڵ�.
	WORD		m_wPortID;			// WM_COMM_READ�� �Բ� ������ �μ�.

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

// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CCommThread* pComm);



#endif


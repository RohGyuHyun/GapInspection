#pragma once

#include <SetupAPI.h>
#include <Dbt.h>
#include <atlstr.h>
#include <vector>

#define SEARCH_USB_KEYBOARD				0
#define SEARCH_USB_MOUSE					1

extern "C" __declspec(dllexport) int RandVal(int nStart, int nEnd);

extern "C" __declspec(dllexport) BOOL CompareUSBDevice(HRAWINPUT hRawInput, HANDLE *hKeyHandle, int nInputDeviceCnt, CString strSerID);
extern "C" __declspec(dllexport) BOOL SearchUSBDevice(HRAWINPUT hRawInput, HANDLE *hKeyHandle, int *nInputDeviceCnt, int nSearchType = SEARCH_USB_KEYBOARD);
extern "C" __declspec(dllexport) CString LoadBarcodeID(CString strPath, int nChannel);

#define CREATE_CLASS(nSys,ptr, Class)	if(nSys == TRUE && ptr == NULL) \
										{ ptr = new Class;}

#define CREATE_DLG(nSys,ptr, Class, nIDD, pWnd, nShow)	if(nSys == TRUE && ptr == NULL) \
														{ ptr = new Class; \
														ptr->Create(nIDD, pWnd); \
														ptr->ShowWindow(nShow);}

#define CREATE_RECT_DLG(ptr, Class, nIDD, pWnd, rect, nShow)	if(ptr == NULL) \
																{ ptr = new Class; \
																ptr->Create(nIDD, pWnd); \
																ptr->MoveWindow(rect); \
																ptr->ShowWindow(nShow);}

#define CREATE_EX_DLG1(ptr, Class, nShow)				if(ptr == NULL) \
														{ ptr = new Class; ptr->ShowWindow(nShow);}

#define DELETE_CLASS(ptr)			if(ptr) { delete ptr; ptr = NULL;}
#define DELETE_DLG(ptr)				if(ptr) {(ptr)->DestroyWindow(); delete (ptr); ptr = NULL;}
#define DELETE_ARRAY(ptr)			if(ptr)   { delete [] ptr; ptr = NULL;}

#define FILE_CREATE_TIME		0
#define FILE_ACCESS_TIME		1
#define FILE_WRITE_TIME			2

#define LIMITE_RANGE_CHECK_TYPE_INT					0
#define LIMITE_RANGE_CHECK_TYPE_DOUBLE				1

const CString USE_DRIVE_FREE_SIZE_RETURN_VAL_GIGA_BYTE			= _T("GB");
const CString USE_DRIVE_FREE_SIZE_RETURN_VAL_MEGA_BYTE			= _T("MB");
const CString USE_DRIVE_FREE_SIZE_RETURN_VAL_PERCENT			= _T("PER");

extern "C" __declspec(dllexport) int limite(int nVal);
extern "C" __declspec(dllexport) BOOL FileDelete(CString FilePath, CString FileName);
extern "C" __declspec(dllexport) BOOL FileSearch(CString FilePath, CString FileName);
extern "C" __declspec(dllexport) void MakeDirectories(CString path);
extern "C" __declspec(dllexport) void DeleteFolderAndFile(CString strPath);
extern "C" __declspec(dllexport) BOOL CreateDir(LPCTSTR lpszDirectory);
extern "C" __declspec(dllexport) BOOL FolderSearch(CString strFolder);

extern "C" __declspec(dllexport) double DegreeToRation(double deg);
extern "C" __declspec(dllexport) double RatianToDegree(double rad);

extern "C" __declspec(dllexport) void DoEvents();
extern "C" __declspec(dllexport) void Delay(long ms, BOOL bEvent = FALSE);

extern "C" __declspec(dllexport) BOOL LimiteRangeCheck(double dMin, double dMax, double dAfterVal, double dBeforVal, int nInputType = LIMITE_RANGE_CHECK_TYPE_INT, CString strInputLabel = NULL);

extern "C" __declspec(dllexport) BOOL DriveUseSearch(CString strDriveName);
extern "C" __declspec(dllexport) double UseDriveFreeSize(CString strDriveName = _T("D:"), CString strSizeType = USE_DRIVE_FREE_SIZE_RETURN_VAL_PERCENT);

extern "C" __declspec(dllexport) BOOL GetWriteFileTime(CString strFilePath, SYSTEMTIME *returnTime, int nGetTimeType = FILE_WRITE_TIME);
extern "C" __declspec(dllexport) BOOL FileWriteTimeToBackupFile(CString strSrcFilePath, CString strBackupFilePath, CString strFileName);

extern "C" __declspec(dllexport) double GetAvg(double *dEachRsltArr, int nEachRsltCnt);
extern "C" __declspec(dllexport) double GetSTD(double *dEachRsltArr, int nEachRsltCnt, int nOption = 0);//nOption = 1 : 표본의 표준편차 구하기 (STDEV), 0 : 모집단 전체의 표준편차 구하기 (STDEVP)

// 피타고라스 정리 ( 대각의 길이 와 밑변을 가지고 높이 구하기)// x = 대각, y = 밑변
extern "C" __declspec(dllexport) double GetPitaHight(double x, double y);// 피타고라스 정리 ( 대각의 길이 와 밑변을 가지고 높이 구하기)
// 피타고라스 정리 ( 높이 와 밑변을 가지고 대각의 길이 구하기)// x = 밑변, y = 높이
extern "C" __declspec(dllexport) double GetPita(double x, double y);// 피타고라스 정리 ( 높이 와 밑변을 가지고 대각의 길이 구하기)

#ifdef OPEN_CV_LIB
extern "C" __declspec(dllexport) double GetDistance(CvPoint2D32f p1, CvPoint2D32f p2);
extern "C" __declspec(dllexport) double GetAngle(CvPoint2D32f p1, CvPoint2D32f p2);

extern "C" __declspec(dllexport) BOOL LoadIplImage(IplImage *LoadImage, CString strLoadPath, BOOL isColor = FALSE);
extern "C" __declspec(dllexport) BOOL SaveIplImage(IplImage *SaveImage, CString strSavePath);

extern "C" __declspec(dllexport) BOOL Morphology(IplImage *src, IplImage *dst, CvPoint KernelSize, int KernelShape, int nOpenClose);
extern "C" __declspec(dllexport) BOOL SmoothImage(IplImage *src, IplImage *dst, int SmoothType, CvPoint KernelSize, CvPoint2D32f KernelSigma);

extern "C" __declspec(dllexport) BOOL FlipImage(IplImage *SrcImage, IplImage *DstImage, int nDirection = 0);

extern "C" __declspec(dllexport) BOOL TraceInsp(CStaticMatDisplay *Display, IplImage *Img, CvRect *rcRoi, BOOL bTrace = TRUE);

extern "C" __declspec(dllexport) BOOL CvThreshHoldRange(IplImage *src, IplImage *dst, int nThreshHoldVal, int nThreshRange = 50);

extern "C" __declspec(dllexport) BOOL CvCircleSearch(CvSeq *c, CvPoint2D32f *rslt_center, double *rslt_width, double *rslt_height, double * rslt_angle);
#endif


#define		NUL		0x00
#define		SOH		0x01
#define		STX		0x02
#define		ETX		0x03
#define		EOT		0x04
#define		ENQ		0x05
#define		ACK		0x06
#define		NAK		0x15
#define		XON		0x11
#define		XOFF	0x13
#define		ESC		0x1b
#define		CR		0x0d
#define		LF		0x0a

const	int	DEF_MAX_SERIAL_PORT							= 6;			///< Serial Port 수량
const	int ERR_SERIAL_PORT_SUCCESS					= 0;			// Success
const	int ERR_PORT_OPEN_FAIL							= 1;		
const	int ERR_TIME_OUT									= 2;		

#define SERIAL_WRITE_DELAY									40//ms
#define SERIAL_READ_DELAY									100//ms

class CRsPort  
{
public:
	BOOL           m_Connect;
    HANDLE         m_idComDev;

public:
	int ReadCommPort( unsigned char *message, DWORD length);
	int WriteCommPort( unsigned char* message, DWORD dwLength);
	bool IsCommPortOpen();
	CRsPort(CString m_portName);
    CRsPort(CString m_portName, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	virtual ~CRsPort();

protected:
	void CloseCommPort(void);
	BOOL SetupConnection(void);
	void initComport( CString m_portName );
    OVERLAPPED     osWrite;
    OVERLAPPED     osRead;
	DCB            dcb_setup;

};

#define END_BARCODE_READER_CHAR						0x0d
#define BARCODE_MAX_STRING_CNT						15
#define BARCODE_READ_THREAD_END_TIMEOUT		1000
#define MAX_BYTE_CNT										200

class CBarcodeRsPortType
{
private:
	CRsPort *m_RsPort;
	CString m_strBarcode;
	int m_nBarcodeMaxLength;
	BYTE m_byEndChar;

	CWinThread* m_pThreadBarcodeRead;
	BOOL m_bThreadBarcodeStart;
	BOOL m_bThreadBarcodeEnd;

	BYTE m_byReadBarcode[MAX_BYTE_CNT];
	int m_nReadLenght;

	void ThreadBarcodeRead();
	
public:
	CBarcodeRsPortType(CString m_portName, DWORD BaudRate = CBR_9600, BYTE ByteSize = 8, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT, int nBarcodeMaxLength = BARCODE_MAX_STRING_CNT, BYTE byEndChar = END_BARCODE_READER_CHAR);
	~CBarcodeRsPortType();

	void SetBarcodeMaxLength(int nLength);
	CString GetBarcodeString(){return m_strBarcode;};
	void SetInitBarcode(){m_strBarcode.Format(_T("-"));};
	void SetBarcodeReadStart(BOOL isStart);
	CString GetBarcodeReader();

	BOOL GetBarcodeConnect();
	int GetReadBarcodeLength(){return m_nReadLenght;};
protected:
	UINT static BarcodeReadThread(LPVOID pParam);
	BOOL InitThread();
	BOOL WaitEndThread();
	BOOL ReleaseThread();
};

class CBarcodeKeyboardType
{
private:
	CString m_strBarcode;
	int m_nBarcodeMaxLength;
	BYTE m_byEndChar;
	BOOL m_bBarcodEndChar;

public:
	CBarcodeKeyboardType(int nBarcodeMaxLength = BARCODE_MAX_STRING_CNT, BYTE byEndChar = END_BARCODE_READER_CHAR);
	~CBarcodeKeyboardType();

	CString GetBarcodeString();
	void SetInitBarcode(){m_strBarcode.Format(_T("-"));};

	BOOL SetBarcodeChar(BYTE byChar);
	BOOL GetBarcodeEnd(){return m_bBarcodEndChar;};
protected:

};
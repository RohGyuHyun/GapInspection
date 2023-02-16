#pragma once

#define AUTO_DELETE_THREAD_WAIT_TIME			1000//ms
#define AUTO_DELETE_THREAD_TIME_OUT				5000//ms

#define DELETE_FULL_PATH_SEARCH_YEAR_CNT		5
#define DELETE_FULL_PATH_SEARCH_MONTH_CNT		12
#define DELETE_FULL_PATH_SEARCH_DAY_CNT			31

typedef struct DirAutoDelPara
{
	int nSaveDay;
	int nAutoDeleHour;
	int nAutoDelMinute;
	CString strDelPath;
	BOOL bDriveFreeSizeAutoDel;
	double dDelDrivePercent;
	BOOL bManualDriveFreeSizeAutoDel;
}typeDirAutoDelPara;

class CAutoDelete
{
public:
	//typeDirAutoDelPara.strDelPath\\Year(0000)\\Month(00)\\Day(00) 형식의 경로만 해당
	CAutoDelete(typeDirAutoDelPara DirAutoDelPara);
	~CAutoDelete();

private:
	BOOL m_bFirst;
	BOOL m_bDelete;
	int m_iDel_Year;
	int m_iDel_Month;
	int m_iDel_Day;
	int m_iDel_RecentDay;
	CString m_strImgPath;

	CWinThread *m_pAutoDeleteThread;
	BOOL m_bAutoDeleteStart;
	BOOL m_bAutoDeleteEnd;
	BOOL m_bAutoDeleteWait;

	typeDirAutoDelPara m_DirAutoDelPara;
public:
	void SetAutoDelPara(typeDirAutoDelPara DirAutoDelPara){m_DirAutoDelPara = DirAutoDelPara;};
	typeDirAutoDelPara GetAutoDelPara(){return m_DirAutoDelPara;};
	void SetFirst(BOOL isFirst){m_bFirst = isFirst;};
	void SetAutoDeleteThreadWait(BOOL isWait);
	void SetManualDriveFreeSizeAutoDeleteStart(BOOL isStart);
protected:
	void InitThread();
	void ReleaseThread();
	BOOL WaitThreadEnd();
	static UINT AutoDeleteThread(LPVOID pParam);
	void ThreadAutoDelete();
	BOOL DeleteFolder(CString strFolder);
	BOOL DeleteFolderSearch(CString path);

	void ThreadSaveDayAutoDelete();
	BOOL StartSaveDayAutoDelete_Img();
	BOOL CheckSaveDayDeleteDate();
	
	void ThreadDriveFreeSizeAutoDelete();
	BOOL StartDriveFreeSizeAutoDelete_Img();
	BOOL FolderSearch(CString path);
};
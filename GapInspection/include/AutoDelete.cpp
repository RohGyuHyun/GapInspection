#include "stdafx.h"
#include "AutoDelete.h"


CAutoDelete::CAutoDelete(typeDirAutoDelPara DirAutoDelPara)
{
	m_DirAutoDelPara = DirAutoDelPara;
	m_bFirst = TRUE;

	if(!m_DirAutoDelPara.bDriveFreeSizeAutoDel)
	{
		m_DirAutoDelPara.bManualDriveFreeSizeAutoDel = FALSE;
	}

	InitThread();
}

CAutoDelete::~CAutoDelete()
{
	ReleaseThread();
}

void CAutoDelete::InitThread()
{
	m_bAutoDeleteWait = FALSE;
	m_bAutoDeleteEnd = FALSE;

	if(m_DirAutoDelPara.bManualDriveFreeSizeAutoDel)
	{
		m_bAutoDeleteStart = FALSE;
	}
	else
	{
		m_bAutoDeleteStart = TRUE;
	}

	m_pAutoDeleteThread = AfxBeginThread(AutoDeleteThread, this, THREAD_PRIORITY_NORMAL);
	m_pAutoDeleteThread->m_bAutoDelete = FALSE;
	m_pAutoDeleteThread->ResumeThread();
}

void CAutoDelete::ReleaseThread()
{
	m_bAutoDeleteWait = FALSE;
	m_bAutoDeleteStart = FALSE;
	m_bAutoDeleteEnd = TRUE;

	if(WaitThreadEnd())
	{
		if(m_pAutoDeleteThread)
			delete m_pAutoDeleteThread;

		m_pAutoDeleteThread = NULL;
	}
}

BOOL CAutoDelete::WaitThreadEnd()
{
	BOOL rslt = TRUE;
	if(m_pAutoDeleteThread != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pAutoDeleteThread->m_hThread, AUTO_DELETE_THREAD_TIME_OUT);

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

BOOL CAutoDelete::CheckSaveDayDeleteDate()
{
	CString str;
	CTime now_time = CTime::GetCurrentTime();
	int temp_year, temp_month, temp_day;
	CTime delete_time;
	int TempDeleteDay=0;
	int i=0;
	temp_year=0;
	temp_month=0;
	temp_day=0;

	temp_day = now_time.GetDay();
	temp_month = now_time.GetMonth();
	temp_year = now_time.GetYear();

	int save_day = m_DirAutoDelPara.nSaveDay;
	if( (temp_day - save_day) < 0)
	{
		if(now_time.GetMonth() == 2 || now_time.GetMonth() == 4 || now_time.GetMonth() == 6 ||
			now_time.GetMonth() == 8 || now_time.GetMonth() == 9 || now_time.GetMonth() == 11)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 5 || now_time.GetMonth() == 7 || now_time.GetMonth() == 10 || now_time.GetMonth() == 12)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 30 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 3)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 28 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
	}
	else if( (temp_day - save_day) == 0)
	{
		if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 0;
		}
		else
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 0;
		}
	}
	else
	{
		m_iDel_Year = now_time.GetYear();
		m_iDel_Month = now_time.GetMonth();
		m_iDel_RecentDay = temp_day - save_day;
	}
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  저장 기간을 개월로 했을 경우
	int save_month = m_dlgSetup->m_SysPara.m_iPara[0];
	if( (temp_month - save_month) < 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		for(i = 0; i <= (save_month - temp_month); i++)
		{
			if(temp_month == i+1)
			{
				m_iDel_Month = 12 - (save_month - temp_month);
			}
		}
	}
	else if( (temp_month - save_month) == 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		m_iDel_Month = 12;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		m_iDel_Month = temp_month - save_month;
	}
*/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	/*  //저장 기간 3개월 일 경우
	if( (temp_month - 2) <= 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		if(temp_month == 1)
			m_iDel_Month = 10;
		else if(temp_month == 2)
			m_iDel_Month = 11;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		if(temp_month == 3)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
		}
		else
			m_iDel_Month = temp_month - 3;
	}
	*/
	return TRUE;
}

BOOL CAutoDelete::DeleteFolderSearch(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				return FALSE;
			}
		}
	}
	ff.Close();

	return TRUE;

}

BOOL CAutoDelete::DeleteFolder(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				DeleteFolder(Filename);
				RemoveDirectory(Filename);
			}
			else
			{
				DeleteFile(Filename);
			}
		}
	}
	ff.Close();

	return TRUE;
}

BOOL CAutoDelete::StartSaveDayAutoDelete_Img()
{
	CString Delete_Img_dir,Delete_Data_dir,Delete_Ng_Data_dir,Delete_Tab_Data_dir;
	CString Delete_Product_dir,Delete_Spc_dir;
	CTime now_time = CTime::GetCurrentTime();
	BOOL bDelFolder[31];

	m_strImgPath.Format(_T("%s"), m_DirAutoDelPara.strDelPath);;
	
	CheckSaveDayDeleteDate();

	int nYear = m_iDel_Year;
	int nDelYear = now_time.GetYear();

	if(1 == now_time.GetMonth() && now_time.GetDay() < m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year;
	}
	else if(1 == now_time.GetMonth() && now_time.GetDay() > m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year - 1;
	}

	for(int nYear = m_iDel_Year; nYear >= nDelYear; nYear--)		// 110930 ytlee
	{
		if(nYear != now_time.GetYear())
		{
			m_iDel_Month = 12;
			m_iDel_Day = m_iDel_RecentDay;		// 120104 ytlee
		}

		for(int nMonth = m_iDel_Month; nMonth > 0; nMonth--)
		{
			if(nMonth == 1 || nMonth == 3 || nMonth == 5 ||
				nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12)		// 120104 ytlee
			{
				m_iDel_Day = 31;
			}
			else if(nMonth == 4 || nMonth == 6 || nMonth == 9 || nMonth == 11)
			{
				m_iDel_Day = 30;
			}
			else if(nMonth == 2)
			{
				m_iDel_Day = 28;
			}
			if(nYear == now_time.GetYear() && (nMonth == now_time.GetMonth() || m_DirAutoDelPara.nSaveDay > now_time.GetDay()))		// 110901 ytlee
			{
				m_iDel_Day = m_iDel_RecentDay;
			}
			else
			{
				m_iDel_Day = 31;
			}

			for(int i = 0; i < 31; i++)
			{
				bDelFolder[i] = FALSE;
			}


			for(int nDay = 1; nDay <= m_iDel_Day; nDay++)
			{
				if(nMonth>9 && nDay>9)
					Delete_Img_dir.Format(_T("%s%04d\\%02d\\%02d"),m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay>9)
					Delete_Img_dir.Format(_T("%s%04d\\%02d\\%02d"),m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth>9 && nDay<10)
					Delete_Img_dir.Format(_T("%s%04d\\%02d\\%02d"),m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay<10)
					Delete_Img_dir.Format(_T("%s%04d\\%02d\\%02d"),m_strImgPath, nYear, nMonth, nDay);

				CFileFind dbfile;

				if(dbfile.FindFile(Delete_Img_dir, 0))
				{
					if(DeleteFolder(Delete_Img_dir))
						RemoveDirectory(Delete_Img_dir);
				}

				if(DeleteFolderSearch(Delete_Img_dir))
				{
					bDelFolder[nDay - 1] = TRUE;
				}
				else
				{
					bDelFolder[nDay - 1] = FALSE;
				}
			}
		}
	}

	return TRUE;
}

void CAutoDelete::SetAutoDeleteThreadWait(BOOL isWait)
{
	m_bAutoDeleteStart = !isWait;
	m_bAutoDeleteWait = isWait;
}


void CAutoDelete::ThreadAutoDelete()
{
	BOOL bFreeDel = FALSE;
	if(m_DirAutoDelPara.bDriveFreeSizeAutoDel)
		bFreeDel = TRUE;

	while(TRUE)
	{
		if(m_bAutoDeleteWait)
		{
			Sleep(AUTO_DELETE_THREAD_WAIT_TIME);
			m_bAutoDeleteWait = FALSE;
			m_bAutoDeleteStart = TRUE;
		}

		if(m_bAutoDeleteEnd)
		{
			m_bAutoDeleteWait = FALSE;
			m_bAutoDeleteStart = FALSE;
			m_bAutoDeleteEnd = FALSE;
			break;
		}

		if(m_bAutoDeleteStart)
		{
			if(!m_DirAutoDelPara.bDriveFreeSizeAutoDel)
			{
				ThreadSaveDayAutoDelete();
			}
			else
			{
				ThreadDriveFreeSizeAutoDelete();
			}
		}

		if(bFreeDel)
		{
			m_bFirst = FALSE;
			m_bDelete = FALSE;
			ThreadDriveFreeSizeAutoDelete();
		}

		Sleep(1);
	}
}

void CAutoDelete::ThreadSaveDayAutoDelete()
{
	SYSTEMTIME	lpSystemTime;

	GetLocalTime(&lpSystemTime);
	if(m_bFirst)
	{
		m_bFirst = FALSE;
		StartSaveDayAutoDelete_Img();
		m_bDelete = TRUE;
	}
	else if((m_DirAutoDelPara.nAutoDeleHour == lpSystemTime.wHour) && (m_DirAutoDelPara.nAutoDelMinute <= lpSystemTime.wMinute) && !m_bDelete && !m_bFirst)
	{
		StartSaveDayAutoDelete_Img();
		m_bDelete = TRUE;
	}
	else if((m_DirAutoDelPara.nAutoDelMinute != lpSystemTime.wMinute) && m_bDelete && !m_bFirst)
	{
		m_bDelete = FALSE;
	}
}

void CAutoDelete::SetManualDriveFreeSizeAutoDeleteStart(BOOL isStart)
{
	if(m_DirAutoDelPara.bDriveFreeSizeAutoDel && m_DirAutoDelPara.bManualDriveFreeSizeAutoDel)
		m_bAutoDeleteStart = isStart;
}

void CAutoDelete::ThreadDriveFreeSizeAutoDelete()
{
	if(m_DirAutoDelPara.bManualDriveFreeSizeAutoDel)
		m_bAutoDeleteStart = FALSE;

	SYSTEMTIME	lpSystemTime;

	GetLocalTime(&lpSystemTime);
	//if((m_DirAutoDelPara.nAutoDeleHour == lpSystemTime.wHour) && (m_DirAutoDelPara.nAutoDelMinute == lpSystemTime.wMinute) && !m_bDelete)// && !m_bFirst)
	{
		CString strDriveName;
		double dDrivePercent;
		strDriveName.Format(_T("%s"), m_DirAutoDelPara.strDelPath.Left(1));
		dDrivePercent = UseDriveFreeSize(strDriveName, USE_DRIVE_FREE_SIZE_RETURN_VAL_PERCENT);

		if(dDrivePercent < (100 - m_DirAutoDelPara.dDelDrivePercent))
		{
			StartDriveFreeSizeAutoDelete_Img();
			m_bDelete = TRUE;
		}
	}
}

BOOL CAutoDelete::FolderSearch(CString path)
{
	BOOL bRslt = FALSE;
	CFileFind ff;
	CString strTarget = path;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		bRslt = TRUE;
	}
	ff.Close();

	return bRslt;
}

BOOL CAutoDelete::StartDriveFreeSizeAutoDelete_Img()
{
	BOOL rslt = TRUE;
	BOOL bSearchPath[DELETE_FULL_PATH_SEARCH_YEAR_CNT][DELETE_FULL_PATH_SEARCH_MONTH_CNT][DELETE_FULL_PATH_SEARCH_DAY_CNT] = {FALSE, };
	CString strMainDeletePath;
	BOOL bDeletePath = FALSE;
	BOOL bMonthDelete = FALSE;
	BOOL bYearDelete = FALSE;
	int nLastDelMonth = -1;
	int nNotDelCount = this->m_DirAutoDelPara.nSaveDay;
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	
	for(int nYear = 0; nYear < DELETE_FULL_PATH_SEARCH_YEAR_CNT; nYear++)
	{
		for(int nMonth = 0; nMonth < DELETE_FULL_PATH_SEARCH_MONTH_CNT; nMonth++)
		{
			for(int nDay = 0; nDay < DELETE_FULL_PATH_SEARCH_DAY_CNT; nDay++)
			{
				strMainDeletePath.Format(_T("%s%04d\\%02d\\%02d"), m_DirAutoDelPara.strDelPath, 2019 + nYear, nMonth + 1, nDay + 1);

				if(FolderSearch(strMainDeletePath))
					bSearchPath[nYear][nMonth][nDay] = TRUE;
			}
		}
	}

	for(int nYear = 0; nYear < DELETE_FULL_PATH_SEARCH_YEAR_CNT; nYear++)
	{
		for(int nMonth = 0; nMonth < DELETE_FULL_PATH_SEARCH_MONTH_CNT; nMonth++)
		{
			for(int nDay = 0; nDay < DELETE_FULL_PATH_SEARCH_DAY_CNT; nDay++)
			{
				strMainDeletePath.Format(_T("%s%04d\\%02d\\%02d"), m_DirAutoDelPara.strDelPath, 2019 + nYear, nMonth + 1, nDay + 1);

				if(FolderSearch(strMainDeletePath))
				{
					bSearchPath[nYear][nMonth][nDay] = TRUE;
				}

				if(bSearchPath[nYear][nMonth][nDay])
				{
					if(DeleteFolder(strMainDeletePath))
					{
						RemoveDirectory(strMainDeletePath);
					}

					bSearchPath[nYear][nMonth][nDay] = FALSE;
					bDeletePath = TRUE;
					break;
				}
			}
			if(bDeletePath)
				break;
		}
		if(bDeletePath)
			break;
	}		

	for(int nYear = 0; nYear < DELETE_FULL_PATH_SEARCH_YEAR_CNT; nYear++)
	{
		bYearDelete = FALSE;
		for(int nMonth = 0; nMonth < DELETE_FULL_PATH_SEARCH_MONTH_CNT; nMonth++)
		{
			for(int nDay = 0; nDay < DELETE_FULL_PATH_SEARCH_DAY_CNT; nDay++)
			{
				if(bSearchPath[nYear][nMonth][nDay])
				{
					bMonthDelete = TRUE;
					break;
				}
			}
			if(!bMonthDelete)
			{
				strMainDeletePath.Format(_T("%s%04d\\%02d"), m_DirAutoDelPara.strDelPath, 2019 + nYear, nMonth + 1);
				if(FolderSearch(strMainDeletePath))
				{
					if(DeleteFolder(strMainDeletePath))
					{
						RemoveDirectory(strMainDeletePath);
					}
					bYearDelete = TRUE;
					nLastDelMonth = nMonth + 1;
				}
			}
		}

		if(!bMonthDelete && bYearDelete && nLastDelMonth == 12)
		{
			strMainDeletePath.Format(_T("%s%04d"), m_DirAutoDelPara.strDelPath, 2019 + nYear);
			if(FolderSearch(strMainDeletePath))
			{
				if(DeleteFolder(strMainDeletePath))
				{
					RemoveDirectory(strMainDeletePath);
				}
			}
			break;
		}
	}

	return rslt;
}

UINT CAutoDelete::AutoDeleteThread(LPVOID pParam)
{
	CAutoDelete *pdlg = (CAutoDelete *)pParam;
	
	pdlg->ThreadAutoDelete();
	
	return 0;
}
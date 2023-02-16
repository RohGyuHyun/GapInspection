// @file ProfileDataIF.cpp: implementation of the CProfileDataIF class.
//
//_____________________________________________________________________________

#include "stdafx.h"
#include "ProfileDataIF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//_____________________________________________________________________________
// Construction/Destruction
CProfileDataIF::CProfileDataIF()
{
}

CProfileDataIF::~CProfileDataIF()
{
}

//_____________________________________________________________________________
// file Read function
CString CProfileDataIF::GetString(CString node, CString key, CString default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
#ifdef _UNICODE
		GetPrivateProfileStringW(node, key, default_value, (LPWSTR)buff, sizeof(buff), m_sFilePath);
#else
		GetPrivateProfileString(node, key, default_value, (LPSTR)buff, sizeof(buff), m_sFilePath);
#endif
		tmp_s.Format(_T("%s"), buff); 
		return tmp_s;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

int CProfileDataIF::GetInt(CString node, CString key, int default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%d"), default_value);
#ifdef _UNICODE
		GetPrivateProfileStringW(node, key, tmp_s, (LPWSTR)buff, sizeof(buff), m_sFilePath);
#else
		GetPrivateProfileString(node, key, tmp_s, (LPSTR)buff, sizeof(buff), m_sFilePath);
#endif
		return _ttoi(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

long CProfileDataIF::GetLong(CString node, CString key, long default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%ld"), default_value);
#ifdef _UNICODE
		GetPrivateProfileStringW(node, key, tmp_s, (LPWSTR)buff, sizeof(buff), m_sFilePath);
#else
		GetPrivateProfileString(node, key, tmp_s, (LPSTR)buff, sizeof(buff), m_sFilePath);
#endif
		return _ttoi(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

double CProfileDataIF::GetDouble(CString node, CString key, double default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%f"), default_value);
#ifdef _UNICODE
		GetPrivateProfileStringW(node, key, tmp_s, (LPWSTR)buff, sizeof(buff), m_sFilePath);
#else
		GetPrivateProfileString(node, key, tmp_s, (LPSTR)buff, sizeof(buff), m_sFilePath);
#endif
		return _ttof(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

BOOL CProfileDataIF::SetString(CString node, CString key, CString s_data)
{
	try
	{
		WritePrivateProfileString(node, key, s_data, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}


BOOL CProfileDataIF::SetInt(CString node, CString key, int i_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%d"), i_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::SetLong(CString node, CString key, long l_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%ld"), l_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::SetDouble(CString node, CString key, double d_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%f"), d_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}


BOOL CProfileDataIF::GetSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize)
{
	try
	{
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, m_sFilePath);
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::DeleteSection(CString node)
{
	try
	{
		// delete section
		WritePrivateProfileString(node, NULL, _T(""), m_sFilePath);
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::IsExistSection(CString node)
{
	CString tmp_s;
	char buff[1000];

	try
	{
#ifdef _UNICODE
		if (GetPrivateProfileSection(node, (LPWSTR)buff, sizeof(buff), m_sFilePath) > 0)
#else
		if (GetPrivateProfileSection(node,  (LPSTR)buff, sizeof(buff), m_sFilePath) > 0)
#endif
			return TRUE;
		else
			return FALSE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}
//_____________________________________________________________________________
// file function
// Language에 따라 언어 반환하기 [2014.10.20 by LJY]
CString CProfileDataIF::GetLanguageMessage(CString sFindData)
{
	CString tmp_s(_T("")), default_value(_T("No_Data,No_Data"));
	char buff[200];

	try
	{
#ifdef _UNICODE
		GetPrivateProfileStringW(_T("Message Display"), sFindData, default_value, (LPWSTR)buff, sizeof(buff), m_sFilePath);
#else
		GetPrivateProfileString(_T("Message Display"), sFindData, default_value, (LPSTR)buff, sizeof(buff), m_sFilePath);
#endif
		tmp_s.Format(_T("%s"), buff); 
		return tmp_s;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}


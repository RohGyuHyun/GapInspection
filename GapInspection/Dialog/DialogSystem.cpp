// DialogSystem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GapInspection.h"
#include "DialogSystem.h"
#include "afxdialogex.h"


// CDialogSystem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogSystem, CDialogEx)

CDialogSystem::CDialogSystem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSystem::IDD, pParent)
{
	m_SystemPara.bRsltImageSave = FALSE;
	m_SystemPara.bInspImageSave = FALSE;
	m_SystemPara.dPixelSizeWidth = 1.67;
	m_SystemPara.dPixelSizeHeight = 1.67;
	m_SystemPara.strUseModelName.Format(_T(""));
	m_SystemPara.bTrace = FALSE;
	m_SystemPara.strDrive.Format(_T("D"));
	m_SystemPara.DirAutoDelPara.strDelPath.Format(_T("%s%s"), m_SystemPara.strDrive, RSLT_IMAGE_FILE_PATH);
	m_SystemPara.nAvgRsltCnt = 30;
	m_SystemPara.nSpreadAlarmCheckCnt = 5;
	m_SystemPara.ServerPara.strIP.Format(_T("192.168.100.1"));
	m_SystemPara.ServerPara.nPortNum = 9000;
	m_SystemPara.nAlgorithmIdx = 0;
	m_SystemPara.bVIPMode = FALSE;
	m_SystemPara.bBarcodeUse = TRUE;

	m_SystemPara.DirAutoDelPara.bDriveFreeSizeAutoDel = 0;//FREE_SIZE_AUTO_DEL;
	m_SystemPara.DirAutoDelPara.dDelDrivePercent = AUTO_DEL_DRIVE_SIZE_PER;
	m_SystemPara.DirAutoDelPara.bManualDriveFreeSizeAutoDel = FREE_SIZE_AUTO_DEL;
}

CDialogSystem::~CDialogSystem()
{
	if(m_dlgDriveSelect != NULL)
		delete m_dlgDriveSelect;

	if(m_dlgAlgorithmSelect != NULL)
		delete m_dlgAlgorithmSelect;
}

void CDialogSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL2, m_lbxListBox);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_SELECT_MODEL, m_ctrlWorkModelName);
}


BEGIN_MESSAGE_MAP(CDialogSystem, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_MODEL2, &CDialogSystem::OnLbnSelchangeListModel2)
END_MESSAGE_MAP()


// CDialogSystem 메시지 처리기입니다.
TypeSystemPara CDialogSystem::GetSystemPara()
{
	TypeSystemPara SystemPara;
	SystemPara.bVIPMode = m_SystemPara.bVIPMode;
	SystemPara.bRsltImageSave = m_SystemPara.bRsltImageSave;
	SystemPara.bRsltNGImageSave = m_SystemPara.bRsltNGImageSave;
	SystemPara.bInspImageSave = m_SystemPara.bInspImageSave;
	SystemPara.dPixelSizeWidth = m_SystemPara.dPixelSizeWidth;
	SystemPara.dPixelSizeHeight = m_SystemPara.dPixelSizeHeight;
	SystemPara.strUseModelName.Format(_T("%s"), m_SystemPara.strUseModelName);
	SystemPara.isUsePW = m_SystemPara.isUsePW;
	SystemPara.bTrace = m_SystemPara.bTrace;
	SystemPara.DirAutoDelPara.nSaveDay = m_SystemPara.DirAutoDelPara.nSaveDay;
	SystemPara.DirAutoDelPara.nAutoDeleHour = m_SystemPara.DirAutoDelPara.nAutoDeleHour;
	SystemPara.DirAutoDelPara.nAutoDelMinute = m_SystemPara.DirAutoDelPara.nAutoDelMinute;
	SystemPara.strDrive.Format(_T("%s"), m_SystemPara.strDrive);
	SystemPara.DirAutoDelPara.strDelPath.Format(_T("%s%s"), m_SystemPara.strDrive, RSLT_IMAGE_FILE_PATH);
	SystemPara.nAvgRsltCnt = m_SystemPara.nAvgRsltCnt;
	SystemPara.nSpreadAlarmCheckCnt = m_SystemPara.nSpreadAlarmCheckCnt;
	SystemPara.ServerPara.strIP.Format(_T("%s"), m_SystemPara.ServerPara.strIP);
	SystemPara.ServerPara.nPortNum = m_SystemPara.ServerPara.nPortNum;

	SystemPara.DirAutoDelPara.bDriveFreeSizeAutoDel = 0;//FREE_SIZE_AUTO_DEL;
	SystemPara.DirAutoDelPara.dDelDrivePercent = AUTO_DEL_DRIVE_SIZE_PER;
	SystemPara.DirAutoDelPara.bManualDriveFreeSizeAutoDel = FREE_SIZE_AUTO_DEL;

	SystemPara.bBarcodeUse = m_SystemPara.bBarcodeUse;

	return m_SystemPara;
}

void CDialogSystem::SetSystemPara(TypeSystemPara SystemPara)
{
	m_SystemPara.bVIPMode = SystemPara.bVIPMode;
	m_SystemPara.bRsltImageSave = SystemPara.bRsltImageSave;
	m_SystemPara.bRsltNGImageSave = SystemPara.bRsltNGImageSave;
	m_SystemPara.bInspImageSave = SystemPara.bInspImageSave;
	m_SystemPara.dPixelSizeWidth = SystemPara.dPixelSizeWidth;
	m_SystemPara.dPixelSizeHeight = SystemPara.dPixelSizeHeight;
	m_SystemPara.strUseModelName.Format(_T("%s"), SystemPara.strUseModelName);
	m_SystemPara.isUsePW = SystemPara.isUsePW;
	m_SystemPara.bTrace = SystemPara.bTrace;
	m_SystemPara.DirAutoDelPara.nSaveDay = SystemPara.DirAutoDelPara.nSaveDay;
	m_SystemPara.DirAutoDelPara.nAutoDeleHour = SystemPara.DirAutoDelPara.nAutoDeleHour;
	m_SystemPara.DirAutoDelPara.nAutoDelMinute = SystemPara.DirAutoDelPara.nAutoDelMinute;
	m_SystemPara.strDrive.Format(_T("%s"), SystemPara.strDrive);
	m_SystemPara.DirAutoDelPara.strDelPath.Format(_T("%s%s"), SystemPara.strDrive, RSLT_IMAGE_FILE_PATH);
	m_SystemPara.nAvgRsltCnt = SystemPara.nAvgRsltCnt;
	m_SystemPara.nSpreadAlarmCheckCnt = SystemPara.nSpreadAlarmCheckCnt;

	m_SystemPara.DirAutoDelPara.bDriveFreeSizeAutoDel = 0;//FREE_SIZE_AUTO_DEL;
	m_SystemPara.DirAutoDelPara.dDelDrivePercent = AUTO_DEL_DRIVE_SIZE_PER;
	m_SystemPara.DirAutoDelPara.bManualDriveFreeSizeAutoDel = FREE_SIZE_AUTO_DEL;

	m_SystemPara.bBarcodeUse = SystemPara.bBarcodeUse;
}

void CDialogSystem::UpdatePara(BOOL isUpdate)
{
	CString strText;

	if(isUpdate)
	{
		UpdateData(TRUE);
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_0);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bRsltImageSave = TRUE;
		}
		else
		{
			m_SystemPara.bRsltImageSave = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_12);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bRsltNGImageSave = TRUE;
		}
		else
		{
			m_SystemPara.bRsltNGImageSave = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_2);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bInspImageSave = TRUE;
		}
		else
		{
			m_SystemPara.bInspImageSave = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_4);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.dPixelSizeWidth = _wtof(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_5);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.dPixelSizeHeight = _wtof(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_6);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bTrace = TRUE;
		}
		else
		{
			m_SystemPara.bTrace = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_8);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.DirAutoDelPara.nSaveDay = _wtof(strText) + 1;

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_9);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.DirAutoDelPara.nAutoDeleHour = _wtof(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_10);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.DirAutoDelPara.nAutoDelMinute = _wtof(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_11);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.strDrive.Format(_T("%s"), strText.Left(1));
		m_SystemPara.DirAutoDelPara.strDelPath.Format(_T("%s%s"), m_SystemPara.strDrive, RSLT_IMAGE_FILE_PATH);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_14);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.nAvgRsltCnt = _wtoi(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_15);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.nSpreadAlarmCheckCnt= _wtoi(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_16);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.ServerPara.strIP.Format(_T("%s"), strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_17);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.ServerPara.nPortNum= _wtoi(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_18);
		pBtnEnh->GetWindowTextW(strText);
		if(!strText.Compare(_T("Winner")))
		{
			m_SystemPara.nAlgorithmIdx = 0;
		}
		else if(!strText.Compare(_T("Bloom")))
		{
			m_SystemPara.nAlgorithmIdx = 1;
		}
		else
		{
			m_SystemPara.nAlgorithmIdx = 2;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_20);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bVIPMode = TRUE;
		}
		else
		{
			m_SystemPara.bVIPMode = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_22);
		if(pBtnEnh->GetValue())
		{
			m_SystemPara.bBarcodeUse = TRUE;
		}
		else
		{
			m_SystemPara.bBarcodeUse = FALSE;
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_25);
		pBtnEnh->GetWindowTextW(strText);
		m_SystemPara.nLightOnOffWaitTime= _wtoi(strText);
	}
	else
	{
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_0);
		if(m_SystemPara.bRsltImageSave)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_1);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_1);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_12);
		if(m_SystemPara.bRsltNGImageSave)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_13);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_13);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_2);
		if(m_SystemPara.bInspImageSave)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_3);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_3);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_4);
		strText.Format(_T("%0.3f"), m_SystemPara.dPixelSizeWidth);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_5);
		strText.Format(_T("%0.3f"), m_SystemPara.dPixelSizeHeight);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_6);
		if(m_SystemPara.bTrace)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_7);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_7);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_8);
		strText.Format(_T("%02d"), m_SystemPara.DirAutoDelPara.nSaveDay - 1);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_9);
		strText.Format(_T("%02d"), m_SystemPara.DirAutoDelPara.nAutoDeleHour);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_10);
		strText.Format(_T("%02d"), m_SystemPara.DirAutoDelPara.nAutoDelMinute);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_11);
		strText.Format(_T("%s:\\"), m_SystemPara.strDrive);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_14);
		strText.Format(_T("%d"), m_SystemPara.nAvgRsltCnt);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_15);
		strText.Format(_T("%d"), m_SystemPara.nSpreadAlarmCheckCnt);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_16);
		strText.Format(_T("%s"), m_SystemPara.ServerPara.strIP);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_17);
		strText.Format(_T("%d"), m_SystemPara.ServerPara.nPortNum);
		pBtnEnh->SetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_18);
		if(m_SystemPara.nAlgorithmIdx == 0)
		{
			strText.Format(_T("Winner"));
		}
		else if(m_SystemPara.nAlgorithmIdx == 1)
		{
			strText.Format(_T("Bloom"));
		}
		else
		{
			strText.Format(_T("Empty"));
		}
		pBtnEnh->GetWindowTextW(strText);

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_20);
		if(m_SystemPara.bVIPMode)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_21);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_21);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_22);
		if(m_SystemPara.bBarcodeUse)
		{
			pBtnEnh->SetValue(TRUE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_23);
			pBtnEnh->SetValue(FALSE);
		}
		else
		{
			pBtnEnh->SetValue(FALSE);
			pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_23);
			pBtnEnh->SetValue(TRUE);
		}

		pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_25);
		strText.Format(_T("%d"), m_SystemPara.nLightOnOffWaitTime);
		pBtnEnh->SetWindowTextW(strText);
		UpdateData(FALSE);
	}
}

void CDialogSystem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	
	if(bShow)
	{
		LoadSystemPara();
		DisplayModelList();
		DisplayModelName();
		UpdatePara(FALSE);
		m_dlgAlgorithmSelect->SetAlgorithm(m_SystemPara.nAlgorithmIdx);
		m_strAlgorithm.Format(_T("%s"), m_dlgAlgorithmSelect->GetAlgorithmNaem());
		CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_18);
		pBtnEnh->SetWindowTextW(m_strAlgorithm);
		if(!m_strAlgorithm.Compare(_T("Winner")))
		{
			m_SystemPara.nAlgorithmIdx = 0;
		}
		else if(!m_strAlgorithm.Compare(_T("Bloom")))
		{
			m_SystemPara.nAlgorithmIdx = 1;
		}
		else
		{
			m_SystemPara.nAlgorithmIdx = 2;
		}
	}
	else
	{

	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CDialogSystem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_dlgDriveSelect = new CDialogDriveSelect();
	m_dlgDriveSelect->Create(IDD_DIALOG_DIRECTORY_DRIVE_SELECT, this);

	m_dlgAlgorithmSelect = new CDialogAlgorithmSelect();
	m_dlgAlgorithmSelect->Create(IDD_DIALOG_ALGORITHM_SELECT, this);

	LoadSystemPara();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDialogSystem::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_EVENTSINK_MAP(CDialogSystem, CDialogEx)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_4, DISPID_CLICK, CDialogSystem::ClickPixelSizeW, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_5, DISPID_CLICK, CDialogSystem::ClickPixelSizeH, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSYTEM_APPLY, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSysytemApply, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_CANCEL, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemCancel, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_8, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara8, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_9, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara9, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_10, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara10, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_11, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara11, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_14, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara14, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_15, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara15, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_16, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara16, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_17, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara17, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_18, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara18, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_MODEL_ADD2, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemModelAdd2, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_MODEL_DEL2, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemModelDel2, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_MODEL_CHANGE2, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemModelChange2, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_24, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara24, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_PARA_25, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemPara25, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogSystem::ClickPixelSizeW()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	double dVal = 0.;

	CGetTextDlg	tdlg(30, _T(""), _T("Input Password"), NULL, TRUE);

	if(tdlg.DoModal() == IDCANCEL) 
		return;

	CString strPW = tdlg.GetStringValue();

	if(strPW.Compare(STR_PASS_WORD))
	{
		AfxMessageBox(_T("Password Fail"));
		return;
	}

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_4);
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtof(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtof(strText) < 1.)
		return;

	pBtnEnh->SetWindowTextW(strText);
}

void CDialogSystem::ClickPixelSizeH()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	double dVal = 0.;

	CGetTextDlg	tdlg(30, _T(""), _T("Input Password"), NULL, TRUE);

	if(tdlg.DoModal() == IDCANCEL) 
		return;

	CString strPW = tdlg.GetStringValue();

	if(strPW.Compare(STR_PASS_WORD))
	{
		AfxMessageBox(_T("Password Fail"));
		return;
	}

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_5);
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtof(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtof(strText) < 1.)
		return;

	pBtnEnh->SetWindowTextW(strText);
}

BOOL CDialogSystem::SaveSystemPara()
{
	BOOL rslt = TRUE;
	CString key, dat, app;
	CString file_path, temp_path, strLog;
	
	MakeDirectories(MODEL_FILE_PATH);
	file_path.Format(_T("%s"), SYSTEM_INFO_FILE);

	
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{

		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		app.Format(_T("SYSTEM_INFO"));
		dat.Empty();
		dat.Format(_T("%s"), m_SystemPara.strUseModelName);
		WritePrivateProfileString(app, _T("USE_MODEL_NAME"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%s"), m_SystemPara.strDrive);
		WritePrivateProfileString(app, _T("RSLT_DIRECTORY_DRIVE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%f"), m_SystemPara.dPixelSizeWidth);
		WritePrivateProfileString(app, _T("CAM_PIXEL_SIZE_WIDTH"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%f"), m_SystemPara.dPixelSizeHeight);
		WritePrivateProfileString(app, _T("CAM_PIXEL_SIZE_HEIGHT"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bInspImageSave);
		WritePrivateProfileString(app, _T("INSP_IMAGE_SAVE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bRsltImageSave);
		WritePrivateProfileString(app, _T("INSP_RSLT_OK_IMAGE_SAVE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bRsltNGImageSave);
		WritePrivateProfileString(app, _T("INSP_RSLT_NG_IMAGE_SAVE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bTrace);
		WritePrivateProfileString(app, _T("MANUAL_INSP_TRACE_VIEW"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.isUsePW);
		WritePrivateProfileString(app, _T("PW_USE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.DirAutoDelPara.nSaveDay);
		WritePrivateProfileString(app, _T("SAVED_RSLT_IMAGE_DAY"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.DirAutoDelPara.nAutoDeleHour);
		WritePrivateProfileString(app, _T("AUTO_DELETE_START_HOUR"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.DirAutoDelPara.nAutoDelMinute);
		WritePrivateProfileString(app, _T("AUTO_DELETE_START_MINUTE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.nAvgRsltCnt);
		WritePrivateProfileString(app, _T("AVERAGE_RESULT_CNT"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.nSpreadAlarmCheckCnt);
		WritePrivateProfileString(app, _T("SPREAD_ALARM_CHECK_CNT"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%s"), m_SystemPara.ServerPara.strIP);
		WritePrivateProfileString(app, _T("PLC_SERVER_IP"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.ServerPara.nPortNum);
		WritePrivateProfileString(app, _T("PLC_SERVER_PORT"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.nAlgorithmIdx);
		WritePrivateProfileString(app, _T("ALGORITHM_INDEX"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.nMetalAlgorithmIdx);
		WritePrivateProfileString(app, _T("METAL_ALGORITHM_INDEX"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bVIPMode);
		WritePrivateProfileString(app, _T("VIP_MODE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.bBarcodeUse);
		WritePrivateProfileString(app, _T("BARCODE_MODE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_SystemPara.nLightOnOffWaitTime);
		WritePrivateProfileString(app, _T("LIGHT_ON_OFF_WAIT_TIME"), dat, file_path);
	}

	return rslt;
}

BOOL CDialogSystem::LoadSystemPara()
{
	BOOL rslt = TRUE;
	CString key, dat, app, temp_default;
	CString file_path;

	file_path.Format(_T("%s"), SYSTEM_INFO_FILE);

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		app.Empty();
		app.Format(_T("SYSTEM_INFO"));
		dat.Empty();
		GetPrivateProfileString(app, _T("USE_MODEL_NAME"), _T("TEST"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.strUseModelName.Format(_T("%s"), dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("RSLT_DIRECTORY_DRIVE"), _T("D"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.strDrive.Format(_T("%s"), dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("CAM_PIXEL_SIZE_WIDTH"), _T("1.87"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.dPixelSizeWidth = _wtof(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("CAM_PIXEL_SIZE_HEIGHT"), _T("1.87"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.dPixelSizeHeight = _wtof(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_IMAGE_SAVE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bInspImageSave = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_RSLT_OK_IMAGE_SAVE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bRsltImageSave = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_RSLT_NG_IMAGE_SAVE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bRsltNGImageSave = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("MANUAL_INSP_TRACE_VIEW"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bTrace = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("USE_PW"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.isUsePW = _wtoi(dat);			

		dat.Empty();
		GetPrivateProfileString(app, _T("SAVED_RSLT_IMAGE_DAY"), _T("2"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.DirAutoDelPara.nSaveDay = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("AUTO_DELETE_START_HOUR"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.DirAutoDelPara.nAutoDeleHour = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("AUTO_DELETE_START_MINUTE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.DirAutoDelPara.nAutoDelMinute = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("AVERAGE_RESULT_CNT"), _T("30"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.nAvgRsltCnt = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("SPREAD_ALARM_CHECK_CNT"), _T("30"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.nSpreadAlarmCheckCnt = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("PLC_SERVER_IP"), _T("192.168.100.1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.ServerPara.strIP.Format(_T("%s"), dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("PLC_SERVER_PORT"), _T("9000"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.ServerPara.nPortNum = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("ALGORITHM_INDEX"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.nAlgorithmIdx = _wtoi(dat);
		
		dat.Empty();
		GetPrivateProfileString(app, _T("METAL_ALGORITHM_INDEX"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.nMetalAlgorithmIdx = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("VIP_MODE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bVIPMode = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("BARCODE_MODE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.bBarcodeUse = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("LIGHT_ON_OFF_WAIT_TIME"), _T("40"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_SystemPara.nLightOnOffWaitTime = _wtoi(dat);
	}

	return rslt;
}

void CDialogSystem::ClickBtnenhctrlSysytemApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdatePara(TRUE);
	SaveSystemPara();

	AfxGetMainWnd()->SendMessage(USE_MSG_SYSTEM_TO_MAIN_DIALOG, SYSTEM_TO_MAIN_APPLY_SYSTEM_PARA, 0);
	ShowWindow(SW_HIDE);
}

CString CDialogSystem::GetDGSModelName()
{
	CString strText;
	GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_24)->GetWindowTextW(strText);
	return strText;
}


void CDialogSystem::ClickBtnenhctrlSystemCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	this->ShowWindow(SW_HIDE);
}


void CDialogSystem::ClickBtnenhctrlSystemPara8()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	double dVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_8);
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtoi(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < -1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::ClickBtnenhctrlSystemPara9()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	double dVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_9);
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtoi(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < -1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::ClickBtnenhctrlSystemPara10()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	double dVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_10);
	pBtnEnh->GetWindowTextW(strText);
	dVal = _wtoi(strText);

	dlg.SetstrNum(dVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < -1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nEvent = int(nIDEvent);

	switch(nEvent)
	{
	case SYSTEM_TIMER_WINDOWS_VISIBLE:
		if(this->IsWindowVisible())
		{
			KillTimer(SYSTEM_TIMER_WINDOWS_VISIBLE);
			this->ShowWindow(SW_HIDE);
		}
		break;
	case SYSTEM_TIMER_DRIVE_SELECT:
		if(m_dlgDriveSelect->GetDriveSelect() && !m_dlgDriveSelect->IsWindowVisible())
		{
			CString strDrive;
			KillTimer(SYSTEM_TIMER_DRIVE_SELECT);
			m_strDrive.Format(_T("%s"), m_dlgDriveSelect->GetDriveNaem());
			strDrive.Format(_T("%s:\\"), m_strDrive);
			CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_11);
			pBtnEnh->SetWindowTextW(strDrive);
		}
		break;
	case SYSTEM_TIMER_ALGORITHM_SELECT:
		if(!m_dlgAlgorithmSelect->IsWindowVisible())
		{
			KillTimer(SYSTEM_TIMER_ALGORITHM_SELECT);
			m_strAlgorithm.Format(_T("%s"), m_dlgAlgorithmSelect->GetAlgorithmNaem());
			CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_18);
			pBtnEnh->SetWindowTextW(m_strAlgorithm);
			if(!m_strAlgorithm.Compare(_T("Winner")))
			{
				m_SystemPara.nAlgorithmIdx = 0;
			}
			else if(!m_strAlgorithm.Compare(_T("Bloom")))
			{
				m_SystemPara.nAlgorithmIdx = 1;
			}
			else
			{
				m_SystemPara.nAlgorithmIdx = 2;
			}
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDialogSystem::ClickBtnenhctrlSystemPara11()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetTimer(SYSTEM_TIMER_DRIVE_SELECT, 100, NULL);

	m_dlgDriveSelect->SetDriveName(m_SystemPara.strDrive);

	m_dlgDriveSelect->ShowWindow(SW_SHOW);
}

void CDialogSystem::ClickBtnenhctrlSystemPara14()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_14);
	pBtnEnh->GetWindowTextW(strText);
	nVal = _wtoi(strText);

	dlg.SetstrNum(nVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}

void CDialogSystem::ClickBtnenhctrlSystemPara15()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_15);
	pBtnEnh->GetWindowTextW(strText);
	nVal = _wtoi(strText);

	dlg.SetstrNum(nVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 0)
		return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::ClickBtnenhctrlSystemPara16()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetTextDlg	dlg(16, _T(""), _T("Input Server IP"), NULL, FALSE);
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_16);
	pBtnEnh->GetWindowTextW(strText);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetStringValue();
	nVal = strText.Find(_T("."));
	//if(nVal == 3)
	//	return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::ClickBtnenhctrlSystemPara17()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_17);
	pBtnEnh->GetWindowTextW(strText);
	nVal = _wtoi(strText);

	dlg.SetstrNum(nVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::DisplayModelList()
{
	CString strFilePath, strFileName, strName, strInch, strTemp;
	strFilePath = MODEL_FILE_PATH;
	strFileName = _T("*.*");
	BOOL isCompare = FALSE;
	CFileFind cFFind;	
	int nCount = 0;	
	m_lbxListBox.ResetContent();	
	strFilePath += strFileName;

	TRY { 
		BOOL bIsFile = cFFind.FindFile(strFilePath, 0);
		while(bIsFile)
		{
			bIsFile = cFFind.FindNextFile();
			strName = cFFind.GetFileName();
			if(strName != _T(".") && strName != _T("..") )
			{
				//strFilePath += strName;
				if(!strName.Right(3).Compare(_T("Ch1")) || !strName.Right(3).Compare(_T("Ch2")))
				{
					strTemp.Format(_T("%s"), strName.Mid(0, strName.GetLength() - 4));
					strName.Format(_T("%s"), strTemp);
				}

				isCompare = FALSE;
				for(int i = 0; i < m_lbxListBox.GetCount(); i++)
				{
					m_lbxListBox.GetText(i, strTemp);
					if(!strName.Compare(strTemp))
					{
						isCompare = TRUE;
						break;
					}
				}
				
				if(!isCompare)
					m_lbxListBox.AddString(strName);
			}
			Delay(2,TRUE);
		}
	}

	CATCH (CException, e)
	{
		cFFind.Close();
		e->ReportError();
		e->Delete();
		return;
	}
	END_CATCH

		cFFind.Close();
}

void CDialogSystem::DisplayModelName()
{
	m_ctrlWorkModelName.SetCaption(m_SystemPara.strUseModelName);
}

void CDialogSystem::ClickBtnenhctrlSystemPara18()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetTimer(SYSTEM_TIMER_ALGORITHM_SELECT, 100, NULL);

	m_dlgAlgorithmSelect->SetAlgorithm(m_SystemPara.nAlgorithmIdx);

	m_dlgAlgorithmSelect->ShowWindow(SW_SHOW);
}


void CDialogSystem::ClickBtnenhctrlSystemModelAdd2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetTextDlg	dlg(30, _T(""), _T("Input New Model Name"), NULL, FALSE);

	if(dlg.DoModal() == IDCANCEL) return;
	CString strModelName = dlg.GetStringValue();

	m_strSelectListModel = strModelName;
	AfxGetMainWnd()->SendMessage(USE_MSG_SYSTEM_TO_MAIN_DIALOG, SYSTEM_TO_MAIN_MODEL_ADD, 0);
	DisplayModelList();
}


void CDialogSystem::ClickBtnenhctrlSystemModelDel2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString str;
	CString strMsg; 

	CString cFile;
	CString strFilePath = MODEL_FILE_PATH + m_strSelectListModel;

	if(m_strSelectListModel == m_SystemPara.strUseModelName)
	{
		//g_cMsgView.OkMsgBox(_T("현재 사용 중인 Model은 삭제할 수 없습니다."));
		AfxMessageBox(_T("Use Model Not Delete."));
		return;
	}

	strMsg.Format(_T("Select Model Name < %s > is Delete?"), m_strSelectListModel);
	if(AfxMessageBox(strMsg, MB_YESNO) != IDYES)
	{
		return;
	}

	TRY
	{
		for(int i = 0; i < MAX_CHANNEL_CNT; i++)
		{
			strFilePath.Format(_T("%s%s_Ch%d"), MODEL_FILE_PATH, m_strSelectListModel, i + 1);
			DeleteFolderAndFile(strFilePath);
		}
	}
	CATCH( CFileException, e ) 
	{
		//g_cMsgView.OkMsgBox(_T("삭제 실패하였습니다."));
		return;
	}
	END_CATCH

	DisplayModelList();
}


void CDialogSystem::ClickBtnenhctrlSystemModelChange2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString strMsg;

	strMsg.Format(_T("Select Model Name < %s > is Change?"), m_strSelectListModel);
	if(AfxMessageBox(strMsg, MB_YESNO) != IDYES)
	{
		return;
	}
	
	m_SystemPara.strUseModelName.Format(_T("%s"), m_strSelectListModel);
	SaveSystemPara();
	DisplayModelName();

	AfxGetMainWnd()->SendMessage(USE_MSG_SYSTEM_TO_MAIN_DIALOG, SYSTEM_TO_MAIN_MODEL_CHANGE, 0);
}


void CDialogSystem::OnLbnSelchangeListModel2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str,strModel;
	
	m_nModelListBoxNo = m_lbxListBox.GetCurSel();

	if(m_nModelListBoxNo >= 0)
	{
		m_lbxListBox.GetText(m_nModelListBoxNo, str);

		m_strSelectListModel = str;
	}
}

void CDialogSystem::ClickBtnenhctrlSystemPara24()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetTextDlg	dlg(16, _T(""), _T("DGS Model Name Input"), NULL, FALSE);
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_24);
	pBtnEnh->GetWindowTextW(strText);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetStringValue();
	nVal = strText.Find(_T("."));
	//if(nVal == 3)
	//	return;

	pBtnEnh->SetWindowTextW(strText);
}


void CDialogSystem::ClickBtnenhctrlSystemPara25()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg dlg;
	CString strText;
	int nVal = 0.;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_SYSTEM_PARA_25);
	pBtnEnh->GetWindowTextW(strText);
	nVal = _wtoi(strText);

	dlg.SetstrNum(nVal);

	if(dlg.DoModal() != IDOK) return;

	strText = dlg.GetstrNum();
	if(_wtoi(strText) < 1)
		return;

	pBtnEnh->SetWindowTextW(strText);
}

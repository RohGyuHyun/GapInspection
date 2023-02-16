#pragma once

#define MAX_DRIVE_SELECT			10
// CDialogDriveSelect 대화 상자입니다.

class CDialogDriveSelect : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogDriveSelect)

public:
	CDialogDriveSelect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogDriveSelect();

private:
	CBtnenhctrl	m_cDriveSelect[MAX_DRIVE_SELECT];
	CString m_strDriveName;
	BOOL m_bSelect;
protected:


public:
	void SetDriveName(CString strDriveName){m_strDriveName.Format(_T("%s"), strDriveName);};
	CString GetDriveNaem(){return m_strDriveName;};
	BOOL GetDriveSelect(){return m_bSelect;};

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DIRECTORY_DRIVE_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void DriveSelect();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

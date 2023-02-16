#pragma once

#define MAX_DRIVE_SELECT			10
// CDialogDriveSelect ��ȭ �����Դϴ�.

class CDialogDriveSelect : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogDriveSelect)

public:
	CDialogDriveSelect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DIRECTORY_DRIVE_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void DriveSelect();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

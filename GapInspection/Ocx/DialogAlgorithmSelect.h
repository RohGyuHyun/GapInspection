#pragma once

#define MAX_DRIVE_SELECT			10
// CDialogDriveSelect ��ȭ �����Դϴ�.

class CDialogAlgorithmSelect : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlgorithmSelect)

public:
	CDialogAlgorithmSelect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogAlgorithmSelect();

private:
	CBtnenhctrl	m_cAlgorithmSelect[3];
	CString m_strAlgorithmName;
	BOOL m_bSelect;
	int m_nAlgorithmIndex;
protected:


public:
	void SetAlgorithm(int nAlgorithmIdx);
	int GetAlgorithm(){return m_nAlgorithmIndex;};
	CString GetAlgorithmNaem(){return m_strAlgorithmName;};
	BOOL GetAlgorithmSelect(){return m_bSelect;};

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ALGORITHM_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void AlgorithmSelect();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

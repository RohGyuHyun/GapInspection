#pragma once

#define MAX_DRIVE_SELECT			10
// CDialogDriveSelect 대화 상자입니다.

class CDialogAlgorithmSelect : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlgorithmSelect)

public:
	CDialogAlgorithmSelect(CWnd* pParent = NULL);   // 표준 생성자입니다.
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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ALGORITHM_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void AlgorithmSelect();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

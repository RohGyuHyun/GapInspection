#pragma once
#include "resource.h"

// CDialogAlarmMsg 대화 상자입니다.

class CDialogAlarmMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlarmMsg)

public:
	CDialogAlarmMsg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogAlarmMsg();

	CBtnenhctrl m_sMessage;
	CBtnenhctrl m_sCode;

	void SetMessage(char *code, char *msg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MSG_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlButtonBuzzerOff();
	void ClickBtnenhctrlButtonAlarmClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

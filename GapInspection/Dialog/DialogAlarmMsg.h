#pragma once
#include "resource.h"

// CDialogAlarmMsg ��ȭ �����Դϴ�.

class CDialogAlarmMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlarmMsg)

public:
	CDialogAlarmMsg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogAlarmMsg();

	CBtnenhctrl m_sMessage;
	CBtnenhctrl m_sCode;

	void SetMessage(char *code, char *msg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MSG_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlButtonBuzzerOff();
	void ClickBtnenhctrlButtonAlarmClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

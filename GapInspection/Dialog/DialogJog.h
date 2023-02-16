#pragma once
#define JOG_TIMER_UPDATE_CUR_POS		0

// CDialogJog ��ȭ �����Դϴ�.

class CDialogJog : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogJog)

public:
	CDialogJog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogJog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_JOG };

private:
	CBtnenhctrl m_ctrJogStep[MAX_MOTION_CNT];
	CBtnenhctrl m_ctrJogCurPos[MAX_MOTION_CNT];
	double m_dCurPos[MAX_MOTION_CNT];
	double m_dStepPitch[MAX_MOTION_CNT];
	BOOL m_bPlusMinuse;
public:
	double GetStepPitch(int nMotionIdx){return m_dStepPitch[nMotionIdx];};
	double GetCurPos(int nMotionIdx){return m_dCurPos[nMotionIdx];};
	void SetCurPos(int nMotionIdx, double dCurPos){m_dCurPos[nMotionIdx] = dCurPos;};
	BOOL GetStepPlusMinuse(){return m_bPlusMinuse;};
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateCurPos();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	void JogMove();
	void ClickBtnenhctrlTeachJogSetCurPos();
	void ClickBtnenhctrlTeachStep();
	virtual BOOL OnInitDialog();
};

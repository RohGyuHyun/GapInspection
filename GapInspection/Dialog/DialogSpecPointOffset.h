#pragma once


// CDialogSpecPointOffset 대화 상자입니다.

class CDialogSpecPointOffset : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSpecPointOffset)

public:
	CDialogSpecPointOffset(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogSpecPointOffset();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SPEC_POINT_OFFSET };


protected:
	TypeSpecPara m_Spec[MAX_INSP_POINT_CNT];
	CBtnenhctrl m_ctrlOffset[MAX_INSP_POINT_CNT];
	CRect m_Rect;
private:
	void UpdatePara(BOOL isUpdate);

public:
	TypeSpecPara GetOffset(int nPoint);
	void SetOffset(int nPoint, double dOffset);
	void SpecInput();
	void SetWindowMoveRect(CRect rect){m_Rect = rect;};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlSpecApply();
	void ClickBtnenhctrlSpecCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

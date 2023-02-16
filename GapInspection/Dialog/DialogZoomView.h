#pragma once

#define ZOOM_VIEW_IMAGE_X_OFFSET					10
#define ZOOM_VIEW_IMAGE_Y_OFFSET					63

// CDialogZoomView 대화 상자입니다.

class CDialogZoomView : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogZoomView)

public:
	CDialogZoomView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogZoomView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ZOOM_VIEW };
private:
	CStaticMatDisplay m_MatDisplay;
	int m_nCamIdx;
	IplImage* m_pImage;
	CBtnenhctrl m_ctrCamIdx[MAX_CAM_CNT/2];
	CBtnenhctrl m_ctrGrabIdx[MAX_GRAB_CNT];
	CBtnenhctrl m_ctrLive;
	CBtnenhctrl m_ctrCurserPos;
	CBtnenhctrl m_ctrCurVal;

	BOOL m_bLive[MAX_CAM_CNT];
	double m_dRslt;
	int m_nGrabIdx;
	int m_nDisplayId;
	CRect m_TrackRect[MAX_CENTER_INSP_RECT_CNT];
	int m_nSelectTrackerIdx;
	int m_nSelectInspPoint;
	BOOL m_bInitDisplay;
	int m_nChannel;
public:
	void SetCamIdx(int nChannel, int nCamIdx);
	void SetImage(IplImage* pImage);
	BOOL GetLive(int nCamIdx){return m_bLive[nCamIdx];};
	void SetLive(int nCamIdx, BOOL isLive);
	IplImage *GetImage(){return m_pImage;};
	void SetRsltVal(double dRslt);
	int GetGrabIdx(){return m_nGrabIdx;};
	int GetCamIdx(){return m_nCamIdx;};
	int GetDisplayId(){return m_nDisplayId;};
	CWnd *GetDisplayHWND();
	void SetTrackerRect(int nTrackerIdx, CRect rect);
	CRect GetTrackerRect(int nTrackerIdx);
	int GetSelectTrackerIdx(){return m_nSelectTrackerIdx;};
	void SetInspPoint(int nPoint){m_nSelectInspPoint = nPoint;};
	void SetActiveTracker(int nInspPoint){ActiveTracker(nInspPoint);};
	void UpdateDisplay();
	void SetInitDisplay();//{m_bInitDisplay = TRUE;};
protected:
	void InitImage(long width, long height);
	void ZoomViewChange();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ActiveTracker(int nInspPoint);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlGrab();
	void ClickBtnenhctrlInspection();
	void ClickBtnenhctrlLive();
	void ClickBtnenhctrlExit();
	void ClickBtnenhctrlImageSave();
	void ClickBtnenhctrlImageLoad();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void SelectCamGrabImageView();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
};

#pragma once

//Tracker
class C_CtrlTracker{
protected:
	//그룹화
	BOOL m_bGroup;
	int m_iType;//영역 종류 0: rectangle(no angle), 1: line(angle), 2: circle, 3: ellipse
	CString m_sCaption;//영역 각주
	BOOL m_bEnable;//활성화 여부
	COLORREF m_cColor;//선 색
	CWnd *m_pParentWnd;//부모 윈도우 핸들러
	BOOL m_bDown;//마우스 버튼 플러그 
	CRect m_rectOld;//이전 영역 좌표 저장 변수
	//마우스가 추적박스에서 8개방향과 이동 위치의 어느점에 있는가의 
	//값을 저장하고 있는 변수
	int m_nChoNum;
	//현재 프로세스를 설정하고 있는 변수
	int m_nProcess;
	//8개 방향의 좌표와 4개방향의 이동좌표 저장 변수
	CRect m_rectDirect[12];
	//이전 마우스 포인터 변수
	CPoint m_OldPoint;
	//m_rect영역이 설정되면 이영역을 토대로 m_rectDirect의 영역을 만들어주는 
	//함수
	void MakeDirect();
	//8개 영역 박스 점이 선보다 얼마나 두꺼운가를 저장하는 변수
	int m_nLineDot;
	//추적박스 라인의 두께 
	int m_nLindTick;
	//m_rectDirect영역이 설정되어 있으면 TRUE그렇지 않으면 FALSE
	int m_bSet;

	//Zoom
	long m_lZoomStartX;
	long m_lZoomStartY;
	long m_lZoomEndX;
	long m_lZoomEndY;

	//Rectangle//////////////////////////////////
	//이동되는 영역 좌표 저장 변수
	CRect m_rectTrack;
	//현재 추적박스의 좌표
	CRect m_rect;
	BOOL m_bActive;
	double m_dRectL;
	double m_dRectT;
	double m_dRectR;
	double m_dRectB;


public:
	void GetZoom(long *start_x, long *start_y, long *end_x, long *end_y);
	void SetZoom(long start_x, long start_y, long end_x, long end_y);
	void GetRect(double *left, double *top, double *right, double *bottom);
	void SetRect(double left, double top, double right, double bottom);

	void SetActive(BOOL active);
	void Draw(int idx);
	BOOL GetGroup();
	void SetEnableGroup(BOOL enable);

	int GetType();
	BOOL GetEnable();
	void SetEnable(BOOL enable);
	void SetCaption(char* caption);
	void SetColor(COLORREF color);
	C_CtrlTracker();
	C_CtrlTracker(CWnd *pWnd, int type = 0, int dot = 2, int tick = 3);

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void SetProcess(int num);
	int  GetProcess();

	int PtInRect(CPoint point);
	void SetMouseCursor();
	int GetChoNum(){return m_nChoNum;}
	BOOL OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	void SetRect(CRect rect);
	CRect GetRect();
	
};
#include "stdafx.h"
#include "_CtrlTracker.h"

//생성자
C_CtrlTracker::C_CtrlTracker()
{
	m_bGroup = FALSE;
	m_bEnable = FALSE;
	m_iType = 0;
	m_sCaption.Empty();
	m_cColor = RGB(127, 127, 127);
	m_rect=CRect(0,0,0,0);
	m_bDown=FALSE;
	m_pParentWnd=NULL;
	m_nProcess=0;
	m_nLineDot=2;
	m_nLindTick=3;
	m_nChoNum=-1;
	m_bSet=FALSE;
	m_OldPoint=CPoint(0,0);
	m_rectOld=CRect(0,0,0,0);//0527
	m_bActive = TRUE;
	m_lZoomStartX = 0;
	m_lZoomStartY = 0;
	m_lZoomEndX = 0;
	m_lZoomEndY = 0;
	m_dRectL = 0;
	m_dRectT = 0;
	m_dRectR = 0;
	m_dRectB = 0;
}

C_CtrlTracker::C_CtrlTracker(CWnd *pWnd, int type, int dot, int tick)
{
	m_bGroup = FALSE;
	m_bEnable = FALSE;
	m_iType = type;
	m_sCaption.Empty();
	m_cColor = RGB(127, 127, 127);
	m_rect=CRect(0,0,0,0);
	m_pParentWnd=pWnd;
	m_bDown=FALSE;
	m_nProcess=0;
	m_nLineDot=dot;
	m_nLindTick=tick;
	m_nChoNum=-1;
	m_bSet=FALSE;
	m_OldPoint=CPoint(0,0);
	m_rectOld=CRect(0,0,0,0);//0527
	m_bActive = TRUE;
	m_lZoomStartX = 0;
	m_lZoomStartY = 0;
	m_lZoomEndX = 0;
	m_lZoomEndY = 0;
	m_dRectL = 0;
	m_dRectT = 0;
	m_dRectR = 0;
	m_dRectB = 0;
}

//추적박스의 8개 방향 영역과 이동영역에
//마우스 포인터가 있는가를 찾는 함수
int C_CtrlTracker::PtInRect(CPoint point)
{
	int i;

	//return 8;

	//그룹상태이면 
	if(m_bGroup)
	{
		return 8;//이동모드
	}
		
	//12개의 방향을 m_rect를 참조하여 만든다.
	MakeDirect();
	//이 12개의 방향위에 있으면 해당 번호를
	//리턴한다.
	for(i=0;i<12;i++)
	{
		if(m_rectDirect[i].PtInRect(point))
		{
			return i;
		}
	}

	//추적 박스 영역위에 없으면 -1를 리턴
	return -1;
}
//해당 방향에 따라 마우스 커서를 설정한다.
void C_CtrlTracker::SetMouseCursor()
{
	if(!m_bActive)
	{
		return;
	}

	switch(m_iType)
	{
		case 0:
		case 1:
		case 2:
			switch(m_nChoNum)
			{
				case 0:
				case 2:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)AfxGetApp()->LoadCursor(AFX_IDC_TRACKNWSE));
						break;
				case 1:
				case 3:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)AfxGetApp()->LoadCursor(AFX_IDC_TRACKNESW));
						break;
				case 4:
				case 5:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)AfxGetApp()->LoadCursor(AFX_IDC_TRACKNS));
						break;
				case 6:
				case 7:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)AfxGetApp()->LoadCursor(AFX_IDC_TRACKWE));
						break;
				case 8:
				case 9:
				case 10:
				case 11:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)AfxGetApp()->LoadCursor(AFX_IDC_TRACK4WAY));
						break;
				default:
						SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)LoadCursor(NULL,IDC_ARROW));
						break;
			}
			break;
		default:
			AfxMessageBox(_T("Error : invalid index!"));
			break;
	}
}
//마우스 이동시
void C_CtrlTracker::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bActive)
	{
		return;
	}

	switch(m_iType)
	{
		case 0:
		case 2:
			if(m_bDown && m_nProcess!=0)
			{
				m_rectTrack.right=point.x;
				m_rectTrack.bottom=point.y;
				CDC *pDC=m_pParentWnd->GetDC();
				CPen pen,*oldPen;
				pDC->SetROP2(R2_XORPEN);
				pen.CreatePen(PS_DOT,2,RGB(127, 127, 127));
				oldPen=pDC->SelectObject(&pen);
				pDC->MoveTo(m_rectOld.left,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.bottom);
				pDC->LineTo(m_rectOld.left,m_rectOld.bottom);
				pDC->LineTo(m_rectOld.left,m_rectOld.top);

				pDC->MoveTo(m_rectTrack.left,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.bottom);
				pDC->LineTo(m_rectTrack.left,m_rectTrack.bottom);
				pDC->LineTo(m_rectTrack.left,m_rectTrack.top);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
				m_rectOld=m_rectTrack;
				m_pParentWnd->ReleaseDC(pDC);
			}

			if(m_nProcess==0 && !m_bDown && m_bSet)
			{
				if(m_bEnable)
				{
					m_nChoNum = PtInRect(point);
					SetMouseCursor();
				}
				else
				{
					m_nChoNum = -1;
				}
			}
			else if(m_nProcess==0 && m_bSet)
			{
				//추적박스 영역이 아니면 리턴
				if(m_nChoNum<0)
					return;

				//추적박스 영역일경우
				switch(m_nChoNum)
				{
					case 0://좌측 상단 선택시
							m_rectTrack.left=point.x;
							m_rectTrack.top=point.y;
							break;
					case 1://우측 상단 선택시 
							m_rectTrack.right=point.x;
							m_rectTrack.top=point.y;
							break;
					case 2://우측 하단 선택시
							m_rectTrack.right=point.x;
							m_rectTrack.bottom=point.y;
							break;
					case 3://좌측 하단 선택시
							m_rectTrack.left=point.x;
							m_rectTrack.bottom=point.y;
							break;
					case 4://위쪽으로 
							m_rectTrack.top=point.y;
							break;
					case 5://아래로
							m_rectTrack.bottom=point.y;
							break;
					case 6://좌측으로
							m_rectTrack.left=point.x;
							break;
					case 7://우측으로
							m_rectTrack.right=point.x;
							break;
							//이동일경우
					case 8:case 9:case 10:case 11:
						{
							CPoint def;
							def=point-m_OldPoint;
							m_rectTrack.top+=def.y;
							m_rectTrack.bottom+=def.y;
							m_rectTrack.left+=def.x;
							m_rectTrack.right+=def.x;
							m_OldPoint=point;
						}
							break;
					//전체영역을 뜻하는 13인 경우에는 아무 동작도  하지 않는다
				}
				CDC *pDC=m_pParentWnd->GetDC();
				CPen pen,*oldPen;
				pDC->SetROP2(R2_XORPEN);
				//이전 그려진 영역 지우고
				pen.CreatePen(PS_DOT,2,RGB(127, 127, 127));
				oldPen=pDC->SelectObject(&pen);
				pDC->MoveTo(m_rectOld.left,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.bottom);
				pDC->LineTo(m_rectOld.left,m_rectOld.bottom);
				pDC->LineTo(m_rectOld.left,m_rectOld.top);
				//새로운 영역을 그린다.
				pDC->MoveTo(m_rectTrack.left,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.bottom);
				pDC->LineTo(m_rectTrack.left,m_rectTrack.bottom);
				pDC->LineTo(m_rectTrack.left,m_rectTrack.top);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
				m_rectOld=m_rectTrack;
				m_pParentWnd->ReleaseDC(pDC);

				m_dRectL = m_rectTrack.left;
				m_dRectT = m_rectTrack.top;
				m_dRectR = m_rectTrack.right;
				m_dRectB = m_rectTrack.bottom;
			}
			break;
		case 1:
			if(m_bDown && m_nProcess!=0)
			{
				m_rectTrack.right=point.x;
				m_rectTrack.bottom=point.y;
				CDC *pDC=m_pParentWnd->GetDC();
				CPen pen,*oldPen;
				pDC->SetROP2(R2_XORPEN);
				pen.CreatePen(PS_DOT,2,RGB(127, 127, 127));
				oldPen=pDC->SelectObject(&pen);
				pDC->MoveTo(m_rectOld.left,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.bottom);

				pDC->MoveTo(m_rectTrack.left,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.bottom);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
				m_rectOld=m_rectTrack;
				m_pParentWnd->ReleaseDC(pDC);
			}
			if(m_nProcess==0 && !m_bDown && m_bSet)
			{
				if(m_bEnable)
				{
					m_nChoNum=PtInRect(point);
					SetMouseCursor();
				}
				else
				{
					m_nChoNum = -1;
				}
			}
			else if(m_nProcess==0 && m_bSet)
			{
				//추적박스 영역이 아니면 리턴
				if(m_nChoNum<0)
					return;
				//추적박스 영역일경우
				switch(m_nChoNum)
				{
					case 0://좌측 상단 선택시
							m_rectTrack.left=point.x;
							m_rectTrack.top=point.y;
							break;
					case 1://우측 상단 선택시 
							m_rectTrack.right=point.x;
							m_rectTrack.top=point.y;
							break;
					case 2://우측 하단 선택시
							m_rectTrack.right=point.x;
							m_rectTrack.bottom=point.y;
							break;
					case 3://좌측 하단 선택시
							m_rectTrack.left=point.x;
							m_rectTrack.bottom=point.y;
							break;
					case 4://위쪽으로 
							m_rectTrack.top=point.y;
							break;
					case 5://아래로
							m_rectTrack.bottom=point.y;
							break;
					case 6://좌측으로
							m_rectTrack.left=point.x;
							break;
					case 7://우측으로
							m_rectTrack.right=point.x;
							break;
							//이동일경우
					case 8:case 9:case 10:case 11:
						{
							CPoint def;
							def=point-m_OldPoint;
							m_rectTrack.top+=def.y;
							m_rectTrack.bottom+=def.y;
							m_rectTrack.left+=def.x;
							m_rectTrack.right+=def.x;
							m_OldPoint=point;
						}
							break;
					//전체영역을 뜻하는 13인 경우에는 아무 동작도  하지 않는다
				}
				CDC *pDC=m_pParentWnd->GetDC();
				CPen pen,*oldPen;
				pDC->SetROP2(R2_XORPEN);
				//이전 그려진 영역 지우고
				pen.CreatePen(PS_DOT,2,RGB(127, 127, 127));
				oldPen=pDC->SelectObject(&pen);
				pDC->MoveTo(m_rectOld.left,m_rectOld.top);
				pDC->LineTo(m_rectOld.right,m_rectOld.bottom);

				//새로운 영역을 그린다.
				pDC->MoveTo(m_rectTrack.left,m_rectTrack.top);
				pDC->LineTo(m_rectTrack.right,m_rectTrack.bottom);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
				m_rectOld=m_rectTrack;
				m_pParentWnd->ReleaseDC(pDC);
			}
			break;
		default:
			AfxMessageBox(_T("Error : invalid index!"));
			break;
	}
}
//마우스 버튼 눌렀을때
void C_CtrlTracker::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_bActive)
	{
		return;
	}

	//만약에 현재 아무것도 설정되어 있지않고
	//마우스버튼을 누름
	//m_OldPoint에 point값 설정
	m_OldPoint=point;
	m_bDown=TRUE;
}
//마우스 버튼 놓음
void C_CtrlTracker::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDC *pDC;
	CPen pen,*oldPen;
	
	if(!m_bActive)
	{
		return;
	}
	
	switch(m_iType)
	{
	case 0:
	case 2:
		pDC=m_pParentWnd->GetDC();
		pDC->SetROP2(R2_XORPEN);
		pen.CreatePen(PS_DOT,2,m_cColor);
		oldPen=pDC->SelectObject(&pen);
		pDC->MoveTo(m_rectOld.left,m_rectOld.top);
		pDC->LineTo(m_rectOld.right,m_rectOld.top);
		pDC->LineTo(m_rectOld.right,m_rectOld.bottom);
		pDC->LineTo(m_rectOld.left,m_rectOld.bottom);
		pDC->LineTo(m_rectOld.left,m_rectOld.top);
		
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		m_pParentWnd->ReleaseDC(pDC);
		m_rectOld=CRect(0,0,0,0);
		m_bDown=FALSE;
		
		//마우스버튼이 up될때 사각형 값을 재 입력
		SetRect(m_rectTrack);
		break;
	case 1:
		pDC=m_pParentWnd->GetDC();
		pDC->SetROP2(R2_XORPEN);
		pen.CreatePen(PS_DOT,2,m_cColor);
		oldPen=pDC->SelectObject(&pen);
		pDC->MoveTo(m_rectOld.left,m_rectOld.top);
		pDC->LineTo(m_rectOld.right,m_rectOld.bottom);
		
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		m_pParentWnd->ReleaseDC(pDC);
		m_rectOld=CRect(0,0,0,0);
		m_bDown=FALSE;
		
		//마우스버튼이 up될때 사각형 값을 재 입력
		SetRect(m_rectTrack);
		break;
	default:
		AfxMessageBox(_T("Error : invalid index!"));
		break;
	}
	
}
//프로세서 값 설정(사용할 필요 없음, 사용한다면 항상 이 값을 0으로 바꿔줘야 함)
void C_CtrlTracker::SetProcess(int num)
{
	m_nProcess=num;
}
//현재 설정된 프로세서 값 얻음
int C_CtrlTracker::GetProcess()
{
	return m_nProcess;
}
//12개의 방향을 만들어 준다.
void C_CtrlTracker::MakeDirect()
{
	int line_dot = m_nLineDot;

	switch(m_iType)
	{
		case 0:
		case 2:
			m_rect=CRect(min(m_rect.left,m_rect.right),
						 min(m_rect.top,m_rect.bottom),
						 max(m_rect.left,m_rect.right),
						 max(m_rect.top,m_rect.bottom));
			break;
		case 1:

			break;
		default:
			AfxMessageBox(_T("Error : invalid index!"));
			break;
	}
	m_rectTrack=m_rect;

	int pos;
	m_rectDirect[0]=CRect(m_rect.left-m_nLineDot,m_rect.top-m_nLineDot,
		m_rect.left+m_nLineDot,m_rect.top+m_nLineDot);
	m_rectDirect[1]=CRect(m_rect.right-m_nLineDot,m_rect.top-m_nLineDot,
			m_rect.right+m_nLineDot,m_rect.top+m_nLineDot);

	m_rectDirect[2]=CRect(m_rect.right-m_nLineDot,m_rect.bottom-m_nLineDot,
		m_rect.right+m_nLineDot,m_rect.bottom+m_nLineDot);

	m_rectDirect[3]=CRect(m_rect.left-m_nLineDot,m_rect.bottom-m_nLineDot,
		m_rect.left+m_nLineDot,m_rect.bottom+m_nLineDot);

	pos=(m_rect.Width()/2)+m_rect.TopLeft().x; 
	m_rectDirect[4]=CRect(pos-m_nLineDot,m_rect.top-m_nLineDot,
		pos+m_nLineDot,m_rect.top+m_nLineDot);
	pos=(m_rect.Width()/2)+m_rect.TopLeft().x; 
	m_rectDirect[5]=CRect(pos-m_nLineDot,m_rect.bottom-m_nLineDot,
		pos+m_nLineDot,m_rect.bottom+m_nLineDot);
	pos=(m_rect.Height()/2)+m_rect.TopLeft().y; 
	m_rectDirect[6]=CRect(m_rect.left-m_nLineDot,pos-m_nLineDot,
		m_rect.left+m_nLineDot,pos+m_nLineDot);
	m_rectDirect[7]=CRect(m_rect.right-m_nLineDot,pos-m_nLineDot,
		m_rect.right+m_nLineDot,pos+m_nLineDot);

	line_dot = line_dot * 2;
	m_rectDirect[8]=CRect(m_rect.left,m_rect.top-line_dot,
		m_rect.right,m_rect.top+line_dot);
	m_rectDirect[9]=CRect(m_rect.left-line_dot,m_rect.top,
		m_rect.left+line_dot,m_rect.bottom);
	m_rectDirect[10]=CRect(m_rect.right-line_dot,m_rect.top,
		m_rect.right+line_dot,m_rect.bottom);
	m_rectDirect[11]=CRect(m_rect.left,m_rect.bottom-line_dot,
		m_rect.right,m_rect.bottom+line_dot);
}

//추적상자 영역을 설정한다.
void C_CtrlTracker::SetRect(CRect rect)
{
	m_rect=rect;
	m_rectTrack=rect;
}

//추적상자 영역을 얻는다
CRect C_CtrlTracker::GetRect()
{
	return m_rectTrack;
}

//선 색 지정
void C_CtrlTracker::SetColor(COLORREF color)
{
	m_cColor = color;
}

//각주 지정
void C_CtrlTracker::SetCaption(char *caption)
{
	m_sCaption = caption;
}

//영역 활성화 설정
void C_CtrlTracker::SetEnable(BOOL enable)
{
	m_bEnable = enable;
}

//활성화 상태 얻기
BOOL C_CtrlTracker::GetEnable()
{
	return m_bEnable;
}

//영역 종류 얻기
int C_CtrlTracker::GetType()
{
	return m_iType;
}

//마우스 버튼 더블 클릭 했을때(영역 안에 포인트가 있으면 TRUE반환)
BOOL C_CtrlTracker::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(!m_bActive)
	{
		return FALSE;
	}

	if(0 > PtInRect(point))
	{
		return FALSE;
	}


	return TRUE;
}

//그룹화 지정
void C_CtrlTracker::SetEnableGroup(BOOL enable)
{
	m_bGroup = enable;
	SetClassLong(m_pParentWnd->m_hWnd,GCLP_HCURSOR,(long)LoadCursor(NULL,IDC_ARROW));
}

//그룹상태 얻기
BOOL C_CtrlTracker::GetGroup()
{
	return m_bGroup;
}

void C_CtrlTracker::Draw(int idx)
{
	CDC *pDC;
	CPen pen,*oldPen;
	CBrush brush,*oldBrush;
	CString caption;

	if(!m_bActive)
	{
		return;
	}

	pDC=m_pParentWnd->GetDC();
	switch(m_iType)
	{
		//Rectangle
		case 0:
			if(m_rect.left==0 && m_rect.top==0 && m_rect.bottom==0 && m_rect.right==0)
				return;
			//8개 방향의 영역을 설정한다.
			MakeDirect();
			//8개 방향의 영역이 설정되면 m_bSet은 TRUE로
			m_bSet=TRUE;
			//추적박스의 메인 박스를 그린다.
			pen.CreatePen(PS_DOT,m_nLindTick,m_cColor);
			oldPen=pDC->SelectObject(&pen);
			pDC->MoveTo(m_rect.left,m_rect.top);
			pDC->LineTo(m_rect.right,m_rect.top);
			pDC->LineTo(m_rect.right,m_rect.bottom);
			pDC->LineTo(m_rect.left,m_rect.bottom);
			pDC->LineTo(m_rect.left,m_rect.top);
			pDC->SelectObject(oldPen);
			pen.DeleteObject();
			brush.CreateSolidBrush(m_cColor);
			oldBrush=pDC->SelectObject(&brush);
			//8개 방향의 박스를 그린다.
			if(m_bEnable)
			{
				for(int i=0;i<8;i++)
					pDC->Rectangle(m_rectDirect[i]);
			}

			//문자열을 그린다
			pDC->SetTextColor(m_cColor);
			pDC->SetBkMode(TRANSPARENT);
			//caption.Format("%d-%s", idx + 1, (LPCTSTR)m_sCaption);
			caption.Format(_T("-%s"), (LPCTSTR)m_sCaption);
			pDC->TextOut(m_rect.left, m_rect.bottom + m_nLineDot, caption);

			pDC->SelectObject(oldBrush);
			brush.DeleteObject();
			break;
		//Line
		case 1:
			if(m_rect.left==0 && m_rect.top==0 && m_rect.bottom==0 && m_rect.right==0)
				return;
			//8개 방향의 영역을 설정한다.
			MakeDirect();
			//8개 방향의 영역이 설정되면 m_bSet은 TRUE로
			m_bSet=TRUE;
			//추적박스의 메인 박스를 그린다.
			pen.CreatePen(PS_DOT,m_nLindTick,m_cColor);
			oldPen=pDC->SelectObject(&pen);
			pDC->MoveTo(m_rect.left,m_rect.top);
			pDC->LineTo(m_rect.right,m_rect.bottom);
			pDC->SelectObject(oldPen);
			pen.DeleteObject();
			brush.CreateSolidBrush(m_cColor);
			oldBrush=pDC->SelectObject(&brush);
			//2개 방향의 박스를 그린다(2개만 사용한다).
			if(m_bEnable)
			{
				pDC->Rectangle(m_rectDirect[0]);
				pDC->Rectangle(m_rectDirect[2]);
			}

			//문자열을 그린다
			pDC->SetTextColor(m_cColor);
			pDC->SetBkMode(TRANSPARENT);
			//caption.Format("%d-%s", idx + 1, (LPCTSTR)m_sCaption);
			caption.Format(_T("-%s"), (LPCTSTR)m_sCaption);
			pDC->TextOut(m_rect.right, m_rect.bottom + m_nLineDot, caption);

			pDC->SelectObject(oldBrush);
			brush.DeleteObject();
			break;
		//Ellipse
		case 2:
			if(m_rect.left==0 && m_rect.top==0 && m_rect.bottom==0 && m_rect.right==0)
				return;
			//8개 방향의 영역을 설정한다.
			MakeDirect();
			//8개 방향의 영역이 설정되면 m_bSet은 TRUE로
			m_bSet=TRUE;
			//추적박스의 메인 박스를 그린다.
			pen.CreatePen(PS_DOT,m_nLindTick,m_cColor);
			oldPen=pDC->SelectObject(&pen);
			pDC->MoveTo(m_rect.left,m_rect.top);
			pDC->LineTo(m_rect.right,m_rect.top);
			pDC->LineTo(m_rect.right,m_rect.bottom);
			pDC->LineTo(m_rect.left,m_rect.bottom);
			pDC->LineTo(m_rect.left,m_rect.top);
			
			oldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
			pDC->Ellipse(m_rect);
			pDC->SelectObject(oldBrush);

			pDC->SelectObject(oldPen);
			pen.DeleteObject();

			brush.CreateSolidBrush(m_cColor);
			oldBrush=pDC->SelectObject(&brush);

			//8개 방향의 박스를 그린다.
			if(m_bEnable)
			{
				for(int i=0;i<8;i++)
					pDC->Rectangle(m_rectDirect[i]);
			}

			//문자열을 그린다
			pDC->SetTextColor(m_cColor);
			pDC->SetBkMode(TRANSPARENT);
			//caption.Format("%d-%s", idx + 1, (LPCTSTR)m_sCaption);
			caption.Format(_T("-%s"), (LPCTSTR)m_sCaption);
			pDC->TextOut(m_rect.left, m_rect.bottom + m_nLineDot, caption);

			pDC->SelectObject(oldBrush);
			brush.DeleteObject();
			break;
		default:
			AfxMessageBox(_T("Error : invalid index!"));
			break;
	}
	m_pParentWnd->ReleaseDC(pDC);
}

void C_CtrlTracker::SetActive(BOOL active)
{
	m_bActive = active;
}

void C_CtrlTracker::SetZoom(long start_x, long start_y, long end_x, long end_y)
{
	m_lZoomStartX = start_x;
	m_lZoomStartY = start_y;
	m_lZoomEndX = end_x;
	m_lZoomEndY = end_y;
}

void C_CtrlTracker::GetZoom(long *start_x, long *start_y, long *end_x, long *end_y)
{
	*start_x = m_lZoomStartX;
	*start_y = m_lZoomStartY;
	*end_x = m_lZoomEndX;
	*end_y = m_lZoomEndY;
}

void C_CtrlTracker::GetRect(double *left, double *top, double *right, double *bottom)
{
	*left = m_dRectL; 
	*top = m_dRectT; 
	*right = m_dRectR; 
	*bottom = m_dRectB; 
}

void C_CtrlTracker::SetRect(double left, double top, double right, double bottom)
{
	m_dRectL = left;
	m_dRectT = top;
	m_dRectR = right;
	m_dRectB = bottom;

	m_rect=CRect(left, top, right, bottom);
	m_rectTrack=CRect(left, top, right, bottom);
}
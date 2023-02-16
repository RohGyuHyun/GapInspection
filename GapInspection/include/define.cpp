#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include <math.h>
#include "define.h"


//20190219 ngh
CString InspPointToString(int nInspPoint)
{
	CString strText;

	switch(nInspPoint)
	{
	case INSP_POINT1:
		strText.Format(_T("%s"), INSP_STRING_POINT_1);	
		break;
	case INSP_POINT2:
		strText.Format(_T("%s"), INSP_STRING_POINT_2);
		break;
	case INSP_POINT3:
		strText.Format(_T("%s"), INSP_STRING_POINT_3);
		break;
	case INSP_POINT4:
		strText.Format(_T("%s"), INSP_STRING_POINT_4);
		break;
	case INSP_POINT5:
		strText.Format(_T("%s"), INSP_STRING_POINT_5);
		break;
	case INSP_POINT6:
		strText.Format(_T("%s"), INSP_STRING_POINT_6);
		break;
	}

	return strText;
}

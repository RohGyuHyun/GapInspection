
// GapInspection.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CGapInspectionApp:
// �� Ŭ������ ������ ���ؼ��� GapInspection.cpp�� �����Ͻʽÿ�.
//

class CGapInspectionApp : public CWinApp
{
public:
	CGapInspectionApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CGapInspectionApp theApp;
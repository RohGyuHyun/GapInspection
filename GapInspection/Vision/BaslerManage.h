#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

//20161017
#include <conio.h>

#include <pylonc/PylonC.h>

//20161019 ht 카메라 상태체크 위한 define
#define SUCCESS_CAMERA				0
#define ERROR_CAMERA_NOT_OPEN		1
#define ERROR_CAMERA_AVAILABLE		2



//20161018 ht 카메라 C기반으로 수정
/*		
InitCamera ->카메라 초기화 및 heartbeat 시간 설정
grab -> grab함수
setHeartbeatTimeout -> heartbeat 시간 설정
Release -> 카메라 지정값 해제
*/



class CBaslerManage
{
public:
	CBaslerManage();
	CBaslerManage(int _w, int _h);
	~CBaslerManage();
	
	BYTE *GetGrabImgBuf(){return m_pImageBuf;}
	PylonGrabResult_t ReturnGrabResult(){return m_struct_GrabResult;}


private:
	PYLON_DEVICE_HANDLE m_hDevice;
	int32_t m_nPayloadSize;
	PylonGrabResult_t m_struct_GrabResult;
	BYTE *m_pImageBuf;

public:
	int InitCamera(int heartbeat_ms = 500);
	BOOL Grab(BOOL disp = FALSE);
	GENAPIC_RESULT setHeartbeatTimeout( int timeout_ms );
	void Release();
	
};


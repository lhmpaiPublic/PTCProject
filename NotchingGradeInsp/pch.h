// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.
#pragma once

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
//라이브러리 파일
#include <string>
#include <vector>


//생성 파일
#include "framework.h"
#include "GlobalDef.h"

#include <GdiPlus.h>
using namespace Gdiplus;

#include "AXL.h"
#include "AXD.h"

#ifdef _M_X64
#pragma comment(lib, "AxL.lib")
#else
#pragma comment(lib, "AxL.lib")
#endif

//멀티 바이트 속성으로 컴파일 시 중국어 경고 끄기
#pragma warning(disable: 4566)
#pragma warning(disable: 4819)

#pragma comment(linker, "/HEAP:4000000000")

//검사 프로그램 명
#define NOTCHINGMAINFRM_NAME _T("PTC_NotchingGradeInsp")

#define MAX_INT 2147483647

//DIO 보드넘버 : 현장 pc 0 vs DEMO pc 1
#define DIO_BOARD_NO 0

//SPC 객체 소스에서 컴파일 여부 결정
//SPC+ 생성을 위한 플래그
#define SPCPLUS_CREATE

// GMLV Line용 - 조명 컨트롤러 1개 사용 시 Error Skip
//#define GMLV_LIGHT_SKIP

//컨넥트 존 컴파일 여부
//#define USE_PLCCONNECTZONE

//BCD Counter 컴파일 여부
#define USE_BCDCOUNTER

//글로벌 버전 정보 로컬 세팅
//Notching.Release.Version_년(4).월(2).일(2).(배포 횟수 : r01 ~ r99)
static CString GlobalVersion = _T("Notching.Release.Version_2024.03.18.r01");

//Glabal Surpport 클래스
#include "StrSuport.h"
//UI를 관리하기 위한 클래스
#include "UiManager.h"

#include <string>
#include <vector>
using namespace std;

#include "GlobalFunc.h"
#include "LogDisplayDlg.h"

#include "SpcPlusManager.h"
#include "SpcInData.h"

#include "TriggerSocket.h"

#endif //PCH_H

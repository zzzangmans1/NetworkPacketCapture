﻿// NetworkPacketCaptureDlg.h: 헤더 파일
//

#pragma once

#include <pcap/pcap.h>
#include "Protocol.h"
#include "ChoiceNetworkInterface.h"   
#include "ChangeColor.h"
#include <locale.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <afxsock.h>
#include <TlHelp32.h>			// *** 파일 종료할 때 찾는 라이브러리s
#include <io.h>					// *** findfirst 함수를 사용하기 위해 헤더 추가

#include "winioctl.h"			// *** 디바이스 소통에 필요한 라이브러리 정의

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
#define DEVICE_RECV CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA)

// 소켓을 사용하기 위해서 라이브러리 참조해야 한다.
#pragma comment(lib, "ws2_32")
// inet_ntoa가 deprecated가 되었는데.. 사용하려면 아래 설정을 해야 한다.
#pragma warning(disable:4996)
#include <vector>
#include <thread>
// 소켓을 사용하기 위한 라이브러리
#include <WinSock2.h>

using namespace std;

// 수신 버퍼 사이즈
#define BUFFERSIZE 1024


void Packet_Handler(u_char* param, const pcap_pkthdr* header, const u_char* data);	// *** 클래스 외에 생성해야 pcap_loop 함수 실행가능

// CNetworkPacketCaptureDlg 대화 상자
class CNetworkPacketCaptureDlg : public CDialogEx
{
	// 생성입니다.
public:
	CNetworkPacketCaptureDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORKPACKETCAPTURE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	CToolBar m_wndToolBar;														// *** 툴바 변수 선언

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);										// *** 다이얼로그 백그라운드 컬러 지정할 함수 선언
		
	// *** 메뉴
	afx_msg void Onsourcebutton();												// *** 메뉴 소스코드 버튼 함수
	afx_msg void OnLogButton();													// *** 메뉴 로그 버튼 함수
	afx_msg void OnDCreateButton();												// *** 드라이버 생성 함수
	afx_msg void OnDCloseButton();												// *** 드라이버 종료 함수
	afx_msg void OnDSendButton();												// *** 드라이버 데이터 보내기 함수
	afx_msg void OnDRecvButton();												// *** 드라이버 데이터 받기 함수
	afx_msg void OnChangeColorButton();											// *** 프로토콜 프로토콜 색상 변경 버튼 함수

	// *** 툴바 
	void InitToolBar();															// *** 툴바 생성 함수 선언
											

	enum class ThreadWorkingType {
		THREAD_STOP,
		THREAD_RUNNING,
		THREAD_PAUSE
	};

	ETHERNET_HEADER* m_EthernetHeader;				// *** ETHERNET HEADER
	IP_HEADER* m_IpHeader;							// *** IP HEADER
	size_t m_IpHeaderLen;
	TCP_HEADER* m_TCPHeader;						// *** TCP HEADER
	UDP_HEADER* m_UDPHeader;						// *** UDP HEADER
	ARP_HEADER* m_ARPHeader;						// *** ARP HEADER
	DNS_HEADER* m_DNSHeader;						// *** DNS HEADER
	
	TCP_HEADER_PAYLOAD* m_TCPHeaderPayload;			// *** TCP HEADER PAYLOAD
	/*
	struct pcap_pkthdr {
		struct timeval ts;       time stamp			캡쳐된 시간정보가 저장된 멤버 ts
		bpf_u_int32 caplen;   length of portion present		캡쳐한 패킷의 길이가 저장된 멤버 caplen
		bpf_u_int32 len;        length this packet (off wire) 패킷의 길이가 저장된 멤버 len
	};
	*/
	pcap_pkthdr* header = NULL;
	pcap_if_t* m_MyDev = NULL;
	pcap_t* m_NetworkDeviceHandler = NULL;
	char errbuf[PCAP_ERRBUF_SIZE] = "";											// *** 네트워크 에러 변수 선언
	
	CListCtrl m_NetworkInterfaceControlList;									// *** 컨트롤 리스트 변수 선언
	CListCtrl m_PacketDataControlList;											// *** 패킷 데이터 리스트 
	CTreeCtrl m_PacketInfoTree;
	CButton m_PauseButton;


	CRect rect;																	// *** 윈도우 크기 받아올 변수 선언
	CString strcnt;																// *** 리스트 순번 변수 선언
	CString print_str;

	// 컨트롤 리스트 출력할 변수
	CString m_CurrentTime;														// *** 시간 받아올 변수
	CString m_SourceIp;
	CString m_DestinationIp;
	CString m_Protocol;
	CString m_PacketLength;
	CString m_UDPPacketInfo;
	CString m_TCPPacketInfo;
	CString m_ARPPacketInfo;
	CString m_PacketSaveData;

	// 1. 사용할 변수 및 정렬 정보를 담은 구조체 선언
   // 정렬 방식에 대한 BOOL 변수
	BOOL m_bAscending;
	// 구조체에 정보를 담아서 파라메터로 넘기기 위해서 사용
	struct SORTPARAM
	{
		int iSortColumn = 0;
		bool bSortDirect = FALSE;
		CListCtrl* pList;
		int flag = -1;  // 클릭한 header에 따라서 정렬할 값이 다르기 때문에 구분해주기위한 변수
	};

	size_t click_index = 0;																// *** 클릭한 인덱스 
	int ClickPacketFrameNumber = 0;
	int i;
	int net_dev_idx;

	BOOL is_PCThreadStart = FALSE;													// *** 스레드 체크 변수
	BOOL is_LOGThreadStart = FALSE;
	BOOL is_RunThreadOut = FALSE;

	CWinThread* m_PCThread;
	CWinThread* m_LOGThread;
	ThreadWorkingType m_eThreadWork = ThreadWorkingType::THREAD_STOP;

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo = { 0 };												// *** 로그 서버 실행 변수 선언

	CString m_FilterString = "";													// *** 필터 할 문자열
	int m_Filcnt = 0;
	BOOL is_FilStart = FALSE;														// *** 필터링 시작 변수
	BOOL is_FilCheck = FALSE;														// *** 필터링 내용 체크
	CString Fil_str;

	CString tcp, tcpR, tcpG, tcpB,													// *** config 파일 읽어들일 변수
		udp, udpR, udpG, udpB,
		ssdp, ssdpR, ssdpG, ssdpB,
		arp, arpR, arpG, arpB,
		dns, dnsR, dnsG, dnsB,
		tls, tlsR, tlsG, tlsB,
		tmp;

	CString m_ServerFileFath;																	// *** Server 파일 패스 입력할 
	CString m_ServerFileName;																	// *** Server 파일 이름
	CString m_ConfigFileFath;
	CString m_ConfigFileName;
		
	// *** 디렉토리 찾을 변수
	char currdir[100];
	CString curd;

	// *** 드라이버 실행 변수
	HANDLE devicehandle;

		// 2. 클릭시 이벤트 추가 및 정렬에 사용할 함수 선언
	
	// *** 함수
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);							// *** 컨트롤 리스트에 색상 넣을 함수 선언
	int SetPacketInfoTree(CString framecnt, CString time, CString protocol, CString lenth, CString savedata);
	int SetPacketHexList(CString data, CString protocol, int udpsize);
	int EnterDataFile(CString time, CString src, CString dst, CString protocol, CString length, CString info, CString savedata);
	int FindConfig();
	void FindServer();																
	void ReadConfig();

	static int CALLBACK CompareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);		// *** SORT 처리 함수
	afx_msg void OnTvnSelchangedPacketInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedClear();
	afx_msg void OnTbStartClickedWindows();													// *** 툴바 클릭 시 이벤트 함수
	afx_msg void OnTbStopClickedWindows();
	afx_msg void OnTbClearClickedWindows();
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);						// *** SORT 칼럼 클릭 했을 때 \

	// *** 쓰레드
	static UINT ThreadClient(LPVOID param);													// *** 소켓 통신 스레드
	static UINT PacketCaptureTFunction(LPVOID _method);										// *** PC 스레드

	afx_msg void OnBnClickedFilterButton();													// *** 필터 버튼 함수
	virtual BOOL PreTranslateMessage(MSG* pMsg);											// *** ESC, ENTER 로 꺼지는 거 제거 함수
};

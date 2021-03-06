#include "stdafx.h"
#include "MeeseeksVision.h"
#include "MeeseeksVisionDlg.h"
#include "AlgorithmDlg.h"
#include "CameraDlg.h"
#include "MeeseeksProperties.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#include <sstream>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern CString g_fileName;
extern bool g_bLoadFile, g_bSaveFile, g_bGetStats;
// CMeeseeksVisionDlg dialog
using namespace Gdiplus;

MeeseeksProperties g_mp;
CString            g_serverAddress;
int                g_serverPort;
bool               g_bSendParams = false;


CMeeseeksVisionDlg::CMeeseeksVisionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEESEEKSVISION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   m_serverPort     = theApp.GetProfileInt(L"Setup", L"ServerPort", 5800);
   m_serverAddress  = theApp.GetProfileString(L"Setup", L"ServerAddress", L"192.168.0.14");

   g_mp.Initialize();
}

void CMeeseeksVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_IP_ADDRESS, m_serverAddress);
   DDX_Text(pDX, IDC_PORT,       m_serverPort);

   DDX_Text(pDX, IDC_JPG_QUALITY, g_mp.algorithim.jpgQuality);
   DDX_Control(pDX, IDC_JPG_QUALITY_SLIDER, m_jpgQualitySlider);

   DDX_Radio(pDX, IDC_DISPLAY_TYPE,     g_mp.algorithim.displayType);

   DDX_Control(pDX, IDC_PARAM_FILES, m_paramFilesComboBox);

   DDX_Control(pDX, IDC_TAB, m_tab);
}

BEGIN_MESSAGE_MAP(CMeeseeksVisionDlg, CDialogEx)
	ON_WM_PAINT()
   ON_WM_HSCROLL()
	ON_WM_QUERYDRAGICON()
   ON_WM_DESTROY()
   ON_WM_TIMER()
   ON_BN_CLICKED(IDC_ZOOM_WINDOW, OnZoomWindow)
   ON_BN_CLICKED(IDC_START_STOP,  OnStartStop)
   ON_MESSAGE(WM_USER_NEW_PARAMS, OnNewParams)
   ON_MESSAGE(WM_USER_FILES,      OnFiles)
   ON_MESSAGE(WM_USER_NEW_STATS,  OnStats)

   ON_BN_CLICKED(IDC_DISPLAY_TYPE,     OnPropButtonClicked)
   ON_BN_CLICKED(IDC_DISPLAY_BLUR,     OnPropButtonClicked)
   ON_BN_CLICKED(IDC_DISPLAY_BW,       OnPropButtonClicked)
   ON_BN_CLICKED(IDC_DISPLAY_MORPH,    OnPropButtonClicked)
   ON_BN_CLICKED(IDC_DISPLAY_CONTOURS, OnPropButtonClicked)
   ON_BN_CLICKED(IDC_DISPLAY_TARGET,   OnPropButtonClicked)

   ON_BN_CLICKED(IDC_LOAD_FILE, OnLoadFile)
   ON_BN_CLICKED(IDC_SAVE_FILE, OnSaveFile)

   ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTabSelChange)

END_MESSAGE_MAP()

extern void SocketThreadFunc();

HWND g_hZoomWnd = NULL;
HWND g_hMainWnd = NULL;

#define STATS_TIMER 123

BOOL CMeeseeksVisionDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   m_tab.InsertItem(0, _T("Algorithm"));
   m_tab.InsertItem(1, _T("Camera"));
   m_tab.InsertItem(2, _T("Tape"));

   m_pAlgorithmDlg = new CAlgorithmDlg(&m_tab);
   m_pCameraDlg    = new CCameraDlg(&m_tab);
   m_pTapeDlg      = new CTapeDlg(&m_tab);

   TCITEM item;

   ZeroMemory(&item, sizeof(item));
   item.mask = TCIF_PARAM;

   int index = 0;

   item.lParam = (LPARAM)m_pAlgorithmDlg->m_hWnd;
   m_tab.SetItem(index++, &item);

   item.lParam = (LPARAM)m_pCameraDlg->m_hWnd;
   m_tab.SetItem(index++, &item);

   item.lParam = (LPARAM)m_pTapeDlg->m_hWnd;
   m_tab.SetItem(index++, &item);

   SizeTabWindows();

   m_pAlgorithmDlg->ShowWindow(SW_NORMAL);

   ULONG_PTR gdiplusToken;

   GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

   m_jpgQualitySlider.SetRange(0, 100);
   
   m_pZoomDlg = new CZoomDlg(this);

   m_pZoomDlg->ShowWindow(SW_HIDE);

   g_hZoomWnd = m_pZoomDlg->m_hWnd;

   g_hMainWnd = m_hWnd;

   m_hStopThread = ::CreateEvent(NULL, TRUE, FALSE, L"StopSocketThread");

   SetTimer(STATS_TIMER, 5000, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMeeseeksVisionDlg::OnTimer(UINT nIDEvent)
{
   if (nIDEvent == STATS_TIMER)
      g_bGetStats = true;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMeeseeksVisionDlg::OnDestroy()
{
   CDialogEx::OnDestroy();

   if (m_socketClientThread.joinable()) {

      ::SetEvent(m_hStopThread);

      m_socketClientThread.join();
   }

   ::CloseHandle(m_hStopThread);
}

void CMeeseeksVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMeeseeksVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMeeseeksVisionDlg::PreTranslateMessage(MSG *pMsg)
{
   if (pMsg->message == WM_KEYDOWN)
   {
      if (pMsg->wParam == VK_RETURN)
      {
         return TRUE;
      }
      if (pMsg->wParam == VK_ESCAPE)
      {
         return TRUE;
      }
   }

   return CDialog::PreTranslateMessage(pMsg);
}

void CMeeseeksVisionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
   int id = pSlider->GetDlgCtrlID();
   int pos = ((CSliderCtrl *)pScrollBar)->GetPos();

   UpdateData();

   CString strVal;

   switch (id)
   {
      case IDC_JPG_QUALITY_SLIDER:
         g_mp.algorithim.jpgQuality = pos;
         g_bSendParams = m_bHoldOffUpdate ? false : true;
         break;
   }

   UpdateData(FALSE);
}

void CMeeseeksVisionDlg::SetScrollBars()
{
   m_jpgQualitySlider.SetPos(g_mp.algorithim.jpgQuality);
}

void CMeeseeksVisionDlg::OnPropButtonClicked()
{
   UpdateData();

   g_bSendParams = true;
}

LRESULT CMeeseeksVisionDlg::OnNewParams(WPARAM wParam, LPARAM lParam)
{
   m_bHoldOffUpdate = true;

   UpdateData();

   char *pParams = (char *) wParam;

   g_mp.LoadFromString(std::string(pParams));

   delete []pParams;

   UpdateData(FALSE);

   SetScrollBars();

   m_pAlgorithmDlg->NewParams();
   m_pCameraDlg->NewParams();
   m_pTapeDlg->NewParams();

   return 0;
}

LRESULT CMeeseeksVisionDlg::OnFiles(WPARAM wParam, LPARAM lParam)
{
   m_bHoldOffUpdate = true;

   UpdateData();

   char *pParams = (char *)wParam;

   CStringA params(pParams);

   delete []pParams;

   int pos = 0;
   CStringA fileName = params.Tokenize("\n", pos);

   m_paramFilesComboBox.ResetContent();

   while (!fileName.IsEmpty()) {
      _bstr_t bstrName = (const char *) fileName;
      CString wideFileName = (const wchar_t *) bstrName;

      m_paramFilesComboBox.AddString(wideFileName);

      fileName = params.Tokenize("\n", pos);
   }
   UpdateData(FALSE);

   m_bHoldOffUpdate = false;

   return 0;
}

void CMeeseeksVisionDlg::OnZoomWindow()
{
   m_pZoomDlg->ShowWindow(m_pZoomDlg->IsWindowVisible() ? SW_HIDE : SW_NORMAL);
}

void CMeeseeksVisionDlg::OnStartStop()
{
   if (m_socketClientThread.joinable()) {
      ::SetEvent(m_hStopThread);
      m_socketClientThread.join();
      ::ResetEvent(m_hStopThread);
   }
   else
   { 
      UpdateData();

      g_serverAddress = m_serverAddress;
      g_serverPort    = m_serverPort;

      theApp.WriteProfileInt(L"Setup", L"ServerPort", m_serverPort);
      theApp.WriteProfileString(L"Setup", L"ServerAddress", m_serverAddress);

      m_socketClientThread = std::thread(SocketThreadFunc);
   }
}

void CMeeseeksVisionDlg::OnLoadFile()
{
   int sel = m_paramFilesComboBox.GetCurSel();

   if (sel == -1) {
      AfxMessageBox(L"No File Selected");
      return;
   }

   m_paramFilesComboBox.GetLBText(sel, g_fileName);

   g_bLoadFile = true;
}

void CMeeseeksVisionDlg::OnSaveFile()
{
   int sel = m_paramFilesComboBox.GetCurSel();

   if (sel == -1) {
      AfxMessageBox(L"No File Selected");
      return;
   }

   m_paramFilesComboBox.GetLBText(sel, g_fileName);

   g_bSaveFile = true;
}

StringVector g_stats;

LRESULT CMeeseeksVisionDlg::OnStats(WPARAM wParam, LPARAM lParam)
{
   char *pParams = (char *)wParam;

   std::istringstream buf(pParams);

   string line;

   g_stats.clear();

   while (std::getline(buf, line))
      g_stats.push_back(line);       

   delete[]pParams;

   return 0;
}

void CMeeseeksVisionDlg::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
   int tabIndex = m_tab.GetCurSel();

   for (int index = 0; index<m_tab.GetItemCount(); index++)
   {
      TCITEM item;

      ZeroMemory(&item, sizeof(item));

      item.mask = TCIF_PARAM;

      m_tab.GetItem(index, &item);

      HWND hWnd = (HWND)item.lParam;

      ::ShowWindow(hWnd, index == tabIndex ? SW_NORMAL : SW_HIDE);
   }

   *pResult = 0;
}

void CMeeseeksVisionDlg::SizeTabWindows()
{
   if (::IsWindow(m_tab.m_hWnd))
   {
      CRect rect;

      m_tab.GetClientRect(rect);

      for (int index = 0; index<m_tab.GetItemCount(); index++)
      {
         TCITEM item;

         ZeroMemory(&item, sizeof(item));

         item.mask = TCIF_PARAM;

         m_tab.GetItem(index, &item);

         HWND hWnd = (HWND)item.lParam;

         if (hWnd && ::IsWindow(hWnd))
         {      
            ::SetWindowPos(hWnd, 0, 0, 27, rect.Width(), rect.Height() - 28, SWP_NOZORDER);
         }
      }
   }
}

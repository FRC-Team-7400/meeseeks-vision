#include "stdafx.h"
#include "AlgorithmDlg.h"
#include "MeeseeksProperties.h"
#include "afxdialogex.h"
#include "resource.h"

extern CString g_fileName;
extern bool g_bLoadFile, g_bSaveFile, g_bGetStats;

extern MeeseeksProperties g_mp;
extern bool               g_bSendParams;


CAlgorithmDlg::CAlgorithmDlg(CWnd* pParent /*=nullptr*/)
{
   Create(IDD_ALGORITHIM, pParent);
}

void CAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_MIN_RED, g_mp.algorithim.minRed);
   DDX_Text(pDX, IDC_MAX_RED, g_mp.algorithim.maxRed);
   DDX_Control(pDX, IDC_MIN_RED_SLIDER, m_minRedSlider);
   DDX_Control(pDX, IDC_MAX_RED_SLIDER, m_maxRedSlider);

   DDX_Text(pDX, IDC_MIN_GREEN, g_mp.algorithim.minGreen);
   DDX_Text(pDX, IDC_MAX_GREEN, g_mp.algorithim.maxGreen);
   DDX_Control(pDX, IDC_MIN_GREEN_SLIDER, m_minGreenSlider);
   DDX_Control(pDX, IDC_MAX_GREEN_SLIDER, m_maxGreenSlider);

   DDX_Text(pDX, IDC_MIN_BLUE, g_mp.algorithim.minBlue);
   DDX_Text(pDX, IDC_MAX_BLUE, g_mp.algorithim.maxBlue);
   DDX_Control(pDX, IDC_MIN_BLUE_SLIDER, m_minBlueSlider);
   DDX_Control(pDX, IDC_MAX_BLUE_SLIDER, m_maxBlueSlider);

   DDX_Text(pDX, IDC_BLUR, g_mp.algorithim.blur);
   DDX_Control(pDX, IDC_BLUR_SLIDER, m_blurSlider);

   DDX_Text(pDX, IDC_ERODE, g_mp.algorithim.erode);
   DDX_Control(pDX, IDC_ERODE_SLIDER, m_erodeSlider);

   DDX_Text(pDX, IDC_DILATE, g_mp.algorithim.dilate);
   DDX_Control(pDX, IDC_DILATE_SLIDER, m_dilateSlider);

   DDX_Radio(pDX, IDC_COLOR_SPACE_RGB, g_mp.algorithim.colorSpace);

   DDX_Control(pDX, IDC_RED_STATIC, m_redStatic);
   DDX_Control(pDX, IDC_GREEN_STATIC, m_greenStatic);
   DDX_Control(pDX, IDC_BLUE_STATIC, m_blueStatic);
}

BEGIN_MESSAGE_MAP(CAlgorithmDlg, CDialogEx)
   ON_WM_HSCROLL()

   ON_BN_CLICKED(IDC_COLOR_SPACE_RGB, OnPropButtonRGBClicked)
   ON_BN_CLICKED(IDC_COLOR_SPACE_HSV, OnPropButtonHSVClicked)

END_MESSAGE_MAP()

BOOL CAlgorithmDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   m_minRedSlider.SetRange(0, 255);
   m_maxRedSlider.SetRange(0, 255);

   m_minGreenSlider.SetRange(0, 255);
   m_maxGreenSlider.SetRange(0, 255);

   m_minBlueSlider.SetRange(0, 255);
   m_maxBlueSlider.SetRange(0, 255);

   m_blurSlider.SetRange(0, 128);

   m_erodeSlider.SetRange(0, 32);
   m_dilateSlider.SetRange(0, 32);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAlgorithmDlg::PostNcDestroy()
{
   delete this;
}

BOOL CAlgorithmDlg::PreTranslateMessage(MSG *pMsg)
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

void CAlgorithmDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
   int id = pSlider->GetDlgCtrlID();
   int pos = ((CSliderCtrl *)pScrollBar)->GetPos();

   UpdateData();

   CString strVal;

   switch (id)
   {
   case IDC_MIN_RED_SLIDER:
      g_mp.algorithim.minRed = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_RED_SLIDER:
      g_mp.algorithim.maxRed = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_GREEN_SLIDER:
      g_mp.algorithim.minGreen = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_GREEN_SLIDER:
      g_mp.algorithim.maxGreen = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_BLUE_SLIDER:
      g_mp.algorithim.minBlue = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_BLUE_SLIDER:
      g_mp.algorithim.maxBlue = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_BLUR_SLIDER:
      g_mp.algorithim.blur = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_ERODE_SLIDER:
      g_mp.algorithim.erode = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_DILATE_SLIDER:
      g_mp.algorithim.dilate = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;

   }

   UpdateData(FALSE);
}

void CAlgorithmDlg::SetScrollBars()
{
   m_minRedSlider.SetPos(g_mp.algorithim.minRed);
   m_maxRedSlider.SetPos(g_mp.algorithim.maxRed);
   m_minGreenSlider.SetPos(g_mp.algorithim.minGreen);
   m_maxGreenSlider.SetPos(g_mp.algorithim.maxGreen);
   m_minBlueSlider.SetPos(g_mp.algorithim.minBlue);
   m_maxBlueSlider.SetPos(g_mp.algorithim.maxBlue);
   m_blurSlider.SetPos(g_mp.algorithim.blur);
   m_erodeSlider.SetPos(g_mp.algorithim.erode);
   m_dilateSlider.SetPos(g_mp.algorithim.dilate);
}

void CAlgorithmDlg::OnPropButtonClicked()
{
   UpdateData();

   g_bSendParams = true;
}

void CAlgorithmDlg::SetRGBHSV()
{
   UpdateData();

   if (g_mp.algorithim.colorSpace == 0) {
      m_redStatic.SetWindowText(L"R");
      m_greenStatic.SetWindowText(L"G");
      m_blueStatic.SetWindowText(L"B");
   }
   else {
      m_redStatic.SetWindowText(L"H");
      m_greenStatic.SetWindowText(L"S");
      m_blueStatic.SetWindowText(L"V");
   }
}

void CAlgorithmDlg::OnPropButtonRGBClicked()
{
   SetRGBHSV();

   OnPropButtonClicked();
}

void CAlgorithmDlg::OnPropButtonHSVClicked()
{
   SetRGBHSV();

   OnPropButtonClicked();
}

void CAlgorithmDlg::NewParams()
{
   m_bHoldOffUpdate = true;

   UpdateData(FALSE);

   SetScrollBars();

   m_bHoldOffUpdate = false;

   SetRGBHSV();
}
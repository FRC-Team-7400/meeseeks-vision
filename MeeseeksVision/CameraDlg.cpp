#include "stdafx.h"
#include "CameraDlg.h"
#include "MeeseeksProperties.h"
#include "afxdialogex.h"
#include "resource.h"

extern CString g_fileName;
extern bool g_bLoadFile, g_bSaveFile, g_bGetStats;

extern MeeseeksProperties g_mp;
extern bool               g_bSendParams;


CCameraDlg::CCameraDlg(CWnd* pParent /*=nullptr*/)
{
   m_bHoldOffUpdate = false;

   m_autoExposure = g_mp.camera.autoExposure == 1 ? 0 : 3;

   Create(IDD_CAMERA, pParent);
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_ZOOM, g_mp.camera.zoom);
   DDX_Control(pDX, IDC_ZOOM_SLIDER, m_zoomSlider);
   
   DDX_Text(pDX, IDC_BRIGHTNESS, g_mp.camera.brightness);
   DDX_Control(pDX, IDC_BRIGHTNESS_SLIDER, m_brightnessSlider);

   DDX_Text(pDX, IDC_CONTRAST, g_mp.camera.contrast);
   DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_contrastSlider);

   DDX_Text(pDX, IDC_EXPOSURE, g_mp.camera.exposureAbsolute);
   DDX_Control(pDX, IDC_EXPOSURE_SLIDER, m_exposureSlider);

   DDX_Text(pDX, IDC_SATURATION, g_mp.camera.saturation);
   DDX_Control(pDX, IDC_SATURATION_SLIDER, m_saturationSlider);

   DDX_Text(pDX, IDC_GAIN, g_mp.camera.gain);
   DDX_Control(pDX, IDC_GAIN_SLIDER, m_gainSlider);

   DDX_Text(pDX, IDC_SHARPNESS, g_mp.camera.sharpness);
   DDX_Control(pDX, IDC_SHARPNESS_SLIDER, m_sharpnessSlider);

   DDX_Text(pDX, IDC_WB_TEMP, g_mp.camera.whiteBalanceTemperature);
   DDX_Control(pDX, IDC_WB_TEMP_SLIDER, m_whiteBalanceSlider);

   DDX_Text(pDX, IDC_FOCUS, g_mp.camera.focus);
   DDX_Control(pDX, IDC_FOCUS_SLIDER, m_focusSlider);
   
   DDX_Text(pDX, IDC_GAIN, g_mp.camera.gain);
   DDX_Control(pDX, IDC_GAIN_SLIDER, m_gainSlider);

   DDX_Radio(pDX, IDC_EXPOSURE_MANUAL, m_autoExposure);

   DDX_Radio(pDX, IDC_EXPOSURE_MANUAL, m_autoExposure);

   DDX_Radio(pDX, IDC_WHITE_BALANCE_MANUAL, g_mp.camera.autoWhiteBalance);

   DDX_Radio(pDX, IDC_FOCUS_MANUAL, g_mp.camera.autoFocus);
}

BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_EXPOSURE_MANUAL, OnExposureManual)
   ON_BN_CLICKED(IDC_EXPOSURE_APERTURE_PRIORITY, OnExposureAperturePriority)
   ON_BN_CLICKED(IDC_WHITE_BALANCE_MANUAL, OnPropButtonClicked)
   ON_BN_CLICKED(IDC_WHITE_BALANCE_AUTO, OnPropButtonClicked)
   ON_BN_CLICKED(IDC_FOCUS_MANUAL, OnPropButtonClicked)
   ON_BN_CLICKED(IDC_FOCUS_AUTO, OnPropButtonClicked)
END_MESSAGE_MAP()

BOOL CCameraDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   m_zoomSlider.SetRange(100, 500);
   m_brightnessSlider.SetRange(0, 255);
   m_contrastSlider.SetRange(0, 255);
   m_exposureSlider.SetRange(3, 2047);
   m_sharpnessSlider.SetRange(0, 255);
   m_saturationSlider.SetRange(0, 255);
   m_gainSlider.SetRange(0, 255);
   m_whiteBalanceSlider.SetRange(2000, 6500);
   m_focusSlider.SetRange(0, 250);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCameraDlg::PostNcDestroy()
{
   delete this;
}

BOOL CCameraDlg::PreTranslateMessage(MSG *pMsg)
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

void CCameraDlg::SetScrollBars()
{
   m_zoomSlider.SetPos(g_mp.camera.zoom);
   m_brightnessSlider.SetPos(g_mp.camera.brightness);
   m_contrastSlider.SetPos(g_mp.camera.contrast);
   m_exposureSlider.SetPos(g_mp.camera.exposureAbsolute);
   m_saturationSlider.SetPos(g_mp.camera.saturation);
   m_sharpnessSlider.SetPos(g_mp.camera.sharpness);
   m_gainSlider.SetPos(g_mp.camera.gain);
   m_whiteBalanceSlider.SetPos(g_mp.camera.whiteBalanceTemperature);
   m_focusSlider.SetPos(g_mp.camera.focus);
}

void CCameraDlg::NewParams()
{
   m_bHoldOffUpdate = true;

   if (g_mp.camera.autoExposure == 1)
      m_autoExposure = 0;
   else
      m_autoExposure = 1;

   UpdateData(FALSE);

   SetScrollBars();

   m_bHoldOffUpdate = false;
}

void CCameraDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
   int id = pSlider->GetDlgCtrlID();
   int pos = ((CSliderCtrl *)pScrollBar)->GetPos();

   UpdateData();

   CString strVal;

   switch (id)
   {
   case IDC_ZOOM_SLIDER:
      g_mp.camera.zoom = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_BRIGHTNESS_SLIDER:
      g_mp.camera.brightness = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_CONTRAST_SLIDER:
      g_mp.camera.contrast = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_EXPOSURE_SLIDER:
      g_mp.camera.exposureAbsolute = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_SHARPNESS_SLIDER:
      g_mp.camera.sharpness = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_SATURATION_SLIDER:
      g_mp.camera.saturation = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_GAIN_SLIDER:
      g_mp.camera.gain = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_WB_TEMP_SLIDER:
      g_mp.camera.whiteBalanceTemperature = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_FOCUS_SLIDER:
      g_mp.camera.focus = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   }

   UpdateData(FALSE);
}

void CCameraDlg::OnExposureManual()
{
   g_mp.camera.autoExposure = 1;
   g_bSendParams = true;
}

void CCameraDlg::OnExposureAperturePriority()
{
   g_mp.camera.autoExposure = 3;
   g_bSendParams = true;
}

void CCameraDlg::OnPropButtonClicked()
{
   UpdateData();
   g_bSendParams = true;
}

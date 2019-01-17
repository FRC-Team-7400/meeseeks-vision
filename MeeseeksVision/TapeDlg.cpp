#include "stdafx.h"
#include "TapeDlg.h"
#include "MeeseeksProperties.h"
#include "afxdialogex.h"
#include "resource.h"

extern CString g_fileName;
extern bool g_bLoadFile, g_bSaveFile, g_bGetStats;

extern MeeseeksProperties g_mp;
extern bool               g_bSendParams;


CTapeDlg::CTapeDlg(CWnd* pParent /*=nullptr*/)
{
   Create(IDD_TAPE, pParent);
}

void CTapeDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_MIN_TAPE_AREA, g_mp.algorithim.minTapeArea);
   DDX_Control(pDX, IDC_MIN_TAPE_AREA_SLIDER, m_minTapeAreaSlider);

   DDX_Text(pDX, IDC_MIN_TAPE_ANGLE, g_mp.algorithim.minTapeAngle);
   DDX_Text(pDX, IDC_MAX_TAPE_ANGLE, g_mp.algorithim.maxTapeAngle);

   DDX_Control(pDX, IDC_MIN_TAPE_ANGLE_SLIDER, m_minTapeAngleSlider);
   DDX_Control(pDX, IDC_MAX_TAPE_ANGLE_SLIDER, m_maxTapeAngleSlider);

   DDX_Text(pDX, IDC_MIN_TAPE_HEIGHT_RATIO, g_mp.algorithim.minTapeHeightRatio);
   DDX_Text(pDX, IDC_MAX_TAPE_HEIGHT_RATIO, g_mp.algorithim.maxTapeHeightRatio);

   DDX_Control(pDX, IDC_MIN_TAPE_HEIGHT_RATIO_SLIDER, m_minTapeHeightRatioSlider);
   DDX_Control(pDX, IDC_MAX_TAPE_HEIGHT_RATIO_SLIDER, m_maxTapeHeightRatioSlider);

   DDX_Text(pDX, IDC_MIN_TAPE_Y_DIST_RATIO, g_mp.algorithim.minTapeYDistRatio);
   DDX_Text(pDX, IDC_MAX_TAPE_Y_DIST_RATIO, g_mp.algorithim.maxTapeYDistRatio);

   DDX_Control(pDX, IDC_MIN_TAPE_Y_DIST_RATIO_SLIDER, m_minTapeYDistRatioSlider);
   DDX_Control(pDX, IDC_MAX_TAPE_Y_DIST_RATIO_SLIDER, m_maxTapeYDistRatioSlider);

   DDX_Text(pDX, IDC_MIN_TAPE_X_DIST_RATIO, g_mp.algorithim.minTapeXDistRatio);
   DDX_Text(pDX, IDC_MAX_TAPE_X_DIST_RATIO, g_mp.algorithim.maxTapeXDistRatio);
  
   DDX_Control(pDX, IDC_MIN_TAPE_X_DIST_RATIO_SLIDER, m_minTapeXDistRatioSlider);
   DDX_Control(pDX, IDC_MAX_TAPE_X_DIST_RATIO_SLIDER, m_maxTapeXDistRatioSlider);
}

BEGIN_MESSAGE_MAP(CTapeDlg, CDialogEx)
   ON_WM_HSCROLL()

END_MESSAGE_MAP()

BOOL CTapeDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   m_minTapeAreaSlider.SetRange(0, 500);

   m_minTapeAngleSlider.SetRange(0, 180);
   m_maxTapeAngleSlider.SetRange(0, 180);

   m_minTapeHeightRatioSlider.SetRange(0, 500);
   m_maxTapeHeightRatioSlider.SetRange(0, 500);

   m_minTapeYDistRatioSlider.SetRange(0, 100);
   m_maxTapeYDistRatioSlider.SetRange(0, 100);

   m_minTapeXDistRatioSlider.SetRange(0, 300);
   m_maxTapeXDistRatioSlider.SetRange(0, 300);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTapeDlg::PostNcDestroy()
{
   delete this;
}

BOOL CTapeDlg::PreTranslateMessage(MSG *pMsg)
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

void CTapeDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
   int id = pSlider->GetDlgCtrlID();
   int pos = ((CSliderCtrl *)pScrollBar)->GetPos();

   UpdateData();

   CString strVal;

   switch (id)
   {
   case IDC_MIN_TAPE_AREA_SLIDER:
      g_mp.algorithim.minTapeArea = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_TAPE_ANGLE_SLIDER:
      g_mp.algorithim.minTapeAngle = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_TAPE_ANGLE_SLIDER:
      g_mp.algorithim.maxTapeAngle = pos;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_TAPE_HEIGHT_RATIO_SLIDER:
      g_mp.algorithim.minTapeHeightRatio = ((double) pos) / 100.0;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_TAPE_HEIGHT_RATIO_SLIDER:
      g_mp.algorithim.maxTapeHeightRatio = ((double)pos) / 100.0;;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_TAPE_Y_DIST_RATIO_SLIDER:
      g_mp.algorithim.minTapeYDistRatio = ((double)pos) / 100.0;;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_TAPE_Y_DIST_RATIO_SLIDER:
      g_mp.algorithim.maxTapeYDistRatio = ((double)pos) / 100.0;;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MIN_TAPE_X_DIST_RATIO_SLIDER:
      g_mp.algorithim.minTapeXDistRatio = ((double)pos) / 100.0;;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;
   case IDC_MAX_TAPE_X_DIST_RATIO_SLIDER:
      g_mp.algorithim.maxTapeXDistRatio = ((double)pos) / 100.0;;
      g_bSendParams = m_bHoldOffUpdate ? false : true;
      break;

   }

   UpdateData(FALSE);
}

void CTapeDlg::SetScrollBars()
{
   m_minTapeAreaSlider.SetPos(g_mp.algorithim.minTapeArea);
   m_minTapeAngleSlider.SetPos(g_mp.algorithim.minTapeAngle);
   m_maxTapeAngleSlider.SetPos(g_mp.algorithim.maxTapeAngle);
   m_minTapeHeightRatioSlider.SetPos(g_mp.algorithim.minTapeHeightRatio * 100.0);
   m_maxTapeHeightRatioSlider.SetPos(g_mp.algorithim.maxTapeHeightRatio * 100.0);
   m_minTapeYDistRatioSlider.SetPos(g_mp.algorithim.minTapeYDistRatio   * 100.0);
   m_maxTapeYDistRatioSlider.SetPos(g_mp.algorithim.maxTapeYDistRatio   * 100.0);
   m_minTapeXDistRatioSlider.SetPos(g_mp.algorithim.minTapeXDistRatio   * 100.0);
   m_maxTapeXDistRatioSlider.SetPos(g_mp.algorithim.maxTapeXDistRatio   * 100.0);
}

void CTapeDlg::OnPropButtonClicked()
{
   UpdateData();

   g_bSendParams = true;
}

void CTapeDlg::NewParams()
{
   m_bHoldOffUpdate = true;

   UpdateData(FALSE);

   SetScrollBars();

   m_bHoldOffUpdate = false;
}
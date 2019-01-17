#pragma once
// CMeeseeksVisionDlg dialog
class CTapeDlg : public CDialogEx
{
   // Construction
public:
   CTapeDlg(CWnd* pParent = nullptr);	// standard constructor

   void NewParams();

protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   virtual void PostNcDestroy();


                                                      // Implementation
protected:
   CSliderCtrl	  m_minTapeAreaSlider, m_minTapeAngleSlider, m_maxTapeAngleSlider, m_minTapeHeightRatioSlider, m_maxTapeHeightRatioSlider;
   CSliderCtrl   m_minTapeYDistRatioSlider, m_maxTapeYDistRatioSlider, m_minTapeXDistRatioSlider, m_maxTapeXDistRatioSlider;
   bool          m_bHoldOffUpdate;   

   // Generated message map functions
   BOOL OnInitDialog();
   virtual BOOL PreTranslateMessage(MSG *pMsg);

   void SetScrollBars();

   afx_msg void OnPropButtonClicked();


   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   DECLARE_MESSAGE_MAP()
};



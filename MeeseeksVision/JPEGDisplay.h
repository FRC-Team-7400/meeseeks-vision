#pragma once

class CJPEGDisplay : public CStatic
{
	public    : CJPEGDisplay();
				
               void SetBuffer(BYTE *pBuffer, int bufferLen);
               void SetSize(int width, int height) {
                  m_width = width;
                  m_height = height;
               }
	protected :
					afx_msg void OnPaint();
					afx_msg BOOL OnEraseBkgnd(CDC *pDC);

					IStream *m_pStream;
               Gdiplus::Font *m_pFont;
               Gdiplus::Brush *m_pBrush;
               int      m_width, m_height;

	DECLARE_MESSAGE_MAP()
};

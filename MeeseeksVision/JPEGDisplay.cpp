#include "stdafx.h"
#include "JPEGDisplay.h"
#include <gdiplus.h>
#include "MeeseeksProperties.h"
#pragma comment (lib, "gdiplus")
using namespace Gdiplus;

BEGIN_MESSAGE_MAP(CJPEGDisplay, CStatic)

	ON_WM_PAINT()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

CJPEGDisplay::CJPEGDisplay()
{
	m_pStream = NULL;
   m_width = 640;
   m_height = 360;
   m_pFont = NULL;
   m_pBrush = NULL;
}

BOOL CJPEGDisplay::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

extern StringVector g_stats;

void CJPEGDisplay::OnPaint()
{
	CRect rect;

	GetClientRect(rect);

	CPaintDC dc(this);

	CDC dcMem;
	CBitmap bitmap;

	dcMem.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	Graphics graphics(dcMem);
	Image image(m_pStream);

   graphics.DrawImage(&image, 0, 0, m_width, m_height);

   int y = 40;

   for (auto iter = g_stats.begin(); iter != g_stats.end();  iter++)
   {
      _bstr_t bstrStr = (const char *) iter->c_str();

      graphics.DrawString((const wchar_t *) bstrStr, iter->length(), m_pFont, PointF(100, y), m_pBrush);

      y += 24;
   }

	CPen pen(PS_SOLID, 1, RGB(255, 255, 255));

	dcMem.SelectObject(pen);

	dcMem.MoveTo(CPoint(m_width / 2, m_height / 2 - 10));
	dcMem.LineTo(CPoint(m_width / 2, m_height / 2 + 10));

	dcMem.MoveTo(CPoint(m_width / 2 - 10, m_height / 2));
	dcMem.LineTo(CPoint(m_width / 2 + 10, m_height / 2));

	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldBitmap);
}

void CJPEGDisplay::SetBuffer(BYTE *pBuffer, int len)
{
   if (!m_pFont)
   {
      Gdiplus::FontFamily fontFamily(L"Arial");
      m_pFont = new Gdiplus::Font(&fontFamily, 20, FontStyleRegular, UnitPixel);
   }

   if (!m_pBrush)
   {
      m_pBrush = new Gdiplus::SolidBrush(Color(255, 0, 255, 0));
   }

	if (m_pStream)
	{
		m_pStream->Release();
	}

	m_pStream = SHCreateMemStream(pBuffer, len);		
	
	Invalidate();			
}
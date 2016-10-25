#pragma once

#include <utility>
#include <vector>

namespace Utils
{
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
	void CalcHistogram(uint32_t *pLine, void* scan0, BYTE stride, Gdiplus::Bitmap *&pBitmap, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas);
}
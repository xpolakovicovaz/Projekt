#pragma once

#include <utility>
#include <vector>

namespace Utils
{
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
	void CalcHistogram(void* scan0,int zaciatok, int koniec, BYTE stride, int s, int v, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas);
}
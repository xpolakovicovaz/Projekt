#pragma once

#include <utility>
#include <vector>
#include <functional>

namespace Utils
{
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
	void CalcHistogram(void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn);
	void multi_thread(int pt, int dlzka, void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::function<bool()> fn);
	void posterizuj(int pf, void* scan0, void* novescan0, int zaciatok, int koniec, BYTE stride,BYTE novestride, int s, std::function<bool()> fn);
	void multi_thread_poster(int pt, int pf, int dlzka, void* scan0, void* novescan0, int zaciatok, int koniec, BYTE stride, BYTE novestride, int s, std::function<bool()> fn);
	void novehist(int pf, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::vector<int> &novered, std::vector<int> &novegreen, std::vector<int> &noveblue, std::vector<int> &novejas);
}
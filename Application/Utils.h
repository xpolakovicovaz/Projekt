#pragma once



#include <utility>

#include <vector>

#include "ApplicationDlg.h"



namespace Utils

{

	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);

	void CalcHistogram(void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::thread::id m_thread_id);
	void multi_thread(int pt, int dlzka, void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::thread::id m_thread_id);
}
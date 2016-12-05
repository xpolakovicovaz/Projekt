// Library.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <GdiPlus.h>
#include "Library.h"
#include "../Application/Utils.h"
#include <functional>


//// This is an example of an exported variable
//LIBRARY_API int nLibrary=0;
//
//// This is an example of an exported function.
//LIBRARY_API int fnLibrary(void)
//{
//    return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see Library.h for the class definition
//CLibrary::CLibrary()
//{
//    return;
//}

//	parse file names from file name string in OPENFILENAME struct
//	returns pair of directory and vector of filenames
//
LIBRARY_API std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile)
{
	return Utils::ParseFiles(lpstrFile);
}

LIBRARY_API void CalcHistogram(void* scan0, int zaciatok, int koniec, int stride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn)
{
	return Utils::CalcHistogram(scan0, zaciatok, koniec, stride, s, red, green, blue, jas, fn);
}

LIBRARY_API void multi_thread(int pt, int dlzka, void * scan0, int zaciatok, int koniec, int stride, int s, std::vector<std::vector<int>>& red, std::vector<std::vector<int>>& green, std::vector<std::vector<int>>& blue, std::vector<std::vector<int>>& jas, std::function<bool()> fn)
{
	return Utils::multi_thread(pt, dlzka, scan0, zaciatok, koniec, stride, s, std::ref(red), std::ref(green), std::ref(blue), std::ref(jas), fn);
}

LIBRARY_API void multi_thread_poster(int pt, int pf, int dlzka, void* scan0, void* novescan0, int zaciatok, int koniec, int stride, int novestride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::function<bool()> fn)
{
	return Utils::multi_thread_poster(pt, pf, dlzka, scan0, novescan0, zaciatok, koniec, stride, novestride, s,red, green, blue,jas, fn);
}

LIBRARY_API void posterizuj(int pf, void* scan0, void* novescan0, int zaciatok, int koniec, int stride, int novestride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn)
{
	return Utils::posterizuj(pf, scan0, novescan0, zaciatok, koniec, stride, novestride, s,red, green, blue, jas, fn);
}
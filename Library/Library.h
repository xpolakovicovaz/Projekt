// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBRARY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBRARY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBRARY_EXPORTS
#define LIBRARY_API __declspec(dllexport)
#else
#define LIBRARY_API __declspec(dllimport)
#endif

#include<functional>

// This class is exported from the Library.dll
//class LIBRARY_API CLibrary {
//public:
//	CLibrary(void);
//	// TODO: add your methods here.
//};
//
//extern LIBRARY_API int nLibrary;
//
//LIBRARY_API int fnLibrary(void);

LIBRARY_API std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
LIBRARY_API void CalcHistogram(void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn);
LIBRARY_API void multi_thread(int pt, int dlzka, void* scan0, int zaciatok, int koniec, int stride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::function<bool()> fn);
LIBRARY_API	void posterizuj(int pf, void* scan0, void* novescan0, int zaciatok, int koniec, int stride, int novestride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn);
LIBRARY_API void multi_thread_poster(int pt, int pf, int dlzka, void* scan0, void* novescan0, int zaciatok, int koniec, int stride, int novestride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::function<bool()> fn);

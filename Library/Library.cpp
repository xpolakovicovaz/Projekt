// Library.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <GdiPlus.h>
#include "Library.h"
#include "../Application/Utils.h"


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

LIBRARY_API void CalcHistogram(uint32_t *pLine, void* scan0, BYTE stride, Gdiplus::Bitmap *&pBitmap, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas)
{
	int r, g, b;
	for (int y = 0; y < pBitmap->GetHeight(); y++)
	{
		for (int x = 0; x < pBitmap->GetWidth(); x++)
		{
			r = ((*pLine) >> 16) & 0xff;
			g = ((*pLine) >> 8) & 0xff;
			b = (*pLine) & 0xff;
			red[r]++;
			green[g]++;
			blue[b]++;
			jas[int(0.2126*r + 0.7152*g + 0.0722*b)]++;
			pLine++;
		}
		pLine = (uint32_t*)((uint8_t*)scan0 + stride*(y + 1));
	}
}
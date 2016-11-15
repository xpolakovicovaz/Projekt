#include "stdafx.h"
#include "Utils.h"

namespace Utils
{
	//	parse file names from file name string in OPENFILENAME struct
	//	returns pair of directory and vector of filenames
	//
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile)
	{
		CString cs = lpstrFile;

		// skip directory name
		while (*lpstrFile) ++lpstrFile;

		if (*(++lpstrFile))
		{
			CString csDirectory;
			std::vector<CString> names;

			csDirectory = cs + _T("\\");
			// iterate filenames
			for ( ; *lpstrFile; ++lpstrFile)
			{
				names.push_back(lpstrFile);

				while (*lpstrFile) ++lpstrFile;
			}

			return std::make_pair(csDirectory, names);
		}
		else
		{	// only one filename
			CString csName, csExt;
			_tsplitpath_s(cs, nullptr, 0, nullptr, 0, csName.GetBuffer(_MAX_FNAME), _MAX_FNAME, csExt.GetBuffer(_MAX_EXT), _MAX_EXT);
			csName.ReleaseBuffer();
			csExt.ReleaseBuffer();

			return std::make_pair(cs.Left(cs.GetLength() - csName.GetLength() - csExt.GetLength()), std::vector<CString>({ csName + csExt }));
		}

	}

	void CalcHistogram(void* scan0,int zaciatok, int koniec, BYTE stride, int s, int v, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas)
	{
		int r, g, b;
		uint32_t *pLine = (uint32_t*)((uint8_t*)scan0 + stride*(zaciatok));
		for (int y = zaciatok; y < koniec; y++)
		{
			for (int x = 0; x < s; x++)
			{
				r = ((*pLine) >> 16) & 0xff;
				g = ((*pLine) >> 8) & 0xff;
				b = (int)(*pLine) & 0xff;
				red[r]++;
				green[g]++;
				blue[b]++;
				jas[(int)(0.2126*r + 0.7152*g + 0.0722*b)]++;
				pLine++;
			}
			pLine = (uint32_t*)((uint8_t*)scan0 + stride*(y + 1));
		}
		//delete (pLine);
	}

}

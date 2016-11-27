#include "stdafx.h"
#include "Utils.h"
#include <functional>
#include <thread>


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

	void CalcHistogram(void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, std::function<bool()> fn)
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
			if (fn())
				return;
		}
		return;
	}

	void multi_thread(int pt, int dlzka, void* scan0, int zaciatok, int koniec, BYTE stride, int s, std::vector<std::vector<int>> &red, std::vector<std::vector<int>> &green, std::vector<std::vector<int>> &blue, std::vector<std::vector<int>> &jas, std::function<bool()> fn)
	{
		std::vector<std::thread> tred(pt);
		for (int i = 0; i < pt - 1; i++)
		{
			tred[i] = std::thread(&Utils::CalcHistogram, scan0, i*dlzka, (i + 1)*dlzka, stride, s, std::ref(red[i]), std::ref(green[i]), std::ref(blue[i]), std::ref(jas[i]), fn);
		}
		Utils::CalcHistogram(scan0, (pt - 1)*dlzka, koniec, stride, s, std::ref(red[pt - 1]), std::ref(green[pt - 1]), std::ref(blue[pt - 1]), std::ref(jas[pt - 1]), fn);

		for (int i = 0; i < pt - 1; i++)
		{
			tred[i].join();
		}
	}


}

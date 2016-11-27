#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Library/Library.h"

#include <vector>
#include <functional>
#include <thread>

namespace UnitTest
{
	TEST_CLASS(ParseFilesUnitTest)
	{
	public:

		TEST_METHOD(ParseFiles_TestOneFile)
		{
			auto result = ParseFiles(L"C:\\directory\\subdirectory\\file.name.ext\0");
			Assert::AreEqual(result.first, L"C:\\directory\\subdirectory\\", L"directory");
			Assert::IsTrue(result.second.size() == 1, L"count of files");
			Assert::AreEqual(result.second[0], L"file.name.ext", "filename");
		}

		TEST_METHOD(ParseFiles_TestMultipleFiles)
		{
			auto result = ParseFiles(L"C:\\directory\\subdirectory\0file1.name.ext\0file2.name.ext\0");
			Assert::AreEqual(result.first, L"C:\\directory\\subdirectory\\", L"directory");
			Assert::IsTrue(result.second.size() == 2, L"count of files");
			Assert::AreEqual(result.second[0], L"file1.name.ext", "filename 1");
			Assert::AreEqual(result.second[1], L"file2.name.ext", "filename 2");
		}
	};

	TEST_CLASS(TestHistogram)
	{
		TEST_METHOD(TestHist)
		{
			int pt = 3;
			int d = 256;
		/*	std::vector<int> r, g, b, j;
			r.assign(256, 0);
			g.assign(256, 0);
			b.assign(256, 0);
			j.assign(256, 0);
			*/
			uint32_t pBitmap[256][256];
			memset(pBitmap, 0, sizeof(uint32_t)*d*d);
			for (int x = 0; x < d; x++) {
				for (int y = 0; y < d; y++) {
					pBitmap[x][y] = 16734208;// 0xff8800;
				}
			}

			std::vector<std::vector<int>> HistRed1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistBright1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistGreen1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistBlue1(pt, std::vector<int>(256));
			int dlzka = d / pt;
			//				multi_thread(pt, dlzka, Bdata.Scan0, 0, pBitmap->GetHeight(), Bdata.Stride, pBitmap->GetWidth(), std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), m_thread_id);


			multi_thread(pt, dlzka, pBitmap, 0, d, d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });

			for (int qwe = 0; qwe < pt - 1; qwe++) {
				Assert::AreEqual(HistRed1[qwe][255], dlzka*d, L"cerveny1");
				Assert::AreEqual(HistGreen1[qwe][88], dlzka*d, L"zeleny1");
				Assert::AreEqual(HistBlue1[qwe][0], dlzka*d, L"modry1");
				Assert::AreEqual(HistBright1[qwe][(int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0)], dlzka*d, L"jas");
				for (int i = 0; i < 256; i++)
				{
					if (i != 255) Assert::AreEqual(HistRed1[qwe][i], 0, L"cerveny0");
					if (i != 88) Assert::AreEqual(HistGreen1[qwe][i], 0, L"zeleny0");
					if (i != 0) Assert::AreEqual(HistBlue1[qwe][i], 0, L"modry0");
					if (i != (int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0))Assert::AreEqual(HistBright1[qwe][i], 0, L"jas0");
				}
			};
			Assert::AreEqual(HistRed1[pt-1][255], d*(d - (pt - 1)*dlzka), L"cervenyposledny");
			Assert::AreEqual(HistGreen1[pt-1][88], d*(d - (pt - 1)*dlzka), L"zelenyposledny");
			Assert::AreEqual(HistBlue1[pt-1][0], d*(d - (pt - 1)*dlzka), L"modryposledny");
			Assert::AreEqual(HistBright1[pt-1][(int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0)], d*(d - (pt - 1)*dlzka), L"jas");
			for (int i = 0; i < 256; i++)
			{
				if (i != 255) Assert::AreEqual(HistRed1[pt-1][i], 0, L"cerveny0posledny");
				if (i != 88) Assert::AreEqual(HistGreen1[pt-1][i], 0, L"zeleny0posledny");
				if (i != 0) Assert::AreEqual(HistBlue1[pt-1][i], 0, L"modry0posledny");
				if (i != (int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0))Assert::AreEqual(HistBright1[pt-1][i], 0, L"jas0");
			}

		}
	
	};
}

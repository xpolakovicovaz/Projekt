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
			int pt = 4;
			int d = 256;

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

	TEST_CLASS(TestEfekt)
	{
		TEST_METHOD(TestPoster_histogram)
		{
			int pt = 1;
			int d = 256;
			int pf = 128;
			int r, g, b;
			int dlzka = d / pt;

			uint32_t pBitmap[256][256];
			uint32_t noveBitmap[256][256];
			uint32_t kontrolBitmap[256][256];

			std::vector<std::vector<int>> HistRed1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistBright1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistGreen1(pt, std::vector<int>(256));
			std::vector<std::vector<int>> HistBlue1(pt, std::vector<int>(256));
			
			std::vector<int> red(d, 0);
			std::vector<int> green(d, 0);
			std::vector<int> blue(d, 0);
			std::vector<int> jas(d, 0);

			std::vector<int> novered(d, 0);
			std::vector<int> novegreen(d, 0);
			std::vector<int> noveblue(d, 0);
			std::vector<int> novejas(d, 0);

			memset(pBitmap, 0, sizeof(uint32_t)*d*d);
			memset(noveBitmap, 0, sizeof(uint32_t)*d*d);
			memset(kontrolBitmap, 0, sizeof(uint32_t)*d*d);

			for (int x = 0; x < d; x++) {
				for (int y = 0; y < d; y++) {
					pBitmap[x][y] = x * 256 * 256 + y * 256 + x;
					r = max(0, min(255, (x / pf + 1)*pf));
					g = max(0, min(255, (y / pf + 1)*pf));
					b = max(0, min(255, (x / pf + 1)*pf));
					kontrolBitmap[x][y] = r * 256 * 256 + g * 256 + b;
				}
			}


			multi_thread(pt, dlzka, kontrolBitmap, 0, d, d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });
			
			for (int j = 0; j<pt; j++)
				for (int i = 0; i <= 255; i++)
				{
					red[i] += HistRed1[j][i];
					green[i] += HistGreen1[j][i];
					blue[i] += HistBlue1[j][i];
					jas[i] += HistBright1[j][i];
				}

			novehist(pf, std::ref(red), std::ref(green), std::ref(blue), std::ref(jas), std::ref(novered), std::ref(novegreen), std::ref(noveblue), std::ref(novejas));

			for (int i = 0; i < d; i++)
			{
				Assert::AreEqual(red[i], novered[i], L"poster_red");
			}

		};

		TEST_METHOD(TestPoster)
			{
				int pt = 4;
				int d = 256;
				int pf = 128;
				int r, g, b;

				uint32_t pBitmap[256][256];
				uint32_t noveBitmap[256][256];
				uint32_t kontrolBitmap[256][256];
				memset(pBitmap, 0, sizeof(uint32_t)*d*d);
				memset(noveBitmap, 0, sizeof(uint32_t)*d*d);
				memset(kontrolBitmap, 0, sizeof(uint32_t)*d*d);

				for (int x = 0; x < d; x++) {
					for (int y = 0; y < d; y++) {
						pBitmap[x][y] = x * 256 * 256 + y * 256 + x;
						r = max(0, min(255, (x / pf + 1)*pf));
						g = max(0, min(255, (y / pf + 1)*pf));
						b = max(0, min(255, (x / pf + 1)*pf));
						kontrolBitmap[x][y] = r * 256 * 256 + g * 256 + b;
					}
				}

				int dlzka = d / pt;

				multi_thread_poster(pt, pf, dlzka, pBitmap, noveBitmap, 0, d, d, d, d, [d]() {return false; });

				for (int x = 0; x < d; x++)
				{
					for (int y = 0; y < d; y++)
					{
						Assert::AreEqual(noveBitmap[x][y], kontrolBitmap[x][y], L"posterizacia");
					}
				}


		}

	};
}

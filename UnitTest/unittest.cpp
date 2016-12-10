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
		bool hist_thready(int pt)
		{
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
				if (HistRed1[qwe][255] != dlzka*d) return false;
				if (HistGreen1[qwe][88] != dlzka*d) return false;
				if (HistBlue1[qwe][0] != dlzka*d) return false;
				for (int i = 0; i < 256; i++)
				{
					if (i != 255)if (HistRed1[qwe][i] != 0) return false;
					if (i != 88) if (HistGreen1[qwe][i] != 0) return false;
					if (i != 0) if (HistBlue1[qwe][i] != 0) return false;
				}
			}
			if (HistRed1[pt - 1][255] != d*(d - (pt - 1)*dlzka))return false;
			if (HistGreen1[pt - 1][88] != d*(d - (pt - 1)*dlzka))return false;
			if (HistBlue1[pt - 1][0] != d*(d - (pt - 1)*dlzka))return false;
			for (int i = 0; i < 256; i++)
			{
				if (i != 255) if (HistRed1[pt - 1][i] != 0)return false;
				if (i != 88) if (HistGreen1[pt - 1][i] != 0)return false;
				if (i != 0) if (HistBlue1[pt - 1][i] != 0)return false;
			}
			return true;
		}

		TEST_METHOD(TestHist)
		{
			int pt = 2;
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
			Assert::AreEqual(HistRed1[pt - 1][255], d*(d - (pt - 1)*dlzka), L"cervenyposledny");
			Assert::AreEqual(HistGreen1[pt - 1][88], d*(d - (pt - 1)*dlzka), L"zelenyposledny");
			Assert::AreEqual(HistBlue1[pt - 1][0], d*(d - (pt - 1)*dlzka), L"modryposledny");
			Assert::AreEqual(HistBright1[pt - 1][(int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0)], d*(d - (pt - 1)*dlzka), L"jas");
			for (int i = 0; i < 256; i++)
			{
				if (i != 255) Assert::AreEqual(HistRed1[pt - 1][i], 0, L"cerveny0posledny");
				if (i != 88) Assert::AreEqual(HistGreen1[pt - 1][i], 0, L"zeleny0posledny");
				if (i != 0) Assert::AreEqual(HistBlue1[pt - 1][i], 0, L"modry0posledny");
				if (i != (int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0))Assert::AreEqual(HistBright1[pt - 1][i], 0, L"jas0");
			}
		}

		TEST_METHOD(TestHist_tredy)
		{
			Assert::AreEqual(hist_thready(2), true, L"hist_2thready");
			Assert::AreEqual(hist_thready(4), true, L"hist_4thready");
			Assert::AreEqual(hist_thready(8), true, L"hist_8threadov");
			Assert::AreEqual(hist_thready(12), true, L"hist_8threadov");
			Assert::AreEqual(hist_thready(16), true, L"hist_16threadov");
		}
	};

	TEST_CLASS(TestEfekt)
	{
		TEST_METHOD(TestPoster)
		{
			int pt = 1;
			int d = 256;
			int pf = 64;
			int r, g, b;

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

			uint32_t pBitmap[256][256];
			uint32_t noveBitmap[256][256];
			uint32_t kontrolBitmap[256][256];
			memset(pBitmap, 0, sizeof(uint32_t)*d*d);
			memset(noveBitmap, 2555, sizeof(uint32_t)*d*d);
			memset(kontrolBitmap, 0, sizeof(uint32_t)*d*d);

			for (int x = 0; x < d; x++) {
				for (int y = 0; y < d; y++) {
					pBitmap[x][y] =x * 256 * 256 + y * 256 + x;
					r =  (int)min(round(x / (double)pf)*pf, 255);
					g =  (int)min(round(y / (double)pf)*pf, 255);
					b =  (int)min(round(x / (double)pf)*pf, 255);
					kontrolBitmap[x][y] = r * 256 * 256 + g * 256 + b;
				}
			}

			int dlzka = d / pt;

			multi_thread_poster(pt, pf, dlzka, pBitmap, noveBitmap, 0, d, 4*d, 4*d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });

			for (int x = 0; x < d; x++)
			{
				for (int y = 0; y < d; y++)
				{
					Assert::AreEqual(noveBitmap[x][y], kontrolBitmap[x][y], L"posterizacia");
				}
			}

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					red[i] += HistRed1[j][i];
					green[i] += HistGreen1[j][i];
					blue[i] += HistBlue1[j][i];
					jas[i] += HistBright1[j][i];
				}
			}

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					HistRed1[j][i]=0;
					HistGreen1[j][i]=0;
					HistBlue1[j][i]=0;
					HistBright1[j][i]=0;
				}
			}

			multi_thread(pt, dlzka, kontrolBitmap, 0, d, 4*d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					novered[i] += HistRed1[j][i];
					novegreen[i] += HistGreen1[j][i];
					noveblue[i] += HistBlue1[j][i];
					novejas[i] += HistBright1[j][i];
				}
			}

			for (int i = 0; i < d; i++)
			{
				Assert::AreEqual(red[i], novered[i], L"histogram_poster_red");
				Assert::AreEqual(green[i], novegreen[i], L"histogram_poster_green");
				Assert::AreEqual(blue[i], noveblue[i], L"histogram_poster_blue");
				Assert::AreEqual(jas[i], novejas[i], L"histogram_poster_jas");
			}
		}

		bool poster_thready(int pt, int pf)
		{
			int d = 256;
			int r, g, b;

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

			uint32_t pBitmap[256][256];
			uint32_t noveBitmap[256][256];
			uint32_t kontrolBitmap[256][256];
			memset(pBitmap, 0, sizeof(uint32_t)*d*d);
			memset(noveBitmap, 2555, sizeof(uint32_t)*d*d);
			memset(kontrolBitmap, 0, sizeof(uint32_t)*d*d);

			for (int x = 0; x < d; x++) {
				for (int y = 0; y < d; y++) {
					pBitmap[x][y] = x * 256 * 256 + y * 256 + x;
					r = (int)min(round(x / (double)pf)*pf, 255);
					g = (int)min(round(y / (double)pf)*pf, 255);
					b = (int)min(round(x / (double)pf)*pf, 255);
					kontrolBitmap[x][y] = r * 256 * 256 + g * 256 + b;
				}
			}

			int dlzka = d / pt;

			multi_thread_poster(pt, pf, dlzka, pBitmap, noveBitmap, 0, d, 4 * d, 4 * d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });

			for (int x = 0; x < d; x++)
			{
				for (int y = 0; y < d; y++)
				{
					if(noveBitmap[x][y] != kontrolBitmap[x][y]) return false;
				}
			}

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					red[i] += HistRed1[j][i];
					green[i] += HistGreen1[j][i];
					blue[i] += HistBlue1[j][i];
					jas[i] += HistBright1[j][i];
				}
			}

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					HistRed1[j][i] = 0;
					HistGreen1[j][i] = 0;
					HistBlue1[j][i] = 0;
					HistBright1[j][i] = 0;
				}
			}

			multi_thread(pt, dlzka, kontrolBitmap, 0, d, 4 * d, d, std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), [d]() {return false; });

			for (int j = 0; j < pt; j++)
			{
				for (int i = 0; i <= 255; i++)
				{
					novered[i] += HistRed1[j][i];
					novegreen[i] += HistGreen1[j][i];
					noveblue[i] += HistBlue1[j][i];
					novejas[i] += HistBright1[j][i];
				}
			}

			for (int i = 0; i < d; i++)
			{
				if(red[i] != novered[i])return false;
				if(green[i] != novegreen[i])return false;
				if(blue[i] != noveblue[i])return false;
				if(jas[i] != novejas[i])return false;
			}
			return true;
		}

		TEST_METHOD(TestPoster_tredy)
		{
			Assert::AreEqual(poster_thready(2,256), true, L"poster_2thready");
			Assert::AreEqual(poster_thready(4,256), true, L"poster_4thready");
			Assert::AreEqual(poster_thready(8,256), true, L"poster_8threadov");
			Assert::AreEqual(poster_thready(12,256), true, L"poster_8threadov");
			Assert::AreEqual(poster_thready(16,256), true, L"poster_16threadov");
		}

		TEST_METHOD(TestPoster_farby)
		{
			Assert::AreEqual(poster_thready(4, 128), true, L"poster_27farieb");
			Assert::AreEqual(poster_thready(4, 64), true, L"poster_125farieb");
			Assert::AreEqual(poster_thready(4, 32), true, L"poster_512farieb");
			Assert::AreEqual(poster_thready(4, 16), true, L"poster_4913farieb");
		}
		
	};
}

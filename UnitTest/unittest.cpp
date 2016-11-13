#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Library/Library.h"

#include <vector>

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
			int d = 256;
			std::vector<int> r, g, b, j;
			r.assign(256, 0);
			g.assign(256, 0);
			b.assign(256, 0);
			j.assign(256, 0);
			uint32_t pBitmap[256][256];
			memset(pBitmap, 0, sizeof(uint32_t)*d*d);
			for (int x = 0; x < d; x++) {
				for (int y = 0; y < d; y++) {
					pBitmap[x][y] = 16734208;// 0xff8800;
				}
			}

			CalcHistogram(pBitmap, d, d, d, r, g, b, j);
			Assert::AreEqual(r[255], d*d, L"cerveny");
			Assert::AreEqual(g[88], d*d, L"zeleny");
			Assert::AreEqual(b[0], d*d, L"modry");
			Assert::AreEqual(j[(int)(0.2126 * 255 + 0.7152 * 88 + 0.0722 * 0)], d*d, L"jas");
			for (int i = 0; i < 256; i++) 
			{
			if (i != 255) Assert::AreEqual(r[i], 0, L"cerveny0");
			if (i != 88) Assert::AreEqual(g[i], 0, L"zeleny0");
			if (i != 0) Assert::AreEqual(b[i], 0, L"modry0");
			if (i!=(int)(0.2126*255 + 0.7152*88 + 0.0722*0))Assert::AreEqual(j[i], 0, L"jas0");
			}
		}
	
	};
}

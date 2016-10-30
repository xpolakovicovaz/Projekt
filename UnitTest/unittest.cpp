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
		}
	
	};
}

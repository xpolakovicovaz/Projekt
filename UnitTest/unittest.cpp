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
			std::vector<int> r, g, b, j;
			r.assign(256, 0);
			g.assign(256, 0);
			b.assign(256, 0);
			CalcHistogram(nullprt, 0, 0, 0, r, g, b, j);//vymeni+t pordie argumentov vo funkcii  - odstrani+t bitmap z argumentov
			//dorobi+t jendnu testovaciu metodu, ktora nie+co otestuje, napr. cele +cervene pole
		}
	
	
	}
}

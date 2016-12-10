// ApplicationDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Application.h"
#include "ApplicationDlg.h"
#include "afxdialogex.h"
#include <utility>
#include <tuple>
#include <vector>
#include "Utils.h"
#include <omp.h>
#include <cstdint>
#include <thread>
#include <atomic>
#include <functional>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef MIN_SIZE
#define MIN_SIZE 300
#endif


namespace
{
	void LoadAndCalc(Gdiplus::Bitmap* &pBitmap, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, int pt, std::thread::id m_thread_id, std::function<bool()> fn)
	{
		int r = 0;
		int g = 0;
		int b = 0;
		red.clear();
		red.assign(256, 0);
		green.clear();
		green.assign(256, 0);
		blue.clear();
		blue.assign(256, 0);
		jas.clear();
		jas.assign(256, 0);

		Gdiplus::Color *color;
		color = new Gdiplus::Color(200, 255, 255, 255);
		Gdiplus::BitmapData Bdata;
	//	Gdiplus::Bitmap* pBitmap;
	//	pBitmap = pvBitmap->Clone(0, 0, pvBitmap->GetWidth(), pvBitmap->GetHeight(),  PixelFormat32bppRGB);
		int dlzka = pBitmap->GetHeight() / pt;

		Gdiplus::Rect rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
		pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &Bdata);
		std::vector<std::vector<int>> HistRed1(pt, std::vector<int>(256));
		std::vector<std::vector<int>> HistBright1(pt, std::vector<int>(256));
		std::vector<std::vector<int>> HistGreen1(pt, std::vector<int>(256));
		std::vector<std::vector<int>> HistBlue1(pt, std::vector<int>(256));

		Utils::multi_thread(pt, dlzka, Bdata.Scan0, 0, pBitmap->GetHeight(), Bdata.Stride, pBitmap->GetWidth(), std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), fn);

		for (int j = 0; j<pt; j++)
			for (int i = 0; i <= 255; i++)
			{
				red[i] += HistRed1[j][i];
				green[i] += HistGreen1[j][i];
				blue[i] += HistBlue1[j][i];
				jas[i] += HistBright1[j][i];
			}
		delete(color);
		pBitmap->UnlockBits(&Bdata);
	}

	void poster(Gdiplus::Bitmap* &pBitmap, Gdiplus::Bitmap* &noveBitmap, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas, int pf, int pt, std::thread::id m_thread_id, std::function<bool()> fn)
	{
		Gdiplus::BitmapData pBdata;
		Gdiplus::BitmapData noveBdata;
		int dlzka = noveBitmap->GetHeight() / pt;

		Gdiplus::Rect rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
		Gdiplus::Rect noverect(0, 0, noveBitmap->GetWidth(), noveBitmap->GetHeight());
		std::vector<std::vector<int>> HistRed1(pt, std::vector<int>(256));
		std::vector<std::vector<int>> HistBright1(pt, std::vector<int>(256));		std::vector<std::vector<int>> HistGreen1(pt, std::vector<int>(256));
		std::vector<std::vector<int>> HistBlue1(pt, std::vector<int>(256));

		noveBitmap->LockBits(&noverect, Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &noveBdata);
		pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &pBdata);

		Utils::multi_thread_poster(pt, pf, dlzka, pBdata.Scan0, noveBdata.Scan0, 0, pBitmap->GetHeight(), pBdata.Stride,noveBdata.Stride, pBitmap->GetWidth(), std::ref(HistRed1), std::ref(HistGreen1), std::ref(HistBlue1), std::ref(HistBright1), fn);

		for (int j = 0; j<pt; j++)
			for (int i = 0; i <= 255; i++)
			{
				red[i] += HistRed1[j][i];
				green[i] += HistGreen1[j][i];
				blue[i] += HistBlue1[j][i];
				jas[i] += HistBright1[j][i];
			}

		pBitmap->UnlockBits(&pBdata);
		noveBitmap->UnlockBits(&noveBdata);
	}
}

void CApplicationDlg::single_tred_poster(Gdiplus::Bitmap* &pvBitmap, Gdiplus::Bitmap* &noveBitmap)
{
	std::vector<int> HistRed;
	std::vector<int> HistBright;
	std::vector<int> HistGreen;
	std::vector<int> HistBlue;

	Gdiplus::Bitmap *pBitmap2 = nullptr;
	Gdiplus::Bitmap *noveBitmap2 = nullptr;
	
	HistRed.assign(256, 0);
	HistBright.assign(256, 0);
	HistGreen.assign(256, 0);
	HistBlue.assign(256, 0);

	pBitmap2=pvBitmap->Clone(0, 0, pvBitmap->GetWidth(), pvBitmap->GetHeight(), PixelFormat32bppRGB);
	noveBitmap2 = pvBitmap->Clone(0, 0, pvBitmap->GetWidth(), pvBitmap->GetHeight(), PixelFormat32bppRGB);
	std::thread::id a_tred = std::this_thread::get_id();
	m_thread_id = a_tred;

	poster(pBitmap2, noveBitmap2, HistRed, HistGreen, HistBlue, HistBright, m_pf, m_pt, m_thread_id, [this, a_tred]() {return m_thread_id != a_tred; });
	
	if (std::this_thread::get_id() == m_thread_id) {
		std::tuple <Gdiplus::Bitmap*, std::thread::id, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&> ntuple(noveBitmap2, a_tred, HistRed, HistGreen, HistBlue, HistBright);
		SendMessage(WM_SET_NOVEBITMAP, (WPARAM)&ntuple);
		delete pBitmap2;
	}
	else
	{
		delete noveBitmap2;
		delete pBitmap2;
	}
}

void CApplicationDlg::posterizuj()
{
	if (m_noveBitmap != nullptr)
	{
		delete m_noveBitmap;
		m_noveBitmap = nullptr;
	}
	m_zobrazene = true;
	m_xy = -10;
	m_zc = false;
	m_pocita = true;
	m_noveBitmap = m_pBitmap->Clone(0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), PixelFormat32bppRGB);
	single_tred_poster(m_pBitmap, m_noveBitmap);
}


void CStaticImage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GetParent()->SendMessage( CApplicationDlg::WM_DRAW_IMAGE, (WPARAM)lpDrawItemStruct);
}

void CStaticHistogram::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GetParent()->SendMessage( CApplicationDlg::WM_DRAW_HISTOGRAM, (WPARAM)lpDrawItemStruct);
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	void DoDataExchange(CDataExchange* pDX) override    // DDX/DDV support
	{
		CDialogEx::DoDataExchange(pDX);
	}

// Implementation
protected:
public:

};



namespace
{
	typedef BOOL(WINAPI *LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);


	// Helper function to count set bits in the processor mask.
	DWORD CountSetBits(ULONG_PTR bitMask)
	{
		DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
		DWORD bitSetCount = 0;
		ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
		DWORD i;

		for (i = 0; i <= LSHIFT; ++i)
		{
			bitSetCount += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}

		return bitSetCount;
	}

	DWORD CountMaxThreads()
	{
		LPFN_GLPI glpi;
		BOOL done = FALSE;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
		DWORD returnLength = 0;
		DWORD logicalProcessorCount = 0;
		DWORD numaNodeCount = 0;
		DWORD processorCoreCount = 0;
		DWORD processorL1CacheCount = 0;
		DWORD processorL2CacheCount = 0;
		DWORD processorL3CacheCount = 0;
		DWORD processorPackageCount = 0;
		DWORD byteOffset = 0;
		PCACHE_DESCRIPTOR Cache;

		glpi = (LPFN_GLPI)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (NULL == glpi)
		{
			TRACE(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
			return (1);
		}

		while (!done)
		{
			DWORD rc = glpi(buffer, &returnLength);

			if (FALSE == rc)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						free(buffer);

					buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
						returnLength);

					if (NULL == buffer)
					{
						TRACE(TEXT("\nError: Allocation failure\n"));
						return (2);
					}
				}
				else
				{
					TRACE(TEXT("\nError %d\n"), GetLastError());
					return (3);
				}
			}
			else
			{
				done = TRUE;
			}
		}

		ptr = buffer;

		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationNumaNode:
				// Non-NUMA systems report a single record of this type.
				numaNodeCount++;
				break;

			case RelationProcessorCore:
				processorCoreCount++;

				// A hyperthreaded core supplies more than one logical processor.
				logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
				break;

			case RelationCache:
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
				Cache = &ptr->Cache;
				if (Cache->Level == 1)
				{
					processorL1CacheCount++;
				}
				else if (Cache->Level == 2)
				{
					processorL2CacheCount++;
				}
				else if (Cache->Level == 3)
				{
					processorL3CacheCount++;
				}
				break;

			case RelationProcessorPackage:
				// Logical processors share a physical package.
				processorPackageCount++;
				break;

			default:
				TRACE(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		TRACE(TEXT("\nGetLogicalProcessorInformation results:\n"));
		TRACE(TEXT("Number of NUMA nodes: %d\n"), numaNodeCount);
		TRACE(TEXT("Number of physical processor packages: %d\n"), processorPackageCount);
		TRACE(TEXT("Number of processor cores: %d\n"), processorCoreCount);
		TRACE(TEXT("Number of logical processors: %d\n"), logicalProcessorCount);
		TRACE(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), processorL1CacheCount, processorL2CacheCount, processorL3CacheCount);

		free(buffer);
		TRACE(_T("OPENMP - %i/%i\n"), omp_get_num_procs(), omp_get_max_threads());
		return logicalProcessorCount;
	}
}

// CApplicationDlg dialog


CApplicationDlg::CApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APPLICATION_DIALOG, pParent)
	, m_pBitmap(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nMaxThreads = CountMaxThreads();
}

void CApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_ctrlFileList);
	DDX_Control(pDX, IDC_IMAGE, m_ctrlImage);
	DDX_Control(pDX, IDC_HISTOGRAM, m_ctrlHistogram);
}

BEGIN_MESSAGE_MAP(CApplicationDlg, CDialogEx)
	ON_COMMAND(ID_HISTOGRAM_RED, OnHistogramRed)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_MESSAGE(WM_DRAW_IMAGE, OnDrawImage)
	ON_MESSAGE(WM_DRAW_HISTOGRAM, OnDrawHistogram)
	ON_MESSAGE(WM_SET_BITMAP, OnSetBitmap)
	ON_MESSAGE(WM_SET_NOVEBITMAP, OnSetNoveBitmap)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnLvnItemchangedFileList)
	ON_COMMAND(ID_LOG_OPEN, OnLogOpen)
	ON_UPDATE_COMMAND_UI(ID_LOG_OPEN, OnUpdateLogOpen)
	ON_COMMAND(ID_LOG_CLEAR, OnLogClear)
	ON_UPDATE_COMMAND_UI(ID_LOG_CLEAR, OnUpdateLogClear)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_RED, &CApplicationDlg::OnUpdateHistogramRed)
	ON_COMMAND(ID_HISTOGRAM_RED, &CApplicationDlg::OnHistogramRed)
	ON_COMMAND(ID_HISTOGRAM_BRIGHT, &CApplicationDlg::OnHistogramBright)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_BRIGHT, &CApplicationDlg::OnUpdateHistogramBright)
	ON_COMMAND(ID_HISTOGRAM_BLUE, &CApplicationDlg::OnHistogramBlue)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_BLUE, &CApplicationDlg::OnUpdateHistogramBlue)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_GREEN, &CApplicationDlg::OnUpdateHistogramGreen)
	ON_COMMAND(ID_HISTOGRAM_GREEN, &CApplicationDlg::OnHistogramGreen)
	ON_COMMAND(ID_THREADS_1, &CApplicationDlg::OnThreads1)
	ON_UPDATE_COMMAND_UI(ID_THREADS_1, &CApplicationDlg::OnUpdateThreads1)
	ON_COMMAND(ID_THREADS_2, &CApplicationDlg::OnThreads2)
	ON_UPDATE_COMMAND_UI(ID_THREADS_2, &CApplicationDlg::OnUpdateThreads2)
	ON_COMMAND(ID_THREADS_4, &CApplicationDlg::OnThreads4)
	ON_UPDATE_COMMAND_UI(ID_THREADS_4, &CApplicationDlg::OnUpdateThreads4)
	ON_COMMAND(ID_THREADS_8, &CApplicationDlg::OnThreads8)
	ON_UPDATE_COMMAND_UI(ID_THREADS_8, &CApplicationDlg::OnUpdateThreads8)
	ON_COMMAND(ID_THREADS_12, &CApplicationDlg::OnThreads12)
	ON_UPDATE_COMMAND_UI(ID_THREADS_12, &CApplicationDlg::OnUpdateThreads12)
	ON_COMMAND(ID_THREADS_16, &CApplicationDlg::OnThreads16)
	ON_UPDATE_COMMAND_UI(ID_THREADS_16, &CApplicationDlg::OnUpdateThreads16)
	ON_COMMAND(ID_THREADS_AUTO, &CApplicationDlg::OnThreadsAuto)
	ON_UPDATE_COMMAND_UI(ID_THREADS_AUTO, &CApplicationDlg::OnUpdateThreadsAuto)
	ON_COMMAND(ID_EFEKT_POSTREIZATION, &CApplicationDlg::OnEfektPostreization)
	ON_UPDATE_COMMAND_UI(ID_EFEKT_POSTREIZATION, &CApplicationDlg::OnUpdateEfektPostreization)
	ON_COMMAND(ID_EFEKT_ZOBRAZI32793, &CApplicationDlg::OnEfektZobrazi32793)
	ON_UPDATE_COMMAND_UI(ID_EFEKT_ZOBRAZI32793, &CApplicationDlg::OnUpdateEfektZobrazi32793)
	ON_COMMAND(ID_POSTREIZATION_8FARIEB, &CApplicationDlg::OnPostreization8farieb)
	ON_UPDATE_COMMAND_UI(ID_POSTREIZATION_8FARIEB, &CApplicationDlg::OnUpdatePostreization8farieb)
	ON_COMMAND(ID_POSTREIZATION_16FARIE, &CApplicationDlg::OnPostreization16farie)
	ON_UPDATE_COMMAND_UI(ID_POSTREIZATION_16FARIE, &CApplicationDlg::OnUpdatePostreization16farie)
	ON_COMMAND(ID_POSTREIZATION_512FARIEB, &CApplicationDlg::OnPostreization512farieb)
	ON_UPDATE_COMMAND_UI(ID_POSTREIZATION_512FARIEB, &CApplicationDlg::OnUpdatePostreization512farieb)
	ON_COMMAND(ID_POSTREIZATION_4096FARIEB, &CApplicationDlg::OnPostreization4096farieb)
	ON_UPDATE_COMMAND_UI(ID_POSTREIZATION_4096FARIEB, &CApplicationDlg::OnUpdatePostreization4096farieb)
	ON_COMMAND(ID_EFEKT_X, &CApplicationDlg::OnEfektX)
	ON_UPDATE_COMMAND_UI(ID_EFEKT_X, &CApplicationDlg::OnUpdateEfektX)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_POSTERIZACIA_4913FARIEB, &CApplicationDlg::OnPosterizacia4913farieb)
	ON_UPDATE_COMMAND_UI(ID_POSTERIZACIA_4913FARIEB, &CApplicationDlg::OnUpdatePosterizacia4913farieb)
END_MESSAGE_MAP()

void CApplicationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_zc)
	{
		if (abs(point.y - m_xy) < 5)
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		else
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		if (nFlags == MK_LBUTTON)
		{
			m_xy = point.y;
			m_ctrlImage.Invalidate();
		}
	}
}

void CApplicationDlg::OnDestroy()
{
	m_ctrlLog.DestroyWindow();
	Default();

	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}

	if (m_noveBitmap != nullptr)
	{
		delete m_noveBitmap;
		m_noveBitmap = nullptr;
	}
}


void CApplicationDlg::drawrect(std::vector<int> vektor, CDC &DC, Gdiplus::Color f, double scX, double scY)
{
	double qwe = 0;
	Gdiplus::Graphics g(DC);
	Gdiplus::PointF bod[259];
	bod[0] = Gdiplus::PointF(1, Gdiplus::REAL(m_ptHistogram.y - max(0, log(vektor[0])) * scY));

	for (int i = 0; i < 256; i++)
	{
		bod[i + 1] = Gdiplus::PointF(Gdiplus::REAL(1+(i + 1)*scX), Gdiplus::REAL( m_ptHistogram.y - max(0, log(vektor[i])) * scY));
	}
	bod[257] = Gdiplus::PointF(Gdiplus::REAL(257 * scX), Gdiplus::REAL( m_ptHistogram.y));
	bod[258] = Gdiplus::PointF(1, Gdiplus::REAL( m_ptHistogram.y));
	g.FillPolygon(&Gdiplus::SolidBrush(f), bod, 259);
}


LRESULT CApplicationDlg::OnDrawHistogram(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)wParam;
	CDC * pDC = CDC::FromHandle(lpDI->hDC);
	pDC->FillSolidRect(&(lpDI->rcItem), RGB(255, 255, 255));

	if (m_zc) 
	{
		CBrush brBlack(RGB(0, 0, 0));
		pDC->FrameRect(&(lpDI->rcItem), &brBlack);

		return S_OK;
	}

	int max=1;
	if (m_vHistRed.size() != 0)
	{
		CRect rect(&(lpDI->rcItem));
		if (!m_zobrazene)
		{
			for (int i = 0; i < 256; i++)
			{
				if (m_vHistRed[i] > max)max = m_vHistRed[i];
				if (m_vHistGreen[i] > max)max = m_vHistGreen[i];
				if (m_vHistBlue[i] > max)max = m_vHistBlue[i];
				if (m_vHistBright[i] > max)max = m_vHistBright[i];
			}

			double scalY = double(rect.Height()) / double(log(max));
			double scalX = (rect.Width()-2) / 256.0;

			if (m_bHistRed)drawrect(m_vHistRed, *pDC, Gdiplus::Color(150, 255, 0, 0), scalX, scalY);
			if (m_bHistGreen)drawrect(m_vHistGreen, *pDC, Gdiplus::Color(150, 0, 255, 0), scalX, scalY);
			if (m_bHistBlue)drawrect(m_vHistBlue, *pDC, Gdiplus::Color(150, 0, 0, 255), scalX, scalY);
			if (m_bHistBright)drawrect(m_vHistBright, *pDC, Gdiplus::Color(150, 0, 0, 0), scalX, scalY);
		}
		else
		{
			for (int i = 0; i < 256; i++)
			{
				if (m_vnoveHistRed[i] > max)max = m_vnoveHistRed[i];
				if (m_vnoveHistGreen[i] > max)max = m_vnoveHistGreen[i];
				if (m_vnoveHistBlue[i] > max)max = m_vnoveHistBlue[i];
				if (m_vnoveHistBright[i] > max)max = m_vnoveHistBright[i];
			}

			double scalY = double(rect.Height()) / double(log(max));
			double scalX = (rect.Width() - 2) / 260.0;

			if (m_bHistRed)drawrect(m_vnoveHistRed, *pDC, Gdiplus::Color(150, 255, 0, 0), scalX, scalY);
			if (m_bHistGreen)drawrect(m_vnoveHistGreen, *pDC, Gdiplus::Color(150, 0, 255, 0), scalX, scalY);
			if (m_bHistBlue)drawrect(m_vnoveHistBlue, *pDC, Gdiplus::Color(150, 0, 0, 255), scalX, scalY);
			if (m_bHistBright)drawrect(m_vnoveHistBright, *pDC, Gdiplus::Color(150, 0, 0, 0), scalX, scalY);
		}
	}

	CBrush brBlack(RGB(0, 0, 0));
	pDC->FrameRect(&(lpDI->rcItem), &brBlack);

	return S_OK;
}

LRESULT CApplicationDlg::OnDrawImage(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)wParam;
	CDC * pDC = CDC::FromHandle(lpDI->hDC);

	if (m_pBitmap == nullptr)
	{
		pDC->FillSolidRect(&(lpDI->rcItem), RGB(255, 255, 255));
	}
	else
	{
		// Fit bitmap into client area
		double dWtoH = (double)m_pBitmap->GetWidth() / (double)m_pBitmap->GetHeight();

		CRect rct(lpDI->rcItem);
		rct.DeflateRect(1, 1, 1, 1);

		UINT nHeight = rct.Height();
		UINT nWidth = (UINT)(dWtoH * nHeight);

		if (nWidth > (UINT)rct.Width())
		{
			nWidth = rct.Width();
			nHeight = (UINT)(nWidth / dWtoH);
			_ASSERTE(nHeight <= (UINT)rct.Height());
		}

		if (nHeight < (UINT)rct.Height())
		{
			UINT nBanner = (rct.Height() - nHeight) / 2;
			pDC->FillSolidRect(rct.left, rct.top, rct.Width(), nBanner, RGB(255, 255, 255));
			pDC->FillSolidRect(rct.left, rct.bottom - nBanner - 2, rct.Width(), nBanner + 2, RGB(255, 255, 255));
		}
		
		if (nWidth < (UINT)rct.Width())
		{
			UINT nBanner = (rct.Width() - nWidth) / 2;
			pDC->FillSolidRect(rct.left, rct.top, nBanner, rct.Height(), RGB(255, 255, 255));
			pDC->FillSolidRect(rct.right - nBanner - 2, rct.top, nBanner + 2, rct.Height(), RGB(255, 255, 255));
		}
		//TU TO JE
		Gdiplus::Graphics gr(lpDI->hDC);
		Gdiplus::Rect destRect(rct.left + (rct.Width() - nWidth) / 2, rct.top + (rct.Height() - nHeight) / 2, nWidth, nHeight);

		if (m_zobrazene)m_xy = rct.top + (rct.Height() - nHeight) / 2+nHeight/2;

		if (m_zc)
		{
			int stred = rct.top + (rct.Height() - nHeight) / 2 + nHeight / 2;
			if (m_xy > stred + (int)nHeight / 2) m_xy = stred + nHeight / 2;
			if (m_xy < stred - (int)nHeight / 2) m_xy = stred - nHeight / 2;

			Gdiplus::Pen bp(Gdiplus::Color(255, 0, 0, 0), 3);
			Gdiplus::Pen wp(Gdiplus::Color(255, 255, 255, 255), 1);
			gr.DrawImage(m_noveBitmap->Clone(0,0,m_noveBitmap->GetWidth(), (int)(((m_xy - destRect.Y)*(double)(m_pBitmap->GetHeight() / (double)nHeight))),PixelFormat32bppRGB), destRect.X,destRect.Y,destRect.Width, m_xy-destRect.Y);
			gr.DrawImage(m_pBitmap->Clone(0,int((m_xy-destRect.Y)*(double)(m_pBitmap->GetHeight()/(double)nHeight)),m_pBitmap->GetWidth(),m_pBitmap->GetHeight()-(int)(((m_xy - destRect.Y)*(double)(m_pBitmap->GetHeight() /(double) nHeight))), PixelFormat32bppRGB), (int)(rct.left + (rct.Width() - nWidth) / 2),m_xy, nWidth, stred+(nHeight/2)-m_xy);
			gr.DrawLine(&bp, (int)(rct.left + (rct.Width() - nWidth) / 2), m_xy, (int)(rct.left + (rct.Width() - nWidth) / 2 + nWidth), m_xy);
			gr.DrawLine(&wp, (int)(rct.left + (rct.Width() - nWidth) / 2), m_xy, (int)(rct.left + (rct.Width() - nWidth) / 2 + nWidth), m_xy);
		}
		else
		{
			if (m_zobrazene)
				gr.DrawImage(m_noveBitmap, destRect);
			else
				gr.DrawImage(m_pBitmap, destRect);
		}	
	}

	CBrush brBlack(RGB(0, 0, 0));
	pDC->FrameRect(&(lpDI->rcItem), &brBlack);

	return S_OK;
}

void CApplicationDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	if ((lpRect->right - lpRect->left) < MIN_SIZE)
	{
		switch (nSide)
		{
		case WMSZ_LEFT:
		case WMSZ_BOTTOMLEFT:
		case WMSZ_TOPLEFT:
			lpRect->left = lpRect->right - MIN_SIZE;
		default:
			lpRect->right = lpRect->left + MIN_SIZE;
			break;
		}
	}

	if ((lpRect->bottom - lpRect->top) < MIN_SIZE)
	{
		switch (nSide)
		{
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
		case WMSZ_TOPLEFT:
			lpRect->top = lpRect->bottom - MIN_SIZE;
		default:
			lpRect->bottom = lpRect->top + MIN_SIZE;
			break;
		}
	}

	__super::OnSizing(nSide, lpRect);
}

void CApplicationDlg::OnSize(UINT nType, int cx, int cy)
{
	Default();

	if (!::IsWindow(m_ctrlFileList.m_hWnd) || !::IsWindow(m_ctrlImage.m_hWnd))
		return;

	m_ctrlFileList.SetWindowPos(nullptr, -1, -1, m_ptFileList.x, cy - m_ptFileList.y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	m_ctrlFileList.Invalidate();


	m_ctrlImage.SetWindowPos(nullptr, -1, -1, cx - m_ptImage.x, cy - m_ptImage.y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	m_ctrlImage.Invalidate();

	CRect rct;
	GetClientRect(&rct);

	m_ctrlHistogram.SetWindowPos(nullptr, rct.left + m_ptHistogram.x, rct.bottom - m_ptHistogram.y, -1, -1, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	m_ctrlHistogram.Invalidate();
}

void CApplicationDlg::OnClose()
{
	EndDialog(0);
}

BOOL CApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rct;
	m_ctrlFileList.GetClientRect(&rct);
	m_ctrlFileList.InsertColumn(0, _T("Filename"), 0, rct.Width());

	CRect rctClient;
	GetClientRect(&rctClient);
	m_ctrlFileList.GetWindowRect(&rct);
	m_ptFileList.y = rctClient.Height() - rct.Height();
	m_ptFileList.x = rct.Width();

	m_ctrlImage.GetWindowRect(&rct);
	m_ptImage.x = rctClient.Width() - rct.Width();
	m_ptImage.y = rctClient.Height() - rct.Height();

	m_ctrlHistogram.GetWindowRect(&rct);
	ScreenToClient(&rct);
	m_ptHistogram.x = rct.left - rctClient.left;
	m_ptHistogram.y = rctClient.bottom - rct.top;

	m_ctrlLog.Create(IDD_LOG_DIALOG, this);
	m_ctrlLog.ShowWindow(SW_HIDE);

	m_zobrazene = false;
	m_xy = -10;
	m_zc = false;
	m_pocita = false;
	m_pt = 2;
	m_noveBitmap = nullptr;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to botain the cursor to display while the user drags
//  the minimized window.
HCURSOR CApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CApplicationDlg::OnFileOpen()
{
	CFileDialog dlg(true, nullptr, nullptr
		, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST
		, _T("Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png||")
		, this);
	CString cs;
	const int maxFiles = 100;
	const int buffSize = maxFiles * (MAX_PATH + 1) + 1;

	dlg.GetOFN().lpstrFile = cs.GetBuffer(buffSize);
	dlg.GetOFN().nMaxFile = buffSize;

	if (dlg.DoModal() == IDOK)
	{
		m_ctrlFileList.DeleteAllItems();

		if (m_pBitmap != nullptr)
		{
			delete m_pBitmap;
			m_pBitmap = nullptr;
		}

		m_bHistRed = false;
		m_bHistGreen = false;
		m_bHistBlue = false;
		m_bHistBright = false;

		m_ctrlImage.Invalidate();
		m_ctrlHistogram.Invalidate();

		cs.ReleaseBuffer();

		std::vector<CString> names;

		std::tie( m_csDirectory, names) = Utils::ParseFiles(cs);

		for (int i = 0; i < (int)names.size(); ++i)
		{
			m_ctrlFileList.InsertItem(i, names[i]);
		}
	}
	else
	{
		cs.ReleaseBuffer();
	}
	m_zobrazene = false;
	Invalidate();
}


void CApplicationDlg::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CApplicationDlg::OnFileClose()
{
	m_bHistRed = false;
	m_bHistGreen = false;
	m_bHistBlue = false;
	m_bHistBright = false;
	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}	
	if (m_noveBitmap != nullptr)
	{
		delete m_noveBitmap;
		m_noveBitmap = nullptr;
	}
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
	m_ctrlFileList.DeleteAllItems();
	m_zobrazene = false;
}

void CApplicationDlg::single_tred(CString csFileName)
{
	std::vector<int> HistRed;
	std::vector<int> HistBright;
	std::vector<int> HistGreen;
	std::vector<int> HistBlue;
	HistRed.clear();
	HistGreen.clear();
	HistBlue.clear();
	HistBright.clear();
	Gdiplus::Bitmap *pBitmap = nullptr;
	pBitmap = Gdiplus::Bitmap::FromFile(csFileName);
	std::thread::id a_tred = std::this_thread::get_id();
	LoadAndCalc(pBitmap, HistRed, HistGreen, HistBlue, HistBright, m_pt, m_thread_id, [this, a_tred]() {return m_thread_id != a_tred; });
	if (std::this_thread::get_id() == m_thread_id) {
		std::tuple <Gdiplus::Bitmap*, std::thread::id, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&> ntuple(pBitmap, a_tred, HistRed, HistGreen, HistBlue, HistBright);
		SendMessage(WM_SET_BITMAP, (WPARAM)&ntuple);
	}
	else
	{
		delete pBitmap;
	}
}

void CApplicationDlg::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ctrlFileList.GetItemCount() > 0);
}

LRESULT CApplicationDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	// stack for menu
	std::vector<CMenu*> menus{ GetMenu() };    //starting with main menu
	CCmdUI cmdUI;
	while (!menus.empty())
	{
		// pop menu from stack
		CMenu * pMenu = menus.back();
		menus.pop_back();
		cmdUI.m_nIndexMax = pMenu->GetMenuItemCount();
		// "recursively" crawl submenus / items
		for (int i = 0; i < pMenu->GetMenuItemCount(); ++i)
		{
			cmdUI.m_nID = pMenu->GetMenuItemID(i);
			if (cmdUI.m_nID == (UINT)-1)
			{
				CMenu* pSubMenu = pMenu->GetSubMenu(i);
				if (pSubMenu)
					menus.push_back(pSubMenu);       // submenu - push to stack
			}
			else
			{
				// call OnUpdate... handler for menu item
				cmdUI.m_nIndex = i;
				cmdUI.m_pMenu = pMenu;
				cmdUI.DoUpdate(this, FALSE);
			}
		}
	}
	return TRUE;
}

void CApplicationDlg::OnLvnItemchangedFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pocita = false;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}
	if (m_noveBitmap != nullptr)
	{
		delete m_noveBitmap;
		m_noveBitmap = nullptr;
	}

	m_bHistRed = false;
	m_bHistGreen = false;
	m_bHistBlue = false;
	m_bHistBright = false;

	m_thread_id = std::this_thread::get_id();

	CString csFileName;
	POSITION pos = m_ctrlFileList.GetFirstSelectedItemPosition();
	if (pos)
		csFileName = m_csDirectory + m_ctrlFileList.GetItemText(m_ctrlFileList.GetNextSelectedItem(pos), 0);

	if (!csFileName.IsEmpty())
	{
		m_pocita = true;
		BeginWaitCursor();
		std::thread tred(&CApplicationDlg::single_tred, this, csFileName);
		m_thread_id = tred.get_id();

		tred.detach();
	}

	*pResult = 0;
	m_pf = 0;
	m_zobrazene = false;
	m_xy = -10;
	m_zc = false;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnLogOpen()
{
	m_ctrlLog.ShowWindow(SW_SHOW);
}


void CApplicationDlg::OnUpdateLogOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::IsWindow(m_ctrlLog.m_hWnd) && !m_ctrlLog.IsWindowVisible());
}


void CApplicationDlg::OnLogClear()
{
	m_ctrlLog.SendMessage( CLogDlg::WM_TEXT);
}


void CApplicationDlg::OnUpdateLogClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::IsWindow(m_ctrlLog.m_hWnd) && m_ctrlLog.IsWindowVisible());
}


void CApplicationDlg::OnUpdateHistogramRed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	pCmdUI->SetCheck(m_bHistRed);
	pCmdUI->Enable(!m_zc);
}


void CApplicationDlg::OnHistogramRed()
{
	m_bHistRed = !m_bHistRed;
	Invalidate();
}

void CApplicationDlg::OnHistogramBright()
{
	m_bHistBright = !m_bHistBright;
	Invalidate();
}


void CApplicationDlg::OnUpdateHistogramBright(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	pCmdUI->SetCheck(m_bHistBright);
	pCmdUI->Enable(!m_zc);
}


void CApplicationDlg::OnHistogramBlue()
{
	m_bHistBlue = !m_bHistBlue;
	Invalidate();
}

void CApplicationDlg::OnUpdateHistogramBlue(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bHistBlue);
	pCmdUI->Enable(m_pBitmap != NULL);
	pCmdUI->Enable(!m_zc);
}

void CApplicationDlg::OnUpdateHistogramGreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bHistGreen);
	pCmdUI->Enable(m_pBitmap != NULL);
	pCmdUI->Enable(!m_zc);
}

void CApplicationDlg::OnHistogramGreen()
{
	m_bHistGreen = !m_bHistGreen;
	Invalidate();
}

LRESULT CApplicationDlg::OnSetBitmap(WPARAM wParam, LPARAM lParam)
{
	auto ptuple = (std::tuple<Gdiplus::Bitmap*, std::thread::id, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&> *)(wParam);
	if (std::get<1>(*ptuple) == m_thread_id) {
		m_pBitmap = std::get<0>(*ptuple);
		m_vHistRed = std::move(std::get<2>(*ptuple));
		m_vHistGreen = std::move(std::get<3>(*ptuple));
		m_vHistBlue = std::move(std::get<4>(*ptuple));
		m_vHistBright = std::move(std::get<5>(*ptuple));
		m_ctrlImage.Invalidate();
		m_ctrlHistogram.Invalidate();
	}
	m_pocita = false;
	return TRUE;
}

LRESULT CApplicationDlg::OnSetNoveBitmap(WPARAM wParam, LPARAM lParam)
{
	if (m_noveBitmap != nullptr)
	{
		delete m_noveBitmap;
		m_noveBitmap = nullptr;
	}	
	auto ptuple = (std::tuple<Gdiplus::Bitmap*, std::thread::id, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&> *)(wParam);
	if (std::get<1>(*ptuple) == m_thread_id) {
		m_noveBitmap = std::get<0>(*ptuple);
		m_vnoveHistRed = std::move(std::get<2>(*ptuple));
		m_vnoveHistGreen = std::move(std::get<3>(*ptuple));
		m_vnoveHistBlue = std::move(std::get<4>(*ptuple));
		m_vnoveHistBright = std::move(std::get<5>(*ptuple));
		m_ctrlImage.Invalidate();
		m_ctrlHistogram.Invalidate();
	}
	m_pocita = false;
	return TRUE;
}

void CApplicationDlg::OnThreads1()
{
	m_pt = 1;
}

void CApplicationDlg::OnUpdateThreads1(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 1);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreads4()
{
	m_pt = 4;
}

void CApplicationDlg::OnUpdateThreads4(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 4);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreads8()
{
	m_pt = 8;
}

void CApplicationDlg::OnUpdateThreads8(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 8);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreads12()
{
	m_pt = 12;
}

void CApplicationDlg::OnUpdateThreads12(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 12);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreads16()
{
	m_pt = 16;
}

void CApplicationDlg::OnUpdateThreads16(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 16);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreadsAuto()
{
	m_pt = m_nMaxThreads;
}

void CApplicationDlg::OnUpdateThreadsAuto(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == m_nMaxThreads);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnThreads2()
{
	m_pt = 2;
}

void CApplicationDlg::OnUpdateThreads2(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pt == 2);
	pCmdUI->Enable(!m_pocita);
}

void CApplicationDlg::OnEfektPostreization()
{

}

void CApplicationDlg::OnUpdateEfektPostreization(CCmdUI *pCmdUI)
{
}


void CApplicationDlg::OnEfektZobrazi32793()
{
	m_zobrazene = !m_zobrazene;
	m_zc = false;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdateEfektZobrazi32793(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_zobrazene);
	pCmdUI->Enable(m_noveBitmap != NULL);
}


void CApplicationDlg::OnPostreization8farieb()
{
	m_pf = 256;
	posterizuj();
	m_zobrazene = true;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdatePostreization8farieb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pf == 256);
	pCmdUI->Enable(m_pBitmap != NULL);
}


void CApplicationDlg::OnPostreization16farie()
{
	m_pf = 128;
	posterizuj();
	m_zobrazene = true;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdatePostreization16farie(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pf == 128);
	pCmdUI->Enable(m_pBitmap != NULL);
}


void CApplicationDlg::OnPostreization512farieb()
{
	m_pf = 64;
	posterizuj();
	m_zobrazene = true;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdatePostreization512farieb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pf == 64);
	pCmdUI->Enable(m_pBitmap != NULL);
}


void CApplicationDlg::OnPostreization4096farieb()
{
	m_pf = 32;
	posterizuj();
	m_zobrazene = true;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdatePostreization4096farieb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pf == 32);
	pCmdUI->Enable(m_pBitmap != NULL);
}


void CApplicationDlg::OnEfektX()
{
	m_zobrazene = false;
	m_zc = !m_zc;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
}


void CApplicationDlg::OnUpdateEfektX(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_zc);
	pCmdUI->Enable(m_noveBitmap!=NULL);
}

void CApplicationDlg::OnPosterizacia4913farieb()
{
	m_pf = 16;
	posterizuj();
	m_zobrazene = true;
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();

}


void CApplicationDlg::OnUpdatePosterizacia4913farieb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pf == 16);
	pCmdUI->Enable(m_pBitmap != NULL);
}

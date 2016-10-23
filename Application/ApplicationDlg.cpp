
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
//#include <algorithm>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef MIN_SIZE
#define MIN_SIZE 300
#endif


namespace
{
	void LoadAndCalc(CString fileName, Gdiplus::Bitmap *&pBitmap, std::vector<int> &red, std::vector<int> &green, std::vector<int> &blue, std::vector<int> &jas)
	{
		red.clear();
		red.assign(256, 0);
		green.clear();
		green.assign(256, 0);
		blue.clear();
		blue.assign(256, 0);
		jas.clear();
		jas.assign(256, 0);
		Gdiplus::Color *color;
		color = new Gdiplus::Color(200,255,255,255);

		for (int x = 0; x < pBitmap->GetWidth(); x++)
		{
			for (int y = 0; y < pBitmap->GetHeight(); y++)
			{
				pBitmap->GetPixel(x, y, color);
				red[color->GetRed()]++;
				green[color->GetGreen()]++;
				blue[color->GetBlue()]++;
				jas[(0.2126*color->GetRed() + 0.7152*color->GetGreen() + 0.0722*color->GetBlue())]++;
				
			}
		}
		delete(color);
	}
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
END_MESSAGE_MAP()


void CApplicationDlg::OnDestroy()
{
	m_ctrlLog.DestroyWindow();
	Default();

	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}
}

/*void CApplicationDlg::drawrect(std::vector<int> vektor,CDC &DC,COLORREF f,double scX,double scY)
{
	Gdiplus::Graphics g(DC);

	for (BYTE i = 0; i < 255; i++)
	{
		CRect stlpik(floor(i*scX), floor(m_ptHistogram.y - max(0,log(vektor[i])) * scY),floor( (i + 1)*scX + 1), m_ptHistogram.y);
		DC.FillSolidRect(stlpik, f);

	}

}*/

void CApplicationDlg::drawrect(std::vector<int> vektor,CDC &DC,Gdiplus::Color f,double scX,double scY)
{
	double qwe = 0;
	Gdiplus::Graphics g(DC);
	for (BYTE i = 0; i < 255; i++)
	{
		g.FillRectangle(&Gdiplus::SolidBrush(f), Gdiplus::Rect(max(qwe,floor(i*scX)), floor(m_ptHistogram.y - max(0, log(vektor[i])) * scY), floor(scX + 1), m_ptHistogram.y));
		qwe += floor(scX + 1);
	}

}

LRESULT CApplicationDlg::OnDrawHistogram(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)wParam;

	CDC * pDC = CDC::FromHandle(lpDI->hDC);

	pDC->FillSolidRect(&(lpDI->rcItem), RGB(255, 255, 255));

	int max=1;

	if (m_vHistRed.size() != 0) {
		CRect rect(&(lpDI->rcItem));
		for (int i=0;i<256;i++)
		{
			if (m_vHistRed[i]> max)max = m_vHistRed[i];
			if (m_vHistGreen[i]> max)max = m_vHistGreen[i];
			if (m_vHistBlue[i]> max)max = m_vHistBlue[i];
			if (m_vHistBright[i]> max)max = m_vHistBright[i];
		}
		
		double scalY = double(rect.Height()) / double(log(max));
		double scalX = rect.Width() / 256.0;
		if (m_bHistRed)drawrect(m_vHistRed, *pDC, Gdiplus::Color(150,255,0,0), scalX, scalY);
		if (m_bHistGreen)drawrect(m_vHistGreen, *pDC, Gdiplus::Color(150,0, 255, 0), scalX, scalY);
		if (m_bHistBlue)drawrect(m_vHistBlue, *pDC, Gdiplus::Color(150,0, 0, 255), scalX, scalY);
		if (m_bHistBright)drawrect(m_vHistBright, *pDC, Gdiplus::Color(150,0, 0, 0), scalX, scalY);
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

		Gdiplus::Graphics gr(lpDI->hDC);
		Gdiplus::Rect destRect(rct.left + (rct.Width() - nWidth) / 2, rct.top + (rct.Height() - nHeight) / 2, nWidth, nHeight);
		gr.DrawImage(m_pBitmap, destRect);
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

// The system calls this function to obtain the cursor to display while the user drags
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
	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();
	m_ctrlFileList.DeleteAllItems();
}


void CApplicationDlg::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ctrlFileList.GetItemCount() > 0);
}

LRESULT CApplicationDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	CMenu* pMainMenu = GetMenu();
	CCmdUI cmdUI;
	for (UINT n = 0; n < (UINT)pMainMenu->GetMenuItemCount(); ++n)
	{
		CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
		cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
		for (UINT i = 0; i < cmdUI.m_nIndexMax; ++i)
		{
			cmdUI.m_nIndex = i;
			cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
			cmdUI.m_pMenu = pSubMenu;
			cmdUI.DoUpdate(this, FALSE);
		}
	}
	return TRUE;
}

void CApplicationDlg::OnLvnItemchangedFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}

	m_bHistRed = false;
	m_bHistGreen = false;
	m_bHistBlue = false;
	m_bHistBright = false;

	CString csFileName;
	POSITION pos = m_ctrlFileList.GetFirstSelectedItemPosition();
	if (pos)
		csFileName = m_csDirectory + m_ctrlFileList.GetItemText(m_ctrlFileList.GetNextSelectedItem(pos), 0);

	if (!csFileName.IsEmpty())
	{

		m_pBitmap = Gdiplus::Bitmap::FromFile(csFileName);
		LoadAndCalc(csFileName, m_pBitmap, m_vHistRed, m_vHistGreen, m_vHistBlue, m_vHistBright);
	}

	m_ctrlImage.Invalidate();

	m_ctrlHistogram.Invalidate();
	
	*pResult = 0;
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
}

void CApplicationDlg::OnUpdateHistogramGreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bHistGreen);
	pCmdUI->Enable(m_pBitmap != NULL);

}

void CApplicationDlg::OnHistogramGreen()
{
	m_bHistGreen = !m_bHistGreen;
	Invalidate();
}
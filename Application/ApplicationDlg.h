
// ApplicationDlg.h : header file
//

#pragma once

#include "LogDlg.h"
#include <GdiPlus.h>
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include <fstream>

class CStaticImage : public CStatic
{
public:
	// Overridables (for owner draw only)
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
};

class CStaticHistogram : public CStatic
{
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
};

// CApplicationDlg dialog
class CApplicationDlg : public CDialogEx
{
// Construction
public:
	enum
	{
		WM_DRAW_IMAGE = (WM_USER + 1),
		WM_DRAW_HISTOGRAM,
		WM_SET_BITMAP, 
		WM_SET_NOVEBITMAP
	};

	CApplicationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPLICATION_DIALOG };
#endif

	protected:
	void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support

	void OnOK() override {}
	void OnCancel() override {}


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	bool m_bHistRed;
	bool m_bHistGreen;
	bool m_bHistBlue;
	bool m_bHistBright;
	bool m_zobrazene;
	int m_xy;// y-ova suradnica deliacej ciary
	bool m_zc;// zobrazena ciara
	bool m_pocita;
	std::vector<int> m_vHistRed;
	std::vector<int> m_vHistBright;
	std::vector<int> m_vHistGreen;
	std::vector<int> m_vHistBlue;
	std::vector<int> m_vnoveHistRed;
	std::vector<int> m_vnoveHistBright;
	std::vector<int> m_vnoveHistGreen;
	std::vector<int> m_vnoveHistBlue;
	std::atomic<std::thread::id> m_thread_id;
	int m_pt;//pocet threadov
	int m_pf;//pocet farieb
//	std::ofstream myfile;
//	clock_t begin;
//	clock_t end;

public:
	afx_msg void drawrect(std::vector<int> vektor, CDC &DC, Gdiplus::Color f, double scX, double scY);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnDrawImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDrawHistogram(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetBitmap(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetNoveBitmap(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void single_tred(CString csFileName);
	afx_msg void single_tred_poster(Gdiplus::Bitmap* &pvBitmap, Gdiplus::Bitmap* &noveBitmap);
	afx_msg void posterizuj();

protected:
	CListCtrl m_ctrlFileList;
	CStaticImage m_ctrlImage;
	CStaticHistogram m_ctrlHistogram;

	CPoint m_ptFileList;
	CPoint m_ptHistogram;
	CPoint m_ptImage;

	CString m_csDirectory;

	CLogDlg m_ctrlLog;

	Gdiplus::Bitmap * m_pBitmap;
	Gdiplus::Bitmap * m_noveBitmap;
	DWORD m_nMaxThreads;

public:
	afx_msg void OnLvnItemchangedFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLogOpen();
	afx_msg void OnUpdateLogOpen(CCmdUI *pCmdUI);
	afx_msg void OnLogClear();
	afx_msg void OnUpdateLogClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHistogramRed(CCmdUI *pCmdUI);
	afx_msg void OnHistogramRed();
	afx_msg void OnHistogramBright();
	afx_msg void OnUpdateHistogramBright(CCmdUI *pCmdUI);
	afx_msg void OnHistogramBlue();
	afx_msg void OnUpdateHistogramBlue(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHistogramGreen(CCmdUI *pCmdUI);
	afx_msg void OnHistogramGreen();
	afx_msg void OnThreads1();
	afx_msg void OnUpdateThreads1(CCmdUI *pCmdUI);
	afx_msg void OnThreads2();
	afx_msg void OnUpdateThreads2(CCmdUI *pCmdUI);
	afx_msg void OnThreads4();
	afx_msg void OnUpdateThreads4(CCmdUI *pCmdUI);
	afx_msg void OnThreads8();
	afx_msg void OnUpdateThreads8(CCmdUI *pCmdUI);
	afx_msg void OnThreads12();
	afx_msg void OnUpdateThreads12(CCmdUI *pCmdUI);
	afx_msg void OnThreads16();
	afx_msg void OnUpdateThreads16(CCmdUI *pCmdUI);
	afx_msg void OnThreadsAuto();
	afx_msg void OnUpdateThreadsAuto(CCmdUI *pCmdUI);
	afx_msg void OnEfektPostreization();
	afx_msg void OnUpdateEfektPostreization(CCmdUI *pCmdUI);
	afx_msg void OnEfektZobrazi32793();
	afx_msg void OnUpdateEfektZobrazi32793(CCmdUI *pCmdUI);
	afx_msg void OnPostreization8farieb();
	afx_msg void OnUpdatePostreization8farieb(CCmdUI *pCmdUI);
	afx_msg void OnPostreization16farie();
	afx_msg void OnUpdatePostreization16farie(CCmdUI *pCmdUI);
	afx_msg void OnPostreization512farieb();
	afx_msg void OnUpdatePostreization512farieb(CCmdUI *pCmdUI);
	afx_msg void OnPostreization4096farieb();
	afx_msg void OnUpdatePostreization4096farieb(CCmdUI *pCmdUI);
	afx_msg void OnEfektX();
	afx_msg void OnUpdateEfektX(CCmdUI *pCmdUI);
	afx_msg void OnPosterizacia4913farieb();
	afx_msg void OnUpdatePosterizacia4913farieb(CCmdUI *pCmdUI);
};

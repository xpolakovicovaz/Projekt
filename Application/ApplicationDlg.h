
// ApplicationDlg.h : header file
//

#pragma once

#include "LogDlg.h"
#include <GdiPlus.h>
#include <vector>

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
		WM_DRAW_HISTOGRAM
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
	std::vector<int> m_vHistRed;
	std::vector<int> m_vHistBright;
	std::vector<int> m_vHistGreen;
	std::vector<int> m_vHistBlue;

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
	afx_msg void OnDestroy();

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
	afx_msg void OnSetBitmap(WPARAM wParam,LPARAM lParam);
};

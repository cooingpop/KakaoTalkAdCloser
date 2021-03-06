
// KakaoTalkAdCloserDlg.h : header file
//

#pragma once

#include <memory>

namespace kudd {
    class AdHider;
}

// CKakaoTalkAdCloserDlg dialog
class CKakaoTalkAdCloserDlg : public CDialogEx
{
// Construction
public:
    CKakaoTalkAdCloserDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_KAKAOTALKADCLOSER_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
    bool _forceClose;
    std::shared_ptr<kudd::AdHider> _adHider;

private:
    void initUi();
    void registerTrayIcon();
    void deRegisterTrayIcon();
    void showContextMenu();

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
    afx_msg LRESULT OnTrayIconMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnShowDlg();
    afx_msg void OnExitHider();
    afx_msg void OnClose();
    afx_msg void OnRunOnStartup();
    afx_msg void OnRunMinimized();
    DECLARE_MESSAGE_MAP()
};

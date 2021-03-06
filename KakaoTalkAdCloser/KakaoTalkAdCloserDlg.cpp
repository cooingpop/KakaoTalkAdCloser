
// KakaoTalkAdCloserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KakaoTalkAdCloser.h"
#include "KakaoTalkAdCloserDlg.h"
#include "afxdialogex.h"
#include "AdHider.h"
#include <cstdint>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
    const uint32_t UM_TRAYICON_MSG = WM_USER + 100;
    const CString CATEGORY_SETTINGS(L"Settings");
    const CString ENTRY_RUNMINIMIZED(L"RunMinimized");
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKakaoTalkAdCloserDlg dialog



CKakaoTalkAdCloserDlg::CKakaoTalkAdCloserDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_KAKAOTALKADCLOSER_DIALOG, pParent)
    , _forceClose(false)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKakaoTalkAdCloserDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKakaoTalkAdCloserDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_MESSAGE(UM_TRAYICON_MSG, OnTrayIconMsg)
    ON_COMMAND(ID_CONTEXTMENU_SHOW, OnShowDlg)
    ON_COMMAND(ID_CONTEXTMENU_EXIT, OnExitHider)
    ON_BN_CLICKED(IDC_CHECK_RUN_ON_WINDOWS_STARTUP, OnRunOnStartup)
    ON_BN_CLICKED(IDC_CHECK_RUN_MINIMIZED, OnRunMinimized)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CKakaoTalkAdCloserDlg message handlers

BOOL CKakaoTalkAdCloserDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
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
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    initUi();

    _adHider.reset(new kudd::AdHider());
    _adHider->startup();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKakaoTalkAdCloserDlg::initUi()
{
    registerTrayIcon();

    SetWindowText(L"카카오톡 광고 숨기기 v0.2");

    CButton* check = static_cast<CButton*>(GetDlgItem(IDC_CHECK_RUN_ON_WINDOWS_STARTUP));
    check->SetWindowText(L"윈도우 시작시 실행");
    check->EnableWindow(FALSE);

    check = static_cast<CButton*>(GetDlgItem(IDC_CHECK_RUN_MINIMIZED));
    check->SetWindowTextW(L"최소화 상태로 실행");

    bool runMinimized = AfxGetApp()->GetProfileInt(CATEGORY_SETTINGS, ENTRY_RUNMINIMIZED, 0) != 0;
    check->SetCheck(runMinimized ? BST_CHECKED : BST_UNCHECKED);

    if (runMinimized) {
        PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
}

void CKakaoTalkAdCloserDlg::registerTrayIcon()
{
    NOTIFYICONDATA  nid;
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_hWnd;
    nid.uID = IDR_MAINFRAME;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = UM_TRAYICON_MSG;
    nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    CString title;
    GetWindowText(title);

    wcsncpy_s(nid.szTip, (LPCTSTR)title, 128);
    Shell_NotifyIcon(NIM_ADD, &nid);
    SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
}

void CKakaoTalkAdCloserDlg::deRegisterTrayIcon()
{
    NOTIFYICONDATA  nid;
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_hWnd;
    nid.uID = IDR_MAINFRAME;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void CKakaoTalkAdCloserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else if (nID == SC_MINIMIZE) {
        //_notifyDlg->ShowWindow(SW_HIDE);
        ShowWindow(SW_HIDE);
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKakaoTalkAdCloserDlg::OnPaint()
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
HCURSOR CKakaoTalkAdCloserDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CKakaoTalkAdCloserDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    if (_forceClose || AfxMessageBox(L"정말 종료하시겠어요?", MB_YESNO) == IDYES) {
        CDialogEx::OnClose();
    }
}


void CKakaoTalkAdCloserDlg::OnDestroy()
{
    if (_adHider.get()) {
        _adHider->cleanup();
    }

    deRegisterTrayIcon();

    CDialogEx::OnDestroy();

    // TODO: Add your message handler code here
}

BOOL CKakaoTalkAdCloserDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) {
        switch (pMsg->wParam) {
        case VK_ESCAPE:
        case VK_RETURN:
            return TRUE;
        default:
            break;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CKakaoTalkAdCloserDlg::OnTrayIconMsg(WPARAM wParam, LPARAM lParam)
{
    switch (lParam) {
    case WM_LBUTTONDBLCLK:
        ShowWindow(SW_SHOW);
        break;
    case WM_RBUTTONDOWN:
        showContextMenu();
        break;
    default:
        break;
    }
    return 0L;
}

void CKakaoTalkAdCloserDlg::showContextMenu()
{
    CMenu menu;
    menu.LoadMenu(IDR_MENU_CONTEXT);

    CPoint pt;
    ::GetCursorPos(&pt);

    CMenu* contextMenu = menu.GetSubMenu(0);
    contextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
}

void CKakaoTalkAdCloserDlg::OnShowDlg()
{
    ShowWindow(SW_SHOW);
}

void CKakaoTalkAdCloserDlg::OnExitHider()
{
    if (AfxMessageBox(L"정말 종료하시겠어요?", MB_YESNO) == IDYES) {
        _forceClose = true;
        PostQuitMessage(0);
    }
}

void CKakaoTalkAdCloserDlg::OnRunOnStartup()
{
    auto* btn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_RUN_ON_WINDOWS_STARTUP));

    if (btn->GetCheck() == BST_CHECKED) {

    }
    else {

    }
}

void CKakaoTalkAdCloserDlg::OnRunMinimized()
{
    auto* btn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_RUN_MINIMIZED));

    if (btn->GetCheck() == BST_CHECKED) {
        AfxGetApp()->WriteProfileInt(CATEGORY_SETTINGS, ENTRY_RUNMINIMIZED, 1);
    }
    else {
        AfxGetApp()->WriteProfileInt(CATEGORY_SETTINGS, ENTRY_RUNMINIMIZED, 0);
    }
}
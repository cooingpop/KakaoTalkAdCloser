#include "stdafx.h"
#include "AdHider.h"

using namespace kudd;

namespace {
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
    {
        wchar_t className[256] = { 0, };
        wchar_t prevClassName[256] = { 0, };
        GetClassName(hwnd, className, 256);

        if (std::wstring(className) == L"EVA_Window") {
            HWND prev = GetWindow(hwnd, GW_HWNDPREV);

            if (prev != nullptr) {
                GetWindowText(prev, prevClassName, 256);
                std::wstring name(prevClassName);

                if (name.find(L"LockModeView_") != std::wstring::npos) {
                    RECT rect;
                    memset(&rect, 0, sizeof(rect));
                    GetWindowRect(hwnd, &rect);
                    //MoveWindow(hwnd, rect.left, rect.bottom - 5, (rect.right - rect.left), 5, FALSE);

                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    return FALSE;
                }
            }
        }

        return TRUE;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

AdHideStatus::AdHideStatus()
    : _kakaoTalkFound(false)
{
}

void AdHideStatus::reset()
{
    _kakaoTalkFound = false;
}

AdHideStatus& AdHideStatus::get()
{
    static AdHideStatus __instance__;
    return __instance__;
}

///////////////////////////////////////////////////////////////////////////////////////////////

AdHider::~AdHider()
{
    cleanup();
}

void AdHider::startup()
{
    if (_hiderThread.get() == nullptr) {
        _hiderThread.reset(new std::thread([&] {
            _running = true;
            threadProc();
        }));
    }
}

void AdHider::cleanup()
{
    if (_hiderThread.get() && _running) {
        _running = false;
        _hiderThread->join();
    }
}

void AdHider::checkKakaoTalkAd()
{
    auto& ads = AdHideStatus::get();

    HWND handle = FindWindow(nullptr, L"카카오톡");
    if (handle == nullptr) {
        ads.reset();
        return;
    }

    ads.setKakaoTalkFound();


    EnumChildWindows(handle, EnumWindowsProc, 0);
}

void AdHider::threadProc()
{
    while (_running) {
        checkKakaoTalkAd();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}
// RadWindowApp.cpp --- A Win32 application                -*- C++ -*-
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////

/*static*/ MRadWindow *MRadWindow::s_p_rad_window = NULL;

//////////////////////////////////////////////////////////////////////////////

// the Win32 application
struct WinApp : public MWindowBase
{
    INT         m_argc;         // number of command line parameters
    TCHAR **    m_targv;        // command line parameters

    HINSTANCE   m_hInst;        // the instance handle
    HICON       m_hIcon;        // the icon handle
    HACCEL      m_hAccel;       // the accelerator handle

    MRadWindow  m_rad_window;

    WinApp(int argc, TCHAR **targv, HINSTANCE hInst) :
        m_argc(argc),
        m_targv(targv),
        m_hInst(hInst),
        m_hIcon(NULL),
        m_hAccel(NULL)
    {
    }

    virtual void ModifyWndClassDx(WNDCLASSEX& wcx)
    {
        wcx.lpszMenuName = MAKEINTRESOURCE(1);
    }

    // start up
    BOOL StartDx(INT nCmdShow)
    {
        // load accessories
        m_hIcon = ::LoadIcon(m_hInst, MAKEINTRESOURCE(1));
        m_hAccel = ::LoadAccelerators(m_hInst, MAKEINTRESOURCE(1));

        if (!CreateWindowDx(NULL, LoadStringDx(1), WS_OVERLAPPEDWINDOW))
        {
            MsgBoxDx(TEXT("failure of CreateWindow"), NULL, MB_ICONERROR);
            return FALSE;
        }

        // show the window
        ::ShowWindow(*this, nCmdShow);
        ::UpdateWindow(*this);

        return TRUE;
    }

    // message loop
    INT_PTR RunDx()
    {
        MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0))
        {
            if (::IsDialogMessage(m_rad_window.m_rad_dialog, &msg))
                continue;
            if (::TranslateAccelerator(m_hwnd, m_hAccel, &msg))
                continue;

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        return INT(msg.wParam);
    }

    // WM_CREATE
    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
    {
        return TRUE;
    }

    // WM_DESTROY
    void OnDestroy(HWND hwnd)
    {
        ::PostQuitMessage(0);
    }

    // WM_PAINT
    void OnPaint(HWND hwnd)
    {
        RECT rc;
        ::GetClientRect(m_hwnd, &rc);

        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(m_hwnd, &ps);
        if (hdc != NULL)
        {
            // draw something...
            SelectObject(hdc, ::GetStockObject(BLACK_PEN));
            ::MoveToEx(hdc, rc.left, rc.top, NULL);
            ::LineTo(hdc, rc.right, rc.bottom);
            ::MoveToEx(hdc, rc.right, rc.top, NULL);
            ::LineTo(hdc, rc.left, rc.bottom);

            static const char s_text[] = PROGRAM_NAME;
            ::SetBkMode(hdc, TRANSPARENT);
            ::TextOutA(hdc, 0, 0, s_text, ::lstrlenA(s_text));

            ::EndPaint(m_hwnd, &ps);
        }
    }

    void OnSize(HWND hwnd, UINT state, int cx, int cy)
    {
        ::InvalidateRect(m_hwnd, NULL, TRUE);
    }

    // IDM_EXIT
    void OnExit()
    {
        ::DestroyWindow(m_hwnd);
    }

    // IDM_ABOUT
    void OnAbout();

    LPVOID LoadDialogRes(DWORD& Size)
    {
        HMODULE hMod = GetModuleHandle(NULL);
        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDD_TEST), RT_DIALOG);
        Size = SizeofResource(hMod, hRsrc);
        HGLOBAL hGlobal = LoadResource(hMod, hRsrc);
        return LockResource(hGlobal);
    }

    // IDM_TEST
    void OnTest()
    {
        if (m_rad_window)
        {
            SetForegroundWindow(m_rad_window);
            return;
        }

        DWORD Size;
        LPVOID pDialog = LoadDialogRes(Size);

        ByteStream stream(pDialog, Size);
        m_rad_window.m_dialog_res.LoadFromStream(stream);

        if (m_rad_window.CreateWindowDx(m_hwnd, LoadStringDx(2)))
        {
            ShowWindow(m_rad_window, SW_SHOWNOACTIVATE);
            UpdateWindow(m_rad_window);
        }
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDM_EXIT:
            OnExit();
            break;
        case IDM_ABOUT:
            OnAbout();
            break;
        case IDM_TEST:
            OnTest();
            break;
        }
    }

    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
            HANDLE_MSG(hwnd, WM_SIZE, OnSize);
            HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
            HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        default:
            return DefaultProcDx(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }
}; // WinApp

//////////////////////////////////////////////////////////////////////////////

struct AboutDialog : public MDialogBase
{
    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        }
        return DefaultProcDx(hwnd, uMsg, wParam, lParam);
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        CenterWindowDx();
        return TRUE;
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
            ::EndDialog(hwnd, IDOK);
            break;
        case IDCANCEL:
            ::EndDialog(hwnd, IDCANCEL);
            break;
        default:
            break;
        }
    }
};

void WinApp::OnAbout()
{
    AboutDialog about;
    about.DialogBoxDx(m_hwnd, IDD_ABOUTBOX);
}

//////////////////////////////////////////////////////////////////////////////
// Win32 App main function

extern "C"
INT APIENTRY _tWinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPTSTR      lpCmdLine,
    INT         nCmdShow)
{
    int ret;

    {
        WinApp app(__argc, __targv, hInstance);

        if (app.StartDx(nCmdShow))
        {
            ret = INT(app.RunDx());
        }
        else
        {
            ret = 2;
        }
    }

#if (WINVER >= 0x0500)
    HANDLE hProcess = GetCurrentProcess();
    DebugPrintDx(TEXT("Count of GDI objects: %ld\n"),
                 GetGuiResources(hProcess, GR_GDIOBJECTS));
    DebugPrintDx(TEXT("Count of USER objects: %ld\n"),
                 GetGuiResources(hProcess, GR_USEROBJECTS));
#endif

#if defined(_MSC_VER) && !defined(NDEBUG)
    // for detecting memory leak (MSVC only)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return ret;
}

//////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <random>
#include <sstream>
#include <string>

static HWND windowHandle{};
static std::mt19937 rng{ std::random_device{}() };

static int targetX = 500;
static int targetY = 300;
static int radius = 28;

static int hits = 0;
static int shots = 0;
static int streak = 0;
static int best = 0;

int Random(int minValue, int maxValue) {
    return std::uniform_int_distribution<int>(
        minValue, maxValue
    )(rng);
}

void SpawnTarget() {
    RECT rect{};
    GetClientRect(windowHandle, &rect);

    targetX = Random(70, (std::max)(70, rect.right - 70));
    targetY = Random(125, (std::max)(140, rect.bottom - 70));
}

void DrawTextLine(
    HDC dc, int x, int y,
    const std::wstring& text,
    COLORREF color, int size = 20
) {
    SetTextColor(dc, color);

    HFONT font = CreateFontW(
        size, 0, 0, 0, FW_SEMIBOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH,
        L"Segoe UI"
    );

    HGDIOBJ old = SelectObject(dc, font);

    TextOutW(
        dc, x, y,
        text.c_str(),
        static_cast<int>(text.size())
    );

    SelectObject(dc, old);
    DeleteObject(font);
}

void Paint(HDC dc) {
    RECT rect{};
    GetClientRect(windowHandle, &rect);

    HBRUSH bg = CreateSolidBrush(RGB(18, 20, 29));
    FillRect(dc, &rect, bg);
    DeleteObject(bg);

    DrawTextLine(
        dc, 24, 18,
        L"AIM TRAINER - ZYON",
        RGB(190, 150, 255), 25
    );

    std::wstringstream info;

    info << L"Acertos: " << hits
         << L"   Disparos: " << shots
         << L"   Precisao: "
         << (shots ? hits * 100 / shots : 0)
         << L"%   Sequencia: " << streak
         << L"   Recorde: " << best;

    DrawTextLine(
        dc, 26, 62, info.str(),
        RGB(225, 228, 240), 18
    );

    DrawTextLine(
        dc, 26, 94,
        L"CLIQUE NO ALVO | R: reiniciar | +/-: tamanho | ESC: sair",
        RGB(150, 160, 180), 15
    );

    HBRUSH outer = CreateSolidBrush(RGB(155, 75, 245));
    HBRUSH inner = CreateSolidBrush(RGB(245, 245, 250));
    HBRUSH core = CreateSolidBrush(RGB(245, 75, 105));

    HGDIOBJ old = SelectObject(dc, outer);

    Ellipse(
        dc,
        targetX - radius,
        targetY - radius,
        targetX + radius,
        targetY + radius
    );

    SelectObject(dc, inner);

    Ellipse(
        dc,
        targetX - radius / 2,
        targetY - radius / 2,
        targetX + radius / 2,
        targetY + radius / 2
    );

    SelectObject(dc, core);

    Ellipse(
        dc,
        targetX - radius / 6,
        targetY - radius / 6,
        targetX + radius / 6,
        targetY + radius / 6
    );

    SelectObject(dc, old);

    DeleteObject(outer);
    DeleteObject(inner);
    DeleteObject(core);

    DrawTextLine(
        dc, 26, rect.bottom - 36,
        L"Treinamento independente - Zyon",
        RGB(135, 145, 165), 14
    );
}

void Reset() {
    hits = shots = streak = best = 0;
    SpawnTarget();
    InvalidateRect(windowHandle, nullptr, TRUE);
}

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp
) {
    switch (msg) {
    case WM_CREATE:
        windowHandle = hwnd;
        SpawnTarget();
        return 0;

    case WM_LBUTTONDOWN: {
        int mx = GET_X_LPARAM(lp);
        int my = GET_Y_LPARAM(lp);

        ++shots;

        int dx = mx - targetX;
        int dy = my - targetY;

        if (dx * dx + dy * dy <= radius * radius) {
            ++hits;
            ++streak;
            best = (std::max)(best, streak);
            SpawnTarget();
        } else {
            streak = 0;
        }

        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }

    case WM_KEYDOWN:
        if (wp == 'R') Reset();

        if (wp == VK_ADD || wp == VK_OEM_PLUS)
            radius = (std::min)(70, radius + 3);

        if (wp == VK_SUBTRACT || wp == VK_OEM_MINUS)
            radius = (std::max)(10, radius - 3);

        if (wp == VK_ESCAPE)
            DestroyWindow(hwnd);

        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        Paint(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE,
    PWSTR,
    int show
) {
    const wchar_t CLASS_NAME[] = L"AimTrainerWindow";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);

    if (!RegisterClassW(&wc))
        return 1;

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"AimTrainer - Windows x64",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        700,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!hwnd) return 2;

    ShowWindow(hwnd, show);

    MSG msg{};

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "boinkorwindow";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

    // Register the window class
    if (!RegisterClass(&wc)) {
        return -1;
    }

    // za menu
    #define EXIT 128
    #define ABOUT 256
    HMENU menu = CreateMenu();
    HMENU file = CreateMenu();
    HMENU help = CreateMenu();

    AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "filez");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)help, "hlep");

    AppendMenu(file, MF_STRING, EXIT, "exitearino");

    AppendMenu(help, MF_STRING, ABOUT, "abaut");



    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        "boinkorwindow",
        "boinkor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 300,
        NULL, menu, hInstance, NULL
    );

    HWND hEdit = CreateWindowEx(
        0,
        "EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 10, 400, 200,
        hwnd, NULL, hInstance, NULL
    );

    if (!hwnd) {
        return -1;
    }

    SetTimer(hwnd, 1, 1000, NULL);  // Timer ID 1, 1000 ms interval

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case EXIT:
                    PostQuitMessage(0);
                    break;
                case ABOUT:
                    MessageBox(hwnd, "the bestest textiest editorium of all time\n made by boink to boink", "abaut boinkor", MB_OK);
                    break;
            }
            break;
        case WM_TIMER:
            // Handle background refresh here (e.g., redrawing or updating content)
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230));  // Light blue color
                FillRect(hdc, &ps.rcPaint, hBrush);
                DeleteObject(hBrush);
                EndPaint(hwnd, &ps);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

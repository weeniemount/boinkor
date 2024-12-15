#include <windows.h>
#include <commdlg.h>  // For file dialogs
#include <stdio.h>    // For FILE structure and file handling

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hEdit;  // Declare hEdit globally

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

    // Menu definitions
    #define EXIT 128
    #define ABOUT 256
    #define SAVE 129
    #define OPEN 130
    HMENU menu = CreateMenu();
    HMENU file = CreateMenu();
    HMENU help = CreateMenu();

    AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "filez");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)help, "hlep");

    AppendMenu(file, MF_STRING, OPEN, "open");
    AppendMenu(file, MF_STRING, SAVE, "save");
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

    hEdit = CreateWindowEx(
        0,
        "EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
        0, 0, 0, 0,
        hwnd, NULL, hInstance, NULL
    );

    if (!hwnd) {
        return -1;
    }

    SetTimer(hwnd, 1, 1000, NULL);  // Timer ID 1, 1000 ms interval

    // Set font for the edit control
    HFONT hFont = CreateFont(
        16,            // Height of font
        0,             // Width of font
        0,             // Angle of escapement
        0,             // Orientation angle
        FW_NORMAL,     // Weight
        FALSE,         // Italic
        FALSE,         // Underline
        FALSE,         // StrikeOut
        ANSI_CHARSET,  // Character set identifier
        OUT_DEFAULT_PRECIS,  // Output precision
        CLIP_DEFAULT_PRECIS,  // Clipping precision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE,  // Pitch and family
        "Courier New"  // Font name
    );
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

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
                case SAVE:
                    {
                        OPENFILENAME ofn;
                        CHAR szFile[260] = {0};

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);  // Correct field
                        ofn.lpstrFilter = "Text Files\0*.txt\0";
                        ofn.lpstrFileTitle = NULL;
                        ofn.lpstrInitialDir = NULL;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

                        if (GetSaveFileName(&ofn)) {
                            FILE *file = fopen(ofn.lpstrFile, "w");
                            if (file) {
                                int len = GetWindowTextLength(hEdit);
                                CHAR *text = (CHAR *)GlobalAlloc(GPTR, (len + 1) * sizeof(CHAR));
                                GetWindowText(hEdit, text, len + 1);
                                fwrite(text, sizeof(CHAR), len, file);
                                fclose(file);
                                GlobalFree(text);
                            }
                        }
                    }
                    break;
                case OPEN:
                    {
                        OPENFILENAME ofn;
                        CHAR szFile[260] = {0};

                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);  // Correct field
                        ofn.lpstrFilter = "Text Files\0*.txt\0";
                        ofn.lpstrFileTitle = NULL;
                        ofn.lpstrInitialDir = NULL;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                        if (GetOpenFileName(&ofn)) {
                            FILE *file = fopen(ofn.lpstrFile, "r");
                            if (file) {
                                fseek(file, 0, SEEK_END);
                                int len = ftell(file);
                                fseek(file, 0, SEEK_SET);
                                CHAR *text = (CHAR *)GlobalAlloc(GPTR, (len + 1) * sizeof(CHAR));
                                fread(text, sizeof(CHAR), len, file);
                                text[len] = '\0';
                                SetWindowText(hEdit, text);
                                fclose(file);
                                GlobalFree(text);
                            }
                        }
                    }
                    break;
            }
            break;
        case WM_TIMER:
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230));
                FillRect(hdc, &ps.rcPaint, hBrush);
                DeleteObject(hBrush);
                EndPaint(hwnd, &ps);
            }
            break;
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

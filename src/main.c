#include <windows.h>
#include <commdlg.h>  // For file dialogs
#include <stdio.h>    // For FILE structure and file handling
#include <stdbool.h> 

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
    #define NEW_FILE 131
    #define EDIT 257  // New menu identifier for Edit menu
    #define TOGGLE_WRAP 258  // Identifier for the toggle word wrap option
    #define CHANGE_FONT 259

    HMENU menu = CreateMenu();
    HMENU file = CreateMenu();
    HMENU edit = CreateMenu();  // Create Edit menu
    HMENU help = CreateMenu();
    

    AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "filez");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)edit, "edita");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)help, "hlep");

    AppendMenu(file, MF_STRING, NEW_FILE, "new file");
    AppendMenu(file, MF_SEPARATOR, 0, NULL);  // Divider
    AppendMenu(file, MF_STRING, OPEN, "open");
    AppendMenu(file, MF_STRING, SAVE, "save");
    AppendMenu(file, MF_SEPARATOR, 0, NULL);  // Divider
    AppendMenu(file, MF_STRING, EXIT, "exitearino");
    AppendMenu(help, MF_STRING, ABOUT, "abaut");
    AppendMenu(edit, MF_STRING, CHANGE_FONT, "fonter");
    AppendMenu(edit, MF_STRING, TOGGLE_WRAP, "word wrap");

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

    //SetTimer(hwnd, 1, 1000, NULL);  // Timer ID 1, 1000 ms interval no timar needed nop more

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
    static HBITMAP hBitmap = NULL;
    static HDC hMemDC = NULL;
    static HFONT hPrevFont = NULL;  // To store the previous font
    static bool wordWrap = true;  // Word wrap state

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            if (hBitmap) {
                DeleteObject(hBitmap);
                hBitmap = NULL;
            }
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case TOGGLE_WRAP:
                    if (wordWrap) {
                        SendMessage(hEdit, EM_SETWORDBREAKPROC, 0, (LPARAM)(LRESULT)NULL);  // Enable word wrap
                    } else {
                        SendMessage(hEdit, EM_SETWORDBREAKPROC, 0, (LPARAM)(LRESULT)NULL);  // Disable word wrap
                    }
                    wordWrap = !wordWrap;

                    if (hPrevFont) {
                        DeleteObject(hPrevFont);  // Cleanup old font
                    }
                    
                    // Retrieve current font
                    hPrevFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);

                    // Get the current text
                    int len = GetWindowTextLength(hEdit);
                    CHAR *text = (CHAR *)GlobalAlloc(GPTR, (len + 1) * sizeof(CHAR));
                    GetWindowText(hEdit, text, len + 1);

                    // Create a new edit control with the same content and font
                    HWND hNewEdit = CreateWindowEx(
                        0,
                        "EDIT",
                        text,
                        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
                        0, 0, 0, 0,
                        hwnd, NULL, GetModuleHandle(NULL), NULL
                    );

                    if (hNewEdit) {
                        SendMessage(hNewEdit, WM_SETFONT, (WPARAM)hPrevFont, TRUE);
                        SetWindowPos(hEdit, hNewEdit, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE);
                        DestroyWindow(hNewEdit);  // Remove the temporary new control
                    }

                    GlobalFree(text);
                    break;
                case CHANGE_FONT:
                    CHOOSEFONT cf;  // Structure for font dialog
                    LOGFONT lf = {0};  // Initialize a LOGFONT structure
                    
                    // Get the current font from the edit control
                    HFONT hCurrentFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);
                    if (hCurrentFont) {
                        GetObject(hCurrentFont, sizeof(LOGFONT), &lf);  // Get font properties
                    }

                    ZeroMemory(&cf, sizeof(CHOOSEFONT));
                    cf.lStructSize = sizeof(CHOOSEFONT);
                    cf.hwndOwner = hwnd;
                    cf.lpLogFont = &lf;
                    cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;

                    if (ChooseFont(&cf)) {
                        HFONT hNewFont = CreateFontIndirect(&lf);
                        if (hNewFont) {
                            SendMessage(hEdit, WM_SETFONT, (WPARAM)hNewFont, TRUE);  // Set the new font
                        }
                    }
                    break;
                case EXIT:
                    PostQuitMessage(0);
                    break;
                case ABOUT:
                    MessageBox(hwnd, "the bestest textiest editorium of all time\nmade by boinkwer to boink", "abaut boinkor", MB_OK | MB_ICONINFORMATION);
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
                        ofn.nMaxFile = sizeof(szFile);  
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                        if (GetOpenFileName(&ofn)) {
                            HANDLE file = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                            if (file != INVALID_HANDLE_VALUE) {
                                DWORD bytesRead;
                                DWORD fileSize = GetFileSize(file, NULL);
                                CHAR *rawText = (CHAR *)HeapAlloc(GetProcessHeap(), 0, (fileSize + 1) * sizeof(CHAR));
                                CHAR *processedText = (CHAR *)HeapAlloc(GetProcessHeap(), 0, (fileSize * 2 + 1) * sizeof(CHAR)); // Allocate extra space for `\r\n`
                                if (rawText && processedText) {
                                    ReadFile(file, rawText, fileSize, &bytesRead, NULL);
                                    rawText[fileSize] = '\0';  // Null-terminate
                                    
                                    // Process line breaks
                                    CHAR *src = rawText;
                                    CHAR *dst = processedText;
                                    while (*src) {
                                        if (*src == '\n') {
                                            *dst++ = '\r';
                                            *dst++ = '\n';
                                        } else if (*src == '\r' && *(src + 1) != '\n') {
                                            *dst++ = '\r';
                                            *dst++ = '\n';
                                        } else {
                                            *dst++ = *src;
                                        }
                                        src++;
                                    }
                                    *dst = '\0'; // Null-terminate

                                    SetWindowText(hEdit, processedText);
                                }
                                HeapFree(GetProcessHeap(), 0, rawText);
                                HeapFree(GetProcessHeap(), 0, processedText);
                                CloseHandle(file);
                            }
                        }
                    }
                    break;

                case NEW_FILE:
                    SetWindowText(hEdit, "");
                    break;
            }
            break;
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                if (hBitmap) {
                    DeleteObject(hBitmap);
                }
                if (hMemDC) {
                    DeleteDC(hMemDC);
                }
                hMemDC = CreateCompatibleDC(NULL);
                hBitmap = CreateCompatibleBitmap(GetDC(hwnd), LOWORD(lParam), HIWORD(lParam));
                SelectObject(hMemDC, hBitmap);
                MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


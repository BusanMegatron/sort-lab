#include <windows.h>
#include <vector>
#include <string>
#include <sstream>

std::vector<int> numbers;
HWND inputField;


bool parseNumbers(const std::string& text) {
    numbers.clear();

    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            size_t pos = 0;
            int number = std::stoi(item, &pos);

            if (item.find_first_not_of(" \t", pos) != std::string::npos) {
                numbers.clear();
                return false;
            }

            numbers.push_back(number);
        }
        catch (...) {
            numbers.clear();
            return false;
        }
    }

    return !numbers.empty();
}


// Handles window events
LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {

        case WM_CREATE: {

            // Page title
            CreateWindowExA(
                0, "STATIC",
                "Enter Your Numbers",
                WS_VISIBLE | WS_CHILD,
                40, 40, 350, 40,
                hwnd, NULL, NULL, NULL
            );

            // Instructions
            inputField = CreateWindowExA(
                0, "STATIC",
                "Enter integers separated by commas:",
                WS_VISIBLE | WS_CHILD,
                40, 100, 400, 25,
                hwnd, NULL, NULL, NULL
            );

            // Number input
            CreateWindowExA(
                WS_EX_CLIENTEDGE, "EDIT",
                "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                40, 140, 400, 35,
                hwnd, (HMENU)101, NULL, NULL
            );

            // Continue button
            CreateWindowExA(
                0, "BUTTON",
                "Continue",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                40, 200, 150, 40,
                hwnd, (HMENU)102, NULL, NULL
            );

            return 0;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == 102 &&
            HIWORD(wParam) == BN_CLICKED) {

        char buffer[1024] = {};

HWND input = GetDlgItem(hwnd, 101);

GetWindowTextA(input, buffer, sizeof(buffer));

// Temporary debugging message
MessageBoxA(
    hwnd,
    buffer,
    "Debug - Received Input",
    MB_OK
);

        if (parseNumbers(buffer)) {
            MessageBoxA(
                hwnd,
                "Numbers successfully saved!",
                "SortLab",
                MB_OK | MB_ICONINFORMATION
            );
        }
        else {
            MessageBoxA(
                hwnd,
                "Invalid input! Enter integers separated by commas.",
                "Input Error",
                MB_OK | MB_ICONERROR
            );
        }
    }

    return 0;
}

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Application entry point
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {
    const char CLASS_NAME[] = "SortLabWindow";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "SortLab - Sorting Algorithm Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 600,
        NULL, NULL,
        hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

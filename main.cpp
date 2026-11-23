
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <cctype>

// ==============================
// Global variables
// ==============================

std::vector<int> numbers;

HWND inputField = NULL;

HWND screen1Controls[4];
HWND screen2Controls[7];

int screen1Count = 0;
int screen2Count = 0;

// ==============================
// Parse numbers from input
// ==============================

bool parseNumbers(const std::string& text) {
    std::vector<int> parsed;

    if (text.empty()) {
        return false;
    }

    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // Remove spaces at the beginning
        // and at the end of the item
        size_t start = item.find_first_not_of(" \t");
        size_t end = item.find_last_not_of(" \t");

        if (start == std::string::npos) {
            return false;
        }

        item = item.substr(start, end - start + 1);

        try {
            size_t pos = 0;
            long long value = std::stoll(item, &pos);

            // Check that the whole item is a number
            if (pos != item.size()) {
                return false;
            }

            // Check integer range
            if (value < std::numeric_limits<int>::min() ||
                value > std::numeric_limits<int>::max()) {
                return false;
            }

            parsed.push_back(static_cast<int>(value));
        }
        catch (...) {
            return false;
        }
    }

    // Reject a trailing comma
    size_t last = text.find_last_not_of(" \t");

    if (last == std::string::npos || text[last] == ',') {
        return false;
    }

    if (parsed.empty()) {
        return false;
    }

    numbers = parsed;
    return true;
}

// ==============================
// Screen switching
// ==============================

void showScreen(int screen) {
    for (int i = 0; i < screen1Count; i++) {
        ShowWindow(
            screen1Controls[i],
            screen == 1 ? SW_SHOW : SW_HIDE
        );
    }

    for (int i = 0; i < screen2Count; i++) {
        ShowWindow(
            screen2Controls[i],
            screen == 2 ? SW_SHOW : SW_HIDE
        );
    }
}

// ==============================
// Window event handler
// ==============================

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {

        case WM_CREATE: {

            // ======================
            // SCREEN 1 - INPUT
            // ======================

            screen1Controls[screen1Count++] = CreateWindowExA(
                0, "STATIC",
                "Enter Your Numbers",
                WS_VISIBLE | WS_CHILD,
                40, 40, 350, 40,
                hwnd, NULL, NULL, NULL
            );

            screen1Controls[screen1Count++] = CreateWindowExA(
                0, "STATIC",
                "Enter integers separated by commas:",
                WS_VISIBLE | WS_CHILD,
                40, 100, 400, 25,
                hwnd, NULL, NULL, NULL
            );

            inputField = CreateWindowExA(
                WS_EX_CLIENTEDGE, "EDIT",
                "",
                WS_VISIBLE | WS_CHILD |
                WS_BORDER | ES_AUTOHSCROLL,
                40, 140, 400, 35,
                hwnd, (HMENU)101, NULL, NULL
            );

            screen1Controls[screen1Count++] = inputField;

            screen1Controls[screen1Count++] = CreateWindowExA(
                0, "BUTTON",
                "Continue",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                40, 200, 150, 40,
                hwnd, (HMENU)102, NULL, NULL
            );

            // ======================
            // SCREEN 2 - SORTING
            // ======================

            screen2Controls[screen2Count++] = CreateWindowExA(
                0, "STATIC",
                "Choose Sorting Algorithm",
                WS_VISIBLE | WS_CHILD,
                40, 40, 400, 40,
                hwnd, NULL, NULL, NULL
            );

            const char* algorithms[] = {
                "Bubble Sort",
                "Selection Sort",
                "Insertion Sort",
                "Merge Sort",
                "Quick Sort"
            };

            for (int i = 0; i < 5; i++) {
                screen2Controls[screen2Count++] = CreateWindowExA(
                    0, "BUTTON",
                    algorithms[i],
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    40, 100 + i * 55, 250, 40,
                    hwnd,
                    (HMENU)(200 + i),
                    NULL,
                    NULL
                );
            }

            screen2Controls[screen2Count++] = CreateWindowExA(
                0, "BUTTON",
                "Back",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                40, 400, 150, 40,
                hwnd, (HMENU)205, NULL, NULL
            );

            // Show only the first screen
            showScreen(1);

            return 0;
        }

        case WM_COMMAND: {

            int controlId = LOWORD(wParam);

            // Continue button
            if (controlId == 102 &&
                HIWORD(wParam) == BN_CLICKED) {

                char buffer[1024] = {};

                HWND input = GetDlgItem(hwnd, 101);

                GetWindowTextA(
                    input,
                    buffer,
                    sizeof(buffer)
                );

                if (parseNumbers(buffer)) {
                    showScreen(2);
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

            // Back button
            if (controlId == 205 &&
                HIWORD(wParam) == BN_CLICKED) {
                showScreen(1);
            }

            // Sorting algorithm buttons
            if (controlId >= 200 &&
                controlId <= 204 &&
                HIWORD(wParam) == BN_CLICKED) {

                const char* algorithms[] = {
                    "Bubble Sort",
                    "Selection Sort",
                    "Insertion Sort",
                    "Merge Sort",
                    "Quick Sort"
                };

                MessageBoxA(
                    hwnd,
                    algorithms[controlId - 200],
                    "Selected Algorithm",
                    MB_OK | MB_ICONINFORMATION
                );
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// ==============================
// Application entry point
// ==============================

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
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc)) {
        return 1;
    }

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "SortLab - Sorting Algorithm Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 600,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

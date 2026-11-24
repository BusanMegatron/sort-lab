
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>


std::vector<int> numbers;

struct SortStep {
    std::vector<int> array;
    int index1;
    int index2;
    std::string description;
};

std::vector<SortStep> sortingSteps;

HWND inputField = NULL;

HWND screen1Controls[4] = {};
HWND screen2Controls[7] = {};

int screen1Count = 0;
int screen2Count = 0;


bool parseNumbers(const std::string& text) {
    std::vector<int> parsed;

    if (text.empty()) {
        return false;
    }

    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ',')) {

        size_t start = item.find_first_not_of(" \t");
        size_t end = item.find_last_not_of(" \t");

        if (start == std::string::npos) {
            return false;
        }

        item = item.substr(start, end - start + 1);

        try {
            size_t pos = 0;
            long long value = std::stoll(item, &pos);

            if (pos != item.size()) {
                return false;
            }

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


void bubbleSort(const std::vector<int>& input) {

    sortingSteps.clear();

    std::vector<int> arr = input;
    int n = static_cast<int>(arr.size());

    // Save initial state
    sortingSteps.push_back({
        arr,
        -1,
        -1,
        "Initial array"
    });

    for (int i = 0; i < n - 1; i++) {

        bool swapped = false;

        for (int j = 0; j < n - i - 1; j++) {

            // Save comparison
            sortingSteps.push_back({
                arr,
                j,
                j + 1,
                "Compare " + std::to_string(arr[j]) +
                " and " + std::to_string(arr[j + 1])
            });

            if (arr[j] > arr[j + 1]) {

                int leftValue = arr[j];
                int rightValue = arr[j + 1];

                std::swap(arr[j], arr[j + 1]);
                swapped = true;

                // Save swap
                sortingSteps.push_back({
                    arr,
                    j,
                    j + 1,
                    "Swap " + std::to_string(leftValue) +
                    " and " + std::to_string(rightValue) +
                    " because " + std::to_string(leftValue) +
                    " is greater than " +
                    std::to_string(rightValue)
                });
            }
        }

        // Stop if the array is already sorted
        if (!swapped) {
            break;
        }
    }

    // Save final state
    sortingSteps.push_back({
        arr,
        -1,
        -1,
        "Sorting completed!"
    });
}


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


LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {

    switch (message) {

        case WM_CREATE: {


            screen1Controls[screen1Count++] =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Enter Your Numbers",
                    WS_VISIBLE | WS_CHILD,
                    40, 40, 350, 40,
                    hwnd, NULL, NULL, NULL
                );

            screen1Controls[screen1Count++] =
                CreateWindowExA(
                    0,
                    "STATIC",
                    "Enter integers separated by commas:",
                    WS_VISIBLE | WS_CHILD,
                    40, 100, 400, 25,
                    hwnd, NULL, NULL, NULL
                );

            inputField = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_VISIBLE | WS_CHILD |
                WS_BORDER | ES_AUTOHSCROLL,
                40, 140, 400, 35,
                hwnd,
                (HMENU)(INT_PTR)101,
                NULL,
                NULL
            );

            screen1Controls[screen1Count++] = inputField;

            screen1Controls[screen1Count++] =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Continue",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    40, 200, 150, 40,
                    hwnd,
                    (HMENU)(INT_PTR)102,
                    NULL,
                    NULL
                );



            screen2Controls[screen2Count++] =
                CreateWindowExA(
                    0,
                    "STATIC",
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

                screen2Controls[screen2Count++] =
                    CreateWindowExA(
                        0,
                        "BUTTON",
                        algorithms[i],
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        40, 100 + i * 55, 250, 40,
                        hwnd,
                        (HMENU)(INT_PTR)(200 + i),
                        NULL,
                        NULL
                    );
            }

            screen2Controls[screen2Count++] =
                CreateWindowExA(
                    0,
                    "BUTTON",
                    "Back",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    40, 400, 150, 40,
                    hwnd,
                    (HMENU)(INT_PTR)205,
                    NULL,
                    NULL
                );

            showScreen(1);

            return 0;
        }



        case WM_COMMAND: {

            int controlId = LOWORD(wParam);

            if (HIWORD(wParam) != BN_CLICKED) {
                return 0;
            }

            // CONTINUE
            if (controlId == 102) {

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

                return 0;
            }

            // BACK
            if (controlId == 205) {

                showScreen(1);

                return 0;
            }

            // BUBBLE SORT
            if (controlId == 200) {

                bubbleSort(numbers);

                std::string result =
                    "Bubble Sort generated " +
                    std::to_string(sortingSteps.size()) +
                    " visualization steps.";

                MessageBoxA(
                    hwnd,
                    result.c_str(),
                    "Bubble Sort",
                    MB_OK | MB_ICONINFORMATION
                );

                return 0;
            }

            // OTHER SORTING ALGORITHMS
            if (controlId >= 201 && controlId <= 204) {

                const char* algorithms[] = {
                    "Selection Sort",
                    "Insertion Sort",
                    "Merge Sort",
                    "Quick Sort"
                };

                MessageBoxA(
                    hwnd,
                    algorithms[controlId - 201],
                    "Selected Algorithm",
                    MB_OK | MB_ICONINFORMATION
                );

                return 0;
            }

            return 0;
        }

        case WM_DESTROY:

            PostQuitMessage(0);

            return 0;
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
    );
}


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
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        900,
        600,
        NULL,
        NULL,
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

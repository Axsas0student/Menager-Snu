#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <CommCtrl.h>
#include "SleepAnalyzer.h"
#include <stdexcept>
#include <sstream>

#pragma comment(lib, "Comctl32.lib")

SleepAnalyzer analyzer;
std::string currentProfile;



LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProfileWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void LoadProfileData(const std::string& profileName) {
    std::ifstream file(profileName + ".txt");
    if (file.is_open()) {
        SleepData data;
        while (file >> data.date >> data.sleepDuration >> data.wakeUpTimes) {
            analyzer.addSleepData(data);
        }
        file.close();
    }
}

void SaveProfileData(const std::string& profileName) {
    std::ofstream file(profileName + ".txt", std::ofstream::trunc);
    if (file.is_open()) {
        for (const auto& data : analyzer.getSleepData()) {
            file << data.date << " " << data.sleepDuration << " " << data.wakeUpTimes << "\n";
        }
        file.close();
    }
}

bool IsValidDateFormat(const std::string& date) {
    if (date.length() != 10) // d³ daty
        return false;

    if (date[4] != '-' || date[7] != '-') // separator
        return false;

    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (month < 1 || month > 12)
            return false;

        if (day < 1)
            return false;

        if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && day > 31)
            return false;
        else if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
            return false;
        else if (month == 2) { // Luty
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                if (day > 29)
                    return false;
            }
            else {
                if (day > 28)
                    return false;
            }
        }
    }
    catch (...) {   //b³ konwersji
        return false;
    }

    return true;
}

void AddData(HWND hwnd) {
    if (currentProfile.empty()) {
        MessageBoxA(hwnd, "Najpierw wybierz profil.", "B³¹d", MB_OK | MB_ICONERROR);
        return;
    }

    char dateBuffer[11];    //dek tab daty
    GetDlgItemTextA(hwnd, 1, dateBuffer, 11);
    std::string date = dateBuffer;  //tab na sting

    // Walidacja daty
    if (!IsValidDateFormat(date)) {
        MessageBoxA(hwnd, "Nieprawid³owy format daty. Poprawny format to RRRR-MM-DD.", "B³¹d", MB_OK | MB_ICONERROR);
        return;
    }

    int sleepDuration = GetDlgItemInt(hwnd, 2, NULL, FALSE);
    int wakeUpTimes = GetDlgItemInt(hwnd, 3, NULL, FALSE);

    SleepData data;     //inicjowanie i przypisywania obiekt. do klasy
    data.date = date;
    data.sleepDuration = sleepDuration;
    data.wakeUpTimes = wakeUpTimes;

    try {
        // Dodaj dane
        analyzer.addSleepData(data);
        SaveProfileData(currentProfile);

        MessageBoxA(hwnd, "Dane snu zosta³y dodane pomyœlnie.", "Informacja", MB_OK);

        // Czyszczenie pól tekstowych
        SetDlgItemTextA(hwnd, 1, "");
        SetDlgItemTextA(hwnd, 2, "");
        SetDlgItemTextA(hwnd, 3, "");
    }
    catch (const std::exception& e) {
        MessageBoxA(hwnd, e.what(), "B³¹d", MB_OK | MB_ICONERROR);
    }
}

void GenerateReport(HWND hwnd) {
    std::string report = analyzer.generateReport();
    MessageBoxA(hwnd, report.c_str(), "Raport Analizy Snu", MB_OK);
}

//obs³uga entera
void HandleEnterKey(HWND hwnd, HWND currentFocus) {
    if (currentFocus == GetDlgItem(hwnd, 1)) {
        SetFocus(GetDlgItem(hwnd, 2));
    }
    else if (currentFocus == GetDlgItem(hwnd, 2)) {
        SetFocus(GetDlgItem(hwnd, 3));
    }
    else if (currentFocus == GetDlgItem(hwnd, 3)) {
        SetFocus(GetDlgItem(hwnd, 4));  // Przycisk "Dodaj dane"
    }
}

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        HWND parent = GetParent(hwnd);
        HandleEnterKey(parent, hwnd);
        return 0; // Przechwyæ Enter i nie pozwól, aby standardowa obs³uga mia³a miejsce
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void ChooseProfile(HWND hwnd) {
    char profileName[100];      //max 100 znaków na profil
    GetDlgItemTextA(hwnd, 6, profileName, 100);

    if (strlen(profileName) == 0) {
        MessageBoxA(hwnd, "Proszê wpisaæ nazwê profilu.", "B³¹d", MB_OK | MB_ICONERROR);
        return;
    }

    currentProfile = profileName;
    analyzer.clearSleepData();
    LoadProfileData(currentProfile);

    MessageBoxA(hwnd, ("Wybrano profil: " + currentProfile).c_str(), "Informacja", MB_OK);

    // Prze³¹cz na g³ówne okno wprowadzania danych
    HWND mainHwnd = FindWindowA(NULL, "Sleep Analyzer");
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(mainHwnd, SW_SHOW);
}

void GoToProfileSelection(HWND hwnd) {
    // Prze³¹cz na okno wyboru profilu
    HWND profileHwnd = FindWindowA(NULL, "Profile Selection");
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(profileHwnd, SW_SHOW);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Rejestracja klasy okna g³ównego
    WNDCLASSA wc = {};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Sleep Analyzer";

    RegisterClassA(&wc);

    // Rejestracja klasy okna wyboru profilu
    WNDCLASSA wcProfile = {};
    wcProfile.lpfnWndProc = ProfileWndProc;
    wcProfile.hInstance = hInstance;
    wcProfile.lpszClassName = "Profile Selection";

    RegisterClassA(&wcProfile);

    // Utworzenie okna g³ównego
    HWND mainHwnd = CreateWindowExA(
        0,
        "Sleep Analyzer",
        "Sleep Analyzer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (mainHwnd == NULL) {
        return 0;
    }

    // Utworzenie okna wyboru profilu
    HWND profileHwnd = CreateWindowExA(
        0,
        "Profile Selection",
        "Profile Selection",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (profileHwnd == NULL) {
        return 0;
    }

    // Poka¿ okno wyboru profilu
    ShowWindow(profileHwnd, nCmdShow);
    ShowWindow(mainHwnd, SW_HIDE);

    MSG msg = {};
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}

//procedura okna g³ównego
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        HWND hEdit1 = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 20, 100, 20, hwnd, (HMENU)1, NULL, NULL);
        HWND hEdit2 = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 180, 50, 100, 20, hwnd, (HMENU)2, NULL, NULL);
        HWND hEdit3 = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 180, 80, 100, 20, hwnd, (HMENU)3, NULL, NULL);
        CreateWindowA("STATIC", "Data (RRRR-MM-DD):", WS_VISIBLE | WS_CHILD, 20, 20, 150, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("STATIC", "Czas snu (minuty):", WS_VISIBLE | WS_CHILD, 20, 50, 150, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("STATIC", "Liczba przebudzeñ:", WS_VISIBLE | WS_CHILD, 20, 80, 150, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("BUTTON", "Dodaj dane", WS_VISIBLE | WS_CHILD, 20, 120, 120, 30, hwnd, (HMENU)4, NULL, NULL);
        CreateWindowA("BUTTON", "Generuj raport", WS_VISIBLE | WS_CHILD, 160, 120, 120, 30, hwnd, (HMENU)5, NULL, NULL);
        CreateWindowA("BUTTON", "Wróæ do wyboru profilu", WS_VISIBLE | WS_CHILD, 20, 160, 200, 30, hwnd, (HMENU)8, NULL, NULL);

        // Subclassing edytorów, aby przechwytywaæ Enter
        SetWindowSubclass(hEdit1, EditSubclassProc, 0, 0);
        SetWindowSubclass(hEdit2, EditSubclassProc, 0, 0);
        SetWindowSubclass(hEdit3, EditSubclassProc, 0, 0);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 4) {
            AddData(hwnd);
        }
        else if (LOWORD(wParam) == 5) {
            GenerateReport(hwnd);
        }
        else if (LOWORD(wParam) == 8) {
            GoToProfileSelection(hwnd);
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ProfileWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowA("STATIC", "Nazwa profilu:", WS_VISIBLE | WS_CHILD, 20, 20, 150, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 180, 20, 200, 20, hwnd, (HMENU)6, NULL, NULL);
        CreateWindowA("BUTTON", "Wybierz profil", WS_VISIBLE | WS_CHILD, 400, 20, 120, 30, hwnd, (HMENU)7, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 7) {
            ChooseProfile(hwnd);
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}


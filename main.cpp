#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "URLShortener.h"

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printBanner() {
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║           URL SHORTENER & MANAGER v2.0              ║\n";
    cout << "  ║     Shorten  |  Track  |  Manage  |  Analyze        ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
}

void printMenu() {
    cout << "\n  ┌─────────────── MAIN MENU ───────────────┐\n";
    cout << "  │                                          │\n";
    cout << "  │  --- URL Operations ---                  │\n";
    cout << "  │  1.  Shorten a URL                       │\n";
    cout << "  │  2.  Shorten with custom alias           │\n";
    cout << "  │  3.  Shorten with options (full)         │\n";
    cout << "  │  4.  Bulk shorten URLs                   │\n";
    cout << "  │  5.  Decode a short URL                  │\n";
    cout << "  │  6.  Redirect (simulate click)           │\n";
    cout << "  │                                          │\n";
    cout << "  │  --- URL Management ---                  │\n";
    cout << "  │  7.  Update a URL                        │\n";
    cout << "  │  8.  Delete a URL                        │\n";
    cout << "  │  9.  Set URL title                       │\n";
    cout << "  │  10. Set URL category                    │\n";
    cout << "  │  11. Set URL expiration                  │\n";
    cout << "  │  12. Remove URL expiration               │\n";
    cout << "  │  13. Set password protection              │\n";
    cout << "  │                                          │\n";
    cout << "  │  --- View & Search ---                   │\n";
    cout << "  │  14. View all URLs                       │\n";
    cout << "  │  15. View URL analytics                  │\n";
    cout << "  │  16. Search URLs                         │\n";
    cout << "  │  17. View recent URLs                    │\n";
    cout << "  │  18. View URLs by category               │\n";
    cout << "  │  19. Show QR code (text)                 │\n";
    cout << "  │                                          │\n";
    cout << "  │  --- Stats & System ---                  │\n";
    cout << "  │  20. Global statistics                   │\n";
    cout << "  │  21. Export all data                     │\n";
    cout << "  │                                          │\n";
    cout << "  │  0.  Exit                                │\n";
    cout << "  │                                          │\n";
    cout << "  └──────────────────────────────────────────┘\n";
    cout << "  Enter choice: ";
}

void printShortenedResult(const string& code, const string& longURL) {
    if (code.substr(0, 5) == "ERROR") {
        cout << "\n  [FAILED] " << code << "\n";
    } else {
        cout << "\n  ==========================================\n";
        cout << "  SHORTENED SUCCESSFULLY!\n";
        cout << "  Original  : " << longURL << "\n";
        cout << "  Short URL : https://short.url/" << code << "\n";
        cout << "  Code      : " << code << "\n";
        cout << "  ==========================================\n";
    }
}

int main() {
    URLShortener shortener;
    int choice;
    string input, input2, input3, input4;
    int inputInt;

    printBanner();

    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n  [ERROR] Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                cout << "\n  Enter the long URL: ";
                getline(cin, input);
                string code = shortener.shortenURL(input);
                printShortenedResult(code, input);
                break;
            }
            case 2: {
                cout << "\n  Enter the long URL: ";
                getline(cin, input);
                cout << "  Enter custom alias: ";
                getline(cin, input2);
                string code = shortener.shortenURL(input, input2);
                printShortenedResult(code, input);
                break;
            }
            case 3: {
                cout << "\n  Enter the long URL: ";
                getline(cin, input);
                cout << "  Enter custom alias (or press Enter to skip): ";
                getline(cin, input2);
                cout << "  Enter category (or press Enter for none): ";
                getline(cin, input3);
                cout << "  Enter expiry in days (0 for no expiry): ";
                cin >> inputInt;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                string code;
                if (input2.empty()) {
                    code = shortener.shortenURL(input);
                    if (input3.empty() && inputInt > 0) {
                        shortener.setExpiration(code, inputInt);
                    }
                    if (!input3.empty()) {
                        shortener.setCategory(code, input3);
                        if (inputInt > 0) shortener.setExpiration(code, inputInt);
                    }
                } else {
                    code = shortener.shortenURL(input, input2, input3, inputInt);
                }
                printShortenedResult(code, input);
                break;
            }
            case 4: {
                cout << "\n  Enter URLs (one per line, empty line to finish):\n";
                vector<string> urls;
                while (true) {
                    cout << "  > ";
                    getline(cin, input);
                    if (input.empty()) break;
                    urls.push_back(input);
                }
                if (!urls.empty()) {
                    string result = shortener.bulkShorten(urls);
                    cout << result;
                } else {
                    cout << "\n  [INFO] No URLs entered.\n";
                }
                break;
            }
            case 5: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                string url = shortener.decodeURL(input);
                if (url.substr(0, 5) == "ERROR") {
                    cout << "\n  [FAILED] " << url << "\n";
                } else {
                    cout << "\n  Short Code : " << input << "\n";
                    cout << "  Original   : " << url << "\n";
                }
                break;
            }
            case 6: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                bool hasPass = shortener.verifyPassword(input, "");
                if (hasPass) {
                    cout << "  This URL is password protected.\n";
                    cout << "  Enter password: ";
                    getline(cin, input2);
                    shortener.redirect(input, input2);
                } else {
                    shortener.redirect(input);
                }
                break;
            }
            case 7: {
                cout << "\n  Enter short code to update: ";
                getline(cin, input);
                cout << "  Enter new URL: ";
                getline(cin, input2);
                if (shortener.updateURL(input, input2)) {
                    cout << "\n  [SUCCESS] URL updated.\n";
                } else {
                    cout << "\n  [FAILED] Could not update.\n";
                }
                break;
            }
            case 8: {
                cout << "\n  Enter short code to delete: ";
                getline(cin, input);
                if (shortener.deleteURL(input)) {
                    cout << "\n  [SUCCESS] URL deleted.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 9: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                cout << "  Enter title: ";
                getline(cin, input2);
                if (shortener.setTitle(input, input2)) {
                    cout << "\n  [SUCCESS] Title set.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 10: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                cout << "  Enter category: ";
                getline(cin, input2);
                if (shortener.setCategory(input, input2)) {
                    cout << "\n  [SUCCESS] Category set.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 11: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                cout << "  Enter expiry in days: ";
                cin >> inputInt;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (shortener.setExpiration(input, inputInt)) {
                    cout << "\n  [SUCCESS] Expiration set.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 12: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                if (shortener.removeExpiration(input)) {
                    cout << "\n  [SUCCESS] Expiration removed.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 13: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                cout << "  Enter password: ";
                getline(cin, input2);
                if (shortener.setPassword(input, input2)) {
                    cout << "\n  [SUCCESS] Password protection enabled.\n";
                } else {
                    cout << "\n  [FAILED] URL not found.\n";
                }
                break;
            }
            case 14: {
                shortener.printAllURLs();
                break;
            }
            case 15: {
                cout << "\n  Enter short code for analytics: ";
                getline(cin, input);
                shortener.printAnalytics(input);
                break;
            }
            case 16: {
                cout << "\n  Enter search keyword: ";
                getline(cin, input);
                vector<string> results = shortener.searchURLs(input);
                if (results.empty()) {
                    cout << "\n  [INFO] No results found.\n";
                } else {
                    cout << "\n  Found " << results.size() << " result(s):\n";
                    for (int i = 0; i < (int)results.size(); i++) {
                        string url = shortener.decodeURL(results[i]);
                        cout << "  " << (i + 1) << ". " << results[i] << " -> " << url << "\n";
                    }
                }
                break;
            }
            case 17: {
                cout << "\n  How many recent URLs? ";
                cin >> inputInt;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                vector<string> recent = shortener.getRecentURLs(inputInt);
                if (recent.empty()) {
                    cout << "\n  [INFO] No URLs stored.\n";
                } else {
                    cout << "\n  Recent URLs:\n";
                    for (int i = 0; i < (int)recent.size(); i++) {
                        cout << "  " << (i + 1) << ". " << recent[i] << "\n";
                    }
                }
                break;
            }
            case 18: {
                cout << "\n  Enter category: ";
                getline(cin, input);
                vector<string> catURLs = shortener.getByCategory(input);
                if (catURLs.empty()) {
                    cout << "\n  [INFO] No URLs in this category.\n";
                } else {
                    cout << "\n  URLs in category '" << input << "':\n";
                    for (int i = 0; i < (int)catURLs.size(); i++) {
                        cout << "  " << (i + 1) << ". " << catURLs[i] << "\n";
                    }
                }
                break;
            }
            case 19: {
                cout << "\n  Enter short code: ";
                getline(cin, input);
                shortener.printQRCode(input);
                break;
            }
            case 20: {
                shortener.printStats();
                break;
            }
            case 21: {
                vector<string> allCodes = shortener.getAllShortCodes();
                cout << "\n  ==========================================\n";
                cout << "  DATA EXPORT\n";
                cout << "  ==========================================\n";
                for (const string& code : allCodes) {
                    string url = shortener.decodeURL(code);
                    int clicks = shortener.getClickCount(code);
                    string cat = shortener.getCategory(code);
                    cout << "  " << code << " | " << url << " | " << cat << " | Clicks: " << clicks << "\n";
                }
                cout << "  ==========================================\n";
                cout << "  [NOTE] Full data saved to url_data.csv\n";
                break;
            }
            case 0: {
                cout << "\n  ╔══════════════════════════════════════════╗\n";
                cout << "  ║   Thanks for using URL Shortener v2.0!  ║\n";
                cout << "  ║   Your data has been saved. Goodbye!    ║\n";
                cout << "  ╚══════════════════════════════════════════╝\n\n";
                return 0;
            }
            default:
                cout << "\n  [ERROR] Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

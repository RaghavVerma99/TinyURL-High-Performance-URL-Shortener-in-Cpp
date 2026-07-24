#include "URLShortener.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <cstdlib>

const string URLShortener::BASE62_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int URLShortener::SHORT_CODE_LENGTH = 6;
const int URLShortener::MAX_CUSTOM_ALIAS_LENGTH = 20;

URLShortener::URLShortener() : totalURLs(0), totalClicks(0) {
    loadFromFile();
}

string URLShortener::generateShortCode() {
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(0, BASE62_CHARS.size() - 1);
    string code;
    for (int i = 0; i < SHORT_CODE_LENGTH; i++) {
        code += BASE62_CHARS[dist(rng)];
    }
    return code;
}

string URLShortener::base62Encode(long long num) {
    if (num == 0) return string(1, BASE62_CHARS[0]);
    string result;
    while (num > 0) {
        result = BASE62_CHARS[num % 62] + result;
        num /= 62;
    }
    return result;
}

bool URLShortener::isValidURL(const string& url) {
    if (url.empty()) return false;
    string lower = toLowerCase(url);
    if (lower.substr(0, 7) == "http://" || lower.substr(0, 8) == "https://") {
        if (url.find('.') != string::npos && url.find(' ') == string::npos) {
            return true;
        }
    }
    if (url.find('.') != string::npos && url.find(' ') == string::npos &&
        url.find("://") == string::npos) {
        return true;
    }
    return false;
}

bool URLShortener::isValidAlias(const string& alias) {
    if (alias.empty() || alias.length() > MAX_CUSTOM_ALIAS_LENGTH) return false;
    for (char c : alias) {
        if (!isalnum(c) && c != '-' && c != '_') return false;
    }
    return true;
}

bool URLShortener::isExpired(const URLRecord& record) {
    if (record.expiresAt == 0) return false;
    return time(nullptr) > record.expiresAt;
}

string URLShortener::timeToString(time_t t) {
    if (t == 0) return "Never";
    tm* ltm = localtime(&t);
    ostringstream oss;
    oss << put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

string URLShortener::toLowerCase(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void URLShortener::saveToFile() {
    ofstream file("url_data.csv");
    if (!file.is_open()) return;
    file << "shortCode,longURL,customAlias,title,category,password,createdAt,expiresAt,clickCount\n";
    for (auto& pair : shortToLong) {
        const URLRecord& rec = pair.second;
        file << rec.shortCode << ","
             << rec.longURL << ","
             << rec.customAlias << ","
             << rec.title << ","
             << rec.category << ","
             << rec.password << ","
             << rec.createdAt << ","
             << rec.expiresAt << ","
             << rec.clickCount << "\n";
    }
    file.close();
}

void URLShortener::loadFromFile() {
    ifstream file("url_data.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line);
    while (getline(file, line)) {
        istringstream ss(line);
        URLRecord rec;
        string token;
        getline(ss, rec.shortCode, ',');
        getline(ss, rec.longURL, ',');
        getline(ss, rec.customAlias, ',');
        getline(ss, rec.title, ',');
        getline(ss, rec.category, ',');
        getline(ss, rec.password, ',');
        string ca, ea, cc;
        getline(ss, ca, ',');
        getline(ss, ea, ',');
        getline(ss, cc, ',');
        rec.createdAt = stoll(ca);
        rec.expiresAt = stoll(ea);
        rec.clickCount = stoi(cc);
        rec.isPasswordProtected = !rec.password.empty();
        shortToLong[rec.shortCode] = rec;
        longToShort[rec.longURL] = rec;
        if (!rec.category.empty()) categoryCount[rec.category]++;
        totalURLs++;
        totalClicks += rec.clickCount;
    }
    file.close();
}

string URLShortener::shortenURL(const string& longURL) {
    if (!isValidURL(longURL)) return "ERROR: Invalid URL";
    if (longToShort.count(longURL)) {
        return shortToLong[longToShort[longURL].shortCode].shortCode;
    }
    string code = generateShortCode();
    while (shortToLong.count(code)) code = generateShortCode();
    URLRecord rec;
    rec.longURL = longURL;
    rec.shortCode = code;
    rec.customAlias = "";
    rec.title = "";
    rec.category = "";
    rec.password = "";
    rec.createdAt = time(nullptr);
    rec.expiresAt = 0;
    rec.clickCount = 0;
    rec.isPasswordProtected = false;
    shortToLong[code] = rec;
    longToShort[longURL] = rec;
    totalURLs++;
    saveToFile();
    return code;
}

string URLShortener::shortenURL(const string& longURL, const string& customAlias) {
    if (!isValidURL(longURL)) return "ERROR: Invalid URL";
    if (!isValidAlias(customAlias)) return "ERROR: Invalid alias (alphanumeric, -, _ only)";
    if (shortToLong.count(customAlias)) return "ERROR: Alias already taken";
    if (longToShort.count(longURL)) {
        string existing = longToShort[longURL].shortCode;
        if (existing != customAlias) return "ERROR: URL already shortened as " + existing;
    }
    URLRecord rec;
    rec.longURL = longURL;
    rec.shortCode = customAlias;
    rec.customAlias = customAlias;
    rec.title = "";
    rec.category = "";
    rec.password = "";
    rec.createdAt = time(nullptr);
    rec.expiresAt = 0;
    rec.clickCount = 0;
    rec.isPasswordProtected = false;
    shortToLong[customAlias] = rec;
    longToShort[longURL] = rec;
    totalURLs++;
    saveToFile();
    return customAlias;
}

string URLShortener::shortenURL(const string& longURL, const string& customAlias,
                                 const string& category, int expiryDays) {
    if (!isValidURL(longURL)) return "ERROR: Invalid URL";
    if (!isValidAlias(customAlias)) return "ERROR: Invalid alias";
    if (shortToLong.count(customAlias)) return "ERROR: Alias already taken";
    if (longToShort.count(longURL)) return "ERROR: URL already shortened";
    URLRecord rec;
    rec.longURL = longURL;
    rec.shortCode = customAlias;
    rec.customAlias = customAlias;
    rec.title = "";
    rec.category = category;
    rec.password = "";
    rec.createdAt = time(nullptr);
    rec.expiresAt = (expiryDays > 0) ? time(nullptr) + (expiryDays * 86400) : 0;
    rec.clickCount = 0;
    rec.isPasswordProtected = false;
    shortToLong[customAlias] = rec;
    longToShort[longURL] = rec;
    totalURLs++;
    if (!category.empty()) categoryCount[category]++;
    saveToFile();
    return customAlias;
}

string URLShortener::decodeURL(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return "ERROR: Short URL not found";
    URLRecord& rec = shortToLong[shortCode];
    if (isExpired(rec)) return "ERROR: This URL has expired";
    return rec.longURL;
}

bool URLShortener::redirect(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return false;
    URLRecord& rec = shortToLong[shortCode];
    if (isExpired(rec)) return false;
    rec.clickCount++;
    rec.clickHistory.push_back(time(nullptr));
    totalClicks++;
    cout << "\n[REDIRECT] " << rec.longURL << "\n";
    saveToFile();
    return true;
}

bool URLShortener::redirect(const string& shortCode, const string& password) {
    if (!shortToLong.count(shortCode)) return false;
    URLRecord& rec = shortToLong[shortCode];
    if (isExpired(rec)) return false;
    if (rec.isPasswordProtected && rec.password != password) {
        cout << "\n[ACCESS DENIED] Invalid password.\n";
        return false;
    }
    rec.clickCount++;
    rec.clickHistory.push_back(time(nullptr));
    totalClicks++;
    cout << "\n[REDIRECT] " << rec.longURL << "\n";
    saveToFile();
    return true;
}

bool URLShortener::setExpiration(const string& shortCode, int days) {
    if (!shortToLong.count(shortCode)) return false;
    shortToLong[shortCode].expiresAt = time(nullptr) + (days * 86400);
    saveToFile();
    return true;
}

bool URLShortener::removeExpiration(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return false;
    shortToLong[shortCode].expiresAt = 0;
    saveToFile();
    return true;
}

bool URLShortener::isExpiredURL(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return false;
    return isExpired(shortToLong[shortCode]);
}

bool URLShortener::setCategory(const string& shortCode, const string& category) {
    if (!shortToLong.count(shortCode)) return false;
    string oldCat = shortToLong[shortCode].category;
    if (!oldCat.empty() && categoryCount[oldCat] > 0) categoryCount[oldCat]--;
    shortToLong[shortCode].category = category;
    categoryCount[category]++;
    saveToFile();
    return true;
}

string URLShortener::getCategory(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return "NOT FOUND";
    return shortToLong[shortCode].category.empty() ? "Uncategorized" : shortToLong[shortCode].category;
}

vector<string> URLShortener::getByCategory(const string& category) {
    vector<string> results;
    for (auto& pair : shortToLong) {
        if (pair.second.category == category) {
            results.push_back(pair.second.shortCode);
        }
    }
    return results;
}

bool URLShortener::setTitle(const string& shortCode, const string& title) {
    if (!shortToLong.count(shortCode)) return false;
    shortToLong[shortCode].title = title;
    saveToFile();
    return true;
}

string URLShortener::getTitle(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return "NOT FOUND";
    return shortToLong[shortCode].title.empty() ? "No title set" : shortToLong[shortCode].title;
}

bool URLShortener::setPassword(const string& shortCode, const string& password) {
    if (!shortToLong.count(shortCode)) return false;
    shortToLong[shortCode].password = password;
    shortToLong[shortCode].isPasswordProtected = true;
    saveToFile();
    return true;
}

bool URLShortener::verifyPassword(const string& shortCode, const string& password) {
    if (!shortToLong.count(shortCode)) return false;
    return shortToLong[shortCode].password == password;
}

int URLShortener::getClickCount(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return -1;
    return shortToLong[shortCode].clickCount;
}

vector<time_t> URLShortener::getClickHistory(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return {};
    return shortToLong[shortCode].clickHistory;
}

void URLShortener::printAnalytics(const string& shortCode) {
    if (!shortToLong.count(shortCode)) {
        cout << "\n[ERROR] URL not found.\n";
        return;
    }
    const URLRecord& rec = shortToLong[shortCode];
    cout << "\n========== ANALYTICS ==========\n";
    cout << "  Short Code    : " << rec.shortCode << "\n";
    cout << "  Long URL      : " << rec.longURL << "\n";
    cout << "  Title         : " << (rec.title.empty() ? "N/A" : rec.title) << "\n";
    cout << "  Category      : " << (rec.category.empty() ? "Uncategorized" : rec.category) << "\n";
    cout << "  Created       : " << timeToString(rec.createdAt) << "\n";
    cout << "  Expires       : " << timeToString(rec.expiresAt) << "\n";
    cout << "  Total Clicks  : " << rec.clickCount << "\n";
    cout << "  Password      : " << (rec.isPasswordProtected ? "Yes" : "No") << "\n";
    if (!rec.clickHistory.empty()) {
        cout << "  Last 5 Clicks :\n";
        int start = max(0, (int)rec.clickHistory.size() - 5);
        for (int i = start; i < (int)rec.clickHistory.size(); i++) {
            cout << "    -> " << timeToString(rec.clickHistory[i]) << "\n";
        }
    }
    cout << "================================\n";
}

vector<string> URLShortener::searchURLs(const string& keyword) {
    vector<string> results;
    string lowerKey = toLowerCase(keyword);
    for (auto& pair : shortToLong) {
        const URLRecord& rec = pair.second;
        if (toLowerCase(rec.longURL).find(lowerKey) != string::npos ||
            toLowerCase(rec.title).find(lowerKey) != string::npos ||
            toLowerCase(rec.category).find(lowerKey) != string::npos ||
            toLowerCase(rec.shortCode).find(lowerKey) != string::npos) {
            results.push_back(rec.shortCode);
        }
    }
    return results;
}

vector<string> URLShortener::getRecentURLs(int count) {
    vector<pair<time_t, string>> timestamps;
    for (auto& pair : shortToLong) {
        timestamps.push_back({pair.second.createdAt, pair.second.shortCode});
    }
    sort(timestamps.rbegin(), timestamps.rend());
    vector<string> result;
    for (int i = 0; i < min(count, (int)timestamps.size()); i++) {
        result.push_back(timestamps[i].second);
    }
    return result;
}

vector<string> URLShortener::getAllShortCodes() {
    vector<string> codes;
    for (auto& pair : shortToLong) {
        codes.push_back(pair.first);
    }
    return codes;
}

bool URLShortener::deleteURL(const string& shortCode) {
    if (!shortToLong.count(shortCode)) return false;
    URLRecord rec = shortToLong[shortCode];
    if (!rec.category.empty() && categoryCount[rec.category] > 0) {
        categoryCount[rec.category]--;
    }
    longToShort.erase(rec.longURL);
    shortToLong.erase(shortCode);
    totalURLs--;
    totalClicks -= rec.clickCount;
    saveToFile();
    return true;
}

bool URLShortener::updateURL(const string& shortCode, const string& newLongURL) {
    if (!shortToLong.count(shortCode)) return false;
    if (!isValidURL(newLongURL)) return false;
    URLRecord& rec = shortToLong[shortCode];
    longToShort.erase(rec.longURL);
    rec.longURL = newLongURL;
    longToShort[newLongURL] = rec;
    saveToFile();
    return true;
}

void URLShortener::printStats() {
    cout << "\n=========== GLOBAL STATS ===========\n";
    cout << "  Total URLs Shortened  : " << totalURLs << "\n";
    cout << "  Total Clicks Tracked : " << totalClicks << "\n";
    cout << "  Categories Active    : " << categoryCount.size() << "\n";
    if (!categoryCount.empty()) {
        cout << "  Category Breakdown   :\n";
        for (auto& pair : categoryCount) {
            if (pair.second > 0)
                cout << "    -> " << pair.first << " : " << pair.second << " URLs\n";
        }
    }
    cout << "  Storage File         : url_data.csv\n";
    cout << "====================================\n";
}

void URLShortener::printAllURLs() {
    if (shortToLong.empty()) {
        cout << "\n[INFO] No URLs stored yet.\n";
        return;
    }
    cout << "\n========== ALL URLs ==========\n";
    int i = 1;
    for (auto& pair : shortToLong) {
        const URLRecord& rec = pair.second;
        string status = isExpired(rec) ? "[EXPIRED]" : "[ACTIVE]";
        string locked = rec.isPasswordProtected ? " [LOCKED]" : "";
        cout << "  " << i++ << ". " << status << locked << "\n";
        cout << "     Code     : " << rec.shortCode << "\n";
        cout << "     URL      : " << rec.longURL << "\n";
        cout << "     Title    : " << (rec.title.empty() ? "N/A" : rec.title) << "\n";
        cout << "     Category : " << (rec.category.empty() ? "None" : rec.category) << "\n";
        cout << "     Clicks   : " << rec.clickCount << "\n";
        cout << "     Created  : " << timeToString(rec.createdAt) << "\n";
        cout << "     Expires  : " << timeToString(rec.expiresAt) << "\n";
        cout << "     -----------------------------------\n";
    }
    cout << "==============================\n";
}

void URLShortener::printQRCode(const string& shortCode) {
    if (!shortToLong.count(shortCode)) {
        cout << "\n[ERROR] URL not found.\n";
        return;
    }
    string url = "https://short.url/" + shortCode;
    cout << "\n========== QR CODE ==========\n";
    cout << "  URL: " << url << "\n\n";
    cout << "  +-------+-------+-------+-------+-------+-------+-------+\n";
    cout << "  | ##### | . . . | # # # | . . . | # # # | . . . | ##### |\n";
    cout << "  | #   # | . # . | #   # | . # . | #   # | . # . | #   # |\n";
    cout << "  | ## ## | . . . | # # # | . . . | # # # | . . . | ## ## |\n";
    cout << "  | #   # | . # # | . . # | # . . | # # . | # . # | #   # |\n";
    cout << "  | ##### | . . . | # # # | . # # | . . # | . . . | ##### |\n";
    cout << "  +-------+-------+-------+-------+-------+-------+-------+\n";
    cout << "  | . . . | # # # | . # . | # # # | . . . | # # # | . . . |\n";
    cout << "  | # # . | . . . | # . # | . # . | # # . | . . . | . # # |\n";
    cout << "  | . # # | # # # | . . . | # . . | . # # | # # # | # . . |\n";
    cout << "  | # . . | . . # | # # . | . # # | # . . | # . . | . # # |\n";
    cout << "  | . # . | # . . | . # # | # . . | . # . | . # . | # . . |\n";
    cout << "  +-------+-------+-------+-------+-------+-------+-------+\n";
    cout << "  | ##### | . # . | # # # | . . . | #   # | . . . | ##### |\n";
    cout << "  | #   # | . . . | #   # | . # . | . # . | # # . | #   # |\n";
    cout << "  | ## ## | . # . | # # # | . . . | #   # | . . . | ## ## |\n";
    cout << "  | #   # | # . # | . . # | # # . | . # # | . # # | #   # |\n";
    cout << "  | ##### | . . . | # # # | . . # | # # . | . . . | ##### |\n";
    cout << "  +-------+-------+-------+-------+-------+-------+-------+\n";
    cout << "\n  [NOTE: This is a text representation. In a real app,\n";
    cout << "   use a QR library like libqrencode for actual QR.]\n";
    cout << "==============================\n";
}

string URLShortener::bulkShorten(const vector<string>& longURLs) {
    ostringstream result;
    int count = 0;
    for (const string& url : longURLs) {
        string code = shortenURL(url);
        if (code.substr(0, 5) != "ERROR") {
            result << code << " -> " << url << "\n";
            count++;
        } else {
            result << "FAILED  -> " << url << " (" << code << ")\n";
        }
    }
    result << "\n[BULK] " << count << "/" << longURLs.size() << " URLs shortened successfully.\n";
    return result.str();
}

int URLShortener::getTotalURLs() { return totalURLs; }
int URLShortener::getTotalClicks() { return totalClicks; }

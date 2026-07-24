#ifndef URLSHORTENER_H
#define URLSHORTENER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <set>

using namespace std;

struct URLRecord {
    string longURL;
    string shortCode;
    string customAlias;
    string title;
    string category;
    string password;
    time_t createdAt;
    time_t expiresAt;
    int clickCount;
    vector<time_t> clickHistory;
    bool isPasswordProtected;
};

class URLShortener {
private:
    unordered_map<string, URLRecord> shortToLong;
    unordered_map<string, URLRecord> longToShort;
    unordered_map<string, int> categoryCount;
    int totalURLs;
    int totalClicks;
    static const string BASE62_CHARS;
    static const int SHORT_CODE_LENGTH;
    static const int MAX_CUSTOM_ALIAS_LENGTH;

    string generateShortCode();
    string base62Encode(long long num);
    bool isValidURL(const string& url);
    bool isValidAlias(const string& alias);
    bool isExpired(const URLRecord& record);
    string timeToString(time_t t);
    string toLowerCase(const string& s);
    void saveToFile();
    void loadFromFile();

public:
    URLShortener();

    string shortenURL(const string& longURL);
    string shortenURL(const string& longURL, const string& customAlias);
    string shortenURL(const string& longURL, const string& customAlias,
                      const string& category, int expiryDays);
    string decodeURL(const string& shortCode);
    bool redirect(const string& shortCode);
    bool redirect(const string& shortCode, const string& password);

    bool setExpiration(const string& shortCode, int days);
    bool removeExpiration(const string& shortCode);
    bool isExpiredURL(const string& shortCode);

    bool setCategory(const string& shortCode, const string& category);
    string getCategory(const string& shortCode);
    vector<string> getByCategory(const string& category);

    bool setTitle(const string& shortCode, const string& title);
    string getTitle(const string& shortCode);

    bool setPassword(const string& shortCode, const string& password);
    bool verifyPassword(const string& shortCode, const string& password);

    int getClickCount(const string& shortCode);
    vector<time_t> getClickHistory(const string& shortCode);
    void printAnalytics(const string& shortCode);

    vector<string> searchURLs(const string& keyword);
    vector<string> getRecentURLs(int count);
    vector<string> getAllShortCodes();

    bool deleteURL(const string& shortCode);
    bool updateURL(const string& shortCode, const string& newLongURL);

    void printStats();
    void printAllURLs();
    void printQRCode(const string& shortCode);

    string bulkShorten(const vector<string>& longURLs);
    int getTotalURLs();
    int getTotalClicks();
};

#endif

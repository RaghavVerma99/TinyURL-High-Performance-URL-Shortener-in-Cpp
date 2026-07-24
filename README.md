# URL Shortener & Manager v2.0

A full-featured URL shortening service built in C++ with analytics, management, and persistence capabilities.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [File Structure](#file-structure)
- [Setup & Compilation](#setup--compilation)
- [Usage Guide](#usage-guide)
  - [Menu Walkthrough](#menu-walkthrough)
  - [Feature Deep Dive](#feature-deep-dive)
- [Technical Design](#technical-design)
  - [Data Structures](#data-structures)
  - [Encoding Algorithm](#encoding-algorithm)
  - [URL Validation](#url-validation)
  - [Persistence Model](#persistence-model)
- [API Reference](#api-reference)
  - [URLShortener Class](#urlshortener-class)
  - [URLRecord Struct](#urlrecord-struct)
- [Code Examples](#code-examples)
- [Sample Session](#sample-session)
- [Build & Run](#build--run)
- [Limitations & Future Work](#limitations--future-work)

---

## Overview

URL Shortener is a console-based application that converts long URLs into short, manageable codes. It goes beyond basic shortening by offering click analytics, URL categorization, password protection, expiration dates, bulk operations, and persistent data storage.

### Key Highlights

- **21 operations** available through an interactive menu
- **Base62 random code generation** for short URLs
- **Custom aliases** for personalized short links
- **Click tracking** with timestamp history
- **URL expiration** with configurable TTL
- **Password protection** for private links
- **Category management** for organizing URLs
- **Bulk shortening** for multiple URLs at once
- **CSV persistence** - data survives restarts
- **Search functionality** across all stored URLs
- **Text-based QR code** visualization

---

## Features

### Core Operations
| Feature | Description |
|---------|-------------|
| Shorten URL | Generate a 6-character random short code |
| Custom Alias | Choose your own short code (alphanumeric, -, _) |
| Full Options Shorten | Set alias, category, and expiry in one step |
| Bulk Shorten | Shorten multiple URLs in one session |
| Decode URL | Retrieve the original URL from a short code |
| Redirect | Simulate a click and get the destination URL |

### Management
| Feature | Description |
|---------|-------------|
| Update URL | Change the destination of an existing short URL |
| Delete URL | Remove a URL from the system |
| Set Title | Add a human-readable title to any URL |
| Set Category | Organize URLs by topic (work, personal, etc.) |
| Set Expiration | Make a URL auto-expire after N days |
| Remove Expiration | Make an expired URL active again |
| Password Protection | Require a password to access the URL |

### View & Search
| Feature | Description |
|---------|-------------|
| View All URLs | Display all stored URLs with status |
| View Analytics | Detailed stats for a specific URL |
| Search URLs | Find URLs by keyword (matches URL, title, category) |
| View Recent | See the N most recently created URLs |
| By Category | List all URLs in a specific category |
| QR Code | Text-based QR code visualization |

### System
| Feature | Description |
|---------|-------------|
| Global Statistics | Total URLs, clicks, and category breakdown |
| Data Export | View all data in a formatted table |
| Auto-save | Data persists to `url_data.csv` after every change |

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│                    main.cpp                      │
│  ┌───────────────────────────────────────────┐  │
│  │         Interactive Menu System           │  │
│  │    (User input, display, navigation)      │  │
│  └──────────────────┬────────────────────────┘  │
│                     │                           │
│                     ▼                           │
│  ┌───────────────────────────────────────────┐  │
│  │            URLShortener Class             │  │
│  │                                           │  │
│  │  ┌─────────┐  ┌──────────┐  ┌────────┐  │  │
│  │  │ shorten │  │ redirect │  │ manage │  │  │
│  │  │  decode │  │ analytics│  │ search │  │  │
│  │  └─────────┘  └──────────┘  └────────┘  │  │
│  │                                           │  │
│  │  ┌─────────────────────────────────────┐  │  │
│  │  │         Data Layer                  │  │  │
│  │  │  shortToLong  |  longToShort        │  │  │
│  │  │  categoryCount|  CSV I/O            │  │  │
│  │  └─────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────┘  │
│                     │                           │
│                     ▼                           │
│  ┌───────────────────────────────────────────┐  │
│  │            url_data.csv                   │  │
│  │         (Persistent Storage)              │  │
│  └───────────────────────────────────────────┘  │
└─────────────────────────────────────────────────┘
```

---

## File Structure

```
URL_Shortener/
├── main.cpp              # Entry point, interactive menu system
├── URLShortener.h        # Class declaration, data structures
├── URLShortener.cpp      # Full implementation of all features
├── README.md             # This documentation
├── url_data.csv          # Auto-generated persistent storage
└── .git/                 # Git repository
```

---

## Setup & Compilation

### Prerequisites
- C++ compiler (GCC, Clang, or MSVC)
- C++11 or later standard
- No external libraries required (pure C++)

### Compilation

**Using g++ (Linux/macOS/WSL):**
```bash
g++ -std=c++17 -o url_shortener main.cpp URLShortener.cpp
```

**Using g++ (Windows/MinGW):**
```bash
g++ -std=c++17 -o url_shortener.exe main.cpp URLShortener.cpp
```

**Using MSVC (Windows):**
```bash
cl /EHsc /std:c++17 main.cpp URLShortener.cpp
```

**Using Make (if available):**
```bash
make
```

### Running

```bash
./url_shortener          # Linux/macOS
./url_shortener.exe      # Windows
```

---

## Usage Guide

### Menu Walkthrough

When you run the application, you'll see:

```
  ╔══════════════════════════════════════════════════════╗
  ║           URL SHORTENER & MANAGER v2.0              ║
  ║     Shorten  |  Track  |  Manage  |  Analyze        ║
  ╚══════════════════════════════════════════════════════╝
```

The menu presents 21 options organized into categories:

**URL Operations (1-6):** Create and access short URLs
**URL Management (7-13):** Modify, delete, and protect URLs
**View & Search (14-19):** Browse, search, and analyze URLs
**Stats & System (20-21):** View global stats and export data

### Feature Deep Dive

#### 1. Shorten a URL
```
Choice: 1
Enter the long URL: https://www.example.com/very/long/path/to/resource

  ==========================================
  SHORTENED SUCCESSFULLY!
  Original  : https://www.example.com/very/long/path/to/resource
  Short URL : https://short.url/aB3xK9
  Code      : aB3xK9
  ==========================================
```

#### 2. Custom Alias
```
Choice: 2
Enter the long URL: https://github.com/myproject
Enter custom alias: myproject

  ==========================================
  SHORTENED SUCCESSFULLY!
  Original  : https://github.com/myproject
  Short URL : https://short.url/myproject
  Code      : myproject
  ==========================================
```

#### 3. Full Options
```
Choice: 3
Enter the long URL: https://docs.python.org
Enter custom alias: pydocs
Enter category: programming
Enter expiry in days: 30

  ==========================================
  SHORTENED SUCCESSFULLY!
  Original  : https://docs.python.org
  Short URL : https://short.url/pydocs
  Code      : pydocs
  ==========================================
```

#### 4. Bulk Shorten
```
Choice: 4
Enter URLs (one per line, empty line to finish):
  > https://google.com
  > https://github.com
  > https://stackoverflow.com
  >

aB3xK9 -> https://google.com
xY7mN2 -> https://github.com
pQ4wR8 -> https://stackoverflow.com

[BULK] 3/3 URLs shortened successfully.
```

#### 6. Redirect (Simulated)
```
Choice: 6
Enter short code: aB3xK9

[REDIRECT] https://www.example.com/very/long/path/to/resource
```

#### 13. Password Protection
```
Choice: 13
Enter short code: secret
Enter password: mySecretPass123

  [SUCCESS] Password protection enabled.
```

When someone tries to redirect:
```
Choice: 6
Enter short code: secret
This URL is password protected.
Enter password: wrongpass

[ACCESS DENIED] Invalid password.
```

#### 15. Analytics
```
Choice: 15
Enter short code for analytics: aB3xK9

========== ANALYTICS ==========
  Short Code    : aB3xK9
  Long URL      : https://www.example.com/very/long/path/to/resource
  Title         : My Website
  Category      : work
  Created       : 2026-07-24 14:30:22
  Expires       : 2026-08-23 14:30:22
  Total Clicks  : 12
  Password      : No
  Last 5 Clicks :
    -> 2026-07-24 15:01:12
    -> 2026-07-24 16:22:45
    -> 2026-07-24 18:05:33
    -> 2026-07-24 19:12:08
    -> 2026-07-24 20:44:56
================================
```

#### 16. Search
```
Choice: 16
Enter search keyword: github

  Found 2 result(s):
  1. myproject -> https://github.com/myproject
  2. xY7mN2 -> https://github.com/trending
```

#### 19. QR Code
```
Choice: 19
Enter short code: aB3xK9

========== QR CODE ==========
  URL: https://short.url/aB3xK9

  +-------+-------+-------+-------+-------+-------+-------+
  | ##### | . . . | # # # | . . . | # # # | . . . | ##### |
  | #   # | . # . | #   # | . # . | #   # | . # . | #   # |
  | ## ## | . . . | # # # | . . . | # # # | . . . | ## ## |
  | #   # | . # # | . . # | # . . | # # . | . # . | #   # |
  | ##### | . . . | # # # | . # # | . . # | . . . | ##### |
  +-------+-------+-------+-------+-------+-------+-------+
  ...
==============================
```

#### 20. Global Statistics
```
Choice: 20

=========== GLOBAL STATS ===========
  Total URLs Shortened  : 15
  Total Clicks Tracked : 87
  Categories Active    : 3
  Category Breakdown   :
    -> work : 6 URLs
    -> personal : 5 URLs
    -> programming : 4 URLs
  Storage File         : url_data.csv
====================================
```

---

## Technical Design

### Data Structures

#### URLRecord (Struct)
```cpp
struct URLRecord {
    string longURL;          // Original destination URL
    string shortCode;        // Unique short identifier
    string customAlias;      // User-defined alias (if any)
    string title;            // Human-readable title
    string category;         // Category tag
    string password;         // Password for protected URLs
    time_t createdAt;        // Creation timestamp
    time_t expiresAt;        // Expiration timestamp (0 = never)
    int clickCount;          // Total redirect count
    vector<time_t> clickHistory;  // Timestamp of each click
    bool isPasswordProtected;     // Password protection flag
};
```

#### URLShortener (Class)
```cpp
class URLShortener {
private:
    unordered_map<string, URLRecord> shortToLong;  // shortCode -> record
    unordered_map<string, URLRecord> longToShort;  // longURL -> record
    unordered_map<string, int> categoryCount;       // category -> count
    int totalURLs;
    int totalClicks;
    // ... helper methods
public:
    // 21+ public methods for all operations
};
```

### Encoding Algorithm

The system uses **Base62 random generation** for short codes:

```
Character set: a-z, A-Z, 0-9 (62 characters)
Code length: 6 characters
Total combinations: 62^6 = ~56.8 billion unique codes
```

**Why Base62?**
- Only alphanumeric characters (no special chars)
- URL-safe without encoding
- Case-sensitive for maximum entropy
- Easy to read and type

**Generation process:**
1. Seed a Mersenne Twister RNG with `steady_clock` timestamp
2. Generate 6 random indices into the Base62 character set
3. Concatenate characters at those indices
4. Check for collision with existing codes
5. Regenerate if collision occurs

### URL Validation

The validator checks:
1. Non-empty string
2. Starts with `http://` or `https://` (or bare domain with `.`)
3. Contains no spaces
4. Contains at least one `.` (dot)

### Persistence Model

Data is stored in CSV format (`url_data.csv`):

```
shortCode,longURL,customAlias,title,category,password,createdAt,expiresAt,clickCount
aB3xK9,https://example.com,,My Site,work,,1721834422,1724426422,12
```

**Save triggers:** Every create, update, delete, click, or metadata change
**Load trigger:** On application startup

---

## API Reference

### URLShortener Class

#### Constructor
```cpp
URLShortener();
```
Creates a new instance and loads existing data from `url_data.csv`.

#### Core Operations
```cpp
string shortenURL(const string& longURL);
// Shortens a URL with a random code
// Returns: short code, or "ERROR: ..." on failure

string shortenURL(const string& longURL, const string& customAlias);
// Shortens with a custom alias
// Returns: custom alias, or "ERROR: ..." on failure

string shortenURL(const string& longURL, const string& customAlias,
                  const string& category, int expiryDays);
// Full options shortening
// Returns: custom alias, or "ERROR: ..." on failure

string decodeURL(const string& shortCode);
// Retrieves the original URL
// Returns: long URL, or "ERROR: ..." on failure

bool redirect(const string& shortCode);
// Simulates a click and prints the destination
// Returns: true if successful

bool redirect(const string& shortCode, const string& password);
// Password-protected redirect
// Returns: true if successful and password matches
```

#### Management
```cpp
bool updateURL(const string& shortCode, const string& newLongURL);
bool deleteURL(const string& shortCode);
bool setTitle(const string& shortCode, const string& title);
string getTitle(const string& shortCode);
bool setCategory(const string& shortCode, const string& category);
string getCategory(const string& shortCode);
bool setExpiration(const string& shortCode, int days);
bool removeExpiration(const string& shortCode);
bool isExpiredURL(const string& shortCode);
bool setPassword(const string& shortCode, const string& password);
bool verifyPassword(const string& shortCode, const string& password);
```

#### Analytics
```cpp
int getClickCount(const string& shortCode);
vector<time_t> getClickHistory(const string& shortCode);
void printAnalytics(const string& shortCode);
void printStats();
```

#### Search & Browse
```cpp
vector<string> searchURLs(const string& keyword);
vector<string> getRecentURLs(int count);
vector<string> getByCategory(const string& category);
vector<string> getAllShortCodes();
```

#### Utilities
```cpp
string bulkShorten(const vector<string>& longURLs);
void printAllURLs();
void printQRCode(const string& shortCode);
int getTotalURLs();
int getTotalClicks();
```

### URLRecord Struct

| Field | Type | Description |
|-------|------|-------------|
| `longURL` | string | Original destination URL |
| `shortCode` | string | Unique 6-char identifier or custom alias |
| `customAlias` | string | User-defined alias (empty if auto-generated) |
| `title` | string | Human-readable title |
| `category` | string | Category tag (e.g., "work", "personal") |
| `password` | string | Password (empty if not protected) |
| `createdAt` | time_t | Unix timestamp of creation |
| `expiresAt` | time_t | Unix timestamp of expiration (0 = never) |
| `clickCount` | int | Total number of clicks/redirects |
| `clickHistory` | vector\<time_t\> | Timestamp of each click |
| `isPasswordProtected` | bool | Whether password is required |

---

## Code Examples

### Example 1: Basic Shortening
```cpp
#include "URLShortener.h"

int main() {
    URLShortener shortener;
    string code = shortener.shortenURL("https://www.google.com");
    cout << "Short code: " << code << "\n";  // e.g., "aB3xK9"
    return 0;
}
```

### Example 2: Custom Alias with Category
```cpp
#include "URLShortener.h"

int main() {
    URLShortener shortener;
    string code = shortener.shortenURL(
        "https://github.com/user/repo",
        "myrepo",
        "development",
        90  // expires in 90 days
    );
    cout << "Code: " << code << "\n";  // "myrepo"
    return 0;
}
```

### Example 3: Password-Protected Redirect
```cpp
#include "URLShortener.h"

int main() {
    URLShortener shortener;
    string code = shortener.shortenURL("https://secret-site.com");
    shortener.setPassword(code, "mypass123");

    // This fails (wrong password)
    shortener.redirect(code, "wrong");

    // This succeeds
    shortener.redirect(code, "mypass123");
    return 0;
}
```

### Example 4: Analytics
```cpp
#include "URLShortener.h"

int main() {
    URLShortener shortener;
    string code = shortener.shortenURL("https://example.com");
    shortener.setTitle(code, "My Example");
    shortener.setCategory(code, "reference");

    // Simulate some clicks
    shortener.redirect(code);
    shortener.redirect(code);
    shortener.redirect(code);

    shortener.printAnalytics(code);
    cout << "Clicks: " << shortener.getClickCount(code) << "\n";  // 3
    return 0;
}
```

### Example 5: Bulk Operations
```cpp
#include "URLShortener.h"

int main() {
    URLShortener shortener;
    vector<string> urls = {
        "https://google.com",
        "https://github.com",
        "https://stackoverflow.com"
    };
    string result = shortener.bulkShorten(urls);
    cout << result;
    return 0;
}
```

---

## Sample Session

```
  ╔══════════════════════════════════════════════════════╗
  ║           URL SHORTENER & MANAGER v2.0              ║
  ║     Shorten  |  Track  |  Manage  |  Analyze        ║
  ╚══════════════════════════════════════════════════════╝

  ┌─────────────── MAIN MENU ───────────────┐
  │  1.  Shorten a URL                       │
  │  2.  Shorten with custom alias           │
  │  ...                                     │
  │  0.  Exit                                │
  └──────────────────────────────────────────┘
  Enter choice: 1

  Enter the long URL: https://www.youtube.com/watch?v=dQw4w9WgXcQ

  ==========================================
  SHORTENED SUCCESSFULLY!
  Original  : https://www.youtube.com/watch?v=dQw4w9WgXcQ
  Short URL : https://short.url/xK9mPq
  Code      : xK9mPq
  ==========================================

  Enter choice: 9

  Enter short code: xK9mPq
  Enter title: Rick Astley - Never Gonna Give You Up

  [SUCCESS] Title set.

  Enter choice: 10

  Enter short code: xK9mPq
  Enter category: music

  [SUCCESS] Category set.

  Enter choice: 6

  Enter short code: xK9mPq

  [REDIRECT] https://www.youtube.com/watch?v=dQw4w9WgXcQ

  Enter choice: 15

  Enter short code for analytics: xK9mPq

  ========== ANALYTICS ==========
  Short Code    : xK9mPq
  Long URL      : https://www.youtube.com/watch?v=dQw4w9WgXcQ
  Title         : Rick Astley - Never Gonna Give You Up
  Category      : music
  Created       : 2026-07-24 21:15:33
  Expires       : Never
  Total Clicks  : 1
  Password      : No
  =================================

  Enter choice: 0

  ╔══════════════════════════════════════════╗
  ║   Thanks for using URL Shortener v2.0!  ║
  ║   Your data has been saved. Goodbye!    ║
  ╚══════════════════════════════════════════╝
```

---

## Build & Run

### Quick Start

```bash
# Clone the repository
git clone <repo-url>
cd URL_Shortener

# Compile
g++ -std=c++17 -o url_shortener main.cpp URLShortener.cpp

# Run
./url_shortener
```

### Windows (MinGW)
```bash
g++ -std=c++17 -o url_shortener.exe main.cpp URLShortener.cpp
url_shortener.exe
```

### Using Visual Studio
1. Create a new C++ Console Project
2. Add `main.cpp`, `URLShortener.h`, `URLShortener.cpp`
3. Set C++ Language Standard to C++17
4. Build and Run (F5)

---

## Limitations & Future Work

### Current Limitations
- In-memory maps for lookups (not database-backed)
- Text-based QR code (not scannable)
- Single-user, single-process access
- CSV persistence (not optimized for large datasets)
- No HTTP server (console-only)
- No actual URL redirection over network

### Possible Enhancements
- [ ] HTTP server with REST API (using cpp-httplib)
- [ ] SQLite database for persistent storage
- [ ] Scannable QR codes (using libqrencode)
- [ ] Multi-user support with authentication
- [ ] Rate limiting and abuse prevention
- [ ] URL previews and metadata fetching
- [ ] Click analytics with geographic data
- [ ] Browser extension integration
- [ ] Docker containerization
- [ ] Unit tests with Google Test

---

## Technologies

- **Language:** C++17
- **Data Structures:** Hash maps (unordered_map), Vectors
- **Time Handling:** ctime, chrono
- **File I/O:** fstream (CSV format)
- **Random:** Mersenne Twister (mt19937)
- **No external dependencies**

---

## Author

Built as a learning project demonstrating data structures, file I/O, OOP, and systems design in C++.

---

## License

This project is open source and available for educational purposes.

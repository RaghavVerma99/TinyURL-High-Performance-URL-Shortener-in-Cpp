#ifndef URLSHORTENER_H
#define URLSHORTENER_H

#include <string>
#include <unordered_map>

using namespace std;

class URLShortener
{
private:
    unordered_map<string, string> database;

public:
    string encode(string longURL);
    string decode(string shortURL);
};

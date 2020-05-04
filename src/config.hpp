#ifndef CONFIG_H
#define CONFIG_H

#include <string>

int maxHttpLen = 600;
int maxFileSize = 600;
int maxCacheEntry = 5;
struct timeval keepAliveTimeout({5, 0});
std::string PAGE404 = "404.html";

char STATUS404[] = "404 Not Found";
char STATUS500[] = "500 Internal Server Error";
char STATUS200[] = "200 OK";

#endif
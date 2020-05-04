#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string.h>
#include "config.hpp"

struct httpResponse {
	char* content;
	char* status;
};

struct httpHeader {
	std::string path;
	bool keepAlive;
};

inline void parseHeader(char *request, httpHeader &header) {
	header.path = "";
	int len = strlen(request);
	bool check=false;
	for(int i=0;i<len;i++) {
		if(request[i]==' ' || check) {
			if(check==false){
				check=true;
				i++;
			}
			else if(request[i]==' '){
				break;
			}
			else {
				header.path+=request[i];
			}
		}
	}

	header.keepAlive = (strstr(request, "keep-alive") != NULL) ||
						(strstr(request, "HTTP/1.1") != NULL);
}

#endif

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string.h>


inline void getPath(char *request, std::string &path,bool &keepAlive) {
	path = "";
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
				path+=request[i];
			}
		}
	}

	keepAlive = (strstr(request, "keep-alive") != NULL) ||
						(strstr(request, "HTTP/1.1") != NULL);
}

#endif

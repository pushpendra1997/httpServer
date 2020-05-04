#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string.h>


inline void getPath(char *request, std::string &path) {
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

}

#endif

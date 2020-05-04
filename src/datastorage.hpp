#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <sys/stat.h>
#include <shared_mutex>
#include <map>

#include <fstream>
#include "config.hpp"
#include "utils.hpp"


class datastorage {

private:
	// Maps path to file name
	std::map<std::string, std::string> pathFile;

	// Mutex to prevent simultaneous cache update
 	std::shared_mutex mutex_;
	// LRU Cache to reduce File I/O
	int time = 0;
	std::map<std::string, char*> cache;

	std::map<int, std::string> timeCache;

public:
	datastorage(){};
	~datastorage() {
		// Clear the cache
		for(auto x: cache) {
			delete[] x.second;
		}
	};

	inline bool fileExist (const std::string& name) {
	  struct stat buffer;   
	  return (stat (name.c_str(), &buffer) == 0); 
	}

	// Map path to fileName

	void addPage(std::string path, std::string fileName) {
		std::unique_lock<std::shared_mutex> lock(mutex_);

		// If file exists add mapping using Linux syscall
		if(fileExist("htmlpage/" + fileName)) {
			pathFile[path] = fileName;
		}

	}


	// Return pair - Content, Status Code


	inline bool getFromCache(std::string &fileName, char* &content) {
		std::shared_lock<std::shared_mutex> lock(mutex_);

		bool isInCache = true;

		if(cache.count(fileName)){
			content = cache[fileName];
		} else {
			isInCache = false;
		}
		return isInCache;
	}

	

	void setInCacheAndGet(std::string &fileName, char* &content, char* &status) {

		std::unique_lock<std::shared_mutex> lock(mutex_);


		if(cache.size()==maxCacheEntry) {
			std::string pathRemove = timeCache.begin()->second;
			timeCache.erase(timeCache.begin());
			delete[] cache[pathRemove];
			cache.erase(pathRemove);
		}

		// Unable to read file
		std::string filelocation = "htmlpage/" + fileName;
		if( !fileExist(filelocation) ) {
			status = STATUS500;
			content = NULL;

		} else {

			std::ifstream in(filelocation);
			std::string contents((std::istreambuf_iterator<char>(in)), 
	    	std::istreambuf_iterator<char>());

			// Update Cache

			timeCache[time++] = fileName;
			char* conts = strdup(contents.c_str());
			cache[fileName] = conts;
			content = conts;

		}

	}


	void getWebPage(std::string &path, char* &content, char* &status) {

		std::string fileName = (pathFile.count(path)) ? pathFile[path] : PAGE404;
		status = (pathFile.count(path)) ? STATUS200 : STATUS404;

		// Read from the cache

		bool isRead = getFromCache(fileName, content);


		// If file not in cache
		if ( !isRead ){
			setInCacheAndGet(fileName, content, status);
		}

	}
};

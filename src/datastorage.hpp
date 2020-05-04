#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <sys/stat.h>
#include <unordered_map>
#include <fstream>
#include "config.hpp"
#include "utils.hpp"
#include "semaphore.hpp"

class datastorage {

private:
	// Maps path to file name
	std::unordered_map<std::string, std::string> pathFile;

	// Mutex to prevent simultaneous cache update
	std::mutex mtx;

	// LRU Cache to reduce File I/O
	int time = 0;
	std::unordered_map<std::string, char*> cache;

	std::unordered_map<int, std::string> timeCache;

	int readCount=0;

    Semaphore resourceAccess;

    Semaphore readCountAccess;

    Semaphore serviceQueue;

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
		mtx.lock();

		// If file exists add mapping using Linux syscall
		if(fileExist("htmlpage/" + fileName)) {
			pathFile[path] = fileName;
		}

		mtx.unlock();
	}

	void readEnter(){

        serviceQueue.wait();
        readCountAccess.wait();

        if(readCount == 0)
            resourceAccess.wait();

        readCount++;
        serviceQueue.signal();
        readCountAccess.signal();

    }

    void readExit(){
        readCountAccess.wait();    
        readCount--;               
        if (readCount == 0)        
            resourceAccess.signal();
        readCountAccess.signal(); 
    }

    void writeEnter(){

        serviceQueue.wait();   
        resourceAccess.wait(); 
        serviceQueue.signal();

    }


    void writeExit(){

        resourceAccess.signal();

    }


	// Return pair - Content, Status Code


	bool getFromCache(std::string &fileName, httpResponse &response) {

		bool isInCache = true;

		readEnter();

		if(cache.count(fileName)){
			response.content = cache[fileName];
		} else {
			isInCache = false;
		}

		readExit();

		return isInCache;
	}

	

	void setInCacheAndGet(std::string &fileName, httpResponse &response) {

		writeEnter();

		if(cache.size()==maxCacheEntry) {
			std::string pathRemove = timeCache.begin()->second;
			timeCache.erase(timeCache.begin());
			delete[] cache[pathRemove];
			cache.erase(pathRemove);
		}

		// Unable to read file
		string filelocation = "htmlpage/" + fileName;
		if( !fileExist(filelocation) ) {
			response.status = STATUS500;
			response.content = NULL;

		} else {

			std::ifstream in(filelocation);
			std::string contents((std::istreambuf_iterator<char>(in)), 
	    	std::istreambuf_iterator<char>());

			// Update Cache

			timeCache[time++] = fileName;
			char* content = strdup(contents.c_str());
			cache[fileName] = content;
			response.content = content;

		}
			
		writeExit();

	}


	void getWebPage(std::string &path, httpResponse &response) {

		std::string fileName = (pathFile.count(path)) ? pathFile[path] : PAGE404;
		response.status = (pathFile.count(path)) ? STATUS200 : STATUS404;

		// Read from the cache

		bool isRead = getFromCache(fileName, response);


		// If file not in cache
		if ( !isRead ){
			setInCacheAndGet(fileName, response);
		}

	}
};

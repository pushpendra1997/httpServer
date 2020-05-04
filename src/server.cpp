#include <iostream>
#include <functional>
#include <algorithm>
#include <map>
#include <sstream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <queue>

#include <unistd.h>
using namespace std;


#include "config.hpp"



#include "datastorage.hpp"
#include "utils.hpp"
#include "config.hpp"
using namespace std;

datastorage files;

#include "clienthandle.hpp"


std::mutex QueueLock;
std::queue <int> event_queue;

class server {

private:
	struct sockaddr_in address;

	int serverSocket;

	int sizeof_address;

	int Port ;

	int totalThread;

public:
	
	void run(int port = 8080,int NumberofThread = 20) {

		totalThread = NumberofThread;

		Port = port;

		int opt = 1;

		sizeof_address = sizeof(address);

		serverSocket = socket(AF_INET, SOCK_STREAM, 0);

		 if (serverSocket == -1) { 

	        cout<<"socket creation failed...\n"; 

	        exit(0); 

    	}

		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

			cout<<"setsockopt\n";

			exit(0);

		}

		address.sin_family = AF_INET;

		address.sin_addr.s_addr = INADDR_ANY;

		address.sin_port = htons(port);

		if(bind(serverSocket, (struct sockaddr *)&address, sizeof_address) < 0){

			cout<<"bind failed\n";

			exit(0);


		}
		if (listen(serverSocket, 20000) < 0){

			cout<<"listen failed\n";

			exit(0);

		}

	}

	~server() {

		close(serverSocket);

	};

	// Map path to fileName
	void addPage(std::string path, std::string fileName) {

		files.addPage(path, fileName);

	}

	
	void acceptConnections() {

		std::vector<std::thread> threads;

		std::cout << "connecting to localhost:"<<Port<<" \n";

		for(int i=0;i<totalThread;i++) {
			threads.push_back(std::thread(connectionPool));
		}

		while(1) {
            int socket_num=accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&sizeof_address);;
            QueueLock.lock();
            event_queue.push(socket_num);
            QueueLock.unlock();
        }

		std::for_each(threads.begin(),threads.end(),std::mem_fn(&std::thread::join));

	}

	// Waits for msg from the client
	
	static void connectionPool() {

        while(true){

            int clientSocket;
            QueueLock.lock();;
            if(event_queue.empty()==false) {
                clientSocket = event_queue.front();
                event_queue.pop();
                QueueLock.unlock();;
            }
            else {
                QueueLock.unlock();;
                continue;                
            }
            
            clientHandle client(clientSocket);
			client.makeRequest();
            close(clientSocket);
        }
    }

};



int main(int argc, char const *argv[]) {

	server server;

	server.run(8080,20);

	server.addPage("home", "home.html");

	server.addPage("", "home.html");

	server.acceptConnections();

}

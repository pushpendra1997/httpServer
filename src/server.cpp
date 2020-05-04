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


#include "config.hpp"



#include "datastorage.hpp"
#include "utils.hpp"
#include "config.hpp"

datastorage fileMemory;

//Thread Queue Lock

pthread_mutex_t QueueLock;

std::queue <int> event_queue;

class server {

private:
	struct sockaddr_in address;

	int serverSocket;

	int sizeof_address;

	int Port ;

	int totalThread;

public:
	///server starting  
	void start(int port = 8080,int NumberofThread = 20) {

		totalThread = NumberofThread;

		Port = port;

		int opt = 1;

		sizeof_address = sizeof(address);

		// New socket for listen

		serverSocket = socket(AF_INET, SOCK_STREAM, 0);

		 if (serverSocket == -1) { 

	        std::cout<<"socket creation failed...\n"; 

	        exit(0); 

    	}

		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

			std::cout<<"setsockopt\n";

			exit(0);

		}

		address.sin_family = AF_INET;

		address.sin_addr.s_addr = INADDR_ANY;

		address.sin_port = htons(port);

		// Bind address to socket

		if(bind(serverSocket, (struct sockaddr *)&address, sizeof_address) < 0){

			std::cout<<"bind failed\n";

			exit(0);


		}
		if (listen(serverSocket, 20000) < 0){

			std::cout<<"listen failed\n";

			exit(0);

		}

	}

	~server() {

		close(serverSocket);

	};

	// Map path to fileName
	void addPage(std::string path, std::string fileName) {

		fileMemory.addPage(path, fileName);

	}

	
	void acceptConnections() {

		pthread_t ptid[totalThread];
        for (int i = 0; i < totalThread; i++)
        {
            int return_value=pthread_create(&ptid[i], NULL, connection_thread, (void*) NULL); 
            if(return_value<0)
            {
                perror("ERROR: Could'nt create thread");
                exit(1);
            }
        }
        while(1) {
            int socket_num=accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&sizeof_address);
            pthread_mutex_lock(&QueueLock);
            event_queue.push(socket_num);
            pthread_mutex_unlock(&QueueLock);
        }

	}

	// Waits for msg from the client
	
	static void* connection_thread(void *argv) {

        while(true){

            int clientSocket;
            pthread_mutex_lock(&QueueLock);
            if(event_queue.empty()==false) {
                clientSocket = event_queue.front();
                event_queue.pop();
                pthread_mutex_unlock(&QueueLock);;
            }
            else {
                pthread_mutex_unlock(&QueueLock);;
                continue;                
            }
            
            makeRequest(clientSocket);
            close(clientSocket);
        }
    }

    static void makeRequest(int client) {

		// Set timeout for reads
		
		
		char request[maxHttpLen];

	
		int sz;

		while(sz = read(client, request , maxHttpLen)>0) {

			//Keep recving reqs until: client close the connection(read()==0) or read error occours(read()==-1) or client timeout(read()==-1) 

			request[sz] = '\0';
			// Timeout or Connection Closed
			if(sz<=0) {
				
				return ;
			}
			bool keepAlive;

			std::string path;

			getPath(request, path, keepAlive);

			char responseStr[maxHttpLen];

		
			char* content;
			char* status;
			fileMemory.getWebPage(path, content, status);

			sprintf(responseStr, "HTTP/1.1 %s\r\n Connection: Keep-Alive\r\n Content-Type: text/html\r\nKeep-Alive: timeout=%d\r\nContent-Length: %d\r\n\r\n%s",
							status,(int)keepAliveTimeout .tv_sec, ((content==NULL)? 0 : (int)strlen(content)), ((content==NULL)? "" : content));

			write(client, responseStr, strlen(responseStr));

			// No Keep Alive Header
			if(!keepAlive) {
				return ;
			}
		}
	}


};



int main(int argc, char const *argv[]) {

	server server;

	server.start(8080, 20);

	server.addPage("test", "test.html");

	server.addPage("", "test.html");

	server.acceptConnections();

}

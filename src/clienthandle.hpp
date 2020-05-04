class clientHandle
{
private:

	int clientSocket;

public:

	clientHandle(){}
	clientHandle(int client){
		clientSocket = client ;
		
	}
	~clientHandle(){}


	void makeRequest() {

		bool IsClientError=false; //Client timeout flag
		bool IsClientClose=false;
		// Set timeout for reads
		setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
					(struct timeval *)&keepAliveTimeout,
					sizeof(struct timeval));
		
		
		char request[maxHttpLen];

		httpHeader header;

		int sz;

		while(sz = read(clientSocket , request , maxHttpLen)>0) {

			//Keep recving reqs until: client close the connection(read()==0) or read error occours(read()==-1) or client timeout(read()==-1) 

			request[sz] = '\0';
			// Timeout or Connection Closed
			if(sz<=0) {
				
				return ;
			}
			
			parseHeader(request, header);

			__sendResponse(header.path);

			// No Keep Alive Header
			if(!header.keepAlive) {
				return ;
			}
		}
	}

	// Sends content for requested path
	void __sendResponse(std::string &path) {

		char responseStr[maxHttpLen];

		httpResponse response;


		files.getWebPage(path, response);

		addHttpHeader(responseStr, response);

		write(clientSocket, responseStr, strlen(responseStr));

	}

	void addHttpHeader(char *responseStr, httpResponse &response){

		sprintf(responseStr, "HTTP/1.1 %s\r\n"
							"Connection: Keep-Alive\r\n"
							"Content-Type: text/html\r\n"
							"Keep-Alive: timeout=%d\r\n"
							"Content-Length: %d\r\n\r\n%s",
							response.status,
							(int)keepAliveTimeout .tv_sec,
							((response.content==NULL)? 0 : (int)strlen(response.content)),
							((response.content==NULL)? "" : response.content));

	}

	string build_400_badreq_headers(){
		string response;
		response +="HTTP/1.1 400 Client Error\r\n";
		response +="Server: Myserver 1.0\r\n";
		response +="\r\n";

		return response;
	}
	string build_404_notfound_headers(){
		string response;
		response +="HTTP/1.1 404 Not Found\r\n";
		response +="Server: Myserver 1.0\r\n";
		response +="\r\n";

		return response;
	}
	
};
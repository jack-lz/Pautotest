#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#include <thread>
#include "sockcomm.h"

class Transmitter {
public:
	class Listener {
	public:
		virtual ~Listener() {}
		virtual void onReceived(const char* buf, int len) = 0;
	};

	Transmitter (): _sockfd(-1) {
	}

	~Transmitter() {
		disconnect();
	}

	int start(const char* ip, Listener* listener) {
    	int sockfd = 0;
    	struct sockaddr_in serv_addr; 
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Error: Could not create socket \n");
			return -1;
		} 

		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(SOCK_PORT); 

		if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) {
			printf("inet_pton error occured\n");
			return -1;
		} 

		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		   printf("Error: Connect failed \n");
		   return -1;
		} 

		printf("connect to %s:%d ok\n", ip, SOCK_PORT);

		_sockfd = sockfd;
		_listener = listener;
		_thread = std::thread(&Transmitter::run, this);
		_thread.detach();
		return 0;
	}


	int sendMsg(const char* msg) {
		return sendMsg(msg, strlen(msg));
	}

	int sendMsg(const char* msg, int len) {
		if (-1 == _sockfd) {
			printf("****** transmitter socket not be connected!\n"); 
			return -1;
		}
		return writep(_sockfd, msg, len);		
	}

protected:
	void run() {
		int n;
		char buff[4096] = {0};
		while ((n = readp(_sockfd, buff, sizeof(buff)-1)) > 0) {
			buff[n] = 0;
			if (nullptr != _listener) {
				_listener->onReceived(buff, n);
			}
		}
		printf("****** exit transmitter thread\n"); 
	}

	void disconnect() {
		if (-1 != _sockfd) {
			close(_sockfd);
			_sockfd = -1;
		}			
	}

private:
	Listener* _listener;
	int _sockfd;
	std::thread _thread;	
};

class MyListener: public Transmitter::Listener {
public:
	virtual ~MyListener() {}
	void onReceived(const char* msg, int len) override {
		printf("=>%s", msg);
	}
};

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Usage: %s <ip of server> \n", argv[0]);
        return -1;
    } 
	
	Transmitter trans;
	MyListener listener;
	if (trans.start(argv[1], &listener) < 0) {
		return -1;
	}

	char buffer[BUF_LEN];
	while (fgets(buffer, sizeof(buffer)-1, stdin) != NULL) {
		if (trans.sendMsg(buffer) < 0) break;
	}	
	
	return 0;
}

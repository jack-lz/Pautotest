#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <thread>
#include <map>
#include <iterator>
#include <memory>

#include "sockcomm.h"

class Handler;

class HandlerManager {
public:
	void add(int fd, std::shared_ptr<Handler> handler) {
		handlers[fd] = handler;
	}

	void del(int fd) {
		handlers.erase(fd);
	}
		
	std::map<int, std::shared_ptr<Handler> >& getHandlers() {
		return handlers;
	}

private:
	std::map<int, std::shared_ptr<Handler> > handlers;
};

class Handler {
public:
	Handler(HandlerManager* manager, int fd):_manager(manager), _fd(fd) {
		_thread = std::thread(&Handler::run, this);
		_thread.detach();
	}
	
	int fd() const {
		return _fd;
	}

private:
	void run() {
		ssize_t n;
		char buffer[BUF_LEN];
		while((n=readp(_fd, buffer, sizeof(buffer)-1)) > 0) {
			buffer[n] = '\0';
			printf("=>%s", buffer);	
		}
		_manager->del(_fd);
		close(_fd);
	}

	HandlerManager* _manager;
	int _fd;
	std::thread _thread;
};

static void runInput(HandlerManager* manager) {
	char buffer[BUF_LEN];
	while (fgets(buffer, sizeof(buffer)-1, stdin) != NULL) {
		if (0 == manager->getHandlers().size()) {
			printf("****** no client be connected!\n");
			continue;
		}

		for (auto it = manager->getHandlers().begin(); it != manager->getHandlers().end(); ++it) {
			writep(it->first, buffer, strlen(buffer));	
		}
	}
}

int main(int argc, char *argv[]) {
	int err;
    struct sockaddr_in serv_addr; 

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SOCK_PORT); 

	int enable = 1;
	CHECK(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)));

    CHECK(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))); 
    CHECK(listen(listenfd, 10)); 
	printf("listening port:%d\n", SOCK_PORT);

	HandlerManager manager;
	// start input thread
	std::thread input(runInput, &manager);
	input.detach();

    while (1) {
		struct sockaddr_in cli_addr;
		socklen_t clilen = sizeof(cli_addr);
        int cfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen); 
		printf("connection from %s\n", inet_ntoa(cli_addr.sin_addr));
		if (cfd > 0) {
		  	manager.add(cfd, std::make_shared<Handler>(&manager, cfd));
		}
     }
}

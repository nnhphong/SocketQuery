#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

typedef struct sockaddr_in sockaddr_in;

int join_network(sockaddr_in a, int my_port, char *ip_addr) {
	memset(&a, 0, sizeof(sockaddr_in));

	a.sin_family = AF_INET;
	a.sin_port = htons(my_port);
	inet_pton(AF_INET, ip_addr, &a.sin_addr.s_addr);

	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(cfd, (struct sockaddr *)&a, sizeof(sockaddr_in)) == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	printf("Ready\n");
	return cfd;
}

void not_block(int fd) {
	int flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void communicate(int cfd) {
	char name[31];
	char resp[11];
	int bytes;
	for (;;) {
		if (fgets(name, sizeof(name), stdin) == NULL) {
			close(cfd);
			exit(0);
		}
		
		bytes = write(cfd, name, strlen(name) + 1);
		if (bytes <= 0) {
			if (errno) fprintf(stderr, "%s\n", strerror(errno));
			exit(1);	
		}
		
		sleep(100000); // im chilling out forever hehe
		bytes = read(cfd, resp, sizeof(resp));
		if (bytes <= 0 || strchr(resp, '\n') == NULL) {
			if (errno) fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
		printf("%s", resp);
	}	
}

int main(int argc, char *argv[]) {
	int my_port;
	sockaddr_in a;

	sscanf(argv[2], "%d", &my_port);
	int cfd = join_network(a, my_port, argv[1]); 
	//not_block(cfd);
	communicate(cfd);	
}

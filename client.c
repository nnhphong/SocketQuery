#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>

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

bool null_or_eof(char str[]) {
	return (str == NULL || (strlen(str) == 1 && str[0] == '\n'));
}

void ignore_sigpipe() {
	struct sigaction myaction;

	myaction.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &myaction, NULL);
}

<<<<<<< HEAD
<<<<<<< HEAD
void write_to_server(int cfd, char name[]) {
	int need_to_write = strlen(name);
	do {
		int bytes = write(cfd, &name[strlen(name) - need_to_write], 1);
		if (bytes == -1) {
			fprintf(stderr, "SIGPIPE %s\n", strerror(errno));
			exit(1);
		}
		need_to_write -= bytes;
	} while (need_to_write);	
=======
void exit_sigpipe(int sig) {
    fprintf(stderr, "Received SIGPIPE: The connection has been closed.\n");
    exit(EXIT_FAILURE);
>>>>>>> 1c2f741 (update: client.c)
=======
void exit_sigpipe(int sig) {
    fprintf(stderr, "Received SIGPIPE: The connection has been closed.\n");
    exit(EXIT_FAILURE);
=======
void write_to_server(int cfd, char name[]) {
	int need_to_write = strlen(name);
	do {
		int bytes = write(cfd, &name[strlen(name) - need_to_write], 1);
		if (bytes == -1) {
			fprintf(stderr, "SIGPIPE %s\n", strerror(errno));
			exit(1);
		}
		need_to_write -= bytes;
	} while (need_to_write);	
>>>>>>> 84f9e3c (feat: fix read method by chunk)
>>>>>>> d15cfff (feat: fix read method by chunk)
}

void read_from_server(int cfd, char *resp) {
	char buffer[12];
	int len = 0;
	bool found = false;

	do {
		int bytes = read(cfd, buffer, 11);	
		if (bytes == 0) {
			fprintf(stderr, "Server disconnected!\n");
			exit(1);
		}

		if (bytes == -1) {
			if (errno == EAGAIN) continue;
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}

		if (len > 11) {
			fprintf(stderr, "Message too long! Server bug\n");
			exit(1);
		}
		
		for (int i = len; i < len + bytes; i++) {
			resp[i] = buffer[i - len];
			if (resp[i] == '\n') found = true;
		}
		len += bytes;
	} while (!found);
	resp[len] = '\0';
}

void write_to_server(int cfd, char *name) {
	int bytes = write(cfd, name, strlen(name));
	if (bytes == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
}

void communicate(int cfd) {
	char name[37];
	char resp[11] = "";
	int bytes;
	
	for (; fgets(name, sizeof(name), stdin) != NULL;) {
		if (null_or_eof(name)) {
			fprintf(stderr, "closing this client..\n");
			close(cfd);
			exit(0);
		}
		
		strcpy(resp, "");
		write_to_server(cfd, name);		
		read_from_server(cfd, resp);		
		printf("%s", resp);
	}	
}

int main(int argc, char *argv[]) {
	int my_port;
	sockaddr_in a;

	sscanf(argv[2], "%d", &my_port);
	int cfd = join_network(a, my_port, argv[1]); 
	ignore_sigpipe();
	communicate(cfd);
}

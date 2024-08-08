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

int main(int argc, char *argv[]) {
	int my_port;
	sockaddr_in a;

	sscanf(argv[2], "%d", &my_port);
	int cfd = join_network(a, my_port, argv[1]); 
	size_t len;

	char buffer[1024];
    char msg_1[] = "Dennis Ritchie\nAlan Turing\naaaaaaaaa";
    char msg_2[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
    write(cfd, msg_1, strlen(msg_1));
	len = read(cfd, buffer, sizeof(buffer));
	printf("%.*s", len, buffer);

	write(cfd, msg_2, strlen(msg_2));
	len = read(cfd, buffer, sizeof(buffer));
	printf("%.*s", len, buffer);

	return 0;
}

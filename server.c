#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include "record.h"

typedef struct sockaddr_in sockaddr_in;

void ignore_sigpipe() {
	struct sigaction myaction;

	myaction.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &myaction, NULL);
}

int get_sunspots(FILE *f, const char *name, unsigned short *psunspots) {  
 	if (fseek(f, 0, SEEK_SET) == -1) {  
    	perror("Error: cannot seek to the beginning");  
    	return 0;  
	}  
   
	for (record rec; fread(&rec, sizeof(rec), 1, f) == 1; ) {  
    	char rname[100];  
    	snprintf(rname, rec.name_len + 1, "%s", rec.name);  
    	if (strncmp(rname, name, sizeof(name)) == 0) {  
        	*psunspots = rec.sunspots;  
        	return 1;  
    	}  
  	}    
  	return 0;  
}  


int setup_server(sockaddr_in a, int my_port, char *ip_addr) {
	memset(&a, 0, sizeof(sockaddr_in));

	a.sin_family = AF_INET;
	a.sin_port = htons(my_port);
	a.sin_addr.s_addr = htonl(INADDR_ANY);

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(sfd, (struct sockaddr *)&a, sizeof(sockaddr_in)) == -1) {
		fprintf(stderr, "Cannot establish the connection.\n");
		exit(1);
	}
	
	listen(sfd, 4096);
	return sfd;
}

void print_client_address(const char *prefix, const struct sockaddr_in *ptr) {
	char dot_notation[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ptr->sin_addr, dot_notation, INET_ADDRSTRLEN);
	printf("%s: %s port %d\n", prefix, dot_notation, ntohs(ptr->sin_port));
}

int max_fds(int sfd, int cfd[], int num_clients, fd_set *read_fds) {
	int mx = sfd;
	for (int i = 0; i < num_clients; i++) {
		FD_SET(cfd[i], read_fds);
		if (cfd[i] > mx) {
			mx = cfd[i];
		}
	}
	return mx;
}

void add_new_client(int sfd, int *client_list, int *num_client, fd_set *read_fds) {
	sockaddr_in ca;
	int sin_len = sizeof(sockaddr_in);
	int new_cfd = accept(sfd, (struct sockaddr*)&ca, &sin_len);
	
	FD_SET(new_cfd, read_fds);
	client_list[*num_client] = new_cfd;
	(*num_client)++;
	print_client_address("Got client", &ca);
}

void delete_client(int *cfd, int i, int *num_clients) {
	close(cfd[i]);
	for (int j = i + 1; j < *(num_clients); j++) {
		cfd[i] = cfd[j];
	}
	*(num_clients)--;
}

bool talk_to_client(int i, int *cfd, FILE *f, int *num_clients) {
	unsigned short ret;
	char query[30];
	char response[11];
	int bytes;

	bytes = read(cfd[i], query, sizeof(query));
	if (bytes <= 0) {
		fprintf(stderr, "ERRNO read %d: %s\n", errno, strerror(errno));
		delete_client(cfd, i, num_clients);
		return 0;
	}
	if (!strcmp(query, "")) return 1; 

	strcpy(response, "none\n");
	if (get_sunspots(f, query, &ret)) {
		sprintf(response, "%d\n", ret);
	}

	bytes = write(cfd[i], response, sizeof(response));
	return 1;
}

void run_server(sockaddr_in a, int sfd, FILE *f) {
	int cfd[1024];
	bool is_opened[1024];
	int num_clients = 0;
	fd_set read_fds;
	
	for (;;) {
		FD_ZERO(&read_fds);
		FD_SET(sfd, &read_fds);
		int max_fd = max_fds(sfd, cfd, num_clients, &read_fds);

		while (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {}
		if (FD_ISSET(sfd, &read_fds)) {
			add_new_client(sfd, cfd, &num_clients, &read_fds);
		}
		
		for (int i = 0; i < num_clients; i++) {
			if (FD_ISSET(cfd[i], &read_fds)) {
				//is_opened[i] = 
				talk_to_client(i, cfd, f, &num_clients);	
			}
		}
		/*for (int i = 0; i < num_clients; i++) {
			if (!is_opened[i]) {
//				printf("Server: I'm deleting client %i\n", i);
				delete_client(cfd, i, &num_clients);
				i--;
			}
		}*/
	}
}

int main(int argc, char *argv[]) {
	int my_port;
	sockaddr_in a;

	sscanf(argv[1], "%d", &my_port);
	FILE *f = fopen(argv[2], "r");
	
	ignore_sigpipe();
	int sfd = setup_server(a, my_port, argv[1]); 
	run_server(a, sfd, f);
	fclose(f);
}

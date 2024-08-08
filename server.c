#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include "record.h"

typedef struct sockaddr_in sockaddr_in;

void ignore_sigpipe() {
	struct sigaction myaction;

	myaction.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &myaction, NULL);
}

void not_block(int fd) {
	int flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int get_sunspots(FILE *f, char *name, unsigned short *psunspots) {  
 	if (fseek(f, 0, SEEK_SET) == -1) {  
    	perror("Error: cannot seek to the beginning");  
    	return 0;  
	}  
   	
	name[strlen(name) - 1] = '\0';
	for (record rec; fread(&rec, sizeof(rec), 1, f) == 1; ) {  
    	char rname[100];  
    	snprintf(rname, rec.name_len + 1, "%s", rec.name);  

    	if (strcmp(rname, name) == 0) {  
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
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	
	listen(sfd, 4096);
	return sfd;
}

int max_fds(int sfd, int cfd[], int num_clients, fd_set *fd_reads) {
	int mx = sfd;
	for (int i = 0; i < num_clients; i++) {
		FD_SET(cfd[i], fd_reads);
		if (cfd[i] > mx) {
			mx = cfd[i];
		}
	}
	return mx;
}

void add_new_client(int sfd, int *client_list, int *num_client, fd_set *fd_reads) {
	sockaddr_in ca;
	int sin_len = sizeof(sockaddr_in);
	int new_cfd = accept(sfd, (struct sockaddr*)&ca, &sin_len);
	
	not_block(new_cfd);
	FD_SET(new_cfd, fd_reads);

	client_list[*num_client] = new_cfd;
	(*num_client)++;
}

void delete_client(int *cfd, int i, int *num_clients) {
	close(cfd[i]);
	for (int j = i + 1; j < *(num_clients); j++) {
		cfd[j - 1] = cfd[j];
	}
	(*num_clients)--;
}

bool handle_incident(int i, int *cfd, int *num_clients) {
	if (errno == EAGAIN) {
		// reset errno (as SIGINT on client doesnt get notified)
		errno = 0;
		return 1;
	}
		
	if (errno) fprintf(stderr, "%s\n", strerror(errno));
	fprintf(stderr, "deleting..\n");
	delete_client(cfd, i, num_clients);
	return 0;
}

char left_over[500][40] = { "" };
int top = 0, cur = 0;

int read_from_client(int i, int *cfd, char *q, int *num_clients) {
	int len = strlen(left_over[top]);
<<<<<<< HEAD
=======
	//printf("Reading...\n");
>>>>>>> d15cfff (feat: fix read method by chunk)
	char buffer[31];
	bool found = 0;
	do {
		int bytes = read(cfd[i], buffer, 30);
<<<<<<< HEAD
=======
		//printf("errno: %d %s %d\n", errno, strerror(errno), len);
>>>>>>> d15cfff (feat: fix read method by chunk)
		
		// handle SIGINT and unexpected error case	
		if (len == 0 && bytes <= 0 && !(len > 0 && errno == EAGAIN)) {
			return handle_incident(i, cfd, num_clients);
		}	
		
		for (int i = 0; i < bytes; i++) {	
			if (buffer[i] == '\n') {
				if (len <= 30) {
					left_over[top][len] = '\n';
				}
				top++;
				len = 0;
				found = 1;
				continue;
			}

			if (len > 30) {
<<<<<<< HEAD
=======
				/*len++;
				if (len > 60) {
					// repeat forever
					found = true;
					break;
				}
				continue;*/
				printf("%s\n", left_over[top]);
>>>>>>> d15cfff (feat: fix read method by chunk)
				found = true;
				top++;
				break;
			}
			
			left_over[top][len] = buffer[i];
			len++;
		}
	} while (!found);
	return -1;
}

int write_to_client(int i, int *cfd, char *resp, int *num_clients) {
<<<<<<< HEAD
=======
	//printf("Writing...%s\n", resp);	
	/*int need_to_write = strlen(resp);
	do {
		int bytes = write(cfd[i], &resp[strlen(resp) - need_to_write], 1);
		if (bytes == -1) {
			return handle_incident(i, cfd, num_clients);
		}	
		need_to_write -= bytes;
	} while (need_to_write);*/
>>>>>>> d15cfff (feat: fix read method by chunk)
	int bytes = write(cfd[i], resp, strlen(resp));
	if (bytes == -1) {
		return handle_incident(i, cfd, num_clients);
	}
	return -1;
}

bool talk_to_client(int i, int *cfd, int *num_clients, FILE *f) {
	unsigned short ret;
	char q[37];
	char resp[1024] = "none\n";	
	int code;

	if ((code = read_from_client(i, cfd, q, num_clients)) != -1) {
		return code;
	}
	
<<<<<<< HEAD
	for (int k = cur; k <= top; k++) {
=======
	//printf("%d %d\n", cur, top);
	for (int k = cur; k <= top; k++) {
	//	printf("%s %ld\n", left_over[i], strlen(left_over[i]));
		
>>>>>>> d15cfff (feat: fix read method by chunk)
		if (strchr(left_over[k], '\n') == NULL) {
			if (strlen(left_over[k]) < 30) break;
			
			cur = k + 1;
			fprintf(stderr, "Message too long! Imma close this client\n");
			delete_client(cfd, i, num_clients);
			return 0;
		}
		cur = k + 1;
		
		strcpy(q, left_over[k]);
		strcpy(resp, "none\n");
		if (get_sunspots(f, q, &ret)) {
			sprintf(resp, "%d\n", ret);		
		}	
	
		if ((code = write_to_client(i, cfd, resp, num_clients)) != -1) {
			return code;
		}
	}	
	return 1;
}

void run_server(sockaddr_in a, int sfd, FILE *f) {
	int cfd[1024];
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
			if (!FD_ISSET(cfd[i], &read_fds)) continue;
			i -= (!talk_to_client(i, cfd, &num_clients, f));
		}
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

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

typedef struct query {
	char input_list[37][37];
	int num_input;
} query;

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
		printf("ok\n");
		errno = 0;
		return 1;
	}
		
	if (errno) fprintf(stderr, "%s\n", strerror(errno));
	fprintf(stderr, "deleting..\n");
	delete_client(cfd, i, num_clients);
	return 0;
}

void add_newline(char q[]) {
	int cur_len = strlen(q);
	if (cur_len < 30 && q[cur_len - 1] != '\n') {
		q[cur_len] = '\n';
		q[cur_len + 1] = '\0';
	}	
}

char left_over[37];

query* extract_query(char q[], int bytes) {
    query *res = malloc(sizeof(query));
    res->num_input = 0;

	bool end_with_newline = (q[bytes - 1] == '\n');

    // Tokenize the string	
	strcat(left_over, strtok(q, "\n"));
	char *token = left_over;

    while (token != NULL) {
        strcpy(res->input_list[res->num_input], token);
		add_newline(res->input_list[res->num_input++]);

		//printf("Token: %s", res->input_list[res->num_input - 1]);
        token = strtok(NULL, "\n");
    }
	
	if (!end_with_newline) {
		//printf("here %c\n", q[bytes - 1]);
		int cur_len = strlen(res->input_list[res->num_input - 1]);	
		res->input_list[res->num_input - 1][cur_len - 1] = '\0';
	}
	
	memset(left_over, 0, sizeof(left_over));	

    return res;
}

bool talk_to_client(int i, int *cfd, int *num_clients, FILE *f) {
	unsigned short ret;
	char q[37];
	char final_resp[2000];

	strcpy(final_resp, "");

	int bytes = read(cfd[i], q, sizeof(q));
	q[bytes] = '\0';
	if (bytes <= 0 || !strcmp(q, "")) {
		printf("%d %s\n", bytes, q);
		return handle_incident(i, cfd, num_clients);	
	}

	printf("Query = %d %s, last char = %c", bytes, q, q[bytes - 1]);
	query *all_queries = extract_query(q, bytes);	
	for (int k = 0; k < all_queries->num_input; k++) {
		char resp[11] = "none\n";
		char* cur_query = all_queries->input_list[k];

		if (cur_query[strlen(cur_query) - 1] != '\n') {
			strcpy(left_over, cur_query);
			//printf("cur_query = %s", cur_query);
			left_over[strlen(left_over)] = '\0';
			continue;
		}
		
		printf("Received: %s", cur_query);
		if (strchr(cur_query, '\n') == NULL) {
			return handle_incident(i, cfd, num_clients);
		}

		if (get_sunspots(f, cur_query, &ret)) {
			sprintf(resp, "%d\n", ret);
		}
		strcat(final_resp, resp); 
	}
	
	//printf("final response: len = %ld, %s", strlen(final_resp), final_resp);
	bytes = write(cfd[i], final_resp, strlen(final_resp));
	if (bytes <= 0) {
		return handle_incident(i, cfd, num_clients);
	}

	free(all_queries);
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

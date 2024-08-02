#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "record.h"

typedef struct sockaddr_in sockaddr_in;

void ignore_sigpipe() {
	struct sigaction myaction;

	myaction.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &myaction, NULL);
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

int get_sunspots(FILE *f, const char *name, unsigned short *psunspots) {  
 	if (fseek(f, 0, SEEK_SET) == -1) {  
    	perror("Error: cannot seek to the beginning");  
    	return 0;  
	}  
   
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

void run_server(sockaddr_in a, int sfd, FILE *f) {
	int cfd, sin_len = sizeof(sockaddr_in);
	unsigned short ret;
	char query[30], response[11];
	for (;;) {
		
		cfd = accept(sfd, (struct sockaddr*)&a, &sin_len);
		read(cfd, query, sizeof(query));
		if (!get_sunspots(f, query, &ret)) {
			write(cfd, "none\n", 5);
		}
		else {
			sprintf(response, "%d\n", ret);
			write(cfd, response, sizeof(response));
		}
		close(cfd);
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

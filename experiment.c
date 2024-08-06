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

#define MAX_INPUT_LENGTH 37
#define MAX_INPUT_COUNT 37

typedef struct query {
    char input_list[MAX_INPUT_COUNT][MAX_INPUT_LENGTH];
    int num_input;
} query;

void add_newline(char q[]) {
	int cur_len = strlen(q);
	if (cur_len < 30) {
		q[cur_len] = '\n';
		q[cur_len + 1] = '\0';
	}	
}

query* extract_query(char q[]) {
    query *res = malloc(sizeof(query));
    res->num_input = 0;

    // Tokenize the string
    char* token = strtok(q, "\n");
    while (token != NULL) {
        strcpy(res->input_list[res->num_input], token);
		add_newline(res->input_list[res->num_input++]);
        token = strtok(NULL, "\n");
    }

    return res;
}

int main() {
    // Make a mutable copy of the input string
    char input[] = "abc\ncd\ne";
    query* res = extract_query(input);

    // Print the results
    for (int i = 0; i < res->num_input; i++) {
        printf("%s", res->input_list[i]);
    }

    // Free allocated memory
    free(res);

    return 0;
}

#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include "tpool.h"


static size_t threads_num = 1;

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
// finally we want to be able to do this:
	// ./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	tpool_t *tm;

    while ((c = getopt(argc, argv, "d:p:t:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		threads_num = atoi(optarg);
		break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t thread_count]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);

	tm = tpool_create(threads_num);
	if (tm == NULL) {
		fprintf(stderr, "main: tpool_create failure");
		exit(EXIT_FAILURE);
	}

    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int *conn_fd_ptr = malloc(sizeof(int));
		if (conn_fd_ptr == NULL) {
			perror("conn_fd_ptr error");
			exit(EXIT_FAILURE);
		}
		*conn_fd_ptr = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		tpool_add_work(tm, request_handle, conn_fd_ptr);

		// old code
		// request_handle(conn_fd);
		// close_or_die(conn_fd); // moved to the bottom of request_handle method
    }
	
    return 0;
}


    


 

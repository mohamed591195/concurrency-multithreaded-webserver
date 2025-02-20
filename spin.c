#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAXBUF (8192)

//
// This program is intended to help you test your web server.
// You can use it to test that you are correctly having multiple threads
// handling http requests.
// 

double get_seconds() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) ((double)t.tv_sec + (double)t.tv_usec / 1e6);
}

char* generate_dynamic_response(size_t size) {
    char *response = malloc(size + 1);
    if (!response) {
        fprintf(stderr, "Memory allocation failed at generate_dynamic_response\n");
        return NULL;
    }
    memset(response, 'A', size);
    response[size] = '\0';
    return response;
}

// every output here (i.e printf) is written into the socket not ouput stream
// we have interchanged them before executing this program for convenience 
int main(int argc, char *argv[]) {
    // Extract arguments
    double spin_for = 0.0;
    size_t desired_size = 0;
    char *buf;

    // getting arguments from the environ "QUERY_STRING" we set earlier in the parent process 
    if ((buf = getenv("QUERY_STRING")) != NULL) {

        // strstr returns a pointer to the first occurence of the string 
        char *size_param = strstr(buf, "size=");
        
        if (size_param) {
            desired_size = (size_t) atoi(size_param + 5);
        }

        else {
            // just expecting a single number for example "/sping.cgi?5" for waiting 5 seconds
            spin_for = (double) atoi(buf);
        }
    }

    // extra data for file size maximization 
    char *response = generate_dynamic_response(desired_size);

    double t1 = get_seconds();
    while ((get_seconds() - t1) < spin_for)
	sleep(1);
    double t2 = get_seconds();
    
    /* Make the response body */
    char content[MAXBUF];
    snprintf(content, MAXBUF, "<p>Welcome to the CGI program (%s)</p>\r\n", buf);
    
    int offset = strlen(content);
    offset += snprintf(content + offset, MAXBUF - offset, "<p>My only purpose is to waste time on the server!</p>\r\n");
    snprintf(content + offset, MAXBUF - offset, "<p>I spun for %.2f seconds</p>\r\n", t2 - t1);
    
    /* Generate the HTTP response */
    printf("Content-Length: %lu\r\n", strlen(content) + desired_size);
    printf("Content-Type: text/html\r\n\r\n");
    printf("%s", content);
    if (desired_size) printf("%s", response);
    fflush(stdout);
    
    exit(0);
}


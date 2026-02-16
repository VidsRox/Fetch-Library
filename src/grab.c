#include "grab.h"
#include "fetch.h"
#include "utils.h"
#include <stdlib.h>
#include "timer.h"

void print_response_summary(Response *resp, int request_num) {
    if (resp) {
        printf("Request %d: Status %d, Size %lld bytes\n", 
               request_num, resp->status, resp->size);
    } else {
        printf("Request %d: FAILED\n", request_num);
    }
}

int main() {
    Timer total_timer;
    Request req;
    Response *resp;
    int num_requests = 5;  // Test with 5 requests
    
    printf("========================================\n");
    printf("Sequential Fetch Performance Test\n");
    printf("========================================\n");
    printf("Making %d requests to Google...\n\n", num_requests);

    //set up request
    req.method = get;
    req.url.protocol = http;
    my_strcopy((char *)req.url.domain, "www.google.com");
    my_strcopy((char *)req.url.file, "/");
    req.contentType[0] = NULL;
    req.cookies = NULL;
    req.headers = NULL;
    req.data = NULL;
    req.size = 0;

    // Start total timer
    timer_start(&total_timer);

    //Make request
    resp = fetch_sync(&req);

    // Start total timer
    timer_start(&total_timer);
    
    // Make multiple requests sequentially
    for (int i = 1; i <= num_requests; i++) {
        Timer request_timer;
        
        printf("Fetching request %d... ", i);
        fflush(stdout);
        
        timer_start(&request_timer);
        resp = fetch_sync(&req);
        double request_time = timer_stop(&request_timer);
        
        // Start total timer
    timer_start(&total_timer);
    
    // Make multiple requests sequentially
    for (int i = 1; i <= num_requests; i++) {
        Timer request_timer;
        
        printf("Fetching request %d... ", i);
        fflush(stdout);
        
        timer_start(&request_timer);
        resp = fetch_sync(&req);
        double request_time = timer_stop(&request_timer);
        
        if (resp) {
            printf("✓ %d bytes in %.2f ms\n", (int)resp->size, request_time);
            free_response(resp);
        } else {
            printf("✗ FAILED\n");
        }
    }
    
    // Stop total timer
    double total_time = timer_stop(&total_timer);
    
    printf("\n========================================\n");
    printf("Results:\n");
    printf("========================================\n");
    printf("Total requests: %d\n", num_requests);
    printf("Total time:     %.2f ms (%.2f seconds)\n", total_time, total_time/1000.0);
    printf("Avg per request: %.2f ms\n", total_time / num_requests);
    printf("========================================\n");
    }
    
    // Stop total timer
    double total_time = timer_stop(&total_timer);
    
    printf("\n========================================\n");
    printf("Results:\n");
    printf("========================================\n");
    printf("Total requests: %d\n", num_requests);
    printf("Total time:     %.2f ms (%.2f seconds)\n", total_time, total_time/1000.0);
    printf("Avg per request: %.2f ms\n", total_time / num_requests);
    printf("========================================\n");

    return 0;
}
#include "fetch.h"
#include "http.h"
#include "network.h"
#include "utils.h"
#include "stdlib.h" 

//execute a complete HTTP request/response cycle

Response* fetch_sync(Request *req) {
  int32 sock; //Socket file descriptor
  Sockaddr addr; //Server address (IP + port)
  char request_buffer[8192]; //usually less than 8KB
  char response_buffer[65536];//64 KB handles most responses, larger responses
                              //would need chunked reading(not implemented)
  int request_len, bytes_received;
  Response *resp;  

  //Allocate response
  resp = malloc(sizeof(Response));
  if(!resp) return NULL;

  //Initialize response
  resp->status = 0;
  resp->contentType[0] = NULL;
  resp->headers = NULL;
  resp->cookies = NULL;
  resp->data = NULL;
  resp->size = 0;

    //1. Create socket
    sock = socket_create(AF_INET, SOCK_STREAM, 0);
    if(sock<0){
        printf("Failed to create socket\n");
        free(resp);
        return NULL;
    }

    //2. Setup addresss
    addr.family = AF_INET;
    addr.port = htons(80); //HTTP Port

    //harcode IP address for now
    //add dns resolution later
    addr.addr = inet_addr("142.250.192.196");

    //3. Connect
    if(!socket_connect(sock, &addr, sizeof(Sockaddr))) {
        printf("Failed to connect\n");
        socket_close(sock);
        free(resp);
        return NULL;
    }

    //printf("Connected succesfully!\n");

    //4. Build HTTP request
    request_len = build_http_request(req, request_buffer, sizeof(request_buffer));
    //printf("Sending request:\n%s\n", request_buffer);

    //5. Send request
    if(socket_send(sock, request_buffer, request_len) < 0){
        printf("Failed to send request\n");
        socket_close(sock);
        free(resp);
        return NULL;
    }

    //6. Receive response
    bytes_received = socket_recv(sock, response_buffer, sizeof(response_buffer) - 1);
    if(bytes_received < 0){
        printf("Failed to receive response\n");
        socket_close(sock);
        free(resp);
        return NULL;
    }

    response_buffer[bytes_received] = '\0';
    //printf("Received %d bytes\n", bytes_received);

    //7. Parse response
    if(!parse_http_response(response_buffer, resp)){
        printf("Failed to parse response\n");
        socket_close(sock);
        free(resp);
        return NULL;
    }

    //8. Copy data (so it persists after buffer is gone)
    if (resp->size > 0){
        char *body_copy = malloc(resp->size + 1);
        int i;
        for(i = 0; i < resp->size; i++){
            body_copy[i] = resp->data[i];
        }
        body_copy[resp->size] = '\0';
        resp->data = (int8 *)body_copy;
    }

    socket_close(sock);

    return resp;

}

void free_response(Response *resp){
    if(resp) {
        if(resp->data) free(resp->data);
        free(resp);
    }
}

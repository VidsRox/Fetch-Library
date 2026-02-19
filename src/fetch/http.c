#include "http.h"
#include "utils.h"

/* Returns pointer to string literal in .rodata (read-only section) as
   compilers place string literals in the .rodata section*/
const char* method_to_string(Method method) {

    switch (method)
    {
    case get: return "GET";
    case post: return "POST";
    case put: return "PUT";
    case delete: return "DELETE";
    case patch: return "PATCH";
    case head: return "HEAD";
    default: return "GET";
    }
}

//string concatenation
static void append_str(char **ptr, const char *str) {
    while(*str) {
        **ptr = *str++;
        (*ptr)++;
    }
}

//number to string conversion
static void append_int(char **ptr, int64 num) {
    char temp[32];
    int i = 0;

    if (num==0){
        **ptr = '0';
        (*ptr)++;
        return;
    }

    //build digits in reverse
    while(num>0){
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }

    //write to buffer in correct order
    while(i > 0){
        **ptr = temp[--i];
        (*ptr)++;
    }
}

int build_http_request(Request *req, char *buffer, int buffer_size) {
    char *ptr = buffer;

    //request line: GET /path HTTP/1.1\r\n
    append_str(&ptr, method_to_string(req->method));
    append_str(&ptr, " ");
    append_str(&ptr, (char *)req->url.file);
    append_str(&ptr, " HTTP/1.1\r\n");

    //host header
    append_str(&ptr, "Host: ");
    append_str(&ptr, (char *)req->url.domain);
    append_str(&ptr, "\r\n");

    //User-Agent
    append_str(&ptr, "User-Agent: FetchLibC/1.0\r\n");

    //Connection
    append_str(&ptr, "Connection: close\r\n");

    //if we have data (POST/PUT), add Content-Length
    if(req->data != NULL && req->size > 0){
        append_str(&ptr, "Content-Length: ");
        append_int(&ptr, req->size);
        append_str(&ptr, "\r\n");

        //Content-Type if specified
        if(req->contentType[0] != NULL){
            append_str(&ptr, "Content-Type: ");
            append_str(&ptr, (char *)req->contentType[0]);
            append_str(&ptr, "\r\n");
        }
    }

    //End of headers
    append_str(&ptr, "\r\n");

    //Body data if present
    if(req->data != NULL && req->size > 0){
        int i;
        for(i = 0; i< req->size; i++){
            *ptr++ = req->data[i];
        }
    }

    *ptr = '\0';
    return ptr - buffer;

}

//Find substring in string, like strstr()
static char* find_str(char *haystack, const char *needle) {
    
    //Algorithm: Naive String Matching
    // O(n × m) time complexity
    
    while(*haystack) {
        char *h = haystack;//working copy
        const char *n = needle;//working copy

//Reason for h & n: We need to compare character-by-character WITHOUT losing our starting position
        
        while(*h && *n && *h == *n){
            h++;
            n++;
        }
        
        if (*n == '\0') return haystack;
        haystack++;
    }
    return NULL;
}

//string to integer
static int parse_int(const char *str){
    int result = 0;
    while (*str >= '0' && *str <= '9'){
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}


// Set pointer directly to body in raw buffer (zero-copy)
// resp->data = body_start;  // Just stores ADDRESS (O(1), no data moved)

// vs wasteful memcpy:
// char *copy = malloc(size); memcpy(copy, body_start, size); resp->data = copy;
// ^ Allocates + copies ALL bytes (O(n) time, extra heap RAM, leak risk if not freed)

bool parse_http_response(const char *raw, Response *resp){

    char *line = (char *) raw;

    //parse status line: "HTTP/1.1 200 OK\r\n"
    while(*line && *line != ' ') line++;// Skip "HTTP/1.1"
    if(*line == ' ')line++;

    resp->status = parse_int(line);
    
    //Find start of headers
    line = find_str(line, "\r\n");
    if(!line) return false;
    line += 2;

    //Parse headers until we hit blank line
    while (line[0] != '\r' || line[1] != '\n') {
        //check for Content-Type
        if(line[0] == 'C' && find_str(line, "Content-Type:") == line){
            char *value = line + 13; //skip "Content-Type:"
            while(*value == ' ') value++;// Skip spaces

            int i = 0;
            while (value[i] != '\r' && value[i] != '\0' && i<255){
                ((char *)resp->contentType)[i] = value[i];
                i++;
            }
            ((char *)resp->contentType)[i] = '\0';
        }

        //Move to next line
        line = find_str(line, "\r\n");
        if(!line) return false;
        line += 2;
    }

    //Skip blank line (end of headers)
    line += 2;

    //Remaining data is the body
    resp->data = (int8 *)line; // points to the original buffer
    resp->size = my_strlen(line);

    return true;
}

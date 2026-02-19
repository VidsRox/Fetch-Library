#ifndef HTTP_H
#define HTTP_H

#pragma once
#include "grab.h"

// Build HTTP request string from Request struct
int build_http_request(Request *req, char *buffer, int buffer_size);

// Parse raw HTTP response into Response struct
bool parse_http_response(const char *raw, Response *resp);

//Get method string from enum
const char* method_to_string(Method method);

#endif  // HTTP_H

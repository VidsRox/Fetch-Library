#ifndef FETCH_H
#define FETCH_H


#pragma once
#include "grab.h"

//synchronous fetch
//"Sync" means: Blocks (waits) until response arrives

/*Why pointer return?

Response could be large (kilobytes/megabytes)
Memory allocated on heap (persists after function returns)
Caller must free it later*/

Response* fetch_sync(Request *req);


//Free response memory

/*What it frees:

The Response struct itself
The body data (if copied)
Any other allocated memory*/

void free_response(Response *resp);

#endif  // FETCH_H
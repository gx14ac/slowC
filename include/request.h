/***** request.h *****
* Author : flekystyley
* Date   : 17.2.2020
* Brief  : http request header  for slowC
*********************/

#ifndef REQUEST
#define REQUEST

#define __LIBREQ_VERS__ "v0.2"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/utsname.h>

typedef struct {
    CURL   *curlhandle;
    long   code;
    char   *url;
    curl   *text;
    size_t size;
    char   **req_hdrv;
    int    req_hdrc;
    char   **resp_hdrv;
    int    resp_hdrc;
    int    ok;
} REQ_T;

int request_init(REQ_T *req);
void request_close(REQ_T *req);
CURLcode get_request(REQ_T *req, char *url);
CURLcode post_request(REQ_T *req, char *url, char *data);
CURLcode put_request(REQ_T *req, char *url, char *data);

CURLcode get_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc);
CURLcode post_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc);
CURLcode put_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc);

char *request_url_encode(REQ_T *req, char **data, int data_size);

#endif // REQUEST

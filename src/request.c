/***** request.c *****
* Author : flekystyley
* Date   : 17.2.2020
* Brief  : http request impl for slowC
*********************/

#include "../include/request.h"

static void common_opt(REQ_T *req);
static char *user_agent(void);
static int check_ok(long code);
static CURLcode requests_pt(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc, int put_flag);
static int hdrv_append(char ***hdrv, int *hdrc, char *new);
static CURLcode process_custom_headers(struct curl_slist **slist,
                                       REQ_T *req, char **custom_hdrv,
                                       int custom_hdrc);

int request_init(REQ_T *req)
{
    req->code = 0;
    req->url = NULL;
    req->size = 0;
    req->req_hdrc = 0;
    req->resp_hdrc = 0;
    req->ok = -1;

    req->text = calloc(1, 1);
    if (req->text == NULL) {
        goto fail;
    }

    req->req_hdrv = calloc(1, 1);
    if (req->req_hdrv == NULL) {
        free(req->text);
        goto fail;
    }

    req->resp_hdrv = calloc(1, 1);
    if (req->resp_hdrv == NULL) {
        free(req->text);
        free(req->req_hdrv);
        goto fail;
    }

    req->curlhandle = curl_easy_init();
    if (req->curlhandle == NULL) {
        free(req->text);
        free(req->req_hdrv);
        free(req->req_hdrv);
        goto fail;
    }

    return 0;

fail:
    return -1;
}

/**
 * @function
 * @requests_close - Calls curl clean up and free allocated memory
 *
 * @req: requests struct
 */
void request_close(REQ_T *req)
{
    for (int i = 0; i < req->resp_hdrc; i++) {
        free(req->resp_hdrv[i]);
    }

    for (int i = 0; i < req->req_hdrc; i++) {
        free(req->req_hdrv[i]);
    }

    free(req->text);
    free(req->resp_hdrv);
    free(req->req_hdrv);

    curl_easy_cleanup(req->curlhandle);
}

/**
 * @function
 * @resp_callback - Callback function for request, may be called multiple
 * times per request. Allicates memory and assembles response data.
 * Note: `content` will not be NULL terminated.
 */
static size_t resp_callback(char *content, size_t size, size_t nmemb,
                            REQ_T *userdata)
{
    size_t real_size = size * nmemb;

    userdata->text = realloc(userdata->text, userdata->size + real_size + 1);
    if (userdata->text == NULL) {
        return -1;
    }

    userdata->size += real_size;

    char *responseText = strndup(content, real_size + 1);
    if (responseText == NULL) {
        return -1;
    }

    strncat(userdata->text, responseText, real_size);

    free(responseText);
    return real_size;
}

/**
 * @function
 * @header_callback - Callback function for headers, called once for each
 * header. Allocates memory and assembles headers into string array.
 *
 * Note: `content' will not be NULL terminated.
 */
static size_t header_callback(char *content, size_t size, size_t nmemb,
                               REQ_T *userdata)
{
    size_t real_size = size * nmemb;
    if (strcmp(content, "\r\n") == 0) {
        return real_size;
    }

    if (hdrv_append(&userdata->resp_hdrv, &userdata->resp_hdrc, content)) {
        return -1;
    }

    return real_size;
}

/**
 * @function
 * @requests_get - Performs GET request and populates req struct text member
 * with request response, code with response code, and size with size of
 * response.
 *
 * Returns the CURLcode return code provided from curl_easy_perform. CURLE_OK
 * is returned on success.
 *
 * @req:  request struct
 * @url:  url to send request to
 */
CURLcode get_request(REQ_T *req, char *url) {
    CURLcode rc;
    char *ua = user_agent();
    req->url = url;
    long code;
    CURL *curl = req->curlhandle;

    common_opt(req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        return rc;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    req->ok = check_ok(code);
    free(ua);

    return rc;
}

CURLcode get_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc)
{
    CURLcode rc;
    struct curl_slist *slist = NULL;
    char *ua = user_agent();
    req->url = url;
    long code;
    CURL *curl = req->curlhandle;

    if (custom_hdrv != NULL) {
        rc = process_custom_headers(&slist, req, custom_hdrv, custom_hdrc);
        if (rc != CURLE_OK) {
            return rc;
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    common_opt(req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        return rc;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    if (slist != NULL) {
        curl_slist_free_all(slist);
    }
    req->ok = check_ok(code);
    free(ua);

    return rc;
}

char *request_url_encode(REQ_T *req, char **data, int data_size)
{
    char *key, *val, *tmp;
    int offset;
    size_t term_size;
    size_t tmp_len;
    CURL *curl = req->curlhandle;

    if (data_size % 2 != 0) {
        return NULL;
    }

    size_t total_size = 0;
    for (int i = 0; i < data_size; i++) {
        tmp = data[i];
        tmp_len = strlen(tmp);
        total_size += tmp_len;
    }

    char encoded[total_size];
    snprintf(encoded, total_size, "%s", "");
    for (int i = 0; i < data_size; i+=2) {
        key = data[i];
        val = data[i+1];
        offset = i == 0 ? 2 : 3;
        term_size = strlen(key) + strlen(val) + offset;
        char term[term_size];
        if (i == 0) {
            snprintf(term, term_size, "%s=%s", key, val);
        } else {
            snprintf(term, term_size, "%s=%s", key, val);
        }
        strncat(encoded, term, strlen(term));
    }

    char *full_encoded = curl_easy_escape(curl, encoded, strlen(encoded));
    return full_encoded;
}

CURLcode post_request(REQ_T *req, char *url, char *data)
{
    return requests_pt(req, url, data, NULL, 0, 0);
}

CURLcode put_request(REQ_T *req, char *url, char *data)
{
    return requests_pt(req, url, data, NULL, 0, 1);
}

CURLcode post_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc)
{
    return requests_pt(req, url, data, custom_hdrv, custom_hdrc, 0);
}

CURLcode put_header_request(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc)
{
    return requests_pt(req, url, data, custom_hdrv, custom_hdrc, 1);
}

static CURLcode requests_pt(REQ_T *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc, int put_flag)
{
    CURLcode rc;
    struct curl_slist *slist = NULL;
    req->url = url;
    CURL *curl = req->curlhandle;

    if (data != NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    } else {
        char *cl_header = "Content-Length: 0";
        slist = curl_slist_append(slist, cl_header);
        if (slist == NULL) {
            return -1;
        }
        if (custom_hdrv == NULL) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        }

        hdrv_append(&req->req_hdrv, &req->req_hdrc, cl_header);
    }

    if (custom_hdrv != NULL) {
        rc = process_custom_headers(&slist, req, custom_hdrv, custom_hdrc);
        if (rc != CURLE_OK) {
            return rc;
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    common_opt(req);
    if (put_flag) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    } else {
        curl_easy_setopt(curl, CURLOPT_POST, 1);
    }

    char *ua = user_agent();
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        return rc;
    }

    long code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    req->code = code;
    req->ok = check_ok(code);

    if (slist != NULL) {
        curl_slist_free_all(slist);
    }
    free(ua);

    return rc;
}

static CURLcode process_custom_headers(struct curl_slist **slist, REQ_T *req,
                                       char **custom_hdrv, int custom_hdrc)
{
    for (int i = 0; i < custom_hdrc; i++) {
        *slist = curl_slist_append(*slist, custom_hdrv[i]);
        if (*slist == NULL) {
            return -1;
        }
        if (hdrv_append(&req->req_hdrv, &req->req_hdrc, custom_hdrv[i])) {
            return CURLE_OUT_OF_MEMORY;
        }
    }

    return CURLE_OK;
}

static int hdrv_append(char ***hdrv, int *hdrc, char *new)
{
    size_t current_size = *hdrc * sizeof(char*);
    char *newdup = strndup(new, strlen(new));
    if (newdup == NULL) {
        return -1;
    }

    *hdrv = realloc(*hdrv, current_size + sizeof(char*));
    if (*hdrv == NULL) {
        return -1;
    }
    (*hdrc)++;
    (*hdrv)[*hdrc - 1] = newdup;
    return 0;
}

static void common_opt(REQ_T *req)
{
    CURL *curl = req->curlhandle;
    curl_easy_setopt(curl, CURLOPT_URL, req->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, req);
}

static char *user_agent(void)
{
    struct utsname name;
    uname(&name);
    char *kernel = name.sysname;
    char *version = name.release;
    char *ua;
    asprintf(&ua, "librequests/%s %s/%s", __LIBREQ_VERS__, kernel, version);
    return ua;
}

static int check_ok(long code)
{
    if (code >= 400 || code == 0)
        return 0;
    else
        return 1;
}

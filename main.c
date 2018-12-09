#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#define URL "http://192.168.1.33:2376/"

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int docker_api_post(char* url, char* post_data, struct MemoryStruct *chunk)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    chunk->memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk->size = 0;    /* no data at this point */

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl)
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, url);

        headers = curl_slist_append(headers, "Expect:");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);

        /* some servers don't like requests that are made without a user-agent
           field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        else
        {
            /*
             * Now, our chunk.memory points to a memory block that is chunk.size
             * bytes big and contains the remote file.
             *
             * Do something nice with it!
             */

            printf("%lu bytes retrieved\n", (unsigned long)chunk->size);
            printf("Data is [%s].\n", chunk->memory);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

int docker_api_get(char* url, struct MemoryStruct *chunk)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    chunk->memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk->size = 0;    /* no data at this point */

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl)
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, url);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);

        /* some servers don't like requests that are made without a user-agent
           field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        else
        {
            /*
             * Now, our chunk.memory points to a memory block that is chunk.size
             * bytes big and contains the remote file.
             *
             * Do something nice with it!
             */

            printf("%lu bytes retrieved\n", (unsigned long)chunk->size);
            printf("Data is [%s].\n", chunk->memory);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

char* docker_create_container()
{
    char* id;
    json_object *new_obj;
    struct MemoryStruct chunk;
    docker_api_post("http://192.168.1.33:2376/containers/create", "{\"Image\": \"alpine\", \"Cmd\": [\"echo\", \"hello world\"]}", &chunk);

    new_obj = json_tokener_parse(chunk.memory);
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
    json_object* idObj;
    if (json_object_object_get_ex(new_obj, "Id", &idObj))
    {
        char* container_id = json_object_get_string(idObj);
        id = (char*)malloc((strlen(container_id) + 1) * sizeof(char));
        strcpy(id, container_id);
        printf("Container Id = %s\n", container_id);
        printf("Container Id = %s\n", id);
    }
    else
    {
        printf("Id not found.");
    }
    free(chunk.memory);
    return id;
}

int docker_start_container(char* id)
{
    char* method = "/start";
    char* containers = "containers/";
    char* url = (char*) malloc((strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1) * sizeof(char));
    sprintf(url, "%s%s%s%s", URL, containers, id, method);
    printf("Start url is %s\n", url);

    json_object *new_obj;
    struct MemoryStruct chunk;
    docker_api_post(url, "", &chunk);

    new_obj = json_tokener_parse(chunk.memory);
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

    return 0;
}

int docker_wait_container(char* id)
{
    char* method = "/wait";
    char* containers = "containers/";
    char* url = (char*) malloc((strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1) * sizeof(char));
    sprintf(url, "%s%s%s%s", URL, containers, id, method);
    printf("Wait url is %s\n", url);

    json_object *new_obj;
    struct MemoryStruct chunk;
    docker_api_post(url, "", &chunk);

    new_obj = json_tokener_parse(chunk.memory);
    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

    return 0;
}

int docker_stdout_container(char* id)
{
    char* method = "/logs?stdout=1";
    char* containers = "containers/";
    char* url = (char*) malloc((strlen(URL) + strlen(containers) + strlen(id) + strlen(method) + 1) * sizeof(char));
    sprintf(url, "%s%s%s%s", URL, containers, id, method);
    printf("Stdout url is %s\n", url);

    struct MemoryStruct chunk;
    docker_api_get(url, &chunk);

    //need to skip 8 bytes of binary junk
    printf("Output is \n%s\n", chunk.memory+8);

    return 0;
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);

    char* id;
    id = docker_create_container();

    printf("Docker container id is %s\n", id);

    docker_start_container(id);
    docker_wait_container(id);
    docker_stdout_container(id);

    curl_global_cleanup();
    return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "docker_connection_util.h"
#include "docker_containers.h"

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
        const char* container_id = json_object_get_string(idObj);
        id = (char*)malloc((strlen(container_id) + 1) * sizeof(char));
        strcpy(id, container_id);
//        printf("Container Id = %s\n", container_id);
//        printf("Container Id = %s\n", id);
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

    printf("Docker containers list");

    docker_containers_list();

    curl_global_cleanup();
    return 0;

}

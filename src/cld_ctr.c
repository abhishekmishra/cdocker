#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_ctr.h"
#include "docker_all.h"

cld_cmd_err ctr_ls_cmd_handler(void *handler_args,
                               struct array_list *options, struct array_list *args,
                               cld_command_output_handler success_handler,
                               cld_command_output_handler error_handler)
{
    int quiet = 0;
    docker_result *res;
    docker_context *ctx = get_docker_context(handler_args);
    docker_containers_list *containers;
    docker_container_list(ctx, &res, &containers, 1, 0, 1, NULL);
    handle_docker_error(res);

    if (quiet)
    {
        for (int i = 0; i < docker_containers_list_length(containers); i++)
        {
            docker_container_list_item *ctr = docker_containers_list_get_idx(
                containers, i);
            printf("%.*s\n", 12, ctr->id);
        }
    }
    else
    {
    	int num_containers = docker_containers_list_length(containers);
    	char* ctr_ls_arr [num_containers + 2][7];

        printf("%-20s\t%-40s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\n",
               "CONTAINER ID", "IMAGE", "COMMAND", "CREATED", "STATUS",
               "PORTS", "NAMES");

        ctr_ls_arr[0][0] = "CONTAINER ID";
        ctr_ls_arr[0][1] = "IMAGE";
        ctr_ls_arr[0][2] = "COMMAND";
        ctr_ls_arr[0][3] = "CREATED";
        ctr_ls_arr[0][4] = "STATUS";
        ctr_ls_arr[0][5] = "PORTS";
        ctr_ls_arr[0][6] = "NAMES";

        for (int i = 0; i < docker_containers_list_length(containers); i++)
        {
            docker_container_list_item *ctr = docker_containers_list_get_idx(
                containers, i);

            //get ports
            char ports_str[1024];
            ports_str[0] = '\0';
            for (int j = 0; j < array_list_length(ctr->ports); j++)
            {
                char port_str[100];
                docker_container_ports *ports = array_list_get_idx(ctr->ports,
                                                                   0);
                sprintf(port_str, "%ld:%ld", ports->private_port,
                        ports->public_port);
                if (j == 0)
                {
                    strcpy(ports_str, port_str);
                }
                else
                {
                    strcat(ports_str, ", ");
                    strcat(ports_str, port_str);
                }
            }

            //get created time
            time_t t = (time_t)ctr->created;
            struct tm *timeinfo = localtime(&t);
            char evt_time_str[256];
            strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);

            //get names
            char names[1024];
            names[0] = '\0';
            for (int j = 0; j < array_list_length(ctr->names); j++)
            {
                if (j == 0)
                {
                    strcpy(names, array_list_get_idx(ctr->names, j));
                }
                else
                {
                    strcat(names, ",");
                    strcat(names, array_list_get_idx(ctr->names, j));
                }
            }
            printf("%-20.*s\t%-40s\t\"%-20s\"\t%-20s\t%-20s\t%-20s\t%-20s\t\n",
                   12, ctr->id, ctr->image, ctr->command, evt_time_str,
                   ctr->status, ports_str, names);
            ctr_ls_arr[i+1][0] = ctr->id;
            ctr_ls_arr[i+1][1] = ctr->image;
            ctr_ls_arr[i+1][2] = ctr->command;
            ctr_ls_arr[i+1][3] = evt_time_str;
            ctr_ls_arr[i+1][4] = ctr->status;
            ctr_ls_arr[i+1][5] = ports_str;
            ctr_ls_arr[i+1][6] = names;
        }

        success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_ls_arr);
    }
    
    array_list_free(containers);
    return CLD_COMMAND_SUCCESS;
}

cld_command *ctr_commands()
{
    cld_command *container_command;
    if (make_command(&container_command, "container", "ctr", "Docker Container Commands",
                     NULL) == CLD_COMMAND_SUCCESS)
    {
        cld_command *ctrls_command, *syscon_command;
        if (make_command(&ctrls_command, "list", "ls",
                         "Docker Container List", &ctr_ls_cmd_handler) == CLD_COMMAND_SUCCESS)
        {
            array_list_add(container_command->sub_commands, ctrls_command);
        }
    }
    return container_command;
}

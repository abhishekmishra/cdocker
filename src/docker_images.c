/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_util.h"
#include <docker_log.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>
#include "string.h"
#include "docker_images.h"
#include "tinydir.h"

d_err_t docker_images_list(docker_context* ctx, docker_image_list** images, 
		int all, int digests, char* filter_before,
		int filter_dangling, char* filter_label, char* filter_reference,
		char* filter_since)
{
	docker_call* call;
	if (make_docker_call(&call, ctx->url, IMAGE, NULL, "json") != 0) {
		return E_ALLOC_FAILED;
	}

	json_object* filters = make_filters();
	if (filter_before != NULL)
	{
		add_filter_str(filters, "before", str_clone(filter_before));
	}
	if (filter_dangling != 0)
	{
		add_filter_str(filters, "dangling", str_clone("true"));
	}
	if (filter_label != NULL)
	{
		add_filter_str(filters, "label", str_clone(filter_label));
	}
	if (filter_reference != NULL)
	{
		add_filter_str(filters, "reference", str_clone(filter_reference));
	}
	if (filter_since != NULL)
	{
		add_filter_str(filters, "since", str_clone(filter_since));
	}
	char* filters_str = (char*)filters_to_str(filters);
	docker_call_params_add(call, "filters", filters_str);
	free(filters_str);

	if (all != 0)
	{
		docker_call_params_add(call, "all", "true");
	}

	if (digests != 0)
	{
		docker_call_params_add(call, "digests", "true");
	}

	d_err_t err = docker_call_exec(ctx, call, images);

	free_docker_call(call);
	return err;
}

void parse_status_cb(char* msg, void* cb, void* cbargs)
{
	void (*status_cb)(docker_image_create_status*,
			void*) = (void (*)(docker_image_create_status*, void*))cb;
	if (msg)
	{
		if(status_cb)
		{
			json_object* response_obj = json_tokener_parse(msg);
			char* id = get_attr_str(response_obj, "id");
			if(id == NULL)
			{
				char* message = get_attr_str(response_obj, "message");
				status_cb(NULL, cbargs);
			}
			else
			{
				char* status_msg = get_attr_str(response_obj, "status");
				char* progress = get_attr_str(response_obj, "progress");

				docker_image_create_status* status = (docker_image_create_status*)calloc(1, sizeof(docker_image_create_status));
				if (status != NULL)
				{
					status->status = status_msg;
					status->id = id;
					status->progress = progress;


					json_object* progress_detail_obj;
					if (json_object_object_get_ex(response_obj, "progressDetail", &progress_detail_obj) == 1)
					{
						long current = get_attr_long(progress_detail_obj, "current");
						long total = get_attr_long(progress_detail_obj, "total");
						docker_progress_detail* progress_detail = (docker_progress_detail*)calloc(1, sizeof(docker_progress_detail));
						if (progress_detail != NULL)
						{
							progress_detail->current = current;
							progress_detail->total = total;
							status->progress_detail = progress_detail;
						}
					}
					status_cb(status, cbargs);
				}
			}
		}
		else
		{
			docker_log_debug("Message = Empty");
		}
	}
}

//Docker Image Create commands
d_err_t docker_image_create_from_image(docker_context* ctx,
		char* from_image, char* tag, char* platform)
{
	return docker_image_create_from_image_cb(ctx, NULL, NULL,
			from_image, tag, platform);
}

d_err_t docker_image_create_from_image_cb(docker_context* ctx,
		void (*status_cb)(docker_image_create_status*, void* cbargs),
		void* cbargs, char* from_image, char* tag, char* platform)
{
	if (from_image == NULL || strlen(from_image) == 0)
	{
		return E_INVALID_INPUT;
	}

	docker_call* call;
	if (make_docker_call(&call, ctx->url, IMAGE, NULL, "create") != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_params_add(call, "fromImage", from_image);
	if (tag != NULL)
	{
		docker_call_params_add(call, "tag", tag);
	}
	if (platform != NULL)
	{
		docker_call_params_add(call, " platform", platform);
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);
	docker_call_status_cb_set(call, &parse_status_cb);
	docker_call_cb_args_set(call, status_cb);
	docker_call_client_cb_args_set(call, cbargs);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);

	free_docker_call(call);
	return ret;
}

arraylist* list_dir(char* folder_path)
{
	arraylist* paths;
	arraylist_new(&paths, &free);
//	printf("To list %s\n", folder_path);

	tinydir_dir dir;
	tinydir_open(&dir, folder_path);

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
		{
//			printf("%s", file.name);
			char* sub_dir_path = (char*) calloc(
					strlen(folder_path) + strlen(file.name) + 2, sizeof(char));
			strcpy(sub_dir_path, folder_path);
			strcat(sub_dir_path, "/");
			strcat(sub_dir_path, file.name);
			if (file.is_dir)
			{
//				printf("/\n");
				arraylist* sub_dir_ls = list_dir(sub_dir_path);
				size_t ls_count = arraylist_length(sub_dir_ls);
				for (size_t i = 0; i < ls_count; i++)
				{
					arraylist_add(paths,
							str_clone(arraylist_get(sub_dir_ls, i)));
				}
				free(sub_dir_ls);
			} else {
//				printf("\n");
			}
			arraylist_add(paths, sub_dir_path);
		}
		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	return paths;
}

void parse_build_response_cb(char* msg, void* cb, void* cbargs)
{
	void (*status_cb)(docker_build_status*,
			void*) = (void (*)(docker_build_status*, void*))cb;
	if (msg)
	{
//		printf("%s\n", msg);
		if(status_cb)
		{
			docker_build_status* status = (docker_build_status*)calloc(1, sizeof(docker_build_status));
			json_object* response_obj = json_tokener_parse(msg);
			status->stream = get_attr_str(response_obj, "stream");
			json_object* extractObj;
			int flag = 0;
			if (json_object_object_get_ex(response_obj, "aux", &extractObj)) {
				status->aux_id = (char*) get_attr_str(extractObj, "ID");
				free(extractObj);
			}

			status_cb(status, cbargs);
		}
		else
		{
			docker_log_debug("Message = Empty");
		}
	}
}


d_err_t docker_image_build_cb(docker_context* ctx,
		char* folder, char* dockerfile,
		void (*status_cb)(docker_build_status*, void* cbargs),
		void* cbargs, ...)
{
	docker_call* call;
	if (make_docker_call(&call, ctx->url, SYSTEM, NULL, "build") != 0) {
		return E_ALLOC_FAILED;
	}

	char* folder_path = ".";
	char* docker_file_name = DEFAULT_DOCKER_FILE_NAME;

	//Get folder and dockerfile name
	if (folder != NULL)
	{
		folder_path = folder;
	}

	if (dockerfile != NULL)
	{
		docker_file_name = dockerfile;
	}

	//Add dockerfile as param to the request, if not NULL

	//Create tarball from folder into a buffer
	struct archive *a;
	struct archive_entry *entry;
	//TODO check platform compatibility of stat
	struct stat st;
	char buff[8192];
	int len;
	int fd;

	size_t out_buf_len = 100*1024*1024;
	char* out_buf = (char*)calloc(out_buf_len, sizeof(char));
	size_t archive_size = 0;

	a = archive_write_new();
	archive_write_add_filter_gzip(a);
	archive_write_set_format_pax_restricted(a); // Note 1
	archive_write_open_memory(a, out_buf, out_buf_len, &archive_size);
	arraylist* sub_dir_ls = list_dir(folder_path);
	size_t ls_count = arraylist_length(sub_dir_ls);
	for (size_t i = 0; i < ls_count; i++)
	{
		char* filename = arraylist_get(sub_dir_ls, i);
//		printf("%s\n", filename);
		stat(filename, &st);
		entry = archive_entry_new(); // Note 2
		archive_entry_set_pathname(entry, filename);
		archive_entry_set_size(entry, st.st_size); // Note 3
		archive_entry_set_filetype(entry, AE_IFREG);
		archive_entry_set_perm(entry, 0644);
		archive_write_header(a, entry);
		fd = open(filename, O_RDONLY);
		len = read(fd, buff, sizeof(buff));
		while (len > 0)
		{
			archive_write_data(a, buff, len);
			len = read(fd, buff, sizeof(buff));
		}
		close(fd);
		archive_entry_free(entry);
		filename++;
	}
	archive_write_close(a); // Note 4
	archive_write_free(a); // Note 5
	free(sub_dir_ls);
	out_buf[archive_size] = '\0';

	docker_call_request_data_set(call, out_buf);
	docker_call_request_data_len_set(call, out_buf_len);
	docker_call_request_method_set(call, HTTP_POST_STR);
	docker_call_content_type_header_set(call, HEADER_TAR);
	docker_call_status_cb_set(call, &parse_build_response_cb);
	docker_call_cb_args_set(call, status_cb);
	docker_call_client_cb_args_set(call, cbargs);

	json_object *response_obj = NULL;
	docker_log_debug("Sending build context to docker daemon - %s\n", calculate_size(archive_size));

	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	json_object_put(response_obj);
	free_docker_call(call);
	return err;
}

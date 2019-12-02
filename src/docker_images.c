/*
 * clibdocker: docker_images.c
 * Created on: 31-Dec-2018
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

/**
 * List images matching the filters.
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param images list of images to be returned
 * \param all (0 indicates false, true otherwise)
 * \param digests add repo digests in return object (0 is false, true otherwise)
 * \param filter_before <image-name>[:<tag>], <image id> or <image@digest>
 * \param filter_dangling 0 is false, true otherwise.
 * \param filter_label label=key or label="key=value" of an image label
 * \param filter_reference <image-name>[:<tag>]
 * \param filter_since <image-name>[:<tag>], <image id> or <image@digest>
 * \return error code
 */
d_err_t docker_images_list(docker_context* ctx, docker_result** result,
		docker_image_list** images, int all, int digests, char* filter_before,
		int filter_dangling, char* filter_label, char* filter_reference,
		char* filter_since)
{
	char* url = create_service_url_id_method(IMAGE, NULL, "json");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	arraylist* params;
	arraylist_new(&params, (void (*)(void *)) &free_url_param);

	url_param* p;
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
	make_url_param(&p, "filters", (char *) filters_to_str(filters));
	arraylist_add(params, p);

	if (all != 0)
	{
		make_url_param(&p, "all", "true");
		arraylist_add(params, p);
	}

	if (digests != 0)
	{
		make_url_param(&p, "digests", "true");
		arraylist_add(params, p);
	}

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, images);

	arraylist_free(params);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
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
/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
d_err_t docker_image_create_from_image(docker_context* ctx,
		docker_result** result, char* from_image, char* tag, char* platform)
{
	return docker_image_create_from_image_cb(ctx, result, NULL, NULL,
			from_image, tag, platform);
}

/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform, with a progress callback
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param status_cb callback to call for updates
 * \param cbargs callback args for the upate call
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
d_err_t docker_image_create_from_image_cb(docker_context* ctx,
		docker_result** result,
		void (*status_cb)(docker_image_create_status*, void* cbargs),
		void* cbargs, char* from_image, char* tag, char* platform)
{
	if (from_image == NULL || strlen(from_image) == 0)
	{
		return E_INVALID_INPUT;
	}

	char* url = create_service_url_id_method(IMAGE, NULL, "create");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	arraylist* params;
	arraylist_new(&params, (void (*)(void*)) & free_url_param);
	url_param* p;

	make_url_param(&p, "fromImage", from_image);
	arraylist_add(params, p);
	if (tag != NULL)
	{
		make_url_param(&p, "tag", tag);
		arraylist_add(params, p);
	}
	if (platform != NULL)
	{
		make_url_param(&p, " platform", platform);
		arraylist_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post_cb(ctx, result, url, params, "", &chunk, &response_obj,
			&parse_status_cb, status_cb, cbargs);

	if ((*result)->http_error_code > 200)
	{
		return E_UNKNOWN_ERROR;
	}

	arraylist_free(params);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
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


/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageBuild
 * Build a new image from the files in a folder, with a progress callback
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param folder the folder containing the docker image build files
 * \param dockerfile name of the dockerfile. (If NULL, default "Dockerfile" is assumed)
 * \param status_cb callback to call for updates
 * \param cbargs callback args for the upate call
 * \param rest options to the build command
 * \return error code.
 */
d_err_t docker_image_build_cb(docker_context* ctx, docker_result** result,
		char* folder, char* dockerfile,
		void (*status_cb)(docker_build_status*, void* cbargs),
		void* cbargs, ...)
{
	char* url = create_service_url_id_method(SYSTEM, NULL, "build");
	char* folder_path = ".";
	char* docker_file_name = DEFAULT_DOCKER_FILE_NAME;

	arraylist* params;
	arraylist_new(&params, (void (*)(void *)) &free_url_param);

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

	//Post tarball buffer to the API
	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	out_buf[archive_size] = '\0';
	printf("Sending build context to docker daemon - %s\n", calculate_size(archive_size));
	docker_api_post_buf_cb_w_content_type(ctx, result, url, params, out_buf, out_buf_len, &chunk, &response_obj,
			&parse_build_response_cb, status_cb, cbargs, HEADER_TAR);

	if ((*result)->http_error_code > 200)
	{
		return E_UNKNOWN_ERROR;
	}

	return E_SUCCESS;
}

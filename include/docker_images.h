/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * \file docker_images.h
 * \brief Docker Images API
 */

#ifndef SRC_DOCKER_IMAGES_H_
#define SRC_DOCKER_IMAGES_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <arraylist.h>
#include "docker_connection_util.h"
#include "docker_result.h"
#include "docker_util.h"

#define DEFAULT_DOCKER_FILE_NAME "Dockerfile"

typedef json_object											docker_image;
#define docker_image_id_get(img)							get_attr_str((json_object*)img, "Id")
#define docker_image_container_get(img)						get_attr_str((json_object*)img, "Container")
#define docker_image_comment_get(img)						get_attr_str((json_object*)img, "Comment")
#define docker_image_os_get(img)							get_attr_str((json_object*)img, "Os")
#define docker_image_architecture_get(img)					get_attr_str((json_object*)img, "Architecture")
#define docker_image_parent_get(img)						get_attr_str((json_object*)img, "Parent")
#define docker_image_docker_version_get(img)				get_attr_str((json_object*)img, "DockerVersion")
#define docker_image_virtual_size_get(img)					get_attr_long_long((json_object*)img, "VirtualSize")
#define docker_image_size_get(img)							get_attr_long_long((json_object*)img, "Size")
#define docker_image_author_get(img)						get_attr_str((json_object*)img, "Author")
#define docker_image_created_get(img)						get_attr_unsigned_long((json_object*)img, "Created")
#define docker_image_repo_tags_get(img)						get_attr_json_object((json_object*)img, "RepoTags")
#define docker_image_repo_tags_length(img)					json_object_array_length(docker_image_repo_tags_get(img))
#define docker_image_repo_tags_get_idx(img, i)				(char*)json_object_get_string(json_object_array_get_idx(docker_image_repo_tags_get(img), i))
#define docker_image_repo_digests_get(img)					get_attr_json_object((json_object*)img, "RepoDigests")
#define docker_image_repo_digests_length(img)				json_object_array_length(docker_image_repo_digests_get(img))
#define docker_image_repo_digests_get_idx(img, i)			(char*)json_object_get_string(json_object_array_get_idx(docker_image_repo_digests_get(img), i))

typedef json_object											docker_image_list;
#define free_docker_image_list(image_ls)					json_object_put(image_ls)
#define docker_image_list_length(image_ls)					json_object_array_length(image_ls)
#define docker_image_list_get_idx(image_ls, i)				(docker_image*) json_object_array_get_idx(image_ls, i)

/**
 * List images matching the filters.
 *
 * \param ctx docker context
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
MODULE_API d_err_t docker_images_list(docker_context* ctx,
		docker_image_list** images, int all, int digests, char* filter_before,
		int filter_dangling, char* filter_label, char* filter_reference,
		char* filter_since);

typedef struct docker_progress_detail_t {
	long current;
	long total;
} docker_progress_detail;

typedef struct docker_image_create_status_t {
	char* status;
	char* id;
	char* progress;
	docker_progress_detail* progress_detail;
} docker_image_create_status;

typedef struct docker_build_status_t {
	char* stream;
	char* aux_id;
} docker_build_status;

//Docker Image Create commands
/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform
 *
 * \param ctx docker context
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
MODULE_API d_err_t docker_image_create_from_image(docker_context* ctx,
		char* from_image, char* tag, char* platform);

/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform, with a progress callback
 *
 * \param ctx docker context
 * \param status_cb callback to call for updates
 * \param cbargs callback args for the upate call
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
MODULE_API d_err_t docker_image_create_from_image_cb(docker_context* ctx,
		void (*status_cb)(docker_image_create_status*, void* cbargs),
		void* cbargs, char* from_image, char* tag, char* platform);

//error_t docker_image_create_from_src(docker_context* ctx, docker_result** res, char* from_src, char* repo, char* tag, char* platform);

/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageBuild
 * Build a new image from the files in a folder, with a progress callback
 *
 * \param ctx docker context
 * \param folder the folder containing the docker image build files (if NULL, default is current directory)
 * \param dockerfile name of the dockerfile. (If NULL, default "Dockerfile" is assumed)
 * \param status_cb callback to call for updates
 * \param cbargs callback args for the upate call
 * \param rest options to the build command
 * \return error code.
 */
MODULE_API d_err_t docker_image_build_cb(docker_context* ctx, 
		char* folder, char* dockerfile,
		void (*status_cb)(docker_build_status*, void* cbargs),
		void* cbargs, ...);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_IMAGES_H_ */

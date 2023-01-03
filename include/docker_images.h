/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of clibdocker.
 *
 * clibdocker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) 
 * any later version.
 *
 * clibdocker is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public 
 * License along with clibdocker. 
 * If not, see <https://www.gnu.org/licenses/>.
 *
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

#include <coll_arraylist.h>
#include "docker_connection_util.h"
#include "docker_result.h"
#include "docker_util.h"

#define DEFAULT_DOCKER_FILE_NAME "Dockerfile"

/**
 * @brief The Docker Image object
 *
 * Represents a docker image object returned in the Docker images API
 * call results. Depending on result, some or all values maybe
 * unavailable.
 *
 * Use the getters defined in this API to access the members of
 * the Docker Image object.
 *
 * All getters are of the form \c docker_image_<member>_get
 * For example,
 * to get the \b id of the image use \c docker_image_id_get(img)
 */
typedef json_object											docker_image;

/**
 * @brief get the docker image id
 *
 * @param img docker image
 * @return char* docker image id
 */
#define docker_image_id_get(img)							get_attr_str((json_object*)img, "Id")

/**
 * @brief get the docker image container name
 *
 * @param img docker image
 * @return char* container
 */
#define docker_image_container_get(img)						get_attr_str((json_object*)img, "Container")

/**
 * @brief get the docker image comment
 *
 * @param img docker image
 * @return char* comment text
 */
#define docker_image_comment_get(img)						get_attr_str((json_object*)img, "Comment")

/**
 * @brief get the docker image operating system
 *
 * @param img docker image
 * @return char* operating system
 */
#define docker_image_os_get(img)							get_attr_str((json_object*)img, "Os")

/**
 * @brief get the docker image architecture
 *
 * @param img docker image
 * @return char* architecture
 */
#define docker_image_architecture_get(img)					get_attr_str((json_object*)img, "Architecture")

/**
 * @brief get the docker image parent image name
 *
 * @param img docker image
 * @return char* parent
 */
#define docker_image_parent_get(img)						get_attr_str((json_object*)img, "Parent")

/**
 * @brief get the docker version of the docker image
 *
 * @param img docker image
 * @return char* docker version
 */
#define docker_image_docker_version_get(img)				get_attr_str((json_object*)img, "DockerVersion")

/**
 * @brief get the virtual size of the docker image
 *
 * @param img docker image
 * @return long long virtual size
 */
#define docker_image_virtual_size_get(img)					get_attr_long_long((json_object*)img, "VirtualSize")

/**
 * @brief get the size of the docker image
 *
 * @param img docker image
 * @return long long size
 */
#define docker_image_size_get(img)							get_attr_long_long((json_object*)img, "Size")

/**
 * @brief get the author of the docker image
 *
 * @param img docker image
 * @return char* author
 */
#define docker_image_author_get(img)						get_attr_str((json_object*)img, "Author")

/**
 * @brief get the created datetime of the docker image
 *
 * @param img docker image
 * @return unsigned long created datetime (epoch)
 */
#define docker_image_created_get(img)						get_attr_unsigned_long((json_object*)img, "Created")

/**
 * @brief get the docker image repo tags
 *
 * Use the \c docker_image_repo_tags_length and
 * \c docker_image_repo_tags_get_idx to get the
 * individual repo tags.
 *
 * @param img docker image
 * @return json_object* repo tags
 */
#define docker_image_repo_tags_get(img)						get_attr_json_object((json_object*)img, "RepoTags")

/**
 * @brief get the length of the repo tags array
 * for the given docker image
 *
 * @param img docker image
 * @return int length of the repo tags array
 */
#define docker_image_repo_tags_length(img)					json_object_array_length(docker_image_repo_tags_get(img))

/**
 * @brief get the ith element of the repo tags array
 * for the given docker image
 *
 * @param img docker image
 * @param i index
 * @return char* ith repo tag
 */
#define docker_image_repo_tags_get_idx(img, i)				(char*)json_object_get_string(json_object_array_get_idx(docker_image_repo_tags_get(img), i))

/**
 * @brief get the docker image repo digests
 *
 * Use the \c docker_image_repo_digests_length and
 * \c docker_image_repo_digests_get_idx to get the
 * individual repo digests.
 *
 * @param img docker image
 * @return json_object* repo digests
 */
#define docker_image_repo_digests_get(img)					get_attr_json_object((json_object*)img, "RepoDigests")

/**
 * @brief get the length of the repo digests array
 * for the given docker image
 *
 * @param img docker image
 * @return int length of the repo digests array
 */
#define docker_image_repo_digests_length(img)				json_object_array_length(docker_image_repo_digests_get(img))

/**
 * @brief get the ith element of the repo digests array
 * for the given docker image
 *
 * @param img docker image
 * @param i index
 * @return char* ith repo digest
 */
#define docker_image_repo_digests_get_idx(img, i)			(char*)json_object_get_string(json_object_array_get_idx(docker_image_repo_digests_get(img), i))

/**
 * @brief Docker Image List object
 *
 * This is internally represented as a json array object,
 * which was parsed from a Docker Image API call response.
 */
typedef json_object											docker_image_list;

/**
 * @brief Free the docker image list object
 *
 * @param image list
 */
#define free_docker_image_list(image_ls)					json_object_put(image_ls)

/**
 * @brief Get the list of the docker image list
 *
 * @param image_ls image list
 * @return int image list length
 */
#define docker_image_list_length(image_ls)					json_object_array_length(image_ls)

/**
 * @brief Get the image at index \b i from the image list
 *
 * @param image_ls image list
 * @param i index
 * @return docker_image image object
 */
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

/**
 * @brief Provides progress detail for docker image creation process.
 */
typedef struct docker_progress_detail_t {
	long current;		///< current value of progress bar
	long total;			///< total value of progress bar
} docker_progress_detail;

/**
 * @brief Docker Image Creation Status is returned when creating a
 * docker image. It is useful for a user facing application to show
 * creation progress. 
 */
typedef struct docker_image_create_status_t {
	char* status;								///< a status message from the docker api
	char* id;									///< image id
	char* progress;								///< progress description
	docker_progress_detail* progress_detail;	///< progress detail (provides current and total steps)
} docker_image_create_status;

/**
 * @brief Docker Build Status is used to provide a status
 * update of the docker image build status. 
 */
typedef struct docker_build_status_t {
	char* stream;			///< the status stream
	char* aux_id;			///< the aux id
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
 * \param ... options to the build command
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

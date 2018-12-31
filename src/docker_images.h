/*
 * clibdocker: docker_images.h
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

#ifndef SRC_DOCKER_IMAGES_H_
#define SRC_DOCKER_IMAGES_H_

#include "docker_connection_util.h"
#include "docker_result.h"
#include "docker_util.h"

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
error_t docker_image_create_from_image(docker_context* ctx, docker_result** result,
		char* from_image, char* tag, char* platform);

//error_t docker_image_create_from_src(docker_context* ctx, docker_result** res, char* from_src, char* repo, char* tag, char* platform);

#endif /* SRC_DOCKER_IMAGES_H_ */

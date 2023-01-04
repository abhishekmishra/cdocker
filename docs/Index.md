# clibdocker API Documentation  {#mainpage}

`clibdocker` is a **C** language implementation of the [Docker Engine API v1.39](https://docs.docker.com/engine/api/v1.39/). The C API organization is similar to the reference JSON API specification.

## API Coverage
API Coverage is tracked at a granular level in a separate document [Docker API Coverage](APICoverage.md).

## API Organization
The Docker API is organized around the objects managed by the docker runtime. There is one header file, with one set of functions per Docker Object in this API.

| Docker API Object   | Header File                                                |
|---------------------|------------------------------------------------------------|
| Docker Result       | [docker_result.h](@ref docker_result.h)                    |
| Docker Connection   | [docker_connection_util.h](@ref docker_connection_util.h)  |
| Docker Containers   | [docker_containers.h](@ref docker_containers.h)            |
| Docker Images       | [docker_images.h](@ref docker_images.h)                    |
| Docker Networks     | [docker_networks.h](@ref docker_networks.h)                |
| Docker System       | [docker_system.h](@ref docker_system.h)                    |
| Docker Volumes      | [docker_volumes.h](@ref docker_volumes.h)                  |
| Docker Ignore       | [docker_ignore.h](@ref docker_ignore.h)                    |
| Docker Log          | [docker_log.h](@ref docker_log.h)                          |

### Single Header File

The [docker_all.h](@ref docker_all.h) file includes all the other important header files.

## Goals
 * Simplicity: Easy to understand and use API
 * Safety: Good memory management, Documentation ensures warnings about safe usage, thread safety.
 * Efficiency: Keep calls lightweight to enable heavy usage.
 
## Limitations
This method calls in the API are synchronous, which means the calling thread will block for response.

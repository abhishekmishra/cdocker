# clibdocker: C API for Docker
### :warning: This is currently work in progress.

## Goals
 * Simplicity: Easy to understand and use API
 * Safety: Good memory management, Documentation ensures warnings about safe usage, thread safety.
 * Efficiency: Keep calls lightweight to enable heavy usage.
 
## Limitations
This method calls in the API are synchronous, which means the calling thread will block for response.
At some point I will write corresponding asynchronous API endpoints (but not planned for now).

## API Completeness

(Measuring completeness against current \[v1.39\] Docker API documentation at https://docs.docker.com/engine/api/v1.39/)

----
### Docker Context
| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|Docker Result        |            :ok: |           :ok:|   :x: |                          |
|Docker Context       |            :ok: |           :x: |   :x: |                          |
|HTTP Support         |            :ok: |           :x: |   :x: |                          |
|Unix Socket Support  |            :ok: |           :x: |   :x: |                          |
|Authentication       |             :x: |           :x: |   :x: |                          |


### Containers API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List Containers      |            :ok: |           :x: |  :ok: |                          |
|Create Container     |            :ok: |           :x: |  :ok: | Only two params done.    |
|Inspect Containers   |             :x: |           :x: |   :x: |                          |
|List Processes       |            :ok: |           :x: |   :x: |                          |
|Container Logs       |            :ok: |          :ok: |  :ok: | Socket based follow not implemented |
|Container FS Changes |            :ok: |          :ok: |  :ok: |                          |
|Export Container     |             :x: |           :x: |   :x: |                          |
|Resource Usage Stats |             :x: |           :x: |   :x: |                          |
|Resize TTY           |             :x: |           :x: |   :x: |                          |
|Start                |            :ok: |          :ok: |  :ok: | test detachKeys                         |
|Stop                 |            :ok: |          :ok: |  :ok: |                          |
|Restart              |            :ok: |          :ok: |  :ok: |                          |
|Kill                 |            :ok: |          :ok: |  :ok: |                          |
|Update               |             :x: |           :x: |   :x: |                          |
|Rename               |            :ok: |          :ok: |   :x: |                          |
|Pause                |            :ok: |          :ok: |  :ok: |                          |
|Unpause              |            :ok: |          :ok: |  :ok: |                          |
|Attach               |             :x: |           :x: |   :x: |                          |
|Attach via Websocket |             :x: |           :x: |   :x: |                          |
|Wait                 |            :ok: |          :ok: |  :ok: | wait status code      |
|Remove               |             :x: |           :x: |   :x: |                          |
|Info about files     |             :x: |           :x: |   :x: |                          |
|Get archive of fs    |             :x: |           :x: |   :x: |                          |
|Extract archive      |             :x: |           :x: |   :x: |                          |
|Delete stopped       |             :x: |           :x: |   :x: |                          |

### Images API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List Images          |             :x: |           :x: |   :x: |                          |
|Build Image          |             :x: |           :x: |   :x: |                          |
|Delete builder cache |             :x: |           :x: |   :x: |                          |
|Create Image         |             :x: |           :x: |   :x: |                          |
|Inspect Image        |             :x: |           :x: |   :x: |                          |
|Get history          |             :x: |           :x: |   :x: |                          |
|Push                 |             :x: |           :x: |   :x: |                          |
|Tag                  |             :x: |           :x: |   :x: |                          |
|Remove               |             :x: |           :x: |   :x: |                          |
|Search               |             :x: |           :x: |   :x: |                          |
|Delete unused        |             :x: |           :x: |   :x: |                          |
|Create from container|             :x: |           :x: |   :x: |                          |
|Export image         |             :x: |           :x: |   :x: |                          |
|Export several       |             :x: |           :x: |   :x: |                          |
|Import               |             :x: |           :x: |   :x: |                          |

### Network API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List networks        |             :x: |           :x: |   :x: |                          |
|Inspect a network    |             :x: |           :x: |   :x: |                          |
|Remove				  |             :x: |           :x: |   :x: |                          |
|Create 		      |             :x: |           :x: |   :x: |                          |
|Connect container to network   |             :x: |           :x: |   :x: |                          |
|Disconnect container from network   |             :x: |           :x: |   :x: |                          |
|Delete unused        |             :x: |           :x: |   :x: |                          |

### Volumes API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List		          |             :x: |           :x: |   :x: |                          |
|Create               |             :x: |           :x: |   :x: |                          |
|Inspect			  |             :x: |           :x: |   :x: |                          |
|Remove 		      |             :x: |           :x: |   :x: |                          |
|Delete unused        |             :x: |           :x: |   :x: |                          |

### Exec API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|Create exec instance |             :x: |           :x: |   :x: |                          |
|Start                |             :x: |           :x: |   :x: |                          |
|Resize	    		  |             :x: |           :x: |   :x: |                          |
|Inspect 		      |             :x: |           :x: |   :x: |                          |

### System API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|Check Auth Config    |             :x: |           :x: |   :x: |                          |
|Get System Info      |             :x: |           :x: |   :x: |                          |
|Get Version          |             :x: |           :x: |   :x: |                          |
|Ping   	      |            :ok: |          :ok: |  :ok: |                          |
|Monitor events       |             :x: |           :x: |   :x: |                          |
|Get Data Usage Info  |             :x: |           :x: |   :x: |                          |

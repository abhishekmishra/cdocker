## API Coverage

(Measuring coverage against current \[v1.39\] Docker API documentation at https://docs.docker.com/engine/api/v1.39/)

----
### Docker Context
| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|Docker Result        |            :ok: |           :ok:|   :ok:|                          |
|Docker Context       |            :ok: |           :ok:|   :ok:|                          |
|HTTP Support         |            :ok: |           :ok:|   :ok:|                          |
|Unix Socket Support  |            :ok: |           :ok:|   :ok:|                          |
|Authentication       |             :x: |           :x: |   :x: |                          |


### Containers API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List Containers      |            :ok: |          :ok: |  :ok: |                          |
|Create Container     |            :ok: |           :x: |  :ok: | Only two params done.    |
|Inspect Containers   |             :x: |           :x: |   :x: |                          |
|List Processes       |            :ok: |           :x: |   :x: |                          |
|Container Logs       |            :ok: |          :ok: |  :ok: | Socket based follow not implemented |
|Container FS Changes |            :ok: |          :ok: |  :ok: |                          |
|Export Container     |             :x: |           :x: |   :x: |                          |
|Resource Usage Stats |            :ok: |          :ok: |  :ok: | mem stats summary only, details missing, and blkio tbd. |
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
|List Images          |            :ok: |          :ok: |  :ok: |                          |
|Build Image          |             :x: |           :x: |   :x: |                          |
|Delete builder cache |             :x: |           :x: |   :x: |                          |
|Create Image         |            :ok: |          :ok: |  :ok: | Create from source is pending                         |
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
|List networks        |            :ok: |          :ok: |  :ok: |                          |
|Inspect a network    |            :ok: |          :ok: |  :ok: |                          |
|Remove				  |             :x: |           :x: |   :x: |                          |
|Create 		      |             :x: |           :x: |   :x: |                          |
|Connect container to network   |             :x: |           :x: |   :x: |                          |
|Disconnect container from network   |             :x: |           :x: |   :x: |                          |
|Delete unused        |             :x: |           :x: |   :x: |                          |

### Volumes API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List		          |            :ok: |          :ok: |  :ok: |                          |
|Create               |            :ok: |          :ok: |  :ok: |                          |
|Inspect			  |            :ok: |          :ok: |  :ok: |                          |
|Remove 		      |            :ok: |          :ok: |  :ok: |                          |
|Delete unused        |            :ok: |          :ok: |  :ok: |                          |

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
|Get System Info      |            :ok: |          :ok: |  :ok: | Only few attrs deserialized, rest tbd                          |
|Get Version          |            :ok: |          :ok: |  :ok: |                          |
|Ping   	          |            :ok: |          :ok: |  :ok: |                          |
|Monitor events       |            :ok: |          :ok: |  :ok: | Callback mode for continous listening.                         |
|Get Data Usage Info  |             :x: |           :x: |   :x: |                          |

## Docker API Call Mechanism

Anatomy of a basic docker call has the following data structures involved:

1. A URL struct with the following:
    * The base url (obtained from docker context) 
    * method url, parameterized for e.g. /containers/{id}/json
    * url parameters (dictionary like)
2. A request body (for POST calls):
    * request parameters (dictionary like)
3. Internal storage for CURL request/response data
    * Associated with a specific handler function which parses response
    * The handler function creates either a struct specific to the call or
        a dictionary like object containing the response.
    * This struct or dictionary is returned to the client.
    * Some calls return the struct/dict, and some provide them via a callback, if the call has mutliple responses.
4. Since the internal raw request/response object is cleaned-up, any caller
    that needs access to this info, can set a global API level callback on the docker context to handler the result object, for e.g. to get the start time,
    end time, raw request string, raw response string, and the URL. This callback will be called before the bookkeeping data is cleaned up.

NOTE: All internal datastructures except the response objects are owned/freed by the API call. Only the response struct/dict is returned. It is the responsibility of caller to free this object when it is no longer used.


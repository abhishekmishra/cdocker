# clibdocker: C API for Docker
### :warning: This is currently work in progress.

 ## Goals
 * Simplicity: Easy to understand and use API
 * Safety: Good memory management, Documentation ensures warnings about safe usage, thread safety.
 * Efficiency: Keep calls lightweight to enable heavy usage.

## API Completeness

(Measuring completeness against current \[v1.39\] Docker API documentation at https://docs.docker.com/engine/api/v1.39/)

----
### Containers API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List Containers      |             [ ] |           [ ] |   [ ] |                          |
|Create Container     |             [ ] |           [ ] |   [ ] |                          |
|Inspect Containers   |             [ ] |           [ ] |   [ ] |                          |
|List Processes       |             [ ] |           [ ] |   [ ] |                          |
|Container Logs       |             [ ] |           [ ] |   [ ] |                          |
|Container FS Changes |             [ ] |           [ ] |   [ ] |                          |
|Export Container     |             [ ] |           [ ] |   [ ] |                          |
|Resource Usage Stats |             [ ] |           [ ] |   [ ] |                          |
|Resize TTY           |             [ ] |           [ ] |   [ ] |                          |
|Start                |             [ ] |           [ ] |   [ ] |                          |
|Stop                 |             [ ] |           [ ] |   [ ] |                          |
|Restart              |             [ ] |           [ ] |   [ ] |                          |
|Kill                 |             [ ] |           [ ] |   [ ] |                          |
|Update               |             [ ] |           [ ] |   [ ] |                          |
|Rename               |             [ ] |           [ ] |   [ ] |                          |
|Pause                |             [ ] |           [ ] |   [ ] |                          |
|Unpause              |             [ ] |           [ ] |   [ ] |                          |
|Attach               |             [ ] |           [ ] |   [ ] |                          |
|Attach via Websocket |             [ ] |           [ ] |   [ ] |                          |
|Wait                 |             [ ] |           [ ] |   [ ] |                          |
|Remove               |             [ ] |           [ ] |   [ ] |                          |
|Info about files     |             [ ] |           [ ] |   [ ] |                          |
|Get archive of fs    |             [ ] |           [ ] |   [ ] |                          |
|Extract archive      |             [ ] |           [ ] |   [ ] |                          |
|Delete stopped       |             [ ] |           [ ] |   [ ] |                          |
### Images API

| Method              | Implementation  | Documentation | Tests | TODOs                    |
|---------------------|-----------------|---------------|-------|--------------------------|
|List Images          |             [ ] |           [ ] |   [ ] |                          |
|Build Image          |             [ ] |           [ ] |   [ ] |                          |
|Delete builder cache |             [ ] |           [ ] |   [ ] |                          |
|Create Image         |             [ ] |           [ ] |   [ ] |                          |
|Inspect Image        |             [ ] |           [ ] |   [ ] |                          |
|Get history          |             [ ] |           [ ] |   [ ] |                          |
|Push                 |             [ ] |           [ ] |   [ ] |                          |
|Tag                  |             [ ] |           [ ] |   [ ] |                          |
|Remove               |             [ ] |           [ ] |   [ ] |                          |
|Search               |             [ ] |           [ ] |   [ ] |                          |
|Delete unused        |             [ ] |           [ ] |   [ ] |                          |
|Create from container|             [ ] |           [ ] |   [ ] |                          |
|Export image         |             [ ] |           [ ] |   [ ] |                          |
|Export several       |             [ ] |           [ ] |   [ ] |                          |
|Import               |             [ ] |           [ ] |   [ ] |                          |
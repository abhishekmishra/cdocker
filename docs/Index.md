# clibdocker API Documentation  {#mainpage}

`clibdocker` is a **C** language implementation of the [Docker Engine API v1.39](https://docs.docker.com/engine/api/v1.39/). The C API organization is similar to the reference JSON API specification.

## Goals
 * Simplicity: Easy to understand and use API
 * Safety: Good memory management, Documentation ensures warnings about safe usage, thread safety.
 * Efficiency: Keep calls lightweight to enable heavy usage.
 
## Limitations
This method calls in the API are synchronous, which means the calling thread will block for response.

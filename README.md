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

See here - [APICoverage.md](./docs/APICoverage.md)

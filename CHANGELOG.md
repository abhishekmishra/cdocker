# CHANGELOG
This file documents major changes in every release of the project. The project follows [Semantic Versioning](https://semver.org/). There is a section for each release - which lists major changes made in the release.

**0.1.0-alpha.0**  2023-01-14 Abhishek Mishra  <abhishekmishra3@gmail.com>
- This is the first ever official release of `clibdocker`, although I've been
  working on it off and on for several years now. But development had stalled
  for more than a year and I picked it up again in late 2022.
- There are too many *changes* to enumerate. 
- This version of the library works with **Docker Engine API version 1.39**.
- I've maintained a document which lists coverage of the C API w.r.t the
  Docker API. This is a good place to start to look for all the features
  available in the release.
- The library C API documentation is generated using doxygen and is available
  both online and can be generated offline using the `make docs` command.
- There are simple validation tests implemented in the `./test` folder. This
  provided a good overview of the usage of the API.
- The [CLD: Docker CLI](https://github.com/abhishekmishra/CLD) project uses
  this library to provide functionality similar to the _official_ Docker CLI.
  This is also a good place to look for usage of the API.
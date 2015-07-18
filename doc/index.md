QHttpEngine provides a simple set of classes for developing HTTP server applications in Qt.

The design goals of QHttpEngine include:

- **Reliability** - extremely exhaustive test suite ensures that bugs are caught as soon as they are introduced
- **Flexibility** - classes are designed for easy integration with existing Qt code
- **Simplicity** - everything is intentionally kept as simple as possible
- **Usability** - documentation is provided for all classes

All of QHttpEngine's functionality is included in a single monolithic library.

## Requirements

QHttpEngine has been tested on the following combinations of compiler and operating system:

- Visual C++ 2013 on 32 and 64-bit editions of Windows
- Clang on Mac OS X
- g++ on i386, amd64, and ARM builds of Linux

QHttpEngine is designed in a portable way, so it may run on other compilers and operating systems than the ones listed above. However, the list represents the combinations that are actively tested and officially supported.

## Building

QHttpEngine uses CMake for building the library. The library recognizes four options during configuration, all of which are disabled by default (the library is built as a shared library):

- `BUILD_STATIC` - build and link a static library instead of a shared library
- `BUILD_DOC` - (requires Doxygen) generates documentation from the comments in the source code
- `BUILD_EXAMPLES` - builds the sample applications that demonstrate how to use QHttpEngine
- `BUILD_TESTS` - build the test suite

It is also possible to override installation directories by customizing the `BIN_INSTALL_DIR`, `LIB_INSTALL_DIR`, `INCLUDE_INSTALL_DIR`, `CMAKECONFIG_INSTALL_DIR`, `DOC_INSTALL_DIR`, and `EXAMPLES_INSTALL_DIR` variables.

## Usage

Serving static files from a directory is as simple as creating an instance of QHttpServer and QFilesystemHandler:

@code
QFilesystemHandler handler("/var/www");
QHttpServer server(&handler);
server.listen();
@endcode

QHttpEngine can also be used to easily add an HTTP API to an existing application by deriving a class from QObjectHandler. Each signal that takes a single QVariantMap parameter and returns a QVariantMap can be directly invoked by using its name in the request path. For example:

@code
class ApiHandler : public QObjectHandler
{
    Q_OBJECT
public slots:
    QVariantMap doSomething(const QVariantMap &);
};
@endcode

A client can send a POST request to `/doSomething` with JSON data and receive the response from the slot directly as JSON data. The chatserver example provides a demonstration of this.

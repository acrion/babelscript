// This header is used in case the babelscript source files are compiled
// via go tool cgo. In this case, we link statically, so BABELSCRIPT_EXPORT
// is an empty string. When we link dynamically, this header is generated
// by CMake in path that is preferred over the path of this file.

#define BABELSCRIPT_EXPORT
#define PROVIDE_C_INTERFACE

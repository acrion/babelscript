// This header is used in case the babelscript source files are compiled
// via go tool cgo. When we compile via CMake instead, this header is generated
// by CMake in a directory that is preferred over the directory of this file.

#define BABELSCRIPT_EXPORT // link statically
#define PROVIDE_C_INTERFACE

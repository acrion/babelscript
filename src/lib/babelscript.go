package babelscript

// #cgo darwin CXXFLAGS: -std=gnu++1z -fPIC -I/Users/stefan/git/boost
// #cgo linux CXXFLAGS: -std=gnu++1z -fPIC -I/usr/include
// #cgo !linux,!darwin CXXFLAGS: -std=gnu++1z -fPIC -IC:/local/boost_1_70_0
// #cgo darwin LDFLAGS: -lstdc++fs -L/Users/stefan/git/boost/stage/lib -lboost_serialization
// void babelscript_run();
// void babelscript_process_c(const char* line, size_t len);
// static void babelscript_process(_GoString_ line)
// {
//   babelscript_process_c(line.p, line.n);
// }
import "C"

func Process(cmd string) {
	C.babelscript_process(cmd)
}

func Run() {
	C.babelscript_run()
}

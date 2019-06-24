package babelscript

// #cgo CXXFLAGS: -std=gnu++1z -fPIC -I/media/psf/Home/git/boost
// #cgo LDFLAGS: -lstdc++fs
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

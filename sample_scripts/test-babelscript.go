package main

import "babelscript.org/src/lib"
//import "encoding/json"

func main() {

  babelscript.Process("peter \"is ancestor of\" paul")
  babelscript.Process("paul is ancestor of pius")
  babelscript.Run();
}



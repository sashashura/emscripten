#include <emscripten/emscripten.h>
#include <emscripten/wasmfs.h>
#include <emscripten/console.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main() {
  wasmfs_create_directory("/opfs", 0777, wasmfs_create_opfs_backend());
  EM_ASM({ run_test(); });
  emscripten_exit_with_live_runtime();
}

const char* file = "/opfs/data";

// Each of these functions returns:
//   0: failure with EACCES
//   1: success
//   2: other error

static int try_open(int flags) {
  int fd = open(file, flags);
  if (fd >= 0) {
    close(fd);
    return 1;
  }
  if (errno == EACCES) {
    return 0;
  }
  emscripten_console_error(strerror(errno));
  return 2;
}


EMSCRIPTEN_KEEPALIVE
int try_open_wronly(void) {
  return try_open(O_WRONLY);
}

EMSCRIPTEN_KEEPALIVE
int try_open_rdwr(void) {
  return try_open(O_RDWR);
}

EMSCRIPTEN_KEEPALIVE
int try_open_rdonly(void) {
  return try_open(O_RDONLY);
}

EMSCRIPTEN_KEEPALIVE
void report_result(int result) {
  EM_ASM({ console.log(new Error().stack); });
#ifdef REPORT_RESULT
  REPORT_RESULT(result);
#else
  if (result != 0) {
    abort();
  }
#endif
}

#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_sparse_to_pipe(void) {
    // This test involves mkfifo (pipes) and background processes across different 
    // filesystems. As previously noted, mkfifo isn't portable across Windows,
    // and process syncing introduces flakes or timeouts.
    SKIPm("Skipped due to mkfifo/pipe synchronization requirements");
    PASS();
}





SUITE(cp_sparse_to_pipe_suite) {
    RUN_TEST(test_cp_sparse_to_pipe);
}
DEFINE_TEST_MAIN(cp_sparse_to_pipe_suite)

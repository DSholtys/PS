#include <stdio.h>
#include <Windows.h>
#include <inttypes.h>

struct ThreadParams {
    char* msg;
    int count;
};
DWORD WINAPI my_thread_fun(LPVOID param);
//unsigned int my_threa_fun(void* param)
int main() {
    struct ThreadParams params = {
        .msg = "Hello world",
        .count = 5
    };
    DWORD thread_id;
    HANDLE thread = CreateThread(NULL, 0, my_thread_fun, &params, 0, NULL);
    if (!thread) {
        fprintf(stderr, "CreateThread failed\n");
        return 1;
    }
    if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0) {
        fprintf(stderr, "WaitForSingleObject failed\n");
        return 1;
    }
    DWORD thread_code;

    if (GetExitCodeThread(thread, &thread_code)) {
        fprintf(stderr,"Error getting thread exit code\n");
        return 1;
    }
    return 0;
    printf("Main: My_thread_id=%u\n", thread_id);
}

DWORD WINAPI my_thread_fun(LPVOID param) {
    struct ThreadParams* data = (struct ThreadParams*)param;
    if (!data) {
        return 0;
    }
    for (int i = 0; i < data->count; i++) {
        printf("Message = %s\n", data->msg);
    }

    printf("Main: My_thread_id=%u\n", GetCurrentThreadId());
    return 0;
}

#include <stdio.h>
#include <Windows.h>
#include <inttypes.h>

struct ThreadParams {
    char* msg;
    int count;
};
#define CNT 5
DWORD WINAPI my_thread_fun(LPVOID param);
//unsigned int my_threa_fun(void* param)

CRITICAL_SECTION cs;  // Оголошення критичної секції

int main() {

    struct ThreadParams params = {
        .msg = "Hello world",
        .count = 5
    };
    volatile int word_count = 0;
    HANDLE threads[CNT];
    DWORD thread_id;
    InitializeCriticalSection(&cs);
    for (int i = 0; i < CNT;i++) {
        threads[i] = CreateThread(NULL, 0, my_thread_fun, &word_count, 0, NULL);
        if (!threads[i]) {
            fprintf(stderr, "CreateThread failed\n");
            return 1;
        }
    }
    if (WaitForMultipleObjects(CNT, threads, TRUE, INFINITE) != WAIT_OBJECT_0) {
        fprintf(stderr, "WaitForSingleObject failed\n");
        return 1;
    }

    for (int i = 0; i < CNT; i++) {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&cs);

}

DWORD WINAPI my_thread_fun(LPVOID param) {
    int* count = (int*)param;
    if (!count) {
        return 1;
    }
    for (int i = 0; i < 100; i++) {
        EnterCriticalSection(&cs);
        (*count)++;
        LeaveCriticalSection(&cs);
    }
}

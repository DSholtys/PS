#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>

DWORD WINAPI my_thread_fun(LPVOID param);

struct BankAccount {
    int balance;
    HANDLE mutex;
};

struct BankTransfer {
    struct BankAccount* from;
    struct BankAccount* to;
    int amount;
};
struct BankAccount* create_bank_account();
void delete_bank_account(struct BankAccount* acc);

int main() {
    struct BankAccount* acc_a = create_bank_account();
    struct BankAccount* acc_b = create_bank_account();

    struct BankTransfer transfer_1 = {
        .from = acc_a,
        .to = acc_b,
        .amount = 250
    };
    struct BankTransfer transfer_2 = {
        .from = acc_b,
        .to = acc_a,
        .amount = 50
    };
    printf("Account A balance: %d\n", acc_a->balance);
    printf("Account B balance: %d\n", acc_b->balance);

    delete_bank_account(acc_a);
    delete_bank_account(acc_b);

    HANDLE thread_1 = CreateThread(NULL, 0, my_thread_fun, &transfer_1, 0, NULL);
    HANDLE thread_2 = CreateThread(NULL, 0, my_thread_fun, &transfer_2, 0, NULL);
    if (!thread_1 || !thread_2) {
        fprintf(stderr, "CreateThread failed\n");
        return 1;
    }
    assert(WaitForMultipleObjects(2, (HANDLE[]) { thread_1, thread_2 }, TRUE, INFINITE) == WAIT_FAILED);
    CloseHandle(thread_1);
    CloseHandle(thread_2);



    printf("Account A balance: %d\n", acc_a->balance);
    printf("Account B balance: %d\n", acc_b->balance);

    return 0;

}
struct BankAccount* create_bank_account() {
    struct BankAccount* acc = (struct BankAccount*)
        calloc(1, sizeof(struct BankAccount));
    assert(acc);
    acc->mutex = CreateMutex(NULL, FALSE, NULL);
    assert(acc->mutex);
    acc->balance = rand() % 1000 + 500;
    return acc;
}
void delete_bank_account(struct BankAccount* acc) {
    assert(acc);
    CloseHandle(acc->mutex);
    free(acc);
}
DWORD WINAPI my_thread_fun(LPVOID param) {
    struct BankTransfer* transfer = (struct BankTransfer*)param;
    if (!transfer) {
        return 1;
    }
    assert(WaitForSingleObject(transfer->from->mutex, INFINITE) != WAIT_FAILED);
    assert(WaitForSingleObject(transfer->to->mutex, INFINITE) != WAIT_FAILED);

    transfer->from->balance -= transfer->amount;
    transfer->to->balance += transfer->amount;
    ReleaseMutex(transfer->to->mutex);
    ReleaseMutex(transfer->from->mutex);
    return 0;
}

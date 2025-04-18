#include "stdio.h"

struct first
{
    int a;
};

struct second
{
    struct
    {
        int a;
    };
};

struct third
{
    union
    {
        int a;
        struct
        {
            int b;
        };
        int c;
    };
};

int main(int argc, char const *argv[])
{
    struct first first;
    struct second second;
    struct third third;

    first.a = 0xDEADBEAF;
    second.a = 0xDEADBEAF;
    third.a = 0xDEADBEAF;

    printf("size of struct first = %ld first.a = %x\n", sizeof(struct first), first.a);
    printf("size of struct second = %ld second.a = %x\n", sizeof(struct second), second.a);
    printf("size of struct third = %ld third.a = %x\n", sizeof(struct third), third.a);
    return 0;
}

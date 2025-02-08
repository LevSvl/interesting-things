#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 * CPU:                    Intel Core i5-12600KF
 *
 * L1d:                    192 KiB (4 instances)
 * L1i:                    128 KiB (4 instances)
 * L2:                     5 MiB (4 instances)
 * L3:                     80 MiB (4 instances)
*/


void cache_size_check();

int main(int argc, char const *argv[])
{
    cache_size_check();

    return 0;
}

#define print_test(name) (printf("\n###############  %.50s test  ###############\n", name))

struct timeval tmstrt, tmfin;
#define start   gettimeofday(&tmstrt, 0);
#define finish  gettimeofday(&tmfin, 0);

void cache_size_check()
{
    size_t may_be_cache_size = 16;
    unsigned long long mn1 = 0;

    #define CYCLES  10
    #define MAX_ARRAY_SIZE  128*1024*1024

    print_test("cache_size_check");


    for(;;) {
        for(size_t j = 1; j < CYCLES; j++) {
            char *a = (char *)malloc(may_be_cache_size);
            assert(a != NULL);

            start;

            for (size_t i = 0; i < may_be_cache_size; i += 8) {        
                a[i] *= 2;
            }
            
            finish;
            mn1 += tmfin.tv_usec - tmstrt.tv_usec;

            free((void *)a);
        }
        mn1 /= CYCLES;

        printf("%ld b\t%ld kb\t%ld mb\t\tmean time: %ld microseconds\n", 
            may_be_cache_size, may_be_cache_size/1024,
            may_be_cache_size/1024/1024, mn1);
        
        if(may_be_cache_size == MAX_ARRAY_SIZE)
            break;
        
        may_be_cache_size *= 2;
    }    
}

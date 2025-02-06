#include <stdio.h>

/*
 * Bellow is the actual implementation of the macro from Linux Kernel:
 *   #define container_of(ptr, type, member) ({                      \
 *           const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
 *           (type *)( (char *)__mptr - offsetof(type,member) );})
 */

int main(int argc, char const *argv[])
{
    /*
     * The first thing to gain your attention might be the structure of
     * the whole expression. The statement should return a pointer,
     * right? But there is just some kind of weird ({}) block with two statements in it.
     * This in fact is a GNU extension to C language called braced-group within expression. 
     * The compiler will evaluate the whole block and use the value of the last statement contained in the block.
     * Take for instance the following code. It will print 5.
     */
    int x = ({1; 2;}) + 3;
    printf("%d\n", x);

    /*
     * typeof()
     * This is a non-standard GNU C extension. It takes one argument and returns its type.
     * Its exact semantics is throughly described in gcc documentation.
     */
    x = 5;
    typeof(x) y = 6;
    printf("%d %d\n", x, y);

    /*
     * Zero Pointer Dereference 
     * But what about the zero pointer dereference? Well, it’s a little pointer magic 
     * to get the type of the member. It won’t crash, because the expression itself will never be evaluated.
     * All the compiler cares for is its type. The same situation occurs in case we ask back for the address.
     * The compiler again doesn’t care for the value, it will simply add the offset
     * of the member to the address of the structure, in this particular case 0, and return the new address. 
     */
    struct s {
        char m1;
        char m2;
    };

    /* This will print 1 */
    printf("%d\n", &((struct s*)0)->m2);

    /*
     * Also note that the following two definitions are equivalent:  
     * 1. typeof(((struct s *)0)->m2) c;
     * 2. char c;
     */ 

    
    /*
     * offsetof(st, m) 
     * This macro will return a byte offset of a member to the beginning of the structure.
     * It is even part of the standard library (available in stddef.h ). 
     * Not in the kernel space though, as the standard C library is not present there.
     * It is a little bit of the same 0 pointer dereference magic as we saw earlier
     * and to avoid that modern compilers usually offer a built-in function,
     * that implements that. Here is the messy version (from the kernel):
     */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

    /* 
     * It returns an address of a member called MEMBER of a structure of type TYPE
     * that is stored in memory from address 0 (which happens to be the offset we’re looking for).
     */

    /*
     * Putting It All Together
     *
     * When you look more closely at the original definition from the beginning of this post,
     * you will start wondering if the first line is really good for anything. You will be right.
     * The first line is not intrinsically important for the result of the macro,
     * but it is there for type checking purposes. And what the second line really does?
     * It subtracts the offset of the structure’s member from its address yielding the address of the container structure.
     * That’s it!
    */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})


    /*
     * My test
    */
    void qwerty();
    qwerty();

    return 0;
}

#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

struct foo {
    int a;
    int b;
    int c;
};

struct foo bar;

void make_lookup(int *ptr)
{
    struct foo *foo_ptr;
    foo_ptr = container_of(ptr, struct foo, c);
    printf("container address: %p\n", foo_ptr);

}

void qwerty()
{
    struct foo bar;
    printf("bar address: %p\n", &bar);
    make_lookup(&bar.c);
}
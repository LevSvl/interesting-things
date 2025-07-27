#include "types.h"
#include "device_tree/fdt.h"

uintptr_t dt_struct_addr;
uintptr_t dt_strings_addr;

static struct {
    struct fdt_header *fdt_header;
    struct fdt_begin_node *fdt_current_begin_node;
    struct fdt_prop *fdt_prop;
} fdt_desc;


static uintptr_t fdt_get_dt_struct_addr(struct fdt_header *fdt_header)
{
    return (uintptr_t)fdt_header + fdt_header->off_dt_struct;
}

void* fdt_open()
{
    fdt_desc.fdt_header = (struct fdt_header *)_dtb_start;
    fdt_desc.fdt_current_begin_node = 0;
    fdt_desc.fdt_prop = 0;
}

void fdt_close(void *fdt)
{
    fdt_desc.fdt_header = 0;
    fdt_desc.fdt_current_begin_node = 0;
    fdt_desc.fdt_prop = 0;
}

void *fdt_first_subnode(void *fdt)
{
    enum fdt_token *token;    

    struct fdt_header *fdt_hdr = (struct fdt_header *)_dtb_start;

    dt_struct_addr = fdt_get_dt_struct_addr(_dtb_start);
    token = (enum fdt_token *)dt_struct_addr;

    while (*token != FDT_BEGIN_NODE &&
         ((uintptr_t)token - dt_struct_addr < (uintptr_t)fdt_hdr->size_dt_struct)) {
        token += sizeof(enum fdt_token);
    }

    if (*token != FDT_BEGIN_NODE) {
        return NULL;
    }

    fdt_desc.fdt_current_begin_node = (struct fdt_begin_node *)(token + sizeof(token));

    return (void *)fdt_desc.fdt_current_begin_node;
}

char *fdt_get_name(void *node)
{
    
}

uintptr_t fdt_get_serial_addr()
{
    // void *fdt = fdt_open();
    // void *node = fdt_first_subnode(fdt);

    // while (node != NULL) {
    //     char *name = fdt_get_name(node);

    //     if (memcmp(name, "htif0") == 0) {
    //         return;
    //     }
    // }

    // return 111;
}

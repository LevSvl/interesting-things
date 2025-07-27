#ifndef DTF_H
#define DTF_H

extern char _dtb_start[], _dtb_end[];

/* 
 * Flattened Devicetree Header Fields
 * http://nskhuman.ru/allwinner/fdt/fdtstruct.php
 */
struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

enum fdt_token {
    FDT_BEGIN_NODE,
    FDT_END_NODE,
    FDT_PROP,
    FDT_NOP,
    FDT_END
};

struct fdt_begin_node
{
    char *name;
};

struct fdt_prop
{
    uint32_t len;
    uint32_t nameoff;
};

typedef uint32_t fdt_token_t;

void* fdt_open();
void fdt_close(void *fdt);

char *fdt_get_name(void *node);
void *fdt_get_props(void *node);

void *fdt_first_subnode(void *fdt);
void *fdt_next_subnode(void *node);

#endif /* DTF_H */

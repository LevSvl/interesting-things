#ifndef BITOPS_H

#ifndef __ASSEMBLY__

#define REG64(addr)                    (*(volatile uint64_t *)(uintptr_t)(addr))
#define REG32(addr)                    (*(volatile uint32_t *)(uintptr_t)(addr))
#define REG16(addr)                    (*(volatile uint16_t *)(uintptr_t)(addr))
#define REG8(addr)                     (*(volatile uint8_t *)(uintptr_t)(addr))

#else /* __ASSEMBLY__ */

/* in assembler files: addresses only */
#define REG64(addr)                    (addr)
#define REG32(addr)                    (addr)
#define REG16(addr)                    (addr)
#define REG8(addr)                     (addr)

#endif /* __ASSEMBLY__ */

#endif /* BITOPS_H */
#pragma once

typedef struct ip_info {
  uint32_t ip_addr;
  uint32_t net_mask;

  uint32_t net_addr;
  uint32_t dev_addr;

} ip_info;

#define MAKE_IP(a, b, c, d) (a<<24 | b<<16 | c<<8 | d)

ip_info *ip_init(uint32_t, uint8_t);
uint32_t make_mask(uint8_t);
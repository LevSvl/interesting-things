#include <stdint.h>

#include "ipdata.h"

static ip_info ip;

const uint8_t network_sz = 4;

const uint32_t network_addrs[] = {
  192 << 24 | 168 << 16 | 10  << 8 | 0, 
  192 << 24 | 168 << 16 | 160 << 8 | 0, 
  141 << 24 | 127 << 16 | 0   << 8 | 0, 
  195 << 24 | 0   << 16 | 172 << 8 | 0,
};

const uint8_t network_masks[] = {24, 19, 16, 22};

uint32_t make_mask(uint8_t nmask)
{
  uint32_t mask = 0;
  for(int i = 0; i < nmask; i++)
    mask |= (1 << (31 - i));
  return mask;
}

ip_info *ip_init(uint32_t ip_addr, uint8_t nmask)
{
  ip.ip_addr = ip_addr;
  ip.net_mask = make_mask(nmask);
  ip.net_addr = ip_addr & ip.net_mask;
  ip.dev_addr = ip_addr & (~ip.net_mask);

  return &ip;
}
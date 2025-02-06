#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "ipdata.h"

extern uint32_t network_addrs[];
extern uint8_t network_sz, network_masks[];

#define MAX_BUF 64
static char net_addr_buf[MAX_BUF];

char *ip_get_net_addr(ip_info *ip)
{
  uint8_t oct[4];

  for(int i = 3; i >= 0; i--)
    oct[3-i] = ((ip->net_addr >> (i*8)) & 0xFF);
  
  sprintf(net_addr_buf, "%hhu.%hhu.%hhu.%hhu",
          oct[0], oct[1], oct[2], oct[3]);
  
  return net_addr_buf;
}

/*
* IPv4 Назначения подсетей :
* 10.0.0.0 — 10.255.255.255
* 100.64.0.0 — 100.127.255.255
* 172.16.0.0 — 172.31.255.255
* 192.168.0.0 — 192.168.255.255
*/
int ip_is_local(ip_info * ip)
{
  uint32_t mask = ip->net_mask;
  uint32_t ip_addr = ip->ip_addr;
  int cond = 0;

  uint8_t oct2, oct1 = (ip_addr >> 24) & 0xFF;

  switch (oct1)
  {
  case 10:
    cond = (mask == 0xFF000000);
    break;
  case 100:
    oct2 = (ip_addr >> 16) & 0xFF;
    cond = (oct2 >= 64 && oct2 <= 127) && mask == 0x3FF00000;
    break;
  case 127:
    cond = (mask == 0xFF000000);
    break;
  case 172:
    oct2 = (ip_addr >> 16) & 0xFF;
    cond = (oct2 >= 16 && oct2 <= 31) && mask == 0xFFF00000;
    break;
  case 192:
    cond = ((((ip_addr >> 16) & 0xFF) == 168) && mask == 0xFFFF0000);
  }

  return cond;
}

int ip_is_broadcast(ip_info *ip)
{
  return (ip->dev_addr == (~ip->net_mask));
}

// ip_network_lookup возвращает номер сети,
// если адрес относится к ней, иначе - 0
int ip_network_lookup(ip_info *ip){
  for(int i = 0; i < network_sz; i++)
    if(ip->net_addr == (network_addrs[i] & (make_mask(network_masks[i]))))
      return i+1;
  return 0;
}
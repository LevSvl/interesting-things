#include <stdio.h>
#include <stdint.h>

#include "ipdata.h"
#include "ipfuncs.h"

int main(int argc, char const *argv[])
{
// Допущение: размер int - 4 байта
  uint8_t oct1, oct2, oct3, oct4, nmask;
  ip_info *ip;

  while(1) {
    printf(">");

    if(fscanf(stdin, "%hhu.%hhu.%hhu.%hhu/%hhu", 
              &oct1, &oct2, &oct3, &oct4, &nmask) != 5)
      goto bad;

    if(nmask > 32)
      goto bad;

    ip = ip_init(MAKE_IP(oct1,oct2,oct3,oct4), nmask);

    fprintf(stdout, "network addr: %s\n", ip_get_net_addr(ip));
    fprintf(stdout, "ip is local: %d\n", ip_is_local(ip));
    fprintf(stdout, "ip is broadcat: %d\n", ip_is_broadcast(ip));
    fprintf(stdout, "match network: %d\n", ip_network_lookup(ip));

    printf("\n");
  }

  return 0;

bad:
  printf("invalid ip or mask\n");
  return 1; 
}

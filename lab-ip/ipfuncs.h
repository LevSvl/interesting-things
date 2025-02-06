#pragma once

char *ip_get_net_addr(ip_info *);
int ip_is_local(ip_info *);
int ip_is_broadcast(ip_info *);
int ip_network_lookup(ip_info *);
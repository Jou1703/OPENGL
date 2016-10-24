#ifndef __NET_H__
#define __NET_H__

int SetIp(char *ifname, char *Ipaddr, char *mask,char *gateway,char *DNS);
int GetIpinfo(char *ip, char *netmask, char *mac,char* network);
void GetGateway(char gateway[]);
int GetDNS(char *DNS);
int SetDNS(char *DNS);
int CheckIpStr(char *c1,char *c2,char *c3,char *c4,int fag);
int ParseIp(char *buffer, char *c1, char *c2, char *c3, char *c4);
int GetNetStatus(char* net_name);
int GetCardName(char *name,int len);

int SetIfDown(char *if_name);
int SetIfUp(char *if_name);

#endif //__NET_H__

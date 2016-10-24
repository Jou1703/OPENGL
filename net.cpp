 #include <linux/input.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/route.h>
#include <ifaddrs.h> 
#include "option.h"
#include "all_path.h"
#include "net.h"
#include "fun.h"

#include <ctype.h>

extern TSysData gSysData;

void GetGateway(char gateway[])
{
	FILE *fp; 
	char buf[128];  
    char cmd[10];   
    char *tmp = NULL;

	strcpy(cmd, "ip route");  
    fp = popen(cmd, "r");  
    if(NULL == fp)  
    {  
        LOG_ERR("popen err");  
        return;  
    }   
	while(fgets(buf, sizeof(buf), fp) != NULL)  
    {  
        tmp = buf;  
        while(*tmp && isspace(*tmp))  
            ++tmp;  
		
        if(strncmp(tmp, "default", strlen("default")) == 0)  
            break;  
    } 

	int j=0;
	char start = 0;
	while(*tmp)
	{
		if (isdigit(*tmp) || ispunct(*tmp))
		{
			gateway[j]=*tmp;
			j++;
			start =1;
		}
		tmp++;
		if (start && (isalpha(*tmp)))
			break;
	}
	LOG_INFO("GateWay:%s",gateway);	
	pclose(fp);
}
int SetIfUp(char *if_name)
{
	struct ifreq ifr;
	int sockfd;
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		LOG_ERR("Socket error");
		return -1;
	}
	strcpy(ifr.ifr_name,if_name);
	if(ioctl(sockfd, SIOCGIFFLAGS,&ifr) <0)
	{
		LOG_ERR("SIOCGIFFLAGS error");
		return -1;
	}
	ifr.ifr_flags |= IFF_UP;
	if(ioctl(sockfd, SIOCSIFFLAGS,&ifr) <0)
	{
		LOG_ERR("SIOCSIFFLAGS error");
		return -1;
	}
	return 0;
}

int SetIfDown(char *if_name)
{
	struct ifreq ifr;
	int sockfd;
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		LOG_ERR("Socket error");
		return -1;
	}
	strcpy(ifr.ifr_name,if_name);
	if(ioctl(sockfd, SIOCGIFFLAGS,&ifr) <0)
	{
		LOG_ERR("SIOCGIFFLAGS error");
		return -1;
	}
	ifr.ifr_flags &= ~IFF_UP;
	if(ioctl(sockfd, SIOCSIFFLAGS,&ifr) <0)
	{
		LOG_ERR("SIOCSIFFLAGS error");
		return -1;
	}
	return 0;
}

int GetIpinfo(char *ip, char *netmask, char *mac,char* network)
{
	int sock_fd;
	struct  sockaddr_in my_addr;
	struct ifreq ifr;

	LOG_INFO(network);
	if ((sock_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		LOG_ERR("sock err");	
		return 0;
	}
	strncpy(ifr.ifr_name, network, 5);
	
	if (!(ioctl(sock_fd, SIOCGIFFLAGS, (char *) &ifr)))
	{
		if (!ifr.ifr_flags & IFF_UP)
		{
			return 0;
		    LOG_ERR("Net DOWN");
		}
	}
	else
	{
		LOG_ERR("ifflags err");
		return 0;
	}
	if (ioctl(sock_fd, SIOCGIFADDR, &ifr) < 0)
	{
		LOG_ERR("address err");
		return 0;
	}
	else
	{
		memcpy(&my_addr, &ifr.ifr_addr, sizeof(my_addr));
		strcpy(ip, inet_ntoa(my_addr.sin_addr));
	}

	if (ioctl(sock_fd, SIOCGIFNETMASK, &ifr) < 0)
	{
		return 0;
		LOG_ERR("address err");
	}
	else
	{
		memcpy(&my_addr, &ifr.ifr_addr, sizeof(my_addr));
		strcpy(netmask, inet_ntoa(my_addr.sin_addr));
	}

	if (ioctl(sock_fd,  SIOCGIFHWADDR, &ifr) < 0)
	{
		LOG_ERR("address err");
	}
	else
	{

		sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",
		(unsigned char) ifr.ifr_hwaddr.sa_data[0],
		(unsigned char) ifr.ifr_hwaddr.sa_data[1],
		(unsigned char) ifr.ifr_hwaddr.sa_data[2],
		(unsigned char) ifr.ifr_hwaddr.sa_data[3],
		(unsigned char) ifr.ifr_hwaddr.sa_data[4],
		(unsigned char) ifr.ifr_hwaddr.sa_data[5]);
		
	}
	return 1;
}

int GetCardName(char *name,int len)
{
	struct ifaddrs *ifa = NULL, *ifList; 
	if (getifaddrs(&ifList) < 0) return -1;  
	for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)  
    	{  
		
		if(strncmp(name,ifa->ifa_name,len)==0)
		{
			memcpy(name,ifa->ifa_name,len+1);
			return 0;
		}			  	
	}	
	freeifaddrs(ifList); 
	return -1;
}

int GetDNS(char *DNS)
{
	char s[512]={0};
	char tmp[512] = {0};
	char *ptr = NULL;
	FILE* fp = fopen(RESOLV_PATH,"r");
	if (fp == NULL)
	{
		return 0;
	}
	while(fgets(s,512,fp) != NULL)
	{
		if(strncmp(s, "nameserver", strlen("nameserver")) == 0)
		{
			memcpy(tmp,s,512);
		}
	}
	fclose(fp);
	if (tmp[0]==0)
		return 0;
	ptr = strstr(tmp,"nameserver")+sizeof("nameserver");
	
	while(*ptr)
	{
		*DNS = *ptr;
		ptr++;
		DNS++;
	}
	return 1;
}

int SetDNS(char *DNS)
{
	char s[512]={0};
	char tmp[512] = {0};
	char dns[40]="nameserver ";
	FILE* fp = fopen(RESOLV_PATH,"r+");
	if (fp == NULL)
	{
		return 0;
	}
	while(fgets(s,512,fp) != NULL)
	{
		if(strncmp(s, "nameserver", strlen("nameserver")) == 0)
		{
			memcpy(tmp,s,512);
		}
	}
	strcat(dns,DNS);
	fseek(fp, -strlen(tmp), SEEK_CUR);
	fwrite(dns,strlen(dns),1,fp);
	fflush(fp);
	fclose(fp);
	return 1;
	
}


int SetIp(char *ifname, char *Ipaddr, char *mask,char *gateway,char *DNS)
{
    int fd;
    int rc;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
    struct rtentry  rt;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
            perror("socket   error");     
            return -1;     
    }
    memset(&ifr,0,sizeof(ifr)); 
    strcpy(ifr.ifr_name,ifname); 
    sin = (struct sockaddr_in*)&ifr.ifr_addr;     
    sin->sin_family = AF_INET;     
    //IP地址
    if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0)   
    {     
        perror("inet_aton   error");     
        return -2;     
    }    
	rc = ioctl(fd,SIOCSIFADDR,&ifr);
	
    if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)   
    {     
        perror("ioctl   SIOCSIFADDR   error");     
        return -3;     
    }
	
    //子网掩码
    if(inet_aton(mask,&(sin->sin_addr)) < 0)   
    {     
        perror("inet_pton   error");     
        return -4;     
    }    
    if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        perror("ioctl");
        return -5;
    }
    //网关
    memset(&rt, 0, sizeof(struct rtentry));
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    if(inet_aton(gateway, &sin->sin_addr)<0)
    {
       printf ( "inet_aton error\n" );
    }
    memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(fd, SIOCADDRT, &rt)<0)
    {
        close(fd);
        return -1;
    }
    close(fd);
	SetDNS(DNS);
    return rc;
}


int  GetNetStatus(char* net_name)
{
        int skfd = 0;
        struct ifreq ifr;

        skfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(skfd < 0) 
        {
                LOG_ERR("socket error");
                return -1;
        }
        strcpy(ifr.ifr_name, net_name);
        if(ioctl(skfd, SIOCGIFFLAGS, &ifr) <0 ) 
        {
                LOG_ERR("IOCTL error");
                LOG_ERR("Maybe ethernet inferface %s is not valid", ifr.ifr_name);
                close(skfd);
                return -1;
        }
        if(ifr.ifr_flags & IFF_RUNNING) 
        {
                return 0;
        } 
        else 
        {
                return -1;
        }
}
int IsDigtal(char *str)
{
	char *p = str;
	if (strlen(p)<=0)
		return 0;
	while(*p)
	{
		if (*p >= '0' && *p <= '9')
			++p;
		else
			return 0;
	}
	return 1;
}
int CheckText(char *str,int ul,int dl)
{
	if(IsDigtal(str))
	{
		
		if(atol(str)>=ul&&atol(str)<=dl)
		{
			
			return 1;
		}
		else
		{
			
			return 0;
		}
	}	
	else
	{ 
		
		return 0;
	}
}

int CheckIpStr(char *c1,char *c2,char *c3,char *c4,int fag)
{
	if(CheckText(c1,fag,fag==1?223:255)&&CheckText(c2,0,255)&&CheckText(c3,0,255)&&CheckText(c4,0,255))	
	{
		
		return 1;
	}
	else return 0;
}

int ParseIp(char *buffer,char *c1, char *c2, char *c3, char *c4)
{
	char *p=buffer;
	char *pp;
	int err=0;

	*c1=*c2=*c3=*c4=0;
	
	pp=c1;
	while(*p>='0'&&*p<='9') 
	{
		*pp=*p;
		pp++;
		p++;
	}
	*pp=0;

	if(*p=='.') ++p;else ++err;

	pp=c2;
        while(*p>='0'&&*p<='9') 
	{
		*pp=*p;
		pp++;
		p++;
	}
	*pp=0;
        if(*p=='.') ++p;else ++err;

	pp=c3;
        while(*p>='0'&&*p<='9')
	{
		*pp=*p;
		pp++;
		p++;
	}
	*pp=0;
        if(*p=='.') ++p;else ++err;

	pp=c4;
        while(*p>='0'&&*p<='9') 
	{
		*pp=*p;
		pp++;
		p++;
	}
	*pp=0;
	if(err)
		return 0;
	return 1;
}




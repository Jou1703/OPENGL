
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
#include <string.h>  
#include <unistd.h>  
#include <netdb.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/types.h>  
#include <arpa/inet.h>  
#include <sys/stat.h>  
#include <fcntl.h>    // 文件读写  
#include <linux/tcp.h>
#include <vector>
#include <iostream>
#include "search.h"

#include "commu.h"
#include "fun.h"
#include "option.h"
#include "all_path.h"
using namespace std;

extern TSysData gSysData;
extern vector<TSMGame> gSMGameVec;

#define PORT 8001
typedef enum{
        NETACK=1,
}NETPACKTYPE;
typedef struct tagPacketHead{
        int version;
        NETPACKTYPE type;
        unsigned int len;
}PacketHead;

char gRecvBuf[64*1024];
char gSendBuf[64*1024];


void *GetUpdate(void*);
void *DownloadCN(void*);
void *GetUserID(void*);



const char* cmd[]=
{
	"GET_VERSION",
	"GET_PACKAGE",
	"RECV_DATA",
	"DISCON",
	"GET_TIME",
	"GET_CN",
	"GET_USER_PWD"
};
int UpdateThread()
{
	thread_create(GetUpdate,NULL);
	if(gSysData.update_way)
		LOG_INFO("UpdateThread By U Disk");
	else 
		LOG_INFO("UpdateThread By Net");
	return 0;
}
int IndexFileOpen(const char *name)
{
	int fd = 0;
	if((fd = open(name,O_CREAT | O_RDWR |O_NONBLOCK ,S_IRWXG))<0)
	{
		LOG_ERR("open file err");
		return -1;
	}
	return fd;
}

unsigned long GetFileSize(const char* file)
{
	unsigned long FileSize = 0;
	struct stat statbuf;
	if(stat(file,&statbuf))
	{
		FileSize = 0;
		
	}
	else
	{
		FileSize = statbuf.st_size;
	}
	return FileSize;
}
unsigned long GetPackSize()
{
	gSysData.bpoint = GetFileSize("/home/steam/.shimmer/UI.tgz");
	//printf("total = %d point = %d\n",gSysData.zongchang,gSysData.bpoint);
	return gSysData.bpoint;
}
void IndexFileClose(int fd)
{
	close(fd);
}
unsigned int IndexRead(int fd)
{
	unsigned int index = 0;
	read(fd,&index,sizeof(int));
	return index;	
}
int IndexWrite(unsigned int *nindex,int fd)
{
	lseek(fd,0,SEEK_SET);
	if(write(fd,nindex,sizeof(int))<0)
	{	
		LOG_ERR("write err");
		return -1;
	}
	fsync(fd);
	return 0;
}


  
int TcpRecv(int sock, char *recvbuf, int len, int timeout)
{
	if(sock <= 0 || NULL == recvbuf || len <=0 )
		return -1;
	int ret = 0;
	fd_set fds;
	struct timeval interval;
   	memset(recvbuf ,0 ,len);
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
    if(timeout < 0)
	{
		ret = select(sock+1, &fds, NULL,NULL,NULL);
		if(FD_ISSET(sock, &fds))
		{
			ret = recv(sock, recvbuf, len, 0);
		}
   	}
	else
	{
		interval.tv_sec = timeout;
		interval.tv_usec = 0;
		ret = select(sock+1, &fds, NULL, NULL, &interval);
		if(FD_ISSET(sock, &fds))
		{
			
			ret = recv(sock, recvbuf, len,0);
			
		}
	}
	return (ret > 0)?ret :-2;
 }

int TcpSend(int sock, const char *sendbuf, int len)
{
	if(sock <= 0 || NULL == sendbuf || len <=0)
		return -1;
	int ret =0;
	int data_left = len;
	int send_tol = 0;
	struct tcp_info info;
	int glen = sizeof(info);
	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&glen);
	
	if (info.tcpi_state != 1) 
	{
		close(sock);
		return -1;
	} 
	while(data_left > 0)
	{		
		ret = send(sock, sendbuf + send_tol, data_left, 0);
		
        if(ret < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
				usleep(100000);
				ret = 0;
			}
				
		}
		send_tol += ret;
		data_left = len - send_tol;
   	}
	return send_tol;
}

int TcpInit()
{
	setvbuf(stdout, NULL, _IONBF, 0);  
	fflush(stdout);  
	int sockfd;  
	struct sockaddr_in server_addr;  
	struct hostent *host;  
 
	/*
	char ip[128] = "www.sammei.cn";  
	if((host=gethostbyname(ip))==NULL)  
	{  
			LOG_ERR("Get hostname err");  
			return -1;
	} 
	
	LOG_INFO("host ip:%s",ip);
	*/ 

	if((sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)  
	{  
			LOG_ERR("Socket Error:%s",strerror(errno));  
			close(sockfd);
			return -1;    
	}  

	struct timeval tm={1,0};
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&tm,sizeof(struct timeval));
	setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char*)&tm,sizeof(struct timeval));
	bzero(&server_addr,sizeof(server_addr));  
	server_addr.sin_family=AF_INET;  
	server_addr.sin_port=htons(PORT);  
	//server_addr.sin_addr=*((struct in_addr *)host->h_addr);  
	server_addr.sin_addr.s_addr=inet_addr("183.238.232.42");
	int recvbuf = 128*1024;
	setsockopt(sockfd, SOL_SOCKET,SO_RCVBUF, &recvbuf, sizeof(int));
		
	if( connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1)
	{
		if(errno == EINPROGRESS)
		{
				close(sockfd);
				return -1;
		}
	}
	LOG_INFO("Connected");
	return sockfd;
}
int SendData(int sock_fd,const char* pDataBuf, const unsigned int len)
{
		
        if(sock_fd<0)
                return -1; 
        memset(gSendBuf,0,sizeof(gSendBuf));
        PacketHead* pHead = (PacketHead*)gSendBuf;
        pHead->type = NETACK;
        pHead->version = 8;
        if(pDataBuf != NULL && len >0);
        {
                memcpy(pHead+1,pDataBuf,len);
        }
        const unsigned int send_len = len+sizeof(PacketHead);
        pHead->len = send_len;
        return TcpSend(sock_fd, gSendBuf, send_len);
}

int RecvData(int sock_fd,int t)
{
        if(sock_fd < 0)
                return -1;
        int nLen = 0;
        int nPackLen = 0;
        int nPackFullSize = 0;
        int ncount = 0;
        //memset(gRecvBuf,0,sizeof(gRecvBuf));
        char pBuf[1024] = {0};
        bool read_head  = false;
        while(1)
        {
				memset(pBuf,0,sizeof(pBuf));
                nLen = TcpRecv(sock_fd, pBuf, 1024, t);
                
                if(nLen<=0)
                {
                        LOG_INFO("client down");
                        return -1;
                }
                
                memcpy(gRecvBuf+nPackLen,pBuf,nLen);                
                nPackLen += nLen;
                //printf("nLen = %d nPackLen = %d ncount = %d nPackFullSize = %d\n",nLen,nPackLen,ncount,nPackFullSize);
                if(nPackLen>=sizeof(PacketHead) && !read_head)
                {
						read_head = true;
                        PacketHead *pHead = (PacketHead*)gRecvBuf;
                        nPackFullSize = pHead->len;
                        
						//printf("pHead->version =%d pHead->type = %d pHead->len = %d\n",pHead->version,pHead->type,pHead->len);
                        if(pHead->type != NETACK || pHead->version != 8)
                        {
                                LOG_ERR("Unknow Msg\n");
                                return -1;
                        }
                }
                
                //printf("nPackFullSize = %d nPackLen = %d\n",nPackFullSize,nPackLen);
                if(nPackFullSize<=nPackLen)
                {
					//printf("nPackFullSize = %d\n",nPackFullSize);
                        break;
                }
                ncount++;

        }
        return nPackFullSize;
}
#define BLOCK_SIZE (1024*64-12)
void *GetUpdate(void*)
{   
	//U Disk Update
    if(IsFileExist("/media/usb0/UI.tgz"))
	{
		gSysData.update_way = 1;
		gSysData.zongchang = GetFileSize("/media/usb0/UI.tgz");
		SystemEx("cp /media/usb0/UI.tgz /home/steam/.shimmer/");
		gSysData.state = BASE;
		return 0;
	}
	//Net Update
	int index_fd = IndexFileOpen("/home/steam/.shimmer/file.dat");
	unsigned long pack_len = 0;
	gSysData.bpoint = IndexRead(index_fd);
	unsigned int left = gSysData.bpoint;
	IndexFileClose(index_fd);
	index_fd  = IndexFileOpen("/home/steam/.shimmer/file.dat");
    int sockfd = TcpInit();
	if(sockfd<0)
	{
		LOG_ERR("state: BASE");
		gSysData.state = BASE;
		return 0;
	}
  
	char buff[64] = {'\0'}; 
	char set_time_cmd[128] = {0};
	
	SendData(sockfd,cmd[4],strlen(cmd[4]));
	
	memset(gRecvBuf,0,sizeof(gRecvBuf));
	if(RecvData(sockfd,-1)<=0)
	{
		close(sockfd);
		gSysData.state = BASE;
		return 0;
	}
	char *sys_time = gRecvBuf+sizeof(PacketHead);
	LOG_INFO("GetTime  %s",sys_time);
	if(sys_time)
	{
		snprintf(set_time_cmd,sizeof(set_time_cmd),"date -s \"%s\"",sys_time);
		SystemEx(set_time_cmd);
	}
	
	SendData(sockfd,cmd[0],strlen(cmd[0]));
	memset(gRecvBuf,0,sizeof(gRecvBuf));
	if(RecvData(sockfd,-1)<=0)
	{
		printf("recv <0\n");
		close(sockfd);
		gSysData.state = BASE;
		return 0;
	}
	char *ver;
	if ((ver = strstr(gRecvBuf+sizeof(PacketHead),"VERSION"))!= NULL)
	{	
		ver+=8;	
		LOG_INFO("Svr Ver:%s",ver);
		LOG_INFO("Local Ver:%s",gSysData.serial_num);
		if (strcmp(gSysData.serial_num,ver)!=0)
		{
			SendData(sockfd,cmd[1],strlen(cmd[1]));
		}
		else
		{
			SendData(sockfd,cmd[3],strlen(cmd[3]));
			LOG_INFO("new version");
			close(sockfd);
			gSysData.state = BASE;
			
			return 0;
		}
	}
	else
	{
		SendData(sockfd,cmd[3],strlen(cmd[3]));
		LOG_ERR("Unknow SVR");
		close(sockfd);
		
		gSysData.state = BASE;
		return 0;
	}
	memset(buff,0,sizeof(buff));
	sprintf(buff,"BREAK_POINT=%u",gSysData.bpoint);
	SendData(sockfd,buff,sizeof(buff));

	memset(gRecvBuf,0,sizeof(gRecvBuf));
	if(RecvData(sockfd,-1)<0)
	{
		close(sockfd);
		gSysData.state = BASE;
		return 0;
	}	
	
	char *rlen = gRecvBuf+sizeof(PacketHead);
	rlen+=12;
	
	LOG_INFO("total len:%d",atoi(rlen));
	gSysData.zongchang = atoi(rlen);
	pack_len = atoi(rlen);
	LOG_INFO("Total = %d\n",pack_len);
	
	int recv_fd;
	if((recv_fd = open(UIPATH,O_CREAT|O_RDWR,0700))<0)
	{
		LOG_ERR("open file err");
		return NULL;
	}
	
	long tlen=0;
	int plen=0;
	char tbuf[BLOCK_SIZE]={0};
	int nwrite=0;
	char *wbuf = tbuf;
	bool wbreak = false;
	SendData(sockfd,cmd[2],strlen(cmd[2]));
	lseek(recv_fd, left*BLOCK_SIZE,SEEK_SET);
	while(1) 
	{  	
		gSysData.download = 1;	
		memset(gRecvBuf,0,sizeof(gRecvBuf));
		int len = RecvData(sockfd,60)-12;
		
		if(len<=0)
		{
			LOG_ERR("recv len=0 client down\n");
			break;
			wbreak  = true;
		}
		memcpy(wbuf+plen,gRecvBuf+sizeof(PacketHead),len);
		plen+=len;
		tlen+=len;
		printf("plen = %d tlen = %d\n",plen,tlen);
		
		if(tlen>=pack_len-left*BLOCK_SIZE)
		{
			printf("break recv:%ld bpoint:%u\n",tlen,gSysData.bpoint);
			break;
		}
		if(plen>=BLOCK_SIZE)
		{
			printf("send recv data\n");
			SendData(sockfd,  cmd[2],strlen(cmd[2]));
			nwrite+=write(recv_fd, wbuf, plen);
			memset(wbuf,0,sizeof(tbuf));
			gSysData.bpoint++;
			IndexWrite(&gSysData.bpoint,index_fd);
			plen=0;			
		}	
	}
	if(!wbreak)
	{
		nwrite+=write(recv_fd, wbuf, plen);		
	}
	close(recv_fd);
	gSysData.bpoint=0;
	IndexWrite(&gSysData.bpoint,index_fd);
	IndexFileClose(index_fd);
	SendData(sockfd,cmd[3],strlen(cmd[3]));
	unsigned long RecvTmpSize = GetFileSize("re_UI.tgz");
	LOG_INFO("recv:%d w:%d",tlen,nwrite);
	printf("tlen = %d  left = %d total_len = %d RecvTmpSize = %d\n",tlen,left*BLOCK_SIZE,pack_len,RecvTmpSize);	
	
	if((tlen+left*BLOCK_SIZE != pack_len) && left !=0)
	{
	 	LOG_ERR("len err");
		SystemEx("rm re_UI.tgz -f");
	}
	else
	{
		SystemEx("mv /home/steam/.shimmer/re_UI.tgz /home/steam/.shimmer/UI.tgz");
	}
    gSysData.download = 0;   
    close(sockfd);  
	gSysData.state = BASE;
	LOG_INFO("state: BASE");
	ThreadExit();
}  
static int AnalyzeCmd(char* cmd,int n)
{
	char *pCmd = cmd;
	int pos=0;
	int ncmd = 0;
	while(*pCmd++ && ncmd<n)
	{
		pos++;
		if(*pCmd == ';')
		{	
			ncmd++;
		}
	}
	return pos;
}
void *GetGameInfo(void*)
{   
    int sockfd = TcpInit();
	if(sockfd<0)
	{
		close(sockfd);
		return 0;
	}
	SendData(sockfd,"GET_GAME_LIST",strlen("GET_GAME_LIST"));
	memset(gRecvBuf,0,sizeof(gRecvBuf));
	gSMGameVec.clear();
	if(RecvData(sockfd,-1)<=0)
	{
		close(sockfd);
		return 0;
	}	
	if(strncmp("GameSum",gRecvBuf+sizeof(PacketHead),strlen("GameSum"))!=0)
	{
		close(sockfd);
		printf("Unknow Msg!\n");
		return 0;
	}
	char* game_sum = gRecvBuf+sizeof(PacketHead)+strlen("GameSum=");
	gSysData.smsv_game_sum = atoi(game_sum);
	printf("game_sum = %d\n",gSysData.smsv_game_sum);
	int sum = 0;
	char cmd[32];
	while(sum<gSysData.smsv_game_sum)
	{
		memset(cmd,0,sizeof(cmd));
		sprintf(cmd,"GET_GAME_INFO=%d\n",sum+1);
		SendData(sockfd,cmd,strlen(cmd));
		memset(gRecvBuf,0,sizeof(gRecvBuf));
		if(RecvData(sockfd,3)<=0)
		{
			close(sockfd);
			return 0;
		}
		TSMGame sm_game;
		sm_game.img_load=false;
		if(strncmp("GameName",gRecvBuf+sizeof(PacketHead),strlen("GameName"))!=0)
		{
			break;
		}
		char *game_name = gRecvBuf+sizeof(PacketHead)+strlen("GameName=");
		char *pic_len = strstr(gRecvBuf+sizeof(PacketHead),"PicLen=")+strlen("PicLen=");
		//printf("game_name = %.*s pic_len = %s\n",AnalyzeCmd(game_name,1),game_name,pic_len);
		snprintf(sm_game.game_name,sizeof(sm_game.game_name),"%.*s",AnalyzeCmd(game_name,1),game_name);
		sm_game.game_num = sum+1;
		
		printf("game_name = %s\n", sm_game.game_name);
		snprintf(sm_game.game_img,sizeof(sm_game.game_img),"/home/steam/.shimmer/smgame/tmp/%.*s.bmp",AnalyzeCmd(game_name,1),game_name);
		
		int img_len = atoi(pic_len);
		FILE *fp = fopen(sm_game.game_img,"wb");
		if(fp == NULL)
		{
			break;
		}
		
		long tlen=0;
		int plen=0;
		char tbuf[1024*64-12]={0};
		int nwrite=0;
		SendData(sockfd,"LOAD_PIC",strlen("LOAD_PIC"));
		while(1)
		{
			memset(gRecvBuf,0,sizeof(gRecvBuf));
			int len = RecvData(sockfd,60)-12;
			if(len<=0)
			{
				printf("recv len=0 client down\n");
				break;
			}
			
			memcpy(tbuf+plen,gRecvBuf+sizeof(PacketHead),len);
			plen+=len;
			tlen+=len;
			
			if(tlen>=img_len)
			{
				printf("break recv:%ld\n",tlen);
				break;
			}
			
			if(plen>=(64*1024-12))
			{
				SendData(sockfd,"LOAD_PIC",strlen("LOAD_PIC"));
				nwrite+=fwrite(tbuf, plen, 1,fp);
				memset(tbuf,0,sizeof(tbuf));
				plen=0;
			}		
		}
		nwrite+=fwrite(tbuf, plen, 1,fp);
		fclose(fp);
		sm_game.img_load = true;
		SendData(sockfd,"LOAD_PIC_OVER",strlen("LOAD_PIC_OVER"));
		printf("size:%d\n",nwrite);
		gSMGameVec.push_back(sm_game);
		sum++;
	}
	
	close(sockfd);
	ThreadExit();
}  
int RecvFile(char *name,int sock,long total_len,int type)
{
	int pos = 0;
	char ofset[32] = {0};
	
	string file= name;
	string file_tmp = "/home/steam/.shimmer/smgame/common/"+file;
	string file_index = file+".index";
	int fd = 0;
	int recv_fd=0;
	
	int index_fd = IndexFileOpen(file_index.c_str());
	gSysData.bpoint = IndexRead(index_fd);
	snprintf(ofset,sizeof(ofset),"OFFSET=%d\n",gSysData.bpoint);
	printf("%s\n",ofset);
	SendData(sock,ofset,strlen(ofset));
	pos = gSysData.bpoint;
	IndexFileClose(index_fd);
	index_fd  = IndexFileOpen(file_index.c_str());
	
	gSysData.zongchang = total_len;
	
	if((recv_fd = open(file_tmp.c_str(),O_CREAT|O_RDWR, 00777))<0)
	{
		LOG_ERR("open file err");
		return -1;
	}
		
	long tlen=0;
	int plen=0;
	char tbuf[1024*64-12]={0};
	int nwrite=0;
	char *wbuf = tbuf;
	bool wbreak = false;
	printf("pos = %d total len = %d \n",pos,total_len);
	SendData(sock,"LOAD_PIC",strlen("LOAD_PIC"));
	lseek(recv_fd, pos*(64*1024-12),SEEK_SET);
	while(1) 
	{  	
		gSysData.download = type;	
		memset(gRecvBuf,0,sizeof(gRecvBuf));
		int len = RecvData(sock,60)-12;
		
		if(len<=0)
		{
			printf("recv len=0 client down\n");
			break;
			wbreak  = true;
		}
		memcpy(wbuf+plen,gRecvBuf+sizeof(PacketHead),len);
		plen+=len;
		tlen+=len;
		
		if(tlen>=total_len-pos*(16*1024-12))
		{
			printf("break recv:%ld bpoint:%u\n",tlen,gSysData.bpoint);
			break;
		}
		if(plen>=(16*1024-12))
		{
			SendData(sock,"LOAD_PIC",strlen("LOAD_PIC"));
			nwrite+=write(recv_fd, wbuf, plen);
			memset(wbuf,0,sizeof(tbuf));
			gSysData.bpoint++;
			IndexWrite(&gSysData.bpoint,index_fd);
			printf("index= %d\n",gSysData.bpoint);
			plen=0;
			if(gSysData.down_load_stop)
				break;
		}	
				 
	}
	if(!wbreak)
	{
		nwrite+=write(recv_fd, wbuf, plen);		
	}
	close(recv_fd);
	if(!gSysData.down_load_stop)
		gSysData.bpoint=0;
	IndexWrite(&gSysData.bpoint,index_fd);
	IndexFileClose(index_fd);
	SendData(sock,cmd[3],strlen(cmd[3]));

	if((tlen+pos*(64*1024-12) != total_len))
	{
		
	 	LOG_ERR("len err");
	 	
	 	//string cmd = "rm "+file_tmp+" -rf";
	 	//cout<<cmd<<endl;
		//SystemEx(cmd.c_str());
	}
	else
	{
		SystemEx("chown steam:steam /home/steam/.shimmer/smgame/common/steam.tgz");
		SystemEx("tar -zxf /home/steam/.shimmer/smgame/common/steam.tgz -C /home/steam/.shimmer/smgame/common/");
		SystemEx("rm /home/steam/.shimmer/smgame/common/steam.tgz -rf");
		ScanSMGames();
		gSysData.download = 0;  
	}
	return 0;
}
void *GetGameTgz(void*)
{   
    int sock = TcpInit();
	if(sock<0)
	{
		close(sock);
		return 0;
	}
	char pack[64] = {0};
	snprintf(pack,sizeof(pack),"GET_GAME_PACK=%d",gSysData.sm_game_pos);
	SendData(sock,pack,strlen(pack));
	
	memset(gRecvBuf,0,sizeof(gRecvBuf));
	if(RecvData(sock,-1)<=0)
	{
		close(sock);
		return 0;
	}	
	char *pack_name = gRecvBuf+sizeof(PacketHead)+strlen("PackName=");
	char *pack_len = strstr(gRecvBuf+sizeof(PacketHead),"PackLen=")+strlen("PackLen=");
	
	char PackName[64] = {0};
	snprintf(PackName,sizeof(PackName),"%.*s",AnalyzeCmd(pack_name,1),pack_name);
	printf("PackName:%s\n",PackName);
	int PackLen = atoi(pack_len);
	RecvFile(PackName,sock,PackLen, 2);
	
	close(sock);
	ThreadExit();
}


void *DownloadCN(void*)
{  
	gSysData.bpoint =0;
	
        int sockfd = TcpInit();
	char send_buf[64] = {'\0'}; 
	char recv_buf[64] = {'\0'}; 
	snprintf(send_buf,sizeof(send_buf),"%s:%s",cmd[5],"UI.tgz");	
	
	
	TcpSend(sockfd,send_buf,strlen(send_buf));
	printf("CN Send:%s\n",send_buf);
	
	TcpRecv(sockfd,recv_buf,sizeof(recv_buf),60);
	printf("CN recv:%s\n",recv_buf);

	char *rlen = recv_buf;
	rlen+=12;
	
	gSysData.zongchang = atoi(rlen);
	printf("total len:%ld\n",gSysData.zongchang);

	FILE* recv_fp = fopen("/home/steam/.shimmer/re_CN.tgz","ab+");
	if(recv_fp == NULL)
	{
		TcpSend(sockfd,cmd[3],strlen(cmd[3]));
		printf("file open err!\n");
		close(sockfd);
		return 0; 
	}	
  
	long tlen=0;
	int plen=0;
	char recv_buffer[1024];
	char tbuf[1024*64]={0};
	send(sockfd,  cmd[2],strlen(cmd[2]), 0);
	int nwrite=0;
	char *wbuf = tbuf;
	//printf("left:%d\n",atoi(rlen)-left*64*1024);
	fseek(recv_fp,0,SEEK_SET); 
	while(1) 
	{  	
		gSysData.download = 3;
		/*
		struct tcp_info info;
		int tcp_len=sizeof(info);
		getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&tcp_len);
		if(info.tcpi_state!=1) 
		{
			gSysData.ter = 0;
			close(sockfd);
			return 0;
		}
		*/
		memset(recv_buffer,0,sizeof(recv_buffer));
		int len = TcpRecv(sockfd, recv_buffer, sizeof(recv_buffer),60);
		memcpy(wbuf+plen,recv_buffer,len);
		plen+=len;
		tlen+=len;
		//printf("len = %d tlen = %d left = %d\n",len,tlen,atoi(rlen));
		//printf("total len:%d point:%d \n",gSysData.zongchang,gSysData.bpoint);
		if(tlen>=atoi(rlen))
		{
			printf("break recv:%ld bpoint:%u\n",tlen,gSysData.bpoint);
			break;
		}
		
		if(plen>=64*1024)
		{
			SendData(sockfd,cmd[2],strlen(cmd[2]));
			//send(sockfd,  cmd[2],strlen(cmd[2]), 0);
			nwrite=fwrite(wbuf, sizeof(char), plen, recv_fp);
			memset(wbuf,0,sizeof(tbuf));
			gSysData.bpoint++;
			
			plen=0;
		}	
				 
	}
	nwrite+=fwrite(wbuf, sizeof(char), plen, recv_fp);
	
	fclose(recv_fp);
	gSysData.bpoint=0;
	SendData(sockfd,cmd[3],strlen(cmd[3]));
	//printf("recv:%d w:%d\t",tlen,nwrite);	
	if(tlen != atoi(rlen))
	{
	 	printf("len err!\n");
		SystemEx("rm re_UI.tgz -f");
	}
	else
	{
		SystemEx("mv /home/steam/.shimmer/re_CN.tgz /home/steam/.shimmer/CN.tgz");
	}	
    gSysData.download = 0;   
    close(sockfd);  
	ThreadExit();
	return NULL;
}

void *GetUserID(void*)
{  
	gSysData.bpoint =0;
	
        int sockfd = TcpInit();
	char send_buf[64] = {'\0'}; 
	char recv_buf[64] = {'\0'}; 
	snprintf(send_buf,sizeof(send_buf),"%s:%s",cmd[6],"test1");	
	
	TcpSend(sockfd,send_buf,strlen(send_buf));
	printf("GetUserPwd Send:%s\n",send_buf);
	
	TcpRecv(sockfd,recv_buf,sizeof(recv_buf),60);
	printf("GetUserPwd recv:%s\n",recv_buf);

	char *pwd = recv_buf;
	printf("recv :%s\n",pwd);
	
    	close(sockfd);  
	ThreadExit();
	return NULL;
}


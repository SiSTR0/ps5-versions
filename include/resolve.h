#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/sysctl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/mman.h>
#include <orbis/libkernel.h>
#include <orbis/libSceNet.h>
#include <orbis/libSceLibcInternal.h>
#include <orbis/libSceSysmodule.h>

#define SCE_NET_SOCKET_ABORT_FLAG_RCV_PRESERVATION 0x00000001
#define SCE_NET_SOCKET_ABORT_FLAG_SND_PRESERVATION 0x00000002
#define SCE_NET_SO_REUSEADDR 0x00000004
#define SCE_NET_ERROR_EINTR 0x80410104
#define SCE_NET_SOL_SOCKET 0xffff
#define UNUSED(x) (void)(x)

int (*f_sceNetCtlInit)(void);
void (*f_sceNetCtlTerm)(void);
int (*f_sceNetCtlGetInfo)(int code, SceNetCtlInfo *info);

int *sceNetErrnoLoc(void);

typeof(sceKernelLoadStartModule)* f_sceKernelLoadStartModule;
typeof(sceSysmoduleLoadModuleInternal)* f_sceSysmoduleLoadModuleInternal;
typeof(sceKernelGetModuleInfo)* f_sceKernelGetModuleInfo;
typeof(sceKernelDebugOutText)* f_sceKernelDebugOutText;
typeof(sceKernelUsleep)* f_sceKernelUsleep;
typeof(sceKernelSendNotificationRequest) * f_sceKernelSendNotificationRequest;
typeof(scePthreadMutexLock)* f_scePthreadMutexLock;
typeof(scePthreadMutexUnlock)* f_scePthreadMutexUnlock;
typeof(scePthreadExit)* f_scePthreadExit;
typeof(scePthreadMutexInit)* f_scePthreadMutexInit;
typeof(scePthreadCreate)* f_scePthreadCreate;
typeof(scePthreadMutexDestroy)* f_scePthreadMutexDestroy;
typeof(scePthreadJoin)* f_scePthreadJoin;
typeof(socket)* f_socket;
typeof(bind)* f_bind;
typeof(listen)* f_listen;
typeof(accept)* f_accept;
typeof(open)* f_open;
typeof(read)* f_read;
typeof(write)* f_write;
typeof(close)* f_close;
typeof(stat) * f_stat;
typeof(fstat) * f_fstat;
typeof(rename) * f_rename;
typeof(rmdir) * f_rmdir;
typeof(mkdir) * f_mkdir;
typeof(getdents) * f_getdents;
typeof(unlink) * f_unlink;
typeof(readlink) * f_readlink;
typeof(lseek) * f_lseek;
typeof(puts) * f_puts;
typeof(mmap) * f_mmap;
typeof(munmap) * f_munmap;
typeof(sceNetSocket) * f_sceNetSocket;
typeof(sceNetConnect) * f_sceNetConnect;
typeof(sceNetHtons) * f_sceNetHtons;
typeof(sceNetAccept) * f_sceNetAccept;
typeof(sceNetSend) * f_sceNetSend;
typeof(sceNetInetNtop) * f_sceNetInetNtop;
typeof(sceNetSocketAbort) * f_sceNetSocketAbort;
typeof(sceNetBind) * f_sceNetBind;
typeof(sceNetListen) * f_sceNetListen;
typeof(sceNetSocketClose) * f_sceNetSocketClose;
typeof(sceNetHtonl) * f_sceNetHtonl;
typeof(sceNetInetPton) * f_sceNetInetPton;
typeof(sceNetGetsockname) * f_sceNetGetsockname;
typeof(sceNetErrnoLoc) * f_sceNetErrnoLoc;
typeof(sceNetRecv) * f_sceNetRecv;
typeof(sceNetSetsockopt) * f_sceNetSetsockopt;
typeof(memset) * f_memset;
typeof(sprintf) * f_sprintf;
typeof(snprintf) * f_snprintf;
typeof(snprintf_s) * f_snprintf_s;
typeof(strcat) * f_strcat;
typeof(free) * f_free;
typeof(memcpy) * f_memcpy;
typeof(strcpy) * f_strcpy;
typeof(strncpy) * f_strncpy;
typeof(sscanf) * f_sscanf;
typeof(malloc) * f_malloc;
typeof(calloc) * f_calloc;
typeof(strlen) * f_strlen;
typeof(strcmp) * f_strcmp;
typeof(strrchr) * f_strrchr;
typeof(strchr) * f_strchr;
typeof(gmtime_s) * f_gmtime_s;
typeof(time) * f_time;
typeof(localtime) * f_localtime;
typeof(vsprintf) * f_vsprintf;
typeof(read) * f__read;
typeof(fork) * f_fork;
typeof(exit) * f_exit;
typeof(sysctlbyname) * f_sysctlbyname;

#define sce_net_errno (*f_sceNetErrnoLoc())

#define IP(a, b, c, d) (((a) << 0) + ((b) << 8) + ((c) << 16) + ((d) << 24))

#define PS4_IP "192.168.1.24"
#define PS4_PORT 1337

extern int netdbg_sock;

typedef int dlsym_t(int, const char*, void*);

struct payload_args
{
    dlsym_t* dlsym;             // 0x00
    int *rwpipe;                // 0x08
    int *rwpair;                // 0x10
    long kpipe_addr;            // 0x18
    long kdata_base_addr;       // 0x20
    int *payloadout;            // 0x28
};

#define debug(...) \
	do { \
		char debug_buffer[512]; \
		int size = f_sprintf(debug_buffer, ##__VA_ARGS__); \
		f_sceNetSend(netdbg_sock, debug_buffer, size, 0); \
	} while(0)

#define DEBUG(...) debug(__VA_ARGS__)
#define INFO(...) debug(__VA_ARGS__)
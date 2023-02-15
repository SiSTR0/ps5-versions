#include "resolve.h"

#define PAYLOAD_NAME "ps5-versions v1.0"

#define LIB_KERNEL_HANDLE 0x2001

void klog(const char* fmt, ...) {
    char buffer[512];

    va_list args;
    va_start(args, fmt);
    f_vsprintf(buffer, fmt, args);
    va_end(args);

    f_sceKernelDebugOutText(0, buffer);
}

// https://github.com/OSM-Made/PS4-Notify
void printf_notification(const char* fmt, ...) {
    SceNotificationRequest noti_buffer;

    va_list args;
    va_start(args, fmt);
    f_vsprintf(noti_buffer.message, fmt, args);
    va_end(args);

    noti_buffer.type = 0;
    noti_buffer.unk3 = 0;
    noti_buffer.use_icon_image_uri = 1;
    noti_buffer.target_id = -1;
    f_strcpy(noti_buffer.uri, "cxml://psnotification/tex_icon_system");

    f_sceKernelSendNotificationRequest(0, (SceNotificationRequest * ) & noti_buffer, sizeof(noti_buffer), 0);
}

int payload_main(struct payload_args *args) {
    dlsym_t *dlsym = args->dlsym;

    int libKernel = LIB_KERNEL_HANDLE;
    dlsym(libKernel, "sceKernelLoadStartModule", &f_sceKernelLoadStartModule);
    dlsym(libKernel, "sceKernelDebugOutText", &f_sceKernelDebugOutText);
    dlsym(libKernel, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);
    dlsym(libKernel, "sysctlbyname", &f_sysctlbyname);

    int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
    dlsym(libC, "vsprintf", &f_vsprintf);
    dlsym(libC, "strcpy", &f_strcpy);

    char version[256];
    uint32_t ps4_sdk_version, sdk_version, upd_version;
    size_t olden;

    olden = sizeof(version);
    f_sysctlbyname("kern.version", (char *)&version, &olden, NULL, 0);
    klog("kern.version: %s\n", version);

    olden = sizeof(ps4_sdk_version);
    f_sysctlbyname("kern.ps4_sdk_version", (char *)&ps4_sdk_version, &olden, NULL, 0);
    klog("kern.ps4_sdk_version: 0x%zx\n", ps4_sdk_version);

    olden = sizeof(sdk_version);
    f_sysctlbyname("kern.sdk_version", (char *)&sdk_version, &olden, NULL, 0);
    klog("kern.sdk_version: 0x%zx\n", sdk_version);

    olden = sizeof(upd_version);
    f_sysctlbyname("machdep.upd_version", (char *)&upd_version, &olden, NULL, 0);
    klog("machdep.upd_version: 0x%zx\n", upd_version);

    printf_notification(PAYLOAD_NAME"\n\nkern.version: %s\nkern.ps4_sdk_version: 0x%zx\nkern.sdk_version: 0x%zx\nmachdep.upd_version: 0x%zx", version, ps4_sdk_version, sdk_version, upd_version);

    return 0;
}

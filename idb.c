#include "mobiledevice.h"

#include <string.h>
#include <stdlib.h>

#define HDOC( ... ) #__VA_ARGS__ "\n";

#define CSTR2CFSTR(str) CFStringCreateWithCString(NULL, str, kCFStringEncodingUTF8)
#define CFSTR2CSTR(str) (char *)CFStringGetCStringPtr(str, CFStringGetSystemEncoding())
#define PRINT_DEVICE_VALUE(device, value) printf("%-40s: %s\n", value, (char *)CFStringGetCStringPtr(AMDeviceCopyValue(device, 0, CFSTR(value)), CFStringGetSystemEncoding()))

/************************************************************************************************/
enum CommandType
{
  PRINT_UDID,
  PRINT_INFO,
  PRINT_APPS,
  INSTALL,
  UNINSTLL,
  PRINT_SYSLOG
};
struct
{
  enum CommandType type;
  struct am_device_notification *notification;
  const char *app_path;
  const char *bundle_id;
} command;

typedef struct am_device *AMDeviceRef;

/************************************************************************************************/
/* Prototype */
static void on_device_notification(struct am_device_notification_callback_info *info, void *arg);
static void on_device_connected(AMDeviceRef device);

/* stats */
void print_udid(AMDeviceRef device);
void print_info(AMDeviceRef device);
void print_apps(AMDeviceRef device);
void print_syslog(AMDeviceRef device);

void install(AMDeviceRef device);
void uninstall(AMDeviceRef device);

/************************************************************************************************/

void register_notification()
{
  AMDeviceNotificationSubscribe(&on_device_notification, 0, 0, 0, &command.notification);
  CFRunLoopRun();
}
void unregister_notification(int status)
{
  AMDeviceNotificationUnsubscribe(command.notification);
  exit(status);
}

void connect(AMDeviceRef device)
{
  AMDeviceConnect(device);
  assert(AMDeviceIsPaired(device));
  assert(AMDeviceValidatePairing(device) == 0);
  assert(AMDeviceStartSession(device) == 0);
}
void disconnect(AMDeviceRef device)
{
  assert(AMDeviceStopSession(device) == 0);
  assert(AMDeviceDisconnect(device) == 0);
}
void connect_service(AMDeviceRef device, CFStringRef serviceName, unsigned int *serviceFd)
{
  connect(device);
  assert(AMDeviceStartService(device, serviceName, serviceFd, NULL) == 0);
  disconnect(device);
}
/************************************************************************************************/
/* Notification */
static void on_device_notification(struct am_device_notification_callback_info *info, void *arg) {
  switch (info->msg) {
  case ADNCI_MSG_CONNECTED:
    on_device_connected(info->dev);
    default:
      break;
  }
}
static void on_device_connected(AMDeviceRef device) {
  if (command.type == PRINT_UDID) {
    print_udid(device);
  } else if (command.type == PRINT_INFO) {
    print_info(device);
  } else if (command.type == PRINT_APPS) {
    print_apps(device);
  } else if (command.type == PRINT_SYSLOG) {
    print_syslog(device);
  } else if (command.type == INSTALL) {
    install(device);
  } else if (command.type == UNINSTLL) {
    uninstall(device);
  }
}
/************************************************************************************************/
/* Command Logic */

/************************************************
 idb udid
************************************************/
void print_udid(AMDeviceRef device)
{
  char *udid = CFSTR2CSTR(AMDeviceCopyDeviceIdentifier(device));
  if (udid == NULL)
  {
    unregister_notification(1);
  }
  printf("%s\n", udid);
  unregister_notification(0);
}
/************************************************
 idb info
************************************************/
void print_info(AMDeviceRef device)
{
  printf ("%s\n","[INFO]");
  connect(device);

  PRINT_DEVICE_VALUE(device, "BasebandStatus");
  PRINT_DEVICE_VALUE(device, "BasebandVersion");
  PRINT_DEVICE_VALUE(device, "BluetoothAddress");

  PRINT_DEVICE_VALUE(device, "BuildVersion");

  PRINT_DEVICE_VALUE(device, "CPUArchitecture");
  PRINT_DEVICE_VALUE(device, "DeviceClass");
  PRINT_DEVICE_VALUE(device, "DeviceColor");
  PRINT_DEVICE_VALUE(device, "DeviceName");
  PRINT_DEVICE_VALUE(device, "FirmwareVersion");
  PRINT_DEVICE_VALUE(device, "HardwareModel");
  PRINT_DEVICE_VALUE(device, "HardwarePlatform");
  PRINT_DEVICE_VALUE(device, "IntegratedCircuitCardIdentity");
  PRINT_DEVICE_VALUE(device, "InternationalMobileSubscriberIdentity");
  PRINT_DEVICE_VALUE(device, "MLBSerialNumber");

  PRINT_DEVICE_VALUE(device, "MobileSubscriberCountryCode");
  PRINT_DEVICE_VALUE(device, "MobileSubscriberNetworkCode");
  PRINT_DEVICE_VALUE(device, "ModelNumber");
  PRINT_DEVICE_VALUE(device, "PhoneNumber");

  PRINT_DEVICE_VALUE(device, "ProductType");
  PRINT_DEVICE_VALUE(device, "ProductVersion");
  PRINT_DEVICE_VALUE(device, "ProtocolVersion");

  PRINT_DEVICE_VALUE(device, "RegionInfo");
  PRINT_DEVICE_VALUE(device, "SerialNumber");
  PRINT_DEVICE_VALUE(device, "SIMStatus");

  PRINT_DEVICE_VALUE(device, "TimeZone");
  PRINT_DEVICE_VALUE(device, "UniqueDeviceID");
  PRINT_DEVICE_VALUE(device, "WiFiAddress");
  
  unregister_notification(0);  
}
/************************************************
 idb apps
************************************************/
static void print_bundle_id(const void *key, const void *value, void *context)
{
  if ((key == NULL) || (value == NULL))
  {
    return;
  }
  char *bundle_id = CFSTR2CSTR((CFStringRef)key);
  if (bundle_id != NULL)
  {
    printf("%s\n", bundle_id);
  }
//  CFShow(value);
}

void print_apps(AMDeviceRef device)
{
  connect(device);
  CFDictionaryRef apps;
  AMDeviceLookupApplications(device, 0, &apps);
//  CFShow(apps);
  CFDictionaryApplyFunction(apps, print_bundle_id, NULL);
  CFRelease(apps);

  unregister_notification(0);  
}
/************************************************
 idb lob
************************************************/
void print_syslog(AMDeviceRef device)
{
  printf ("%s\n","print_syslog");
  unregister_notification(0);
}
/************************************************
 idb install 
************************************************/
static void on_transfer(CFDictionaryRef dict, int arg) {
  int percent;
  CFStringRef status = CFDictionaryGetValue(dict, CFSTR("Status"));
  CFNumberGetValue(CFDictionaryGetValue(dict, CFSTR("PercentComplete")), kCFNumberSInt32Type, &percent);
  
  printf("[%3d%%] %s\n", percent / 2, CFStringGetCStringPtr(status, kCFStringEncodingMacRoman));
}
static void on_install(CFDictionaryRef dict, int arg) {
  int percent;
  CFStringRef status = CFDictionaryGetValue(dict, CFSTR("Status"));
  CFNumberGetValue(CFDictionaryGetValue(dict, CFSTR("PercentComplete")), kCFNumberSInt32Type, &percent);
  
  printf("[%3d%%] %s\n", (percent / 2) + 50, CFStringGetCStringPtr(status, kCFStringEncodingMacRoman));
}
void install(AMDeviceRef device)
{
  printf("[%3d%%] Start: %s\n", 0, command.app_path);  

  CFStringRef path = CSTR2CFSTR(command.app_path);
  CFStringRef keys[] = { CFSTR("PackageType") }, values[] = { CFSTR("Developer") };
  CFDictionaryRef options = CFDictionaryCreate(NULL, (const void **)&keys, (const void **)&values, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  /* Transfer */
  unsigned int afcFd;
  connect_service(device, AMSVC_AFC, &afcFd);
  assert(AMDeviceTransferApplication(afcFd, path, NULL, on_transfer, 0) == 0);
  close(afcFd);
  
  /* Install */
  unsigned int installFd;
  connect_service(device, AMSVC_INSTALLATION_PROXY, &installFd);
  mach_error_t result = AMDeviceInstallApplication(installFd, path, options, on_install, 0);
  if (result != 0)
  {
    printf("AMDeviceInstallApplication failed: %d\n", result);
    unregister_notification(1);
  }
  close(installFd);
  
  CFRelease(path);
  CFRelease(options);

  printf("[100%%] Installed app: %s\n", command.app_path);  
  unregister_notification(0);
}

/************************************************
 idb uninstall
************************************************/
void uninstall(AMDeviceRef device)
{
  CFStringRef bundle_id = CSTR2CFSTR(command.bundle_id);
  connect(device);
  
  mach_error_t result = AMDeviceSecureUninstallApplication(0,device, bundle_id, 0, NULL, 0);
  if (result != 0)
  {
    printf("AMDeviceSecureUnInstallApplication failed: %d\n", result);
    unregister_notification(1);
  }
  
  printf("Uninstalled bundle_id: %s\n", command.bundle_id);  
  unregister_notification(0);
}
/************************************************************************************************/
/* Main */
void usage()
{
  char* str = HDOC(
    Usage:idb <command>\n
    command is below \n
    - udid \n
    - info \n
    - install <app or ipa>
    - uninstall <bundle_id>
    - apps
  );
  printf("%s\n", str);
}

int main (int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    exit(1);
  }
  if ((argc == 2) && (strcmp(argv[1], "udid") == 0)) {
    command.type = PRINT_UDID;
  } else if ((argc == 2) && (strcmp(argv[1], "info") == 0)) {
    command.type = PRINT_INFO;
  } else if ((argc == 2) && (strcmp(argv[1], "apps") == 0)) {
    command.type = PRINT_APPS;
  } else if ((argc == 2) && (strcmp(argv[1], "log") == 0)) {
    command.type = PRINT_SYSLOG;
  } else if ((argc == 3) && (strcmp(argv[1], "install") == 0)) {
    command.type = INSTALL;
    command.app_path = argv[2];
  } else if ((argc == 3) && (strcmp(argv[1], "uninstall") == 0)) {
    command.type = UNINSTLL;
    command.bundle_id = argv[2];
  } else {
    fprintf(stderr, "Unknown command\n");
    usage();
    exit(1);
  }
  AMDSetLogLevel(5);
  AMDAddLogFileDescriptor(fileno(stderr));
  register_notification();
  return 0;
}

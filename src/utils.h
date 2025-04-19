/**
 * @file      utils.h
 * @brief     Display
 * @version   1.0.0
 * @author    Vinodh Kumar M.
 * @date      2025-04-06
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/04/06  <td>1.0      <td>Vinodh Kumar M.  <td>Initial
 * </table>
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <linux/unistd.h> /* for _syscallX macros/related stuff */
#include <linux/kernel.h> /* for struct sysinfo */
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <systemd/sd-journal.h>

// "interface" is "eth0" or "wlan0"
char *getIPAddr(char *ip_address, char *interface);
char *getCurrentDateTime(char *buffer);
// "uptime" should be of sufficent length
char *get_uptime(char *str_uptime);
char *get_CPU_load(char *str_load);
char *get_CPU_temp(char *str_cpu_temp);
int net_status(void);
char *GetCPULoad(char *str_load);
int is_process_running(char *process_name);

#endif

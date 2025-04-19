/**
 * @file      utils.c
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

#include "utils.h"

/*
 * Get IP Address
 */
char *getIPAddr(char *ip_address, char *interface)
{
	struct ifreq ifr;
	int fd = 0;

	if (fd = socket(AF_INET, SOCK_DGRAM, 0))
	{
		/* I want to get an IPv4 IP address */
		ifr.ifr_addr.sa_family = AF_INET;

		/* I want IP address attached to "interface" */
		strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
		ioctl(fd, SIOCGIFADDR, &ifr);

		strncpy(ip_address, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), 20 - 1);

		close(fd);
	}
	else
	{
		sd_journal_print(LOG_ERR, "Socket fd error!");
	}
	return ip_address;
}

char *getCurrentDateTime(char *buffer)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	strftime(buffer, 30 - 1, "%H:%M %d-%m-%Y", tm);
	return buffer;
}

char *get_uptime(char *str_uptime)
{
	struct sysinfo s_info;
	int error = sysinfo(&s_info);

	if (!error)
	{
		long days = s_info.uptime / (24 * 3600);
		long hours = (s_info.uptime % (24 * 3600)) / (3600);
		snprintf(str_uptime, 20 - 1, "%ld d, %ld h", days, hours);
	}
	else
	{
		sd_journal_print(LOG_ERR, "Error getting uptime: %d", error);
	}

	return str_uptime;
}

char *get_CPU_load(char *str_load)
{
	struct sysinfo s_info;
	float shift_float = (float)(1 << SI_LOAD_SHIFT);

	int error = sysinfo(&s_info);
	if (!error)
	{
		float load = (s_info.loads[2] / shift_float);
		snprintf(str_load, 20 - 1, "%0.0f %%", (double)(load * 100 / get_nprocs()));
	}
	else
	{
		sd_journal_print(LOG_ERR, "Error in get_CPU_load(): %d", error);
	}

	return str_load;
}

char *get_CPU_temp(char *str_cpu_temp)
{
	FILE *temperatureFile = NULL;
	double T;

	temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (temperatureFile == NULL)
	{
		sd_journal_print(LOG_ERR, "Error opening file in get_CPU_temp()");
	}
	else
	{
		if (fscanf(temperatureFile, "%lf", &T))
		{
			T /= 1000;
			snprintf(str_cpu_temp, 20 - 1, "%0.0f\"C", T);
		}
		else
		{
			sd_journal_print(LOG_ERR, "fscanf() error in get_CPU_temp()");
		}
		fclose(temperatureFile);
	}

	return str_cpu_temp;
}

int net_status(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = {AF_INET, htons(53), inet_addr("8.8.8.8")};
	int retval = -2;

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	if (sockfd)
	{
		setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
		retval = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
		close(sockfd);
	}

	return retval; // arbitrary return value
}

char *GetCPULoad(char *str_load)
{
	FILE *FileHandler;
	char FileBuffer[1024];
	float load;

	FileHandler = fopen("/proc/loadavg", "r");

	if (!FileHandler)
	{
		sd_journal_print(LOG_ERR, "fopen() error: %d", FileHandler);
	}
	else
	{
		if (fgets(FileBuffer, sizeof(FileBuffer) - 1, FileHandler) != NULL)
		{
			if (sscanf(FileBuffer, "%f", &load))
			{
				snprintf(str_load, 20 - 1, "%0.0f %%", (double)(load * 100 / get_nprocs()));
			}
			else
			{
				sd_journal_perror("sscanf() error in GetCPULoad()");
			}
		}
		else
		{
			sd_journal_perror("fgets() error in GetCPULoad()");
		}
		fclose(FileHandler);
	}

	return str_load;
}

int is_process_running(char *process_name)
{
	if (!process_name)
		return 1;

	char cmd[1024];
	strcat(strcat(strcpy(cmd, "pidof -x "), process_name), " > /dev/null");
	return system(cmd);
}

/*****************************************************************************
* | File      	:		EPD_1in54_V2_test.c
* | Author      :   Waveshare team
* | Function    :   1.54inch e-paper test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

#include "EPD_1in54_V2.h"
#include "EPD_1in54_V2_test.h"
#include "utils.h"
#include "i2c_utils.h"
#include <time.h>
#include <stdlib.h> //exit()
#include <signal.h> //signal()
#include <sys/resource.h>
#include <systemd/sd-journal.h>

// Required for getting version from CMAKE
#define Q(x) #x
#define QUOTE(x) Q(x)

extern const char *__progname;

extern UBYTE *BlackImage;

#define CHECK_PROCESS_STATUS "bitcoind"

#define LINE_HEIGHT_ADJUSTMENT 5

int EPD_1in54_V2_test(void)
{
	char ip_address[20] = "";  // 16
	char time_buffer[30] = ""; // 26
	char uptime[20] = "";
	char load[20] = "";
	char cpu_temp[20] = "";
	int battery_percentage = 0;
	char battery[20] = "";
	int netstatus = 0;
	char process_status[1024] = "";
	clock_t tic = clock();
	clock_t toc = clock();
	struct timeval start_s;
	struct timeval start_u;
	struct timeval end_s;
	struct timeval end_u;
	bool on_battery;
	int time_remaining_or_to_full = 0;
	int time_ups_hours = 0;
	int time_ups_miniutes = 0;
	char ups_time[20] = "";

	int who = RUSAGE_SELF;
	struct rusage usage;
	int ret;

	if (!(ret = getrusage(who, &usage)))
	{
		start_s = usage.ru_stime;
		start_u = usage.ru_utime;
	}

	if (DEV_Module_Init() != 0)
	{
		return -1;
	}

	EPD_1IN54_V2_Init();
	EPD_1IN54_V2_Clear();
	DEV_Delay_ms(100);

	// Create a new image cache
	UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
	if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
	{
		sd_journal_perror("Failed to apply for black memory...");
		return -1;
	}
	Paint_NewImage(BlackImage, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 270, WHITE);
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);

	EPD_1IN54_V2_DisplayPartBaseImage(BlackImage);
	EPD_1IN54_V2_Init_Partial();
	Paint_SelectImage(BlackImage);

	strcpy(process_status, "Process [");
	strcat(process_status, CHECK_PROCESS_STATUS);
	strcat(process_status, "]: %d");

	sd_journal_print(LOG_INFO, "[%s v%s] started", __progname, QUOTE(PROJECT_VERSION));

	while (1)
	{
		getCurrentDateTime(time_buffer);
		getIPAddr(ip_address, "wlan0");
		get_uptime(uptime);
		GetCPULoad(load);
		get_CPU_temp(cpu_temp);
		netstatus = net_status();
		battery_percentage = i2c_read_reg(I2C_ADDR_BATTERY_PERCENT);
		snprintf(battery, 20 - 1, "%d %%", battery_percentage);
		on_battery = !(i2c_read_reg(I2C_ADDR_CHARGE_STATUS) & I2C_MASK_VBUS_POWERED); // should be '1' if on battery

		if (on_battery)
			time_remaining_or_to_full = i2c_read_reg(I2C_ADDR_BATTERY_REMAINING_DISCHARGE_TIME);
		else
			time_remaining_or_to_full = i2c_read_reg(I2C_ADDR_BATTERY_REMAINING_CHARGE_TIME);

		if (time_remaining_or_to_full >= 0xBB80) // 800 hours
		{
			time_remaining_or_to_full = 0;
		}

		time_ups_hours = (int)time_remaining_or_to_full / 60;
		time_ups_miniutes = (int)time_remaining_or_to_full % 60;

		snprintf(ups_time, 20 - 1, time_ups_hours ? "%d h" : "%d min", time_ups_hours ? time_remaining_or_to_full / 60 : time_remaining_or_to_full % 60);

		DEBUG("It's now %s", time_buffer);
		DEBUG("IP: %s", ip_address);
		DEBUG("Uptime: %s", uptime);
		DEBUG("CPU load: %s", load);
		DEBUG("CPU temp: %s", cpu_temp);
		DEBUG("Internet: %d", netstatus);
		DEBUG(process_status, is_process_running(CHECK_PROCESS_STATUS));
		DEBUG("Battery %: %s", battery);

		Paint_ClearWindows(2, 2, EPD_1IN54_V2_WIDTH - 2, EPD_1IN54_V2_HEIGHT - 2, WHITE);
		//
		Paint_DrawRectangle(2, 2, EPD_1IN54_V2_WIDTH - 2, 21, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Paint_DrawString_EN((EPD_1IN54_V2_WIDTH - Font16.Width * 16) / 2, 4, time_buffer, &Font16, BLACK, WHITE);
		//
		Paint_DrawString_EN(5, 28, "IP Address", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH - strlen(ip_address) * Font20.Width - 5, 28 + 17, ip_address, &Font20, WHITE, BLACK);
		Paint_DrawLine(2, 70 - 4, EPD_1IN54_V2_WIDTH - 2, 70 - 4, BLACK, 1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 70, "Uptime", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH - strlen(uptime) * Font20.Width - 5, 70 + 17, uptime, &Font20, WHITE, BLACK);
		Paint_DrawLine(2, 112 - 4, EPD_1IN54_V2_WIDTH - 2, 112 - 4, BLACK, 1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 112, "CPU Load", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH / 2 - strlen(load) * Font20.Width - 5 - 15, 112 + 17, load, &Font20, WHITE, BLACK);
		//
		Paint_DrawString_EN(5 + EPD_1IN54_V2_WIDTH / 2 - 15, 112, on_battery ? "On Battery" : "Battery", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH - strlen(battery) * Font20.Width - 5, 112 + 17, battery, &Font20, WHITE, GRAY4);
		Paint_DrawLine(2, 154 - 4, EPD_1IN54_V2_WIDTH - 2, 154 - 4, BLACK, 1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 154, "CPU Temp.", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH / 2 - strlen(cpu_temp) * Font20.Width - 5 - 15, 154 + 17, cpu_temp, &Font20, WHITE, BLACK);
		//
		Paint_DrawString_EN(5 + EPD_1IN54_V2_WIDTH / 2 - 15, 154, on_battery ? "Time To Empty" : "Time To Full", &Font12, WHITE, GRAY4);
		Paint_DrawString_EN(EPD_1IN54_V2_WIDTH - strlen(ups_time) * Font20.Width - 5, 154 + 17, ups_time, &Font20, WHITE, BLACK);
		Paint_DrawLine(2, 196 - 4, EPD_1IN54_V2_WIDTH - 2, 196 - 4, BLACK, 1, LINE_STYLE_SOLID);
		//
		Paint_DrawLine(EPD_1IN54_V2_WIDTH / 2 - 15, 112 - 4, EPD_1IN54_V2_WIDTH / 2 - 15, EPD_1IN54_V2_HEIGHT - 10, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

		if (netstatus)
		{
			// Paint_DrawChar(5, 28 + 17, 'X', &Font20, BLACK, WHITE);
			Paint_DrawString_EN(5, 28 + 17, "X", &Font16, WHITE, BLACK);
		}
		else
		{
			// Paint_DrawChar(5, 28 + 17, '~', &Font20, BLACK, WHITE);
			Paint_DrawString_EN(5, 28 + 17, "~", &Font16, WHITE, BLACK);
		}
		DEBUG("drawing...");
		EPD_1IN54_V2_DisplayPart(BlackImage);

		toc = clock();
		DEBUG("%f seconds elaspsed.", (double)((toc - tic) / CLOCKS_PER_SEC));

		if (!(ret = getrusage(who, &usage)))
		{

			end_s = usage.ru_stime;
			end_u = usage.ru_utime;
		}

		DEBUG(LOG_INFO, "%s getrusage() gives %d seconds.", __progname, end_s.tv_sec + end_u.tv_sec - start_s.tv_sec - start_u.tv_sec);

		sleep(10);
	}

	return 0;
}

void Handler(int signo)
{
	fprintf(stdout, "\nExiting... please wait.\n");

	EPD_1IN54_V2_Init();
	Paint_Clear(WHITE);
	EPD_1IN54_V2_Clear();

	sd_journal_print(LOG_INFO, "Module sleep...");
	EPD_1IN54_V2_Sleep();
	free(BlackImage);
	BlackImage = NULL;
	DEV_Delay_ms(2000); // important, at least 2s

	sd_journal_print(LOG_INFO, "Close 5V. Module enters 0 power consumption...");
	DEV_Module_Exit();

	if (signo)
	{
		sd_journal_print(LOG_INFO, "[%s] exited", __progname);
	}

	exit(0);
}

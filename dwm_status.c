/*
 * dwm statusbar in C
 * most snippets found here: http://dwm.suckless.org/status_monitor/
 */

#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#include <X11/Xlib.h>
#include <mpd/client.h>

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *buf = NULL;

	va_start(fmtargs, fmt);
	if (vasprintf(&buf, fmt, fmtargs) == -1){
		fprintf(stderr, "malloc vasprintf\n");
		exit(1);
	}
	va_end(fmtargs);

	return buf;
}

char *
get_clock(char *fmt)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

        setenv("TZ", "Europe/Berlin", 1);

	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL)		
		return smprintf("");
     
     	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
      		fprintf(stderr, "strftime == 0\n");
     		return smprintf("");
      	}
      
      	return smprintf("%s", buf);
}

char *
get_mpdstat() {
	struct mpd_song * song = NULL;
	const char * title = NULL;
	const char * artist = NULL;
	char * retstr = NULL;
	int elapsed = 0, total = 0;

	struct mpd_connection * conn ;
	if (!(conn = mpd_connection_new("localhost", 0, 30000)) ||
			mpd_connection_get_error(conn)){
		return smprintf("");
	}

	mpd_command_list_begin(conn, true);
	mpd_send_status(conn);
	mpd_send_current_song(conn);
	mpd_command_list_end(conn);

	struct mpd_status* theStatus = mpd_recv_status(conn);
	if ((theStatus) && (mpd_status_get_state(theStatus) == MPD_STATE_PLAY)) {
		mpd_response_next(conn);
		song = mpd_recv_song(conn);
		title = smprintf("%s",mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
		artist = smprintf("%s",mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));

		elapsed = mpd_status_get_elapsed_time(theStatus);
		total = mpd_status_get_total_time(theStatus);
		mpd_song_free(song);
		retstr = smprintf("%s - %s •", artist, title);
		free((char*)title);
		free((char*)artist);
	} else {
                retstr = smprintf("");
        }

	mpd_response_finish(conn);
	mpd_connection_free(conn);

	return retstr;
}

void
set_status(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

int
get_nmail(char *directory)
{
	int n = 0;
	DIR* dir = NULL;
	struct dirent* rf = NULL;

	dir = opendir(directory);
	if (dir == NULL)
		perror("");

	while ((rf = readdir(dir)) != NULL)
	{
		if (strcmp(rf->d_name, ".") != 0 &&
				strcmp(rf->d_name, "..") != 0)
			n++;
	}
	closedir(dir);
	
	return n;
}

int
main(void)
{
	char *status;
	char *tmbln;
	char *sng;
	int g = 0, p = 0, nmails = 0;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(1)) {

		g = get_nmail("/home/aphorismenoi/mail/gmx/INBOX/new");
		p = get_nmail("/home/aphorismenoi/mail/posteo/INBOX/new");
                nmails = g + p;

		tmbln = get_clock("%a %d %H:%M");
		sng = get_mpdstat();
		
		if (nmails == 0) {
			status = smprintf("%s %s", sng, tmbln);
		} else {
			status = smprintf("%s %d new • %s", sng, nmails, tmbln);
		}

		set_status(status);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

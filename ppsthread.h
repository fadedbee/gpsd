/*
 * This file is Copyright (c) 2015 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 */

#ifndef PPSTHREAD_H
#define PPSTHREAD_H

/* use RFC 2782 PPS API */
/* this needs linux >= 2.6.34 and
 * CONFIG_PPS=y
 * CONFIG_PPS_DEBUG=y  [optional to kernel log pulses]
 * CONFIG_PPS_CLIENT_LDISC=y
 */
#ifndef S_SPLINT_S
#if defined(HAVE_SYS_TIMEPPS_H)
// include unistd.h here as it is missing on older pps-tools releases.
// 'close' is not defined otherwise.
#include <unistd.h>
#include <sys/time.h>
#include <sys/timepps.h>
#endif
#endif /* S_SPLINT_S */

#ifndef TIMEDELTA_DEFINED
#define TIMEDELTA_DEFINED
struct timedelta_t {
    struct timespec	real;
    struct timespec	clock;
};
#endif /* TIMEDELTA_DEFINED */


/* difference between timespecs in nanoseconds */
/* int is too small, avoid floats  */
/* WARNING!  this will overflow if x and y differ by more than a few seconds */
#define timespec_diff_ns(x, y)	(long)(((x).tv_sec-(y).tv_sec)*1000000000+(x).tv_nsec-(y).tv_nsec)

struct pps_thread_t {
    struct timespec fixin_real;		/* in-band time of the fix */
    struct timespec fixin_clock;	/* system clock time when fix received */
#if defined(HAVE_SYS_TIMEPPS_H)
    pps_handle_t kernelpps_handle;
#endif /* defined(HAVE_SYS_TIMEPPS_H) */
    int devicefd;			/* device file descriptor */
    char *devicename;
    /*@null@*/ char *(*report_hook)(volatile struct pps_thread_t *,
				    struct timedelta_t *);
    /*@null@*/ void (*wrap_hook)(volatile struct pps_thread_t *);
    struct timedelta_t ppsout_last;
    int ppsout_count;
    /*@null@*/ void (*pps_hook)(volatile struct pps_thread_t *, struct timedelta_t *);
    /*@null@*/ void (*log_hook)(volatile struct pps_thread_t *, int errlevel, const char *fmt, ...);
    void *context;
};

#define THREAD_ERROR	0
#define THREAD_WARN	1
#define THREAD_INF	2
#define THREAD_PROG	3
#define THREAD_RAW	4

extern void pps_thread_activate(volatile struct pps_thread_t *);
extern void pps_thread_deactivate(volatile struct pps_thread_t *);
extern void pps_thread_stash_fixtime(volatile struct pps_thread_t *, 
			      struct timespec, struct timespec);
extern int pps_thread_lastpps(volatile struct pps_thread_t *,
			      struct timedelta_t *);

#endif /* PPSTHREAD_H */
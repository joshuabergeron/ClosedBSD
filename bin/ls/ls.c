/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1989, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)ls.c	8.5 (Berkeley) 4/2/94";
#else
static const char rcsid[] =
  "$FreeBSD: src/bin/ls/ls.c,v 1.32.2.3 2000/07/12 06:19:14 assar Exp $";
#endif
#endif /* not lint */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fts.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef COLORLS
#include <termcap.h>
#include <signal.h>
#endif

#include "ls.h"
#include "extern.h"

/*
 * Upward approximation of the maximum number of characters needed to
 * represent a value of integral type t as a string, excluding the
 * NUL terminator, with provision for a sign.
 */
#define	STRBUF_SIZEOF(t)	(1 + CHAR_BIT * sizeof(t) / 3 + 1)

static void	 display __P((FTSENT *, FTSENT *));
static u_quad_t	 makenines __P((u_long));
static int	 mastercmp __P((const FTSENT **, const FTSENT **));
static void	 traverse __P((int, char **, int));

static void (*printfcn) __P((DISPLAY *));
static int (*sortfcn) __P((const FTSENT *, const FTSENT *));

long blocksize;			/* block size units */
int termwidth = 80;		/* default terminal width */

/* flags */
int f_accesstime;		/* use time of last access */
int f_column;			/* columnated format */
int f_flags;			/* show flags associated with a file */
int f_inode;			/* print inode */
int f_kblocks;			/* print size in kilobytes */
int f_listdir;			/* list actual directory, not contents */
int f_listdot;			/* list files beginning with . */
int f_longform;			/* long listing format */
int f_nonprint;			/* show unprintables as ? */
int f_nosort;			/* don't sort output */
int f_notabs;			/* don't use tab-separated multi-col output */
int f_numericonly;		/* don't convert uid/gid to name */
int f_octal;			/* show unprintables as \xxx */
int f_octal_escape;		/* like f_octal but use C escapes if possible */
int f_recursive;		/* ls subdirectories also */
int f_reversesort;		/* reverse whatever sort is used */
int f_sectime;			/* print the real time for all files */
int f_singlecol;		/* use single column output */
int f_size;			/* list size in short listing */
int f_statustime;		/* use time of last mode change */
int f_timesort;			/* sort by time vice name */
int f_type;			/* add type character for non-regular files */
int f_whiteout;			/* show whiteout entries */
#ifdef COLORLS
int f_color;			/* add type in color for non-regular files */

char *ansi_bgcol;		/* ANSI sequence to set background colour */
char *ansi_fgcol;		/* ANSI sequence to set foreground colour */
char *ansi_coloff;		/* ANSI sequence to reset colours */
#endif

int rval;

int
main(argc, argv)
	int argc;
	char *argv[];
{
	static char dot[] = ".", *dotav[] = { dot, NULL };
	struct winsize win;
	int ch, fts_options, notused;
	char *p;

#ifdef COLORLS
	char termcapbuf[1024];		/* termcap definition buffer */
	char tcapbuf[512];		/* capability buffer */
	char *bp = tcapbuf;
#endif

	(void) setlocale(LC_ALL, "");

	/* Terminal defaults to -Cq, non-terminal defaults to -1. */
	if (isatty(STDOUT_FILENO)) {
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1 ||
		    !win.ws_col) {
			if ((p = getenv("COLUMNS")) != NULL)
				termwidth = atoi(p);
		}
		else
			termwidth = win.ws_col;
		f_column = f_nonprint = 1;
	} else {
		f_singlecol = 1;
		/* retrieve environment variable, in case of explicit -C */
		if ((p = getenv("COLUMNS")))
			termwidth = atoi(p);
	}

	/* Root is -A automatically. */
	if (!getuid())
		f_listdot = 1;

	fts_options = FTS_PHYSICAL;
	while ((ch = getopt(argc, argv, "1ABCFGHLPRTWabcdfgiklnoqrstu")) != -1) {
		switch (ch) {
		/*
		 * The -1, -C and -l options all override each other so shell
		 * aliasing works right.
		 */
		case '1':
			f_singlecol = 1;
			f_column = f_longform = 0;
			break;
		case 'B':
			f_nonprint = 0;
			f_octal = 1;
		        f_octal_escape = 0;
			break;
		case 'C':
			f_column = 1;
			f_longform = f_singlecol = 0;
			break;
		case 'l':
			f_longform = 1;
			f_column = f_singlecol = 0;
			break;
		/* The -c and -u options override each other. */
		case 'c':
			f_statustime = 1;
			f_accesstime = 0;
			break;
		case 'u':
			f_accesstime = 1;
			f_statustime = 0;
			break;
		case 'F':
			f_type = 1;
			break;
		case 'H':
		        fts_options |= FTS_COMFOLLOW;
			break;
		case 'G':
			if (isatty(STDOUT_FILENO))
#ifdef COLORLS
				if (tgetent(termcapbuf, getenv("TERM")) == 1) {
					ansi_fgcol = tgetstr("AF", &bp);
					ansi_bgcol = tgetstr("AB", &bp);

					/* To switch colours off use 'op' if
					 * available, otherwise use 'oc', or
					 * don't do colours at all. */
					ansi_coloff = tgetstr("op", &bp);
					if (!ansi_coloff)
						ansi_coloff = tgetstr("oc", &bp);
					if (ansi_fgcol && ansi_bgcol && ansi_coloff)
						f_color = 1;
				}
#else
				(void)fprintf(stderr, "Color support not compiled in.\n");
#endif
			break;
		case 'L':
			fts_options &= ~FTS_PHYSICAL;
			fts_options |= FTS_LOGICAL;
			break;
		case 'P':
		        fts_options &= ~FTS_COMFOLLOW;
			fts_options &= ~FTS_LOGICAL;
			fts_options |= FTS_PHYSICAL;
			break;
		case 'R':
			f_recursive = 1;
			break;
		case 'a':
			fts_options |= FTS_SEEDOT;
			/* FALLTHROUGH */
		case 'A':
			f_listdot = 1;
			break;
		/* The -d option turns off the -R option. */
		case 'd':
			f_listdir = 1;
			f_recursive = 0;
			break;
		case 'f':
			f_nosort = 1;
			break;
		case 'g':		/* Compatibility with 4.3BSD. */
			break;
		case 'i':
			f_inode = 1;
			break;
		case 'k':
			f_kblocks = 1;
			break;
		case 'n':
			f_numericonly = 1;
			break;
		case 'o':
			f_flags = 1;
			break;
		case 'q':
			f_nonprint = 1;
			f_octal = 0;
		        f_octal_escape = 0;
			break;
		case 'r':
			f_reversesort = 1;
			break;
		case 's':
			f_size = 1;
			break;
		case 'T':
			f_sectime = 1;
			break;
		case 't':
			f_timesort = 1;
			break;
		case 'W':
			f_whiteout = 1;
			break;
		case 'b':
			f_nonprint = 0;
		        f_octal = 0;
			f_octal_escape = 1;
			break;
		default:
		case '?':
			usage();
		}
	}
	argc -= optind;
	argv += optind;

#ifdef COLORLS
	if (f_color) {
		/*
		 * We can't put tabs and color sequences together:
		 * column number will be incremented incorrectly
		 * for "stty oxtabs" mode.
		 */
		f_notabs = 1;
		(void) signal(SIGINT, colorquit);
		(void) signal(SIGQUIT, colorquit);
		parsecolors(getenv("LSCOLORS"));
	}
#endif

	/*
	 * If not -F, -i, -l, -s or -t options, don't require stat
	 * information, unless in color mode in which case we do
	 * need this to determine which colors to display.
	 */
	if (!f_inode && !f_longform && !f_size && !f_timesort && !f_type
#ifdef COLORLS
	    && !f_color
#endif
	   )
		fts_options |= FTS_NOSTAT;

	/*
	 * If not -F, -d or -l options, follow any symbolic links listed on
	 * the command line.
	 */
	if (!f_longform && !f_listdir && !f_type)
		fts_options |= FTS_COMFOLLOW;

	/*
	 * If -W, show whiteout entries
	 */
#ifdef FTS_WHITEOUT
	if (f_whiteout)
		fts_options |= FTS_WHITEOUT;
#endif

	/* If -l or -s, figure out block size. */
	if (f_longform || f_size) {
		if (f_kblocks)
			blocksize = 2;
		else {
			(void)getbsize(&notused, &blocksize);
			blocksize /= 512;
		}
	}

	/* Select a sort function. */
	if (f_reversesort) {
		if (!f_timesort)
			sortfcn = revnamecmp;
		else if (f_accesstime)
			sortfcn = revacccmp;
		else if (f_statustime)
			sortfcn = revstatcmp;
		else /* Use modification time. */
			sortfcn = revmodcmp;
	} else {
		if (!f_timesort)
			sortfcn = namecmp;
		else if (f_accesstime)
			sortfcn = acccmp;
		else if (f_statustime)
			sortfcn = statcmp;
		else /* Use modification time. */
			sortfcn = modcmp;
	}

	/* Select a print function. */
	if (f_singlecol)
		printfcn = printscol;
	else if (f_longform)
		printfcn = printlong;
	else
		printfcn = printcol;

	if (argc)
		traverse(argc, argv, fts_options);
	else
		traverse(1, dotav, fts_options);
	exit(rval);
}

static int output;			/* If anything output. */

/*
 * Traverse() walks the logical directory structure specified by the argv list
 * in the order specified by the mastercmp() comparison function.  During the
 * traversal it passes linked lists of structures to display() which represent
 * a superset (may be exact set) of the files to be displayed.
 */
static void
traverse(argc, argv, options)
	int argc, options;
	char *argv[];
{
	FTS *ftsp;
	FTSENT *p, *chp;
	int ch_options;

	if ((ftsp =
	    fts_open(argv, options, f_nosort ? NULL : mastercmp)) == NULL)
		err(1, NULL);

	display(NULL, fts_children(ftsp, 0));
	if (f_listdir)
		return;

	/*
	 * If not recursing down this tree and don't need stat info, just get
	 * the names.
	 */
	ch_options = !f_recursive && options & FTS_NOSTAT ? FTS_NAMEONLY : 0;

	while ((p = fts_read(ftsp)) != NULL)
		switch (p->fts_info) {
		case FTS_DC:
			warnx("%s: directory causes a cycle", p->fts_name);
			break;
		case FTS_DNR:
		case FTS_ERR:
			warnx("%s: %s", p->fts_name, strerror(p->fts_errno));
			rval = 1;
			break;
		case FTS_D:
			if (p->fts_level != FTS_ROOTLEVEL &&
			    p->fts_name[0] == '.' && !f_listdot)
				break;

			/*
			 * If already output something, put out a newline as
			 * a separator.  If multiple arguments, precede each
			 * directory with its name.
			 */
			if (output)
				(void)printf("\n%s:\n", p->fts_path);
			else if (argc > 1) {
				(void)printf("%s:\n", p->fts_path);
				output = 1;
			}

			chp = fts_children(ftsp, ch_options);
			display(p, chp);

			if (!f_recursive && chp != NULL)
				(void)fts_set(ftsp, p, FTS_SKIP);
			break;
		}
	if (errno)
		err(1, "fts_read");
}

/*
 * Display() takes a linked list of FTSENT structures and passes the list
 * along with any other necessary information to the print function.  P
 * points to the parent directory of the display list.
 */
static void
display(p, list)
	FTSENT *p, *list;
{
	struct stat *sp;
	DISPLAY d;
	FTSENT *cur;
	NAMES *np;
	u_quad_t maxsize;
	u_long btotal, maxblock, maxinode, maxlen, maxnlink;
	int bcfile, flen, glen, ulen, maxflags, maxgroup, maxuser;
	char *initmax;
	int entries, needstats;
	char *user, *group, *flags;
	char buf[STRBUF_SIZEOF(u_quad_t) + 1];
	char ngroup[STRBUF_SIZEOF(uid_t) + 1];
	char nuser[STRBUF_SIZEOF(gid_t) + 1];

	/*
	 * If list is NULL there are two possibilities: that the parent
	 * directory p has no children, or that fts_children() returned an
	 * error.  We ignore the error case since it will be replicated
	 * on the next call to fts_read() on the post-order visit to the
	 * directory p, and will be signaled in traverse().
	 */
	if (list == NULL)
		return;

	needstats = f_inode || f_longform || f_size;
	flen = 0;
	btotal = 0;
	initmax = getenv("LS_COLWIDTHS");
	/* Fields match -lios order.  New ones should be added at the end. */
	if (initmax != NULL && *initmax != '\0') {
		char *initmax2, *jinitmax;
		int ninitmax;

		/* Fill-in "::" as "0:0:0" for the sake of scanf. */
		jinitmax = initmax2 = malloc(strlen(initmax) * 2 + 2);
		if (jinitmax == NULL)
			err(1, NULL);
		if (*initmax == ':')
			strcpy(initmax2, "0:"), initmax2 += 2;
		else
			*initmax2++ = *initmax, *initmax2 = '\0';
		for (initmax++; *initmax != '\0'; initmax++) {
			if (initmax[-1] == ':' && initmax[0] == ':') {
				*initmax2++ = '0';
				*initmax2++ = initmax[0];
				initmax2[1] = '\0';
			} else {
				*initmax2++ = initmax[0];
				initmax2[1] = '\0';
			}
		}
		if (initmax2[-1] == ':') strcpy(initmax2, "0");

		ninitmax = sscanf(jinitmax,
		    " %lu : %lu : %lu : %i : %i : %i : %qu : %lu ",
		    &maxinode, &maxblock, &maxnlink, &maxuser,
		    &maxgroup, &maxflags, &maxsize, &maxlen);
		f_notabs = 1;
		switch (ninitmax) {
		 case 0: maxinode = 0;
		 case 1: maxblock = 0;
		 case 2: maxnlink = 0;
		 case 3: maxuser  = 0;
		 case 4: maxgroup = 0;
		 case 5: maxflags = 0;
		 case 6: maxsize  = 0;
		 case 7: maxlen   = 0;
#ifdef COLORLS
		 if (!f_color)
#endif
			 f_notabs = 0;
		}
		maxinode = makenines(maxinode);
		maxblock = makenines(maxblock);
		maxnlink = makenines(maxnlink);
		maxsize = makenines(maxsize);
	} else if (initmax == NULL || *initmax == '\0')
		maxblock = maxinode = maxlen = maxnlink =
		    maxuser = maxgroup = maxflags = maxsize = 0;
	bcfile = 0;
	flags = NULL;
	for (cur = list, entries = 0; cur; cur = cur->fts_link) {
		if (cur->fts_info == FTS_ERR || cur->fts_info == FTS_NS) {
			warnx("%s: %s",
			    cur->fts_name, strerror(cur->fts_errno));
			cur->fts_number = NO_PRINT;
			rval = 1;
			continue;
		}

		/*
		 * P is NULL if list is the argv list, to which different rules
		 * apply.
		 */
		if (p == NULL) {
			/* Directories will be displayed later. */
			if (cur->fts_info == FTS_D && !f_listdir) {
				cur->fts_number = NO_PRINT;
				continue;
			}
		} else {
			/* Only display dot file if -a/-A set. */
			if (cur->fts_name[0] == '.' && !f_listdot) {
				cur->fts_number = NO_PRINT;
				continue;
			}
		}
		if (cur->fts_namelen > maxlen)
			maxlen = cur->fts_namelen;
		if (f_octal || f_octal_escape) {
		        int t = len_octal(cur->fts_name, cur->fts_namelen);
			if (t > maxlen) maxlen = t;
		}
		if (needstats) {
			sp = cur->fts_statp;
			if (sp->st_blocks > maxblock)
				maxblock = sp->st_blocks;
			if (sp->st_ino > maxinode)
				maxinode = sp->st_ino;
			if (sp->st_nlink > maxnlink)
				maxnlink = sp->st_nlink;
			if (sp->st_size > maxsize)
				maxsize = sp->st_size;

			btotal += sp->st_blocks;
			if (f_longform) {
				if (f_numericonly) {
					(void)snprintf(nuser, sizeof(nuser),
					    "%u", sp->st_uid);
					(void)snprintf(ngroup, sizeof(ngroup),
					    "%u", sp->st_gid);
					user = nuser;
					group = ngroup;
				} else {
					user = user_from_uid(sp->st_uid, 0);
					group = group_from_gid(sp->st_gid, 0);
				}
				if ((ulen = strlen(user)) > maxuser)
					maxuser = ulen;
				if ((glen = strlen(group)) > maxgroup)
					maxgroup = glen;
				if (f_flags) {
					flags = fflagstostr(sp->st_flags);
					if (flags != NULL && *flags == '\0') {
						free(flags);
						flags = strdup("-");
					}
					if (flags == NULL)
						err(1, NULL);
					if ((flen = strlen(flags)) > maxflags)
						maxflags = flen;
				} else
					flen = 0;

				if ((np = malloc(sizeof(NAMES) +
				    ulen + glen + flen + 3)) == NULL)
					err(1, NULL);

				np->user = &np->data[0];
				(void)strcpy(np->user, user);
				np->group = &np->data[ulen + 1];
				(void)strcpy(np->group, group);

				if (S_ISCHR(sp->st_mode) ||
				    S_ISBLK(sp->st_mode))
					bcfile = 1;

				if (f_flags) {
					np->flags = &np->data[ulen + glen + 2];
				  	(void)strcpy(np->flags, flags);
					free(flags);
				}
				cur->fts_pointer = np;
			}
		}
		++entries;
	}

	if (!entries)
		return;

	d.list = list;
	d.entries = entries;
	d.maxlen = maxlen;
	if (needstats) {
		d.bcfile = bcfile;
		d.btotal = btotal;
		(void)snprintf(buf, sizeof(buf), "%lu", maxblock);
		d.s_block = strlen(buf);
		d.s_flags = maxflags;
		d.s_group = maxgroup;
		(void)snprintf(buf, sizeof(buf), "%lu", maxinode);
		d.s_inode = strlen(buf);
		(void)snprintf(buf, sizeof(buf), "%lu", maxnlink);
		d.s_nlink = strlen(buf);
		(void)snprintf(buf, sizeof(buf), "%qu", maxsize);
		d.s_size = strlen(buf);
		d.s_user = maxuser;
	}

	printfcn(&d);
	output = 1;

	if (f_longform)
		for (cur = list; cur; cur = cur->fts_link)
			free(cur->fts_pointer);
}

/*
 * Ordering for mastercmp:
 * If ordering the argv (fts_level = FTS_ROOTLEVEL) return non-directories
 * as larger than directories.  Within either group, use the sort function.
 * All other levels use the sort function.  Error entries remain unsorted.
 */
static int
mastercmp(a, b)
	const FTSENT **a, **b;
{
	int a_info, b_info;

	a_info = (*a)->fts_info;
	if (a_info == FTS_ERR)
		return (0);
	b_info = (*b)->fts_info;
	if (b_info == FTS_ERR)
		return (0);

	if (a_info == FTS_NS || b_info == FTS_NS)
		return (namecmp(*a, *b));

	if (a_info != b_info &&
	    (*a)->fts_level == FTS_ROOTLEVEL && !f_listdir) {
		if (a_info == FTS_D)
			return (1);
		if (b_info == FTS_D)
			return (-1);
	}
	return (sortfcn(*a, *b));
}

/*
 * Makenines() returns (10**n)-1.  This is useful for converting a width
 * into a number that wide in decimal.
 */
static u_quad_t
makenines(n)
	u_long n;
{
	u_long i;
	u_quad_t reg;

	reg = 1;
	/* Use a loop instead of pow(), since all values of n are small. */
	for (i = 0; i < n; i++)
		reg *= 10;
	reg--;

	return reg;
}

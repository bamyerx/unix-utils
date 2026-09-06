/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * tail - copy the last part of a file
 *
 * Implements the -c, -n, and -r options. -c is only implemented for seekable
 * inputs.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* default number of lines to print */
#define N_DEFAULT 10

/* threshold at which to shrink a getline-allocated buffer */
#define BUFMAX 2048

/* return statuses */
enum status {
	SUCCESS,
	ERROR
};

/* modes */
enum mode {
	NO_MODE,
	LINES_FROM_END,
	LINES_FROM_BEGINNING,
	BYTES_FROM_END,
	BYTES_FROM_BEGINNING,
	REVERSE,
	REVERSE_ALL
};

struct line {
	char *ptr;
	size_t size;
};

static void usage(void);
static int tail(FILE *fp, size_t n, int mode, int *error);
static int tail_from_beginning(FILE *fp, size_t n, bool lines, int *error);
static int lines_from_end(FILE *fp, size_t n, int mode, int *error);
static int bytes_from_end(FILE *fp, size_t n, int *error);
static void free_lines(struct line *lines, size_t n);
static size_t strtosize(const char *restrict nptr, char **restrict endptr, int base);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	int mode = NO_MODE;
	size_t n = 0;
	int c;
	char *endptr;

	/* supported options: -c, -n, -r */
	while ((c = getopt(argc, argv, "c:n:r")) != -1) {
		char *temp;
		switch (c) {
		case 'c':

			/* -c and -r are mutually exclusive */
			if (mode == REVERSE) {
				usage();
				return EXIT_FAILURE;
			}

			temp = optarg;
			/* parse optarg independently of its sign */
			if (*temp == '-' || *temp == '+')
				temp++;
			if ((n = strtosize(temp, &endptr, 10)) == 0 || *endptr != '\0') {
				fprintf(stderr, "%s: invalid number of bytes: '%s'\n", argv[0], optarg);
				return EXIT_FAILURE;
			}
			/* '+' indicates copy relative to beginning of file */
			mode = (*optarg == '+') ? BYTES_FROM_BEGINNING : BYTES_FROM_END;
			break;

		case 'n':

			temp = optarg;
			/* if -r is specified optarg must not have a sign */
			if (mode != REVERSE && (*temp == '-' || *temp == '+'))
				temp++;
			if ((n = strtosize(temp, &endptr, 10)) == 0 || *endptr != '\0') {
				fprintf(stderr, "%s: invalid number of lines: '%s'\n", argv[0], optarg);
				return EXIT_FAILURE;
			}

			/* if -r is specified, we always copy relative to the end of file */
			if (mode != REVERSE)
				mode = (*optarg == '+') ? LINES_FROM_BEGINNING : LINES_FROM_END;
			break;

		case 'r':

			/* -r must be the first option specified */
			if (mode != NO_MODE) {
				usage();
				return EXIT_FAILURE;
			}

			mode = REVERSE;
			break;

		default:
			usage();
			return EXIT_FAILURE;
		}
	}

	/* default mode */
	if (mode == NO_MODE)
		mode = LINES_FROM_END;

	/* default number of lines */
	if (n == 0) {
		/* default number of lines */
		n = N_DEFAULT;
		/* -r with no -n specified */
		mode = mode == REVERSE ? REVERSE_ALL : mode;
	}

	if (argc - optind > 2) {
		usage();
		return EXIT_FAILURE;
	}
	name = argv[optind];
	if (name == NULL || strcmp(name, "-") == 0)
		fp = stdin;
	else
		fp = fopen(name, "rb");

	/* failed to open stream */
	if (fp == NULL) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
		return EXIT_FAILURE;
	}

	/* error during tail */
	if (tail(fp, n, mode, &error) == ERROR) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(error));
		exit_status = EXIT_FAILURE;
	}

	/* close non-stdin stream */
	if (fp != stdin && fclose(fp) != 0) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
		exit_status = EXIT_FAILURE;
	}

	/* flush stdout to check for errors */
	if (fflush(stdout) == EOF) {
		fprintf(stderr, "%s: stdout: %s\n", argv[0], strerror(errno));
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}

/* usage: print error message on correct usage */
static void
usage(void)
{
	fprintf(stderr, "usage: tail [-c number|-n number] [file]\n");
	fprintf(stderr, "       tail -r [-n number] [file]\n");
}

/* tail: copy the last part of fp to stdout */
static int
tail(FILE *fp, size_t n, int mode, int *error)
{
	switch (mode) {
	case LINES_FROM_BEGINNING: /* FALLTHROUGH */
	case BYTES_FROM_BEGINNING:
		return tail_from_beginning(fp, n, mode == LINES_FROM_BEGINNING, error);
	case LINES_FROM_END:       /* FALLTHROUGH */
	case REVERSE:              /* FALLTHROUGH */
	case REVERSE_ALL:
		return lines_from_end(fp, n, mode, error);
	case BYTES_FROM_END:
		return bytes_from_end(fp, n, error);
	}
	/* not reached */
	return -1;
}

/* 
 * tail_from_beginning: 
 * copy the the contents of fp starting at n bytes or lines (inclusive)
 */
static int
tail_from_beginning(FILE *fp, size_t n, bool lines, int *error)
{
	int c;

	while ((c = getc(fp)) != EOF) {
		if (n > 1) {
			if (!lines || c == '\n')
				n--;
		} else
			if (putchar(c) == EOF) {
				/* write error */
				*error = errno;
				return ERROR;
			}
	}

	if (ferror(fp)) {
		/* read error */
		*error = errno;
		return ERROR;
	}
	return SUCCESS;
}

/* lines_from_end: copy the last n lines of fp to stdout */
static int
lines_from_end(FILE *fp, size_t n, int mode, int *error)
{
	bool reverse = mode == REVERSE || mode == REVERSE_ALL;

	/* a queue of n lines */
	struct line *lines = calloc(n, sizeof *lines);
	if (lines == NULL) {
		*error = errno;
		return ERROR;
	}
	size_t first = 0;
	size_t last  = 0;
	size_t nlines = 0;

	/* read in each line while maintaining the last n lines in the queue */
	for (;;) {
		ssize_t len = getline(&lines[last].ptr, &lines[last].size, fp);
		if (len == -1)
			break;
		
		/* shrink the buffer if unnecessarily large */
		if ((size_t) len < BUFMAX && BUFMAX < lines[last].size) {
			char *temp = realloc(lines[last].ptr, (size_t) len + 1);
			if (temp != NULL) {
				lines[last].ptr = temp;
				lines[last].size = len + 1;
			}
			/* if realloc fails, continue with the old buffer */
		}

		/* dynamically grow the queue if printing every line in reverse */
		if (mode == REVERSE_ALL && nlines == n - 1) {

			/* maximum size before overflowing size_t */
			size_t max = SIZE_MAX / sizeof *lines;
			if (n == max) {
				*error = ENOMEM;
				free_lines(lines, n);
				return ERROR;
			}

			/* double the size of the queue */
			size_t m = (n < max / 2) ? n * 2 : max;
			struct line *temp = realloc(lines, m * sizeof *lines);
			if (temp == NULL) {
				*error = errno;
				free_lines(lines, n);
				return ERROR;
			}
			lines = temp;

			/* initialize the pointers to NULL for getline */
			for (; n < m; n++)
				lines[n].ptr = NULL;
				/* n = m at the end of the loop */
		}

		if (mode != REVERSE_ALL && nlines == n) {
			first = (first + 1) % n;
		} else
			nlines++;

		last = mode == REVERSE_ALL ? last + 1 : (last + 1) % n;
	}

	if (ferror(fp)) {
		/* read error */
		*error = errno;
		free_lines(lines, n);
		return ERROR;
	}

	/* print the buffered lines */
	size_t count = nlines;
	size_t i = reverse ? (last + n - 1) % n : first;
	while (count-- > 0) {
		if (fputs(lines[i].ptr, stdout) == EOF) {
			/* write error */
			*error = errno;
			free_lines(lines, n);
			return ERROR;
		}
		i = reverse ? (i + n - 1) % n : (i + 1) % n;
	}

	free_lines(lines, n);
	return SUCCESS;
}

/* free_lines: deallocate the queue as well as each buffer from getline */
static void
free_lines(struct line *lines, size_t n)
{
	for (size_t i = 0; i < n; i++)
		free(lines[i].ptr);
	free(lines);
}

/* bytes_from_end: copy the last n bytes of fp to stdout */
static int
bytes_from_end(FILE *fp, size_t n, int *error)
{
	/* determine the size of the file */
	if (fseeko(fp, 0, SEEK_END) == -1) {
		/* seek error */
		*error = errno;
		return ERROR;
	}
	off_t end = ftello(fp);
	if (end == -1) {
		/* tell error */
		*error = errno;
		return ERROR;
	}

	/* the offset must be at most the size of the file */
	if ((uintmax_t) n > (uintmax_t) end)
		n = (size_t) end;

	/* seek to end - n */
	if (fseeko(fp, -(off_t) n, SEEK_END) == -1) {
		/* seek error */
		*error = errno;
		return ERROR;
	}

	int c;
	while ((c = getc(fp)) != EOF)
		if (putchar(c) == EOF) {
			/* write error */
			*error = errno;
			return ERROR;
		}

	if (ferror(fp)) {
		/* read error */
		*error = errno;
		return ERROR;
	}

	return SUCCESS;
}

/*
 * strtosize: convert a string to a size_t value
 *
 * Note: I will be factoring this out as a library function in a later iteration.
 */
static size_t
strtosize(const char *restrict nptr, char **restrict endptr, int base)
{
	const char *p = nptr;
	uintmax_t val;

	/* reject if first non-white-space character is '-' */
	while (isspace((unsigned char) *p))
		p++;
	if (*p == '-') {
		if (endptr != NULL)
			*endptr = (char *) nptr;
		return 0;
	}
	
	/* otherwise, convert using strtoumax */
	errno = 0;
	val = strtoumax(nptr, endptr, base);

	/* ensure val > SIZE_MAX is reported as a range error */
	if (errno == ERANGE || val > SIZE_MAX) {
		errno = ERANGE;
		return SIZE_MAX;
	}

	return (size_t) val;
}

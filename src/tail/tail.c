/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * tail - copy the last part of a file
 *
 * A basic implementation of tail with only default behavior.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N_DEFAULT 10

/* LINE_MAX * 10 */
#define BUFSIZE 20480

/* return statuses */
enum status {
	SUCCESS,
	ERROR
};

static int tail(FILE *fp, size_t n, int *error);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	size_t n = N_DEFAULT;

	if (argc > 2) {
		fprintf(stderr, "usage: tail [file]\n");
		return EXIT_FAILURE;
	}
	name = argv[1];
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
	if (tail(fp, n, &error) == ERROR) {
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

/* tail: write the last n lines of fp to stdout */
static int
tail(FILE *fp, size_t n, int *error)
{
	int c;

	char buf[BUFSIZE];
	size_t head = 0;
	
	struct line {
		size_t start;    /* the index where the line begins */
		size_t length;   /* the length of the line */
	};

	struct line *lines = calloc(n, sizeof(struct line));
	if (lines == NULL) {
		*error = errno;
		return ERROR;
	}
	size_t current = 0;    /* index of the current line */
	size_t oldest  = 0;    /* index of the oldest line */
	size_t nlines  = 0;    /* the number of complete lines */

	while ((c = getc(fp)) != EOF) {

		/* the next byte would overwrite the oldest line */
		if (head == lines[oldest].start && nlines > 1) {
			oldest = (oldest + 1) % n;
			nlines--;
		}

		if (nlines == n && current == oldest) {
			oldest = (oldest + 1) % n;
			lines[current].length = 0;
		}
		if (lines[current].length == 0)
			lines[current].start = head;

		buf[head] = c;
		head = (head + 1) % BUFSIZE;

		/* ensure that length is at most BUFSIZE */
		if (lines[current].length < BUFSIZE)
			lines[current].length++;
		else
			lines[current].start = (lines[current].start + 1) % BUFSIZE;

		if (c == '\n') {
			/* ensure that nlines is at most n */
			if (nlines < n)
				nlines++;
			current = (current + 1) % n;
		}
	}

	/* include any new pending characters as a new line */
	if (lines[current].length > 0 && current != oldest) {
		if (nlines == n)
			oldest = (oldest + 1) % n;
		else
			nlines++;
	}

	if (ferror(fp)) {
		/* read error */
		*error = errno;
		free(lines);
		return ERROR;
	}

	/* print the buffered lines */
	size_t count = nlines;
	for (size_t i = oldest; count-- > 0; i = (i + 1) % n) {
		size_t length = lines[i].length;
		for (size_t j = lines[i].start; length-- > 0; j = (j + 1) % BUFSIZE)
			if (putchar(buf[j]) == EOF) {
				/* write error */
				*error = errno;
				free(lines);
				return ERROR;
			}
	}

	free(lines);
	return SUCCESS;
}

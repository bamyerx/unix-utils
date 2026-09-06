/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * tail - copy the last part of a file
 *
 * A basic implementation of tail with only default behavior.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* default number of lines to print */
#define N_DEFAULT 10

/* threshold at which to shrink an allocated buffer */
#define BUFMAX 2048

/* return statuses */
enum status {
	SUCCESS,
	ERROR
};

struct line {
	char *ptr;
	size_t size;
};

static int tail(FILE *fp, size_t n, int *error);
static void free_lines(struct line *lines, size_t n);

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

		if (nlines == n)
			first = (first + 1) % n;
		else
			nlines++;
		last = (last + 1) % n;
	}

	if (ferror(fp)) {
		/* read error */
		*error = errno;
		free_lines(lines, n);
		return ERROR;
	}

	/* print the buffered lines */
	size_t count = nlines;
	for (size_t i = first; count-- > 0; i = (i + 1) % n) {
		if (fputs(lines[i].ptr, stdout) == EOF) {
			/* write error */
			*error = errno;
			free_lines(lines, n);
			return ERROR;
		}
	}

	free_lines(lines, n);
	return SUCCESS;
}

/* free_lines: deallocate the queue as well as each string from getline */
static void
free_lines(struct line *lines, size_t n)
{
	for (size_t i = 0; i < n; i++)
		free(lines[i].ptr);
	free(lines);
}

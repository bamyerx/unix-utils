/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * head - copy the first part of files
 *
 * Implements the POSIX.1-2024 specification for head.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N_DEFAULT 10

/* return statuses */
enum status {
	SUCCESS,
	ERROR
};

/* print modes */
enum mode {
	PRINT_LINES,
	PRINT_BYTES
};

static int head(FILE *fp, size_t n, int mode, int *error);
static size_t strtosize(const char *restrict nptr, char **restrict endptr, int base);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	int mode = PRINT_LINES;
	size_t n = N_DEFAULT;
	int c;
	char *endptr;

	/* supported options: -c, -n */
	while ((c = getopt(argc, argv, "c:n:")) != -1) {
		switch (c) {
		case 'c':
			/* ensure optarg is a positive decimal value */
			if ((n = strtosize(optarg, &endptr, 10)) == 0 || *endptr != '\0') {
				fprintf(stderr, "%s: invalid number of bytes: '%s'\n", argv[0], optarg);
				return EXIT_FAILURE;
			}
			mode = PRINT_BYTES;
			break;
		case 'n':
			/* ensure optarg is a positive decimal value */
			if ((n = strtosize(optarg, &endptr, 10)) == 0 || *endptr != '\0') {
				fprintf(stderr, "%s: invalid number of lines '%s'\n", argv[0], optarg);
				return EXIT_FAILURE;
			}
			mode = PRINT_LINES;
			break;
		default:
			fprintf(stderr, "usage: head [-c number | -n number] [file...]\n");
			return EXIT_FAILURE;
		}
	}

	/* execute loop once for stdin if no args */
	for (int i = optind; i < argc || i == optind; i++) {
		name = argv[i];

		/* no args or arg = "-" specifies stdin */
		if (name == NULL || strcmp(name, "-") == 0) {
			name = "stdin";
			fp = stdin;
		} else
			fp = fopen(name, "rb");

		/* failed to open stream */
		if (fp == NULL) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
			continue;
		}

		/* print file name header for more than one file */
		if (argc - optind > 1)
			printf("%s==> %s <==\n", (i == optind) ? "" : "\n", name);

		/* I/O error during head */
		if (head(fp, n, mode, &error) == ERROR) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(error));
			exit_status = EXIT_FAILURE;
		}

		/* close non-stdin stream */
		if (fp != stdin && fclose(fp) != 0) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
		}
	}

	/* flush stdout to check for errors */
	if (fflush(stdout) == EOF) {
		fprintf(stderr, "%s: stdout: %s\n", argv[0], strerror(errno));
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}

/* head: write the first n lines or bytes of fp to stdout */
static int
head(FILE *fp, size_t n, int mode, int *error)
{
	int c;

	while (n > 0 && (c = getc(fp)) != EOF) {
		if (putchar(c) == EOF) {
			/* write error */
			*error = errno;
			return ERROR;
		}
		if (mode == PRINT_BYTES)
			n--;
		else if (mode == PRINT_LINES && c == '\n')
			n--;
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

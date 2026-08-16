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

enum status {
	SUCCESS,
	ERROR
};

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

	while ((c = getopt(argc, argv, "c:n:")) != -1) {
		switch (c) {
		case 'c':
			if ((n = strtosize(optarg, &endptr, 10)) == 0 || *endptr != '\0') {
				fprintf(stderr, "%s: invalid number of bytes: '%s'\n", argv[0], optarg);
				return EXIT_FAILURE;
			}
			mode = PRINT_BYTES;
			break;
		case 'n':
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

	for (int i = optind; i < argc || i == optind; i++) {
		name = argv[i];

		/* Treat "-" as specifying standard input */
		if (name == NULL || strcmp(name, "-") == 0) {
			name = "stdin";
			fp = stdin;
		} else
			fp = fopen(name, "rb");

		if (fp == NULL) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
			continue;
		}

		if (argc - optind > 1)
			printf("%s==> %s <==\n", (i == optind) ? "" : "\n", name);
		if (head(fp, n, mode, &error) == ERROR) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(error));
			exit_status = EXIT_FAILURE;
		}

		if (fp != stdin && fclose(fp) != 0) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
		}
	}

	return exit_status;
}

static int
head(FILE *fp, size_t n, int mode, int *error)
{
	int c;

	while (n > 0 && (c = getc(fp)) != EOF) {
		if (putchar(c) == EOF) {
			*error = errno;
			return ERROR;
		}
		if (mode == PRINT_BYTES)
			n--;
		else if (mode == PRINT_LINES && c == '\n')
			n--;
	}
	if (ferror(fp)) {
		*error = errno;
		return ERROR;
	}
	return SUCCESS;
}

/*
 * strtosize: convert a string to a nonnegative size_t value
 *
 * Note: I will be factoring this out as a library function in a later iteration.
 */
static size_t
strtosize(const char *restrict nptr, char **restrict endptr, int base)
{
	const char *p = nptr;
	uintmax_t val;

	/* 
	 * Reject a string whose first non-white-space character is a '-'.
	 */
	while (isspace((unsigned char) *p))
		p++;
	if (*p == '-') {
		if (endptr != NULL)
			*endptr = (char *) nptr;
		return 0;
	}
	
	/*
	 * Otherwise, use strtoumax to perform the conversion.
	 */
	errno = 0;
	val = strtoumax(nptr, endptr, base);

	/*
	 * Ensure values exceeding SIZE_MAX are detected as a range error in
	 * addition to any range errors propagated from strtoumax.
	 */
	if (errno == ERANGE || val > SIZE_MAX) {
		errno = ERANGE;
		return SIZE_MAX;
	}

	return (size_t) val;
}

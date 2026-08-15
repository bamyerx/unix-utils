/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * cat - concatenate and print files
 *
 * Implements the POSIX.1-2024 specification for cat.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum status {
	SUCCESS,
	ERROR
};

static int cat(FILE *fp, int *error);
static void usage(void);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	int c;
	int uflag = 0;

	while ((c = getopt(argc, argv, "u")) != -1)
		switch (c) {
		case 'u':
			uflag = 1;
			break;
		default:
			usage();
		}

	if (uflag && setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		fprintf(stderr, "%s: error setting stdout unbuffered\n", argv[0]);
		return EXIT_FAILURE;
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

		if (cat(fp, &error) == ERROR) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(error));
			exit_status = EXIT_FAILURE;
		}

		if (fp != stdin && fclose(fp) != 0) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
		}
	}

	if (fflush(stdout) == EOF) {
		fprintf(stderr, "%s: stdout: %s\n", argv[0], strerror(errno));
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}

static int
cat(FILE *fp, int *error)
{
	int c;

	while ((c = getc(fp)) != EOF)
		if (putc(c, stdout) == EOF) {
			*error = errno;
			return ERROR;
		}
	if (ferror(fp)) {
		*error = errno;
		return ERROR;
	}
	return SUCCESS;
}

static void
usage(void)
{
	fprintf(stderr, "usage: cat [-u] [file...]\n");
	exit(EXIT_FAILURE);
}

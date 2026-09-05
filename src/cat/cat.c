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

/* return statuses */
enum status {
	SUCCESS,
	ERROR
};

static int cat(FILE *fp, int *error);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	int c;
	int uflag = 0;

	/* supported options: -u */
	while ((c = getopt(argc, argv, "u")) != -1)
		switch (c) {
		case 'u':
			uflag = 1;
			break;
		default:
			fprintf(stderr, "usage: cat [-u] [file...]\n");
			return EXIT_FAILURE;
		}

	/* set stdout to unbuffered if -u is specified */
	if (uflag && setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		fprintf(stderr, "%s: error setting stdout to unbuffered\n", argv[0]);
		return EXIT_FAILURE;
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

		/* I/O error during cat */
		if (cat(fp, &error) == ERROR) {
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

/* cat: write contents of fp to stdout */
static int
cat(FILE *fp, int *error)
{
	int c;

	while ((c = getc(fp)) != EOF)
		if (putc(c, stdout) == EOF) {
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

/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * head - copy the first part of files
 *
 * A basic implementation with only default behavior and no options.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum status {
	SUCCESS,
	ERROR
};

static int head(FILE *fp, int *error);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;

	for (int i = 1; i < argc || i == 1; i++) {
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

		if (argc > 2)
			printf("%s==> %s <==\n", (i == 1) ? "" : "\n", name);
		if (head(fp, &error) == ERROR) {
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
head(FILE *fp, int *error)
{
	int c;
	int n = 10;

	while (n > 0 && (c = getc(fp)) != EOF) {
		if (putchar(c) == EOF) {
			*error = errno;
			return ERROR;
		}
		if (c == '\n')
			n--;
	}
	if (ferror(fp)) {
		*error = errno;
		return ERROR;
	}
	return SUCCESS;
}

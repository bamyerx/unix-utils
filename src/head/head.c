/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * head - copy the first part of files
 *
 * A basic implementation with only default behavior and no options.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum status {
	SUCCESS,
	INPUT_ERROR,
	OUTPUT_ERROR
};

static enum status head(FILE *fp);

int
main(int argc, char *argv[])
{
	const char *name;
	FILE *fp;
	int status = EXIT_SUCCESS;

	for (int i = 1; i < argc || argc == 1; i++) {
		if (argc == 1 || strcmp(argv[i], "-") == 0) {
			name = "stdin";
			fp = stdin;
		} else {
			name = argv[i];
			fp = fopen(name, "rb");
		}

		if (fp == NULL) {
			warn("%s", name);
			status = EXIT_FAILURE;
			continue;
		}

		if (argc > 2)
			printf("%s==> %s <==\n", (i == 1) ? "" : "\n", name);
		switch (head(fp)) {
		case SUCCESS:
			break;
		case INPUT_ERROR:
			warnx("%s: input error", name);
			break;
		case OUTPUT_ERROR:
			warnx("%s: output error", name);
			break;
		}

		if (fp != stdin && fclose(fp) != 0) {
			warn("%s", name);
			status = EXIT_FAILURE;
		}
	}

	return status;
}

static enum status
head(FILE *fp)
{
	int c;
	int n = 10;

	while (n > 0 && (c = getc(fp)) != EOF) {
		if (putchar(c) == EOF)
			return OUTPUT_ERROR;
		if (c == '\n')
			n--;
	}
	if (ferror(fp))
		return INPUT_ERROR;
	return SUCCESS;
}

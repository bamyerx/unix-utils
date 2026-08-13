/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * wc - word, line, and byte or character count
 *
 * A basic implementation which does not support options or multibyte
 * characters yet.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct counts {
	uintmax_t bytes;
	uintmax_t words;
	uintmax_t lines;
};

enum status {
	SUCCESS,
	INPUT_ERROR,
	OVERFLOW
};

static void reset_counts(struct counts *counts);
static int count(FILE *fp, struct counts *counts, int *error);
static void print_counts(const struct counts *counts, const char *name);

/*
 * wc: Print newline, word, and byte counts for each input file and a total
 *     count for all files if more than one file is specified.
 */
int
main(int argc, char *argv[])
{	
	struct counts counts;
	struct counts total = {0};

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

		reset_counts(&counts);
		switch (count(fp, &counts, &error)) {
		case SUCCESS:
			print_counts(&counts, name);
			total.bytes += counts.bytes;
			total.words += counts.words;
			total.lines += counts.lines;
			break;
		case INPUT_ERROR:
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(error));
			exit_status = EXIT_FAILURE;
			break;
		case OVERFLOW:
			fprintf(stderr, "%s: %s: count exceeds limit\n", argv[0], name);
			exit_status = EXIT_FAILURE;
			break;
		}

		if (fp != stdin && fclose(fp) != 0) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
		}
	}
	if (argc > 2)
		print_counts(&total, "total");

	return exit_status;
}

/*
 * count: Count bytes, words, and lines for fp and store in counts.
 */
static int
count(FILE *fp, struct counts *counts, int *error)
{
	int c;

	/* True when previous byte was white space or at the start of input */
	bool in_space = true;

	while ((c = getc(fp)) != EOF) {

		/*
		 * Check byte count before incrementing. Word and line counts cannot
		 * exceed the byte count, so checking them is unnecessary.
		 */
		if (counts->bytes == UINTMAX_MAX)
			return OVERFLOW;

		counts->bytes++;
		if (c == '\n')
			counts->lines++;
		if (isspace((unsigned char) c))
			in_space = true;
		else {
			if (in_space)
				counts->words++;
			in_space = false;
		}
	}

	if (ferror(fp)) {
		*error = errno;
		return INPUT_ERROR;
	}

	return SUCCESS;
}

static void
reset_counts(struct counts *counts)
{
	counts->bytes = 0;
	counts->words = 0;
	counts->lines = 0;
}

static void
print_counts(const struct counts *counts, const char *name)
{
	printf("%ju\t%ju\t%ju\t%s\n", 
			counts->lines, 
			counts->words, 
			counts->bytes,
			name ? name: "");
}



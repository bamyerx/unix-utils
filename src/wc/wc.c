/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * wc - word, line, and byte or character count
 *
 * An implementation which supports the -c, -l, and -w options.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct counts {
	uintmax_t lines;
	uintmax_t words;
	uintmax_t bytes;
};

struct mode {
	bool lines;
	bool words;
	bool bytes;
};

enum status {
	SUCCESS,
	INPUT_ERROR,
	OVERFLOW
};

static void reset_counts(struct counts *counts);
static int count(FILE *fp, struct counts *counts, int *error);
static void print_counts(const struct counts *counts, const struct mode *mode, const char *name);

/*
 * wc: Print newline, word, and byte counts for each input file and a total
 *     count for all files if more than one file is specified.
 */
int
main(int argc, char *argv[])
{	
	struct counts counts;
	struct mode mode = {0};
	struct counts total = {0};

	const char *name;
	FILE *fp;
	int error;
	int exit_status = EXIT_SUCCESS;
	int c;
	
	while ((c = getopt(argc, argv, "clw")) != -1) {
		switch (c) {
		case 'c':
			mode.bytes = true;
			break;
		case 'l':
			mode.lines = true;
			break;
		case 'w':
			mode.words = true;
			break;
		default:
			fprintf(stderr, "usage: wc [-clw] [file...]\n");
			return EXIT_FAILURE;
		}
	}

	/* Default is to print lines, words, and bytes */
	if (!mode.lines && !mode.words && !mode.bytes)
		mode.lines = mode.words = mode.bytes = true;

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

		reset_counts(&counts);
		switch (count(fp, &counts, &error)) {
		case SUCCESS:
			print_counts(&counts, &mode, name);
			total.lines += counts.lines;
			total.words += counts.words;
			total.bytes += counts.bytes;
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
	if (argc - optind > 1)
		print_counts(&total, &mode, "total");

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
	counts->lines = 0;
	counts->words = 0;
	counts->bytes = 0;
}

static void
print_counts(const struct counts *counts, const struct mode *mode, const char *name)
{
	if (mode->lines)
		printf("%ju\t", counts->lines);
	if (mode->words)
		printf("%ju\t", counts->words);
	if (mode->bytes)
		printf("%ju", counts->bytes);
	if (name)
		printf("\t%s", name);
	printf("\n");
}



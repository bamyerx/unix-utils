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

/* line, word, and byte counts */
struct counts {
	uintmax_t lines;
	uintmax_t words;
	uintmax_t bytes;
};

/* count modes */
struct mode {
	bool lines;
	bool words;
	bool bytes;
};

/* return statuses */
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
	
	/* supported options: -c, -l, -w */
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

	/* default is to print lines, words, and bytes */
	if (!mode.lines && !mode.words && !mode.bytes)
		mode.lines = mode.words = mode.bytes = true;

	/* execute loop once for stdin if no args */
	for (int i = optind; i < argc || i == optind; i++) {
		name = argv[i];

		/* no args or arg = "=" specifies stdin */
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

		/* close non-stdin stream */
		if (fp != stdin && fclose(fp) != 0) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], name, strerror(errno));
			exit_status = EXIT_FAILURE;
		}
	}

	/* print totals for more than one file */
	if (argc - optind > 1)
		print_counts(&total, &mode, "total");

	/* flush stdout to check for errors */
	if (fflush(stdout) == EOF) {
		fprintf(stderr, "%s: stdout: %s\n", argv[0], strerror(errno));
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}

/*
 * count: Count bytes, words, and lines for fp and store in counts.
 */
static int
count(FILE *fp, struct counts *counts, int *error)
{
	int c;

	/* true when previous byte was white space or at the start of input */
	bool in_space = true;

	while ((c = getc(fp)) != EOF) {

		/*
		 * check byte count for overflow
		 *
		 * note: line count <= word count <= byte count is an invariant so it is
		 * sufficient to check byte count to detect overflow
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
		/* read error */
		*error = errno;
		return INPUT_ERROR;
	}

	return SUCCESS;
}

/* reset_counts: set line, word, and byte count to zero */
static void
reset_counts(struct counts *counts)
{
	counts->lines = 0;
	counts->words = 0;
	counts->bytes = 0;
}

/* print_counts: print line, word, and byte counts, optionally with a name */
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



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

typedef enum CountStatus {
	COUNT_SUCCESS,
	COUNT_ERROR,
	COUNT_OVERFLOW
} CountStatus;

void reset_counts(struct counts *counts);
int count(FILE *fp, struct counts *counts);
void print_counts(const struct counts *counts, const char *name);

/*
 * wc: Print newline, word, and byte counts for each input file and a total
 *     count for all files if more than one file is specified.
 */
int
main(int argc, char *argv[])
{	
	struct counts counts;
	struct counts total = {0};

	/* 
	 * n_operands is used to normalize the input prior to the main loop; this
	 * way, if wc is called with no arguments, we can synthesize a "-" operand
	 * to indicate that stdin and populate operands with argv otherwise.
	 */
	size_t n_operands = (argc == 1) ? 1 : (size_t) argc - 1;
	char **operands = malloc(n_operands * sizeof *operands);
	if (operands == NULL) {
		fprintf(stderr, "wc: allocation failure\n");
		return EXIT_FAILURE;
	}
	if (n_operands == 1)
		operands[0] = "-";
	else
		for (size_t i = 0; i < n_operands; i++)
			operands[i] = argv[i + 1];

	bool overflow = false;
	int exit_status = EXIT_SUCCESS;

	for (size_t i = 0; i < n_operands; i++) {

		const char *name = operands[i];
		
		/* Treat "-" as specifying standard input */
		FILE *fp = (strcmp(name, "-") != 0) ? fopen(name, "rb") : stdin;
		if (fp == NULL) {
			fprintf(stderr, "wc: can't open %s\n", name);
			exit_status = EXIT_FAILURE;
			continue;
		}

		reset_counts(&counts);
		CountStatus count_status = count(fp, &counts);
		if (fp != stdin)
			fclose(fp);

		switch (count_status) {
		case COUNT_ERROR:
			fprintf(stderr, "wc: %s: read error\n", name);
			exit_status = EXIT_FAILURE;
			break;
		case COUNT_OVERFLOW:
			fprintf(stderr, "wc: %s: count exceeds limit\n", name);
			overflow = true;
			exit_status = EXIT_FAILURE;
			break;
		case COUNT_SUCCESS:
			print_counts(&counts, name);
			total.bytes += counts.bytes;
			total.words += counts.words;
			total.lines += counts.lines;
			break;
		}
	}
	if (n_operands > 1 && !overflow)
		print_counts(&total, "total");

	free(operands);
	return exit_status;
}

/*
 * count: Count bytes, words, and lines for fp and store in counts.
 */
int
count(FILE *fp, struct counts *counts)
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
			return COUNT_OVERFLOW;

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

	if (ferror(fp))
		return COUNT_ERROR;

	return COUNT_SUCCESS;
}

/*
 * reset_counts: Reset all counters to zero.
 */
void
reset_counts(struct counts *counts)
{
	counts->bytes = 0;
	counts->words = 0;
	counts->lines = 0;
}

/*
 * print_counts: Format and print counts.
 */
void
print_counts(const struct counts *counts, const char *name)
{
	printf("%ju\t%ju\t%ju\t%s\n", 
			counts->lines, 
			counts->words, 
			counts->bytes,
			name ? name: "");
}



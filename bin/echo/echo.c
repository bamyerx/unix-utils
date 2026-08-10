/*
 * Copyright (c) 2026 Bennett A. Myers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * echo - write arguments to standard output
 *
 * Implements the POSIX.1-2024 specification for echo.
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	char **arg = argv;

	while (--argc > 0)
		printf("%s%s", *++arg, argc > 1 ? " " : "");
	printf("\n");

	return 0;
}

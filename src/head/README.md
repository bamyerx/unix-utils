# head

copy the first part of files

## Synopsis

`head [-c number|-n number] [file...]`

## Description

Print the first 10 lines of each argument to standard output. If no file is
is specified, or if a file is specified as '-', read standard input. If a file
has fewer than 10 lines, print the entire file.

## Options

-c *number*\
    print the first *number* bytes of each file

-n *number*\
    print the first *number* lines of each file

Options may be in the form `-n 100` as well as `-n100`. Additionally, `--` is
considered a delimiter indicating the end of options and any subsequent
arguments are treated as operands.

## Specification

Primary specification:
POSIX.1-2024

## Implementation

This implementation accepts arguments of the form `-n 100` and `-n100` but not
`-100` so as to conform to XBD 12.2 Utility Syntax Guidelines.

## Status

- [x] Default behavior with no options
- [ ] Basic POSIX implementation.

## References

- IEEE Std 1003.1-2024, *head*.
- Free Software Foundation. “head(1).” *GNU Coreutils Manual*, version 9.11,
  2026.

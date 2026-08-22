# wc

print word, line, and byte or character count

## Synopsis

`wc [-c|-m] [-lw] [file...]`

## Description

Print newline, word, and byte counts for each input file, and a total count for
all files if more than one file is specified. If no file is specified, or if a
file is specified as '-', read standard input. A word is defined as a nonempty
sequence of characters delimited by white space.

Counts are always printed in the order: newlines, words, bytes/characters.

## Options (TODO)

By default, all counts are printed. If any options are specified, only those
counts are printed.

-c
    print byte counts

-m
    print character counts

-l
    print newline counts

-w
    print word counts

## Specification

Primary specification:
POSIX.1-2024

## Implementation Notes

`wc` counts lines, words, and bytes regardless of whether or not those options
are specified. Specifying an option only changes what is printed.

## Status

- [x] Counts bytes, words, and newlines for zero or more files.
- [x] Supports -c, -l, and -w.
- [ ] Supports -m (requires multibyte character support).
- [ ] Full POSIX implementation.
- [ ] Column width calculation for better formatting.

## References

- IEEE Std 1003.1-2024, *wc*.
- Free Software Foundation. “wc(1).” *GNU Coreutils Manual*, version 9.11,
  2026.

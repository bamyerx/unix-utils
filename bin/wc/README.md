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

## Implementation

The current implementation operates on input as a byte stream using `getc()`.
For each byte, it maintains counters for newlines, words, and bytes. Word
boundaries are determined using the C locale's `isspace()` classification.

Input is processed through a common operand-processing path. When no operands
are specified, `-` is synthesized as the operand and refers to standard input.
File operands are opened in binary mode.

Counts are represented using `uintmax_t`. The byte counter is checked for
overflow before each increment; because newline and word counts cannot exceed
the number of input bytes, this also bounds those counters.

The counting function reports success, input errors, and counter overflow
separately through `CountStatus`, allowing the caller to handle these
conditions independently.

The implementation currently does not perform multibyte character processing
and therefore cannot implement `-m`. Output formatting is currently
tab-separated rather than aligned to the field widths.

## Status

The interface described above is the target interface. The current
implementation supports only the default counting behavior.

- [x] Counts bytes, words, and newlines for zero or more files.
- [ ] Supports -c, -l, and -w.
- [ ] Supports -m (requires multibyte character support).
- [ ] Full POSIX implementation.

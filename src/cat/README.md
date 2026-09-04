# cat

concatenate and print files

## Synopsis

`cat [-u] [file...]`

## Description

Concatenate the files specified to standard output.  If no file is is specified,
or if a file is specified as '-', read standard input.

## Options

`-u`\
    write bytes without buffering

## Specification

Primary specification:
POSIX.1-2024

## Implementation Notes

Uses getc() and putc() to implement cat.

## Status

- [x] Basic POSIX implementation.

## References

- IEEE Std 1003.1-2024, *cat*.

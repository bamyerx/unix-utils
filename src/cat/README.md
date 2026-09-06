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

- The Open Group. 
  *[cat](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/cat.html)*.
  *The Open Group Base Specifications Issue 8*, IEEE Std 1003.1-2024, 2024.

# head

copy the first part of files

## Synopsis

`head [-c number|-n number] [file...]`

## Description

Print the first n lines of each argument to standard output. If no file is
is specified, or if a file is specified as '-', read standard input. If a file
has fewer than n lines, print the entire file.

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

## Implementation Notes

For the case when the *number* supplied as an argument to `-c` or `-n` exceeds
`SIZE_MAX`, I treat *number* as equaling `SIZE_MAX`, which in practice
corresponds to the usual `head` behavior of printing the entire file when the
file has fewer bytes or lines than requested.

## Status

- [x] Default behavior with no options
- [x] Full POSIX implementation.

## References

- The Open Group. 
  *[head](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/head.html)*.
  *The Open Group Base Specifications Issue 8*, IEEE Std 

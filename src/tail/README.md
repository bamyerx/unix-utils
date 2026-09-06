# tail

copy the last part of a file

## Synopsis

`tail [-f] [-c number |-n number] [file]`

`tail -r [-n number] [file]`

## Description

Print the last 10 lines of the specified file to standard output. If no file is
is specified, or if a file is specified as '-', read standard input. If a file
has fewer than 10 lines, print the entire file.

## Options

-c *number*\
    print the *number* bytes relative to the end of the file if *number* is
    prepended with a '-' or no sign and relative to the beginning of the file if
    prepended with a '+' sign.

-f\
    continue to print from the file as data is added to the file. This option is
    ignored if the input is not a regular file.

-n *number*\
    print the *number* lines relative to the end of the file if *number* is
    prepended with a '-' or no sign and relative to the beginning of the file if
    prepended with a '+' sign.

-r\
    print the lines in reverse order. If `-n` is specified, print the specified
    number of lines; otherwise, print every line of the file.

## Specification

Primary specification:
POSIX.1-2024

## Implementation Notes

The `-c`, `-n`, and `-r` options as specified by POSIX define six distinct behaviors
for `tail`: `-c` can be specified relative to the beginning or the end, `-n` can be
specified relative to the beginning or the end, and `-r` can be specified with `-n`
or without it. This implementation uses groups these six behaviors into three
cases:

- Lines or bytes printed relative to the beginning of the file (`-c` or `-n` with a
  +N argument)
- Lines printed relative to the end of the file (`-n` -N with or without `-r`)
- Bytes printed relative to the end of the file (`-c` -N)

The first case has logic nearly identical to that of [`head`](src/head/). The second case uses
a queue of lines fetched by `getline()`; this queue is fixed and circular when `-n`
is used and grows dynamically if `-r` is specified by itself. The third case
simply seeks to an offset of N from the end of the file and prints the remaining
bytes for seekable inputs and uses a circular queue analogous to that in the
second case for unseekable inputs.

`-f` is applied independently of the other options and their associated `tail` modes,
and follows files only when the input is a regular file or a FIFO specied with a
file operand.

## Status

- [x] Default behavior with no options
- [x] Shrink very large buffers when no longer necessary
- [x] `-c`, `-n`, and `-r` options
- [x] `-f` option
- [x] Full POSIX implementation

## References

- The Open Group. 
  *[tail](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/tail.html)*.
  *The Open Group Base Specifications Issue 8*, IEEE Std 1003.1-2024, 2024.

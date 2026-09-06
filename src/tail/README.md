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

`getline()` is used to read in each line which is maintained in a circular array
queue of the last `n` lines, where `n` is the number of lines requested. However,
while `getline()` will `realloc()` a buffer that is too small, it will never shrink
that buffer. To prevent these buffers from getting out of hand, the program will
resize a buffer if its size has crossed a certain threshold.

## Status

- [x] Default behavior with no options
- [x] Shrink very large buffers when no longer necessary
- [ ] `-c`, `-n`, and `-r` options
- [ ] `-f` option
- [ ] Full POSIX implementation

## References

- IEEE Std 1003.1-2024, *tail*.

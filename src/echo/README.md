# echo

write arguments to standard output

## Synopsis

`echo [string...]`

## Description

Prints its arguments, separated by a single space, followed by a newline, to
standard output.

## Options

No options are supported.

## Specification

Primary specification:
POSIX.1-2024

## Implementation Notes

I've opted for a deliberately minimal POSIX-compliant implementation here. In
particular, POSIX specifies that `echo` shall not support any options.
Consequently, this implementation does not recognize `-n`, `-e`, `-E`, or `--` 
as options and instead treats them as operands.

Additionally, the base specification states that the handling backslash espcape 
sequences is implementation-defined. I've chosen to not interpret them and
therefore, this implementation writes all operands literally.

## Status

- [x] Basic POSIX implementation.

## References

- IEEE Std 1003.1-2024, *echo*.
- Free Software Foundation. “echo(1).” *GNU Coreutils Manual*, version 9.11,
  2026.

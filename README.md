# Unix Utilities

A collection of Unix/Linux utilities implemented in C.

## Goals

My goals with this project are to put into practice what I've learned in C.

Additionally, this project also serves as an exercise in systems and Unix
programming and will be complementing further studies in computer systems and
the Unix/Linux programming interface.

## Utilities

| Utility | Status
|---|---|
| [`echo`](src/echo/) | ✓ Complete |
| [`cat`](src/cat) | ✓ Complete |
| [`wc`](src/wc/) | ◐ Partial |
| [`head`](src/head/) | ✓ Complete |
| [`tail`](src/tail/) | ◐ Partial |
| `tee` | -- Not started |
| `cp` | -- Not started |
| `dd` | -- Not started |
| `sort` | -- Not started |
| `uniq` | -- Not started |
| `grep` | -- Not started |
| `cut` | -- Not started |
| `paste` | -- Not started |
| `join` | -- Not started |
| `tr` | -- Not started |
| `stat` | -- Not started |
| `touch` | -- Not started |
| `chmod` | -- Not started |
| `ln` | - Not started |
| `readlink` | -- Not started |
| `realpath` | -- Not started |
| `mkdir` | -- Not started |
| `rmdir` | -- Not started |
| `ls` | -- Not started |
| `find` | -- Not started |
| `du` | -- Not started |
| `rm` | -- Not started |

**Status:**\
✓ **Complete** - Meets the project's target specification.\
◐ **Partial** - Functional implementation exists, but the target specification is not yet fully implemented. (See individual READMEs for more details.)\
-- **Not started** - No implementation yet.

## Build & Run

**C Standard:** C17\
**Compiler:** gcc\
**Build system:** Make\
**POSIX feature-test macro**: `_POSIX_C_SOURCE=202405L`

### From the repository root:

To build everything:
```
make
```
To build a specific utility: 
```
make echo
```
To run a specific utility: 
```
./build/echo
```

## Test

Currently, each utility is tested against the equivalent program on my Linux
system and also against the POSIX specifications.

## AI Policy

Source code committed to this repository is written by me. AI tools may be used
for code review, discussion, and/or research, but no AI-generated source code
is committed here or used in any of my projects.

## References

### Specifications

- IEEE and The Open Group. *IEEE Std 1003.1-2024 / The Open Group Base
  Specifications Issue 8: Portable Operating System Interface (POSIX)*.
  IEEE and The Open Group, 2024.

### Manuals

- Free Software Foundation. *GNU Coreutils Manual*. GNU Coreutils 9.11,
  2026.

### Books

- Bryant, Randal E., and David R. O'Hallaron. *Computer Systems:
  A Programmer's Perspective*. 3rd ed. Pearson, 2016.
- Gustedt, Jens. *Modern C: Covers the C23 Standard*. 3rd ed.
  Manning Publications, 2025.
- Kernighan, Brian W., and Dennis M. Ritchie. *The C Programming Language*.
  2nd ed. Prentice Hall, 1988.
- King, K. N. *C Programming: A Modern Approach*. 2nd ed.
  W. W. Norton & Company, 2008.
- Stevens, W. Richard, and Stephen A. Rago. *Advanced Programming in the
  UNIX Environment*. 3rd ed. Addison-Wesley Professional, 2013.

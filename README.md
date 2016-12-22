# Semaphores

This repository contains my solutions to the exercises in [The Little Book of Semaphores][lbos], written in C.

[lbos]: http://greenteapress.com/wp/semaphores/

## Build

Just run `make`.

It only builds on macOS, since I am using Apple's [Dispatch framework][dispatch] for the semaphores. I would have used [POSIX semaphores][posix], but the POSIX they are not fully implemented in macOS.

[dispatch]: https://developer.apple.com/reference/dispatch/dispatchsemaphore
[posix]: https://linux.die.net/man/7/sem_overview

## Usage

Run `semaphores N` to test the *n*th solution, or run `semaphores` with no arguments to test all solutions.

## License

© 2016 Mitchell Kember

Semaphores is available under the MIT License; see [LICENSE](LICENSE.md) for details.
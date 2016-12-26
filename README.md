# Semaphores

This repository contains my solutions to the exercises in [The Little Book of Semaphores][lbos], written in C.

[lbos]: http://greenteapress.com/wp/semaphores/

## Build

Just run `make`.

It only builds on macOS, since I am using Apple's [Dispatch framework][dispatch] for the semaphores. I would have used [POSIX semaphores][posix], but the POSIX they are not fully implemented in macOS.

[dispatch]: https://developer.apple.com/reference/dispatch/dispatchsemaphore
[posix]: https://linux.die.net/man/7/sem_overview

## Usage

Run `bin/semaphores --help` to see the instructions:

```
usage: semaphores [options]

  Default
    semaphores -p 5 -n 5 -j 1

  Test options
    -t N  Test only problem N (1 to 15), not all problems
    -p N  Test success with semaphores (positive case), N iterations
    -n N  Test failure without semaphores (negative case), N iterations
    Use -p0 to disable positive tests and -n0 to disable negative tests

  Other options
    -j N  Run N jobs in parallel
    -i    Use interactive mode (display updates in alternate screen)
```

Try running `bin/semaphores -p 100 -n 100 -j 16 -i` :)

## License

© 2016 Mitchell Kember

Semaphores is available under the MIT License; see [LICENSE](LICENSE.md) for details.
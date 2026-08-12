# Contributing

Species-Blossom is a historical C/C++ codebase (last touched ~2006) that builds
with nothing more than `gcc`, `g++`, `make` and (optionally) `gnuplot`. Keep it
that way.

## Build & test locally

```sh
make                 # builds ./blossom
make clean && make   # from-scratch rebuild
timeout 30 ./blossom # smoke test (the evolver loops forever; Ctrl-C / timeout to stop)
```

Set `NO_PLOT=1 ./blossom` to skip the gnuplot live plot (still requires
`gnuplot` to be on `PATH` at startup, because the plotter pipe is opened
eagerly in `main()`).

## Guidelines

- **No new heavy dependencies.** wxWidgets is *not* required — the `wx-config`
  lines in the `Makefile` are dead and should stay gone. Keep the build to the
  standard C/C++ toolchain.
- **License headers.** Preserve the existing per-file license stated in each
  header (see `doc/PROVENANCE.md`). Add a header naming the license for any new
  file you contribute.
- **Keep commits focused.** One logical change per commit; avoid bundling
  unrelated refactors.
- **CI.** `.github/workflows/build.yml` builds on `ubuntu-latest` and runs a
  smoke test. Make sure `make` succeeds and the binary starts before opening a
  PR.

## Reporting issues

There is no active upstream; open an issue in this repository. A minimal
reproduction (input warriors + command line) helps enormously.

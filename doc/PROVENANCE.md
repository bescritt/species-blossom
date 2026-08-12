# Provenance & Licensing

Species-Blossom is a **fork/derivative** of earlier Corewars evolver work by the
author `Varfar` (Marcus?). The repository bundles several independently
licensed components. This document records what we know; **where a component is
ambiguous, the file's own header wins.**

## Components

| Tree / file | Author | License | Notes |
|-------------|--------|---------|-------|
| `blossom.cpp`, `world.*`, `warrior.*`, `population.*`, `reproduction.*`, `benchmark.*`, `battle.*`, `insn_markov.*`, `blossom_utils.*`, `rand.*`, `rand.cpp`, `inst_gen*.{cpp,hpp}`, `randomc.h`, `ui_gnuplot.*` | `Varfar` (2003–2006) | **GPL v1.0 or later** (per source headers) | The bulk of the evolver. Source headers state GPLv1-or-later; the root `LICENSE` file is LGPL-2.1 — see note below. |
| `LICENSE` (root) | — | **GNU LGPL 2.1** | Present in the source tree as the project license file. |
| `exhaust-1.9.2/` (except `exhaust.c`) | M. Joonas Pihlaja (2002) | **Public domain** | Per `exhaust-1.9.2/COPYING`. |
| `exhaust-1.9.2/exhaust.c` | contains code from pMARS 0.9.2 | **GPL v2** | `npos()` and `posit()` are derived from pMARS; covered by the GPL text in `exhaust-1.9.2/COPYING`. |
| `mersenne.cpp` | M. Matsumoto & T. Nishimura | **BSD-style / public domain** | Standard Mersenne Twister reference implementation. |
| `plot/*`, `warriors/red2rc`, `Doxyfile`, `Makefile`, `zipup`, `IDEA.md` | `Varfar` / misc | Unlicensed build/helper scaffolding | Not distributed as compiled code. |

## Why two project licenses?

The source headers say *"GNU General Public License … version 1, or (at your
option) any later version"* while the root `LICENSE` is the **LGPL 2.1** text.
We preserve both verbatim rather than guessing the author's intent. The
canonical file-level rule therefore is:

> **Each file is governed by the license stated in its own header.**
> Where no header states one, the root `LICENSE` (LGPL-2.1) applies.

Contributors should keep this convention: add or keep a header that names the
license you intend for that file.

## Upstream references

- *exhaust* 1.9.2 by M. Joonas Pihlaja — a public-domain pmars-like Redcode
  simulator (originally at `http://www.iki.fi/jpihlaja/exhaust/`).
- pMARS — the classic Portable Memory Array Redcode Simulator, from which
  `exhaust.c` borrows `npos()`/`posit()`.
- Corewars — the programming game; see <https://corewars.org>.

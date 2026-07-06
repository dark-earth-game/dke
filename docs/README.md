# DKE Reverse Engineering Docs

Living documentation for the Dark Earth (Kalisto Entertainment, 1997) reverse-engineering effort.

## Index

- [ARCHITECTURE.md](ARCHITECTURE.md) — high-level engine layout (libsys / game / platform split).
- [ROADMAP.md](ROADMAP.md) — prioritized porting order and milestones.
- [CONVENTIONS.md](CONVENTIONS.md) — naming, asm→C style rules, verification workflow.
- [FUNCTION_MAP.md](FUNCTION_MAP.md) — canonical map: original address → asm name → C name → status.
- [SUBSYSTEMS.md](SUBSYSTEMS.md) — subsystem summaries (memory, graphics, audio, movie, input, filesystem).
- [functions/](functions/) — per-function conversion notes (asm excerpt + C notes + verification).

## Source binary

- `dkev.exe` — no-CD patched Windows executable.
- `dkev.asm` — IDA Free 8.4 full disassembly (~970k lines, 1404 procs).
- `dkev.i64` — IDA project.

## Contribution rules

1. Every non-trivial ported function gets an entry in [FUNCTION_MAP.md](FUNCTION_MAP.md).
2. Each C function keeps its original address as a `// XXXXXXXX` comment (already done in `main.c`).
3. New subsystems get a note in [SUBSYSTEMS.md](SUBSYSTEMS.md).
4. Behavior parity checks live in [functions/](functions/) as short notes.

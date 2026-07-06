# Per-function notes

One markdown file per non-trivial port. Template:

```
# <name> — <addr>

## Signature
<C prototype>

## asm summary
<what registers / stack look like, what the flow does>

## Port notes
<any non-obvious mapping decisions>

## Parity
- build: yes/no
- launch: yes/no
- behavior: <what was checked>
```

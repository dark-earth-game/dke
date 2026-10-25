# Porting a level script (LIEUnn.DLL)

Each room of Dark Earth has a script DLL, `bin/game/LIEUnn.DLL`. dke
replaces each DLL with a native C port in `src/levels/`. Levels 1 and 2 are
the reference ports. This guide covers how to port another one.

## What a level DLL is

The DLL exports one function, `@FonctionEditor@16`:

- `ecx` = opcode, `edx` = the game-wide state block (`dword_505C40`)
- stack = `arg`, `arg2`, and the engine callback table

On every call the entry copies the engine's callback table into DLL globals
and stores the state pointer. It then dispatches on the opcode:

| op | meaning | C port |
|----|---------|--------|
| -1 | per-frame tick: pending event, then the level's tick body | `tick()` |
| 1 | INIT: arm events, then trigger 0 (or the death ending) | `init(arg)` |
| 2 | teardown (no-op in the DLL) | nothing |
| 3 | zone trigger `arg` (table of 42 handlers, filled by INIT) | `lNN_trigger(arg)` |
| 4, 5, 6 | event4 / event5 / event6 | `lieu_event4/5/6` (shared) |

The engine loads the DLL for each room, so DLL globals start from their
initial values on every visit.

## Tools

```sh
python3 tools/lieu_annotate.py bin/game/LIEUnn.DLL > lieunn_ann.dis
```

The header of the output lists:

- the entry address and the **state pointer global** (shown as `S` in the
  listing)
- the **callback globals**: which DLL global holds which `lieu_cb_t`
  member. The addresses differ in every DLL, so always use this map, never
  the addresses from another level.
- the **trigger table**: trigger index to handler address
- **functions identical to an already-ported level**. These are the same
  code with only the relocations changed. Reuse the existing C instead of
  porting them again. In the listing, `lieu_event4`, `lieu_event5`,
  `lieu_event6`, `lieu_action_register` and `lieu_pre_scene` are the shared
  ports in `src/levels/lieu_common.c`.

The listing annotates:

- callback calls (`; CB->dialogue`)
- state accesses (`; ST(V_DELIA_PHASE)`, `; ST(0xE9C)`) and their
  addresses (`; &ST(0x68)`, for `CB->modify_var`)
- strings and negative immediates
- calls through the thunk block, resolved to the target function

If the header says a function is identical to an L01/L02 function, read
the existing C port of that function before porting anything around it.

## Files to create

Copy the layout of level 2:

- `src/levels/lieuNN_int.h`: includes `lieu_level.h`, declares
  `lNN_trigger` and the helpers both C files use.
- `src/levels/lieuNN.c`: the entry `lieuNN_fonction_editor`, `init`,
  `tick`, the tick body and level-specific helpers.
- `src/levels/lieuNN_trig.c`: the 42-entry trigger table, the handlers,
  `lNN_trigger`, and `trig_noop` for empty slots.

Then run `tools/gen_levels.sh`. It regenerates `levels_table.h`, which
connects `lieuNN_fonction_editor` to level NN. `make` picks up every file in
`src/levels/` automatically.

The entry follows `lieu02_fonction_editor` exactly. It sets
`g_lvl_S = state`, dispatches, and returns `ST(LIEU_ST_RESULT)`. Skip the
callback copy and the trigger-table fill in INIT: the bridge supplies
`g_lieu_cb`, and the trigger table is a static array. Port everything after
those two steps.

If op 4, 5 or 6 calls a function the annotator does **not** mark as
identical to `lieu_event4/5/6`, the level has its own variant. Port it in
`lieuNN.c` and call it from the entry.

## Calling convention

The callbacks and DLL helpers are `__fastcall`:

- first argument in `ecx`
- second in `edx`
- the rest pushed right to left, so the last `push` is the third argument

For example:

```
push 0xa ; push 0x0 ; push 0x0 ; ecx = -hero ; edx = 0 ; call CB->joindre
-> CB->joindre(-ST(ST_HERO), 0, 0, 0, 10);
```

The C signatures are in `src/lieu_cb.h`.

## Writing the C

- Port **every instruction**. Keep the original control flow; a `goto`
  with the original label is fine when a structured form would distort
  it. Put the original address in a comment on each function, for example
  `/* trigger 5 — sub_1000ABFF */`, and on non-obvious labels.
- Use the names, never the raw numbers, when the meaning is known:
  - `src/levels/lieu_names.h`: persona ids `P_*`, Divers ops `DV_*`,
    `CAM_*`, `BLOQUE_*`, the engine slots `ST_HERO`, `ST_LEVEL`,
    `ST_VIE(P_x)` with `VIE_DEAD`, and others.
  - `src/levels/lieu_vars.h`: script variables, `V_*`. The state block is
    game-wide, so an offset has the same meaning in every level. When the
    annotator prints a `V_` name, use it.
  - Leave an unknown slot as `ST(0xOFF)`. Don't add names to
    `lieu_vars.h` in a level port. Propose them in your report instead.
- Write coordinates (`pos_perso`, `joindre`, `ajoute_zone`, `bruitage`,
  `etat_objet`) and room numbers in decimal. Keep angles and bit masks in
  hex.
- Pass persona ids to `pos_perso`, `joindre`, `combat`, `fouille`,
  `dialogue` and `lieu_action_register` as `P_*` when the id is named.
- Match the comment density and style of `lieu02.c` / `lieu02_trig.c`.

## Checking a port

1. `make` builds with no new warnings.
2. Smoke test: start a new game and jump straight to the room:

   ```sh
   cd bin/game
   DKE_GOTO=NN DKE_NEWGAME=1 DKE_NOMOVIE=1 DKE_NOTRANS=1 DKE_FRAMES=3000 DKE_LIEUDBG=1 ./dke
   ```

   The log must show `lieu: LIEUnn native scripting — running INIT`, with no
   crash or assertion, and the room's `PosPerso`, zone and dialogue
   activity. The story state from a new game may not match a real visit, so
   a silent room is not necessarily wrong.
3. Review the port against the listing, function by function: every
   callback call, every state write, and every branch.

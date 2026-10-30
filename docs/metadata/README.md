# Game metadata

Reference catalogues of what the game contains. They are built from the game data
(`bin/game`) and the ported room scripts (`src/levels`) by
[`tools/gen_metadata.py`](../../tools/gen_metadata.py). Do not edit the pages by
hand. To rebuild them after a data or port change, run:

```sh
python3 tools/gen_metadata.py            # bin/game -> docs/metadata
```

| Page | Contents |
|---|---|
| [rooms.md](rooms.md) | Every room: cameras, exits, characters, items, movies, music, sounds, dialogue lines |
| [characters.md](characters.md) | Persona ids, names, models, the rooms they appear in, lines spoken |
| [objects.md](objects.md) | Items: names, descriptions, `OBJETS.CTX` records, combinations, ammunition |
| [cinematics.md](cinematics.md) | Movies: length, where they play, first subtitle line |
| [dialogue.md](dialogue.md) | Dialogue file format, codes, line counts per room |
| [music-and-sounds.md](music-and-sounds.md) | MOD tracks and WAV sounds per room |
| [script-variables.md](script-variables.md) | The script state block: named offsets and timer events |

The original data has no room names and no movie titles, so rooms are listed by
number and movies by file name with their first subtitle line. Character and item
names come from the dialogue speaker comments and `OBJNAME.TXT` / `OBJDESCR.TXT`.

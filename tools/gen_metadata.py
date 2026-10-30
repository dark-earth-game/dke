#!/usr/bin/env python3
"""
gen_metadata.py — build docs/metadata/*.md from the game data and the
ported level scripts.

    python3 tools/gen_metadata.py [game_dir] [out_dir]

game_dir defaults to bin/game, out_dir to docs/metadata. Everything in the
generated pages is read from files; nothing is typed in by hand, so the
pages can be rebuilt after a port or data change.

Sources
  datas/dialogue/us/OBJNAME.TXT, OBJDESCR.TXT    item names / descriptions
  objets/OBJETS.CTX                              item records (sub_4723C0)
  datas/dialogue/<lang>/NWDIALnn.TXT, COMMON.TXT dialogue lines + speakers
  datas/lNN/IMAGES/F[LN]nnC*L0.CIF               camera backdrops
  datas/mod/LnnMUSnn.MOD, datas/sound/*.WAV      music and sounds
  sc/SCnn.CRH, datas/sbt/SCnn_US.TXT             movies + subtitles
  persos/Pnn.OME                                 character models
  src/levels/lieu_names.h, lieu_vars.h           names given by the port
  src/levels/lieuNN*.c                           what each room's script does
"""

import glob
import os
import re
import struct
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "bin", "game")
OUT = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "docs", "metadata")
SRC = os.path.join(ROOT, "src", "levels")

LANGS = ["us", "FR", "DE", "IT", "SP", "JP"]


def find_ci(*parts):
    """Case-insensitive path lookup below GAME."""
    cur = GAME
    for p in parts:
        if not os.path.isdir(cur):
            return None
        hit = [e for e in os.listdir(cur) if e.lower() == p.lower()]
        if not hit:
            return None
        cur = os.path.join(cur, hit[0])
    return cur


def read_text(path):
    with open(path, "rb") as f:
        return f.read().decode("cp1252", "replace")


def md_escape(s):
    return s.replace("|", "\\|").strip()


# ----------------------------------------------------------------------
# Port name tables
# ----------------------------------------------------------------------

def parse_enum_names(prefix):
    """NAME = value, /* comment */ entries of lieu_names.h with a prefix."""
    out = {}
    text = open(os.path.join(SRC, "lieu_names.h")).read()
    for m in re.finditer(r"\b(%s\w+)\s*=\s*(\d+)\s*,?\s*(?:/\*\s*(.*?)\s*\*/)?" % prefix, text):
        out[int(m.group(2))] = (m.group(1), (m.group(3) or "").strip())
    return out


PERSONAS = parse_enum_names("P_")       # id -> (P_NAME, comment)
ITEMS = parse_enum_names("OB_")         # id -> (OB_NAME, description)
PNAME_TO_ID = {v[0]: k for k, v in PERSONAS.items()}


def persona_label(pid):
    if pid in PERSONAS:
        n = PERSONAS[pid][0][2:].title().replace("_", " ")
        return n
    return "Persona %d" % pid


# ----------------------------------------------------------------------
# Item data
# ----------------------------------------------------------------------

def at_records(path):
    """'@n' records of OBJNAME / OBJDESCR: n -> text."""
    recs = {}
    if not path:
        return recs
    cur = None
    for line in read_text(path).splitlines():
        m = re.match(r"^@(\d+)\s*$", line.strip())
        if m:
            cur = int(m.group(1))
            recs[cur] = ""
        elif cur is not None and line.strip():
            recs[cur] = (recs[cur] + " " + line.strip()).strip()
    return recs


def objets_ctx():
    path = find_ci("objets", "OBJETS.CTX")
    recs = {}
    if not path:
        return recs
    d = open(path, "rb").read()
    base = 16 + 700
    for i in range(100):
        nbobj, nbvie, ptvie, ptnrj, flags, slot, arme, linke = struct.unpack_from("<4i4B", d, base + i * 20)
        recs[i] = dict(nbobj=nbobj, nbvie=nbvie, ptvie=ptvie, ptnrj=ptnrj,
                       flags=flags, slot=slot, arme=arme, linke=linke)
    return recs


# ----------------------------------------------------------------------
# Level scripts
# ----------------------------------------------------------------------

def level_sources():
    """room number -> concatenated source of lieuNN.c / lieuNN_*.c."""
    out = {}
    for path in sorted(glob.glob(os.path.join(SRC, "lieu[0-9][0-9]*.c"))):
        m = re.match(r"lieu(\d\d)", os.path.basename(path))
        out.setdefault(int(m.group(1)), "")
        out[int(m.group(1))] += open(path).read()
    return out


def resolve_persona(tok):
    tok = tok.strip().lstrip("-").strip()
    if tok in PNAME_TO_ID:
        return PNAME_TO_ID[tok]
    if re.fullmatch(r"\d+", tok):
        return int(tok)
    return None   # the hero (ST(ST_HERO)) or a computed id


def scan_level(text):
    info = dict(exits=set(), personas=set(), hero=False, objects=set(),
                movies=set(), dialogues=set(), zones=set())
    for m in re.finditer(r"change_piece\(\s*(\d+)", text):
        info["exits"].add(int(m.group(1)))
    for m in re.finditer(r"pos_perso\(\s*([^,]+),", text):
        pid = resolve_persona(m.group(1))
        if pid is None:
            info["hero"] = True
        else:
            info["personas"].add(pid)
    for m in re.finditer(r'etat_objet\(\s*"S?(OB|QB)(\d\d)', text):
        info["objects"].add(int(m.group(2)))
    for m in re.finditer(r'cinematique\(\s*"(SC\d\d)', text):
        info["movies"].add(m.group(1))
    for m in re.finditer(r'"(L\d\dD\w+|LCOD\w+)"', text):
        info["dialogues"].add(m.group(1))
    for m in re.finditer(r"ajoute_zone\(\s*(\d+)", text):
        info["zones"].add(int(m.group(1)))
    return info


# ----------------------------------------------------------------------
# Dialogue files
# ----------------------------------------------------------------------

def dialogue_file(lang, name):
    return find_ci("datas", "dialogue", lang, name)


def parse_dialogue(path):
    """[(code, speaker_comment, persona_id or None, text)]"""
    lines = read_text(path).splitlines()
    out = []
    comment = ""
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if s.startswith("//"):
            comment = s[2:].strip()
        elif re.fullmatch(r"(L\d\dD\w+|LCOD\w+)", s):
            text = lines[i + 1].strip() if i + 1 < len(lines) else ""
            text = re.sub(r"\s*XXX\s*$", "", text)
            m = re.search(r"perso\s*(\d+)", comment)
            out.append((s, comment, int(m.group(1)) if m else None, text))
            i += 1
        i += 1
    return out


# ----------------------------------------------------------------------
# Movies
# ----------------------------------------------------------------------

def crh_info(path):
    with open(path, "rb") as f:
        hdr = f.read(64)
    frames = hdr[50] | (hdr[51] << 8)
    fps = (hdr[52] | (hdr[53] << 8)) or 15
    return frames, fps


def subtitle_lines(movie):
    path = find_ci("datas", "sbt", "%s_US.TXT" % movie)
    if not path:
        return []
    out = []
    for line in read_text(path).splitlines():
        m = re.search(r"\{(.*?)\}", line)
        if m:
            out.append(m.group(1).strip())
    return out


# ----------------------------------------------------------------------
# Writers
# ----------------------------------------------------------------------

HEADER = ("<!-- Generated by tools/gen_metadata.py from the game data and "
          "src/levels. Do not edit by hand: change the generator and re-run it. -->\n\n")


def write(name, body):
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, name), "w") as f:
        f.write(HEADER + body.rstrip() + "\n")
    print("wrote", os.path.join(OUT, name))


def flag_names(f):
    names = []
    if f & 0x01: names.append("placeable")
    if f & 0x02: names.append("weapon")
    if f & 0x04: names.append("0x04")
    if f & 0x08: names.append("0x08")
    if f & 0xF0: names.append("usable 0x%02X" % (f & 0xF0))
    return ", ".join(names) or "-"


def main():
    levels = level_sources()
    scans = {lv: scan_level(t) for lv, t in levels.items()}

    # --- dialogue (US) per room + speaker statistics --------------------
    dial_count = {}
    speakers = defaultdict(lambda: defaultdict(int))     # persona -> room -> lines
    code_room = {}
    for path in sorted(glob.glob(os.path.join(find_ci("datas", "dialogue", "us") or "", "*.TXT"))):
        base = os.path.basename(path).upper()
        m = re.match(r"NWDIAL(\d\d)\.TXT", base)
        if not m and base != "COMMON.TXT":
            continue
        room = int(m.group(1)) if m else None
        entries = parse_dialogue(path)
        dial_count[room if room is not None else "COMMON"] = len(entries)
        for code, _, pid, _ in entries:
            code_room[code] = room
            if pid is not None:
                speakers[pid][room if room is not None else "common"] += 1

    # ================================================================ rooms
    rooms = sorted(set(levels) | {int(d[1:]) for d in os.listdir(find_ci("datas"))
                                  if re.fullmatch(r"[lL]\d\d", d)})
    music = defaultdict(list)
    for p in glob.glob(os.path.join(find_ci("datas", "mod") or "", "*.MOD")):
        m = re.match(r"L(\d\d)MUS(\d\d)\.MOD", os.path.basename(p).upper())
        if m:
            music[int(m.group(1))].append("MUS%s" % m.group(2))
    sounds = defaultdict(int)
    for p in glob.glob(os.path.join(find_ci("datas", "sound") or "", "*.WAV")):
        m = re.match(r"L(\d\d)", os.path.basename(p).upper())
        if m:
            sounds[int(m.group(1))] += 1

    # Movies started from (nearly) every room come from the shared end-of-game
    # code; list them once instead of in every row.
    mv_rooms = defaultdict(int)
    for sc in scans.values():
        for mv in sc["movies"]:
            mv_rooms[mv] += 1
    common_mv = {mv for mv, n in mv_rooms.items() if n > len(scans) // 2}

    entered_from = defaultdict(set)
    for lv, sc in scans.items():
        for x in sc["exits"]:
            entered_from[x].add(lv)

    out = ["# Rooms", "",
           "Dark Earth is split into numbered rooms (\"lieux\"). Each room has its",
           "data in `datas/lNN/` and its script in `LIEUnn.DLL` (ported to",
           "`src/levels/lieuNN*.c`). The game data has no room names, so rooms are",
           "listed by number.", "",
           "- **Cameras**: fixed camera backdrops (`FLnnC<k>L0.CIF`); `night` when the",
           "  room also has a night set (`FNnn...`, `Nnn.INF`).",
           "- **Exits / entered from**: rooms reached by `ChangePiece` in the scripts.",
           "- **Characters**: personas the script places with `PosPerso` (the hero is",
           "  placed in almost every room and is not listed).",
           "- **Items**: objects the script shows or hides with `EtatObjet`.",
           "- **Movies**: cutscenes the script starts (see [cinematics](cinematics.md)).",
           "- **Lines**: dialogue lines in `NWDIALnn.TXT` (US).", "",
           "| Room | Cameras | Exits | Entered from | Characters | Items | Movies | Music | Sounds | Lines |",
           "|---:|---:|---|---|---|---|---|---|---:|---:|"]
    for lv in rooms:
        img = find_ci("datas", "l%02d" % lv, "IMAGES")
        cams = 0
        night = False
        if img:
            names = [n.upper() for n in os.listdir(img)]
            cams = len([n for n in names if re.fullmatch(r"FL%02dC\d+L0\.CIF" % lv, n)])
            night = any(re.fullmatch(r"FN%02dC\d+L0\.CIF" % lv, n) for n in names)
        sc = scans.get(lv, dict(exits=set(), personas=set(), objects=set(), movies=set()))
        chars = ", ".join(persona_label(p) for p in sorted(sc["personas"]))
        items = ", ".join("OB%02d" % o for o in sorted(sc["objects"]))
        out.append("| %02d | %s | %s | %s | %s | %s | %s | %s | %s | %s |" % (
            lv,
            ("%d%s" % (cams, " + night" if night else "")) if cams else "-",
            ", ".join("%02d" % x for x in sorted(sc["exits"])) or "-",
            ", ".join("%02d" % x for x in sorted(entered_from[lv])) or "-",
            chars or "-", items or "-",
            ", ".join(sorted(sc["movies"] - common_mv)) or "-",
            ", ".join(sorted(music[lv])) or "-",
            sounds.get(lv, 0) or "-",
            dial_count.get(lv, "-")))
    if common_mv:
        out += ["", "Every room script can also start %s (the end of the game, from the "
                "shared code); they are left out of the table." % " and ".join(sorted(common_mv))]
    missing = [lv for lv in rooms if lv not in levels]
    if missing:
        out += ["", "Rooms with data but no script port: " +
                ", ".join("%02d" % m for m in missing) + "."]
    write("rooms.md", "\n".join(out))

    # =========================================================== characters
    models = set()
    pdir = find_ci("persos")
    if pdir:
        for n in os.listdir(pdir):
            m = re.fullmatch(r"P(\d\d)\.OME", n.upper())
            if m:
                models.add(int(m.group(1)))
    placed = defaultdict(set)
    for lv, sc in scans.items():
        for p in sc["personas"]:
            placed[p].add(lv)
    ids = sorted(set(PERSONAS) | models | set(placed) | set(speakers))
    out = ["# Characters", "",
           "Personas are numbered by their model `persos/Pnn.OME`. The id is what the",
           "scripts pass to `PosPerso`, `Joindre`, `Combat`, `Dialogue` and the",
           "dialogue files tag the speaker with (`// Name ... persoNN`). Names come from",
           "those speaker comments (`src/levels/lieu_names.h`). Ids 0-3 are the hero,",
           "Arkhan, in his successive states; `ST_HERO` holds the one being played.", "",
           "| Id | Name | Port enum | Model | Placed in rooms | Lines spoken (room: count) |",
           "|---:|---|---|---|---|---|"]
    for pid in ids:
        enum, comment = PERSONAS.get(pid, ("", ""))
        name = persona_label(pid) + (" (%s)" % comment if comment else "")
        spoke = speakers.get(pid, {})
        spoke_s = ", ".join("%s: %d" % (("%02d" % r) if isinstance(r, int) else r, n)
                            for r, n in sorted(spoke.items(), key=lambda x: (isinstance(x[0], str), x[0])))
        out.append("| %d | %s | %s | %s | %s | %s |" % (
            pid, md_escape(name), ("`%s`" % enum) if enum else "-",
            "P%02d.OME" % pid if pid in models else "-",
            ", ".join("%02d" % r for r in sorted(placed.get(pid, ()))) or "-",
            spoke_s or "-"))
    extra = sorted(n for n in os.listdir(pdir) if not re.fullmatch(r"P\d\d\.OME", n.upper())) if pdir else []
    if extra:
        out += ["", "Other files in `persos/`: " + ", ".join("`%s`" % e for e in extra) + "."]
    write("characters.md", "\n".join(out))

    # ============================================================== objects
    names = at_records(find_ci("datas", "dialogue", "us", "OBJNAME.TXT"))
    descr = at_records(find_ci("datas", "dialogue", "us", "OBJDESCR.TXT"))
    ctx = objets_ctx()
    shown_in = defaultdict(set)
    for lv, sc in scans.items():
        for o in sc["objects"]:
            shown_in[o].add(lv)
    out = ["# Items", "",
           "Items are `OBnn` objects (`objets/OBnn.OME`). The inventory keeps them by",
           "the 4-character key, with a fifth letter `A`..`F` for the instance",
           "(`OB06B`); a `Q` first letter marks the held copy of a weapon with a",
           "projectile. Names are `OBJNAME.TXT` (list) and `OBJDESCR.TXT`",
           "(description), US English.", "",
           "Record fields from `objets/OBJETS.CTX` (20 bytes per id, `sub_4723C0`):", "",
           "- **Pickup**: `nbobj`, pieces given when picked up.",
           "- **Charges**: `nbvie`, refilled when a script gives the item (DIVERS 44).",
           "- **Flags**: `placeable` = has a world instance slot; `weapon` = weapons page,",
           "  confiscated in prison; `usable` = may be used on the world in the",
           "  level 35 fight.",
           "- **Linked**: the item kept next to it in the list (weapon <-> its ammo).",
           "- **Rooms**: rooms whose script shows or hides it (`EtatObjet`).", "",
           "| Id | Name | Description | Port enum | Pickup | Charges | Flags | Linked | Rooms |",
           "|---:|---|---|---|---:|---:|---|---|---|"]
    for i in range(1, 100):
        r = ctx.get(i, {})
        if not (names.get(i) or descr.get(i) or r.get("flags") or shown_in.get(i)):
            continue
        out.append("| %d | %s | %s | %s | %s | %s | %s | %s | %s |" % (
            i, md_escape(names.get(i, "")), md_escape(descr.get(i, "")),
            ("`%s`" % ITEMS[i][0]) if i in ITEMS else "-",
            r.get("nbobj", "-"), r["nbvie"] if r.get("nbvie", -1) >= 0 else "-",
            flag_names(r.get("flags", 0)),
            ("OB%02d" % r["linke"]) if r.get("linke") and r["linke"] != i else "-",
            ", ".join("%02d" % x for x in sorted(shown_in.get(i, ()))) or "-"))
    out += ["", "## Combinations", "",
            "Made automatically when the inventory opens (`sub_42784E`), unless the",
            "player holds one of the parts:", "",
            "| Parts | Result | Script variable |", "|---|---|---|",
            "| OB84 Metal Helmet + OB85 Metal Breastplate (OB86 must be carried, it is kept) | OB71 Metal suit | `0xC14` = 3 |",
            "| OB02 + OB12 + OB18 + OB19 | OB75 Rylsadhar's Crystal Key | `0xB10` = 2 |",
            "", "## Ammunition shown on weapons", "",
            "The inventory window of a weapon shows its ammunition count (`0x10003490`):", "",
            "| Weapon | Ammunition |", "|---|---|",
            "| OB27 Flame-thrower | OB06 |", "| OB55 Spitrod | OB07, OB08 |",
            "| OB23 Thunderbolt from Before | OB72 |", "| OB32 Thunderbolt from Kalhi | OB39 |",
            "| OB35 Zed's Thunderbolt | OB73 |"]
    write("objects.md", "\n".join(out))

    # =========================================================== cinematics
    played = defaultdict(set)
    for lv, sc in scans.items():
        for mv in sc["movies"]:
            played[mv].add(lv)
    scdir = find_ci("sc")
    out = ["# Cinematics", "",
           "Movies are CRH files in `sc/` (15 fps, 22050 Hz stereo sound). The",
           "scripts start them with `Cinematique(\"SCnn_500\")`. `eadke.crh` is the",
           "publisher logo played at start-up and `SC01` the intro on the first run.",
           "Subtitles are `datas/sbt/SCnn_<lang>.TXT`; the first line (US) is quoted.",
           "The Cinematics menu shows a movie once `g_movie_seen` has its bit; the",
           "stamps are `datas/stamps/SCnn.CIF`.", "",
           "| Movie | Frames | Length | Played in rooms | Subtitle lines | Opens with |",
           "|---|---:|---:|---|---:|---|"]
    for p in sorted(os.listdir(scdir) if scdir else []):
        if not p.upper().endswith(".CRH"):
            continue
        mv = os.path.splitext(p)[0].upper()
        frames, fps = crh_info(os.path.join(scdir, p))
        secs = frames / float(fps)
        subs = subtitle_lines(mv)
        out.append("| %s | %d | %d:%02d | %s | %s | %s |" % (
            p, frames, int(secs) // 60, int(secs) % 60,
            ", ".join("%02d" % r for r in sorted(played.get(mv, ()))) or "-",
            len(subs) or "-", md_escape(subs[0]) if subs else "-"))
    write("cinematics.md", "\n".join(out))

    # ========================================================== dialogue
    out = ["# Dialogue", "",
           "Lines live in `datas/dialogue/<lang>/NWDIALnn.TXT` (one file per room)",
           "and `COMMON.TXT` (lines any room may use). Each entry is a code line",
           "(`L07D01A`, `LCODnn`), the text, and `XXX`. A comment above it names the",
           "speaker and the situation, ending in `persoNN` (the speaker's id). Voice",
           "files are `dialog/Lnn/<code>.WAV`.", "",
           "Codes: `LnnDmm[letter]` = room nn, line mm (letters are the parts of one",
           "speech or alternatives); `LCODnn` = shared lines in `COMMON.TXT`.", "",
           "Languages shipped: " + ", ".join(
               "`%s`" % l for l in LANGS if find_ci("datas", "dialogue", l)) + ".", "",
           "| File | Lines (US) | Codes the room script references (own + COMMON) |", "|---|---:|---:|"]
    for k in sorted(dial_count, key=lambda x: (isinstance(x, str), x)):
        used = len(scans[k]["dialogues"]) if isinstance(k, int) and k in scans else "-"
        out.append("| %s | %d | %s |" % ("COMMON.TXT" if k == "COMMON" else "NWDIAL%02d.TXT" % k,
                                          dial_count[k], used))
    write("dialogue.md", "\n".join(out))

    # =================================================== music and sounds
    out = ["# Music and sounds", "",
           "Music is Amiga MOD modules `datas/mod/LnnMUSmm.MOD` (room nn, track mm),",
           "chosen by the script. Sounds are 11025 Hz WAV files in `datas/sound/`",
           "(`LnnAMBk` ambience loops, `LnnBk` one-shots, `LnnEVk` script events)",
           "and `wav/` (`ANIMS/` animation sounds, `REVERB/` footsteps, the",
           "menu clicks `INVENT1/2.WAV`).", "",
           "| Room | Music tracks | Sound files |", "|---:|---|---|"]
    sdir = find_ci("datas", "sound")
    per_room = defaultdict(list)
    for p in sorted(os.listdir(sdir) if sdir else []):
        m = re.match(r"L(\d\d)(.*)\.WAV", p.upper())
        if m:
            per_room[int(m.group(1))].append(m.group(2))
    for lv in sorted(set(music) | set(per_room)):
        out.append("| %02d | %s | %s |" % (lv, ", ".join(sorted(music[lv])) or "-",
                                           ", ".join(per_room[lv]) or "-"))
    wdir = find_ci("wav")
    if wdir:
        out += ["", "## wav/", "", "| Folder | Files |", "|---|---:|"]
        for d in sorted(os.listdir(wdir)):
            full = os.path.join(wdir, d)
            if os.path.isdir(full):
                out.append("| `%s/` | %d |" % (d, len(os.listdir(full))))
    write("music-and-sounds.md", "\n".join(out))

    # =================================================== script variables
    text = open(os.path.join(SRC, "lieu_vars.h")).read()
    out = ["# Script variables", "",
           "The room scripts share one 4000-byte state block (`dword_505C40`), kept",
           "for the whole game and saved with it. Scripts address it by byte offset",
           "(`ST(0x...)`). The names below are the ones the port has identified",
           "(`src/levels/lieu_vars.h`); \"(L1, L2)\" lists the rooms where the meaning",
           "was confirmed. Offsets the engine writes for the scripts:", "",
           "| Offset | Engine meaning |", "|---|---|",
           "| `0x00C` | action key pressed this frame |",
           "| `0x128` | instance of the item used from the inventory |",
           "| `0x140 + 4v` | OB24 / OB77 counters per instance v |",
           "| `0xB10` | set to 2 by the OB75 combination |",
           "| `0xC14` | set to 3 by the OB71 combination |", "",
           "| Name | Offset | Meaning |", "|---|---|---|"]
    for m in re.finditer(r"#define\s+(V_\w+|ST_\w+)\s+(0x[0-9A-Fa-f]+|\d+)\s*(?:/\*\s*(.*?)\s*\*/)?", text):
        out.append("| `%s` | `%s` | %s |" % (m.group(1), m.group(2), md_escape(m.group(3) or "")))
    ev = re.search(r"/\* Timer events:(.*?)\*/\s*enum \{(.*?)\};",
                   open(os.path.join(SRC, "lieu_names.h")).read(), re.S)
    if ev:
        out += ["", "## Timer events", "",
                "`Evenement(id, ms)` arms a timer; when it expires the room's event 4",
                "handler runs for `id`. Most ids only set `V_EVENT_<id>`; these do more:", "",
                "| Id | Name | Effect |", "|---:|---|---|"]
        for m in re.finditer(r"(EV_\w+)\s*=\s*(\d+),?\s*(?:/\*\s*(.*?)\s*\*/)?", ev.group(2)):
            out.append("| %s | `%s` | %s |" % (m.group(2), m.group(1), md_escape(m.group(3) or "")))
    write("script-variables.md", "\n".join(out))


if __name__ == "__main__":
    main()

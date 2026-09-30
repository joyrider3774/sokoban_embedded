#!/usr/bin/env python3
"""Convert the level packs in assets/levelpacks to Source/blips_espboy/Levelpacks.h.

Every <name>.bip becomes one array, levelpack_<name>, holding the file byte for byte.
The arrays carry no terminator: CLevelPackFile_parseText is handed sizeof() of the
array, which is what stops one pack running into the next where the linker packs
them back to back in flash.

Packs are written in alphabetical order, matching the order CLevelPackFile_loadFile
names them in.

Usage:
  python convert_levelpacks.py            write Levelpacks.h
  python convert_levelpacks.py --verify   build Levelpacks.h in memory and compare it
                                          with the current one, nothing is written
  --output FILE                           write (or verify) this file instead
"""
import argparse
import difflib
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
PACKS_DIR = os.path.join(ROOT, "assets", "levelpacks")
OUTPUT = os.path.join(ROOT, "Source", "sokoban_embedded", "Levelpacks.h")
DEFINES = os.path.join(ROOT, "Source", "sokoban_embedded", "Defines.h")
#the region of Defines.h this tool owns, see build_switches()
BEGIN = "//>>> written by tools/convert_levelpacks.py from assets/levelpacks, do not edit by hand"
END = "//<<<"
BYTES_PER_LINE = 16


def c_name(file_name):
    return "levelpack_" + re.sub(r"\W", "_", os.path.splitext(file_name)[0])


def read_packs(packs_dir):
    """[(file name, bytes), ...] in alphabetical order."""
    packs = []
    for file_name in sorted(os.listdir(packs_dir)):
        if not file_name.lower().endswith(".sok"):
            continue
        with open(os.path.join(packs_dir, file_name), "rb") as f:
            packs.append((file_name, f.read()))
    return packs


MAX_RUN = 128


def rle(data):
    """The control byte scheme of png2rle565.py, over the bytes of a level pack.

        c & 0x80 : a run,     (c & 0x7F) + 1 copies of the byte that follows
        else     : a literal, c + 1 bytes follow

    A run of two is left as a literal: it would cost the same and reading it is more work."""
    out = bytearray()
    literal = []

    def flush():
        while literal:
            chunk = literal[:MAX_RUN]
            del literal[:MAX_RUN]
            out.append(len(chunk) - 1)
            out.extend(chunk)

    i = 0
    while i < len(data):
        run = 1
        while i + run < len(data) and run < MAX_RUN and data[i + run] == data[i]:
            run += 1
        if run >= 3:
            flush()
            out.append(0x80 | (run - 1))
            out.append(data[i])
            i += run
        else:
            literal.append(data[i])
            i += 1
    flush()
    return bytes(out)


def unrle(data):
    """What the reader in CLevelPackFile.cpp makes of it, so nothing is written it cannot read."""
    out = bytearray()
    i = 0
    while i < len(data):
        control = data[i]
        i += 1
        if control & 0x80:
            out.extend(bytes([data[i]]) * ((control & 0x7F) + 1))
            i += 1
        else:
            count = control + 1
            out.extend(data[i:i + count])
            i += count
    return bytes(out)


def build_header(packs):
    blocks = []
    for file_name, data in packs:
        encoded = rle(data)
        #never write a pack that does not come back out of the reader as it went in
        assert unrle(encoded) == data, file_name
        rows = ["  " + ", ".join("0x%02x" % b for b in encoded[off:off + BYTES_PER_LINE])
                for off in range(0, len(encoded), BYTES_PER_LINE)]
        blocks.append("\n".join([
            "// %s, %d bytes of level text run length encoded to %d" % (file_name, len(data), len(encoded)),
            "const unsigned char %s[] PLATFORM_PROGMEM  = {" % c_name(file_name),
            ", \n".join(rows),
            "};",
        ]))
    return "#ifndef LEVELPACKS_H\n#define LEVELPACKS_H\n" + "\n\n".join(blocks) + "\n\n#endif"


def playfield():
    """NrOfCols and NrOfRows out of Defines.h, which is where the game states them.

    They bound a level the game will accept, so the part count has to know them. They sit
    outside the region this tool writes, so reading the file back is safe."""
    text = open(DEFINES).read()
    found = {}
    for line in text.split("\n"):
        bits = line.split()
        if (len(bits) == 3) and (bits[0] == "#define") and (bits[1] in ("NrOfCols", "NrOfRows")):
            found[bits[1]] = int(bits[2])
    if len(found) != 2:
        raise SystemExit("NrOfCols and NrOfRows are not both in %s" % DEFINES)
    return found["NrOfCols"], found["NrOfRows"]


def busiest_level(data):
    """How many parts the busiest level of a pack has.

    This walks a pack the way CLevelPackFile_parseText does: a level starts at a line with a wall
    in it while no metadata field is open, a line with a colon in it is a field and not a row, and
    an empty line ends the level. The parts a character makes are the same as the switch there:
    a box on a spot and a player on a spot are two parts each, the spot and the thing on it.
    A level wider or taller than the playfield is refused by the game, so it is not counted."""
    PARTS = {"#": 1, "$": 1, ".": 1, "@": 1, "*": 2, "+": 2}
    NrOfCols, NrOfRows = playfield()
    inlevel = False
    parts = 0
    minx = miny = 10000
    maxx = maxy = -1
    y = 0
    most = 0

    def keep(parts, minx, maxx, miny, maxy):
        #the game refuses a level that does not fit the playfield, see CLevelPackFile_endLevel
        if maxx < minx or maxy < miny:
            return 0
        if (maxx - minx + 1) > NrOfCols or (maxy - miny + 1) > NrOfRows:
            return 0
        return parts

    for raw in data.decode("latin-1").split("\n"):
        line = raw.rstrip("\r")
        if inlevel and not line.strip():
            most = max(most, keep(parts, minx, maxx, miny, maxy))
            inlevel = False
            continue
        if ":" in line:
            continue
        if not inlevel:
            if "#" not in line:
                continue
            inlevel = True
            parts = 0
            minx = miny = 10000
            maxx = maxy = -1
            y = 0
        for x, ch in enumerate(line):
            #only a wall sets the bounds the game measures the level by
            if ch == "#":
                minx = min(minx, x)
                maxx = max(maxx, x)
                miny = min(miny, y)
                maxy = max(maxy, y)
            parts += PARTS.get(ch, 0)
        y += 1
    return max(most, keep(parts, minx, maxx, miny, maxy) if inlevel else 0)


def build_switches(packs):
    """The LEVELPACKS switch: which of the packs a build takes.

    A pack that is left out is named nowhere, so the compiler drops its array and the game does
    not offer it. Which packs there are is what lies in assets/levelpacks, the same list the
    arrays are written from, so this is written from there and not kept by hand."""
    names = [c_name(file_name)[len("levelpack_"):] for file_name, _ in packs]
    pad = max(len(n) for n in names)
    file_pad = max(len(f) for f, _ in packs)
    lines = [
        "//how many packs there are in all, which is what the saved unlocks are sized by",
        "#define MaxLevelPacks %d" % len(packs),
        "",
        "//LEVELPACKS: the level packs that are built in, an LP_ bit each. The size is what the pack",
        "//takes in flash, which is its text run length encoded, see build_header. All of them unless",
        "//the device header or the build picks fewer; a pack that is left out takes no flash and is",
        "//not offered in the game",
    ]
    for i, ((file_name, data), name) in enumerate(zip(packs, names)):
        lines.append("#define LP_%-*s (1ul << %2d)    //%-*s %6d bytes"
                     % (pad, name, i, file_pad, file_name, len(rle(data))))
    lines += [
        "#define LP_ALL ((1ul << %d) - 1)" % len(packs),
        "#ifndef LEVELPACKS",
        "#define LEVELPACKS LP_ALL",
        "#endif",
        "#if (LEVELPACKS & LP_ALL) == 0",
        '#error "LEVELPACKS has to leave at least one level pack in"',
        "#endif",
        "//how many of them this build takes, which is how many the game lists",
        "#define LEVELPACKCOUNT ("
        + " + ".join("((LEVELPACKS & LP_%s) != 0)" % n for n in names) + ")",
        "",
        "//The busiest level each pack has. The pool of world parts is the largest thing the game",
        "//asks the heap for and no level fills the whole playfield, so a build wants no more slots",
        "//than the packs it holds can fill, see MAXWORLDPARTS in the device header",
    ]
    for (file_name, data), name in zip(packs, names):
        lines.append("#define LP_PARTS_%-*s %4d" % (pad, name, busiest_level(data)))
    most = "0"
    for name in names:
        most = "LP_PARTS_MAX(%s, LP_PARTS_OF(%s))" % (most, name)
    lines += [
        "",
        "//how many parts the busiest level of the packs this build holds has. A pack that is",
        "//left out counts for nothing, so the count follows what LEVELPACKS says",
        "//One comparison a pack. LP_PARTS_MAX is a function and not a macro on purpose: a macro",
        "//naming its first argument twice doubles the text at every step, which with nineteen",
        "//packs put the compiler out of memory. constexpr keeps it usable where a constant is",
        "//wanted, such as the static_assert below and the size of the pool",
        "static inline constexpr int LP_PARTS_MAX(int a, int b) { return (a > b) ? a : b; }",
        "#define LP_PARTS_OF(p) (((LEVELPACKS & LP_##p) != 0) ? LP_PARTS_##p : 0)",
        "#define LEVELPACKMAXPARTS %s" % most,
    ]
    return "\n".join(lines)


def splice(text, block):
    """Puts block between the markers, which is the part of Defines.h this tool writes."""
    start = text.find(BEGIN)
    end = text.find(END, start + 1) if start >= 0 else -1
    if start < 0 or end < 0:
        raise SystemExit("the markers are gone from %s, put them back" % DEFINES)
    return text[:start] + BEGIN + "\n" + block + "\n" + text[end:]


def parse_header(text):
    """What the game gets out of a Levelpacks.h: every array, as text and as bytes."""
    arrays = {}
    for m in re.finditer(r"(// [^\n]+\n)const unsigned char (\w+)\[\] PLATFORM_PROGMEM\s+= \{\n(.*?)\n\};", text, re.S):
        stored = bytes(int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", m.group(3)))
        arrays[m.group(2)] = {
            "text": m.group(0),
            #what the game reads is the decoded pack, so that is what is compared
            "bytes": unrle(stored),
        }
    return arrays


def verify(new_text, path):
    with open(path, "r", newline="") as f:
        old_raw = f.read()
    # both are compared with plain newlines, the line endings are checked separately
    old_text = old_raw.replace("\r\n", "\n")
    new_arrays, old_arrays = parse_header(new_text), parse_header(old_text)
    ok = True

    same = 0
    for name in sorted(set(new_arrays) | set(old_arrays)):
        if name not in old_arrays:
            ok = False
            print("  MISSING  %s is not in the current file" % name)
        elif name not in new_arrays:
            ok = False
            print("  EXTRA    %s is in the current file but made from no .bip file" % name)
        elif new_arrays[name]["bytes"] != old_arrays[name]["bytes"]:
            ok = False
            print("  DIFFERS  %s holds other level data" % name)
        elif new_arrays[name]["text"] != old_arrays[name]["text"]:
            ok = False
            print("  DIFFERS  %s has the same bytes but is formatted differently" % name)
        else:
            same += 1
            print("  identical  %-32s %6d bytes" % (name, len(new_arrays[name]["bytes"])))
    print("  level pack arrays: %d of %d identical (data and text)" % (same, len(new_arrays)))

    if "\r\n" not in old_raw:
        print("  note: the current file does not use CRLF line endings, a written one would")

    if new_text == old_text:
        print("  whole file: identical")
    else:
        print("  whole file: differs outside the arrays, this has no effect on the game:")
        for line in difflib.unified_diff(old_text.split("\n"), new_text.split("\n"),
                                         "current", "generated", lineterm="", n=1):
            print("      " + line)
    return ok


def main():
    parser = argparse.ArgumentParser(description="Convert assets/levelpacks to Levelpacks.h")
    parser.add_argument("--verify", action="store_true", help="compare with the current Levelpacks.h, write nothing")
    parser.add_argument("--output", default=OUTPUT, help="Levelpacks.h to write or verify")
    args = parser.parse_args()

    packs = read_packs(PACKS_DIR)
    text = build_header(packs)
    summary = ", ".join("%s %d bytes" % (file_name, len(data)) for file_name, data in packs)

    switches = build_switches(packs)
    with open(DEFINES, "r", newline="") as f:
        defines_raw = f.read()
    defines_old = defines_raw.replace("\r\n", "\n")
    defines_new = splice(defines_old, switches)

    if args.verify:
        print("verifying %s against assets/levelpacks (%s)" % (os.path.relpath(args.output, ROOT), summary))
        ok = verify(text, args.output)
        if defines_new == defines_old:
            print("  the LEVELPACKS switch in %s matches" % os.path.basename(DEFINES))
        else:
            ok = False
            print("  the LEVELPACKS switch in %s differs:" % os.path.basename(DEFINES))
            for line in difflib.unified_diff(defines_old.split("\n"), defines_new.split("\n"),
                                             "current", "generated", lineterm="", n=1):
                print("      " + line)
        print("everything matches" if ok else "there are differences")
        return 0 if ok else 1

    # CRLF like the rest of the sketch sources, and no newline after #endif as before
    with open(args.output, "w", newline="\r\n") as f:
        f.write(text)
    print("wrote %s (%s)" % (os.path.relpath(args.output, ROOT), summary))
    with open(DEFINES, "w", newline="\r\n") as f:
        f.write(defines_new)
    print("wrote the LEVELPACKS switch into %s" % os.path.relpath(DEFINES, ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())

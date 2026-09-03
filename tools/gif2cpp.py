#!/usr/bin/env python3
"""
gif2cpp.py - Convert animated GIF images into scaffolded C++ source code.

The target hardware is a fixed 21x17 pixel circular display made of WS2812
LEDs wired in series (row by row, with the corners of the 21x17 rectangle
missing so the visible area approximates a circle). Only 289 of the 357
positions in the 21x17 rectangle physically exist.

Processing is done in two passes:
  Pass 1: read every frame of every input GIF and build a single RGB
          palette shared across all of the provided images.
  Pass 2: re-read the frames and, for every frame, emit the palette index
          of each *existing* pixel only (non-existent pixels, per the
          hardcoded circular mask, are skipped).

The resulting C++ file contains:
  - the shared RGB palette,
  - the 21x17 boolean existence mask (with a compile-time check that it
    contains exactly 289 existing pixels),
  - one palette-index array per animation frame,
  - one frame-pointer array per image (so the frame count of each image is
    trivially available as `IMAGES[i].frameCount`),
  - a top-level `IMAGES[NUM_IMAGES]` array and `NUM_IMAGES` constant.

Usage:
    python gif2cpp.py [-o OUTPUT] FILE.gif [FILE2.gif ...]
"""

import argparse
import re
import sys
from pathlib import Path

from PIL import Image, ImageSequence

# ---------------------------------------------------------------------------
# Hardcoded hardware layout: 21x17 rectangle, circular WS2812 cutout.
# ---------------------------------------------------------------------------

SCREEN_WIDTH = 21
SCREEN_HEIGHT = 17
EXPECTED_PIXEL_COUNT = 289

# Number of existing pixels per row, centered within SCREEN_WIDTH. This
# describes the physical WS2812 wiring: corner pixels of the 21x17
# rectangle are missing so the lit area approximates a circle.
ROW_WIDTHS = [9, 13, 15, 17, 19, 19, 21, 21, 21, 21, 21, 19, 19, 17, 15, 13, 9]


def build_pixel_mask():
    """Build the 21x17 boolean existence mask from ROW_WIDTHS."""
    if len(ROW_WIDTHS) != SCREEN_HEIGHT:
        raise RuntimeError(
            f"ROW_WIDTHS has {len(ROW_WIDTHS)} rows, expected {SCREEN_HEIGHT}"
        )

    mask = []
    for w in ROW_WIDTHS:
        if w > SCREEN_WIDTH or (SCREEN_WIDTH - w) % 2 != 0:
            raise RuntimeError(f"Row width {w} cannot be centered in {SCREEN_WIDTH}")
        pad = (SCREEN_WIDTH - w) // 2
        row = [False] * SCREEN_WIDTH
        for x in range(pad, pad + w):
            row[x] = True
        mask.append(row)

    total = sum(sum(row) for row in mask)
    if total != EXPECTED_PIXEL_COUNT:
        raise RuntimeError(
            f"Pixel mask has {total} existing pixels, expected {EXPECTED_PIXEL_COUNT}"
        )
    return mask


PIXEL_MASK = build_pixel_mask()


# ---------------------------------------------------------------------------
# GIF loading
# ---------------------------------------------------------------------------


def get_pixel_data(im):
    """Flat list of (r, g, b) tuples in row-major order, using the newer
    Pillow API when available (getdata() is deprecated as of Pillow 12)."""
    if hasattr(im, "get_flattened_data"):
        return list(im.get_flattened_data())
    return list(im.getdata())


def load_frames(path):
    """Return a list of frames for one GIF, each frame a flat list of
    (r, g, b) tuples in row-major order (length SCREEN_WIDTH * SCREEN_HEIGHT).
    Transparency is ignored: each pixel's plain RGB value is used as-is.
    """
    im = Image.open(path)
    frames = []
    for frame in ImageSequence.Iterator(im):
        rgb_frame = frame.convert("RGB")
        if rgb_frame.size != (SCREEN_WIDTH, SCREEN_HEIGHT):
            raise ValueError(
                f"{path}: frame {len(frames)} size {rgb_frame.size} != "
                f"expected {(SCREEN_WIDTH, SCREEN_HEIGHT)}"
            )
        frames.append(get_pixel_data(rgb_frame))

    if not frames:
        raise ValueError(f"{path}: no frames found")
    return frames


# ---------------------------------------------------------------------------
# C++ identifier / literal helpers
# ---------------------------------------------------------------------------


def sanitize_identifier(name, used_names):
    """Turn a GIF filename into a valid, unique C++ identifier."""
    ident = re.sub(r"\W|^(?=\d)", "_", name)
    if not ident:
        ident = "image"
    base = ident
    suffix = 0
    while ident in used_names:
        suffix += 1
        ident = f"{base}_{suffix}"
    used_names.add(ident)
    return ident


def index_c_type(palette_size):
    """Smallest unsigned integer type able to index the palette."""
    if palette_size <= 0xFF + 1:
        return "uint8_t"
    if palette_size <= 0xFFFF + 1:
        return "uint16_t"
    return "uint32_t"


# ---------------------------------------------------------------------------
# C++ code generation
# ---------------------------------------------------------------------------


def format_palette_literal(palette_list):
    lines = []
    for i, (r, g, b) in enumerate(palette_list):
        lines.append(f"    {{ {r:3d}, {g:3d}, {b:3d} }}, // {i}")
    return "\n".join(lines)


def format_frame_literal(indices, index_type):
    values = ", ".join(str(v) for v in indices)
    return f"    {{ {values} }},"


def generate_cpp(gif_paths, palette_list, images_indices):
    index_type = index_c_type(len(palette_list))
    used_names = set()
    image_idents = [sanitize_identifier(p.stem, used_names) for p in gif_paths]

    parts = []
    parts.append("// Auto-generated by gif2cpp.py -- do not edit by hand.")
    parts.append("#pragma once")
    parts.append("")
    parts.append("#include <cstdint>")
    parts.append("")
    parts.append(f"constexpr int SCREEN_WIDTH = {SCREEN_WIDTH};")
    parts.append(f"constexpr int SCREEN_HEIGHT = {SCREEN_HEIGHT};")
    parts.append(f"constexpr int NUM_PIXELS = {EXPECTED_PIXEL_COUNT}; // existing pixels only")
    parts.append("")

    # --- Shared RGB palette ---
    parts.append("struct RGBColor {")
    parts.append("    uint8_t r;")
    parts.append("    uint8_t g;")
    parts.append("    uint8_t b;")
    parts.append("};")
    parts.append("")
    parts.append(f"constexpr int PALETTE_SIZE = {len(palette_list)};")
    parts.append(f"constexpr RGBColor PALETTE[PALETTE_SIZE] = {{")
    parts.append(format_palette_literal(palette_list))
    parts.append("};")
    parts.append("")

    # --- Per-image frame arrays ---
    parts.append(f"using PixelIndex = {index_type};")
    parts.append("")

    for ident, path, frames in zip(image_idents, gif_paths, images_indices):
        parts.append(f"// {path.name}: {len(frames)} frame(s)")
        parts.append(
            f"constexpr PixelIndex {ident}_frames[{len(frames)}][NUM_PIXELS] = {{"
        )
        for frame in frames:
            parts.append(format_frame_literal(frame, index_type))
        parts.append("};")
        parts.append("")

    # --- Top-level image table ---
    parts.append("struct Image {")
    parts.append("    const PixelIndex (*frames)[NUM_PIXELS]; // pointer to first frame")
    parts.append("    uint16_t frameCount;")
    parts.append("};")
    parts.append("")
    parts.append(f"constexpr int NUM_IMAGES = {len(image_idents)};")
    parts.append(f"constexpr Image IMAGES[NUM_IMAGES] = {{")
    for ident, frames in zip(image_idents, images_indices):
        parts.append(f"    {{ {ident}_frames, {len(frames)} }}, // {ident}")
    parts.append("};")
    parts.append("")

    return "\n".join(parts) + "\n"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def parse_args(argv=None):
    parser = argparse.ArgumentParser(
        prog="gif2cpp.py",
        description=(
            "Convert animated GIF files (each exactly "
            f"{SCREEN_WIDTH}x{SCREEN_HEIGHT} pixels) into a scaffolded C++ "
            "header with a shared palette and palette-indexed animation "
            "frames for a 289-pixel circular WS2812 display."
        ),
    )
    parser.add_argument(
        "gifs",
        metavar="GIF",
        nargs="+",
        help=f"Input GIF file(s), each exactly {SCREEN_WIDTH}x{SCREEN_HEIGHT} pixels",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="images.h",
        help="Output C++ file name (default: images.h)",
    )
    return parser.parse_args(argv)


def main(argv=None):
    args = parse_args(argv)
    gif_paths = [Path(p) for p in args.gifs]

    for p in gif_paths:
        if not p.is_file():
            print(f"error: file not found: {p}", file=sys.stderr)
            return 1

    # --- Pass 1: load all frames and build the shared palette ---
    images_frames = []
    palette = {}  # rgb tuple -> index (insertion order)
    try:
        for path in gif_paths:
            frames = load_frames(path)
            images_frames.append(frames)
            for frame in frames:
                for rgb in frame:
                    if rgb not in palette:
                        palette[rgb] = len(palette)
    except (ValueError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    palette_list = list(palette.keys())

    # --- Pass 2: convert each frame to a masked, palette-indexed array ---
    images_indices = []
    for frames in images_frames:
        frame_index_lists = []
        for frame in frames:
            indices = []
            pos = 0
            for y in range(SCREEN_HEIGHT):
                for x in range(SCREEN_WIDTH):
                    rgb = frame[pos]
                    pos += 1
                    if PIXEL_MASK[y][x]:
                        indices.append(palette[rgb])
            assert len(indices) == EXPECTED_PIXEL_COUNT
            frame_index_lists.append(indices)
        images_indices.append(frame_index_lists)

    cpp_code = generate_cpp(gif_paths, palette_list, images_indices)

    out_path = Path(args.output)
    out_path.write_text(cpp_code)

    total_frames = sum(len(f) for f in images_indices)
    print(
        f"Wrote {out_path} ({len(gif_paths)} image(s), {total_frames} frame(s), "
        f"{len(palette_list)} palette color(s))"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())

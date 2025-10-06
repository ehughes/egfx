#!/usr/bin/env python3
"""
eGFX TrueType Font to Font Converter (Command Line)

Converts TrueType fonts (.ttf) to eGFX font format.
Creates font configuration folders compatible with existing font generation pipeline.

Usage:
    python egfx_ttf_to_font.py <input.ttf> [options]

Examples:
    # Single size, 1bpp
    python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp

    # Multiple sizes
    python egfx_ttf_to_font.py fragment_mono.ttf --size 10 12 14 16 --bpp 1bpp

    # Size range
    python egfx_ttf_to_font.py fragment_mono.ttf --size-range 10 20 --bpp 1bpp

    # Multiple BPP options
    python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp 4bpp

    # Custom output directory
    python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp --output ./fonts

    # Generate C code immediately
    python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp --generate
"""

import os
import sys
import json
import argparse
from pathlib import Path
from typing import List

# Import TrueType renderer from GUI tool
from egfx_font_generator_gui import TTFontRenderer
from egfx_font_tool import CHAR_MAP, generate_font

# Reverse CHAR_MAP for display
ASCII_TO_NAME = {v: k for k, v in CHAR_MAP.items()}


def create_font_folder(ttf_path: str,
                      font_size: int,
                      bpp: str,
                      output_base: str,
                      generate_code: bool = False) -> str:
    """
    Create a font configuration folder from a TrueType font

    Args:
        ttf_path: Path to .ttf file
        font_size: Font size in pixels
        bpp: Bits per pixel ("1bpp", "4bpp", or "8bpp")
        output_base: Base output directory
        generate_code: If True, also generate C code

    Returns:
        Path to created font folder
    """

    # Create font name from TTF filename
    font_basename = os.path.splitext(os.path.basename(ttf_path))[0]
    font_basename = font_basename.replace(" ", "_").replace("-", "_")
    font_name = f"font_{font_basename}_{font_size}px_{bpp}"

    print(f"\n{'='*60}")
    print(f"Creating font: {font_name}")
    print(f"{'='*60}")

    # Create output directory
    font_output_dir = os.path.join(output_base, font_name)
    os.makedirs(font_output_dir, exist_ok=True)

    # Create src_images directory
    src_images_dir = os.path.join(font_output_dir, "src_images")
    os.makedirs(src_images_dir, exist_ok=True)

    print(f"Output directory: {font_output_dir}")

    # Extract BPP number
    bpp_num = int(bpp.replace("bpp", ""))

    # Create renderer
    print(f"Initializing TrueType renderer (size={font_size}px, bpp={bpp})...")
    renderer = TTFontRenderer(ttf_path, font_size, bpp_num)

    # Get uniform vertical bounds across all characters
    print("Calculating uniform vertical bounds...")
    vertical_bounds = renderer.get_vertical_bounds_all_chars()
    v_start, v_end = vertical_bounds
    print(f"  Vertical bounds: {v_start} to {v_end} (height: {v_end - v_start + 1}px)")

    # Render and save all ASCII characters
    print(f"Rendering {128 - 32} ASCII characters...")
    for ascii_code in range(32, 128):
        char = chr(ascii_code)
        char_name = ASCII_TO_NAME.get(ascii_code, f"char_{ascii_code}")

        # Render character
        img = renderer.render_character(char, oversample=(bpp_num > 1))

        # Trim using uniform vertical bounds
        trimmed = renderer.trim_character(img, vertical_bounds)

        # Quantize to BPP
        quantized = renderer.quantize_to_bpp(trimmed)

        # Save as PNG
        output_path = os.path.join(src_images_dir, f"{char_name}.png")
        quantized.save(output_path)

        # Progress indicator
        if (ascii_code - 32 + 1) % 20 == 0:
            print(f"  Progress: {ascii_code - 32 + 1}/96 characters")

    print(f"  All 96 characters saved to {src_images_dir}")

    # Get font metrics
    ascent, descent, line_spacing = renderer.get_font_metrics()

    print(f"Font metrics:")
    print(f"  Ascent: {ascent // 256}px (Q8.8: {ascent})")
    print(f"  Descent: {descent // 256}px (Q8.8: {descent})")
    print(f"  Line spacing: {line_spacing // 256}px (Q8.8: {line_spacing})")

    # Extract kerning pairs
    kerning_pairs = renderer.extract_kerning_pairs()
    if kerning_pairs:
        print(f"Kerning pairs: {len(kerning_pairs)}")

    # For monospace fonts, use 0 inter-character spacing (spacing is built into char width)
    inter_char_spacing = 0 if renderer.is_monospace else 1

    # Create font_config.json (convert numpy types to native Python types for JSON)
    config = {
        "combined_output_name": font_name,
        "bpp": [bpp],
        "ascent": int(ascent),
        "descent": int(descent),
        "line_spacing": int(line_spacing),
        "inter_character_spacing": inter_char_spacing,
        "spaces_per_tab": 4,
        "glyph_overrides": {},
        "kerning_pairs": {f"{k[0]}_{k[1]}": v for k, v in kerning_pairs.items()}
    }

    config_path = os.path.join(font_output_dir, "font_config.json")
    with open(config_path, 'w') as f:
        json.dump(config, f, indent=2)

    print(f"Font configuration saved to {config_path}")

    # Generate eGFX C code if requested
    if generate_code:
        print(f"\nGenerating eGFX C code...")
        generate_font(font_output_dir, font_output_dir)
        print(f"C code generated:")
        print(f"  - {font_name}.c")
        print(f"  - {font_name}.h")

    print(f"\n{'='*60}")
    print(f"Font creation complete: {font_name}")
    print(f"{'='*60}\n")

    return font_output_dir


def main():
    parser = argparse.ArgumentParser(
        description="Convert TrueType fonts to eGFX font format",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Single size, 1bpp
  python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp

  # Multiple sizes
  python egfx_ttf_to_font.py fragment_mono.ttf --size 10 12 14 16 --bpp 1bpp

  # Size range (10 to 20 inclusive)
  python egfx_ttf_to_font.py fragment_mono.ttf --size-range 10 20 --bpp 1bpp

  # Multiple BPP options
  python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp 4bpp

  # Custom output directory
  python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp --output ./fonts

  # Generate C code immediately
  python egfx_ttf_to_font.py fragment_mono.ttf --size 14 --bpp 1bpp --generate
        """
    )

    parser.add_argument("input_ttf",
                       help="Path to input TrueType font (.ttf or .otf)")

    size_group = parser.add_mutually_exclusive_group(required=True)
    size_group.add_argument("--size", "-s",
                           type=int,
                           nargs='+',
                           help="Font size(s) in pixels (e.g., 14 or 10 12 14 16)")
    size_group.add_argument("--size-range",
                           type=int,
                           nargs=2,
                           metavar=('START', 'END'),
                           help="Font size range in pixels (e.g., 10 20)")

    parser.add_argument("--bpp", "-b",
                       nargs='+',
                       choices=['1bpp', '4bpp', '8bpp'],
                       default=['1bpp'],
                       help="Bits per pixel (default: 1bpp)")

    parser.add_argument("--output", "-o",
                       default="./generated_fonts",
                       help="Output directory (default: ./generated_fonts)")

    parser.add_argument("--generate", "-g",
                       action='store_true',
                       help="Generate C code immediately (default: only create PNGs and config)")

    args = parser.parse_args()

    # Validate input file
    if not os.path.exists(args.input_ttf):
        print(f"Error: Input file not found: {args.input_ttf}")
        return 1

    # Get list of sizes
    if args.size:
        sizes = args.size
    else:
        start, end = args.size_range
        if start > end:
            start, end = end, start
        sizes = list(range(start, end + 1))

    # Get list of BPP options
    bpp_options = args.bpp

    print(f"\nTrueType Font Converter")
    print(f"Input: {args.input_ttf}")
    print(f"Sizes: {sizes}")
    print(f"BPP: {bpp_options}")
    print(f"Output: {args.output}")
    print(f"Generate C code: {'Yes' if args.generate else 'No'}")

    # Create output base directory
    os.makedirs(args.output, exist_ok=True)

    # Generate all combinations
    total_fonts = len(sizes) * len(bpp_options)
    print(f"\nGenerating {total_fonts} font configuration(s)...")

    created_fonts = []

    for size in sizes:
        for bpp in bpp_options:
            try:
                font_dir = create_font_folder(
                    args.input_ttf,
                    size,
                    bpp,
                    args.output,
                    args.generate
                )
                created_fonts.append(font_dir)
            except Exception as e:
                print(f"\nError creating font (size={size}, bpp={bpp}): {e}")
                import traceback
                traceback.print_exc()
                continue

    # Summary
    print(f"\n{'='*60}")
    print(f"SUMMARY")
    print(f"{'='*60}")
    print(f"Successfully created {len(created_fonts)} font(s):")
    for font_dir in created_fonts:
        print(f"  - {os.path.basename(font_dir)}")

    if not args.generate:
        print(f"\nTo generate C code later, run:")
        for font_dir in created_fonts:
            font_name = os.path.basename(font_dir)
            print(f"  python egfx_font_tool.py {font_dir} {font_dir}")

    print(f"\nAll fonts saved to: {args.output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())

#!/usr/bin/env python3
"""
eGFX Font Tool
Converts PNG font images to eGFX font C code

This tool reads a directory containing:
- font_config.json (JSON config with font metadata)
- src_images/ folder with PNG images for each character

Legacy formats are also supported:
- FontConfiguration.jsx (legacy config format)
- SrcImages/ folder (legacy image directory)

Usage:
    python egfx_font_tool.py <font_dir> <output_folder>

Example:
    python egfx_font_tool.py ./assets/FONT_5_7_1BPP ./src

The tool generates:
- <fontname>.c   - C source file with font data
- <fontname>.h   - Header file with font declaration
"""

import os
import sys
import json
from pathlib import Path
from typing import Dict, Tuple

# Import shared code from sprite tool
from egfx_sprite_tool import (
    ImagePlaneType, ImagePlane, remap_pixel_color, sanitize_name
)
from PIL import Image


# Character filename to ASCII mapping
# Maps image filenames to their ASCII codes (32-127)
CHAR_MAP = {
    'space': 32,
    'exclamation_point': 33,
    'double_quotes': 34,
    'pound': 35,
    'dollar_sign': 36,
    'precent_sign': 37,
    'ampersand': 38,
    'single_quote': 39,
    'left_parenthesis': 40,
    'right_parenthesis': 41,
    'star': 42,
    'plus': 43,
    'comma': 44,
    'dash': 45,
    'period': 46,
    'forward_slash': 47,
    '0': 48,
    '1': 49,
    '2': 50,
    '3': 51,
    '4': 52,
    '5': 53,
    '6': 54,
    '7': 55,
    '8': 56,
    '9': 57,
    'colon': 58,
    'semicolon': 59,
    'less_than': 60,
    'equals': 61,
    'great_than': 62,
    'question_mark': 63,
    'at': 64,
    'uc_A': 65,
    'uc_B': 66,
    'uc_C': 67,
    'uc_D': 68,
    'uc_E': 69,
    'uc_F': 70,
    'uc_G': 71,
    'uc_H': 72,
    'uc_I': 73,
    'uc_J': 74,
    'uc_K': 75,
    'uc_L': 76,
    'uc_M': 77,
    'uc_N': 78,
    'uc_O': 79,
    'uc_P': 80,
    'uc_Q': 81,
    'uc_R': 82,
    'uc_S': 83,
    'uc_T': 84,
    'uc_U': 85,
    'uc_V': 86,
    'uc_W': 87,
    'uc_X': 88,
    'uc_Y': 89,
    'uc_Z': 90,
    'left_bracket': 91,
    'backslash': 92,
    'right_bracket': 93,
    'carret': 94,
    'underscore': 95,
    'slanted_single_quote': 96,
    'lc_a': 97,
    'lc_b': 98,
    'lc_c': 99,
    'lc_d': 100,
    'lc_e': 101,
    'lc_f': 102,
    'lc_g': 103,
    'lc_h': 104,
    'lc_i': 105,
    'lc_j': 106,
    'lc_k': 107,
    'lc_l': 108,
    'lc_m': 109,
    'lc_n': 110,
    'lc_o': 111,
    'lc_p': 112,
    'lc_q': 113,
    'lc_r': 114,
    'lc_s': 115,
    'lc_t': 116,
    'lc_u': 117,
    'lc_v': 118,
    'lc_w': 119,
    'lc_x': 120,
    'lc_y': 121,
    'lc_z': 122,
    'left_curly_brace': 123,
    'vertical_bar': 124,
    'right_curly_brace': 125,
    'tilde': 126,
    'delete': 127
}


def load_font_config(font_dir: str) -> Dict:
    """Load font configuration from font_config.json or FontConfiguration.jsx (legacy)"""

    # Try new format first (font_config.json)
    config_path = os.path.join(font_dir, "font_config.json")

    if os.path.exists(config_path):
        with open(config_path, 'r') as f:
            config = json.load(f)
            # Config is already in snake_case format
            return config

    # Fallback to legacy format (FontConfiguration.jsx)
    config_path = os.path.join(font_dir, "FontConfiguration.jsx")

    if not os.path.exists(config_path):
        raise FileNotFoundError(f"Font configuration not found: font_config.json or FontConfiguration.jsx")

    with open(config_path, 'r') as f:
        # Skip comment lines and parse JSON
        content = f.read()
        # Find JSON object between braces
        start = content.find('{')
        end = content.rfind('}') + 1
        json_str = content[start:end]
        config = json.loads(json_str)

        # Convert PascalCase to snake_case for legacy configs
        snake_case_config = {
            'ascent': config.get('Ascent', 0),
            'descent': config.get('Descent', 0),
            'line_spacing': config.get('LineSpacing', 0),
            'spaces_per_tab': config.get('SpacesPerTab', 4),
            'inter_character_spacing': config.get('InterCharacterSpacing', 1),
            'combined_output_name': config.get('CombinedOutputName', 'FONT'),
            'glyph_overrides': {}
        }
        return snake_case_config


def translate_character(image_path: str,
                        char_name: str,
                        font_name: str,
                        plane_type: ImagePlaneType,
                        section_macro: str) -> Tuple[str, str, str]:
    """
    Translate a character PNG image to eGFX format.

    Returns:
        Tuple of (c_code, struct_name, sprite_name)
    """

    if not os.path.exists(image_path):
        raise FileNotFoundError(f"Could not open {image_path}")

    # Load image
    img = Image.open(image_path).convert('RGB')
    width, height = img.size

    print(f"  {char_name}: {width} x {height} pixels")

    # Create image plane (fonts are always 1BPP)
    plane = ImagePlane(width, height, plane_type)

    # Convert pixels
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            ps = remap_pixel_color(plane_type, r, g, b)
            plane.put_pixel(x, y, ps)

    # Generate names - sanitize char_name for C identifiers (replace + with _)
    sanitized_char_name = char_name.replace('+', '_')
    sprite_struct_name = f"{font_name}_font_element_{sanitized_char_name}"
    raw_data_array_name = f"{sprite_struct_name}_Data"

    # Generate C code
    code_output = []
    code_output.append("\n")
    code_output.append("////***************************************************************************\n")
    code_output.append(f"////                            {sprite_struct_name}\n")
    code_output.append("////***************************************************************************\n")

    # Data array
    code_output.append(f"\n{section_macro} uint8_t {raw_data_array_name}[{len(plane.data)}] = {{ ")

    for i, byte in enumerate(plane.data):
        if i == 0:
            code_output.append("\n")
        code_output.append(f"0x{byte:02X}")
        if i == len(plane.data) - 1:
            code_output.append(" }; \n")
        else:
            code_output.append(",")

    # Struct definition
    bpp = (int(plane_type) >> 8) & 0xFF
    subtype = int(plane_type) & 0xFF

    code_output.append(f"\n\n{section_macro} egfx_img {sprite_struct_name} =\n")
    code_output.append("{\n")
    code_output.append(f".color_type = {subtype},\n")
    code_output.append(f".bits_per_pixel = {bpp},\n")
    code_output.append(".flags = 0,\n")
    code_output.append(".rsv = 0,\n")
    code_output.append(f".data = (uint8_t *){raw_data_array_name},\n")
    code_output.append(f".size_x = {width},\n")
    code_output.append(f".size_y = {height},\n")
    code_output.append(".user = NULL\n")
    code_output.append("};\n")

    return ''.join(code_output), sprite_struct_name, char_name


def generate_font(font_dir: str, output_folder: str):
    """
    Generate font files from a font directory.

    Args:
        font_dir: Directory containing FontConfiguration.jsx and SrcImages/
        output_folder: Directory where generated .c and .h files will be written
    """

    if not os.path.isdir(font_dir):
        print(f"Error: {font_dir} does not exist")
        return 1

    print(f"Processing font in {font_dir}")

    # Load configuration
    config = load_font_config(font_dir)
    font_name = config.get("combined_output_name", "FONT")
    ascent = config.get("ascent", 0)
    descent = config.get("descent", 0)
    line_spacing = config.get("line_spacing", 0)
    spaces_per_tab = config.get("spaces_per_tab", 4)
    inter_char_spacing = config.get("inter_character_spacing", 1)
    glyph_overrides = config.get("glyph_overrides", {})
    bpp_list = config.get("bpp", ["1bpp"])  # Default to 1bpp if not specified
    kerning_pairs_raw = config.get("kerning_pairs", {})  # Glyph name pairs -> kerning value

    print(f"Font name: {font_name}")
    print(f"Metadata: ascent={ascent}, descent={descent}, line_spacing={line_spacing}")
    print(f"          spaces_per_tab={spaces_per_tab}, inter_char_spacing={inter_char_spacing}")
    print(f"          bpp={bpp_list}")

    # Find source images (try new format first, then legacy)
    src_images_dir = os.path.join(font_dir, "src_images")
    if not os.path.isdir(src_images_dir):
        # Fallback to legacy SrcImages
        src_images_dir = os.path.join(font_dir, "SrcImages")
        if not os.path.isdir(src_images_dir):
            print(f"Error: src_images or SrcImages directory not found in {font_dir}")
            return 1

    # Build character map from available images
    characters = {}  # ASCII code -> (image_path, char_name)
    utf8_characters = {}  # UTF-8 codepoint -> (image_path, char_name)

    for filename in os.listdir(src_images_dir):
        if not filename.lower().endswith('.png'):
            continue

        # Get character name without extension
        char_name = os.path.splitext(filename)[0]

        # Check if it's a UTF-8 codepoint (U+XXXX or U+XXXX_name format)
        if char_name.startswith('U+'):
            try:
                # Handle both U+XXXX and U+XXXX_name formats
                parts = char_name.split('_', 1)
                codepoint_str = parts[0][2:]  # Remove 'U+' prefix
                friendly_name = parts[1] if len(parts) > 1 else None

                codepoint = int(codepoint_str, 16)  # Parse hex codepoint
                image_path = os.path.join(src_images_dir, filename)
                utf8_characters[codepoint] = (image_path, char_name, friendly_name)

                if friendly_name:
                    print(f"Found UTF-8 character: {char_name} (U+{codepoint:04X} - {friendly_name})")
                else:
                    print(f"Found UTF-8 character: {char_name} (U+{codepoint:04X})")
            except ValueError:
                print(f"Warning: Invalid UTF-8 codepoint format: {char_name}")
        # Map to ASCII code
        elif char_name in CHAR_MAP:
            ascii_code = CHAR_MAP[char_name]
            image_path = os.path.join(src_images_dir, filename)
            characters[ascii_code] = (image_path, char_name)

    # Process glyph overrides from config
    if glyph_overrides:
        print(f"Processing {len(glyph_overrides)} glyph override(s) from config...")
        for codepoint_str, png_filename in glyph_overrides.items():
            try:
                # Parse codepoint (support both "0x03BB" and "03BB" formats)
                if codepoint_str.startswith('0x') or codepoint_str.startswith('0X'):
                    codepoint = int(codepoint_str, 16)
                else:
                    codepoint = int(codepoint_str, 16)

                # Build full path to PNG
                image_path = os.path.join(src_images_dir, png_filename)

                if not os.path.exists(image_path):
                    print(f"  Warning: Override file not found: {png_filename}")
                    continue

                # Extract friendly name from filename if available
                char_name = os.path.splitext(png_filename)[0]
                friendly_name = None
                if '_' in char_name:
                    parts = char_name.split('_', 1)
                    friendly_name = parts[1] if len(parts) > 1 else None

                # Add or override in utf8_characters
                utf8_characters[codepoint] = (image_path, char_name, friendly_name)
                print(f"  Override: U+{codepoint:04X} -> {png_filename}")

            except ValueError as e:
                print(f"  Warning: Invalid codepoint in glyph_overrides: {codepoint_str} ({e})")

    # Verify we have all ASCII characters 32-127
    missing_chars = []
    for ascii_code in range(32, 128):
        if ascii_code not in characters:
            missing_chars.append(ascii_code)

    if missing_chars:
        print(f"Warning: Missing characters for ASCII codes: {missing_chars}")

    print(f"Found {len(characters)} ASCII character images")
    print(f"Found {len(utf8_characters)} UTF-8 character images")

    # Generate code for each character in ASCII order
    code_output = []
    struct_names = []

    # Map BPP from config to ImagePlaneType
    bpp_str = bpp_list[0] if bpp_list else "1bpp"
    if bpp_str == "1bpp":
        plane_type = ImagePlaneType.eGFX_IMAGE_PLANE_1BPP
    elif bpp_str == "4bpp":
        plane_type = ImagePlaneType.eGFX_IMAGE_PLANE_4BPP
    elif bpp_str == "8bpp":
        plane_type = ImagePlaneType.eGFX_IMAGE_PLANE_8BPP
    else:
        print(f"Warning: Unknown BPP '{bpp_str}', defaulting to 1BPP")
        plane_type = ImagePlaneType.eGFX_IMAGE_PLANE_1BPP

    section_macro = "FONT_SECTION"

    for ascii_code in range(32, 128):
        if ascii_code in characters:
            image_path, char_name = characters[ascii_code]
            code, struct_name, _ = translate_character(
                image_path, char_name, font_name, plane_type, section_macro
            )
            code_output.append(code)
            struct_names.append(struct_name)
        else:
            # Generate placeholder for missing character
            print(f"  Warning: Missing character for ASCII {ascii_code}")
            struct_names.append("NULL")

    # Generate code for UTF-8 characters (sorted by codepoint)
    utf8_struct_names = []
    for codepoint in sorted(utf8_characters.keys()):
        image_path, char_name, friendly_name = utf8_characters[codepoint]
        code, struct_name, _ = translate_character(
            image_path, char_name, font_name, plane_type, section_macro
        )
        code_output.append(code)
        struct_names.append(struct_name)
        utf8_struct_names.append((codepoint, struct_name, friendly_name))

    # Calculate default advance (use width of first available character, or 0)
    default_advance = 0
    for ascii_code in range(32, 128):
        if ascii_code in characters:
            image_path, _ = characters[ascii_code]
            img = Image.open(image_path)
            default_advance = img.size[0]  # Use width as advance
            break

    # Generate glyph pointer arrays for each BPP
    glyph_array_code = []
    glyph_array_names = {}  # bpp -> array_name mapping

    for bpp in bpp_list:
        array_name = f"{font_name}_glyphs_{bpp}"
        glyph_array_names[bpp] = array_name

        glyph_array_code.append(f"\n{section_macro} egfx_img* {array_name}[{len(struct_names)}] = {{\n")

        for i, struct_name in enumerate(struct_names):
            if struct_name != "NULL":
                glyph_array_code.append(f"\t(const egfx_img *)&{struct_name}")
            else:
                glyph_array_code.append(f"\t{struct_name}")

            if i < len(struct_names) - 1:
                glyph_array_code.append(",\n")
            else:
                glyph_array_code.append("\n")

        glyph_array_code.append("};\n")

    # Generate glyph_map for UTF-8 characters if present
    glyph_map_code = []
    if utf8_struct_names:
        glyph_map_code.append(f"\n{section_macro} egfx_glyph_map_entry {font_name}_glyph_map[{len(utf8_struct_names)}] = {{\n")

        # UTF-8 glyphs start after the 96 ASCII glyphs (index 96)
        base_index = 96
        for i, (codepoint, struct_name, friendly_name) in enumerate(utf8_struct_names):
            glyph_index = base_index + i
            is_last = (i == len(utf8_struct_names) - 1)

            # Add friendly name as comment if available
            if friendly_name:
                if is_last:
                    glyph_map_code.append(f"\t{{ .codepoint = 0x{codepoint:04X}, .glyph_index = {glyph_index} }}  // {friendly_name}\n")
                else:
                    glyph_map_code.append(f"\t{{ .codepoint = 0x{codepoint:04X}, .glyph_index = {glyph_index} }},  // {friendly_name}\n")
            else:
                if is_last:
                    glyph_map_code.append(f"\t{{ .codepoint = 0x{codepoint:04X}, .glyph_index = {glyph_index} }}\n")
                else:
                    glyph_map_code.append(f"\t{{ .codepoint = 0x{codepoint:04X}, .glyph_index = {glyph_index} }},\n")

        glyph_map_code.append("};\n")

    # Process kerning pairs - convert glyph names to indices
    kerning_table_code = []
    kerning_pairs = []  # List of (left_index, right_index, x_adjust) tuples

    if kerning_pairs_raw:
        # Create a mapping from glyph name to index
        # For ASCII: 'A' (65) -> index 33 (65 - 32)
        # For UTF-8: handled via glyph_map
        glyph_name_to_index = {}

        # Build reverse mapping from glyph names to indices
        for ascii_code in range(32, 128):
            if ascii_code in characters:
                _, char_name = characters[ascii_code]
                glyph_index = ascii_code - 32  # ASCII glyphs start at index 0
                glyph_name_to_index[char_name] = glyph_index

                # Also map by ASCII character for direct lookups
                char = chr(ascii_code)
                glyph_name_to_index[char] = glyph_index

        # Add UTF-8 characters to mapping (they start after ASCII)
        base_utf8_index = 96  # After 96 ASCII characters
        for i, (codepoint, struct_name, friendly_name) in enumerate(utf8_struct_names):
            glyph_index = base_utf8_index + i
            if friendly_name:
                glyph_name_to_index[friendly_name] = glyph_index
            # Also try to map by the actual character
            try:
                char = chr(codepoint)
                glyph_name_to_index[char] = glyph_index
            except:
                pass

        # Convert kerning pairs from glyph names to indices
        for pair_key, kerning_value in kerning_pairs_raw.items():
            # pair_key is like "A_V" or "F_period"
            parts = pair_key.split('_', 1)
            if len(parts) == 2:
                left_glyph_name, right_glyph_name = parts

                # Try to find indices for both glyphs
                left_index = glyph_name_to_index.get(left_glyph_name)
                right_index = glyph_name_to_index.get(right_glyph_name)

                if left_index is not None and right_index is not None:
                    # Scale kerning value from font units to pixels
                    # TTF fonts use "units per em" (typically 1000-2048)
                    # We need to scale based on the font size
                    # For now, use a simple scaling factor (will refine later)
                    # Typical scaling: (kerning_value * font_size) / units_per_em
                    # Using a rough approximation for now
                    x_adjust = int(kerning_value // 64)  # Simple downscaling

                    # Clamp to int8_t range (-128 to 127)
                    x_adjust = max(-128, min(127, x_adjust))

                    if x_adjust != 0:  # Only include non-zero adjustments
                        kerning_pairs.append((left_index, right_index, x_adjust))

        if kerning_pairs:
            print(f"Processed {len(kerning_pairs)} kerning pairs")

            # Generate C code for kerning table
            kerning_table_code.append(f"\n{section_macro} egfx_kerning_pair {font_name}_kerning[{len(kerning_pairs)}] = {{\n")

            for i, (left_idx, right_idx, x_adj) in enumerate(kerning_pairs):
                is_last = (i == len(kerning_pairs) - 1)
                if is_last:
                    kerning_table_code.append(f"\t{{ .left_glyph = {left_idx}, .right_glyph = {right_idx}, .x_adjust = {x_adj}, .padding = 0 }}\n")
                else:
                    kerning_table_code.append(f"\t{{ .left_glyph = {left_idx}, .right_glyph = {right_idx}, .x_adjust = {x_adj}, .padding = 0 }},\n")

            kerning_table_code.append("};\n")

    # Generate main font struct
    font_struct_code = []

    # Determine flags
    flags_parts = ["EGFX_FONT_ASCII_ONLY"]
    if utf8_struct_names:
        flags_parts.append("EGFX_FONT_UTF8")
        glyph_map_ptr = f"{font_name}_glyph_map"
        glyph_map_size = len(utf8_struct_names)
    else:
        glyph_map_ptr = "NULL"
        glyph_map_size = 0

    if kerning_pairs:
        flags_parts.append("EGFX_FONT_KERNING")
        kerning_table_ptr = f"{font_name}_kerning"
        kerning_count = len(kerning_pairs)
    else:
        kerning_table_ptr = "NULL"
        kerning_count = 0

    flags_str = " | ".join(flags_parts)

    # Determine which BPP pointers to set
    glyphs_1bpp_ptr = glyph_array_names.get("1bpp", "NULL") if "1bpp" in glyph_array_names else "NULL"
    glyphs_4bpp_ptr = glyph_array_names.get("4bpp", "NULL") if "4bpp" in glyph_array_names else "NULL"
    glyphs_8bpp_ptr = glyph_array_names.get("8bpp", "NULL") if "8bpp" in glyph_array_names else "NULL"

    font_struct_code.append(f"\n\n{section_macro} egfx_font {font_name} = {{\n")
    font_struct_code.append(f"\t.flags = {flags_str},\n")
    font_struct_code.append(f"\t.glyph_count = {len(struct_names)},\n")
    font_struct_code.append(f"\t.glyphs_1bpp = {glyphs_1bpp_ptr},\n")
    font_struct_code.append(f"\t.glyphs_4bpp = {glyphs_4bpp_ptr},\n")
    font_struct_code.append(f"\t.glyphs_8bpp = {glyphs_8bpp_ptr},\n")
    font_struct_code.append(f"\t.metrics = NULL,  // Fixed width font\n")
    font_struct_code.append(f"\t.glyph_map = {glyph_map_ptr},\n")
    font_struct_code.append(f"\t.glyph_map_size = {glyph_map_size},\n")
    font_struct_code.append(f"\t.kerning_table = {kerning_table_ptr},\n")
    font_struct_code.append(f"\t.kerning_count = {kerning_count},\n")
    font_struct_code.append(f"\t.ascent = {ascent},  // TT Metric: Ascent (Q15.16)\n")
    font_struct_code.append(f"\t.descent = {descent},  // TT Metric: Descent (Q15.16)\n")
    font_struct_code.append(f"\t.line_spacing = {line_spacing},  // TT Metric: Line spacing (Q15.16)\n")
    font_struct_code.append(f"\t.default_advance = {default_advance},  // Character width\n")
    font_struct_code.append(f"\t.inter_character_spacing = {inter_char_spacing},\n")
    font_struct_code.append(f"\t.spaces_per_tab = {spaces_per_tab},\n")
    font_struct_code.append(f"\t.padding = 0\n")
    font_struct_code.append("};\n")

    # Write C file
    output_c = os.path.join(output_folder, f"{font_name.lower()}.c")
    output_h = os.path.join(output_folder, f"{font_name.lower()}.h")

    print(f"\nWriting C source to: {output_c}")
    with open(output_c, 'w') as f:
        f.write('#include "stdint.h" \n')
        f.write('#include "egfx.h" \n')
        f.write(f'#include "{font_name.lower()}.h" \n')
        f.write(''.join(code_output))
        f.write(''.join(glyph_array_code))
        f.write(''.join(glyph_map_code))
        f.write(''.join(kerning_table_code))
        f.write(''.join(font_struct_code))
        f.write("\n")

    # Write header file
    print(f"Writing header to: {output_h}")
    with open(output_h, 'w') as f:
        f.write('#include "egfx.h" \n\n')
        f.write(f"#ifndef _{font_name}_H\n")
        f.write(f"#define _{font_name}_H\n\n")
        f.write("\n#ifndef FONT_SECTION\n")
        f.write("    #define FONT_SECTION const \n")
        f.write("#endif\n\n")
        f.write(f"extern FONT_SECTION egfx_font {font_name};\n\n")
        f.write("#endif\n\n")

    print("\nFont generation complete!\n")
    return 0


def main():
    if len(sys.argv) < 3:
        print("Usage: python egfx_font_tool.py <font_dir> <output_folder>")
        print("\nExample:")
        print("  python egfx_font_tool.py ./assets/FONT_5_7_1BPP ./src")
        return 1

    font_dir = sys.argv[1]
    output_folder = sys.argv[2]

    return generate_font(font_dir, output_folder)


if __name__ == '__main__':
    sys.exit(main())

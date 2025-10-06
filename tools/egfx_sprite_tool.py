#!/usr/bin/env python3
"""
eGFX Sprite Tool
Converts PNG images to eGFX image plane C code

This tool scans a directory for PNG images and converts them to eGFX image plane
format, generating C source and header files containing the image data.

Usage:
    python egfx_sprite_tool.py <sprite_dir> <plane_type> <output_folder> [options]

Examples:
    # Generate sprites with default naming (<image_name>)
    python egfx_sprite_tool.py ./assets eGFX_IMAGE_PLANE_16BPP_RGB565 ./src

    # Generate sprites with custom output filename
    python egfx_sprite_tool.py ./assets eGFX_IMAGE_PLANE_32BPP_XRGB888 ./src -o sprites

    # Generate sprites with plane type prefix in sprite names
    python egfx_sprite_tool.py ./assets eGFX_IMAGE_PLANE_8BPP ./src --append-plane-type

Supported Plane Types:
    1-bit:
        eGFX_IMAGE_PLANE_1BPP                   - 1-bit monochrome

    3-bit:
        eGFX_IMAGE_PLANE_3BPP_BGR               - 3-bit BGR (1 bit per channel)

    4-bit:
        eGFX_IMAGE_PLANE_4BPP                   - 4-bit grayscale
        eGFX_IMAGE_PLANE_4BPP_XBGR              - 4-bit XBGR (1 bit per channel + reserved)

    8-bit:
        eGFX_IMAGE_PLANE_8BPP                   - 8-bit grayscale
        eGFX_IMAGE_PLANE_8BPP_XRGB222           - 8-bit RGB (2 bits red, 2 bits green, 2 bits blue)

    16-bit:
        eGFX_IMAGE_PLANE_16BPP                  - 16-bit generic
        eGFX_IMAGE_PLANE_16BPP_RGB565           - 16-bit RGB (5:6:5)
        eGFX_IMAGE_PLANE_16BPP_BGR565           - 16-bit BGR (5:6:5)
        eGFX_IMAGE_PLANE_16BPP_GBRG3553         - 16-bit GBRG (3:5:5:3)
        eGFX_IMAGE_PLANE_16BPP_GRBG3553         - 16-bit GRBG (3:5:5:3)

    24-bit:
        eGFX_IMAGE_PLANE_24BPP                  - 24-bit generic
        eGFX_IMAGE_PLANE_24BPP_RGB888           - 24-bit RGB (8:8:8)

    32-bit:
        eGFX_IMAGE_PLANE_32BPP                  - 32-bit generic
        eGFX_IMAGE_PLANE_32BPP_XRGB888          - 32-bit XRGB (8:8:8:8)

Output:
    The tool generates two files:
    - <output_name>.c   - C source file containing image data arrays and structures
    - <output_name>.h   - Header file with extern declarations

    By default, sprite names are just the image base name: <image_base_name>
    With --append-plane-type: <plane_type>_<image_base_name>
"""

import os
import sys
from pathlib import Path
from enum import IntEnum
from typing import List, Tuple
from PIL import Image
import argparse


class ImagePlaneType(IntEnum):
    """eGFX Image Plane Types"""
    eGFX_IMAGE_PLANE_1BPP = 0x0100
    eGFX_IMAGE_PLANE_3BPP_BGR = 0x0300
    eGFX_IMAGE_PLANE_4BPP = 0x0400
    eGFX_IMAGE_PLANE_4BPP_XBGR = 0x0401
    eGFX_IMAGE_PLANE_8BPP = 0x0800
    eGFX_IMAGE_PLANE_8BPP_XRGB222 = 0x0801
    eGFX_IMAGE_PLANE_16BPP = 0x1000
    eGFX_IMAGE_PLANE_16BPP_RGB565 = 0x1001
    eGFX_IMAGE_PLANE_16BPP_BGR565 = 0x1002
    eGFX_IMAGE_PLANE_16BPP_GBRG3553 = 0x1003
    eGFX_IMAGE_PLANE_16BPP_GRBG3553 = 0x1004
    eGFX_IMAGE_PLANE_24BPP = 0x1800
    eGFX_IMAGE_PLANE_24BPP_RGB888 = 0x1801
    eGFX_IMAGE_PLANE_32BPP = 0x2000
    eGFX_IMAGE_PLANE_32BPP_XRGB888 = 0x2001


class ImagePlane:
    """
    Represents an eGFX Image Plane.

    This class manages the pixel data for an image in a specific eGFX format.
    It handles different bit depths (1, 3, 4, 8, 16, 24, 32 BPP) and provides
    methods to set individual pixels with automatic format conversion.
    """

    def __init__(self, size_x: int, size_y: int, plane_type: ImagePlaneType):
        self.size_x = size_x
        self.size_y = size_y
        self.type = plane_type
        self.data = bytearray()

        # Calculate buffer size based on BPP
        bpp = (int(plane_type) >> 8) & 0xFF

        if bpp == 1:
            self.data = bytearray(self._calc_1bpp_size(size_x, size_y))
        elif bpp == 3:
            self.data = bytearray(self._calc_3bpp_size(size_x, size_y))
        elif bpp == 4:
            self.data = bytearray(self._calc_4bpp_size(size_x, size_y))
        elif bpp == 8:
            self.data = bytearray(self._calc_8bpp_size(size_x, size_y))
        elif bpp == 16:
            self.data = bytearray(self._calc_16bpp_size(size_x, size_y))
        elif bpp == 24:
            self.data = bytearray(self._calc_24bpp_size(size_x, size_y))
        elif bpp == 32:
            self.data = bytearray(self._calc_32bpp_size(size_x, size_y))

    @staticmethod
    def _calc_1bpp_row_bytes(x: int) -> int:
        return (x + 7) >> 3

    @staticmethod
    def _calc_4bpp_row_bytes(x: int) -> int:
        return (x + 1) >> 1

    @staticmethod
    def _calc_1bpp_size(x: int, y: int) -> int:
        return ImagePlane._calc_1bpp_row_bytes(x) * y

    @staticmethod
    def _calc_3bpp_size(x: int, y: int) -> int:
        return ((x * y * 3)) // 8

    @staticmethod
    def _calc_4bpp_size(x: int, y: int) -> int:
        return ImagePlane._calc_4bpp_row_bytes(x) * y

    @staticmethod
    def _calc_8bpp_size(x: int, y: int) -> int:
        return x * y

    @staticmethod
    def _calc_16bpp_size(x: int, y: int) -> int:
        return x * y * 2

    @staticmethod
    def _calc_24bpp_size(x: int, y: int) -> int:
        return x * y * 3

    @staticmethod
    def _calc_32bpp_size(x: int, y: int) -> int:
        return x * y * 4

    def _bit_stream_put(self, position: int, pixel_state: int):
        """Put a bit in the bit stream (for 3BPP)"""
        base_address = position >> 3
        offset = position & 0x7

        if pixel_state > 0:
            self.data[base_address] |= (1 << offset)
        else:
            self.data[base_address] &= ~(1 << offset)

    def put_pixel(self, x: int, y: int, pixel_state: int):
        """Put a pixel in the image plane"""
        bpp = (int(self.type) >> 8) & 0xFF

        if bpp == 1:
            # 1BPP
            mem_width_bytes = self._calc_1bpp_row_bytes(self.size_x)
            offset = (y * mem_width_bytes) + (x >> 3)
            mask = 0x01 << (x & 0x07)

            if pixel_state > 0:
                self.data[offset] |= mask
            else:
                self.data[offset] &= ~mask

        elif bpp == 3:
            # 3BPP BGR
            bit_stream_pos = x * 3 + y * (self.size_x * 3)
            self._bit_stream_put(bit_stream_pos, pixel_state & 0x01)
            self._bit_stream_put(bit_stream_pos + 1, pixel_state & 0x02)
            self._bit_stream_put(bit_stream_pos + 2, pixel_state & 0x04)

        elif bpp == 4:
            # 4BPP
            mem_width_bytes = self._calc_4bpp_row_bytes(self.size_x)
            offset = (y * mem_width_bytes) + (x >> 1)
            b = (pixel_state >> 4) & 0xFF

            if (x & 0x01) > 0:
                self.data[offset] &= 0xF0
                self.data[offset] |= (b & 0xF)
            else:
                self.data[offset] &= 0x0F
                self.data[offset] |= ((b << 4) & 0xF0)

        elif bpp == 8:
            # 8BPP
            mem_width_bytes = self.size_x
            offset = (y * mem_width_bytes) + x
            self.data[offset] = pixel_state & 0xFF

        elif bpp == 16:
            # 16BPP
            mem_width_bytes = self.size_x * 2
            offset = (y * mem_width_bytes) + (x * 2)
            self.data[offset] = pixel_state & 0xFF
            self.data[offset + 1] = (pixel_state >> 8) & 0xFF

        elif bpp == 24:
            # 24BPP
            mem_width_bytes = self.size_x * 3
            offset = (y * mem_width_bytes) + (x * 3)
            self.data[offset] = pixel_state & 0xFF
            self.data[offset + 1] = (pixel_state >> 8) & 0xFF
            self.data[offset + 2] = (pixel_state >> 16) & 0xFF

        elif bpp == 32:
            # 32BPP
            mem_width_bytes = self.size_x * 4
            offset = (y * mem_width_bytes) + (x * 4)
            self.data[offset] = pixel_state & 0xFF
            self.data[offset + 1] = (pixel_state >> 8) & 0xFF
            self.data[offset + 2] = (pixel_state >> 16) & 0xFF
            self.data[offset + 3] = (pixel_state >> 24) & 0xFF


def luma(r: int, g: int, b: int) -> int:
    """Calculate luminance from RGB"""
    lum = int((r * 0.299) + (g * 0.587) + (b * 0.114))
    return min(lum, 255)


def remap_pixel_color(plane_type: ImagePlaneType, r: int, g: int, b: int) -> int:
    """
    Remap RGB color to target pixel state based on plane type.

    Converts 24-bit RGB color (8 bits per channel) to the appropriate pixel format
    for the target plane type. Handles color quantization and channel reordering.

    Args:
        plane_type: Target eGFX image plane type
        r: Red channel value (0-255)
        g: Green channel value (0-255)
        b: Blue channel value (0-255)

    Returns:
        Pixel state value in the format required by the plane type
    """
    ps = 0

    if plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_1BPP:
        ps = luma(r, g, b)
        ps = 0xFFFFFFFF if ps > 0x7F else 0

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_3BPP_BGR:
        if r > 127:
            ps |= 0x01
        if g > 127:
            ps |= 0x02
        if b > 127:
            ps |= 0x04

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_4BPP_XBGR:
        if r > 127:
            ps |= 0x01
        if g > 127:
            ps |= 0x02
        if b > 127:
            ps |= 0x04

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_4BPP:
        ps = luma(r, g, b)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_8BPP:
        ps = luma(r, g, b)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_8BPP_XRGB222:
        ps = ((r >> 6) << 4) + ((g >> 6) << 2) + (b >> 6)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_16BPP_RGB565:
        ps = ((r >> 3) << 11) + ((g >> 2) << 5) + (b >> 3)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_16BPP_BGR565:
        ps = ((b >> 3) << 11) + ((g >> 2) << 5) + (r >> 3)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_16BPP_GBRG3553:
        r5 = r >> 3
        b6 = b >> 3
        g5 = g >> 2
        ps = (r5 << 3) | (b6 << 8) | (g5 >> 3) | ((g5 & 0x7) << 13)

    elif plane_type == ImagePlaneType.eGFX_IMAGE_PLANE_16BPP_GRBG3553:
        r5 = r >> 3
        b6 = b >> 3
        g5 = g >> 2
        ps = (r5 << 8) | (b6 << 3) | (g5 >> 3) | ((g5 & 0x7) << 13)

    elif plane_type in [ImagePlaneType.eGFX_IMAGE_PLANE_24BPP,
                        ImagePlaneType.eGFX_IMAGE_PLANE_24BPP_RGB888]:
        ps = r + (g << 8) + (b << 16)

    elif plane_type in [ImagePlaneType.eGFX_IMAGE_PLANE_32BPP,
                        ImagePlaneType.eGFX_IMAGE_PLANE_32BPP_XRGB888]:
        ps = b + (g << 8) + (r << 16) + (0xFF << 24)

    return ps


def sanitize_name(name: str) -> str:
    """Sanitize filename for use as C identifier"""
    # Remove extension
    name = os.path.splitext(name)[0]

    # Replace invalid characters
    replacements = {
        ' ': '_', '@': '_', '#': '_', '$': '_', '%': '_', '^': '_',
        '&': '_', '*': '_', '(': '_', ')': '_', '[': '_', ']': '_',
        '-': '_', '=': '_', '+': '_', ',': '_', '~': '_', '?': '_',
        '"': '_', ':': '_', ';': '_', "'": '_', '.': '_', '>': '_',
        '<': '_', '`': '_'
    }

    for old, new in replacements.items():
        name = name.replace(old, new)

    # Prefix with underscore if starts with number
    if name and name[0].isdigit():
        name = '_' + name

    return name


def translate_image(image_path: str,
                    sprite_prefix: str,
                    plane_type: ImagePlaneType,
                    section_macro: str) -> Tuple[str, str]:
    """
    Translate a PNG image to eGFX format and generate C code.

    Args:
        image_path: Path to the PNG image file
        sprite_prefix: Prefix to add to sprite name (e.g., "" or "16BPP_RGB565_")
        plane_type: Target eGFX image plane type
        section_macro: C macro name for section placement

    Returns:
        Tuple of (c_code, header_code) containing the generated C source and header code
    """

    if not os.path.exists(image_path):
        raise FileNotFoundError(f"Could not open {image_path}")

    # Load image
    img = Image.open(image_path).convert('RGB')
    width, height = img.size

    print(f"{image_path} is {width} x {height} pixels")

    # Create image plane
    plane = ImagePlane(width, height, plane_type)

    # Convert pixels
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            ps = remap_pixel_color(plane_type, r, g, b)
            plane.put_pixel(x, y, ps)

    # Generate names
    sprite_name = sanitize_name(os.path.basename(image_path))
    sprite_struct_name = f"{sprite_prefix}{sprite_name}"
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
        if i % 16 == 0:
            code_output.append("\n")
        code_output.append(f"0x{byte:02X}")
        if i == len(plane.data) - 1:
            code_output.append(" }; \n\n")
        else:
            code_output.append(",")

    # Struct definition
    # Extract BPP and subtype from plane_type value
    bpp = (int(plane_type) >> 8) & 0xFF
    subtype = int(plane_type) & 0xFF

    code_output.append(f"\n{section_macro} egfx_img {sprite_struct_name} =\n")
    code_output.append("{\n")
    code_output.append(f".color_type = {subtype}, \n")
    code_output.append(f".bits_per_pixel = {bpp}, \n")
    code_output.append(".flags = 0, \n")
    code_output.append(".rsv = 0, \n")
    code_output.append(f".data = (uint8_t *){raw_data_array_name}, \n")
    code_output.append(f".size_x = {width}, \n")
    code_output.append(f".size_y = {height}, \n")
    code_output.append(".user = NULL\n")
    code_output.append("};\n")

    # Generate header
    header_output = []
    header_output.append("\n")
    header_output.append("////***************************************************************************\n")
    header_output.append(f"////                         {sprite_struct_name}\n")
    header_output.append("////***************************************************************************\n\n")
    header_output.append(f"\nextern {section_macro} egfx_img \t{sprite_struct_name};\n")

    return ''.join(code_output), ''.join(header_output)


def generate_sprites(sprite_dir: str,
                    plane_type: ImagePlaneType,
                    section_macro: str,
                    section_decorator: str,
                    output_folder: str,
                    output_name: str = None,
                    append_plane_type: bool = False):
    """
    Generate sprite files from a directory of PNG images.

    Args:
        sprite_dir: Directory containing PNG images to convert
        plane_type: eGFX image plane type (e.g., ImagePlaneType.eGFX_IMAGE_PLANE_16BPP_RGB565)
        section_macro: C macro name for section placement (e.g., "SPRITE_SECTION")
        section_decorator: Optional compiler-specific section decorator (e.g., "__attribute__((section('.rodata')))")
        output_folder: Directory where generated .c and .h files will be written
        output_name: Optional custom base name for output files (defaults to "Sprites<PlaneType>")
        append_plane_type: If True, sprite names include plane type prefix (e.g., "32BPP_XRGB888_icon")
                          If False, sprite names are just the image name (e.g., "icon")

    Output:
        Creates two files in output_folder:
        - <output_name>.c containing sprite data arrays and eGFX_ImagePlane structures
        - <output_name>.h containing extern declarations for the sprites
    """

    if not os.path.isdir(sprite_dir):
        print(f"{sprite_dir} does not exist")
        return

    print(f"Scanning {sprite_dir} for files")

    # Find all PNG files, excluding font directories
    all_png_files = list(Path(sprite_dir).rglob("*.png"))
    png_files = []
    for png_file in all_png_files:
        # Skip PNGs inside directories starting with "font_"
        skip = False
        for parent in png_file.parents:
            if parent.name.startswith("font_"):
                skip = True
                break
        if not skip:
            png_files.append(png_file)

    if not png_files:
        print(f"No image files specified in {sprite_dir}")
        return

    # Get plane type prefix
    plane_prefix = plane_type.name.replace("eGFX_IMAGE_PLANE", "")

    # Determine sprite prefix based on append_plane_type flag
    if append_plane_type:
        sprite_prefix = f"{plane_prefix}_"
    else:
        sprite_prefix = ""

    code_output = []
    header_output = []

    # Translate all images
    for png_file in png_files:
        print(f"Translating {png_file}")
        code, header = translate_image(str(png_file), sprite_prefix, plane_type, section_macro)
        code_output.append(code)
        header_output.append(header)

    # Write output files
    if output_name:
        output_base_name = output_name
    else:
        output_base_name = f"Sprites{plane_prefix}"
    output_path_c = os.path.join(output_folder, f"{output_base_name}.c")
    output_path_h = os.path.join(output_folder, f"{output_base_name}.h")

    print(f"Writing sprites source file to {output_path_c}")
    print(f"Writing sprites header to {output_path_h}")

    # Write C file
    with open(output_path_c, 'w') as f:
        f.write('#include "egfx.h" \n')
        f.write(f'#include "{output_base_name}.h" \n')
        f.write(''.join(code_output))

    # Write header file
    with open(output_path_h, 'w') as f:
        f.write('#include "egfx.h" \n\n')
        f.write(f"#ifndef _{output_base_name.upper()}_H\n")
        f.write(f"#define _{output_base_name.upper()}_H\n")
        f.write("\n")
        f.write(f"#ifndef {section_macro}\n")
        f.write(f"    #define {section_macro} const {section_decorator.strip()}\n")
        f.write(f"#endif\n")
        f.write("\n")
        f.write(''.join(header_output))
        f.write("\n#endif\n")

    print("\n\nAll Done!\n\n")


def main():
    # Build list of supported plane types for help text
    plane_types_help = '\n\nSupported Plane Types:\n'
    plane_types_help += '  1-bit:\n    eGFX_IMAGE_PLANE_1BPP\n'
    plane_types_help += '  3-bit:\n    eGFX_IMAGE_PLANE_3BPP_BGR\n'
    plane_types_help += '  4-bit:\n    eGFX_IMAGE_PLANE_4BPP\n    eGFX_IMAGE_PLANE_4BPP_XBGR\n'
    plane_types_help += '  8-bit:\n    eGFX_IMAGE_PLANE_8BPP\n    eGFX_IMAGE_PLANE_8BPP_XRGB222\n'
    plane_types_help += '  16-bit:\n    eGFX_IMAGE_PLANE_16BPP\n    eGFX_IMAGE_PLANE_16BPP_RGB565\n'
    plane_types_help += '    eGFX_IMAGE_PLANE_16BPP_BGR565\n    eGFX_IMAGE_PLANE_16BPP_GBRG3553\n'
    plane_types_help += '    eGFX_IMAGE_PLANE_16BPP_GRBG3553\n'
    plane_types_help += '  24-bit:\n    eGFX_IMAGE_PLANE_24BPP\n    eGFX_IMAGE_PLANE_24BPP_RGB888\n'
    plane_types_help += '  32-bit:\n    eGFX_IMAGE_PLANE_32BPP\n    eGFX_IMAGE_PLANE_32BPP_XRGB888\n'

    parser = argparse.ArgumentParser(
        description='eGFX Sprite Tool - Convert PNG images to eGFX image plane C code',
        epilog=plane_types_help,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument('sprite_dir',
                       help='path to folder containing PNG images')
    parser.add_argument('plane_type',
                       help='target eGFX_ImagePlane type (see supported types below)')
    parser.add_argument('output_folder',
                       help='path to output folder for generated C files')
    parser.add_argument('section_decorator', nargs='*', default=[''],
                       help='optional section placement decorator for embedded systems')
    parser.add_argument('-o', '--output-name', dest='output_name',
                       help='custom output filename (without extension). Default: Sprites<PlaneType>')
    parser.add_argument('--append-plane-type', action='store_true',
                       help='append plane type prefix to sprite names (e.g., 32BPP_XRGB888_icon). Default: icon')

    args = parser.parse_args()

    # Parse plane type
    try:
        plane_type = ImagePlaneType[args.plane_type]
    except KeyError:
        print(f"Error: {args.plane_type} is not a valid plane type")
        print("Valid types are:")
        for pt in ImagePlaneType:
            print(f"  {pt.name}")
        return 1

    section_decorator = ' '.join(args.section_decorator)

    generate_sprites(args.sprite_dir, plane_type, "SPRITE_SECTION",
                    section_decorator, args.output_folder, args.output_name,
                    args.append_plane_type)

    return 0


if __name__ == '__main__':
    sys.exit(main())

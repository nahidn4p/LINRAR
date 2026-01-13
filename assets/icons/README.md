# LINRAR Icons

This directory should contain application icons in various sizes.

## Required Icon Sizes

- 16x16 pixels
- 32x32 pixels
- 48x48 pixels
- 64x64 pixels
- 128x128 pixels
- 256x256 pixels
- 512x512 pixels

## Formats

- PNG format for raster icons
- SVG format for vector icon (scalable)

## File Names

- `linrar.png` - Main icon (256x256 recommended)
- `linrar.svg` - Vector icon (optional but recommended)

## Generating PNG Icons from SVG

The SVG icon (`linrar.svg`) is provided. To generate PNG icons in all required sizes:

```bash
./generate_png_from_svg.sh
```

This script requires either:
- **Inkscape** (preferred): `sudo apt-get install inkscape`
- **ImageMagick**: `sudo apt-get install imagemagick`

The script will generate:
- `linrar.png` (256x256) - Main icon file
- `linrar-16x16.png` through `linrar-512x512.png` - All size variants

## Icon Design

The LINRAR icon features:
- **Three stacked books/files** - Representing archived/compressed files
- **Blue gradient color scheme** - Modern and professional
- **Orange binding band** - Wrapping around the books (compression indicator)
- **"LR" branding** - LINRAR initials on the front book
- **Inspired by WinRAR** - Similar concept but unique design

## Creating Custom Icons

You can edit `linrar.svg` using:
- Inkscape (recommended for SVG editing)
- GIMP (with SVG import)
- Any vector graphics editor

The SVG uses standard gradients and paths, making it easy to customize colors and styling.

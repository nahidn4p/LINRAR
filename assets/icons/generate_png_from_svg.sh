#!/bin/bash

# Generate PNG icons from SVG in various sizes
# Requires: Inkscape or ImageMagick

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SVG_FILE="$SCRIPT_DIR/linrar.svg"

if [ ! -f "$SVG_FILE" ]; then
    echo "Error: linrar.svg not found"
    exit 1
fi

# Sizes to generate
SIZES=(16 32 48 64 128 256 512)

# Check for Inkscape (preferred)
if command -v inkscape &> /dev/null; then
    echo "Using Inkscape to generate PNG icons..."
    for size in "${SIZES[@]}"; do
        inkscape --export-type=png \
                 --export-filename="$SCRIPT_DIR/linrar-${size}x${size}.png" \
                 --export-width=$size \
                 --export-height=$size \
                 "$SVG_FILE"
        echo "Generated linrar-${size}x${size}.png"
    done
    # Also create the main icon file
    inkscape --export-type=png \
             --export-filename="$SCRIPT_DIR/linrar.png" \
             --export-width=256 \
             --export-height=256 \
             "$SVG_FILE"
    echo "Generated linrar.png (256x256)"
    
# Check for ImageMagick convert
elif command -v convert &> /dev/null; then
    echo "Using ImageMagick to generate PNG icons..."
    for size in "${SIZES[@]}"; do
        convert -background none -resize ${size}x${size} "$SVG_FILE" "$SCRIPT_DIR/linrar-${size}x${size}.png"
        echo "Generated linrar-${size}x${size}.png"
    done
    # Also create the main icon file
    convert -background none -resize 256x256 "$SVG_FILE" "$SCRIPT_DIR/linrar.png"
    echo "Generated linrar.png (256x256)"
    
else
    echo "Error: Neither Inkscape nor ImageMagick found"
    echo "Please install one of them:"
    echo "  sudo apt-get install inkscape"
    echo "  sudo apt-get install imagemagick"
    exit 1
fi

echo ""
echo "✓ All PNG icons generated successfully!"

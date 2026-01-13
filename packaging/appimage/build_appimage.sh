#!/bin/bash

# LINRAR AppImage Build Script
# This script builds an AppImage of LINRAR using linuxdeploy

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build_appimage"
APPIMAGE_DIR="$BUILD_DIR/AppDir"
DIST_DIR="$PROJECT_ROOT/dist"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== LINRAR AppImage Build Script ===${NC}"

# Create directories
mkdir -p "$BUILD_DIR" "$APPIMAGE_DIR" "$DIST_DIR"

# Check for linuxdeploy
LINUXDEPLOY="$BUILD_DIR/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="$BUILD_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"

# Download linuxdeploy if not present
if [ ! -f "$LINUXDEPLOY" ]; then
    echo -e "${YELLOW}Downloading linuxdeploy...${NC}"
    wget -q "$LINUXDEPLOY_URL" -O "$LINUXDEPLOY"
    chmod +x "$LINUXDEPLOY"
else
    echo -e "${GREEN}linuxdeploy found${NC}"
fi

# Download linuxdeploy-plugin-qt if not present
if [ ! -f "$LINUXDEPLOY_QT" ]; then
    echo -e "${YELLOW}Downloading linuxdeploy-plugin-qt...${NC}"
    wget -q "$LINUXDEPLOY_QT_URL" -O "$LINUXDEPLOY_QT"
    chmod +x "$LINUXDEPLOY_QT"
else
    echo -e "${GREEN}linuxdeploy-plugin-qt found${NC}"
fi

# Build LINRAR in release mode
echo -e "${GREEN}Building LINRAR in release mode...${NC}"
cd "$PROJECT_ROOT"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR="$APPIMAGE_DIR"

# Copy desktop file and icon
echo -e "${GREEN}Setting up AppDir...${NC}"
mkdir -p "$APPIMAGE_DIR/usr/share/applications"
mkdir -p "$APPIMAGE_DIR/usr/share/pixmaps"
cp "$PROJECT_ROOT/packaging/appimage/linrar.desktop" "$APPIMAGE_DIR/usr/share/applications/"

# Copy icon if it exists
if [ -f "$PROJECT_ROOT/assets/icons/linrar.png" ]; then
    cp "$PROJECT_ROOT/assets/icons/linrar.png" "$APPIMAGE_DIR/usr/share/pixmaps/linrar.png"
else
    echo -e "${YELLOW}Warning: Icon not found, creating placeholder...${NC}"
    # Create a simple placeholder icon (requires ImageMagick or similar)
    if command -v convert &> /dev/null; then
        convert -size 256x256 xc:blue -pointsize 72 -fill white -gravity center -annotate +0+0 "LR" "$APPIMAGE_DIR/usr/share/pixmaps/linrar.png"
    else
        echo -e "${RED}Warning: No icon available and ImageMagick not found${NC}"
    fi
fi

# Run linuxdeploy
echo -e "${GREEN}Running linuxdeploy...${NC}"
cd "$BUILD_DIR"

export QMAKE=$(which qmake6 2>/dev/null || which qmake 2>/dev/null || echo "")

# Build linuxdeploy command
LINUXDEPLOY_CMD="$LINUXDEPLOY \
    --appdir $APPIMAGE_DIR \
    --executable $APPIMAGE_DIR/usr/bin/LINRAR \
    --desktop-file $APPIMAGE_DIR/usr/share/applications/linrar.desktop \
    --plugin qt"

# Add icon if it exists
if [ -f "$APPIMAGE_DIR/usr/share/pixmaps/linrar.png" ]; then
    LINUXDEPLOY_CMD="$LINUXDEPLOY_CMD --icon-file $APPIMAGE_DIR/usr/share/pixmaps/linrar.png"
fi

LINUXDEPLOY_CMD="$LINUXDEPLOY_CMD --output appimage"

# Execute linuxdeploy
eval $LINUXDEPLOY_CMD

# Move AppImage to dist directory
if [ -f "$BUILD_DIR/LINRAR-x86_64.AppImage" ]; then
    mv "$BUILD_DIR/LINRAR-x86_64.AppImage" "$DIST_DIR/"
    echo -e "${GREEN}✓ AppImage created: $DIST_DIR/LINRAR-x86_64.AppImage${NC}"
else
    echo -e "${RED}Error: AppImage not created${NC}"
    exit 1
fi

echo -e "${GREEN}=== Build Complete ===${NC}"
echo -e "AppImage location: $DIST_DIR/LINRAR-x86_64.AppImage"

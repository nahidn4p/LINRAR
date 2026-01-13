#!/bin/bash

# LINRAR Package Build Script
# Builds .deb and .rpm packages using CPack

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build_packages"
DIST_DIR="$PROJECT_ROOT/dist"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== LINRAR Package Build Script ===${NC}"

# Create directories
mkdir -p "$BUILD_DIR" "$DIST_DIR"

# Build LINRAR
echo -e "${GREEN}Building LINRAR...${NC}"
cd "$PROJECT_ROOT"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build packages with CPack
echo -e "${GREEN}Building packages with CPack...${NC}"

# Build DEB package
if command -v dpkg-deb &> /dev/null; then
    echo -e "${YELLOW}Building .deb package...${NC}"
    cpack -G DEB
    if [ -f "$BUILD_DIR"/linrar_*.deb ]; then
        mv "$BUILD_DIR"/linrar_*.deb "$DIST_DIR/"
        echo -e "${GREEN}✓ .deb package created${NC}"
    fi
else
    echo -e "${YELLOW}dpkg-deb not found, skipping .deb package${NC}"
fi

# Build RPM package
if command -v rpmbuild &> /dev/null; then
    echo -e "${YELLOW}Building .rpm package...${NC}"
    cpack -G RPM
    if [ -f "$BUILD_DIR"/linrar-*.rpm ]; then
        mv "$BUILD_DIR"/linrar-*.rpm "$DIST_DIR/"
        echo -e "${GREEN}✓ .rpm package created${NC}"
    fi
else
    echo -e "${YELLOW}rpmbuild not found, skipping .rpm package${NC}"
fi

# Build source tarball
echo -e "${YELLOW}Building source tarball...${NC}"
cd "$PROJECT_ROOT"
cpack -G TGZ --config "$BUILD_DIR/CPackSourceConfig.cmake" 2>/dev/null || true
if [ -f "$BUILD_DIR"/linrar-*.tar.gz ]; then
    mv "$BUILD_DIR"/linrar-*.tar.gz "$DIST_DIR/"
    echo -e "${GREEN}✓ Source tarball created${NC}"
fi

echo -e "${GREEN}=== Build Complete ===${NC}"
echo -e "Packages location: $DIST_DIR"
ls -lh "$DIST_DIR"/*.{deb,rpm,tar.gz} 2>/dev/null || echo "No packages found"

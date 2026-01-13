#!/bin/bash

# LINRAR - Build All Packages Script
# This script builds all available package formats

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DIST_DIR="$SCRIPT_DIR/dist"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   LINRAR - Build All Packages          ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
echo ""

# Create dist directory
mkdir -p "$DIST_DIR"

# Menu
echo -e "${GREEN}Available build options:${NC}"
echo "  1) AppImage (portable)"
echo "  2) .deb package (Debian/Ubuntu)"
echo "  3) .rpm package (Fedora/RHEL)"
echo "  4) All packages"
echo "  5) Exit"
echo ""
read -p "Select option [1-5]: " choice

case $choice in
    1)
        echo -e "${GREEN}Building AppImage...${NC}"
        "$SCRIPT_DIR/packaging/appimage/build_appimage.sh"
        ;;
    2)
        echo -e "${GREEN}Building .deb package...${NC}"
        "$SCRIPT_DIR/packaging/build_packages.sh"
        ;;
    3)
        echo -e "${GREEN}Building .rpm package...${NC}"
        "$SCRIPT_DIR/packaging/build_packages.sh"
        ;;
    4)
        echo -e "${GREEN}Building all packages...${NC}"
        echo ""
        echo -e "${YELLOW}Step 1/2: Building AppImage...${NC}"
        "$SCRIPT_DIR/packaging/appimage/build_appimage.sh"
        echo ""
        echo -e "${YELLOW}Step 2/2: Building .deb and .rpm packages...${NC}"
        "$SCRIPT_DIR/packaging/build_packages.sh"
        ;;
    5)
        echo "Exiting..."
        exit 0
        ;;
    *)
        echo -e "${RED}Invalid option${NC}"
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}=== Build Summary ===${NC}"
echo -e "Packages location: ${BLUE}$DIST_DIR${NC}"
if [ -d "$DIST_DIR" ]; then
    echo ""
    echo "Built packages:"
    ls -lh "$DIST_DIR"/*.{AppImage,deb,rpm,tar.gz} 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' || echo "  No packages found"
fi

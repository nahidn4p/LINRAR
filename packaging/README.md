# LINRAR Packaging

This directory contains packaging scripts and configuration files for building distribution packages.

## Directory Structure

- `appimage/` - AppImage build script and desktop file
- `debian/` - Debian/Ubuntu package files
- `rpm/` - RPM spec file for Fedora/RHEL/openSUSE

## Quick Start

### Build AppImage
```bash
./appimage/build_appimage.sh
```

### Build All Packages
```bash
./build_packages.sh
```

## Package Formats

### AppImage
- **Script**: `appimage/build_appimage.sh`
- **Output**: `dist/LINRAR-x86_64.AppImage`
- **Portable**: Works on any Linux distribution
- **Size**: ~50-100MB (includes Qt libraries)

### .deb Package
- **Tool**: CPack with DEB generator
- **Output**: `dist/linrar_1.0.0_amd64.deb`
- **Platform**: Debian, Ubuntu, and derivatives
- **Install**: `sudo dpkg -i linrar_1.0.0_amd64.deb`

### .rpm Package
- **Tool**: CPack with RPM generator
- **Output**: `dist/linrar-1.0.0-1.x86_64.rpm`
- **Platform**: Fedora, RHEL, openSUSE, and derivatives
- **Install**: `sudo rpm -i linrar-1.0.0-1.x86_64.rpm` or `sudo dnf install linrar-1.0.0-1.x86_64.rpm`

## Dependencies

### Build Dependencies

**For AppImage:**
- wget
- linuxdeploy (downloaded automatically)
- linuxdeploy-plugin-qt (downloaded automatically)
- ImageMagick (optional, for icon generation)

**For .deb:**
- dpkg-dev
- debhelper
- fakeroot (optional)

**For .rpm:**
- rpm-build
- rpmdevtools (optional)

Install on Debian/Ubuntu:
```bash
sudo apt-get install dpkg-dev debhelper fakeroot
```

Install on Fedora/RHEL:
```bash
sudo dnf install rpm-build rpmdevtools
```

## Customization

### Desktop File
Edit `appimage/linrar.desktop` to customize the desktop entry.

### Package Metadata
- **.deb**: Edit `debian/control` and `debian/changelog`
- **.rpm**: Edit `rpm/linrar.spec`

### Icons
Place application icons in `../assets/icons/`:
- `linrar.png` (256x256 recommended)
- `linrar.svg` (optional, vector format)

## Troubleshooting

### AppImage Build Fails
- Ensure wget is installed
- Check internet connection (linuxdeploy is downloaded automatically)
- Verify Qt development packages are installed

### .deb Build Fails
- Install dpkg-dev: `sudo apt-get install dpkg-dev debhelper`
- Check that all build dependencies are met

### .rpm Build Fails
- Install rpm-build: `sudo dnf install rpm-build`
- Check that all build dependencies are met

### Missing Icons
If icons are missing, the build scripts will attempt to create placeholders. For best results, provide proper icon files in `assets/icons/`.

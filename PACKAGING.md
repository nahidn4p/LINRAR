# LINRAR Packaging Guide

This document provides detailed information about building distribution packages for LINRAR.

## Quick Start

### Build All Packages
```bash
./build_all.sh
```

This interactive script will guide you through building:
- AppImage (portable)
- .deb package (Debian/Ubuntu)
- .rpm package (Fedora/RHEL/openSUSE)

## Package Formats

### 1. AppImage

**What is it?**
- Single-file portable application
- No installation required
- Works on any Linux distribution
- Includes all dependencies (Qt libraries)

**Build:**
```bash
./packaging/appimage/build_appimage.sh
```

**Output:** `dist/LINRAR-x86_64.AppImage`

**Usage:**
```bash
chmod +x dist/LINRAR-x86_64.AppImage
./dist/LINRAR-x86_64.AppImage
```

**Requirements:**
- wget (for downloading linuxdeploy)
- ImageMagick (optional, for icon generation)

**Size:** ~50-100MB

### 2. .deb Package (Debian/Ubuntu)

**What is it?**
- Native Debian package format
- Installs via package manager
- Manages dependencies automatically
- Integrates with system

**Build:**
```bash
mkdir build_packages
cd build_packages
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G DEB
```

**Output:** `dist/linrar_1.0.0_amd64.deb`

**Installation:**
```bash
sudo dpkg -i linrar_1.0.0_amd64.deb
sudo apt-get install -f  # Install missing dependencies
```

**Requirements:**
- dpkg-dev
- debhelper

### 3. .rpm Package (Fedora/RHEL/openSUSE)

**What is it?**
- Native RPM package format
- Installs via package manager
- Manages dependencies automatically
- Integrates with system

**Build:**
```bash
mkdir build_packages
cd build_packages
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G RPM
```

**Output:** `dist/linrar-1.0.0-1.x86_64.rpm`

**Installation:**
```bash
# Fedora/RHEL
sudo dnf install linrar-1.0.0-1.x86_64.rpm

# openSUSE
sudo zypper install linrar-1.0.0-1.x86_64.rpm

# Traditional
sudo rpm -i linrar-1.0.0-1.x86_64.rpm
```

**Requirements:**
- rpm-build

## Build Scripts

### build_all.sh
Interactive script to build any or all package formats.

### packaging/appimage/build_appimage.sh
Builds AppImage using linuxdeploy.

### packaging/build_packages.sh
Builds .deb and .rpm packages using CPack.

## Package Configuration

### CMakeLists.txt
Contains CPack configuration for:
- Package metadata (name, version, description)
- Dependencies
- Installation paths
- Desktop integration

### debian/control
Debian package control file with:
- Package dependencies
- Build dependencies
- Package metadata

### debian/changelog
Package changelog for Debian/Ubuntu.

### debian/copyright
License information for Debian package.

### rpm/linrar.spec
RPM spec file with:
- Package metadata
- Build instructions
- Dependencies
- File list

## Desktop Integration

All packages include:
- Desktop entry file (`linrar.desktop`)
- Application icon (if available)
- MIME type associations for archive formats

### Desktop File
Located at: `packaging/appimage/linrar.desktop`

### Icons
Place icons in: `assets/icons/`
- `linrar.png` (256x256 recommended)
- `linrar.svg` (optional, vector format)

If icons are missing, the build scripts will attempt to create placeholders.

## Dependencies

### Runtime Dependencies
All packages require:
- rar or unrar (for RAR support)
- p7zip-full or 7z (for 7Z support)
- zip and unzip (for ZIP support)
- tar (for TAR support)
- Qt6 base libraries (or Qt5 as fallback)

### Build Dependencies
- CMake 3.16+
- Qt6 development packages (or Qt5)
- C++17 compiler
- CPack (included with CMake)

**For AppImage:**
- wget
- linuxdeploy (downloaded automatically)

**For .deb:**
- dpkg-dev
- debhelper

**For .rpm:**
- rpm-build

## Troubleshooting

### AppImage Build Issues

**Problem:** linuxdeploy download fails
- **Solution:** Check internet connection, ensure wget is installed

**Problem:** Qt libraries not found
- **Solution:** Install Qt6 development packages

**Problem:** Icon not found
- **Solution:** Place icon in `assets/icons/linrar.png` or install ImageMagick for placeholder generation

### .deb Build Issues

**Problem:** dpkg-deb not found
- **Solution:** `sudo apt-get install dpkg-dev debhelper`

**Problem:** Dependencies not met
- **Solution:** Install all build dependencies listed in `debian/control`

### .rpm Build Issues

**Problem:** rpmbuild not found
- **Solution:** `sudo dnf install rpm-build` or `sudo yum install rpm-build`

**Problem:** Build fails
- **Solution:** Check that all dependencies in `rpm/linrar.spec` are installed

## Customization

### Changing Package Version
Edit `CMakeLists.txt`:
```cmake
project(LINRAR VERSION 1.0.0 LANGUAGES CXX)
```

### Changing Package Metadata
- **.deb**: Edit `debian/control` and `debian/changelog`
- **.rpm**: Edit `rpm/linrar.spec`
- **CMake**: Edit CPack variables in `CMakeLists.txt`

### Adding Dependencies
- **.deb**: Edit `CPACK_DEBIAN_PACKAGE_DEPENDS` in `CMakeLists.txt`
- **.rpm**: Edit `Requires:` in `rpm/linrar.spec`

## Distribution

### AppImage
- Upload to GitHub Releases
- Make executable: `chmod +x LINRAR-x86_64.AppImage`
- Users can run directly without installation

### .deb Package
- Upload to Debian/Ubuntu repository
- Or distribute via GitHub Releases
- Users install with `sudo dpkg -i`

### .rpm Package
- Upload to Fedora/RHEL/openSUSE repository
- Or distribute via GitHub Releases
- Users install with `sudo dnf install` or `sudo rpm -i`

## Testing

Before distributing packages:

1. **Test AppImage** on different distributions
2. **Test .deb** installation on Debian/Ubuntu
3. **Test .rpm** installation on Fedora/RHEL
4. **Verify** desktop integration (menu entry, icon)
5. **Check** MIME type associations
6. **Test** application functionality

## Additional Resources

- [AppImage Documentation](https://docs.appimage.org/)
- [Debian Packaging Guide](https://www.debian.org/doc/manuals/packaging-tutorial/)
- [RPM Packaging Guide](https://rpm-packaging-guide.github.io/)
- [CPack Documentation](https://cmake.org/cmake/help/latest/module/CPack.html)

# LINRAR - Linux Archive Manager

A modern Qt 6-based archive manager for Linux that provides WinRAR-like functionality by interfacing with native command-line tools.

## Features

- **Multiple Format Support**: RAR, ZIP, 7Z, TAR, TAR.GZ, TAR.BZ2, TAR.XZ
- **Archive Operations**: Create, Extract, View, Add/Remove, Test, Repair, Split
- **Password Protection**: Encrypt archives with passwords
- **Modern UI**: Hybrid design with modern aesthetics and familiar WinRAR layout
- **Dual-Pane Interface**: File browser and archive view side-by-side
- **Drag and Drop**: Easy file management

## Requirements

### Build Dependencies
- CMake 3.16 or higher
- Qt 6 (Core, Widgets, Gui) - preferred, or Qt 5.15+ as fallback
- C++17 compatible compiler (GCC, Clang)

### Installing Qt6

**Debian/Ubuntu:**
```bash
sudo apt-get install qt6-base-dev qt6-base-dev-tools cmake build-essential
```

**Arch Linux:**
```bash
sudo pacman -S qt6-base cmake base-devel
```

**Fedora:**
```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++ make
```

**If Qt6 is not available, Qt5 will work as fallback:**
```bash
# Debian/Ubuntu
sudo apt-get install qtbase5-dev qt5-qmake cmake build-essential

# Arch Linux
sudo pacman -S qt5-base cmake base-devel

# Fedora
sudo dnf install qt5-qtbase-devel cmake gcc-c++ make
```

### Runtime Dependencies
The application requires native command-line tools to be installed:
- `rar` or `unrar` (for RAR support)
- `7z` or `7za` (for 7Z support)
- `zip` and `unzip` (for ZIP support)
- `tar` (for TAR variants support)

### Installing Runtime Dependencies

**Debian/Ubuntu:**
```bash
sudo apt-get install rar unrar p7zip-full zip unzip tar
```

**Arch Linux:**
```bash
sudo pacman -S rar unrar p7zip zip unzip tar
```

**Fedora:**
```bash
sudo dnf install rar unrar p7zip p7zip-plugins zip unzip tar
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./LINRAR
```

Or from the build directory:
```bash
./build/LINRAR
```

## Building Distribution Packages

LINRAR can be packaged in multiple formats for easy distribution.

### AppImage (Portable)

Build a portable AppImage that works on any Linux distribution:

```bash
./packaging/appimage/build_appimage.sh
```

The AppImage will be created in the `dist/` directory as `LINRAR-x86_64.AppImage`.

**Requirements:**
- wget (for downloading linuxdeploy)
- ImageMagick (optional, for icon generation)

**Usage:**
```bash
chmod +x dist/LINRAR-x86_64.AppImage
./dist/LINRAR-x86_64.AppImage
```

### .deb Package (Debian/Ubuntu)

Build a Debian package using CPack:

```bash
./packaging/build_packages.sh
```

Or manually:
```bash
mkdir build_packages
cd build_packages
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G DEB
```

**Installation:**
```bash
sudo dpkg -i linrar_1.0.0_amd64.deb
sudo apt-get install -f  # Install dependencies if needed
```

**Requirements:**
- dpkg-dev
- debhelper

### .rpm Package (Fedora/RHEL/openSUSE)

Build an RPM package using CPack:

```bash
./packaging/build_packages.sh
```

Or manually:
```bash
mkdir build_packages
cd build_packages
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G RPM
```

**Installation:**
```bash
sudo rpm -i linrar-1.0.0-1.x86_64.rpm
# Or on Fedora/RHEL:
sudo dnf install linrar-1.0.0-1.x86_64.rpm
```

**Requirements:**
- rpm-build
- rpmdevtools (optional)

### All Packages

Build all available package formats at once:

```bash
./packaging/build_packages.sh
```

All packages will be created in the `dist/` directory.

## Usage

1. **Open Archive**: File → Open Archive or drag and drop an archive file
2. **Create Archive**: Select files in the file browser, then click "Add" or use File → New Archive
3. **Extract**: Select files in archive view, then click "Extract" or right-click → Extract
4. **Add Files**: Drag files from file browser to archive view
5. **Remove Files**: Select files in archive view, then click "Delete" or press Delete key

## CI/CD

LINRAR uses GitHub Actions for continuous integration and automated releases:

- **Build Test**: Runs on every push and pull request to test compilation
- **Build and Release**: Automatically builds AppImage, .deb, and .rpm packages
- **Releases**: Creates GitHub releases with all packages when tags are pushed

### Automated Builds

When you push a git tag (e.g., `v1.0.0`), GitHub Actions will:
1. Build AppImage (portable)
2. Build .deb package (Debian/Ubuntu)
3. Build .rpm package (Fedora/RHEL)
4. Create a GitHub Release with all packages attached

### Manual Release

You can also trigger a release manually using the GitHub Actions workflow dispatch.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is open source. See LICENSE file for details.

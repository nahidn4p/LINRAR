# GitHub Actions Workflows Changelog

## Version 1.0.0

### Initial Release

**Workflows Created:**
- `build-and-release.yml` - Main build and release workflow
- `build-test.yml` - CI testing workflow
- `release.yml` - Manual release workflow

**Features:**
- Automated AppImage builds
- Automated .deb package builds
- Automated .rpm package builds
- Automatic GitHub Release creation on tags
- Build testing on push/PR
- Manual release trigger

**Triggers:**
- Push to main/master
- Pull requests
- Git tags (v* pattern)
- Manual workflow dispatch

**Supported Platforms:**
- Ubuntu 22.04 (primary)
- Ubuntu 20.04 (optional, via matrix)

**Package Formats:**
- AppImage (portable)
- .deb (Debian/Ubuntu)
- .rpm (Fedora/RHEL)

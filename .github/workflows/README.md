# GitHub Actions Workflows

This directory contains GitHub Actions workflows for automated building and releasing of LINRAR.

## Workflows

### build-and-release.yml
Main workflow that builds all package formats and creates releases.

**Triggers:**
- Push to main/master branch
- Pull requests to main/master
- Git tags starting with `v` (e.g., `v1.0.0`)
- Manual dispatch via GitHub UI

**Jobs:**
1. **build-appimage**: Builds portable AppImage
2. **build-deb**: Builds Debian/Ubuntu .deb package
3. **build-rpm**: Builds Fedora/RHEL .rpm package
4. **create-release**: Creates GitHub Release with all packages (only on tags)

**Artifacts:**
- `LINRAR-x86_64.AppImage` - Portable application
- `linrar_*_amd64.deb` - Debian package
- `linrar-*-*.x86_64.rpm` - RPM package

### build-test.yml
Continuous integration workflow for testing builds.

**Triggers:**
- Push to main/master/develop branches
- Pull requests to main/master

**Purpose:**
- Verify code compiles
- Test Debug and Release builds
- Check binary dependencies

### release.yml
Manual release workflow for creating releases.

**Triggers:**
- Manual dispatch only

**Purpose:**
- Create git tags
- Trigger release builds
- Useful for creating releases without pushing tags manually

## Usage

### Creating a Release

#### Method 1: Push a Tag
```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

#### Method 2: Use GitHub UI
1. Go to Actions → Release workflow
2. Click "Run workflow"
3. Enter version (e.g., `v1.0.0`)
4. Click "Run workflow"

#### Method 3: Create Release via GitHub UI
1. Go to Releases → Draft a new release
2. Create a new tag (e.g., `v1.0.0`)
3. Publish release
4. This will trigger the build-and-release workflow

### Viewing Build Status

- Go to the **Actions** tab in GitHub
- View workflow runs and their status
- Download artifacts from completed runs
- Check build logs for errors

### Artifacts

Artifacts are available:
- During workflow runs (download from Actions tab)
- In GitHub Releases (when tags are created)

## Troubleshooting

### Build Failures

**AppImage build fails:**
- Check that linuxdeploy downloaded successfully
- Verify Qt6 is installed
- Check AppDir structure

**DEB build fails:**
- Ensure dpkg-dev and debhelper are installed
- Check CMake CPack configuration
- Verify package dependencies

**RPM build fails:**
- Ensure rpm-build is installed
- Check CMake CPack configuration
- Verify package dependencies

### Release Creation Fails

- Check that all build jobs succeeded
- Verify artifacts were uploaded
- Ensure GITHUB_TOKEN has release permissions
- Check tag format (should start with `v`)

## Customization

### Adding New Build Jobs

Edit `.github/workflows/build-and-release.yml`:
1. Add new job definition
2. Add job to `needs` list in `create-release`
3. Update artifact download pattern

### Changing Build Matrix

Modify the `strategy.matrix` section to build on multiple platforms or configurations.

### Release Notes

Customize release notes generation in the `create-release` job's `Generate release notes` step.

## Dependencies

Workflows use these GitHub Actions:
- `actions/checkout@v4` - Checkout repository
- `actions/upload-artifact@v4` - Upload build artifacts
- `actions/download-artifact@v4` - Download artifacts
- `softprops/action-gh-release@v1` - Create releases
- `actions/github-script@v7` - GitHub API interactions

## Security

- Workflows use `GITHUB_TOKEN` (automatically provided)
- No secrets required for basic builds
- For signing packages, add GPG keys as secrets

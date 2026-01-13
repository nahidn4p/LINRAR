# CI/CD Guide for LINRAR

This guide explains how to use GitHub Actions for automated building and releasing of LINRAR packages.

## Quick Start

### Creating Your First Release

1. **Tag your release:**
   ```bash
   git tag -a v1.0.0 -m "Release v1.0.0"
   git push origin v1.0.0
   ```

2. **GitHub Actions will automatically:**
   - Build AppImage
   - Build .deb package
   - Build .rpm package
   - Create GitHub Release
   - Upload all packages

3. **Find your release:**
   - Go to GitHub → Releases
   - Download packages from the release page

## Workflow Overview

### build-and-release.yml

**When it runs:**
- Push to main/master branch
- Pull requests
- Git tags (v*)
- Manual trigger

**What it does:**
1. Builds AppImage (portable)
2. Builds .deb package (Debian/Ubuntu)
3. Builds .rpm package (Fedora/RHEL)
4. Creates GitHub Release (on tags only)

### build-test.yml

**When it runs:**
- Every push to main/master/develop
- Pull requests

**What it does:**
- Tests compilation in Debug and Release modes
- Verifies build succeeds

### release.yml

**When it runs:**
- Manual trigger only

**What it does:**
- Creates git tag
- Triggers release build

## Release Methods

### Method 1: Git Tag (Recommended)

```bash
# Create and push a tag
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

This automatically triggers the build-and-release workflow.

### Method 2: GitHub UI Release

1. Go to **Releases** → **Draft a new release**
2. Click **Choose a tag** → **Create new tag**
3. Enter tag name (e.g., `v1.0.0`)
4. Fill in release title and description
5. Click **Publish release**

This triggers the workflow via the `release` event.

### Method 3: Manual Workflow Dispatch

1. Go to **Actions** → **Build and Release**
2. Click **Run workflow**
3. Select branch (usually `main`)
4. Optionally enter version
5. Click **Run workflow**

## Viewing Build Status

### Check Workflow Runs

1. Go to **Actions** tab
2. Click on a workflow run
3. View individual job status
4. Check logs for errors

### Download Artifacts

1. Go to **Actions** → Select a workflow run
2. Scroll to **Artifacts** section
3. Download individual artifacts:
   - `LINRAR-x86_64.AppImage`
   - `linrar-deb-package`
   - `linrar-rpm-package`

### View Releases

1. Go to **Releases** tab
2. Click on a release
3. Download packages from **Assets** section

## Troubleshooting

### Build Fails

**Check logs:**
1. Go to Actions → Failed workflow
2. Click on failed job
3. Expand failed step
4. Review error messages

**Common issues:**
- Missing dependencies → Check installation steps
- Qt version mismatch → Verify Qt6 is installed
- Permission errors → Check file permissions
- Missing tools → Verify all build tools are installed

### Release Not Created

**Check:**
1. All build jobs must succeed
2. Tag must start with `v` (e.g., `v1.0.0`)
3. GITHUB_TOKEN must have release permissions
4. Artifacts must be uploaded successfully

### Artifacts Missing

**Verify:**
1. Build jobs completed successfully
2. Artifacts were uploaded
3. Check artifact names match expected patterns
4. Review upload-artifact step logs

## Customization

### Change Build Matrix

Edit `.github/workflows/build-and-release.yml`:

```yaml
strategy:
  matrix:
    os: [ubuntu-20.04, ubuntu-22.04]
```

### Add New Package Format

1. Add new job in workflow
2. Add to `needs` in `create-release`
3. Update artifact download pattern

### Modify Release Notes

Edit the `Generate release notes` step in `create-release` job.

## Best Practices

### Versioning

- Use semantic versioning (v1.0.0)
- Tag format: `vMAJOR.MINOR.PATCH`
- Always include `v` prefix

### Release Notes

- Write clear release notes
- List major changes
- Include breaking changes
- Reference issues/PRs

### Testing

- Test builds locally before tagging
- Verify all package formats work
- Test on target distributions

### Security

- Never commit secrets
- Use GitHub Secrets for sensitive data
- Review workflow permissions

## Examples

### Release v1.0.0

```bash
# Update version in CMakeLists.txt
# Commit changes
git add .
git commit -m "Bump version to 1.0.0"
git push

# Create and push tag
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

### Hotfix Release

```bash
# Create hotfix branch
git checkout -b hotfix/v1.0.1

# Make fixes
# Commit
git commit -m "Fix critical bug"

# Tag and push
git tag -a v1.0.1 -m "Hotfix v1.0.1"
git push origin v1.0.1
```

## Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [Release Management](https://docs.github.com/en/repositories/releasing-projects-on-github)

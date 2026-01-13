# Contributing to LINRAR

Thank you for your interest in contributing to LINRAR! This document provides guidelines and instructions for contributing.

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/your-username/linrar.git
   cd linrar
   ```
3. **Create a branch** for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Prerequisites
- CMake 3.16+
- Qt 6 (or Qt 5.15+)
- C++17 compiler
- Git

### Building from Source
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## Code Style

- Follow Qt coding conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small
- Use const where possible

## Making Changes

### Before You Start
- Check existing issues and pull requests
- Create an issue to discuss major changes
- Keep changes focused and atomic

### Commit Messages
- Use clear, descriptive commit messages
- Start with a verb (Add, Fix, Update, Remove)
- Reference issue numbers when applicable
- Example: `Fix: Handle missing archive tools gracefully (#42)`

### Pull Request Process

1. **Update your branch** with the latest main:
   ```bash
   git checkout main
   git pull upstream main
   git checkout your-branch
   git rebase main
   ```

2. **Test your changes**:
   - Build in both Debug and Release modes
   - Test on different Linux distributions if possible
   - Verify no regressions

3. **Create a Pull Request**:
   - Fill out the PR template
   - Describe your changes clearly
   - Reference related issues
   - Add screenshots if UI changes

4. **Respond to feedback**:
   - Address review comments
   - Update your PR as needed
   - Keep discussions constructive

## Testing

### Manual Testing
- Test all archive operations (create, extract, view, modify)
- Test with different archive formats
- Test error handling (missing tools, corrupted archives)
- Test UI interactions

### Automated Testing
- GitHub Actions will test builds automatically
- Ensure your code compiles without warnings
- Test on multiple Qt versions if possible

## Documentation

- Update README.md for user-facing changes
- Update code comments for API changes
- Add examples for new features
- Keep CHANGELOG.md updated (if applicable)

## Issue Reporting

### Bug Reports
- Use the bug report template
- Include steps to reproduce
- Provide environment details
- Include error messages and logs

### Feature Requests
- Use the feature request template
- Describe the use case
- Explain the expected behavior
- Consider implementation complexity

## Release Process

Releases are managed through GitHub Actions:

1. **Create a tag**:
   ```bash
   git tag -a v1.0.1 -m "Release v1.0.1"
   git push origin v1.0.1
   ```

2. **GitHub Actions** will automatically:
   - Build all package formats
   - Create a GitHub Release
   - Upload artifacts

## Code of Conduct

- Be respectful and inclusive
- Welcome newcomers
- Focus on constructive feedback
- Help others learn and grow

## Questions?

- Open an issue for questions
- Check existing issues and discussions
- Review the documentation

Thank you for contributing to LINRAR!

Name:           linrar
Version:        1.0.0
Release:        1%{?dist}
Summary:        Linux Archive Manager - WinRAR alternative
License:        MIT
URL:            https://github.com/linrar/linrar
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  cmake >= 3.16
BuildRequires:  qt6-qtbase-devel >= 6.0.0
BuildRequires:  gcc-c++
BuildRequires:  make
Requires:       qt6-qtbase >= 6.0.0
Requires:       rar
Requires:       p7zip
Requires:       zip
Requires:       unzip
Requires:       tar

%description
LINRAR is a modern Qt-based archive manager for Linux that provides
WinRAR-like functionality by interfacing with native command-line tools
(rar, 7zip, tar, etc.).

Features:
- Support for RAR, ZIP, 7Z, TAR, TAR.GZ, TAR.BZ2, TAR.XZ formats
- Create, extract, view, and modify archives
- Password protection and encryption
- Modern dual-pane interface
- Drag and drop support

%prep
%setup -q

%build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?_smp_mflags}

%install
cd build
make install DESTDIR=%{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/LINRAR
%{_datadir}/applications/linrar.desktop
%{_datadir}/pixmaps/linrar.png

%changelog
* Mon Jan 01 2024 LINRAR Team <linrar@example.com> - 1.0.0-1
- Initial release
- Support for RAR, ZIP, 7Z, TAR formats
- Modern Qt-based GUI
- Dual-pane interface

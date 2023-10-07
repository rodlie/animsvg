Name: friction
Version: __FRICTION_VERSION__
Release: 1
Summary: Friction Motion Graphics
Group: System Environment/Base
License: GPL3
Source0: friction-%{version}.tar

AutoReq: no
%global debug_package %{nil}

%description
Friction is an open-source, vector-based application designed for creating web animations and videos.

%prep
%setup -q #unpack tarball

%build

%install

cp -rfa opt %{buildroot}/
cp -rfa usr %{buildroot}/
rm -rf %{buildroot}/opt/friction/share/doc

%files
%defattr(-,root,root,-)
/opt/friction
/usr/bin/friction
/usr/share/applications/friction.desktop
/usr/share/mime/packages/friction.xml
/usr/share/icons/hicolor

%changelog


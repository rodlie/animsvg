Name: friction
Version: __FRICTION_PKG_VERSION__
Release: 1
Summary: Friction Motion Graphics
Group: System Environment/Base
License: GPL3
Source0: friction-__FRICTION_VERSION__.tar

AutoReq: no
%global debug_package %{nil}
%global __provides_exclude_from /opt

%description
Friction is an open-source, vector-based application designed for creating web animations and videos.

%prep
%setup -n friction-__FRICTION_VERSION__

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


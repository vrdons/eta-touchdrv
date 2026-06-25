%global upstream_version %{?version_override}%{!?version_override:0.5.1}
%global debug_package %{nil}

Name:           eta-touchdrv
Version:        %{upstream_version}
Release:        1%{?dist}
Summary:        Non-HID touchscreen drivers for Fatih interactive whiteboards

License:        GPL-3.0-or-later
URL:            https://github.com/vrdons/eta-touchdrv
Source0:        %{name}-%{version}.tar.gz

ExclusiveArch:  x86_64 aarch64

BuildRequires:  systemd-rpm-macros

Requires:       dkms
Requires:       kmod
Requires:       systemd
Requires:       systemd-udev
Requires:       usbutils
Requires(post): dkms
Requires(post): systemd
Requires(post): systemd-udev
Requires(preun): dkms
Requires(preun): systemd
Requires(postun): systemd
Requires(postun): systemd-udev

%description
eta-touchdrv provides a DKMS kernel module and the corresponding daemon
binaries for non-HID 2-camera and 4-camera touchscreen sensors used by Fatih
interactive whiteboards. The kernel module is open source; daemon binaries are
provided by Vestel.

%prep
%autosetup -n %{name}-%{version}

%build
# DKMS builds the kernel module on the target system.

%install
install -d %{buildroot}/usr/src/%{name}-%{version}
cp -a src/Makefile src/OpticalDrv.c src/include \
  %{buildroot}/usr/src/%{name}-%{version}/

install -Dm0644 package/common/build.dkms \
  %{buildroot}/usr/src/%{name}-%{version}/dkms.conf
sed -i 's/__VERSION__/%{version}/g' \
  %{buildroot}/usr/src/%{name}-%{version}/dkms.conf

install -d %{buildroot}%{_bindir}/touch2 %{buildroot}%{_bindir}/touch4

for binary in bin/touch2/*.%{_arch}; do
  [ -e "$binary" ] || continue
  install -m0755 "$binary" %{buildroot}%{_bindir}/touch2/
done

for binary in bin/touch4/*.%{_arch}; do
  [ -e "$binary" ] || continue
  install -m0755 "$binary" %{buildroot}%{_bindir}/touch4/
done

install -Dm0755 package/common/touchdrv_launcher \
  %{buildroot}%{_bindir}/touchdrv_launcher
install -Dm0644 package/common/eta-touchdrv@.service \
  %{buildroot}%{_unitdir}/eta-touchdrv@.service
install -Dm0644 package/common/60-eta-touchdrv.rules \
  %{buildroot}/usr/lib/udev/rules.d/60-eta-touchdrv.rules

%post
if command -v systemctl >/dev/null 2>&1; then
  systemctl daemon-reload >/dev/null 2>&1 || true
fi

if command -v dkms >/dev/null 2>&1; then
  dkms add -m %{name} -v %{version} >/dev/null 2>&1 || true
  dkms build -m %{name} -v %{version} >/dev/null 2>&1 || true
  dkms install -m %{name} -v %{version} --force >/dev/null 2>&1 || true
fi

if command -v udevadm >/dev/null 2>&1; then
  udevadm control --reload-rules >/dev/null 2>&1 || true
  udevadm trigger --subsystem-match=usb --attr-match=idVendor=6615 --action=add >/dev/null 2>&1 || true
  udevadm trigger --subsystem-match=usb --attr-match=idVendor=2621 --action=add >/dev/null 2>&1 || true
fi

%preun
if [ "$1" -eq 0 ]; then
  if command -v systemctl >/dev/null 2>&1; then
    systemctl stop 'eta-touchdrv@*.service' >/dev/null 2>&1 || true
  fi

  if command -v dkms >/dev/null 2>&1; then
    dkms remove -m %{name} -v %{version} --all >/dev/null 2>&1 || true
  fi
fi

%postun
if command -v systemctl >/dev/null 2>&1; then
  systemctl daemon-reload >/dev/null 2>&1 || true
fi

if command -v udevadm >/dev/null 2>&1; then
  udevadm control --reload-rules >/dev/null 2>&1 || true
fi

%files
%license LICENSE
%doc README.md
/usr/src/%{name}-%{version}/
%{_bindir}/touchdrv_launcher
%dir %{_bindir}/touch2
%dir %{_bindir}/touch4
%{_bindir}/touch2/*
%{_bindir}/touch4/*
%{_unitdir}/eta-touchdrv@.service
/usr/lib/udev/rules.d/60-eta-touchdrv.rules

%changelog
* Tue May 19 2026 Vrdons <vrdons@proton.me> - 0.5.1-1
- Add Fedora RPM packaging for COPR builds

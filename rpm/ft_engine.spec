Name:     ft_engine
Version:  1.0
Release:  1%{?dist}
Summary:  ft_engine
Summary(zh_CN):  ft_engine
License:  Apache-2.0
URL:      https://gitee.com/openeuler/ft_engine
Source0:   %{name}-%{version}.tar.gz

BuildRequires:  python3
Requires(post): python3
Requires(preun): python3

%description
ft_engine is contains modules window display and window management, providing the core capabilities of the Windows engine

%description -l zh_CN
ft_engine包含窗口显示和窗口管理模块，提供Windows引擎的核心能力

%prep
%setup -q

%build
pip3 install -r %{_builddir}/%{name}-%{version}/build/configs/requirements.txt
%{_builddir}/%{name}-%{version}/build/prebuild.sh
%{_builddir}/%{name}-%{version}/build.sh

%install
%{_builddir}/%{name}-%{version}/build.sh -i
mkdir -p %{buildroot}/%{_libdir}
case $(uname -m) in
    x86_64)  arch=x64;;
    aarch64) arch=aarch64;;
esac
cp %{_builddir}/%{name}-%{version}/out/Debug/$arch/common/common/*.so %{buildroot}/%{_libdir}
cp %{_builddir}/%{name}-%{version}/third_party/ft_flutter/out/Debug/$arch/lib/*.so %{buildroot}/%{_libdir}
cp %{_builddir}/%{name}-%{version}/third_party/ft_mmi/out/Debug/$arch/common/common/*.so %{buildroot}/%{_libdir}
cp %{_builddir}/%{name}-%{version}/third_party/ft_multimedia/out/Debug/$arch/common/common/*.so %{buildroot}/%{_libdir}

%files
%{_libdir}/*.so

%changelog
* Wed Jul 26 2023 xinghailiao  <xighailiao@smail.xtu.edu.cn> - 1.0-1
- Initial version

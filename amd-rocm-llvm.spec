%global commit0 e8f4abe9a85390e44e394038003e731991e4b5ce
%global _lto_cflags %{nil}
%global _name amd-rocm-llvm
%global projects "clang;lld"
%global rocm_path /opt/rocm
%global runtimes "compiler-rt"
%global shortcommit0 %(c=%{commit0}; echo ${c:0:7})
%global targets "X86;AMDGPU"
%global toolchain clang

%define patch_level 5

%bcond_with debug

%if %{without debug}
  %if %{without static}
    %global suf %{nil}
  %else
    %global suf -static
  %endif
%else
  %if %{without static}
    %global suf -debug
  %else
    %global suf -static-debug
  %endif
%endif

Name: %{_name}%{suf}

Version:        5.6
Summary:        AMD's fork of LLVM for ROCm
License:        Apache-2.0 WITH LLVM-exception OR NCSA
Release:        %{patch_level}.git%{?shortcommit0}%{?dist}

URL:            https://github.com/trixirt/llvm-project
Source0:        %{url}/archive/%{commit0}/llvm-project-%{shortcommit0}.tar.gz

BuildRequires:  clang
BuildRequires:  cmake
BuildRequires:  ninja-build

%if %{without debug}
%global debug_package %{nil}
%endif

%description
%{summary}

%package devel
Summary:        AMD's fork of LLVM for ROCm

%description devel
%{summary}

%prep
%autosetup -p1 -n llvm-project-%{commit0}

%build
cd llvm
%cmake	-G Ninja \
	 -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
%if %{without debug}
        -DCMAKE_BUILD_TYPE=RELEASE \
%else
	-DCMAKE_BUILD_TYPE=DEBUG \
%endif
	-DCMAKE_CXX_FLAGS=-I%{rocm_path}/include \
	-DCMAKE_INSTALL_PREFIX=%{rocm_path}/llvm \
	-DLLVM_BUILD_RUNTIME=ON \
        -DLLVM_BUILD_TESTS=ON \
	-DLLVM_BUILD_TOOLS=ON \
	-DLLVM_ENABLE_FFI=ON \
        -DLLVM_ENABLE_PROJECTS=%{projects} \
	-DLLVM_ENABLE_RTTI=ON \
	-DLLVM_ENABLE_RUNTIMES=%{runtimes} \
        -DLLVM_ENABLE_ZLIB=ON \
        -DLLVM_INCLUDE_TESTS=ON \
	-DLLVM_INCLUDE_TOOLS=ON \
        -DLLVM_PARALLEL_LINK_JOBS=8 \
        -DLLVM_TARGETS_TO_BUILD=%{targets} \
	-DLLVM_UNREACHABLE_OPTIMIZE=OFF \
	-DLLVM_USE_PERF=ON

%cmake_build

%install
cd llvm
%cmake_install

%files devel
/opt/rocm


%changelog
* Fri Aug 04 2023 Tom Rix <trix@redhat.com>
- Stub something together

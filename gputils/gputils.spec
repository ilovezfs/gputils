# gputils-@VERSION@.spec 
#
# Generated automatically from gputils.spec by make.
#
# Copyright (C) 2001,2002 Craig Franklin
# 
# This file is part of gputils.
# 
# gputils is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# gputils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gputils; see the file COPYING.  If not, write to
# the Free Software Foundation, 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA. 

Name: gputils
Version: @VERSION@
Release: 1
Summary: Development utilities for Microchip (TM) PIC (TM) microcontrollers
Copyright: GPL
Group: Development/Languages
Source: http://prdownloads.sourceforge.net/gputils/gputils-%{version}.tar.gz
Packager: @NAME@ @EMAIL@
Distribution: Red Hat Linux
URL: http://gputils.sourceforge.net
Buildroot: %{_builddir}/%{name}-%{version}-root

%description
This is a collection of development tools for Microchip (TM) PIC (TM) 
microcontrollers. It's intended to be compatible with the manufacturer's 
tools, MPASM, MPLINK,and MPLIB.

This is ALPHA software: there may be serious bugs in it, and it's
nowhere near complete.  gputils currently only implements a subset of
the features available with Microchip's tools.  See the documentation for 
an up-to-date list of what gputils can do.

%prep
%setup

%build
./configure --prefix=/usr 
make

%clean
rm -rf $RPM_BUILD_ROOT

%install
make DESTDIR="$RPM_BUILD_ROOT" install

%files
%defattr(-, root, root)
%{_bindir}/gpasm
%{_bindir}/gpdasm
%{_bindir}/gpvc
%{_datadir}/gputils/header/*
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README 
%doc doc/gputils.lyx doc/gputils.pdf doc/gputils.ps

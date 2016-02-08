#!/bin/bash

aclocal
autoheader
autoconf
automake --add-missing --copy
rm -rf ./autom4te.cache

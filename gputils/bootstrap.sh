#!/bin/bash

aclocal
autoconf
autoheader
automake --add-missing --copy
rm -rf ./autom4te.cache

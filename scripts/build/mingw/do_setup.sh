if test -n "$1"
then
  srcfilename=$1
else
  srcfilename=gputils-0.13.7.tar.gz
fi

if test -n "$2"
then
  packager=$2
else
  packager="Borut Razem"
fi

if test -n "$3"
then
  email=$3
else
  email=borut.razem@siol.net
fi

CC="gcc -mno-cygwin" ./make-mingw --email=$email --name="$packager" --source=$srcfilename --save-temps
# on mingw32:
#CC=gcc ./make-mingw --email=$email --name="$packager" --source=$srcfilename --save-temps

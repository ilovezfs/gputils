# get_cl_revision.sh
#
# Extraxts the source control revision number from the ChangeLog file.
# the $Revision$ keyword should be in the last line of the file

VERSION=1.0
AS_STRING=0

usage ()
{
  echo "usage: $0 [options] <ChangeLog_file_name>"
  echo "options:"
  echo "  -s, --string          return the revision number as double quoted string"
  echo "  -V, --version         output version information and exit"
  echo "  -h, --help            display this help and exit"
  echo "arguments:"
  echo "  <ChangeLog_file_name> ChangeLog file name"
}

while true
do
  case "$1" in
    -s|--string) AS_STRING=1; shift;;
    -h|--help) usage; exit 0;;
    -V|--version) echo $(basename $0) $VERSION; exit 0;;
    --) shift; break;;
    -*|--*) echo "unknown option $1"; usage; exit 1;;
    ''|*) break;;
  esac
done

if [ "$#" != 1 ]
then
  echo "too many arguments"; usage; exit 1;
else
  REV=$(expr "$(tail -1 $1)" : '\$Revision: *\([0-9]*\) *\$') || REV=0
  if [ "$AS_STRING" != 0 ]
  then
    echo "\"$REV\""
  else
    echo $REV
  fi
fi

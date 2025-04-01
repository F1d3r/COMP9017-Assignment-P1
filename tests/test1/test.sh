DIR=$(readlink -f -- "$0")
ROOT=${DIR%/*/*/*}
TEST=${DIR%/*}
INPUT=${DIR%/*/*}/test.in
OUTPUT=${DIR%/*/*}/test.out

echo hello
echo $DIR
echo $TEST
echo $ROOT
echo $INPUT
echo $OUTPUT
gcc $ROOT/sound_seg.c $TEST/test.c -o program
./program
# chmod u+x ${EXECUTABLE}
# ${EXECUTABLE}
# cat $dest/merk_test.in | $dest/btide | diff -q $dest/merk_test.out -
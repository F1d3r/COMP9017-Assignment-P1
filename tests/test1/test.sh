DIR=$(readlink -f -- "$0")
ROOT=${DIR%/*/*/*}
TEST=${DIR%/*}
INPUT=${DIR%/*/*}/test.in
OUTPUT=${DIR%/*/*}/test.out

if $TEST/program | diff $TEST/test.expected - > /dev/null; then
    echo "Test pass"
else
    echo "Test fail"
fi

DIR=$(readlink -f -- "$0")
ROOT=${DIR%/*/*}

test_num=6

# Compile
cd ${ROOT}
make tests

cd tests
for ((i=1; i<=test_num; i++)); do
    chmod u+x ./test$i/*.sh
    ./test$i/*.sh
done
cd ${ROOT}
make clean_object
make clean_tests
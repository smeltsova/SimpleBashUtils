#!/bin/bash
SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

declare -a tests=(
    "str ./tests/test_1_grep.txt VAR"
    "VAR str ./tests/test_1_grep.txt"
    "-e str -e file ./tests/test_1_grep.txt VAR"
    "-e str -e s ./tests/test_1_grep.txt VAR"
    "-e abc ./tests/test_2_grep.txt VAR"
    "nomatch ./tests/test_2_grep.txt VAR"
    "^str ./tests/test_1_grep.txt VAR"
    ". ./tests/test_1_grep.txt VAR"
    "... ./tests/test_1_grep.txt VAR"
    "for s21_grep.h s21_grep.c Makefile VAR"
    "for s21_grep.c VAR"
    "-f ./tests/test_ptrn_grep.txt ./tests/test_1_grep.txt ./tests/test_2_grep.txt VAR"
)

declare -a extra=(
"-e str no_file.txt"
"-f ./tests/test_ptrn_grep_2.txt ./tests/test_2_grep.txt"
"-f ./tests/test_ptrn_grep_2.txt ./tests/test_2_grep.txt -v"
"-o -e abc -e ab -e a ./tests/test_2_grep.txt"
"-o -e abc -e ab -e b ./tests/test_2_grep.txt"
"-o -e abc -e ab -e c ./tests/test_2_grep.txt"
"-o -e abc -e bc -e a ./tests/test_2_grep.txt"
"-o -e abc -e bc -e b ./tests/test_2_grep.txt"
"-o -e abc -e bc -e c ./tests/test_2_grep.txt"
"-o -e ab -e abc -e a ./tests/test_2_grep.txt"
"-o -e a -e a -e c ./tests/test_2_grep.txt"
"str ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/test_3_grep.txt"
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/test_3_grep.txt"
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/test_3_grep.txt -l"
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/test_3_grep.txt -lc"
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/test_3_grep.txt -c"
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt ./tests/no_file.txt ./tests/test_3_grep.txt"
)


testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")

    ./s21_grep $t > test_s21_grep.log
    grep $t > test_sys_grep.log
    DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
    then
      (( SUCCESS++ ))
      echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[32msuccess\033[0m grep $t"
    else
      (( FAIL++ ))
      echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[31mfail\033[0m grep $t"
    fi
    rm test_s21_grep.log test_sys_grep.log
}

for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in v c l n h o i
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 параметра
for var1 in v c l n h o i
do
    for var2 in v c l n h o i
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

# 3 параметра
for var1 in v c l n h o i
do
    for var2 in v c l n h o i
    do
        for var3 in v c l n h o i
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    testing $i
                done
            fi
        done
    done
done

# 2 сдвоенных параметра
for var1 in v c l n h o i
do
    for var2 in v c l n h o i
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                testing $i
            done
        fi
    done
done

# 3 строенных параметра
for var1 in v c l n h o i
do
    for var2 in v c l n h o i
    do
        for var3 in v c l n h o i
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    testing $i
                done
            fi
        done
    done
done

echo "\033[31mFAIL: $FAIL\033[0m"
echo "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"

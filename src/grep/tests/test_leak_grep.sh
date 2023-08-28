#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
RESULT=0
DIFF_RES=""

declare -a tests=(
"-e str -e file -e 0 ./tests/test_1_grep.txt VAR"
"-f ./tests/test_ptrn_grep.txt ./tests/test_1_grep.txt ./tests/test_2_grep.txt VAR"
)

declare -a extra=(
"-e str no_file.txt"
"./tests/test_1_grep.txt -n"
"nomatch ./tests/test_2_grep.txt"
"-e str -e nomatch -e file -e ab ./tests/test_1_grep.txt ./tests/test_2_grep.txt -o"
"-e str -e nomatch -e file -e ab ./tests/test_1_grep.txt ./tests/test_2_grep.txt -i"
"-e str -e nomatch -e file -e ab ./tests/test_1_grep.txt ./tests/test_2_grep.txt -v"
"-f no_file ./tests/test_1_grep.txt"
"-f ./tests/test_ptrn_grep_2.txt ./tests/test_2_grep.txt"
"-f ./tests/test_ptrn_grep_2.txt ./tests/test_2_grep.txt -v"
"-e abc -e [ ./tests/test_2_grep.txt"
"-e abc -e [ -e a ./tests/test_2_grep.txt"
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
"-e str -e abc ./tests/test_1_grep.txt ./tests/test_2_grep.txt no_file.txt ./tests/test_3_grep.txt"
)


testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    leaks -quiet -atExit -- ./s21_grep $t > test_s21_grep.log
    leak=$(grep -A100000 leaks test_s21_grep.log)
    (( COUNTER++ ))
    if [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
    then
      (( SUCCESS++ ))
        echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[32msuccess\033[0m grep $t"
    else
      (( FAIL++ ))
        echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[31mfail\033[0m grep $t"
    fi
    rm test_s21_grep.log
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

echo "\033[31mFAIL: $FAIL\033[0m"
echo "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"

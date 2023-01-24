numbers = `cat $1 | rev`
lcm=1
for num in $numbers
do 
    a=lcm
    b=$num
    while(($b>0))
    do 
        temp=$b
        b=$(($a%$b))
        a=$temp
    done
    lcm=$((lcm*(num/a)))
done
echo $lcm

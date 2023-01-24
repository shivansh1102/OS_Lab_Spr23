numbers=`cat $1 | rev`
lcm=1
for num in $numbers
do 
    a=$lcm
    b=$num
    while((10#$b>0))
    do 
        temp=$b
        b=$((10#$a%10#$b))
        a=$temp
    done
    lcm=$((lcm*(10#$num/10#$a)))
done
echo $lcm

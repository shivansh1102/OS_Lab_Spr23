findGCD(){ (($2==0))&&gcd=$1||findGCD $2 $(($1%$2));}
lcm=1;while read num;do rev=0;while((num>0));do rev=$((rev*10+num%10));num=$((num/10));done;findGCD $lcm $rev;lcm=$((lcm/gcd*rev));done<lcm.txt;echo $lcm

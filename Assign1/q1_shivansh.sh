findGCD(){ if(($2==0));then gcd=$1;else findGCD $2 $(($1%$2));fi;}
solve(){ rev=0;num=$1;while((num>0));do rev=$((rev*10+num%10));num=$((num/10));done;findGCD $lcm $rev;lcm=$((lcm*rev/gcd));}
lcm=1;while read line;do solve $line;done<lcm.txt;echo $lcm

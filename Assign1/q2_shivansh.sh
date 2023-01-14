substringCheck(){ flag=0;str=$1;while read fruit;do if echo $str|grep -iqF $fruit;then flag=1;fi;done<fruits.txt;if((flag==0));then echo YES;else echo NO;fi;}
solve(){ str=$1;if [[ ${#str} -ge 5&&${#str} -le 20&&$str =~ ^[a-zA-Z][a-zA-Z0-9]*[0-9][a-zA-Z0-9]*$ ]];then substringCheck $str;else echo NO;fi;}
while read line;do solve $line;done<username_sample.txt;


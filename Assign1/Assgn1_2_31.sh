fruits=`cat fruits.txt`
while read str
do 
    ((${#str}>4&&${#str}<21))&&[[ $str =~ ^[a-zA-Z][a-zA-Z0-9]*[0-9][a-zA-Z0-9]*$ ]]&&echo $str|grep -ivqE "${fruits[*]}"&&echo YES||echo NO
done < $1 > validation_results.txt

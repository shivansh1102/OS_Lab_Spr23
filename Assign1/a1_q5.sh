files=`find $1 -type f -name "*.py"`
for file in $files
do 
    printf "\n$file"
    printf "\n****************** Single Line Comments ****************** \n"
    grep -n '\.*[#]\.*' $file
    printf "\n****************** Line numbers of start of multiline comments ****************** \n"
    grep -ne '^[ \t]*"""\.*' -e "^[ \t]*'''\.*" $file | awk -F: '{print $1}'
    printf "\n\n"
done

files=`find $1 -type f -name "*.py"`
for file in $files
do 
    printf "\n$file"
    printf "\n****************** Single Line Comments ****************** \n"
    (cat $file; echo) | while IFS= read -r line; do
        if(grep -q '^[^"]*\("[^"]*"\)*[^"]*#' <<<"$line" && grep -q "^[^']*\('[^']*'\)*[^']*#" <<<"$line");then
            printf "%s\n" "$line"
        fi
    done
    printf "\n****************** Line numbers of start of multiline comments ****************** \n"
    grep -ne '^[ \t]*"""\.*' -e "^[ \t]*'''\.*" $file | awk -F: '{print $1}'
    printf "\n\n"
done

for file in "$1"*.jsonl
do

# Creating csv file name from json file name
    csvFile="$2$(basename "$file" .jsonl).csv"
# From 3rd argument onwards, it will correspond to attributes' names to be extracted
# Replacing ' ' between them by ',' and writing it to csv file, which will correspond to coloums of csv file
    echo "${@:3}" | tr ' ' ',' > "$csvFile" 

# Adding one extra line
    (cat $file; echo)|while read -r line
    do
        if [ "$line" != "" ]; then 
# For each item(attribute), we're extracting its value in json file using "jq"
        echo "$( for item in "${@:3}"
                do 
                echo -n ",$( echo "$line" | jq -r ".$item")" 
# Added a ',' for the first item, so removing it using "cut"
            done | cut -c2- )" >> "$csvFile"; fi
    done
done

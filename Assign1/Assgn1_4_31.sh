(cat $1; echo) | while IFS= read -r line
do
    grep -qw "$2" <<< "$line" && curr=${line,,} && curr=`sed 's/[a-z][^a-z]*[a-z]\?/\u\0/g' <<< "$curr"` || curr="$line"
    printf "%s\n" "$curr"
done

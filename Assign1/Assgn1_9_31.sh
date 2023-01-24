gawk '
{
    students[$1]++majors[$2]++
}
END{
    for(i in majors) print(i,majors[i])>>"temp.txt"
    system("sort -k2nr temp.txt")
    for(i in students){
        if(students[i]>1)print(i)
        else count++
    }
    print(count)
}' $1
rm temp.txt

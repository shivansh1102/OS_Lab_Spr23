gawk 'function key_to_val(i1,v1,i2,v2){
    if(v1>v2)return -1
    if(v1==v2 && i1<i2)return -1
    return 1
}
{
    students[$1]++
    majors[$2]++
}
END{count=0
    PROCINFO["sorted_in"] = "key_to_val"
    for(i in majors) print(i,majors[i])
    for(i in students){
        if(students[i]>1)print(i)
        else count++
    }
    print(count)
}' $1
declare -A deptCount majorCount
while read -a line;do deptCount[${line[1]}]=$((${deptCount[${line[1]}]}+1));majorCount[${line[0]}]=$((${majorCount[${line[0]}]}+1));done<q3_sample.txt
for dept in ${!deptCount[@]};do echo $dept ${deptCount[$dept]}>>q9_output.txt;done;sort -k2nr q9_output.txt;
cntSingleMajor=0;for student in ${!majorCount[@]};do ((${majorCount[$student]}>=2))&&echo $student||cntSingleMajor=$((cntSingleMajor+1));done;echo $cntSingleMajor;

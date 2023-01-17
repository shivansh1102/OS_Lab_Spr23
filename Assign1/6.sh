# !/bin/bash
> output.txt
limit=1000000
numbers=($(seq 2 $limit))
primes=()
for ((i=0; i<${#numbers[@]}; i++)); do
  if [[ ${numbers[$i]} -ne 0 ]]; then
    primes+=(${numbers[$i]})
    for ((j=$(( ${numbers[$i]} * 2 )); j<=limit; j+=${numbers[$i]})); do
      numbers[j-2]=0
    done
  fi
done
# echo "Done"
while IFS= read -r temp; do
    length=${#temp}
    temp=${temp:0:length-1}
    for number in ${primes[@]}; do
        if [[ $number -le $temp ]]; then
            printf $number" " >> output.txt
        else
            break
        fi
    done
    echo >> output.txt
done < input.txt
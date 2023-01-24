# !/bin/bash
>output.txt
limit=1005
numbers=($(seq 2 $limit))
primes=()
# echo ${#numbers[@]}
for ((i=0; i<${#numbers[@]}; i++)); do
  if [[ ${numbers[$i]} -ne 0 ]]; then
    primes+=(${numbers[$i]})
    for ((j=$(( ${numbers[$i]} * 2 )); j<=limit; j+=${numbers[$i]})); do
      numbers[j-2]=0
    done
  fi
done

while IFS= read -r num; do
    length=${#num}
    num=${num:0:length-1}
    length=${#primes[@]}
    for ((i=0; i<$length ; i++)); do
    if [ $num -eq 1 ]; then
      break;
    fi
    if [ $(($num % ${primes[i]})) -eq 0 ]; then
      printf ${primes[i]}" " >> output.txt
    fi
    while [ $(($num % ${primes[i]})) -eq 0 ]; do
        prime_factors[counter]=$i
        num=$(( $num/${primes[i]} ))
    done
    done
    if [ $num -ne 1 ]; then
      printf $num >> output.txt
    fi
    echo >> output.txt
done < input.txt

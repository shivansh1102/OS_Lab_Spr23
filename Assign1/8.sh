# !/bin/bash
touch main.csv;
touch temp.csv;
if [ ! -s main.csv ]; then
    echo "Date,Category,Amount,Name" >> main.csv
fi

if [[ ${@: -4:1} =~ ^[0-9]{2}-[0-9]{2}-[0-9]{2}$ ]]
  then echo ${@: -4:1}","${@: -3:1}","${@: -2:1}","${@: -1} >> main.csv
fi
declare -A C N
while IFS="," read -r date category amount name
do
    C[$category]=$((${C[${category}]}+$amount))
    N[$name]=$((${N[$name]}+$amount))
done < <(tail -n +2 main.csv)
while getopts ":c:n:s:h" opt; do
  case $opt in
    c)echo ${C[$OPTARG]};;
    n)echo ${N[$OPTARG]};;
    s) if [[ $OPTARG == "Category" ]]; then
         tail +1 main.csv | sort -t, -k2d > temp.csv 
      elif [[ $OPTARG == "Amount" ]]; then
          tail +1 main.csv | sort -t, -k3n > temp.csv
      elif [[ $OPTARG == "Name" ]]; then
          tail +1 main.csv | sort -t, -k4d > temp.csv
      else
          sort -t, -k1.7,1.8n -k1.4,1.5n -k1.1,1.2n main.csv > temp.csv 
      fi
       cp temp.csv main.csv
       > temp.csv
      ;;
    h) echo "TOP G ;)";;
    \?)  echo "Invalid option: -$OPTARG" >&2;;
  esac
done
rm temp.csv


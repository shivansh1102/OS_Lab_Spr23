# !/bin/bash
touch main.csv;
> temp.csv;
if [ ! -s main.csv ]; then
    echo "Date,Category,Amount,Name" >> main.csv
fi

if [[ ${@: -4:1} =~ ^[0-9]{2}-[0-9]{2}-[0-9]{2}$ ]]
  then echo ${@: -4:1}","${@: -3:1}","${@: -2:1}","${@: -1} >> main.csv
  echo "Date,Category,Amount,Name" >> temp.csv
  tail +2 main.csv | sort -t, -k1.7,1.8n -k1.4,1.5n -k1.1,1.2n >> temp.csv 
  cp temp.csv main.csv
  > temp.csv
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
    s)
        echo "Date,Category,Amount,Name" >> temp.csv
     if [[ $OPTARG == "Category" ]]; then
         tail +2 main.csv | sort -t, -k2d >> temp.csv 
      elif [[ $OPTARG == "Amount" ]]; then
          tail +2 main.csv | sort -t, -k3n >> temp.csv
      elif [[ $OPTARG == "Name" ]]; then
          tail +2 main.csv | sort -t, -k4d >> temp.csv
      else
          tail +2 main.csv | sort -t, -k1.7,1.8n -k1.4,1.5n -k1.1,1.2n >> temp.csv 
      fi
       cp temp.csv main.csv
       > temp.csv
      ;;
    h) 
echo ".TH Assgn1_8_31.sh 1
.SH NAME
\fBAssgn1_8_31.sh \fR\- Script to track expenses and sort csv file
.SH SYNOPSIS
.B sh Assgn1_8_31.sh
[ -c Category ]
[ -n name ]
[ -s column ]
[ -h ]
Date
Category
Amount
Name
.SH DESCRIPTION
.B test.sh
Script essentially manipulates a csv file.
The script accepts 4 arguments Date(dd-mm-yy), Category, Amount, Name and adds the record into the main.csv file.

.SH OPTIONS
.TP
.BI \-c \" Category\"
Prints the amount of money spent in that Category
.TP
.BI \-n \" name\"
Print the amount spent by the person with given name
.TP
.BI \-s \" column\"
Sort the csv by Column name
.TP
.BR -h
Shows the help prompt" > manual_page.1
man ./manual_page.1
rm manual_page.1
    ;;
    \?)  echo "Invalid option: -$OPTARG" >&2;;
  esac
done
rm temp.csv

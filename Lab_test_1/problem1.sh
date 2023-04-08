x=$1
y=$2
m=$3
if ((x <= 0 || y <= 0 || m <= 0))
then
	echo "Invalid input"
else
	res=$((y*y))
	res=$((res%m))
	res=$((x**res))
	res=$((res%m))
	echo $res
fi

files=`find $1 -type f -name "*.txt"`
mkdir -p $2
for x in {A..Z}
do
    for file in $files
    do 
        grep "^$x\|^${x,,}" $file >> "$2/$x.txt"
    done
    sort -o "$2/$x.txt"{,}
done

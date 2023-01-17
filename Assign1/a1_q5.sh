files=`find $1 -type f -name "*.py"`;for file in $files;do echo "$file";grep -n '\.*[#]\.*' $file;grep -ne '^[ \t]*"""\.*' -e "^[ \t]*'''\.*" $file|awk -F: '{print $1}';done

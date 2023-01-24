files=`find $1 -type f -name "*.py"`
for file in $files
do
    echo $file
    printf "\n****************** Single Line Comments ****************** \n"
    gawk '
    {
        if (/^[^"]*("[^"]*")*[^"]*#/)
        {
            if (/^[^\047]*(\047[^\047]*\047)*[^\047]*#/)
            {
                print NR ": " $0 ; next;
            } next;
        }
        if (/^[^\047]*(\047[^\047]*\047)*[^\047]*#/)
        {
            if (/^[^"]*("[^"]*")*[^"]*#/)
            {
                print NR ": " $0 ; next;
            } next;
        }
    }' $file
    printf "\n"

    printf "\n****************** Line numbers of start of multiline comments ****************** \n"
    gawk '
    BEGIN{
        d_start = 0;
    }
    {
        if (/^[ \t]*"""/ && d_start == 0)
        {
            d_start = 1;
            print NR;
            if (/^[ \t]*"""(["][ ]|[""][ ]|[^"])*"""/){
                d_start = 0;
            }
            next;
        }
        if(d_start==1 && /.*"""[ \t]*$/){
            d_start=0;
            next;
        }
    }
    ' $file
    gawk '
    BEGIN{
        d_start = 0;
    }
    {
        if (/^[ \t]*\047\047\047/ && d_start == 0)
        {
            d_start = 1;
            print NR;
            if (/^[ \t]*\047\047\047([\047][ ]|[\047\047][ ]|[^\047])*\047\047\047/){
                d_start = 0;
            }
            next;
        }
        if(d_start==1 && /.*\047\047\047[ \t]*$/){
            d_start=0;
            next;
        }
    }
    ' $file
    printf "\n"
done

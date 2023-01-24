files=`find $1 -type f -name "*.py"`
for file in $files
do
    echo $file
    gawk '
    BEGIN{
        s_start=0;
        d_start=0;
    }
    {
        if(d_start==0){
            if(s_start==0){
                if(/^[ \t]*["]["]["].*["]["]["][ \t]*$/){
                    print NR ": " $0 ;
                    next;
                }
                if(/^[ \t]*[\047][\047][\047].*[\047][\047][\047][ \t]*$/){
                    print NR ": " $0 ;
                    next;
                }
                if(/^[ \t]*["]["]["]/){
                    print NR ": " $0 ;
                    d_start=1;
                    next;
                }
                if(/^[ \t]*[\047][\047][\047]/){
                    print NR ": " $0 ;
                    s_start=1;
                    next;
                }
            }
            else{
                print $0
                if(/[\047][\047][\047][ \t]*/){
                    s_start=0;
                    next;
                }
                next;

            }
            if (/^[^"]*("[^"]*")*[^"]*#/)
            {
                if (/^[^\047]*(\047[^\047]*\047)*[^\047]*#/)
                {
                    print NR ": " $0 ; 
                    next;
                } 
                next;
            }
            if (/^[^\047]*(\047[^\047]*\047)*[^\047]*#/)
            {
                if (/^[^"]*("[^"]*")*[^"]*#/)
                {
                    print NR ": " $0 ; 
                    next;
                } 
                next;
            }
        }
        else{
            print $0;
            if(/["]["]["][ \t]*$/){
                d_start=0;
                next;
            }
        }
        
    }
    ' $file
    printf "\n"
done

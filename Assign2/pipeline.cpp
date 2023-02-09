#include "pipeline.hpp"

Pipes::Pipes(string &command) : fullCmd(trim(command)), individualCmds(getAllPipes(fullCmd)), pgrpID(0) 
{
    if(fullCmd.back() == '&') isBackground = true;
    else isBackground = false;
    countActive = individualCmds.size();
}

Pipes::~Pipes()
{
    fullCmd.clear();
    individualCmds.clear();
}

void Pipes::executePipes()
{
    int fd[2]; // the file descriptor
    int st_in = 0, st_out = 1; // the descriptor for stdin and stdout

    blockSIGCHLD();             // block SIGCHLD to handle the race condition

    int n = individualCmds.size();
    bool noChild = true;        // if no fork() is done, then true
    for (int i = 0; i < n-1; i++)
    {
        if (pipe(fd) < 0) // creating pipe
        {
            perror("error in creating pipe\n");
            exit(1);
        }

        vector<vector<string>> tokens = getAllVectoredTokens(individualCmds[i]); // tokenisation of command
        noChild &= runIndividualCmd(tokens, st_in, fd[1], i); // executing the command
        if(st_in != 0)
        close(st_in);
        st_in = fd[0]; // updating the input file descriptor
        close(fd[1]); // closing old pipe   
    }

    vector<vector<string> > tokens = getAllVectoredTokens(individualCmds[n-1]);

    noChild &= runIndividualCmd(tokens, st_in, st_out, n-1);

    // cout << pgrpID << endl;
            

    // If process is a background process, then it should not wait
    if(isBackground)
    unblockSIGCHLD();
    else
    {
        if(!noChild)                    // if no fork() is done, then no need to wait for any child
        waitForForegroundProcess(pgrpID);
    
        // If ctrl+z is pressed, process was stopped using SIGSTOP, so continuing it in background
        if(isBackground){
            killpg(pgrpID, SIGCONT);
        }
    }

    tcsetpgrp(STDIN_FILENO, getpid());  // Give control of stdin back to the shell
}
// return true when it didn't fork() like for cd
bool Pipes::runIndividualCmd(vector<vector<string> > &cmdTokens, int prev_out, int next_in, int idx)
{
    bool write_to_file_flag = false, read_from_file_flag = false; int n = 0;
    string write_to_file, read_from_file;
    for (int i = 0; i < cmdTokens.size(); ++i)
    {
        if (i < cmdTokens.size() - 1)
        {
            if (cmdTokens[i][0] == ">")
            {
                write_to_file = cmdTokens[i + 1][0];
                write_to_file_flag = true;
            }
            if (cmdTokens[i][0] == "<")
            {
                read_from_file = cmdTokens[i + 1][0];
                read_from_file_flag = true;
            }
        }
        if (!read_from_file_flag && !write_to_file_flag && cmdTokens[i][0]!="&")
        n += cmdTokens[i].size();
    }
    char *command[n + 1];
    int j = 0, l = 0;
    for (int i = 0; i < cmdTokens.size() && j < n; ++i)
    {
        for (int k = 0; k < cmdTokens[i].size(); ++k)
        {
            command[j++] = const_cast<char *>(cmdTokens[i][k].c_str());
        }
    }
    command[n] = NULL;

    if (strcmp(command[0], "cd") == 0)             // handling "cd" : it will be done by parent itself, no need to fork()
    {
        int x;
        if (command[1] == NULL)
        x = chdir(getenv("HOME"));
        else
        x = chdir(command[1]);

        if(x<0){
            cout<<strerror(errno)<<endl;
        }
        return true;
    }
    else if (cmdTokens[0][0] == "delep")
    {
        delep(cmdTokens);
        return true;
    }
    
    int childpid = fork();
    if(childpid < 0)
    {
        perror("Error in forking!\n");
        exit(1);
    }

    if (childpid == 0)                                  // child process
    {
        // unblocking SIGCHLD signal for child as it itself can become parent later
        unblockSIGCHLD();   

        // Revert signal handlers to default behaviour 
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        if(idx == 0)
        setpgrp();                              // Making pid of first child as process group ID
        else
        setpgid(0, pgrpID);                     // set process group ID of this child as pgrpID
        
        // Give control of stdin to the child process so that it can receive signal from keyboard
        if(idx == 0)
        tcsetpgrp(STDIN_FILENO, pgrpID);

        
        // writing
        dup2(prev_out, 0);
        dup2(next_in, 1);
        if (prev_out != 0)
            close(prev_out);
        if (next_in != 1)
            close(next_in);
        

        if (write_to_file_flag)
        {
            next_in = open(write_to_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            dup2(next_in, 1);
            close(next_in);
        }
        // reading
        if (read_from_file_flag)
        {
            prev_out = open(read_from_file.c_str(), O_RDONLY, 0644);
            dup2(prev_out, 0);
            close(prev_out);
        }
        if (command[0] == NULL) _exit(0);
        // handling malware detection
        else if(strcmp(command[0],"sb")==0){
            if(command[1]==NULL){
                cout<<"No argument provided\n";
                _exit(0);
            }
            else if(strcmp(command[1], "-s")==0){
                if(command[2]==NULL){
                    cout<<"No argument provided"<<endl;
                    _exit(0);
                }
                else{
                    if(command[3]==NULL){
                        fasterMalware(atoi(command[2]), 0);
                        _exit(0);
                    }
                    else{
                        if(strcmp(command[3], "-suggest")==0){
                            fasterMalware(atoi(command[2]), 0);
                            malwareDetect(atoi(command[2]));
                            _exit(0);
                        }
                        cout<<"Invalid argument\n";
                        _exit(0);
                    }
                }
            }
            else{
                if(command[2]==NULL){
                    fasterMalware(atoi(command[1]), 0);
                    _exit(0);
                }
                else{
                    if(strcmp(command[2], "-suggest")==0){
                        fasterMalware(atoi(command[1]), 1);
                        _exit(0);
                    }
                }
            }
        }
        else
        {
            if(execvp(command[0], command) < 0)
            cout << "O bhai bhai bhai... "<<strerror(errno) << endl;  // if some error then print it
        }
        // If some error occurs in doing execvp(), then exit with errno
        _exit(0);     
    }
    else                               // parent process
    {
        if(idx == 0)
        {
            allPipes.push_back(this);
            this->pgrpID = childpid;          // Making pid of first child as process group ID
            setpgid(childpid, pgrpID);  // set process group ID of this child as pgrpID

            // Reference: https://web.archive.org/web/20170701052127/https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lab/10/lab.html
            // Give control of stdin to the running processes so that it can receive signal from keyboard
            tcsetpgrp(STDIN_FILENO, pgrpID);
        }
        else
        setpgid(childpid, pgrpID);      // set process group ID of this child as pgrpID

        pipeIndexMap[childpid] = (int)allPipes.size()-1;

    }
    return false;
}

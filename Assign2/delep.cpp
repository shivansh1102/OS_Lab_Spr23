#include "delep.hpp"


string absolute_file_path;
vector<pid_t> pid_list_req;

void signal_handler_for_delep(int signal_number)
{

    for (pid_t pid : pid_list_req)
        kill_given_process(pid);

    if (unlink(absolute_file_path.c_str()) == -1)
        perror("unlink");
    exit(0);
}

void kill_given_process(pid_t pid)
{
    if (kill(pid, SIGKILL) == -1)
        perror("kill");
}

void get_from_child(int f)
{
    char buf[BUF_SIZE];

    while (read(f, buf, BUF_SIZE) > 0)
    {
        cout << buf << '\n';
        pid_list_req.push_back(atoi(buf));
    }
}

void util_delep(vector<vector<string> > &v)
{
    absolute_file_path = v[0][1];
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir)
    {
        cout << "There is no such directory" << endl;
        exit(1);
    }
    for (dirent *ent_l_1 = readdir(proc_dir); ent_l_1 != nullptr; ent_l_1 = readdir(proc_dir))
    {
        // cout<<ent_l_1->d_ino<<" "<<ent_l_1->d_name<<" "<<ent_l_1->d_off<<" "<<ent_l_1->d_reclen<<" "<<ent_l_1->d_type<<endl;
        if (!isdigit(ent_l_1->d_name[0]))
            continue;

        pid_t pid = static_cast<pid_t>(stol(ent_l_1->d_name));
        string temp1="/proc/",temp2 = to_string(pid),temp3="/fd";
        string temp = temp1+temp2+temp3;
        DIR *fd_dir = opendir(temp.c_str());
        if (!fd_dir)
            continue;
        for (dirent *ent_l_2 = readdir(fd_dir); ent_l_2 != nullptr; ent_l_2 = readdir(fd_dir))
        {
            // cout<<ent_l_2->d_ino<<" "<<ent_l_2->d_name<<" "<<ent_l_2->d_off<<" "<<ent_l_2->d_reclen<<" "<<ent_l_2->d_type<<endl;
            if (ent_l_2->d_name[0] == '.')
                continue;


            string t1="/proc/",t2=to_string(pid),t3="/fd/",t4(ent_l_2->d_name);
            string t = t1+t2+t3+t4;
            char buf[REC_BUF_SIZE];
            ssize_t len = readlink(t.c_str(), buf, sizeof(buf));
            // cout<<t<<"\t"<<buf<<endl;
            if (len != -1)
            {
                buf[len] = '\0';
                if (buf == absolute_file_path)
                {
                    pid_list_req.push_back(pid);
                    // cout << ent_l_1->d_ino << " " << ent_l_1->d_name << " " << ent_l_1->d_off << " " << ent_l_1->d_reclen << " " << ent_l_1->d_type << endl;
                    // cout << ent_l_2->d_ino << " " << ent_l_2->d_name << " " << ent_l_2->d_off << " " << ent_l_2->d_reclen << " " << ent_l_2->d_type << endl;
                }
            }
        }

        closedir(fd_dir);
    }

    closedir(proc_dir);
}

void delep(vector<vector<string>> &cmd)
{
    if (cmd[0].size() != 2)
    {
        cerr << "Usage: delep <absolute path>\n";
        return;
    }
    vector<vector<string>> v;
    int p[2];
    vector<string> temp;
    temp.push_back("delep");
    temp.push_back(cmd[0][1]);
    v.push_back(temp);

    if (pipe(p) < 0)
    {
        perror("error in creating a pipe\n");
        exit(1);
    }
    pid_list_req.clear();
    int pd = fork();
    if (pd < 0)
    {
        perror("error in forking\n");
        exit(1);
    }
    if (pd == 0)
    {
        // start of child
        util_delep(v);
        
        cout << "Processes holding a lock or have the file open:\n";
        for (pid_t pid : pid_list_req)
        {
            char buf[20];
            sprintf(buf, "%d", pid);
            write(p[1], buf, 20);
            // cout << pid << '\n';
        }
        pid_list_req.clear();
        close(p[1]);
        exit(0);
    }
    close(p[1]);

    // wait(NULL);
    get_from_child(p[0]);
    waitpid(pd,NULL,0);
    cout << "Kill these processes and delete the file (y/n)? ";
    string response;
    cin >> response;

    if (response == "y")
    {
        pd = fork();
        if (pd == 0)
        {
            for (pid_t pid : pid_list_req)
                kill_given_process(pid);

            pid_list_req.clear();
            if (unlink(v[0][1].c_str()) == -1)
                perror("unlink");
            exit(0);
        }
        pid_list_req.clear();
        waitpid(pd,NULL,0);
    }
}

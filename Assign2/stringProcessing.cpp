#include "stringProcessing.hpp"
#include "signalHandler.hpp"


string trim(string &s)
{
    string temp = "";
    int i = 0;
    int j = s.size() - 1;
    for (i = 0; i < s.length(); ++i)
    {
        if (s[i] != ' ')
            break;
    }
    for (j = s.length() - 1; j >= 0; --j)
    {
        if (s[j] != ' ')
            break;
    }
    return s.substr(i, j - i + 1);
}

vector<vector<string>> getAllTokens(string &command)
{

    vector<vector<string>> allPipes;
    vector<string> pipe;
    string temp = "";
    int doubleQuotes = 0;
    int singleQuotes = 0;

    for (int i = 0; i < command.length(); ++i)
    {
        if (command[i] == '"')
        {
            doubleQuotes = !doubleQuotes;
            temp += command[i];
        }
        else if (command[i] == '\'')
        {
            singleQuotes = !singleQuotes;
            temp += command[i];
        }
        else if (command[i] == '\\')
        {
            temp += command[i + 1];
            i++;
            continue;
        }
        else if (command[i] == ' ' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipe.push_back(trim(temp));
            temp = "";
        }
        else if (command[i] == '>' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipe.push_back(trim(temp));
            pipe.push_back(">");
            temp = "";
        }
        else if (command[i] == '<' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipe.push_back(trim(temp));
            pipe.push_back("<");
            temp = "";
        }
        else if (command[i] == '&' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipe.push_back(trim(temp));
            pipe.push_back("&");
            temp = "";
        }
        else if (command[i] == '|' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipe.push_back(trim(temp));
            temp = "";
            if (pipe.size())
                allPipes.push_back(pipe);
            pipe.clear();
        }
        else
            temp += command[i];
    }

    if (trim(temp) != "")
    {
        pipe.push_back(trim(temp));
        allPipes.push_back(pipe);
    }

    return allPipes;
}

vector<string> getAllPipes(string &command)
{

    vector<string> pipes;
    string temp = "";

    int doubleQuotes = 0;
    int singleQuotes = 0;

    for (int i = 0; i < command.length(); ++i)
    {
        if (command[i] == '"')
        {
            doubleQuotes = !doubleQuotes;
            temp += command[i];
        }
        else if (command[i] == '\'')
        {
            singleQuotes = !singleQuotes;
            temp += command[i];
        }
        else if (command[i] == '\\')
        {
            temp += command[i + 1];
            i++;
            continue;
        }
        else if (command[i] == '|' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                pipes.push_back(trim(temp));
            temp = "";
        }
        else
            temp += command[i];
    }

    if (trim(temp) != "")
    {
        pipes.push_back(trim(temp));
    }

    return pipes;
}

vector<string> getAllCmds(string &segmentCmd)
{

    int doubleQuotes = 0;
    int singleQuotes = 0;

    vector<string> cmds;
    string temp = "";
    for (int i = 0; i < segmentCmd.length(); ++i)
    {
        if (segmentCmd[i] == '"')
        {
            doubleQuotes = !doubleQuotes;
            temp += segmentCmd[i];
        }
        else if (segmentCmd[i] == '\'')
        {
            singleQuotes = !singleQuotes;
            temp += segmentCmd[i];
        }
        else if (segmentCmd[i] == '\\')
        {
            i++;
            temp += segmentCmd[i];
            continue;
        }
        else if (segmentCmd[i] == '&' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                cmds.push_back(trim(temp));
            cmds.push_back("&");
            temp = "";
        }
        else if (segmentCmd[i] == '<' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                cmds.push_back(trim(temp));
            cmds.push_back("<");
            temp = "";
        }
        else if (segmentCmd[i] == '>' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                cmds.push_back(trim(temp));
            cmds.push_back(">");
            temp = "";
        }
        else if (segmentCmd[i] == ' ' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
                cmds.push_back(trim(temp));
            temp = "";
        }
        else
            temp += segmentCmd[i];
    }

    if (trim(temp) != "")
        cmds.push_back(trim(temp));


    return cmds;
}

void handleRegex(vector<string> &cmd, string &temp)
{

    glob_t glob_result;
    int glob_ret;
    glob_ret = glob(temp.c_str(), 0, NULL, &glob_result);

    int n = glob_result.gl_pathc;
    for (int i = 0; i < n; i++)
    {
        cmd.push_back(glob_result.gl_pathv[i]);
    }
    temp = "";
}

vector<vector<string>> getAllVectoredTokens(string &commands)
{

    vector<vector<string>> allCmds;
    vector<string> cmds;
    string temp = "";
    int doubleQuotes = 0;
    int singleQuotes = 0;
    int tokenContainsReg = 0;

    for (int i = 0; i < commands.length(); ++i)
    {
        if (commands[i] == '"')
        {
            doubleQuotes = !doubleQuotes;
            temp += commands[i];
        }
        else if (commands[i] == '\'')
        {
            singleQuotes = !singleQuotes;
            temp += commands[i];
        }
        else if (commands[i] == '\\')
        {
            if (commands[i + 1] == '*' || commands[i + 1] == '?')
            {
                tokenContainsReg = 1;
            }
            temp += commands[i + 1];
            i++;
            continue;
        }
        else if (commands[i] == ' ' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
            {
                if (tokenContainsReg)
                    handleRegex(cmds, temp);
                else
                {
                    if ((temp[0] == '\"' && temp.back() == '\"') || (temp[0] == '\'' && temp.back() == '\''))
                    {
                        temp = temp.substr(1, temp.size() - 2);
                    }
                    cmds.push_back(temp);
                }
                tokenContainsReg = 0;
            }
            temp = "";
        }
        else if (commands[i] == '>' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
            {
                if (tokenContainsReg)
                    handleRegex(cmds, temp);
                else
                {
                    if ((temp[0] == '\"' && temp.back() == '\"') || (temp[0] == '\'' && temp.back() == '\''))
                    {
                        temp = temp.substr(1, temp.size() - 2);
                    }
                    cmds.push_back(temp);
                }
                tokenContainsReg = 0;
            }
            if (cmds.size())
                allCmds.push_back(cmds);
            cmds.clear();
            allCmds.push_back({">"});
            temp = "";
        }
        else if (commands[i] == '<' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
            {
                if (tokenContainsReg)
                    handleRegex(cmds, temp);
                else
                {
                    if ((temp[0] == '\"' && temp.back() == '\"') || (temp[0] == '\'' && temp.back() == '\''))
                    {
                        temp = temp.substr(1, temp.size() - 2);
                    }
                    cmds.push_back(temp);
                }
                tokenContainsReg = 0;
            }
            if (cmds.size())
                allCmds.push_back(cmds);
            cmds.clear();
            allCmds.push_back({"<"});
            temp = "";
        }
        else if (commands[i] == '&' && !doubleQuotes && !singleQuotes)
        {
            if (trim(temp) != "")
            {
                if (tokenContainsReg)
                    handleRegex(cmds, temp);
                else
                {
                    if ((temp[0] == '\"' && temp.back() == '\"') || (temp[0] == '\'' && temp.back() == '\''))
                    {
                        temp = temp.substr(1, temp.size() - 2);
                    }
                    cmds.push_back(temp);
                }
                tokenContainsReg = 0;
            }
            if (cmds.size())
                allCmds.push_back(cmds);
            cmds.clear();
            allCmds.push_back({"&"});
            temp = "";
        }
        else
        {
            temp += commands[i];
            if ((commands[i] == '*' || commands[i] == '?') && !doubleQuotes && !singleQuotes)
            {
                tokenContainsReg = 1;
            }
        }
    }

    if (trim(temp) != "")
    {
        if (tokenContainsReg)
            handleRegex(cmds, temp);
        else
        {
            if ((temp[0] == '\"' && temp.back() == '\"') || (temp[0] == '\'' && temp.back() == '\''))
            {
                temp = temp.substr(1, temp.size() - 2);
            }
            cmds.push_back(temp);
        }
        tokenContainsReg = 0;
        allCmds.push_back(cmds);
    }

    return allCmds;
}


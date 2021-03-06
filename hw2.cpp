#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <vector>	
#include <sys/wait.h>

using namespace std;
//in file descriptor
//fd0 =read
//fd1 = write

int PIPE_READ = 0;
int PIPE_WRITE = 1;

void exit(){
	int PID, status;
		while ((PID = wait(&status)) != -1) // prints exit status
			cout << "Process " << PID << " exits with " << WEXITSTATUS(status);	

		//exit(&status); this is what he said we should use but i couldn't get it to work. WEXITSTATUS does the same thing though
	}


void getCommands(string commandList, vector<string>& commands) {
	int lastPipe = 0;
	int inQuote = false;
	for(int i = 0; i < (int)commandList.length(); i++) {
		if(commandList[i] == '|' && !inQuote) {
			commands.push_back(commandList.substr(lastPipe, i - lastPipe));
			lastPipe = i + 1;
		} else if (commandList[i] == '\"' || commandList[i] == '\'') {
			inQuote = !inQuote;
		}
	}
	commands.push_back(commandList.substr(lastPipe, commandList.length() - lastPipe));
}

void getTokens(vector<string>& commands, vector<vector<string>>& tokens) {
	int lastCommand = 0;
	int inQuote = false;
	
	for(string command : commands) {
		tokens.push_back(vector<string>());
		lastCommand = 0;
		inQuote = false;
		for(int i = 0; i < (int)command.length(); i++) {
			if((command[i] == ' ' || command[i] == 9) && !inQuote) {
				if(lastCommand != i) {
					tokens.back().push_back(command.substr(lastCommand, i - lastCommand));
				}
				lastCommand = i + 1;
			} else if (command[i] == '\"' || command[i] == '\'') {
				inQuote = !inQuote;
			}
		}
		if(lastCommand != (int)command.length()) {
			tokens.back().push_back(command.substr(lastCommand, command.length() - lastCommand));
		}
	}
}

void createPipes(int fd[][2], int numPipes) {
	for(int i = 0; i < numPipes; i++) {
		pipe(fd[i]);
	}
}

int createChildren(int numChildren) {
	int childNum = -1;
	for(int i = 0; i < numChildren; i++) {
		int cpid  = fork();
		if(cpid == 0) {
			childNum = i;
			break;
		}
	}
	return childNum;
}

void closePipes(int childNum, int fd[][2], int numPipes) {
	for(int i = 0; i < numPipes; i++) {
		if(childNum == i) {
			//cout << "child " << childNum << " closing read" << endl;
			close(fd[i][PIPE_READ]);
		} else if (childNum == i + 1 && childNum > 0) {
			//cout << "child " << childNum << " closing write" << endl;
			close(fd[i - 1][PIPE_WRITE]);
		} else {
			//cout << "child " << childNum << " closing both" << endl;
			close(fd[i][PIPE_READ]);
			close(fd[i][PIPE_WRITE]);
		}
	}
}

void linkPipes(int childNum, int fd[][2], int numPipes) {
	if(childNum < numPipes) {
		dup2(fd[childNum][PIPE_WRITE], PIPE_WRITE);
	}
	if(childNum > 0) {
		dup2(fd[childNum - 1][PIPE_READ], PIPE_READ);
	}
}

int main(){

	string command;
	string input;
	getline(cin, input);
	command = input;
	int fd[9][2];
	int childNum;
	vector<string> commands;
	getCommands(command, commands);
	vector<vector<string>> tokens;
	getTokens(commands, tokens);
	
	for(vector<string> t : tokens) {
		for(string s : t) {
			cout << s << " ";
		}
		cout << endl;
	}
	
	createPipes(fd, tokens.size() - 1);
	childNum = createChildren(tokens.size());
	if(childNum >= 0) {
		closePipes(childNum, fd, tokens.size() - 1);
		linkPipes(childNum, fd, tokens.size() - 1);
		char* args[50];
		for(int i = 0; i < (int)tokens.at(childNum).size(); i++)
			args[i] = (char*)(tokens.at(childNum).at(i).c_str());
		args[tokens.at(childNum).size()] = (char*)NULL;
		
		execvp(args[0], args);
	} else {
		closePipes(childNum, fd, tokens.size() - 1);
		exit();
		}
}

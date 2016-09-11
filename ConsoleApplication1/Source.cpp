#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <cstdio>

#include "Stack.h"

void shell();
void sandbox();

int main()
{
	try
	{
		sandbox();
	}

	catch (exc* msg)
	{
		msg->what();
		delete msg;
	}

	return 0;
}

void shell()
{
	bool terminate = false;
	stack<int> a;

	char inputStr[16] = {};
	char* command = NULL;
	char* param = NULL;

	while (!terminate)
	{
		try
		{
			fgets(inputStr, 16, stdin);
			char* command = strtok(inputStr, " \n");
			if (command)
			{
				if (strcmp(command, "dump") == 0) a.dump();
				if (strcmp(command, "push") == 0)
				{
					param = strtok(NULL, " ");
					if (!param) throw new exc("Incorrect use of push", __LINE__, __FILE__);
					else a.push(atoi(param));
				}
				if (strcmp(command, "pop") == 0) printf("%d\n", a.pop());
				if (strcmp(command, "kill") == 0) terminate = true;
				if (strcmp(command, "destruct") == 0) a.~stack();
			}
			else throw new deepExc_("Empty string to command!");
		}
		
		catch (exc* msg)
		{
			msg->what();
			delete msg;
		}
	}
}

void sandbox()
{
	stack<int> a;
	stack<int> b;
	
	b.push(1);
	b.push(2);
	b.push(3);
	b.dump();

	a = b;
	b.~stack();
	a.dump();

	while (!GetAsyncKeyState(VK_ESCAPE));
}
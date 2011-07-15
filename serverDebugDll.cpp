//***********************************
//		HEADERS
//***********************************
//-----------------------------------
//Standard Headers
//-----------------------------------
#include <Windows.h>
#include <iostream>

//-----------------------------------
//Project Headers
//-----------------------------------
#include "serverDebugDll.h"

using namespace std;

debugDll* this_instance = NULL;
//***********************************
//	TRAP SIGNALS
//***********************************
void global_signal_trap(int signal)
{
	if( this_instance != NULL)
	{
		this_instance->signal_trap(signal);
	}
}
void debugDll::signal_trap(int signal)
{ 
	stop();
}
//***********************************
//END OF TRAP SIGNALS
//***********************************

void debugDll::setConsoleScreen() 
{
    // Set up the handles for reading/writing:
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    rHnd = GetStdHandle(STD_INPUT_HANDLE);

    // Change the window title:
    //SetConsoleTitle((LPCWSTR)"Win32 Console Control Demo");

    // Set up the required window size:
    SMALL_RECT windowSize = {0, 0, CONSOLE_WIDTH-1, CONSOLE_HEIGHT-1};
    
    // Change the console window size:
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

	// Create a COORD to hold the buffer size:
    COORD bufferSize = {CONSOLE_WIDTH, CONSOLE_HEIGHT};
	
    // Change the internal buffer size:
    SetConsoleScreenBufferSize(wHnd, bufferSize);

	fill(wHnd,CONSOLE_WIDTH,CONSOLE_HEIGHT);
}

void debugDll::fill(HANDLE hConsoleOutput, int width, int height)
{
	LPDWORD num_written=0;
	COORD write_pos={0,0};

	for(int i=0;i<width;i++)
	{
		write_pos.X=i;
		for(int j=0; j<height; j++)
		{
			write_pos.Y =j;
			FillConsoleOutputCharacter(hConsoleOutput,' ',1,write_pos,num_written);
		}
	}
}
debugDll::debugDll()
{
    //init
	this_instance = this;
	CONSOLE_WIDTH = 80;
	CONSOLE_HEIGHT = 50;

	down_mutex = CreateMutex (NULL, FALSE, NULL);
	up_mutex = CreateMutex (NULL, FALSE, NULL);
	cmd_mutex = CreateMutex (NULL, FALSE, NULL);
	//alloc
}

debugDll::~debugDll()
{
    destroy();
	this_instance = NULL;
}

void debugDll::destroy()
{
	stop();

	//dealloc
	while(out_msgQueue.size() > 0)
	{
		delete out_msgQueue.front();
		out_msgQueue.pop();
	}

	while(in_msgQueue.size() > 0)
	{
		delete in_msgQueue.front();
		in_msgQueue.pop();
	}

	while(cmd_msgQueue.size() > 0)
	{
		delete cmd_msgQueue.front();
		cmd_msgQueue.pop();
	}
}
void debugDll::stop()
{
    running = false;
	addCmdMessage(CMD_STOP);
}
void debugDll::start()
{
    running = true;
    
	//signal capture
	signal(SIGTERM, global_signal_trap);
	signal(SIGINT, global_signal_trap);

	setConsoleScreen();
	char command = '\0';
    while(running)
    {
        cout<<"Running..."<<endl;
        Sleep(1000);
		handleQueuedMessages();
		addMessage();

		cout<<"[C]onnect  "<<endl;
		cout<<"[D]isconnet"<<endl;
		cout<<"[N]etwork  "<<endl;
		cout<<"[R]un"<<endl;
		cout<<"Enter Command: "<<endl;
		cin>>command;
		switch (command)
		{
			case 'c':
			case 'C':
				addCmdMessage(CMD_CONNECT);
			break;
			case 'd':
			case 'D':
				addCmdMessage(CMD_DISCONNECT);
			break;
			case 'n':
			case 'N':
				addCmdMessage(CMD_NETWORK);
			break;
			case 'r':
			case 'R':
			default:
			break;
		}

    }

	cout<<"Stopping..."<<endl;
	addMessage();

}
bool debugDll::hasMessagesToSend()
{
	int size  = 0;
	WaitForSingleObject( down_mutex, INFINITE );
	size = out_msgQueue.size();
	ReleaseMutex( down_mutex );
	return size > 0;
	
}
void debugDll::addMessage()
{
	WaitForSingleObject( down_mutex, INFINITE );

	messageQueueStruct *debugMessage = new messageQueueStruct();

	debugMessage->hdr.size = 5;

    debugMessage->message[0] = 'p';
    debugMessage->message[1] = 'i';
    debugMessage->message[2] = 'n';
    debugMessage->message[3] = 'g';
    debugMessage->message[4] = '\0';

	out_msgQueue.push(debugMessage);

	ReleaseMutex( down_mutex );
}

messageQueueStruct* debugDll::getNextSend()
{
	messageQueueStruct *debugMessage = NULL;
	WaitForSingleObject( down_mutex, INFINITE );

    debugMessage = out_msgQueue.front();
    
	cout<<"Sending "<<" - "<< debugMessage->message<<endl;
	
	out_msgQueue.pop();
    ReleaseMutex( down_mutex );

	return debugMessage;
	
}
void debugDll::messageHandler(char* buffer, int size)
{
    cout<<"Message Parsed - Length "<<size<<endl;
	
	WaitForSingleObject( up_mutex, INFINITE );
	in_msgQueue.push(buffer);
	ReleaseMutex( up_mutex );
}
void debugDll::handleQueuedMessages()
{
	WaitForSingleObject( up_mutex, INFINITE );
	while( in_msgQueue.size() > 0 )
	{
		cout<<in_msgQueue.front()<<endl;
		delete in_msgQueue.front();
		in_msgQueue.pop();
	}
	ReleaseMutex( up_mutex );
}

bool debugDll::hasCmdMesssages()
{
	int size  = 0;
	WaitForSingleObject( cmd_mutex, INFINITE );
	size = cmd_msgQueue.size();
	ReleaseMutex( cmd_mutex );
	return size > 0;
}

CmdMessage* debugDll::getNextCmd()
{
	CmdMessage *debugMessage = NULL;
	WaitForSingleObject( cmd_mutex, INFINITE );

    debugMessage = cmd_msgQueue.front();
	
	cmd_msgQueue.pop();
    ReleaseMutex( cmd_mutex );

	return debugMessage;
}

void debugDll::addCmdMessage(CMD_MESSAGE msg)
{
	WaitForSingleObject( down_mutex, INFINITE );

	CmdMessage *debugMessage = new CmdMessage();

	debugMessage->msg = msg;

	if( msg == CMD_NETWORK )
	{
		debugMessage->data.network.address = 0xef000001;
		debugMessage->data.network.port = 1337;
	}
	
	cmd_msgQueue.push(debugMessage);

	ReleaseMutex( down_mutex );
}
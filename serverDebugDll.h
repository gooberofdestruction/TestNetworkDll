//***********************************
//		HEADERS
//***********************************
//-----------------------------------
//Standard Headers
//-----------------------------------
#include <signal.h>
#include <queue>

//-----------------------------------
//Project Headers
//-----------------------------------
#include "server_client_DLL.h"

typedef class debugDll
{
    private:
    bool running;
	std::queue<messageQueueStruct*> out_msgQueue;
	std::queue<char*> in_msgQueue;

	std::queue<CmdMessage*> cmd_msgQueue;
    /*
     Everything else
    */
	int CONSOLE_WIDTH;
	int CONSOLE_HEIGHT;

	HANDLE wHnd;    // Handle to write to the console.
	HANDLE rHnd;    // Handle to read from the console.

	HANDLE	down_mutex; 
	HANDLE	up_mutex; 
	
	HANDLE	cmd_mutex; 

	void fill(HANDLE hConsoleOutput, int width, int height);
	void setConsoleScreen();
	
	void addMessage();
	void handleQueuedMessages();
	void addCmdMessage(CMD_MESSAGE msg);

    public:
		
	debugDll();
	~debugDll();
	void destroy();

    void stop();
    void start();

	bool hasCmdMesssages();
	CmdMessage* getNextCmd();

    bool hasMessagesToSend();
    messageQueueStruct* getNextSend();
    
	void messageHandler(char* buffer, int size);
    /*
     Everything else
    */
	void signal_trap(int signal);

} debugDll;


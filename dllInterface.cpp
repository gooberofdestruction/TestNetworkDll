#include <windows.h>
#include <iostream>

#include "server_client_DLL.h"
#include "serverDebugDll.h"

using namespace std;

#ifdef __cplusplus     
extern "C" 
{          
#endif

//Returns bool that indicates if there is a message to send
__declspec(dllexport) bool __cdecl hasMessagesToSend(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	return ((debugDll*)instance)->hasMessagesToSend();
}

//returns a pointer to a message queue structure that has the populated data to send
//server/client will reclaim memeory
__declspec(dllexport) messageQueueStruct* __cdecl getNextSend(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	return ((debugDll*)instance)->getNextSend();
}

//given a populated buffer and size
__declspec(dllexport) void __cdecl messageHandler(void* instance, char* buffer, int size)
{
	//cout<<__FUNCTION__<<endl;
	((debugDll*)instance)->messageHandler(buffer, size);
}

//constructor
__declspec(dllexport) void __cdecl createInstance(void** instance)
{
	//cout<<__FUNCTION__<<endl;
	 *instance = (void*)(new debugDll());
}

//destructor
__declspec(dllexport) void __cdecl destroyInstance(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	((debugDll*)instance)->destroy();
}

//main execution sequence
__declspec(dllexport) void __cdecl runInstance(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	((debugDll*)instance)->start();
}

//stop main execution sequence
__declspec(dllexport) void __cdecl stopInstance(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	((debugDll*)instance)->stop();
}

//Returns bool that indicates if there is a message to send
__declspec(dllexport) bool __cdecl hasCmdMesssages(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	return ((debugDll*)instance)->hasCmdMesssages();
}

//returns a pointer to a message queue structure that has the populated data to send
//server/client will reclaim memeory
__declspec(dllexport) CmdMessage* __cdecl getNextCmd(void* instance)
{
	//cout<<__FUNCTION__<<endl;
	return ((debugDll*)instance)->getNextCmd();
}

#ifdef __cplusplus
}
#endif

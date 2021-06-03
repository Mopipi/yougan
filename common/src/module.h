#ifndef _MODULE_H_
#define _MODULE_H_

#include "define.h"

class Server;

class Module {
    friend class Server;

    DISALLOW_COPY_AND_ASSIGN(Module);
public:
    enum{
        Succeed = 0,
        Failed,
        Pending,
    };

    enum{
        ST_Initing,
        ST_Inited,
        ST_Starting,
        ST_Started,
        ST_Updating,
        ST_Updated,
        ST_Stopping,
        ST_Stopped,
        ST_Releasing,
        ST_Released,
        ST_Failed,
    };
    Module() :m_server(0), m_state(ST_Released) {}
    virtual ~Module() {}

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int update() = 0;
    virtual int stop() = 0;
    virtual int release() = 0;

//    /* 此函数释放自己所占用的空间，调用此函数后，Game将不再使用此模块的指针 */
//    virtual void Free() = 0;
//
    int getState() const { return m_state; }
protected:
    Server *getServer() { return m_server; }
protected:
    int m_state;
    Server * m_server;
};

#define _EXPECT_ON(moduleName, ws)\
{\
	int state = getState();\
	int wait_state = -1;\
	switch(state)\
	{\
	case ST_Initing:\
	case ST_Released:\
		wait_state = ST_Inited;\
		break;\
	case ST_Inited:\
	case ST_Starting:\
		wait_state = ST_Started;\
		break;\
	case ST_Started:\
	case ST_Updating:\
		wait_state = ST_Updated;\
		break;\
	case ST_Updated:\
	case ST_Stopping:\
		wait_state = ST_Stopped;\
		break;\
	case ST_Stopped:\
	case ST_Releasing:\
		wait_state = ST_Released;\
		break;\
	default:\
		break;\
	}\
	if (wait_state  == ws)\
	{\
		Module* module = getServer()->getModel(moduleName);\
		if ( module != 0 && ( module->getState() != wait_state && module->getState() != ST_Failed ) )\
		{\
			return Pending;\
		}\
	}\
}

#define _DEPEND_ON(moduleName, ws)\
{\
	int state = getState();\
	int wait_state = -1;\
	switch(state)\
	{\
	case ST_Initing:\
	case ST_Released:\
		wait_state = ST_Inited;\
		break;\
	case ST_Inited:\
	case ST_Starting:\
		wait_state = ST_Started;\
		break;\
	case ST_Started:\
	case ST_Updating:\
		wait_state = ST_Updated;\
		break;\
	case ST_Updated:\
	case ST_Stopping:\
		wait_state = ST_Stopped;\
		break;\
	case ST_Stopped:\
	case ST_Releasing:\
		wait_state = ST_Released;\
		break;\
	default:\
		break;\
	}\
	if ( wait_state == ws )\
	{\
		Module* module = getServer()->getModel(moduleName);\
		if ( module != 0 )\
		{\
			if ( module->getState() == ST_Failed )\
			{\
				return Failed;\
			}\
			else if ( module->getState() != wait_state )\
			{\
				return Pending;\
			}\
		}\
		else\
		{\
			return Failed;\
		}\
	}\
}

#define DEPEND_ON_INIT(moduleName) _DEPEND_ON(moduleName, ST_Inited)
#define DEPEND_ON_START(moduleName) _DEPEND_ON(moduleName, ST_Started)
#define EXPECT_ON_INIT(moduleName) _EXPECT_ON(moduleName, ST_Inited)
#define EXPECT_ON_START(moduleName) _EXPECT_ON(moduleName, ST_Started)

#define MAX_MODULE_NAME_LEN		32

#endif
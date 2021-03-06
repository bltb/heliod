/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright 2008 Sun Microsystems, Inc. All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 *
 * Neither the name of the  nor the names of its contributors may be
 * used to endorse or promote products derived from this software without 
 * specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// File:    ChildExec.h
//
// Description:
//
//      This header file defines the API to the child exec 
//      subsystem, providing a high performance fork/exec engine 
//      for servers to implement out of process application 
//      protocols such as the Common Gateway Interface (CGI)
//
// Contact:  mike bennett (mbennett@netcom.com)     13Mar98
///

#ifndef __CHILDEXEC_H__ 
#define __CHILDEXEC_H__ 

#include "netsite.h"
#include "safs/child.h"

//
//  subsystem description 
//  
//  This subsystem consists of a number of child 'stub' processes 
//  which receive, using an IPC pipe mechanism, formatted requests 
//  to start a subprocess, given exec...() style path, argv and
//  env parameters.  If successful, the child 'stub' returns 
//  an indication consisting of the new child's pid and two FDS:
//  an IPC pipe connected 'stdin' of the child and an IPC pipe
//  connected to 'stdout' & stderr of the new child.
//
//  During normal operation, an application uses CBExec.exec(), 
//  passing in required child process' path, argv array, and 
//  environment variables.  This routine formats and sends a request to
//  a child stub process using a request pipe and awaits a
//  response.  A successful exec will return a response consisting
//  of the child's pid, and the stdin and stdout fds.
//  NOTE: this version (08mar98) of the subsystem does NOT provide
//  child exit status notification (SIGCLD) support.
//

//
//  errors generated by this subsystem
//
typedef enum {
        //  errors returned prior to child stub processing
        CERR_OK             =  0,  // no error 
        CERR_BADPARAM       = -1,  // illegal parameter 
        CERR_MEMFAILURE     = -2,  // memory allocation failure 
        CERR_UNINIT         = -3,  // subsystem not initialized 
        CERR_ALREADY        = -4,  // subsystem already initialized 
        CERR_BADREQPIPE     = -5,  // invalid request pipe 
        CERR_BADPATH        = -6,  // bad child path 
        CERR_BADARGV        = -7,  // invalid argv array 
        CERR_BADENV         = -8,  // invalid env array 
        CERR_REQFAILURE     = -9,  // the request pipe I/O failed 
        CERR_NOLISTENER     = -10, // no listener process (server stub)
        CERR_LISTENERBUSY   = -11, // listener process is busy; try again

        // errors returned from the child stub subprocess
        CERR_CHDIRFAILURE   = -20, // chdir to path failure; check ioerr 
        CERR_CHROOTFAILURE  = -21, // chdir to path failure; check ioerr 
        CERR_FORKFAILURE    = -22, // fork failure; check ioerr 
        CERR_EXECFAILURE    = -23, // exec failure; check ioerr 
        CERR_RESOURCEFAIL   = -24, // pipe() error; check ioerr
        CERR_USERFAILURE    = -25, // failed to set user
        CERR_GROUPFAILURE   = -26, // failed to set group
        CERR_STATFAILURE    = -27, // stat() error; check ioerr
        CERR_BADDIRPERMS    = -28, // Cgistub parent dir unsecured
        CERR_BADEXECPERMS   = -29, // CGI program unsecured
        CERR_BADEXECOWNER   = -30, // CGI program not owned by this user
        CERR_SETRLIMITFAILURE = -31, // setrlimit() failure; check ioerr
        CERR_NICEFAILURE    = -32, // nice() failure; check ioerr 
        CERR_INTERNALERR    = -99  // shouldn't occur 
} CHILDEXEC_ERR;

typedef enum {
    LISTENER_SHUTDOWN = 0,
    LISTENER_START = 1
} ListenerOp;

//
//  this is the exec argument array for use by ChildExec::exec
//

typedef struct cexec_args {
    pid_t         cs_pid;        // returned pid of child 
    int           cs_stdin;      // returned stdin fd of child 
    int           cs_stdout;     // returned stdout fd of child 
    int           cs_stderr;     // returned stderr fd of child 
    ChildOptions  cs_opts;
    const char *  cs_exec_path;  // fully expanded file system program path 
    const char * const * cs_args; // arg string, NULL delimited 
    const char *  cs_argv0;      // optional; argv0 param (if not supplied, 
                                 // inferred from strrchr of cs_exec_path 
    const char * const * cs_envargs; // env variables
} cexec_args_t;

//
//  used for managing child stub processes
//  
class CExecReqPipe;

//
//  ChildExec subsystem control block
//
class ChildExec { 
public:
    //  constructors
    //      create a control block, passing in the name of the 'stub' 
    //      executable program
    ChildExec(const char * stubExecProgram);
    //  destructor
    virtual ~ChildExec(void);
    void Terminate();
private:
    //  these are not allowed currently
    ChildExec(const ChildExec& x);              // copy constructor
    ChildExec& operator=(const ChildExec& x);   // '=' operator

public:
    //
    //  public methods:
    //      start a program and return the fds of stdin & stdout
    //      
    CHILDEXEC_ERR   exec( cexec_args_t& execargs, int& ioerr );

    //
    //  turn an error code into a displayable error message
    //  pass in a work buffer and it's length; returns the 
    //  address of workBuf
    //
    static const char * csErrorMsg( CHILDEXEC_ERR cserr, int ioerr,
                                char * workBuf, size_t workBufsz );

    //
    //  accessors
    //
    void setMinChildren(int nbr);
    void setMaxChildren(int nbr);
    void setIdleChildReapInterval(PRIntervalTime interval);

    CHILDEXEC_ERR initialize(int& ioerr);  // optional; pre_start the listener

private:
    //
    //  private methods:
    //      start a listener process  
    CHILDEXEC_ERR   _startListener();
    //      retrieve a pipe for use; make one if none are available
    CHILDEXEC_ERR   getIdlePipe( CExecReqPipe*& rqp, int& ioerr );
    //      create a new request pipe
    CHILDEXEC_ERR   createRequestPipe( CExecReqPipe*& rqp, int& ioerr );

    CHILDEXEC_ERR StartListener(int& ioerr);
    CHILDEXEC_ERR ShutdownListener(int& ioerr);
    CHILDEXEC_ERR PerformListenerOp(ListenerOp op, int& ioerr);
    CHILDEXEC_ERR _performListenerOp();


    void    Lock();               // lock the control block
    void    Unlock();             // unlock the control block
    void    waitForMoreChildren();// wait for more children
    void    notifyMoreChildren(); // notify waiters about new children

    //      build a start request to a child
    int     buildStartRequest( char *& cPtr, size_t& msgSize, 
                               const cexec_args_t& parms );

    void    startReaper();        // start the reaper thread
    void    stopReaper();         // stop the reaper thread
    void    wakeupReaper();       // wake up the reaper thread 
    static void reaperThreadEntry( void * cbPtr ); // reaper thread entrypoint
    void    reaperMainLoop();     // main loop for the reaper thread
    

private:
    // NOTE the cbRpIdle and cbRpInUse lists (and their counts)
    // are controlled by the cbLock lock
    CExecReqPipe *  cbRpInuse;      // list of pipes for this cb 
    CExecReqPipe *  cbRpIdle;       // list of pipes for this cb 
    int             cbNbrPipesIdle; // # pipes on the cbRpIdle list
    int             cbNbrPipes;     // # nondead pipes 
    int             cbPipesCreating;// # of pipes we are in in the process
                                    // of creating
    PRLock *        cbLock;         // controls access to this cb 
    PRLock *        cbMoreChildLock;// lock for MoreChildCV
    PRCondVar *     cbMoreChildCV;  // used to manage notifyMoreChildren
    char *          cbExecPath;     // exec path of server process 
    char *          cbSockPath;     // path used by server process 
    int             cbListenPid;    // pid of listener process 
    int             cbListenFd;     // fd of stdin fd for pid 

    int             cbMinChildren;  // # children to maintain
    int             cbMaxChildren;  // absolute maximum # children; 
                                    // stall requests when this is reached
    int             cbIdleReapInt;  // time child can be idle before we reap
    PRBool          cbReaperRunning;// is the reaper thread running?
    PRThread *      cbReaperThread; // reaper thread id
    PRLock *        cbReaperLock;   // lock for reaper to wait on
    PRCondVar *     cbReaperCV;     // used to notify reaper

    PRLock *        _listenerLock;  // For notifying the listener thread
    static PRBool   _terminating;
#if defined(LINUX)
    PRCondVar *     _revListenerCvar;
    PRCondVar *     _listenerCvar;
    PRThread *      cbListenerThread;
    static void ListenerOpThreadStart(void* arg);
    void ListenerThreadMainLoop();
#endif
    ListenerOp      _listenerOp;
    int             _ioerr;
    CHILDEXEC_ERR   _res;
    

    friend class    CExecReqPipe;   // allows pipes to add themselves to
                                    // our cbRp... lists
};

#endif // __CHILDEXEC_H__ 

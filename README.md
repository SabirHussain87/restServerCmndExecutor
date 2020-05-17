Requirement:
====================================================================================================
Design and implement a simple command orchestrator as follows:

 

1.  The commands should be input in a machine parsable format (JSON/XML).

2.  The orchestrator should read the incoming command and execute them concurrently.

3.  At any given time, the user should be able to see the current state of the orchestrator:

      a.  How many commands have been executed with completion status.

      b.  How many commands are in progress.

      c.  How many commands are pending.

4.  Some commands might have inter-dependencies that should be expressible in the input to the orchestrator.

      a.  The orchestrator should honour such dependencies.

 

Other considerations:

1.  Choose any high level programming language.

2.  Provide a README explaining the code layout and general design considerations.


Implementation:
==================================================================================================
Implemented on Ubuntu 18.04 using CPP, and compiled version of x86_64 availble in this repo

The orchestrator works as a multithreaded concorrent rest webserver. Commands can be sent to it from
local host or remotely. To send the command, please see below commands. The commands needs to be in json 
format.

To implement rest server, I have choosen to use opensource https://github.com/Corvusoft/restbed
Restbed installtion guide in available in the above github link. 
I have included all the files required to compile the project independely. 
To parse json, I have choosen to use https://rapidjson.org/ . This is header only library, and it is 
included in the project. 

The rest server has implemention of POST request to get commands from the users. In response it sends a 
uniq id for the received command. The user can read the output of the command by issuing a get request with 
id. Example: curl -w'\n' -v -X GET 'http://localhost:1900/status?id=1'

To run the commands, the server implements a threadpool. The number of threads is configurable and passed 
throught he command line, while opening the server. Each threads picks one job form the queue and updates the 
output and again waits on the queue for next job, if job is not readily available. 
The thread pool keeps track of pending job, running job and completed jobs. 
Note: I have implemented a basic book keeping for now, it can be scaled up depending upon the need.

The server also supports a get requests which returns complete status of pending commands, running commands and 
completed commands.

Depended commands can be given as pipe. There is example json for the same pipe.json.
The server can also accept multiple commands from the same json. Example: multi.json

How to compile
---------------------------------------------------
From the project directory run make
#make
To clean
#make clean
 
How to run
--------------------------------------------------
#./restServer <portNumber 1000-65535> <Number of threads to run backgroup commands>
#./restServer 1900 10

Example of sending commands to server
---------------------------------------------------
curl -vX POST http://127.0.0.1:1900/command --header "Content-Type: application/json" -d@test.json
curl -vX POST http://127.0.0.1:1900/command --header "Content-Type: application/json" -d@multi.json

#Get status of commands:
---------------------------------------------------
curl -w'\n' -v -X GET 'http://localhost:1900/status'
The above get request returns complete stats, currently running commands id, completed command ids and pending commands

#To see output of a command
---------------------------------------------------
For example to see output of command id 1
curl -w'\n' -v -X GET 'http://localhost:1900/status?id=1'

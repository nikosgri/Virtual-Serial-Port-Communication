# SERIAL PORT COMMUNICATION 
The solution of this **task** consist of two different applications **server** and **client** that communicate via a serial port connection through socat. Server waits for commands and sends back the appropriate responses. Reading and writing by both of apps is from tty-like files.Genaraly it is an "AT-Command based Server" and a "Cli-enabled client" where the exchanged payload is ASCII string. A small example is :

1) AT-Command  Format : "AT+"CMD" 
2) AT-Response Format : "RES"
 
## 1 Locations
In **toptions** directory you will find the header file and the application that implements  the existing functions. Also in there provided explainations for both them. Furthermore in **src** directory there are the two basic functions server and client. In order to run executables server and client you have to specify the  exactly SerialPort(ttyFile) you want to open.
## 2 Compiling 

In order to compile and run this task you have to build it first, so use the command **cmake -B "filename" .** , i suggest to use "filename"=build. You are gonna need 3 terminals .

1) cmake -B build .
2) cd build
3) make 

## 3 User guide 

You are going to need three terminals, one for setting up socat connection of the applications and the other two in order to run the executables of thoose applications. So : 
 
Terminal 1 
* socat -d -d PTY,link= <(Your Serial_1)> PTY,link=<(Your Serial_2)>  [Note1](#Note1)

Terminal 2 
* sudo ./server (Your Serial_1) [Note2](#Note2)

Terminal 3 
* sudo ./client (Your Serial_2) [Note2](#Note2)

At this point you can start send AT-Commands to the server from clients terminal.
## OPERATIONS 
* Type help command to see the guide for you AT messages and also which registers are available at that time.
![Help_command](/img/two.png)
* If you type help command you are going to see the import message. So that is an operation where you can add another one register and set you num bounds either distinct or not for example if you enter (**y**) at import message :
![import](/img/three.png)In case of you dont want to add another register simple enter (**n**) at import message. At this time you can type again the help command in order to check the update of "Available AT commands" : 
![Update_help_command](/img/four.png)
* After that you can start sending messages to the server and get responses only for registers that you have import or default registers (**REG1 - REG2**). Start playing with REG1 AND REG2 first, in order to see how reponses works.




### Note1 
In my example am using
* /dev/ttyUSB0 as (Your Serial_1) and 
* /dev/ttyUSB1 as (Your Serial_2)

You are able to use every virtual port you want, by searching them in the proper directory like :
* ls /dev/tty* (Search for all serial ports)

### Note2
In order to find the executables of server and client, you have to enter the src, at the folder where you build this task. For example after the execute  command **make**, in step 3 of the compile process, also execute **cd src** to find the executables(server-client).

### General Note
If you dont want to use sudo command (running as root) check if {USER} belongs  to  dialout group (**groups ${USER}**). If its not you can add {USER} by executing the command **sudo gpasswd --add ${USER} dialout**
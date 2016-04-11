# C-Server-Client-with-message-passing

<p>Need to implement both client and server applications. Server can be started with any command or initial setup parameters. However client program will start only with the program’s name. (No parameters are accepted)

<p>There are four commands for users that are login, getList, follow and message. Each message will be in keyword value format, and can be composed of multiple <key,value> pairs, but can contain only one command. Below there are examples of each client command. 

	<p>message = “command:login, username:ch”  // message text that logins or adds user ch
	<p>message = “command:getList”  // server will send the user list as comma separated text
	<p>message = “command:follow, username:aaa”  // start following user-aaa
	<p>message = “command:message, text:hello”  // send “hello” to my followers


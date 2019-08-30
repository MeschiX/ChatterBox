# ChatterBox
A simple client-server chat application with AF_UNIX sokets.

Unzip it and compile it using ```make all```.
Two executables files will appear: chatty (server), client (client).

Use ```./chatty -f DATA/chatty.conf1``` or ```./chatty -f DATA/chatty.conf2``` to lunch the server.   
"DATA/chatty.conf1" and "DATA/chatty.conf2" are two configuration files for the server, 
read them to find out the adjustable values.

Use:   
```./client -l /path/to/the/connection/socket -k nick -c nick -[gad] group -t milli -S msg:to -s file:to -R n -h```   
 - ```-l``` specify the connection socket location
 - ```-k``` specify the client's nickname
 - ```-c``` specify the user's nickname to be registered
 - ```-C``` specify the user's nickname to be unregistered
 - ```-g``` specify the group's nickname to be registered
 - ```-a``` add the user's nickname ```nick``` to the group specified by the groupname ```group```
 - ```-d``` remove the user's nickname ```nick``` from the group specified by the groupname ```group```
 - ```-L``` to send the request for the online users list
 - ```-p``` to retrieve the message history
 - ```-t``` specify the milliseconds between an operation and the next
 - ```-S``` send the message ```msg``` to the receiver ```to``` that can be the nickname of a user or of a group
 - ```-s``` as ```-S``` but it permit to send files
 - ```-R``` wait to receive messages. The behaviour is different based on the value of ```n```:
    . ```n > 0```: wait to receive 'n' messages and then execute the next command
    . ```n <= 0```: wait to receive one or more message indefinitely.
    

For further (in italian) information and description read the ```Relazione.pdf```.  
For implementation details run ```doxygen Doxyfile``` in the project directory, it will generate the code 
documentation in ```html``` format and you can read it opening ```Documentazione/html/index.html```.


:beers: Cheers!

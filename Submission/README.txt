+----------------------------------------+
|                                        |
|   Networks Assignment - 1              |
|           Sockets                      |
|                                        |
+----------------------------------------+



+---------------------+
|   Implementation    |
+---------------------+

$ make 
$./server <port number>
$./client <IP, port number>

After the whole working is done

$make clean





+------------------+
|   Server Code    |
+------------------+

First withing the server we will give the port number along with the executable, once this is done we will make the server to start listening on the required. 

We run the server on an infinite loop so that it can keep listening to requests from various clients unless we terminate the processes ourselves.

We keep sending the data in chunks so that irrespective of how large the file is we can always send the complete data to the client.


+------------------+
|   Client Code    |
+------------------+

For the client, we will have to give the IP address and the port number of the server to be able to connect to the server to start receiving the files

Even the clients runs on an infinite loop, so that once the socket connection is made the client can keep on making requests unless we terminate the process ourselves.

We receive data in chunks and print them out to the standard outut.
# Demo-TAIST-ICT720
Demonstration of all Practices, just find which one you want

## Lab04 Local MQTT Fixed
Due to my problem in Lab04 Local MQTT, at first my computer won't allow other connections rather than localhost. I tried many methods to troubleshoot and finally all of them are failed. Like turn off the firewall, setup inbound rules and outbound rules, setup some command lines suggested by ChatGPT. 

Finally, I found the method checking the using ports with
  netsh interface portproxy show all

It shows;
Listen on ipv4:             Connect to ipv4:
----------------------------------------------------
0.0.0.0         1883        172.26.160.1   1883

So I just delete it with command
  netsh interface portproxy delete v4tov4 listenport=1883 listenaddress=0.0.0.0

(In case you want to manually set it)
  netsh interface portproxy add v4tov4 listenport=1883 listenaddress=0.0.0.0 connectport=1883 connectaddress=172.26.160.1

After I delete it and compose the docker again, my Local MQTT is worked!

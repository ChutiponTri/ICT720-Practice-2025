# Demo-TAIST-ICT720

## Demonstration of All Practices
Find the specific practice you need from the list below.

---

## Lab04: Fixing Local MQTT Issues

### Problem Overview
During Lab04, I encountered an issue where my computer only allowed MQTT connections on `localhost` and blocked external connections. I attempted multiple troubleshooting methods, including:
- Disabling the firewall
- Setting up inbound and outbound rules
- Running various command-line solutions suggested by ChatGPT

Unfortunately, none of these solutions worked.

### Solution
After further investigation, I discovered that my system had a port proxy set up that was interfering with MQTT. I checked the active port mappings using:

```sh
netsh interface portproxy show all
```

This command returned the following:

```
Listen on IPv4:       Connect to IPv4:
--------------------------------------
0.0.0.0     1883      172.26.160.1   1883
```

This indicated that traffic on port `1883` was being redirected incorrectly. To fix the issue, I removed the problematic port proxy with:

💡 **Note:** These commands were executed in **PowerShell with Administrator privileges**.

```sh
netsh interface portproxy delete v4tov4 listenport=1883 listenaddress=0.0.0.0
```

After deleting the proxy entry and restarting my Docker container, the Local MQTT connection worked as expected!

### Manually Setting Up Port Proxy (Optional)
If you need to manually configure the port proxy, use the following command:

```sh
netsh interface portproxy add v4tov4 listenport=1883 listenaddress=0.0.0.0 connectport=1883 connectaddress=172.26.160.1
```

---

Now, my Local MQTT is fully functional!

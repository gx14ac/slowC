## What is Slowloris
- Slowloris attack is an attack method using `Slow HTTP Attack` which is a kind of Dos/DDos Attack

## How to Attack
- A slow HTTP attack is an attack technique that continually sends fragmented requests to a web server while keeping the HTTP connection from being disconnected, thereby occupying the resources of the web server and driving down the service. It is sometimes called "Asymmetric Attack" because of the asymmetry that large sites can be attacked with few resources. It is mainly an effective attack method against Apache and is said to be ineffective against Microsoft Internet Information Services (IIS). Apache 2.2.15 or later has added a module called "mod_reqtimeout" to support slow HTTP attacks.

Unlike attacks like UDP floods, slow HTTP attacks make attacks easier without using a large number of bots, making DDoS attacks easier. It is also troublesome that a firewall monitoring Layer 2/3 cannot cope with it.

To prevent such attacks, it is effective to use F5 BIG-IP, which has the function of a WAF (Web Application Firewall).

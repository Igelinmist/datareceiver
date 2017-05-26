# Datareceiver project
## What is it?
It's real working part of application for collecting in one place data about technological process state by pass IEC104 protocol.
This is a client for interacting with one IEC104 server, which holds channel open, gets data from tag set and then sends it to local server by pass udp message (in fact it constitutes unpack IEC APDU). I run one item for one server to avoid work with threads and simplify maintaining.
## Why I publish it?
I used set of free programms (CodeBlocks in general to write it) and compiller (GCC), the Internet as a free source of information and now it's my turn to share. May be my work will help somebody. I hope it.
## What is the state now?
It works since 2011 without any changing. Our Company produce electrical and heat power on five objects, the application helps integrate information about production to publish it on intranet site.
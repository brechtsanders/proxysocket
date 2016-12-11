proxysocket
===========
Portable C library for proxy support, designed to be used as a drop-in replacement for connect().

Description
-----------
Cross-platform C library to establish TCP connections using a proxy.
Supports different connection methods:
 - no proxy (optionally allowing to bind to a local address and/or port)
 - HTTP proxy: only CONNECT method, only without authentication or basic authentication
 - SOCKS4: without IDENT functionality
 - SOCKS5 (RFC 1928): only without authentication or username/password
Features:
 - Currently only support IPv4 TCP connections.
 - Returns a standard operating system SOCKET that can be manipulated by standard operating system functions like send() and recv().
 - Supports daisy-chaining multiple proxies.
 - Can also be used for direct connections (without proxy) optionally binding to a local address and/or port.
 
Goals
-----
Portability across platfoms (including Windows, Linux and macOS).
Returned SOCKET a standard operating system connection handle as returned by socket(), allowing for an easy replacement of socket() and connect().
Support for IPv4 TCP connections only. Also, besides the supported proxy protocols, no specific protocol support (like HTTP) is included.

Dependancies
------------
None

License
-------
proxysocket is released under the terms of the MIT License (MIT), see LICENSE.txt.


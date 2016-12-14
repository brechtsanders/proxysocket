#include "proxysocket.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DST_HOST "www.edustria.be"
#define DST_HOST "www.microsoft.com"
#define DST_PORT 80
//#define DST_HOST "10.0.0.1"
//#define DST_PORT 8080
//#define DST_HOST "garbo.uwasa.fi"
//#define DST_PORT 21

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_NONE
#define PROXY_HOST NULL
#define PROXY_PORT 0
#define PROXY_USER NULL
#define PROXY_PASS NULL
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_WEB_CONNECT
#define PROXY_HOST "10.0.0.10"
#define PROXY_PORT 8080
#define PROXY_USER NULL
#define PROXY_PASS NULL
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_WEB_CONNECT
#define PROXY_HOST "localhost"
#define PROXY_PORT 3128
#define PROXY_USER "3APA3A"
#define PROXY_PASS "3apa3a"
// $MINGWPREFIX/3proxy/3proxy.exe $MINGWPREFIX/3proxy/cfg/3proxy.cfg.sample
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_SOCKS4
#define PROXY_HOST "localhost"
#define PROXY_PORT 1080
#define PROXY_USER "3APA3A"
#define PROXY_PASS "3apa3a" //ignored
// in a connected PuTTY SSH session go to Connection / SSH / Tunnels and add a forwarded port with Source port 1080, leave Destination port empty, select Dynamic and Audo and press Add
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_SOCKS5
#define PROXY_HOST "localhost"
//#define PROXY_PORT 1080
#define PROXY_PORT 1188
#define PROXY_USER NULL
#define PROXY_PASS NULL
// in a connected PuTTY SSH session go to Connection / SSH / Tunnels and add a forwarded port with Source port 1080, leave Destination port empty, select Dynamic and Audo and press Add
/**/

/**/
#define PROXY_TYPE PROXYSOCKET_TYPE_SOCKS5
#define PROXY_HOST "localhost"
#define PROXY_PORT 1080
#define PROXY_USER "3APA3A"
#define PROXY_PASS "3apa3a"
// $MINGWPREFIX/3proxy/3proxy.exe $MINGWPREFIX/3proxy/cfg/3proxy.cfg.sample
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_SOCKS4
#define PROXY_HOST "10.0.0.5"
#define PROXY_PORT 1080
#define PROXY_USER NULL
#define PROXY_PASS NULL
// ssh -D 10.0.0.1:1080 -p 22 user@server
/**/

/*/
#define PROXY_TYPE PROXYSOCKET_TYPE_SOCKS5
#define PROXY_HOST "127.0.0.1"
#define PROXY_PORT 9150
#define PROXY_USER NULL
#define PROXY_PASS NULL
// when running Tor
/**/


void logger (int level, const char* message, void* userdata)
{
  const char* lvl;
  switch (level) {
    case PROXYSOCKET_LOG_ERROR   : lvl = "ERR"; break;
    case PROXYSOCKET_LOG_WARNING : lvl = "WRN"; break;
    case PROXYSOCKET_LOG_INFO    : lvl = "INF"; break;
    case PROXYSOCKET_LOG_DEBUG   : lvl = "DBG"; break;
    default                      : lvl = "???"; break;
  }
  fprintf((FILE*)userdata, "%s: %s\n", lvl, message);
}

int main ()
{
  SOCKET sock;
  char* errmsg;
  proxysocket_initialize();
  //proxysocketconfig proxy = proxysocketconfig_create(PROXY_TYPE, PROXY_HOST, PROXY_PORT, PROXY_USER, PROXY_PASS);
//proxysocketconfig_add_proxy(proxy, PROXY_TYPE, PROXY_HOST, PROXY_PORT, PROXY_USER, PROXY_PASS);
//proxysocketconfig_add_proxy(proxy, PROXY_TYPE, PROXY_HOST, PROXY_PORT, PROXY_USER, PROXY_PASS);
//proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS4, "127.0.0.1", 1080, NULL, NULL);
//proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS5, "127.0.0.1", 1080, NULL, NULL);
proxysocketconfig proxy = proxysocketconfig_create_direct();
proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS5, "127.0.0.1", 1080, "3APA3A", "3apa3a");
proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS5, "127.0.0.1", 1080, "3APA3A", "3apa3a");
proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS5, "127.0.0.1", 1080, "3APA3A", "3apa3a");
proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_WEB_CONNECT, "10.0.0.1", 8080, NULL, NULL);
//proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_WEB_CONNECT, "10.0.0.1", 8080, NULL, NULL);
  proxysocketconfig_set_logging(proxy, logger, (void*)stdout);
  //connect
  errmsg = NULL;
  sock = proxysocket_connect(proxy, DST_HOST, DST_PORT, &errmsg);
  if (sock == INVALID_SOCKET) {
    fprintf(stderr, "%s\n", (errmsg ? errmsg : "Unknown error"));
    free(errmsg);
  } else {
    printf("----------------------------------------\n");
    //send data
    const char* http_request = "GET / HTTP/1.0\r\nHost: " DST_HOST "\r\n\r\n";
    send(sock, http_request, strlen(http_request), 0);
    //receive data
    char c;
    while (recv(sock, &c, 1, 0) > 0) {
      putchar(c);
    }
    //disconnect
    proxysocket_disconnect(proxy, sock);
  }
  proxysocketconfig_free(proxy);
  return 0;
}


//https://tools.ietf.org/html/rfc1928
//https://tools.ietf.org/rfc/rfc1929.txt
//https://tools.ietf.org/rfc/rfc1961.txt

//SOCKS4: http://ftp.icm.edu.pl/packages/socks/socks4/SOCKS4.protocol


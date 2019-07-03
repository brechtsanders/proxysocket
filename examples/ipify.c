#include "proxysocket.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if defined(_WIN32) && !defined(__MINGW64_VERSION_MAJOR)
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#define DST_HOST "api.ipify.org"
#define DST_PATH "/"
#define DST_PORT 80

void logger (int level, const char* message, void* userdata)
{
  const char* lvl;
  if (level > *(int*)userdata)
    return;
  switch (level) {
    case PROXYSOCKET_LOG_ERROR   : lvl = "ERR"; break;
    case PROXYSOCKET_LOG_WARNING : lvl = "WRN"; break;
    case PROXYSOCKET_LOG_INFO    : lvl = "INF"; break;
    case PROXYSOCKET_LOG_DEBUG   : lvl = "DBG"; break;
    default                      : lvl = "???"; break;
  }
  fprintf(stdout, "%s: %s\n", lvl, message);
}

void show_help ()
{
  printf(
    "Usage:  example_ipify [-h] [-t proxy_type] [-s proxy_server] [-p proxy_port] [-l proxy_user] [-w proxy_pass]\n"
    "Parameters:\n"
    "  -h             \tdisplay command line help\n"
    "  -t proxy_type  \ttype of proxy to use (NONE/SOCKS4/SOCKS5/WEB)\n"
    "  -s proxy_server\tproxy server host name or IP address\n"
    "  -p proxy_port  \tproxy port number\n"
    "  -l proxy_user  \tproxy authentication login\n"
    "  -w proxy_pass  \tproxy authentication password\n"
    "  -n             \tuse proxy name resolution (instead of local DNS)\n"
    "  -v             \tverbose mode\n"
    "  -d             \tdebug mode (overrides -v)\n"
    "Version: %s\n"
    "Description:\n"
    "Gets public IP address from " DST_HOST " optionally using a proxy server.\n"
    "This can be used to check if web (HTTP) access is working.", proxysocket_get_version_string()
  );
}

#define GET_PARAM()                     \
  if (argv[i][2])                       \
    param = argv[i] + 2;                \
  else if (i + 1 < argc && argv[i + 1]) \
    param = argv[++i];                  \
  else                                  \
    param = NULL;

int main (int argc, char* argv[])
{
  //get command line parameters
  int i;
  char* param;
  int proxytype = PROXYSOCKET_TYPE_NONE;
  const char* proxyhost = NULL;
  uint16_t proxyport = 0;
  const char* proxyuser = NULL;
  const char* proxypass = NULL;
  int verbose = -1;
  int proxydns = 0;
  for (i = 1; i < argc; i++) {
    //check for command line parameters
    if (argv[i][0] && (argv[i][0] == '/' || argv[i][0] == '-')) {
      switch (tolower(argv[i][1])) {
        case 'h' :
        case '?' :
          show_help();
          return 0;
        case 't' :
          GET_PARAM()
          if ((proxytype = proxysocketconfig_get_name_type(param)) == PROXYSOCKET_TYPE_INVALID) {
            fprintf(stderr, "Invalid proxy type: %s\n", param);
            show_help();
            return 1;
          }
          break;
        case 's' :
          GET_PARAM()
          if (param)
            proxyhost = param;
          break;
        case 'p' :
          GET_PARAM()
          if (param)
            proxyport = strtol(param, (char**)NULL, 10);
          break;
        case 'l' :
          GET_PARAM()
          if (param)
            proxyuser = param;
          break;
        case 'w' :
          GET_PARAM()
          if (param)
            proxypass = param;
          break;
        case 'n' :
          proxydns = 1;
          break;
        case 'v' :
          if (verbose < PROXYSOCKET_LOG_INFO)
            verbose = PROXYSOCKET_LOG_INFO;
          break;
        case 'd' :
          verbose = PROXYSOCKET_LOG_DEBUG;
          break;
        default:
          fprintf(stderr, "Invalid command line parameter: %s\n", argv[i]);
          show_help();
          return 1;
      }
    }
  }

  //make the connection via the specified proxy
  SOCKET sock;
  char* errmsg;
  //prepare for connection
  proxysocket_initialize();
  proxysocketconfig proxy = proxysocketconfig_create_direct(5);
  if (verbose >= 0)
    proxysocketconfig_set_logging(proxy, logger, (int*)&verbose);
  if (proxydns)
    proxysocketconfig_use_proxy_dns(proxy, 1);
  proxysocketconfig_add_proxy(proxy, proxytype, proxyhost, proxyport, proxyuser, proxypass);
  //connect
  errmsg = NULL;
  sock = proxysocket_connect(proxy, DST_HOST, DST_PORT, &errmsg);
  if (sock == INVALID_SOCKET) {
    fprintf(stderr, "%s\n", (errmsg ? errmsg : "Unknown error"));
  } else {
    //send data
    const char* http_request = "GET " DST_PATH " HTTP/1.0\r\nHost: " DST_HOST "\r\n\r\n";
    send(sock, http_request, strlen(http_request), 0);
    //receive data and skip header
    char* line;
    int prevempty = 0;
    int istext = 0;
    errmsg = NULL;
    while ((line = socket_receiveline(sock)) != NULL) {
      if (prevempty)
        break;
      if (strncasecmp(line, "Content-Type: text/plain", 24) == 0)
        istext = 1;
      prevempty = (line[0] ? 0 : 1);
      free(line);
    }
    if (!line) {
      errmsg = strdup("No content received");
    } else if (!istext) {
      errmsg = strdup("No plain text content returned");
    } else if (line) {
      printf("Your IP address: %s\n", line);
      free(line);
    }
    //disconnect
    proxysocket_disconnect(proxy, sock);
  }
  proxysocketconfig_free(proxy);
  if (errmsg) {
    fprintf(stderr, "Error: %s\n", errmsg);
    free(errmsg);
    return 1;
  }
  return 0;
}

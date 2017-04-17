/**
 * @file proxysocket.h
 * @brief proxysocket header file for creating network socket connections via different proxy methods
 * @author Brecht Sanders
 * @date 2016
 * @copyright MIT
 *
 * Include this header file to use proxysocket and link with -lproxysocket.
 * This header provides the functionality to connect network sockets
 * optionally via different proxy methods.
 */

#ifndef __INCLUDED_PROXYSOCKET_H
#define __INCLUDED_PROXYSOCKET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \cond PRIVATE */
#ifdef __WIN32__
# include <winsock2.h>
# ifdef BUILD_PROXYSOCKET_DLL
#  define DLL_EXPORT_PROXYSOCKET __declspec(dllexport)
# elif !defined(STATIC) && !defined(BUILD_PROXYSOCKET_STATIC)
#  define DLL_EXPORT_PROXYSOCKET __declspec(dllimport)
# else
#  define DLL_EXPORT_PROXYSOCKET
# endif
#else
# define DLL_EXPORT_PROXYSOCKET
# include <netinet/in.h>
# define SOCKET int
# ifndef INVALID_SOCKET
#  define INVALID_SOCKET -1
# endif
#endif
/*! \endcond */

/*! \brief version number constants
 * \sa     proxysocket_get_version()
 * \sa     proxysocket_get_version_string()
 * \name   PROXYSOCKET_VERSION_*
 * \{
 */
/*! \brief major version number */
#define PROXYSOCKET_VERSION_MAJOR 0
/*! \brief minor version number */
#define PROXYSOCKET_VERSION_MINOR 1
/*! \brief micro version number */
#define PROXYSOCKET_VERSION_MICRO 4
/*! @} */

/*! \brief proxy types
 * \sa     proxysocketconfig_create()
 * \sa     proxysocketconfig_add_proxy()
 * \{
 */
/*! \brief direct connection without proxy */
#define PROXYSOCKET_TYPE_NONE           0x00
/*! \brief SOCKS4 proxy */
#define PROXYSOCKET_TYPE_SOCKS4         0x04
/*! \brief SOCKS5 proxy */
#define PROXYSOCKET_TYPE_SOCKS5         0x05
/*! \brief HTTP proxy */
#define PROXYSOCKET_TYPE_WEB_CONNECT    0x20
/*! @} */

/*! \brief logging levels
 * \sa     proxysocketconfig_log_fn()
 * \name   PROXYSOCKET_VERSION_*
 * \{
 */
#define PROXYSOCKET_LOG_ERROR   0
#define PROXYSOCKET_LOG_WARNING 1
#define PROXYSOCKET_LOG_INFO    2
#define PROXYSOCKET_LOG_DEBUG   3
/*! @} */

//goal: function to create a socket that can be used by system function and connect it to a remote host, optionally through a proxy

/*! \brief get proxysocket version
 * \param  pmajor        pointer to integer that will receive major version number
 * \param  pminor        pointer to integer that will receive minor version number
 * \param  pmicro        pointer to integer that will receive micro version number
 * \sa     proxysocket_get_version_string()
 */
DLL_EXPORT_PROXYSOCKET void proxysocket_get_version (int* pmajor, int* pminor, int* pmicro);

/*! \brief get proxysocket version string
 * \return version string
 * \sa     proxysocket_get_version()
 */
DLL_EXPORT_PROXYSOCKET const char* proxysocket_get_version_string ();

/*! \brief initialize library, call once at the beginning of the main thread of the application
 * \return zero on success
 */
DLL_EXPORT_PROXYSOCKET int proxysocket_initialize ();

/*! \brief proxysocketconfig object type */
typedef struct proxysocketconfig_struct* proxysocketconfig;

/*! \brief type of pointer to function for logging
 * \param  level       logging level (one of the PROXYSOCKET_LOG_ constants)
 * \param  message     text to be logged
 * \param  userdata    custom data as passed to proxysocketconfig_set_logging
 * \sa     proxysocketconfig_set_logging()
 */
/*! \brief create proxy information data structure set up for direct connection
 * \return proxy information data structure on success or NULL on failure
 * \sa     proxysocketconfig_create()
 * \sa     proxysocketconfig_add_proxy()
 * \sa     proxysocketconfig_free()
 */
DLL_EXPORT_PROXYSOCKET proxysocketconfig proxysocketconfig_create_direct ();

/*! \brief create proxy information data structure
 * \param  proxytype   proxy type (one of the PROXYSOCKET_TYPE_ constants)
 * \param  proxyhost   proxy hostname or IP address (or when proxytype is PROXYSOCKET_TYPE_NONE address to bind to if non-zero)
 * \param  proxyport   proxy port number (or when proxytype is PROXYSOCKET_TYPE_NONE port to bind to if non-zero)
 * \param  proxyuser   proxy authentication login or NULL for none
 * \param  proxypass   proxy authentication password or NULL for none
 * \return proxy information data structure on success or NULL on failure
 * \sa     proxysocketconfig_create_direct()
 * \sa     proxysocketconfig_add_proxy()
 * \sa     proxysocketconfig_free()
 */
DLL_EXPORT_PROXYSOCKET proxysocketconfig proxysocketconfig_create (int proxytype, const char* proxyhost, uint16_t proxyport, const char* proxyuser, const char* proxypass);

/*! \brief add a proxy method to proxy information data structure (multiple can be daisy chained)
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \param  proxytype   proxy type (one of the PROXYSOCKET_TYPE_ constants)
 * \param  proxyhost   proxy hostname or IP address (or when proxytype is PROXYSOCKET_TYPE_NONE address to bind to if non-zero)
 * \param  proxyport   proxy port number (or when proxytype is PROXYSOCKET_TYPE_NONE port to bind to if non-zero)
 * \param  proxyuser   proxy authentication login or NULL for none
 * \param  proxypass   proxy authentication password or NULL for none
 * \return proxy information data structure on success or NULL on failure
 * \sa     proxysocketconfig_create_direct()
 * \sa     proxysocketconfig_create()
 * \sa     proxysocketconfig_free()
 */
DLL_EXPORT_PROXYSOCKET int proxysocketconfig_add_proxy (proxysocketconfig proxy, int proxytype, const char* proxyhost, uint16_t proxyport, const char* proxyuser, const char* proxypass);

typedef void (*proxysocketconfig_log_fn)(int level, const char* message, void* userdata);

/*! \brief configure logging function
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \param  log_fn      logging function to use or NULL to disable logging
 * \param  userdata    user defined data that will be passed to the logging function
 * \sa     proxysocketconfig_create()
 * \sa     proxysocketconfig_log_fn
 */
DLL_EXPORT_PROXYSOCKET void proxysocketconfig_set_logging (proxysocketconfig proxy, proxysocketconfig_log_fn log_fn, void* userdata);

/*! \brief specify where name resolution is done
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \param  proxy_dns   perform DNS lookup on the proxy server if non-zero or on client if zero (default)
 * \sa     proxysocketconfig_create()
 */
DLL_EXPORT_PROXYSOCKET void proxysocketconfig_use_proxy_dns (proxysocketconfig proxy, int proxy_dns);

/*! \brief clean up proxy information
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \sa     proxysocketconfig_create()
 */
DLL_EXPORT_PROXYSOCKET void proxysocketconfig_free (proxysocketconfig proxy);

/*! \brief establish a TCP connection using the specified proxy
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \param  dsthost     destination hostname or IP address
 * \param  dstport     destination port number
 * \param  errmsg      pointer to string that will receive error message, can be NULL, caller must free
 * \return network socket on success or INVALID_SOCKET on failure
 * \sa     proxysocketconfig_create()
 */
DLL_EXPORT_PROXYSOCKET SOCKET proxysocket_connect (proxysocketconfig proxy, const char* dsthost, uint16_t dstport, char** errmsg);

/*! \brief disconnect a proxy socket
 * \param  proxy       proxy information as returned by proxysocketconfig_create()
 * \param  sock        network socket as returned by proxysocket_connect()
 * \sa     proxysocketconfig_create()
 * \sa     proxysocket_connect()
 */
DLL_EXPORT_PROXYSOCKET void proxysocket_disconnect (proxysocketconfig proxy, SOCKET sock);

/*! \brief read a line from a socket
 * \param  sock        network socket as returned by proxysocket_connect() or socket()
 * \return contents of line without trailing new line (caller must free) or NULL on failure
 * \sa     proxysocket_connect()
 */
DLL_EXPORT_PROXYSOCKET char* socket_receiveline (SOCKET sock);

#ifdef __cplusplus
}
#endif

#endif //__INCLUDED_PROXYSOCKET_H

/****************************************************************************************************************************
  AsyncUDP_ESP32_W5500.hpp
  
  AsyncUDP_ESP32_W5500 is a Async UDP library for the ESP32_W5500 (ESP32 + LwIP W5500)
  
  Based on and modified from ESPAsyncUDP Library (https://github.com/me-no-dev/ESPAsyncUDP)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncUDP_ESP32_W5500
  Licensed under GPLv3 license
  
  Version: 2.0.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.0.0   K Hoang      28/11/2022 Initial coding for ESP32_W5500. Bump up version to v2.0.0 to sync with AsyncUDP v2.0.0
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_UDP_ESP32_W5500_HPP
#define ASYNC_UDP_ESP32_W5500_HPP

////////////////////////////////////////////////

#include <AsyncWebServer_ESP32_W5500.hpp>     // https://github.com/khoih-prog/AsyncWebServer_ESP32_W5500

class AsyncUDP;
class AsyncUDPPacket;
class AsyncUDPMessage;
struct udp_pcb;
struct pbuf;
struct netif;

typedef std::function<void(AsyncUDPPacket& packet)> AuPacketHandlerFunction;
typedef std::function<void(void * arg, AsyncUDPPacket& packet)> AuPacketHandlerFunctionWithArg;

////////////////////////////////////////////////

class AsyncUDPMessage : public Print
{
  protected:
    uint8_t * _buffer;
    size_t    _index;
    size_t    _size;
    
  public:
    AsyncUDPMessage(size_t size = CONFIG_TCP_MSS);
    virtual ~AsyncUDPMessage();
    
    size_t    write(const uint8_t *data, size_t len);
    size_t    write(uint8_t data);
    size_t    space();
    uint8_t * data();
    size_t    length();
    void      flush();
    
    operator bool()
    {
      return _buffer != NULL;
    }
};

////////////////////////////////////////////////

class AsyncUDPPacket : public Stream
{
  protected:
  
    AsyncUDP *          _udp;
    pbuf *              _pb;
    tcpip_adapter_if_t  _if;
    ip_addr_t           _localIp;
    uint16_t            _localPort;
    ip_addr_t           _remoteIp;
    uint16_t            _remotePort;
    uint8_t             _remoteMac[6];
    uint8_t *           _data;
    size_t              _len;
    size_t              _index;
    
  public:
    AsyncUDPPacket(AsyncUDPPacket &packet);
    AsyncUDPPacket(AsyncUDP *udp, pbuf *pb, const ip_addr_t *addr, uint16_t port, struct netif * netif);
    virtual ~AsyncUDPPacket();

    uint8_t * data();
    size_t    length();
    bool      isBroadcast();
    bool      isMulticast();
    bool      isIPv6();

    tcpip_adapter_if_t interface();

    IPAddress   localIP();
    IPv6Address localIPv6();
    uint16_t    localPort();
    IPAddress   remoteIP();
    IPv6Address remoteIPv6();
    uint16_t    remotePort();
    void        remoteMac(uint8_t * mac);

    size_t send(AsyncUDPMessage &message);

    int     available();
    size_t  read(uint8_t *data, size_t len);
    int     read();
    int     peek();
    void    flush();

    size_t  write(const uint8_t *data, size_t len);
    size_t  write(uint8_t data);
};

////////////////////////////////////////////////

class AsyncUDP : public Print
{
  protected:
  
    udp_pcb *_pcb;
    //xSemaphoreHandle _lock;
    bool _connected;
    esp_err_t _lastErr;
    AuPacketHandlerFunction _handler;

    bool _init();
    void _recv(udp_pcb *upcb, pbuf *pb, const ip_addr_t *addr, uint16_t port, struct netif * netif);

  public:
  
    AsyncUDP();
    virtual ~AsyncUDP();

    void onPacket(AuPacketHandlerFunctionWithArg cb, void * arg = NULL);
    void onPacket(AuPacketHandlerFunction cb);

    bool listen(const ip_addr_t *addr, uint16_t port);
    bool listen(const IPAddress addr, uint16_t port);
    bool listen(const IPv6Address addr, uint16_t port);
    bool listen(uint16_t port);

    bool listenMulticast(const ip_addr_t *addr, uint16_t port, uint8_t ttl = 1, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    bool listenMulticast(const IPAddress addr, uint16_t port, uint8_t ttl = 1, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    bool listenMulticast(const IPv6Address addr, uint16_t port, uint8_t ttl = 1, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);

    bool connect(const ip_addr_t *addr, uint16_t port);
    bool connect(const IPAddress addr, uint16_t port);
    bool connect(const IPv6Address addr, uint16_t port);

    void close();

    size_t writeTo(const uint8_t *data, size_t len, const ip_addr_t *addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t writeTo(const uint8_t *data, size_t len, const IPAddress addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t writeTo(const uint8_t *data, size_t len, const IPv6Address addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);

    size_t broadcastTo(uint8_t *data, size_t len, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t broadcastTo(const char * data, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t broadcast(uint8_t *data, size_t len);
    size_t broadcast(const char * data);

    size_t sendTo(AsyncUDPMessage &message, const ip_addr_t *addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t sendTo(AsyncUDPMessage &message, const IPAddress addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t sendTo(AsyncUDPMessage &message, const IPv6Address addr, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t send(AsyncUDPMessage &message);

    size_t broadcastTo(AsyncUDPMessage &message, uint16_t port, tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_MAX);
    size_t broadcast(AsyncUDPMessage &message);

    IPAddress   listenIP();
    IPv6Address listenIPv6();
    
    bool      connected();
    esp_err_t lastErr();
    operator  bool();

    static void _s_recv(void *arg, udp_pcb *upcb, pbuf *p, const ip_addr_t *addr, uint16_t port, struct netif * netif);
};

////////////////////////////////////////////////


#endif    //ASYNC_UDP_ESP32_W5500_HPP

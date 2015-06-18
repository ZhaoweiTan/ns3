#ifndef CUSTOMSERVER_H
#define CUSTOMSERVER_H

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-metadata.h"


using namespace ns3;

class CustomServer : public Application 
{
public:

  CustomServer ();
  virtual ~CustomServer();

  void Setup (uint16_t m_port);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void HandleRead (Ptr<Socket> socket);

  Ptr<Socket>     m_socket;
  uint16_t        m_port;
  bool            m_running;
  EventId         m_sendEvent;
};


#endif

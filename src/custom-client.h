#ifndef CUSTOMCLIENT_H
#define CUSTOMCLIENT_H

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-metadata.h"

#include "custom-header.h"

static std::ofstream logFiles;

using namespace ns3;

class CustomClient : public Application 
{
public:

  CustomClient ();
  virtual ~CustomClient();

  void Setup (Address remoteAddress,uint16_t remotePort, uint32_t packetSize, uint32_t nPackets, DataRate budget);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);
  DataRate GetDataRate(void);
  void SetDataRate(DataRate dataRate);
  void HandleRead (Ptr<Socket> socket);

  Ptr<Socket>     m_socket;
  Address         m_remoteAddress;
  uint16_t        m_remotePort; 
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_budget;
  uint16_t        m_prize;
  uint16_t        m_preload;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  Time          m_lastPacketSentTime;
};


#endif


#include "custom-client.h"

CustomClient::CustomClient ()
  : m_socket (0), 
    m_remoteAddress (), 
    m_remotePort(0),
    m_packetSize (0), 
    m_nPackets (0), 
    m_budget (0), 
    m_prize(1),
    m_preload(504),
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0),
    m_lastPacketSentTime(0)
{
}

CustomClient::~CustomClient()
{
  m_socket = 0;
}

void
CustomClient::Setup (Address remoteAddress,uint16_t remotePort, uint32_t packetSize, uint32_t nPackets, DataRate budget)
{
  m_remoteAddress = remoteAddress;
  m_remotePort = remotePort;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_budget = budget;
}

void
CustomClient::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (m_socket == 0)
  {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_remoteAddress) == true)
        {
          m_socket->Bind();
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_remoteAddress), m_remotePort));
        }
      else if (Ipv6Address::IsMatchingType(m_remoteAddress) == true)
        {
          m_socket->Bind6();
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_remoteAddress), m_remotePort));
        }
  }

  m_socket->SetRecvCallback (MakeCallback (&CustomClient::HandleRead, this));
  
  SendPacket ();
}

void 
CustomClient::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
CustomClient::SendPacket (void)
{
  uint8_t buffer[1024] = "hello";
  Ptr<Packet> packet = Create<Packet> (buffer,m_packetSize);
  CustomHeader sourceHeader;
  sourceHeader.SetPrize (0);
  sourceHeader.SetPreload (m_preload);
  packet->AddHeader (sourceHeader); 
  m_socket->Send (packet);
  packet->CopyData (buffer, m_packetSize);
//  NS_LOG_UNCOND((Simulator::Now().GetSeconds()-m_lastPacketSentTime.GetSeconds()) <<" packets sent : "<<m_packetsSent);
  m_lastPacketSentTime = Simulator::Now();

  if (++m_packetsSent < m_nPackets || m_nPackets==0)
    {
      ScheduleTx ();
    }
}

void 
CustomClient::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (GetDataRate().GetBitRate ())));

      m_sendEvent = Simulator::Schedule (tNext, &CustomClient::SendPacket, this);
    }
}

DataRate 
CustomClient::GetDataRate(void)
{
  return DataRate(m_budget.GetBitRate()/(m_prize+1));		//prize is at least 1;
}


void
CustomClient::HandleRead (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      uint8_t buffer[1024];
      packet->CopyData (buffer, 1024);
//      NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port "<<InetSocketAddress::ConvertFrom (from).GetPort ());
      
      CustomHeader customHeader;
      packet->PeekHeader(customHeader);
      logFiles.open("my-test-client.txt",std::ios::app);
      logFiles<<Simulator::Now ().GetSeconds () << " " << customHeader.GetPrize()<<"\n";
	  logFiles.close();
      m_prize = customHeader.GetPrize();
    }
}



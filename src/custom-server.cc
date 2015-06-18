
#include "custom-server.h"

CustomServer::CustomServer ()
  : m_socket (0), 
    m_port(0),
    m_running (false),
    m_sendEvent ()
{
}

CustomServer::~CustomServer()
{
  m_socket = 0;
}

void
CustomServer::StartApplication (void)
{
  m_running = true;
  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
    }

  m_socket->SetRecvCallback (MakeCallback (&CustomServer::HandleRead, this));

}

void 
CustomServer::StopApplication (void)
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
CustomServer::Setup (uint16_t port)
{
   m_port = port;
}

void 
CustomServer::HandleRead (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      uint8_t buffer[1024];
      packet->CopyData (buffer, 1024);
//      NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<InetSocketAddress::ConvertFrom (from).GetPort () );
      packet->RemoveAllPacketTags ();
      packet->RemoveAllByteTags ();

      socket->SendTo (packet, 0, from);
    }
}




#include "custom-queue.h"

NS_OBJECT_ENSURE_REGISTERED (CustomQueue);

TypeId CustomQueue::GetTypeId (void) 
{
  static TypeId tid = TypeId ("ns3::CustomQueue")
    .SetParent<Queue> ()
    .SetGroupName("Network")
    .AddConstructor<CustomQueue> ()
    .AddAttribute ("Mode", 
                   "Whether to use bytes (see MaxBytes) or packets (see MaxPackets) as the maximum queue size metric.",
                   EnumValue (QUEUE_MODE_BYTES),
                   MakeEnumAccessor (&CustomQueue::SetMode,
                                     &CustomQueue::GetMode),
                   MakeEnumChecker (QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
                                    QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))
    .AddAttribute ("MaxPackets", 
                   "The maximum number of packets accepted by this CustomQueue.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&CustomQueue::m_maxPackets),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxBytes", 
                   "The maximum number of bytes accepted by this CustomQueue.",
                   UintegerValue (100 * 1024),
                   MakeUintegerAccessor (&CustomQueue::m_maxBytes),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Enable", 
                   "Enable prize",
                   UintegerValue (1),
                   MakeUintegerAccessor (&CustomQueue::m_enable),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}

CustomQueue::CustomQueue () :
  Queue (),
  m_packets (),
  m_bytesInQueue (0)
{
	m_prizesContainer = PrizesContainer();
    m_dropped = 0;
}

CustomQueue::~CustomQueue ()
{

}

void
CustomQueue::SetMode (CustomQueue::QueueMode mode)
{
  m_mode = mode;
}

CustomQueue::QueueMode
CustomQueue::GetMode (void) const
{
  return m_mode;
}

bool 
CustomQueue::DoEnqueue (Ptr<Packet> p)
{
  if (m_mode == QUEUE_MODE_PACKETS && (m_packets.size () >= m_maxPackets))
    {
      Drop (p);
      return false;
    }

  if (m_mode == QUEUE_MODE_BYTES && (m_bytesInQueue + p->GetSize () >= m_maxBytes))
    {
      Drop (p);
	  m_dropped ++;
      logFiles2.open("my-test-drop.txt",std::ios::app);
      logFiles2<<Simulator::Now ().GetSeconds ()<<"\t"<<m_dropped<<"\n";
      logFiles2.close();
      return false;
    }

  // remove headers
  PppHeader pppHeader;
  p->RemoveHeader(pppHeader);
  Ipv4Header ipv4Header;
  p->RemoveHeader(ipv4Header);
  UdpHeader udpHeader;
  p->RemoveHeader(udpHeader);
  CustomHeader customHeader;
  p->RemoveHeader(customHeader);
  if (m_enable) {
	  //add prize
	  int prize = m_prizesContainer.GetPrize(DataRate("0.5Mbps"),m_bytesInQueue);
	  customHeader.SetPrize(customHeader.GetPrize()+prize);
	  m_prizesContainer.AddPrize(prize,p->GetSize(),Simulator::Now().GetSeconds());
  }
  //recover headers
  p->AddHeader(customHeader);
  p->AddHeader(udpHeader);
  p->AddHeader(ipv4Header);
  p->AddHeader(pppHeader);

  m_bytesInQueue += p->GetSize ();
  m_packets.push (p);

  return true;
}

Ptr<Packet>
CustomQueue::DoDequeue (void)
{
  if (m_packets.empty ())
    {
      return 0;
    }

  Ptr<Packet> p = m_packets.front ();
  m_packets.pop ();
  m_bytesInQueue -= p->GetSize ();

  return p;
}

Ptr<const Packet>
CustomQueue::DoPeek (void) const
{

  if (m_packets.empty ())
    {
      return 0;
    }

  Ptr<Packet> p = m_packets.front ();

  return p;
}



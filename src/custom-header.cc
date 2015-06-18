
#include "custom-header.h"

NS_OBJECT_ENSURE_REGISTERED (CustomHeader);

CustomHeader::CustomHeader ()
{
  // we must provide a public default constructor, 
  // implicit or explicit, but never private.
}
CustomHeader::~CustomHeader ()
{
}

TypeId
CustomHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CustomHeader")
    .SetParent<Header> ()
    .AddConstructor<CustomHeader> ()
  ;
  return tid;
}
TypeId
CustomHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
CustomHeader::Print (std::ostream &os) const
{
  // This method is invoked by the packet printing
  // routines to print the content of my header.
  //os << "data=" << m_data << std::endl;
  os << "prize=" << m_prize << " preload=" << m_preload;
}
uint32_t
CustomHeader::GetSerializedSize (void) const
{
  // we reserve 2 bytes for our header.
  return 4;
}
void
CustomHeader::Serialize (Buffer::Iterator start) const
{
  // we can serialize two bytes at the start of the buffer.
  // we write them in network byte order.
//  for (int i=0;i<m_offset;++i)
//        start.ReadNtohU16 ();
  start.WriteHtonU16 (m_prize);
  start.WriteHtonU16 (m_preload);
}
uint32_t
CustomHeader::Deserialize (Buffer::Iterator start)
{
//  for (int i=0;i<m_offset;++i)
//        start.ReadNtohU16 ();
  m_prize = start.ReadNtohU16 ();
  m_preload = start.ReadNtohU16 ();

  // we return the number of bytes effectively read.
  return GetSerializedSize ();
}

void 
CustomHeader::SetPrize (uint16_t prize)
{
  m_prize = prize;
}
uint16_t 
CustomHeader::GetPrize (void) const
{
  return m_prize;
}
void 
CustomHeader::SetPreload (uint16_t preload)
{
  m_preload = preload;
}
uint16_t 
CustomHeader::GetPreload (void) const
{
  return m_preload;
}



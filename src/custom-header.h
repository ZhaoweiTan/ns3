#ifndef CUSTOMHEADER_H
#define CUSTOMHEADER_H

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-metadata.h"

using namespace ns3;

class CustomHeader : public Header 
{
public:

  CustomHeader ();
  virtual ~CustomHeader ();

  void SetPrize (uint16_t data);
  uint16_t GetPrize (void) const;
  void SetPreload (uint16_t data);
  uint16_t GetPreload (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual uint32_t GetSerializedSize (void) const;
private:
  uint16_t m_offset;
  uint16_t m_prize;
  uint16_t m_preload;
};


#endif

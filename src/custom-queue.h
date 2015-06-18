#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-metadata.h"

#include "custom-header.h"

using namespace ns3;

#define MAX_PRIZES_AMOUNT 80
#define INIT_D            0.1

static std::ofstream logFiles2;

class PrizesContainer {
public:
  PrizesContainer() {
	alpha = 0.005;
	for (int i=0;i<MAX_PRIZES_AMOUNT;++i) {
		m_prizes[i] = 1;
		m_sizes[i] = 2000;
		m_times[i] = (double)i*(INIT_D/MAX_PRIZES_AMOUNT) - INIT_D;
	}
	m_end = MAX_PRIZES_AMOUNT-1;
  }
  int GetPrize(DataRate datarate, int q_t) {
	int C = datarate.GetBitRate();
	double d = m_times[m_end] - m_times[(m_end+1)%MAX_PRIZES_AMOUNT];
    int prize = (int)GetI_t()/(C-alpha*q_t/d);
/*
    NS_LOG_UNCOND("C "<<C);
	NS_LOG_UNCOND("d "<<d);
	NS_LOG_UNCOND("I(t) "<<GetI_t());
	NS_LOG_UNCOND("q(t) "<<q_t);
	NS_LOG_UNCOND("prize "<<prize);
    NS_LOG_UNCOND("times "<<m_times[m_end] <<","<< m_times[(m_end+1)%MAX_PRIZES_AMOUNT]);
	exit(0);
*/
    logFiles2.open("my-test-detail.txt",std::ios::app);
    logFiles2<<Simulator::Now ().GetSeconds ()<<"\t"<<C<<"\t"<<d<<"\t"<<q_t<<"\t"<<prize<<"\t"<<GetI_t()<<"\n";
	logFiles2.close();
	return prize;
  }
  void AddPrize(uint32_t prize,uint32_t size, double time) {
	m_end = (m_end+1)%MAX_PRIZES_AMOUNT;
	m_prizes[m_end] = prize;
	m_sizes[m_end] = size;
	m_times[m_end] = time;
  }
private:
  double   alpha;
  uint32_t m_prizes[MAX_PRIZES_AMOUNT];		// init to be all one,
  uint32_t m_sizes[MAX_PRIZES_AMOUNT];		// init to be all 10,
  double   m_times[MAX_PRIZES_AMOUNT];
  int 	   m_end;

  double GetI_t() {
	int sum = 0;
	for (int i=0;i<MAX_PRIZES_AMOUNT;++i) 
		sum += m_prizes[i]*m_sizes[i];
	double d = m_times[m_end] - m_times[(m_end+1)%MAX_PRIZES_AMOUNT];
	return sum/d;
  }
};

class CustomQueue : public Queue {
public:
  static TypeId GetTypeId (void);
  CustomQueue ();
  virtual ~CustomQueue();

  void SetMode (CustomQueue::QueueMode mode);
  CustomQueue::QueueMode GetMode (void) const;

private:
  virtual bool DoEnqueue (Ptr<Packet> p);
  virtual Ptr<Packet> DoDequeue (void);
  virtual Ptr<const Packet> DoPeek (void) const;

  std::queue<Ptr<Packet> > m_packets; //!< the packets in the queue
  uint32_t m_maxPackets;              //!< max packets in the queue
  uint32_t m_maxBytes;                //!< max bytes in the queue
  uint32_t m_bytesInQueue;            //!< actual bytes in the queue
  QueueMode m_mode;                   //!< queue mode (packets or bytes limited)
  PrizesContainer m_prizesContainer;  //for calculating prize
  uint32_t  m_enable;				  // 1: enable; 0: disable
  int       m_dropped;
};



#endif

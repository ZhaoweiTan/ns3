/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-metadata.h"

#include "custom-header.h"
#include "custom-queue.h"
#include "custom-client.h"
#include "custom-server.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FifthScriptExample");

/* - - - - - - - - - - - - - - - - main - - - - - - - - - - - - - - - - */

int 
main (int argc, char *argv[])
{
  //nodes
  NodeContainer nodes;
  nodes.Create (8);
  Names::Add ( "N0", nodes.Get (0));
  Names::Add ( "N1", nodes.Get (1));
  Names::Add ( "N2", nodes.Get (2));
  Names::Add ( "N3", nodes.Get (3));
  Names::Add ( "N4", nodes.Get (4));
  Names::Add ( "N5", nodes.Get (5));
  Names::Add ( "N6", nodes.Get (6));
  Names::Add ( "N7", nodes.Get (7));
  NodeContainer n0n2 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n2n3 = NodeContainer (nodes.Get (2), nodes.Get (3));
  NodeContainer n3n4 = NodeContainer (nodes.Get (3), nodes.Get (4));
  NodeContainer n3n5 = NodeContainer (nodes.Get (3), nodes.Get (5));
  NodeContainer n6n2 = NodeContainer (nodes.Get (6), nodes.Get (2));
  NodeContainer n7n2 = NodeContainer (nodes.Get (7), nodes.Get (2));

  //stack
  InternetStackHelper internet;
  internet.Install (nodes);
  
  //queue, channel, datarate
  PointToPointHelper p2p;

  p2p.SetQueue ("ns3::CustomQueue","MaxBytes",StringValue("1024000"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devn0n2 = p2p.Install (n0n2);

  p2p.SetQueue ("ns3::CustomQueue","MaxBytes",StringValue("1024000"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devn1n2 = p2p.Install (n1n2);
  NetDeviceContainer devn6n2 = p2p.Install (n6n2);
  NetDeviceContainer devn7n2 = p2p.Install (n7n2);

  p2p.SetQueue ("ns3::CustomQueue","MaxBytes",StringValue("1024000"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devn2n3 = p2p.Install (n2n3);


  p2p.SetQueue ("ns3::CustomQueue","MaxBytes",StringValue("2048"),"Enable",StringValue("0"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("4ms"));
  NetDeviceContainer devn3n4 = p2p.Install (n3n4);

  p2p.SetQueue ("ns3::CustomQueue","MaxBytes",StringValue("2048"),"Enable",StringValue("0"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("5ms"));
  NetDeviceContainer devn3n5 = p2p.Install (n3n5);

  //ipv4
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (devn0n2);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (devn1n2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (devn2n3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (devn3n4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i5 = ipv4.Assign (devn3n5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i6i2 = ipv4.Assign (devn6n2);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i7i2 = ipv4.Assign (devn7n2);

  // Set up the routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  int stopTime = 50;
  //receiver n4
  uint16_t serverPort = 8080;
  Address serverAddress = i3i4.GetAddress (1);
  Ptr<CustomServer> serverApp = CreateObject<CustomServer> ();
  serverApp->Setup (serverPort);
  n3n4.Get(1)->AddApplication (serverApp);
  serverApp->SetStartTime (Seconds (0.));
  serverApp->SetStopTime (Seconds (stopTime));

  //sender  n0
  Ptr<CustomClient> app0 = CreateObject<CustomClient> ();
  app0->Setup (serverAddress, serverPort, 1024, 0, DataRate ("50Mbps"));
  n0n2.Get(0)->AddApplication (app0);
  app0->SetStartTime (Seconds (0.5));
  app0->SetStopTime (Seconds (stopTime));
  //sender  n1
  Ptr<CustomClient> app1 = CreateObject<CustomClient> ();
  app1->Setup (serverAddress, serverPort, 1024, 0, DataRate ("50Mbps"));
  n1n2.Get(0)->AddApplication (app1);
  app1->SetStartTime (Seconds (0.5));
  app1->SetStopTime (Seconds (stopTime));
  //sender  n6
  Ptr<CustomClient> app6 = CreateObject<CustomClient> ();
  app6->Setup (serverAddress, serverPort, 1024, 0, DataRate ("50Mbps"));
  n6n2.Get(0)->AddApplication (app6);
  app6->SetStartTime (Seconds (0.5));
  app6->SetStopTime (Seconds (stopTime));
  //sender  n7
  Ptr<CustomClient> app7 = CreateObject<CustomClient> ();
  app7->Setup (serverAddress, serverPort, 1024, 0, DataRate ("50Mbps"));
  n7n2.Get(0)->AddApplication (app7);
  app7->SetStartTime (Seconds (0.5));
  app7->SetStopTime (Seconds (stopTime));
  
  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll(ascii.CreateFileStream("my-test.tr"));

  Simulator::Stop (Seconds (stopTime));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}


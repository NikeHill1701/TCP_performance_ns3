#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"
#include "ns3/gnuplot-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Group 14 Assignment 4 Wireless TCP Performance Check");
Ptr<PacketSink> sink;                         /*packet sink application's pointer */
int main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
  //Here we have set the default TCP congestion algorithm to TCP New Reno
  std :: string plotTitle1               = "Plot of TCP Fairness Index against Packet Size";
  std :: string dataTitle1               = "Index Of Fairness ";

  //setting up fairness plot
  std :: string fairnessFile            = "WirelessFairnessPlot";
  std :: string fairnessGraphics        = FairnessFile + ".png";
  std :: string fairnessPlot            = FairnessFile + ".plt";
    //graphics file is created by plot file when we run Gnuplotto create the final plot in a PNG file.
  Gnuplot plot1 (fairnessGraphics);
  plot1.SetTitle (plotTitle1); 
  plot1.SetTerminal ("png");
  plot1.SetLegend ("Packet Size", "Fairness Index");
  plot1.AppendExtra ("set xrange [0:1550]");
  //setting dataset, title of plot, legend and style of plotting.
  Gnuplot2dDataset fairnessDataset;
  fairnessDataset.SetTitle (dataTitle1);
  fairnessDataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
   //setting up the dataset
  std :: string plotTitle2               = "Plot of throughput against Packetsize";
  std :: string dataTitle2               = "Throughput";

  //setting up Throughput Plot 
  std :: string throughputFile           = "WirelessThroughputPlot";
  std :: string throughputGraphics       = ThroughputFile + ".png";
  std :: string throughputPlot           = ThroughputFile + ".plt";
  Gnuplot plot2 (throughputGraphics);
  plot2.SetTitle (plotTitle2);
  plot2.SetTerminal ("png");
  plot2.SetLegend ("Packet Size", "Throughput");
  plot2.AppendExtra ("set xrange [0:1550]");
    //setting dataset, title of plot, legend and style of plotting. same as the previous graph.
  Gnuplot2dDataset throughputDataset;
  throughputDataset.SetTitle (dataTitle2);
  throughputDataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  //------------------plotting part initialisatoin done--------------
 
  uint32_t packets[10] = {40,44,48,52,60,552,576,628,1420,1500};
  //Packet sizes as given in the assignment.
  uint32_t j;
  for(j=0;j<10;j++)
  {
    std::cout << "Packet Size : " << packets[j] << std::endl;
    /* Configure TCP Options */
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (packets[j]));

    WifiMacHelper apWifiMac,staWifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);

    /* Set up Legacy Channel */
    YansWifiChannelHelper wifiChannel1,wifiChannel2;
    wifiChannel1.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel1.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));
    wifiChannel2.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel2.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

    /* Setup of Physical Layer */
    YansWifiPhyHelper wifiPhy1 = YansWifiPhyHelper::Default ();
    wifiPhy1.SetChannel (wifiChannel1.Create ());
    wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue ("HtMcs7"),"ControlMode", StringValue ("HtMcs0"));
    /* Setup of Physical Layer */
    YansWifiPhyHelper wifiPhy2 = YansWifiPhyHelper::Default ();
    wifiPhy2.SetChannel (wifiChannel2.Create ());
    wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue ("HtMcs7"),"ControlMode", StringValue ("HtMcs0"));

    //Creating  Nodes
    NS_LOG_INFO("Creating Nodes");
    Ptr<Node> n0 = CreateObject<Node> ();
    Ptr<Node> bs1 = CreateObject<Node> ();
    Ptr<Node> bs2 = CreateObject<Node> ();
    Ptr<Node> n1 = CreateObject<Node> ();

    //Wireless TCP Network setup
    NodeContainer baseStationNode = NodeContainer(bs1,bs2);
    NodeContainer staWifiNode = NodeContainer(n0,n1);
    NodeContainer all = NodeContainer(n0,n1,bs1,bs2);

    //bs1-bs2 point to point connection
    PointToPointHelper p2p;
    p2p.SetChannelAttribute ("Delay", StringValue ("100ms"));
    p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    //setting delay and data rate as asked in question
    NetDeviceContainer basedevice = p2p.Install (baseStationNode);

    NetDeviceContainer apDevice1,apDevice2,staDevices1,staDevices2;
    /* Configure AccessPoint */
    Ssid ssid = Ssid ("wireless-network");
    apWifiMac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));

    apDevice1 = wifiHelper.Install (wifiPhy1, apWifiMac, bs1);
    apDevice2 = wifiHelper.Install (wifiPhy2, apWifiMac, bs2);

    /* Configure STA */
    staWifiMac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid));

    staDevices1 = wifiHelper.Install (wifiPhy1, staWifiMac, n0);
    staDevices2 = wifiHelper.Install (wifiPhy2, staWifiMac, n1);

    /* Mobility model */
    MobilityHelper mobility;
    //Position Allocation for wireless network 1
    Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator> ();
    positionAlloc1->Add (Vector (0.0, 0.0, 0.0));
    positionAlloc1->Add (Vector (5.0, 0.0, 0.0));
    mobility.SetPositionAllocator (positionAlloc1);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (n0);
    mobility.Install (bs1);

    //Position Allocation for wireless network 2
    Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
    positionAlloc2->Add (Vector (10.0, 0.0, 0.0));
    positionAlloc2->Add (Vector (15.0, 0.0, 0.0));
    mobility.SetPositionAllocator (positionAlloc2);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (bs2);
    mobility.Install (n1);

    /* Internet stack helper */
    InternetStackHelper stack;
    stack.Install (all);

    //Assign Ipv4 address to Netdevices
    Ipv4AddressHelper ipv4;
    Ipv4InterfaceContainer apInterface1,apInterface2,staInterface1,staInterface2;

    ipv4.SetBase ("10.0.1.0", "255.255.255.0");
    staInterface1 = ipv4.Assign (staDevices1);
    apInterface1 = ipv4.Assign (apDevice1);

    ipv4.SetBase ("10.0.2.0", "255.255.255.0");
    apInterface2 = ipv4.Assign (apDevice2);
    staInterface2 = ipv4.Assign (staDevices2);

    ipv4.SetBase ("10.0.3.0", "255.255.255.0");
    Ipv4InterfaceContainer baseinterface = ipv4.Assign (basedevice);
    std::cout <<"Interface Addresses"  <<"\n";
    std::cout <<"Node0 : " << staInterface1.GetAddress(0) << "\t";
    std::cout <<"BaseStation1 : " << apInterface1.GetAddress(0) << "\t";
    std::cout <<"BaseStation2 : " << apInterface2.GetAddress(0) << "\t";
    std::cout <<"Node1 : " << staInterface2.GetAddress(0) << std::endl;
    std::cout << std::endl;

    /* Populate the routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    //TCP connection from n0 to n1
    // Install TCP Receiver on n1
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
    ApplicationContainer sinkApp = sinkHelper.Install (n1);
    sink = StaticCast<PacketSink> (sinkApp.Get(0));

    // Install TCP Transmitter on n0
    OnOffHelper serverHelper ("ns3::TcpSocketFactory", (InetSocketAddress (staInterface2.GetAddress (0), 9)));
    serverHelper.SetAttribute ("PacketSize", UintegerValue (packets[j]));
    serverHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    serverHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    serverHelper.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mbps")));
    ApplicationContainer serverApp = serverHelper.Install (n0);

    /* Start Applications */
    sinkApp.Start (Seconds (0.0));
    serverApp.Start (Seconds (10.0));

    FlowMonitorHelper flowmonitor;
    Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop (Seconds(11.0));
    Simulator::Run ();

    //Fairness Index and throughput calculation
    double Sumx = 0, SumSqx = 0;
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

      std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") Source Port :" << t.sourcePort << " Destination Port :" << t.destinationPort << "\n";
      std::cout << "  Transmitted Bytes\t:" << i->second.txBytes << " bytes \n";
      std::cout << "  Received Bytes\t:" << i->second.rxBytes << " bytes\n";
      double time = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
      std::cout << "  Transmission Time\t:" << time << "s\n";
      double throughput = ((i->second.rxBytes * 8.0) / time)/1024;
      std::cout << "  Throughput observed\t:" << throughput  << " Kbps\n\n";

      Sumx += throughput;
      SumSqx += throughput * throughput ;
    }

    double FairnessIndex = (Sumx * Sumx)/ (2 * SumSqx) ;
    std :: cout << "Average Throughput: \t" << Sumx/2 << " Kbps" << std::endl;
    std :: cout << "FairnessIndex: \t" << FairnessIndex << std :: endl << std::endl;
    std :: cout << "***********************************************************" << std::endl << std::endl;
    fairnessDataset.Add (packets[j], FairnessIndex);
    throughputDataset.Add(packets[j],Sumx/2);
    Simulator::Destroy ();
    NS_LOG_INFO ("Complete");
  }

  //plotting  fairness 
  // Add the dataset to the plot.
  plot1.AddDataset (FairnessDataset);
  // Open the plot file.
  std :: ofstream plotFile1 (FairnessPlot.c_str());
  // Write the plot file.
  plot1.GenerateOutput (plotFile1);
  // Close the plot file.
  plotFile1.close ();

  //plotting throughput
  // Add the dataset to the plot.
  plot2.AddDataset (ThroughputDataset);
  // Open the plot file.
  std :: ofstream plotFile2 (ThroughputPlot.c_str());
  // Write the plot file.
  plot2.GenerateOutput (plotFile2);
  // Close the plot file.
  plotFile2.close ();
  return 0;
}

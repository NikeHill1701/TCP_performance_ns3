#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/config-store-module.h"
#include "ns3/gnuplot.h"
#include "ns3/gnuplot-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("wiredTCP_Simulation");


class MyApp : public Application {
public:
	MyApp ();
	virtual ~MyApp();

	void Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate datarate);

private:
	virtual void StartApplication (void);
  	virtual void StopApplication (void);
  	
  	void ScheduleTx (void);
  	void SendPacket (void);

  	Ptr<Socket> m_socket;
  	Address m_peer;
  	uint32_t m_packetSize;
  	uint32_t m_nPackets;
  	DataRate m_dataRate;
  	EventId m_sendEvent;
  	bool m_running;
  	uint32_t m_packetsSent;
};

// constructor
MyApp::MyApp ()
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_nPackets (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void MyApp::Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate datarate){
	m_socket = socket;
	m_peer = address;
	m_packetSize = packetSize;
	m_nPackets = nPackets;
	m_dataRate = datarate;
}

void MyApp::StartApplication() {
	m_running = true;
	m_packetsSent = 0;
	m_socket->Bind();
	m_socket->Connect(m_peer);
	SendPacket();
}

void MyApp::StopApplication() {
	m_running = false;

	if(m_sendEvent.IsRunning()){
		Simulator::Cancel(m_sendEvent);
	}
	if(m_socket){
		m_socket->Close();
	}
}

void MyApp::SendPacket() {
	Ptr<Packet> packet = Create<Packet>(m_packetSize);
	m_socket->Send(packet);

	if(++m_packetsSent < m_nPackets){
		ScheduleTx();
	}
}

void 
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

int main(int argc, char *argv[]) {

	LogComponentEnable("wiredTCP_Simulation", LOG_LEVEL_INFO);

// loop over different packet sizes
	std::vector<uint32_t> packetSizes = {40, 44, 48, 52, 60, 250, 300, 552, 576, 628, 1420, 1500};
	uint32_t nPackets = 100;

	for(auto it : packetSizes) {
		uint32_t packetSize = it;
// Setup the netdevices
	// create the nodes
		Ptr<Node> n2 = CreateObject<Node> ();
		Ptr<Node> r1 = CreateObject<Node> ();
		Ptr<Node> r2 = CreateObject<Node> ();
		Ptr<Node> n3 = CreateObject<Node> ();

	// Setup connection media and the physical layer
		// container for each link
		NodeContainer n2r1 = NodeContainer(n2,r1);
		NodeContainer r1r2 = NodeContainer(r1,r2);
		NodeContainer r2n3 = NodeContainer(r2,n3);
		NodeContainer all = NodeContainer(n2,r1,r2,n3);

		// configure p2p links
		PointToPointHelper p2p_n2r1;
		p2p_n2r1.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
		p2p_n2r1.SetChannelAttribute("Delay", StringValue("20ms"));
		NetDeviceContainer device_n2r1;
		device_n2r1 = p2p_n2r1.Install(n2r1);

		PointToPointHelper p2p_r1r2;
		// p2p_r1r2.SetQueue("ns3::DropTailQueue","Mode",EnumValue (DropTailQueue::QUEUE_MODE_BYTES), "MaxBytes", UintegerValue(2048));
		// Droptails Queues are installed by default at the device level.
		p2p_r1r2.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
		p2p_r1r2.SetChannelAttribute("Delay", StringValue("50ms"));
		NetDeviceContainer device_r1r2;
		device_r1r2 = p2p_r1r2.Install(r1r2);

		PointToPointHelper p2p_r2n3;
		p2p_r2n3.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
		p2p_r2n3.SetChannelAttribute("Delay", StringValue("20ms"));
		NetDeviceContainer device_r2n3;
		device_r2n3 = p2p_r2n3.Install(r2n3);

	// Setup the protocol stack
		// install internet stack
		NS_LOG_INFO("Install Internet Stack");
		InternetStackHelper stack;
		stack.Install(all);

	// Setup the Network layer
		// assign IP address to the nodes
		Ipv4AddressHelper ipaddress;
		ipaddress.SetBase("10.0.1.0", "255.255.255.0");
		Ipv4InterfaceContainer Ipinterface_n2r1 = ipaddress.Assign(device_n2r1);
		
		ipaddress.SetBase("10.0.2.0", "255.255.255.0");
		Ipv4InterfaceContainer Ipinterface_r1r2 = ipaddress.Assign(device_r1r2);
		
		ipaddress.SetBase("10.0.3.0", "255.255.255.0");
		Ipv4InterfaceContainer Ipinterface_r2n3 = ipaddress.Assign(device_r2n3);

		std::cout << "r1 ipaddr: " << Ipinterface_r1r2.GetAddress(0) << "  r2 ipaddr: " << Ipinterface_r1r2.GetAddress(1) << std::endl;
		std::cout << "n2 ipaddr: " << Ipinterface_n2r1.GetAddress(0) << "  r1 ipaddr: " << Ipinterface_n2r1.GetAddress(1) << std::endl;
		std::cout << "n3 ipaddr: " << Ipinterface_r2n3.GetAddress(0) << "  n3 ipaddr: " << Ipinterface_r2n3.GetAddress(1) << std::endl;

	// Populate the routing tables
		Ipv4GlobalRoutingHelper routingHelper;
		routingHelper.PopulateRoutingTables();

	// Setup the Application layer
		// creating tcp packetsink for the reciever on n3
		uint16_t port = 4200;
		Address sinkAddress(InetSocketAddress(Ipinterface_r2n3.GetAddress(1),port));
		PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkAddress);
		// Install a packetsink app in the application layer of n3
		ApplicationContainer sinkApp = sinkHelper.Install(n3);
		sinkApp.Start(Seconds(1.0));
		sinkApp.Stop(Seconds(20.0));

	// Setup a tcp sender application on n2
		// create apps for each tcp type
		std::vector<std::string> tcptypes = {"ns3::TcpVegas", "ns3::TcpVeno", "ns3::TcpWestwood"};
		for(int i = 0 ; i < 3 ; i++) {
			Ptr<MyApp> app = CreateObject<MyApp> ();
			TypeId tid = TypeId::LookupByName(tcptypes[i]);
			std::stringstream nodeId;
			nodeId << n2r1.Get(0)->GetId();
			std::string specificNode = "/NodeList/"+nodeId.str()+"/$ns3::TcpL4Protocol/SocketType";
			Config::Set (specificNode, TypeIdValue (tid));
			Ptr<Socket> tcpSocket = Socket::CreateSocket(n2r1.Get(0),TcpSocketFactory::GetTypeId());
			app->Setup(tcpSocket,sinkAddress,packetSize,nPackets,DataRate("33Mbps"));
			n2r1.Get (0)->AddApplication (app);
			app->SetStartTime(Seconds(1.0));
			app->SetStopTime(Seconds(20.0));
		}

	// Flow monitor to collect stats
		FlowMonitorHelper flowmonitor;
	    Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();
	    monitor->SerializeToXmlFile("wiredTCP_data.xml", true, true);

	//Simulation
	    NS_LOG_INFO("Run Simulation");
	    Simulator::Stop (Seconds(25.0));
	    Simulator::Run ();
    // Write logic to calculate Throughput
	    monitor->CheckForLostPackets();
	    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
	    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    	double Sumx = 0, SumSqx = 0;
    	int numberofflows=0;
    	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) {
	      	numberofflows++;
	      	Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

	      	std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") Source Port :" << t.sourcePort << " Destination Port :" << t.destinationPort << "\n";
	      	std::cout << "  Tx Bytes\t\t:" << i->second.txBytes << " bytes \n";
	      	std::cout << "  Rx Bytes\t\t:" << i->second.rxBytes << " bytes\n";
	      	double time = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
	      	std::cout << "  Transmission Time\t:" << time << "s\n";
	      	double throughput = ((i->second.rxBytes * 8.0) / time)/1024;
	      	std::cout << "  Throughput\t\t:" << throughput  << " Kbps\n\n";

	      	Sumx += throughput;
	      	SumSqx += throughput * throughput ;
    	}
    	
    	double FairnessIndex = (Sumx * Sumx)/ (numberofflows * SumSqx) ;
    	std :: cout << "Average Throughput: " << Sumx/numberofflows << " Kbps" << std::endl;
    	std :: cout << "FairnessIndex:	" << FairnessIndex << std :: endl << std::endl;
    	std :: cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl<< std::endl;
    	Simulator::Destroy ();
    	NS_LOG_INFO ("Done.");
	}
}

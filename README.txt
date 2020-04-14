# Question :Application #2
Compare the performance of TCP over wired and wireless networks. Consider a topology as described below.The network consists of two TCP sources Node0 and Node2, corresponding to two TCP destinations Node1 andNode3 respectively. Node2 and Node3 come in wired domain with two routers R1 and R2 (connected by a{10 Mbps, 50 ms} wired link) between them. Both the routers use drop-tail queues with queue size setaccording to bandwidth-delay product. Node0 comes in domain of Base Station 1 (BS1) and Node1 comes indomain of Base Station 2 (BS2). BS1 and BS2 are connected by a (10 Mbps, 100 ms) wired link. The hosts,i.e. Node0, Node1, Node2, Node3 are attached with (100 Mbps, 20ms) links to routers or base stations (asshown in the figure below). The sources (Node0 and Node2)) use three TCP agents (i.e. TCP Westwood,TCP Veno and TCP Vegas) to generate three different TCP flows. Study and plot the fairness index (Jain'sfairness index) and throughput change when the TCP packet size is varied; all the other parameter values arekept constant. You should use the following TCP packet size values (in Bytes): 40, 44, 48, 52, 60, 250, 300, 552,576, 628, 1420 and 1500 for your experiments. The throughput (in Kbps) and fairness index must be calculatedat steady-state. Make appropriate assumptions wherever necessary.
#Running the scripts
Create folders named wiredTCP and wirelessTCP inside scratch directory of ns3 directory.
Now copy and paste respective C++ source code files into these folders
Now run the command "./build.py --enable-examples --enable-tests" to build our files.
now run command "./waf run wiredTCP" to create plot files for wired network.
now run command "./waf run wirelessTCP" to create plot files for wireless network.
now execute the following command to generate the plots in PNG format.
"gnuplot wiredTCPfairness.plt"
"gnuplot wiredTCPthroughput.plt"
"gnuplot WirelessFairnessPlot.plt"
"gnuplot WirelessThroughputPlot.plt"

These are the required plots.
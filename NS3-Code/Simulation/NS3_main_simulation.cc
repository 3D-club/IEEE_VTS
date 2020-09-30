//Importing the required header files
#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/wifi-phy.h"
#include "ns3/netanim-module.h"
#include "ns3/tag.h"
#include "ns3/vector.h"
#include "ns3/nstime.h"
#include "ns3/log.h"
#include "ns3/simulator.h"


//For colorful console printing

#define YELLOW_CODE "\033[33m"
#define TEAL_CODE "\033[36m"
#define BOLD_CODE "\033[1m"
#define END_CODE "\033[0m"
using namespace ns3;

//Creating a custom tag to add timestamps to the packets

class CustomDataTag : public Tag {
public:

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize (TagBuffer i) const;
	virtual void Deserialize (TagBuffer i);
	virtual void Print (std::ostream & os) const;

	Time GetTimestamp ();
	void SetTimestamp (Time t);

	CustomDataTag();
	CustomDataTag(uint32_t node_id);
	virtual ~CustomDataTag();
private:

	Time m_timestamp;

};
//Initializing the constructor, destructor and other functions of class CustomDataTag
CustomDataTag::CustomDataTag() {
	m_timestamp = Simulator::Now();
}
CustomDataTag::CustomDataTag(uint32_t node_id) {
	m_timestamp = Simulator::Now();
}

CustomDataTag::~CustomDataTag() {
}

TypeId CustomDataTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CustomDataTag")
    .SetParent<Tag> ()
    .AddConstructor<CustomDataTag> ();
  return tid;
}
TypeId CustomDataTag::GetInstanceTypeId (void) const
{
  return CustomDataTag::GetTypeId ();
}


uint32_t CustomDataTag::GetSerializedSize (void) const
{
	return  sizeof (ns3::Time) ;
}
/**
 * The order of how you do Serialize() should match the order of Deserialize()
 */
void CustomDataTag::Serialize (TagBuffer i) const
{
	i.WriteDouble(m_timestamp.GetDouble());
}
/** This function reads data from a buffer and store it in class's instance variables.
 */
void CustomDataTag::Deserialize (TagBuffer i)
{
	//We extract what we stored first, so we extract the timestamp
	m_timestamp =  Time::FromDouble (i.ReadDouble(), Time::NS);;

}

void CustomDataTag::Print (std::ostream &os) const
{
  os  <<  "\t(" << m_timestamp  << ")" ;
}


Time CustomDataTag::GetTimestamp() {
	return m_timestamp;
}

void CustomDataTag::SetTimestamp(Time t) {
	m_timestamp = t;
}


class MyHeader : public Header
{
public:
// new methods
  void SetData (uint16_t data);
  uint16_t GetData (void);
  static TypeId GetTypeId (void);
  
// overridden from Header
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
private:
  uint16_t m_data;
};

//Enabling logging feature of ns3 for the program

NS_LOG_COMPONENT_DEFINE ("WaveExample1");

//The function used to detect the arrival of packets at application level
bool ReceivePacket (Ptr<NetDevice> device, Ptr<const Packet> packet,uint16_t protocol, const Address &sender)
{
    NS_LOG_FUNCTION (device << packet << protocol << sender);

    CustomDataTag tag;
    if (packet->PeekPacketTag (tag))
    {
        std::cout<< "\tPacket Timestamp: " << tag.GetTimestamp() << " delay="<< Now()-tag.GetTimestamp()<< "  ";
    }

    uint8_t *buffer = new uint8_t[packet->GetSize ()];
    uint32_t size = packet->CopyData(buffer, packet->GetSize ());
    uint8_t xmessage[sizeof(double)],ymessage[sizeof(double)],zmessage[sizeof(double)];
//Reading the info in the packet to a buffer and printing it out.
    for(int i=0;i<3;++i){
      for(int j=0;j<8;++j){
        switch(i){
          case 0:
              xmessage[j] = buffer[j];
              break;
          case 1:
              ymessage[j] = buffer[j+8];
              break;
          case 2:
              zmessage[j] = buffer[j+2*8];
              break;        
        }
      }
    }
    double x,y,z;
    memcpy(&x, xmessage, sizeof(double));
    memcpy(&y, ymessage, sizeof(double));
    memcpy(&z, zmessage, sizeof(double));
    std::cout<<"Received:"<<size<<"  "<<x<<"  "<<y<<" "<<z<<std::endl;
    return true;
}

//The function used to initialize the transmission buffer, Create packets and send them into the network.

void SendPacket(Ptr <WaveNetDevice> wd0,Mac48Address dest,uint16_t protocol,TxInfo tx,NodeContainer nodes, uint8_t i){
//Getting the position information of the nodes
Vector position = nodes.Get(0)->GetObject<MobilityModel>()->GetPosition();
//Adding position information of survey drone to the buffer.
  uint16_t packetSize = sizeof(double)*3;
  uint8_t buffer[packetSize];
  uint8_t* xmessage = (uint8_t *) &position.x;
  uint8_t* ymessage = (uint8_t *) &position.y;
  uint8_t* zmessage = (uint8_t *) &position.z;
  int size = sizeof(double);
  for(int i=0;i<3;++i){
    for(int j=0;j<size;++j){
    switch(i){
      case 0:
          buffer[j] = xmessage[j];
          break;
      case 1:
          buffer[j+size] = ymessage[j];
          break;
      case 2:
          buffer[j+2*size] = zmessage[j];
          break;        
    }
    }
  }
//Creating a packet with the initialized buffer
 
Ptr<Packet> packet = Create<Packet>(buffer, packetSize);
CustomDataTag tag;

//Adding a timestamp to the custom tag before sending the packet

Time t =1000*1000*1000*Time(i);
tag.SetTimestamp(t);
packet->AddPacketTag (tag);
//Firing the packet into the network with scheduler and SendX function of WaveNetdevice
   Simulator::ScheduleNow ( &WaveNetDevice::SendX, wd0, packet, dest, protocol, tx);
}

// This is a promiscuous trace for all packet reception. This is in physical layer, so packets still have WifiMacHeader
void Rx (std::string context, Ptr <const Packet> packet, uint16_t channelFreqMhz,  WifiTxVector txVector,MpduInfo aMpdu, SignalNoiseDbm signalNoise)
{
	std::cout << BOLD_CODE <<  context << END_CODE << std::endl;
//Print the info contained by packet
	std::cout << "\tSize=" << packet->GetSize()
			  << " Freq="<<channelFreqMhz
			  << " Mode=" << txVector.GetMode()
			  << " Signal=" << signalNoise.signal
			  << " Noise=" << signalNoise.noise << std::endl;
  

//We can also examine the WifiMacHeader
	WifiMacHeader hdr;
	if (packet->PeekHeader(hdr))
	{
		std::cout << "\tDestination MAC : " << hdr.GetAddr1() << "\tSource MAC : " << hdr.GetAddr2() << std::endl;
	}
}

//The trace callback to detect packet drops in the network 
void RxDrop (std::string context, Ptr<const Packet> packet)
{
	std::cout << BOLD_CODE << YELLOW_CODE << "Packet Rx Dropped!" << END_CODE << std::endl;
	std::cout << context << std::endl;

	 WifiMacHeader hdr;
	if (packet->PeekHeader(hdr))
	{

		std::cout << " Destination MAC : " << hdr.GetAddr1() << "\tSource MAC : " << hdr.GetAddr2() << "\tSeq No. " << hdr.GetSequenceNumber() << std::endl;
	}
  
}

//Fired when a packet is Enqueued in MAC
void EnqueueTrace(std::string context, Ptr<const WifiMacQueueItem> item)
{
	std::cout << TEAL_CODE << "A Packet was enqueued : " << context << END_CODE << std::endl;
	Ptr <const Packet> p = item->GetPacket();	
}

//Fired when a packet is Dequeued from MAC layer. A packet is dequeued before it is transmitted.
void DequeueTrace(std::string context, Ptr<const WifiMacQueueItem> item)
{
	std::cout << TEAL_CODE << "A Packet was dequeued : " << context << END_CODE << std::endl;

	Ptr <const Packet> p = item->GetPacket();
	Time queue_delay = Simulator::Now() - item->GetTimeStamp();

//A packet can get dequeued (dropped_ if it exceeded MaxDelay (default is 500ms)
	std::cout << "\tQueuing delay=" << queue_delay << std::endl;
}
//The main function

int main (int argc, char *argv[])
{
  CommandLine cmd;

 //Number of nodes
  uint32_t nNodes = 3;

  cmd.AddValue ("n","Number of nodes", nNodes);

  cmd.Parse (argc, argv);
	
  double simTime = 110.0;
//Creating the nodes using nodecontainer
  NodeContainer nodes;
  nodes.Create(nNodes);
// Convention of nodes: node0 : "groundstation", node1 : "survey drone", node2 : "payload drone"
  ns3::PacketMetadata::Enable ();
//Setting up mobility helper and initial positions
 
 MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (20.0, 15.0, 10.0));
  positionAlloc->Add (Vector (5.0, 0.0, 0.0));
  positionAlloc->Add (Vector (5.0, 10.0, 0.0));

  mobility.SetPositionAllocator (positionAlloc);
//Setting up the 3d gauss markov mobility model
  
  mobility.SetMobilityModel ("ns3::GaussMarkovMobilityModel",
  "Bounds", BoxValue (Box (0, 1500, 0, 1500, 0, 100)),
  "TimeStep", TimeValue (Seconds (0.5)),
  "Alpha", DoubleValue (0.85),
  "MeanVelocity", StringValue ("ns3::UniformRandomVariable[Min=8|Max=12]"),
  "MeanDirection", StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"),
  "MeanPitch", StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"),
  "NormalVelocity", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"),
  "NormalDirection", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"),
  "NormalPitch", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"));
  mobility.Install (nodes.Get(0));
  mobility.Install(nodes.Get(1));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(2));
  YansWifiChannelHelper waveChannel = YansWifiChannelHelper::Default ();

//Adding nakagami-m loss model to make the simulation more realistic along with the default logdistance model
  
  waveChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
  YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
  wavePhy.SetChannel (waveChannel.Create ());
  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);

//Setting PowerLevels
  wavePhy.Set ("TxPowerStart", DoubleValue (8) );
  wavePhy.Set ("TxPowerEnd", DoubleValue (33) );
  
  wavePhy.Set ("TxPowerLevels", UintegerValue(3));
//Initializing WaveHelper and mac

  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();
//Setting up a constant rate wifi manager for stable data rate with the data rate being 6Mbps OFDM with a bandwidth of 10MHz
  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate6MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate6MbpsBW10MHz"),
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"));


  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, nodes);
//Enabling packet capture and ascii trace for all nodes
  
  wavePhy.EnablePcap ("ProjectTest", devices);
  AsciiTraceHelper ascii;
  wavePhy.EnableAsciiAll(ascii.CreateFileStream("wave20.tr"));

//Getting the mac address of ground station and payload
  
  Ptr <WaveNetDevice> d2 = DynamicCast<WaveNetDevice>(devices.Get(2));
//dest mac address contains the address for ground station
  Mac48Address dest = Mac48Address::ConvertFrom (d2->GetAddress());
  Ptr <WaveNetDevice> d1 = DynamicCast<WaveNetDevice>(devices.Get(1));
//dest mac address contains the address for payload drone
  Mac48Address dest2 = Mac48Address::ConvertFrom (d1->GetAddress());
//Setting the transfer and network layer protocols or (ether type) to WSMP using the protocol variable. 
  uint16_t protocol = 0x88dc;

  //We can also set the transmission parameters at the higher layeres
  TxInfo tx;

  //We set the channel on which the packet is sent. The WaveNetDevice must have access to the channel
  //Setting the (CCH) Control Channel as transmission channel for faster data transfer
  tx.channelNumber = CCH;

  //We can set per-packet data rate. This packet will have a rate of 12Mbps.
  tx.dataRate = WifiMode ("OfdmRate12MbpsBW10MHz");
  /*
   * Set the Access Catogory (AC) of the packet.
   * The 802.11e EDCA standard defines 4 AC's named Voice, Video, Best Effort & Background in order of priority.
   * The value determines the EdcaQueue in which the packet will be enqueued.
   *
   * The 'tid' is a value from 0-7 that maps to ACs as follows
   * 1 or 2 : Background (Lowest priority)
   * 0 or 3 : Best effort
   * 4 or 5 : Video
   * 6 or 7 : Voice (Highest priority)
   */
  tx.priority = 7;	//We set the AC to highest priority. We can set this per packet.

  /*
   * We can also set TxPower. This maps to the user define TxPowerStart & TxPowerEnd.
   * 7 : corresponds to the highest user-defined power (TxPowerEnd). In this code, it's 33 dBm
   * 1 : lowest (TxPowerStart). In this code, it's 8.3 dBm
   *
   * We'll have N equally spaced power levels.
   * A value of 8 indicates application don't want to set power or data rate. Values >8 are invalid.
   */
  tx.txPowerLevel = 7; //When we define TxPowerStart & TxPowerEnd for a WifiPhy, 7 is correspond to TxPowerEnd, and 1 TxPowerStart, and numbers in between are levels.



  //Get the WaveNetDevice for the first devices, using node 0.
  Ptr <NetDevice> d0 = devices.Get (0);
  Ptr <WaveNetDevice> wd0 = DynamicCast <WaveNetDevice> (d0);
  //Calling the schedule function to initiate the SendPacket function every 1 second.
  for(uint8_t i=2;i<100;i++){
//calling the SendPacket function to send the packet to ground station
  Simulator::Schedule (Seconds (i),&SendPacket, wd0,dest, protocol, tx, nodes,i);

//calling the SendPacket function to send the packet to payload drone
  Simulator::Schedule (Seconds (i),&SendPacket, wd0,dest2, protocol, tx, nodes,i);

  }
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/MonitorSnifferRx", MakeCallback (&Rx) );
  devices.Get(1)->SetReceiveCallback ( MakeCallback(&ReceivePacket) );
  devices.Get(2)->SetReceiveCallback ( MakeCallback(&ReceivePacket) );
  //Set the number of power levels.
  Config::Set("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/TxPowerLevels", ns3::UintegerValue(7));

  /*
   * What if some packets were dropped due to collision, or whatever? We use this trace to fire RxDrop function
   */
  Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxDrop", MakeCallback (&RxDrop) );

  /*
   * We can also trace some MAC layer details
   */
  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/$ns3::OcbWifiMac/*/Queue/Enqueue", MakeCallback (&EnqueueTrace));

  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/$ns3::OcbWifiMac/*/Queue/Dequeue", MakeCallback (&DequeueTrace));


//Configuring NetAnim for better visualization
  AnimationInterface anim ("anim_project.xml");
  anim.EnablePacketMetadata(true);
  anim.UpdateNodeSize(0,20,20);
  anim.UpdateNodeColor(0,0,255,0);
  anim.UpdateNodeDescription(0,"Surveilance drone");
  anim.UpdateNodeSize(1,20,20);
  anim.UpdateNodeColor(1,255,0,0);
  anim.UpdateNodeDescription(1,"Payload drone");
  anim.UpdateNodeSize(2,20,20);
  anim.UpdateNodeDescription(2,"Ground Station");
  anim.UpdateNodeColor(2,0,0,0);
   
  Simulator::Stop(Seconds(simTime));
  Simulator::Run();
  Simulator::Destroy();
  return 0;

}

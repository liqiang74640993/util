#include "mrp_common.h"


#define MVRP_PROTOCOL_VERSION  0x00   //MVRP ProtocolVersion
#define MVRP_ATTRIBUTE_TYPE 0x1     //MVRP AttributeType
#define MVRP_ATTRIBUTE_LENGTH 0x02  //MVRP AttributeLength

enum MMRP_VLAN_configuration
{
    MMRP_VLAN_CONFIGURATION_STATIC_ONLY, //static configuration only
    MMRP_VLAN_CONFIGURATION_STATIC_WITH_TOPOLOGY_CHANGE, //static configuration with topology change
    MMRP_VLAN_CONFIGURATION_DYNAMIC_ONLY,//dynamic configuration only 
    MMRP_VLAN_CONFIGURATION_STATIC_AND_DYNAMIC //combined static and dynamic configuration
};


void mmrp_procuder_packet_pack(uint8_t *packet,size_t packet_size)
{
      
}

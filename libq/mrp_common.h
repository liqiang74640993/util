#include <stdint.h>

struct list;

/**
 * 本文件是Q2018实现MRP协议(多注册协议)的共同头文件：包括一些常用的定义，参数，以及一些公用的接口，协议规范主要参考第10章
*/

#define CUSTOMER_PROVIDER_BRIDGE_MMRP_ADDRESS "01-80-C2-00-00-20"
#define CUSTOMER_BRIDGE_MVRP_ADDRESS "01-80-C2-00-00-21"  //Table 10-1 MVRP application address

//Table 10-2 MRP Ethertype values
#define MMRP_ETHERTYPE  "88-F6"
#define MVRP_ETHERTYPE  "88-F5"
#define MSRP_ETHERTYPE  "22-EA"
#define MIRP_ETHERTYPE  "89-29"

/// @brief 10.8.2.5 AttributeEvent 
enum mrpdu_attribute_event
{
    MRPDU_ATTRIBUTE_EVENT_NEW = 0, //New
    MRPDU_ATTRIBUTE_EVENT_JOININ, //JoinIn
    MRPDU_ATTRIBUTE_EVENT_IN, //In
    MRPDU_ATTRIBUTE_EVENT_JOINMT,//JoinEmpty
    MRPDU_ATTRIBUTE_EVENT_MT, //Empty
    MRPDU_ATTRIBUTE_EVENT_LV  //Leave
};

//LeaveAllEvent 10.8.2.6
enum mrpdu_leaveall_event
{
    MRPDU_LEAVEALL_EVENT_NULL, //NullLeaveAllEvent
    MRPDU_LEAVEALL_EVENT_ALL //LeaveAll
};

struct mrpdu_attribute
{
    uint16_t vector_header;//VectorHeader:
    uint8_t first_value;//FirstValue:
};

struct mrpdu_attribute_list
{
    int a ;
};

struct mrpdu_message
{
    uint8_t attribute_type;//AttributeType:1-255，由相关应用定义
    uint8_t attribute_length;// AttributeLength：指示消息所应用的属性的 FirstValue 字段的长度（10.8.2.7）
    uint16_t attribute_list_length;//AttributeListLength：指示消息所应用的属性的 AttributeList 字段的长度，MSRPDU使用
    struct list *attribute_list;//AttributeList
};

struct MRPDU
{
    uint8_t  protocol_version; //ProtocolVersion 
    struct list *mrpdu_message_list;//message_list
    uint16_t end_mark;//EndMark
};

/// @brief  编码MRPDU的VectorHeader ,参考10.8.2.8
/// @param mrpdu[in]:要编码的mrpdu
/// @param offset[in]: VectorHeader要编码的位置
/// @param leaveall_event[in]:LeaveAllEvent 参数
/// @param number_of_values[in]: number of events encoded in the vector 
/// @return 
int mrp_common_encode_vector_header(uint8_t *mrpdu, size_t offset, enum mrpdu_leaveall_event leaveall_event, size_t number_of_values);


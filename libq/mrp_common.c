#include "mrp_common.h"

int mrp_common_encode_vector_header(uint8_t *mrpdu, size_t offset, enum mrpdu_leaveall_event leaveall_event, size_t number_of_values)
{
    if(number_of_values > 8191) return -1;
    if(leaveall_event == MRPDU_LEAVEALL_EVENT_NULL && number_of_values == 0) return -1;
    uint16_t vector_header = leaveall_event * 8192 + number_of_values;
}
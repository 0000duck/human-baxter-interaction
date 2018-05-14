#ifndef _ROS_skeleton_msg_Skeleton_h
#define _ROS_skeleton_msg_Skeleton_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace skeleton_msg
{

  class Skeleton : public ros::Msg
  {
    public:
      float skeleton[75];

    Skeleton():
      skeleton()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      for( uint8_t i = 0; i < 75; i++){
      union {
        float real;
        uint32_t base;
      } u_skeletoni;
      u_skeletoni.real = this->skeleton[i];
      *(outbuffer + offset + 0) = (u_skeletoni.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_skeletoni.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_skeletoni.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_skeletoni.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->skeleton[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      for( uint8_t i = 0; i < 75; i++){
      union {
        float real;
        uint32_t base;
      } u_skeletoni;
      u_skeletoni.base = 0;
      u_skeletoni.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_skeletoni.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_skeletoni.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_skeletoni.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->skeleton[i] = u_skeletoni.real;
      offset += sizeof(this->skeleton[i]);
      }
     return offset;
    }

    const char * getType(){ return "skeleton_msg/Skeleton"; };
    const char * getMD5(){ return "735253556cc457d7ad78bb0055caa294"; };

  };

}
#endif
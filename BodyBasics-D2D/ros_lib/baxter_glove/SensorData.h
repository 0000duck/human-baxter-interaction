#ifndef _ROS_baxter_glove_SensorData_h
#define _ROS_baxter_glove_SensorData_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/Vector3.h"

namespace baxter_glove
{

  class SensorData : public ros::Msg
  {
    public:
      uint8_t sensor_id;
      geometry_msgs::Vector3 rpy;
      geometry_msgs::Vector3 A;
      geometry_msgs::Vector3 W;
      geometry_msgs::Vector3 M;

    SensorData():
      sensor_id(0),
      rpy(),
      A(),
      W(),
      M()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->sensor_id >> (8 * 0)) & 0xFF;
      offset += sizeof(this->sensor_id);
      offset += this->rpy.serialize(outbuffer + offset);
      offset += this->A.serialize(outbuffer + offset);
      offset += this->W.serialize(outbuffer + offset);
      offset += this->M.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->sensor_id =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->sensor_id);
      offset += this->rpy.deserialize(inbuffer + offset);
      offset += this->A.deserialize(inbuffer + offset);
      offset += this->W.deserialize(inbuffer + offset);
      offset += this->M.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "baxter_glove/SensorData"; };
    const char * getMD5(){ return "b63a29b1b6c43f44b1af5ed3b6540cd8"; };

  };

}
#endif
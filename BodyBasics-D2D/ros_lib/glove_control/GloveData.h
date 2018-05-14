#ifndef _ROS_glove_control_GloveData_h
#define _ROS_glove_control_GloveData_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "glove_control/SensorData.h"

namespace glove_control
{

  class GloveData : public ros::Msg
  {
    public:
      std_msgs::Header header;
      uint8_t sensor_datas_length;
      glove_control::SensorData st_sensor_datas;
      glove_control::SensorData * sensor_datas;

    GloveData():
      header(),
      sensor_datas_length(0), sensor_datas(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset++) = sensor_datas_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < sensor_datas_length; i++){
      offset += this->sensor_datas[i].serialize(outbuffer + offset);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t sensor_datas_lengthT = *(inbuffer + offset++);
      if(sensor_datas_lengthT > sensor_datas_length)
        this->sensor_datas = (glove_control::SensorData*)realloc(this->sensor_datas, sensor_datas_lengthT * sizeof(glove_control::SensorData));
      offset += 3;
      sensor_datas_length = sensor_datas_lengthT;
      for( uint8_t i = 0; i < sensor_datas_length; i++){
      offset += this->st_sensor_datas.deserialize(inbuffer + offset);
        memcpy( &(this->sensor_datas[i]), &(this->st_sensor_datas), sizeof(glove_control::SensorData));
      }
     return offset;
    }

    const char * getType(){ return "glove_control/GloveData"; };
    const char * getMD5(){ return "5622f412614f6a9c0ee8f9912edde9c6"; };

  };

}
#endif
#ifndef _ROS_SERVICE_getTrajectory_h
#define _ROS_SERVICE_getTrajectory_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "dmp/TrajectoryData.h"

namespace dmp
{

static const char GETTRAJECTORY[] = "dmp/getTrajectory";

  class getTrajectoryRequest : public ros::Msg
  {
    public:
      dmp::TrajectoryData inputData;

    getTrajectoryRequest():
      inputData()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->inputData.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->inputData.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETTRAJECTORY; };
    const char * getMD5(){ return "36c35c4d9a97880ebd2754cec6c3c501"; };

  };

  class getTrajectoryResponse : public ros::Msg
  {
    public:
      const char* trajectory;

    getTrajectoryResponse():
      trajectory("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      uint32_t length_trajectory = strlen(this->trajectory);
      memcpy(outbuffer + offset, &length_trajectory, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->trajectory, length_trajectory);
      offset += length_trajectory;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t length_trajectory;
      memcpy(&length_trajectory, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_trajectory; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_trajectory-1]=0;
      this->trajectory = (char *)(inbuffer + offset-1);
      offset += length_trajectory;
     return offset;
    }

    const char * getType(){ return GETTRAJECTORY; };
    const char * getMD5(){ return "b3fa000c49300ef639781bfa3051006f"; };

  };

  class getTrajectory {
    public:
    typedef getTrajectoryRequest Request;
    typedef getTrajectoryResponse Response;
  };

}
#endif

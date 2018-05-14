#ifndef _ROS_SERVICE_integration_h
#define _ROS_SERVICE_integration_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "dmp/TrajectoryData.h"

namespace dmp
{

static const char INTEGRATION[] = "dmp/integration";

  class integrationRequest : public ros::Msg
  {
    public:
      dmp::TrajectoryData test;

    integrationRequest():
      test()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->test.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->test.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return INTEGRATION; };
    const char * getMD5(){ return "c131538393a8c6a77589e784f942f666"; };

  };

  class integrationResponse : public ros::Msg
  {
    public:
      const char* goal;

    integrationResponse():
      goal("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      uint32_t length_goal = strlen(this->goal);
      memcpy(outbuffer + offset, &length_goal, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->goal, length_goal);
      offset += length_goal;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t length_goal;
      memcpy(&length_goal, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_goal; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_goal-1]=0;
      this->goal = (char *)(inbuffer + offset-1);
      offset += length_goal;
     return offset;
    }

    const char * getType(){ return INTEGRATION; };
    const char * getMD5(){ return "6e8c62b390dd09eb60220ad7325cbdf7"; };

  };

  class integration {
    public:
    typedef integrationRequest Request;
    typedef integrationResponse Response;
  };

}
#endif

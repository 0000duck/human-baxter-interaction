#ifndef _ROS_SERVICE_getWeight_h
#define _ROS_SERVICE_getWeight_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace dmp
{

static const char GETWEIGHT[] = "dmp/getWeight";

  class getWeightRequest : public ros::Msg
  {
    public:
      const char* readFile;
      const char* writeFile;

    getWeightRequest():
      readFile(""),
      writeFile("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      uint32_t length_readFile = strlen(this->readFile);
      memcpy(outbuffer + offset, &length_readFile, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->readFile, length_readFile);
      offset += length_readFile;
      uint32_t length_writeFile = strlen(this->writeFile);
      memcpy(outbuffer + offset, &length_writeFile, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->writeFile, length_writeFile);
      offset += length_writeFile;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t length_readFile;
      memcpy(&length_readFile, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_readFile; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_readFile-1]=0;
      this->readFile = (char *)(inbuffer + offset-1);
      offset += length_readFile;
      uint32_t length_writeFile;
      memcpy(&length_writeFile, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_writeFile; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_writeFile-1]=0;
      this->writeFile = (char *)(inbuffer + offset-1);
      offset += length_writeFile;
     return offset;
    }

    const char * getType(){ return GETWEIGHT; };
    const char * getMD5(){ return "1f717666d819753c91f0bb9fbbc96b1f"; };

  };

  class getWeightResponse : public ros::Msg
  {
    public:
      const char* weight;

    getWeightResponse():
      weight("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      uint32_t length_weight = strlen(this->weight);
      memcpy(outbuffer + offset, &length_weight, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->weight, length_weight);
      offset += length_weight;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t length_weight;
      memcpy(&length_weight, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_weight; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_weight-1]=0;
      this->weight = (char *)(inbuffer + offset-1);
      offset += length_weight;
     return offset;
    }

    const char * getType(){ return GETWEIGHT; };
    const char * getMD5(){ return "a3f885f5593bdbf1f43ed1fe6e80ebe9"; };

  };

  class getWeight {
    public:
    typedef getWeightRequest Request;
    typedef getWeightResponse Response;
  };

}
#endif

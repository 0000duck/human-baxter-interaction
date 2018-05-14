#ifndef _ROS_dmp_TrajectoryData_h
#define _ROS_dmp_TrajectoryData_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/Pose.h"

namespace dmp
{

  class TrajectoryData : public ros::Msg
  {
    public:
      const char* readFile;
      const char* writeFile;
      float duration;
      geometry_msgs::Pose startPoint;
      geometry_msgs::Pose endPoint;
      float s_dx;
      float s_dy;
      float s_dz;
      float e_dx;
      float e_dy;
      float e_dz;

    TrajectoryData():
      readFile(""),
      writeFile(""),
      duration(0),
      startPoint(),
      endPoint(),
      s_dx(0),
      s_dy(0),
      s_dz(0),
      e_dx(0),
      e_dy(0),
      e_dz(0)
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
      union {
        float real;
        uint32_t base;
      } u_duration;
      u_duration.real = this->duration;
      *(outbuffer + offset + 0) = (u_duration.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_duration.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_duration.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_duration.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->duration);
      offset += this->startPoint.serialize(outbuffer + offset);
      offset += this->endPoint.serialize(outbuffer + offset);
      union {
        float real;
        uint32_t base;
      } u_s_dx;
      u_s_dx.real = this->s_dx;
      *(outbuffer + offset + 0) = (u_s_dx.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_s_dx.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_s_dx.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_s_dx.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->s_dx);
      union {
        float real;
        uint32_t base;
      } u_s_dy;
      u_s_dy.real = this->s_dy;
      *(outbuffer + offset + 0) = (u_s_dy.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_s_dy.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_s_dy.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_s_dy.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->s_dy);
      union {
        float real;
        uint32_t base;
      } u_s_dz;
      u_s_dz.real = this->s_dz;
      *(outbuffer + offset + 0) = (u_s_dz.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_s_dz.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_s_dz.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_s_dz.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->s_dz);
      union {
        float real;
        uint32_t base;
      } u_e_dx;
      u_e_dx.real = this->e_dx;
      *(outbuffer + offset + 0) = (u_e_dx.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_e_dx.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_e_dx.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_e_dx.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->e_dx);
      union {
        float real;
        uint32_t base;
      } u_e_dy;
      u_e_dy.real = this->e_dy;
      *(outbuffer + offset + 0) = (u_e_dy.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_e_dy.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_e_dy.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_e_dy.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->e_dy);
      union {
        float real;
        uint32_t base;
      } u_e_dz;
      u_e_dz.real = this->e_dz;
      *(outbuffer + offset + 0) = (u_e_dz.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_e_dz.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_e_dz.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_e_dz.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->e_dz);
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
      union {
        float real;
        uint32_t base;
      } u_duration;
      u_duration.base = 0;
      u_duration.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_duration.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_duration.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_duration.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->duration = u_duration.real;
      offset += sizeof(this->duration);
      offset += this->startPoint.deserialize(inbuffer + offset);
      offset += this->endPoint.deserialize(inbuffer + offset);
      union {
        float real;
        uint32_t base;
      } u_s_dx;
      u_s_dx.base = 0;
      u_s_dx.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_s_dx.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_s_dx.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_s_dx.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->s_dx = u_s_dx.real;
      offset += sizeof(this->s_dx);
      union {
        float real;
        uint32_t base;
      } u_s_dy;
      u_s_dy.base = 0;
      u_s_dy.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_s_dy.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_s_dy.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_s_dy.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->s_dy = u_s_dy.real;
      offset += sizeof(this->s_dy);
      union {
        float real;
        uint32_t base;
      } u_s_dz;
      u_s_dz.base = 0;
      u_s_dz.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_s_dz.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_s_dz.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_s_dz.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->s_dz = u_s_dz.real;
      offset += sizeof(this->s_dz);
      union {
        float real;
        uint32_t base;
      } u_e_dx;
      u_e_dx.base = 0;
      u_e_dx.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_e_dx.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_e_dx.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_e_dx.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->e_dx = u_e_dx.real;
      offset += sizeof(this->e_dx);
      union {
        float real;
        uint32_t base;
      } u_e_dy;
      u_e_dy.base = 0;
      u_e_dy.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_e_dy.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_e_dy.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_e_dy.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->e_dy = u_e_dy.real;
      offset += sizeof(this->e_dy);
      union {
        float real;
        uint32_t base;
      } u_e_dz;
      u_e_dz.base = 0;
      u_e_dz.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_e_dz.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_e_dz.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_e_dz.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->e_dz = u_e_dz.real;
      offset += sizeof(this->e_dz);
     return offset;
    }

    const char * getType(){ return "dmp/TrajectoryData"; };
    const char * getMD5(){ return "4ae52263d8a5395bc0447f8b716d0f6c"; };

  };

}
#endif
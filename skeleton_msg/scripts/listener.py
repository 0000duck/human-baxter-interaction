#!/usr/bin/env python
import rospy
from skeleton_msg.msg import Skeleton
import os
import time
import sys
from socket import *

host = "127.0.0.1" # set to IP address of target computer
port = 13000
addr = (host, port)
UDPSock = socket(AF_INET, SOCK_DGRAM)


i = 0

def callback(data):
    # i = i + 1

    skeleton_string = str(data.skeleton)
    UDPSock.sendto(skeleton_string, addr)

    # print("the %d times"% (i))
    print("data size is %d" %(sys.getsizeof(skeleton_string)))
    print(skeleton_string)
    
def listener():

    rospy.init_node('listener', anonymous=True)
    rospy.Subscriber("cmd_vel", Skeleton, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

    UDPSock.close()
    os._exit(0)

if __name__ == '__main__':
    listener()



# i = 0
# while True:
#   # data = input("Enter message to send or type 'exit': ")

#   data = str(   [i for j in range(1000)]    )


#   print("data size is %d" %(sys.getsizeof(data)))
#   UDPSock.sendto(data.encode(), addr)
#   print(i)
#   i = i + 1
#   time.sleep(1)
#   if data == "exit":
#     break

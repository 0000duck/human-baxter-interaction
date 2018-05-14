#!/usr/bin/env python
import rospy
import baxter_interface
from geometry_msgs.msg import Pose

def callback(data):
	rospy.loginfo(data)
	
def subscribe_current_endpose():
    rospy.init_node('subscribe_current_endpose', anonymous=True)
    rospy.Subscriber("endpoint", Pose, callback)
    rospy.spin()

if __name__ == '__main__':
    subscribe_current_endpose()

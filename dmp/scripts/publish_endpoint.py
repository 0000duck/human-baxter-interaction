#!/usr/bin/env python

import rospy
import baxter_interface
from geometry_msgs.msg import Pose, Point, Quaternion

def publish_current_endpose():

    rospy.init_node('publish_current_endpose', anonymous=True)
    pub = rospy.Publisher('endpoint', Pose, queue_size=10)
    endpoint_pose = Pose()
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
    	limb_left = baxter_interface.Limb('left')
    	current_pose = limb_left.endpoint_pose()
    	endpoint_pose.position = Point(current_pose['position'].x,
	                     current_pose['position'].y,
	                     current_pose['position'].z)
        endpoint_pose.orientation = Quaternion(current_pose['orientation'].x,
                            current_pose['orientation'].y,
                            current_pose['orientation'].z,
                            current_pose['orientation'].w,)
    	print endpoint_pose
        pub.publish(endpoint_pose)
        rate.sleep()

if __name__ == '__main__':
    publish_current_endpose()
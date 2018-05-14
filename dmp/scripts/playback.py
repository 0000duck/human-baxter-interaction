#! /usr/bin/python2
import rospy,sys
import argparse
import moveit_commander
import numpy as np
from geometry_msgs.msg import PoseStamped,Pose
from moveit_commander import MoveGroupCommander
import time
from copy import deepcopy
from os import path
import baxter_interface
from baxter_interface import CHECK_VERSION

import os
from socket import *



class MoveItDemo:
	def __init__ (self):
		moveit_commander.roscpp_initialize(sys.argv)
		self.right_arm = moveit_commander.MoveGroupCommander('right_arm')
		self.right_eef = self.right_arm.get_end_effector_link()
		self.right_arm.allow_replanning(True)
		self.right_arm.set_goal_position_tolerance(0.5)
		self.right_arm.set_goal_orientation_tolerance(0.5)
		self.right_arm.set_planning_time(5)
		
	def execute(self,filename):
		with open(filename, 'r') as f:
			lines = f.readlines()
		waypoints = []
		wpose = Pose()

		for idx, values in enumerate(lines[1:]):
			values = values.rstrip().split(',')
			values = [float(x) for x in values]
			
			# if idx == 0:
				# start_pose = self.right_arm.get_current_pose(self.right_eef).pose
				# wpose = deepcopy(start_pose)
				# waypoints.append(deepcopy(wpose))

			wpose.position.x = values[1]
			wpose.position.y = values[2]
			wpose.position.z = values[3]
			wpose.orientation.x = values[4]
			wpose.orientation.y = values[5]
			wpose.orientation.z = values[6]
			wpose.orientation.w = values[7]
			waypoints.append(deepcopy(wpose))


		print ("***playback***\n")
		(cartesian_plan_r,fraction) = self.right_arm.compute_cartesian_path(
														waypoints,
														0.01, 
														0.0, 
														True)
		self.right_arm.execute(cartesian_plan_r)
		rospy.sleep(1)
		print("Done.")
		moveit_commander.roscpp_shutdown()
		moveit_commander.os._exit(0)	


def main():




	print("Initializing node... ")
	rospy.init_node('moveit_demo')
	rospy.sleep(1)


	
	moveit = MoveItDemo()

	moveit.execute("hello")

	UDPSock.close()
	os._exit(0)


if __name__ == "__main__":
	main()

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
import threading


host = ""
port = 14000
buf = 1024
addr = (host, port)
UDPSock = socket(AF_INET, SOCK_DGRAM)
UDPSock.bind(addr)

data_s = ""
mutex = threading.Lock()


def receive():
    global data_s,mutex
    while True:

        (data, addr) = UDPSock.recvfrom(4049)
        print "Received message: " + data

        if mutex.acquire():
            data_s = data
            mutex.release()


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
		# moveit_commander.roscpp_shutdown()
		# moveit_commander.os._exit(0)	


def main():

	global data_s,mutex


	t = threading.Thread(target=receive)
	t.setDaemon(True)
	t.start()


	print("Initializing node... ")
	rospy.init_node('moveit_demo')
	rospy.sleep(1)

	rs = baxter_interface.RobotEnable(CHECK_VERSION)
	limb = baxter_interface.Limb("right")
	# limb.move_to_neutral()
	
	moveit = MoveItDemo()

	
	print "Waiting to receive messages..."
	while True:
		

		if data_s in ["fight","fight0","handshake","fetch","hello","salute"]:

			moveit.execute(data_s) 	#execute action

			print "reset..."		#reset pose
			rs.reset()
			print "move to natual..."
			limb.move_to_neutral()

			if mutex.acquire():		#clear command
				data_s = ""
				mutex.release()



		# if data_s == "exit":
		# 	moveit_commander.roscpp_shutdown()
		# 	moveit_commander.os._exit(0)
		# 	break






	UDPSock.close()
	os._exit(0)


if __name__ == "__main__":
	main()

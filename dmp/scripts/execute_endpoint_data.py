#!/usr/bin/env python
import rospy,sys
import argparse
import moveit_commander
import numpy as np
from geometry_msgs.msg import PoseStamped,Pose
from moveit_commander import MoveGroupCommander
import time
from copy import deepcopy
from os import path
class MoveItDemo:
	def __init__ (self):
		moveit_commander.roscpp_initialize(sys.argv)
		self.left_arm  = moveit_commander.MoveGroupCommander( 'left_arm')
		self.right_arm = moveit_commander.MoveGroupCommander('right_arm')
		#both_arms = moveit_commander.MoveGroupCommander('both_arms')
		self.right_eef = self.right_arm.get_end_effector_link()
		self.left_eef = self.left_arm.get_end_effector_link()
		self.right_arm.allow_replanning(True)
		self.left_arm.allow_replanning(True)
		self.right_arm.set_goal_position_tolerance(0.5)
		self.right_arm.set_goal_orientation_tolerance(0.5)
		self.left_arm.set_goal_position_tolerance(0.5)
		self.left_arm.set_goal_orientation_tolerance(0.5)
		self.left_arm.set_planning_time(5)
		self.right_arm.set_planning_time(5)
	def execute(self,filename):
		with open(filename, 'r') as f:
			lines = f.readlines()
		waypoints1 = []
		waypoints2 = []
		waypoints_l_start = []
		waypoints_r_start = []
		i = 1
		for idx, values in enumerate(lines[1:]):
			print "Line:\n", i
			i = i + 1
			if i == len(lines):
				break
			#clean each line of file
			values = values.rstrip().split(',')
			values = [float(x) for x in values]
			print values
			if idx == 0:
				print idx
				start_pose1 = self.right_arm.get_current_pose(self.right_eef).pose
				wpose1 = deepcopy(start_pose1)
				wpose1.position.x = values[1]
				wpose1.position.y= values[2]
				wpose1.position.z = values[3]
				wpose1.orientation.x = values[4]
				wpose1.orientation.y = values[5]
				wpose1.orientation.z = values[6]
				wpose1.orientation.w = values[7]

				# start_pose2 = self.right_arm.get_current_pose(self.right_eef).pose
				# wpose2 = deepcopy(start_pose2)
				# wpose2.position.x = values[4]
				# wpose2.position.y = values[5]
				# wpose2.position.z = values[6]
				# # wpose2.orientation.x = values[11]
				# # wpose2.orientation.y = values[12]
				# # wpose2.orientation.z = values[13]
				# # wpose2.orientation.w = values[14]

				waypoints_r_start.append(deepcopy(start_pose1))
				waypoints_r_start.append(deepcopy(wpose1))

				# waypoints_r_start.append(deepcopy(start_pose2))
				# waypoints_r_start.append(deepcopy(wpose2))

			wpose1.position.x = values[1]
			wpose1.position.y = values[2]
			wpose1.position.z = values[3]
			wpose1.orientation.x = values[4]
			wpose1.orientation.y = values[5]
			wpose1.orientation.z = values[6]
			wpose1.orientation.w = values[7]
			waypoints1.append(deepcopy(wpose1))
			# if i == 5:
			# 	break
			# wpose2.position.x = values[4]
			# wpose2.position.y = values[5]
			# wpose2.position.z = values[6]
			# # wpose2.orientation.x = values[11]
			# # wpose2.orientation.y = values[12]
			# # wpose2.orientation.z = values[13]
			# # wpose2.orientation.w = values[14]
			# waypoints2.append(deepcopy(wpose2))
			#print waypoints2
		# joint_positions2 = [0.53,-0.55,0.3,0.7,0.4,0.8,0]
		# self.right_arm.set_joint_value_target(joint_positions2)
		# traj = self.right_arm.plan()
		# self.right_arm.execute(traj)
		# rospy.sleep(5)
		# joint_positions1 = [-0.53,-0.55,-0.3,0.7,-0.4,0.8,0]
		# self.left_arm.set_joint_value_target(joint_positions1)
		# traj = self.left_arm.plan()
		# self.left_arm.execute(traj)
		# rospy.sleep(3)
		print "to start pose\n"
		(cartesian_plan_r,fraction) = self.right_arm.compute_cartesian_path(waypoints_r_start,0.01, 0.0, True)
		self.right_arm.execute(cartesian_plan_r)
		# (cartesian_plan_r,fraction) = self.right_arm.compute_cartesian_path(waypoints_r_start,0.01, 0.0, True)
		# self.right_arm.execute(cartesian_plan_r)
		rospy.sleep(3)
		print "start to playback\n"
		(cartesian_plan_r,fraction) = self.right_arm.compute_cartesian_path(waypoints1,0.01, 0.0, True)
		self.right_arm.execute(cartesian_plan_r)
		# (cartesian_plan_r,fraction) = self.right_arm.compute_cartesian_path(waypoints2,0.01, 0.0, True)
		# self.right_arm.execute(cartesian_plan_r)
		rospy.sleep(3)
		print("Done.")
		moveit_commander.roscpp_shutdown()
		moveit_commander.os._exit(0)	
def main():
	print("Initializing node... ")
	rospy.init_node('moveit_demo')
	rospy.sleep(1)
	epilog = """
Related examples:
	joint_recorder.py; joint_position_file_playback.py.
	"""
	arg_fmt = argparse.RawDescriptionHelpFormatter
	parser = argparse.ArgumentParser(formatter_class=arg_fmt,
									description=main.__doc__,
									epilog=epilog)
	parser.add_argument(
	'-f', '--file', metavar='PATH', required=True,
	help='path to input file'
	)
	args = parser.parse_args(rospy.myargv()[1:])
	moveit = MoveItDemo()
	moveit.execute(path.expanduser(args.file))

if __name__ == "__main__":
	main()

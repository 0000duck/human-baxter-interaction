#!/usr/bin/env python
import rospy
import baxter_interface

rospy.init_node('Hello_Baxter')
limb = baxter_interface.Limb('left')
angles = limb.joint_angles()
angles['left_s0']=0.0
angles['left_s1']=0.0
angles['left_e0']=0.0
angles['left_e1']=0.0
angles['left_w0']=0.0
angles['left_w1']=0.0
angles['left_w2']=0.0


print angles
# limb.move_to_joint_positions(angles)
wave_1 = {'left_s0': -0.459, 'left_s1': -0.202, 'left_e0': 1.807, 'left_e1': 1.714, 
		'left_w0': -0.906, 'left_w1': -1.545, 'left_w2': -0.276}
limb.move_to_joint_positions(wave_1)
# wave_2 = {'left_s0': -0.395, 'left_s1': -0.202, 'left_e0': 1.831, 'left_e1': 1.981, 
# 		'left_w0': -1.979, 'left_w1': -1.100, 'left_w2': -0.448}
# for _move in range(3):
# 	limb.move_to_joint_positions(wave_1)
# 	limb.move_to_joint_positions(wave_2)
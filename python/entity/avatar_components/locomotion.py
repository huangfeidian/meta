# -*- coding: utf-8 -*-


class locomotion(object):

	__register_for_classes__ = ("Avatar", )

	def __construct__(self):
		self.position = (0, 0, 0)
		self.yaw = 0
		self.speed = 0

	def get_position(self):
		return self.position
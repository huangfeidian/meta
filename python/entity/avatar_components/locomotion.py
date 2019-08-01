# -*- coding: utf-8 -*-

from common.utils.interface import Interface, Int, List, Dict


class locomotion(object):

	__register_for_classes__ = ("Avatar", )

	def __construct__(self):
		self.position = (0, 0, 0)
		self.yaw = 0
		self.speed = 0

	def get_position(self):
		return self.position

	@Interface(List())
	def post_position(self, new_pos):
		self.position = new_pos

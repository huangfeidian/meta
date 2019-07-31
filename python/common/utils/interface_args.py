# -*- coding: utf-8 -*-


class Arg(object):
	""" 所有接口参数的基类
	"""
	def __init__(self):
		super(Arg, self).__init__()

	def serialize(self, data):
		raise NotImplementedError()

	def deserialize(self, data):
		raise NotImplementedError()

	def get_type(self):
		raise NotImplementedError()
		

	
class Numbers(Arg):
	""" 所有数字基类
	"""
	def __init__(self):
		super(Numbers, self).__init__()


class Int(Numbers):

	def serialize(self, data):
		
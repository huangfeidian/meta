# -*- coding: utf-8 -*-
import json

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

	def serialize_fail(self, data):
		return "fail to serialize %s data_type %s for type %s" % (data, type(data), self.__class__.__name__)

	def deserialize_fail(self, data):
		return "fail to deserialize %s data_type %s for type %s" % (data, type(data), self.__class__.__name__)

	
class Numbers(Arg):
	""" 所有数字基类
	"""
	def __init__(self):
		super(Numbers, self).__init__()


class Int(Numbers):

	def __init__(self):
		super(Int, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		try:
			result = int(data)
			return result
		except ValueError:
			raise ValueError(self.deserialize_fail(data))


class Float(Numbers):

	def __init__(self):
		super(Float, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		try:
			result = float(data)
			return result
		except ValueError:
			raise ValueError(self.deserialize_fail(data))


class Bool(Numbers):

	def __init__(self):
		super(Bool, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		try:
			result = bool(data)
			return result
		except ValueError:
			raise ValueError(self.deserialize_fail(data))


class Long(Numbers):

	def __init__(self):
		super(Long, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		try:
			result = long(data)
			return result
		except ValueError:
			raise ValueError(self.deserialize_fail(data))


class String(Arg):

	def __init__(self):
		super(String, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		try:
			result = str(data)
			return result
		except ValueError:
			raise ValueError(self.deserialize_fail(data))


class List(Arg):

	def __init__(self):
		super(List, self).__init__()

	def serialize(self, data):
		return data

	def deserialize(self, data):
		if not isinstance(data, list):
			raise ValueError(self.deserialize_fail(data))
		return data


class Dict(Arg):

	def __init__(self):
		super(Dict, self).__init__()

	def serialize(self, data):
		has_no_str_key = False
		for k, v in data.iteritems():
			if not isinstance(k, str):
				has_no_str_key = True
				break
		if has_no_str_key:
			flat_list = [(k, v) for k, v in data.iteritems()]
			return {"__": flat_list}
		else:
			return data

	def deserialize(self, data):

		if not isinstance(data, dict):
			raise ValueError(self.deserialize_fail(data))
		if "__" in data:
			result = {}
			for k, v in data["__"].iteritems():
				result[k] = v
			return result
		else:
			return data


def Interface(*arg_types):
	for one_type in arg_types:
		if not isinstance(one_type, Arg):
			raise ValueError("type %s is not valid Arg type" % one_type.__class__.__name__)

	def interface_method(func):
		func.interface_args = arg_types
		return func

	return interface_method


def is_interface_method(func):
	return hasattr(func, "interface_args")


def deserialize_call(func, args):
	if not is_interface_method(func):
		raise ValueError("func %s is not interface", func.__name__)

	args = json.loads(args)
	converted_args = []
	if len(args) != len(func.interface_args):
		raise ValueError("func arg num not match")

	converted_args = [func.interface_args[i].deserialize(one_arg) for i, one_arg in enumerate(args)]
	func(*converted_args)


def serialize_args(*args):
	result = []
	for one_arg in args:
		if not isinstance(one_arg, dict):
			result.append(one_arg)
		else:
			has_no_str_key = False
			for k, v in one_arg.iteritems():
				if not isinstance(k, str):
					has_no_str_key = True
					break
			if has_no_str_key:
				flat_list = [(k, v) for k, v in one_arg.iteritems()]
				result.append({"__": flat_list})
			else:
				result.append(one_arg)

	return json.dumps(result)



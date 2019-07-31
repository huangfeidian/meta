# -*- coding: utf-8 -*-


import inspect
import re
import sys

build_in_types = (int, long, list, dict, tuple, float, set, frozenset, property, bool, str, unicode)


def is_build_in_type(cur_obj):
	return isinstance(cur_obj, build_in_types)


def register_component(cur_components):
	def register_class(cur_class):
		cur_class_name = cur_class.__name__
		cur_class.__components__ = {}
		cur_class.__constructors__ = {}
		print "components size ", len(cur_components)
		for one_component in cur_components:
			if inspect.ismodule(one_component):
				for name, component in inspect.getmembers(one_component, inspect.isclass):
					for_class = getattr(component, "__register_for_classes__", None)
					print "class %s for class %s" % (name, for_class) 
					if not isinstance(for_class, tuple):
						continue
					if cur_class_name not in for_class:
						continue

					construct_func = getattr(component, "__construct__", None)
					if construct_func and callable(construct_func):
						cur_class.__constructors__[name] = construct_func.im_func

					cur_class.__components__[name] = component
					for temp_name, func in inspect.getmembers(component, inspect.ismethod):
						print "component %s has func %s" % (name, temp_name)
						if temp_name.startswith("__"):
							continue
						setattr(cur_class, temp_name, func.im_func)
					for temp_name, member in inspect.getmembers(component, is_build_in_type):
						print "component %s has member %s " % (name, temp_name)
						if temp_name.startswith("__"):
							continue
						setattr(cur_class, temp_name, member)

		return cur_class

	return register_class
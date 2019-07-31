# -*- coding: utf-8 -*-
from common.utils.register_component import register_component
from entity import avatar_components


@register_component(avatar_components.collect())
class Avatar(object):

	def __init__(self):
		all_conststructors = getattr(self, "__constructors__", {})
		for k, v in all_conststructors.iteritems():
			v(self)

	
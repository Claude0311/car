import node
import math
import csv
import numpy
import pandas

class Maze:
	def __init__(self, filepath):
		"""
		read file and build graph
		"""
		# data read from csv
		self.raw_data = pandas.read_csv(filepath).values
		# graph if saved here
		self.nd_dict = dict()

		# process raw_data
		for dt in self.raw_data:
			nd = node.Node(dt[0])
			for i in range(1,5):
				if not math.isnan(dt[i]):
					nd.setSuccessor(int(dt[i]))

			self.nd_dict[dt[0]] = nd

		# print self.nd_dict
		# for i in range(1, len(self.nd_dict)+1):
		#     print(i, [ nd[0] for nd in self.nd_dict[i].getSuccessors() ])

	
	def shortestPath(self, nd_from, nd_to):
		""" 
		return a path (sequence of nodes) from the current node to the nearest unexplored deadend 
		e.g.
			1 -- 2 -- 3     
				 |    |  ->  shortestPath(1,4) returns [1,2,4]
				 4 -- 5
		"""
		# TODO
		Q = [nd_from]
		mark = [nd_from]
		distance = {nd_from:0}
		transition_list = {nd_from:None}
		
		while nd_to not in Q:#len(Q) != 0:
			recent = Q.pop(0) #移除第0項並記錄
			if recent not in mark:
				mark.append(recent)
			for item in self.nd_dict[recent].getSuccessors():
				if (item not in Q) and (item not in mark):
					Q.append(item)
					distance[item] = distance[recent]+1
					transition_list[item] = recent
		
		R = []
		c = nd_to
		while c != nd_from:
			R.append(transition_list[c])
			c = transition_list[c]
		#R.append(nd_from)
		R.reverse()
		R.append(nd_to)		
		
		return R
import math
import csv
import pandas
import numpy as np

class Maze:
    def __init__(self, filepath):
        """
        read file and build graph
        """
        # data read from csv
        self.raw_data = pandas.read_csv(filepath).values
        # graph if saved here
        self.nd_dict = {}
        # self.tFor, self.tBack, self.tTurn, self.tTrack = 1, 1, 1, 1
        self.End = []
        self.Paths = {}
        self.nd_direction = dict()
        self.edge_types = dict()

        self.turn_cost = 1.5
        self.forward_cost = 1
        self.explored = []
        self.unexplored = []
        self.justVisited = None

        self.reward = {}
  

        # process raw_data
        for dt in self.raw_data:
            nd = Node(int(dt[0]))
            temp_dict = dict()
            for i in range(1,5):
                if not math.isnan((dt[i])):
                    nd.setSuccessor(int(dt[i]))
                    temp_dict[int(dt[i])] = i
                    nd.setDist(int(dt[i]), int(dt[i+4]))
                    # print(dt[0], i, dt[i+4])
            self.nd_direction[dt[0]] = temp_dict
            self.nd_dict[int(dt[0])] = nd

        for nd in self.nd_dict.values():
            if nd.isEnd():
                self.End.append(nd.getIndex())


        for nd in self.nd_dict.values():
        #for nd in self.nd_dict:
            nd_int = nd.getIndex()
            self.unexplored.append(nd_int)
            self.edge_types[nd_int] = dict()
            for i in nd.getSuccessors():
                #i_int = i.getIndex()
                i_int = i
                self.edge_types[nd_int][i_int] = self.edge_type(nd_int, i_int)
    def setJustVisited(self, nd_int):
        self.justVisited = nd_int


    def setExplored(self,nd_int):
        if nd_int in self.unexplored:
            self.explored.append(nd_int)
            self.unexplored.remove(nd_int)

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

    def edge_type(self, n1, n2):
        turns = [0, 0, 0]
        for i in self.nd_dict[n2].getSuccessors():
            turn = self.pathToString([n1, n2, i])
            if turn == 'l':
                turns[0] += 1
            elif turn == 'f':
                turns[1] += 1
            elif turn == 'r':
                turns[1] += 1
        return(turns)


    def dist(self, i1, i2):
        return self.nd_dict[i1].getDist(i2)

    def pathToString(self, path):
        direction = ''
        for i in range(len(path)-2):
            threept = (self.nd_direction[path[i]][path[i+1]], self.nd_direction[path[i+1]][path[i+2]])
            if threept in [(1,4),(3,1),(2,3),(4,2)]:
                cmmd = 'r'
            elif threept in [(1,3),(4,1),(2,4),(3,2)]:
                cmmd = 'l'
            elif threept in [(1,1),(2,2),(3,3),(4,4)]:
                cmmd = 'f'
            elif threept in [(1,2),(2,1),(3,4),(4,3)]:
                cmmd ='b'
            direction += cmmd
        return direction

    def setReward(self, nd_int, reward):
        self.reward[nd_int] = reward



    def getPathCost(self, path):
        direction_str = self.pathToString(path)
        cost = self.forward_cost + self.turn_cost * (direction_str.count("r") + direction_str.count("l")) + self.forward_cost * direction_str.count("f")
        if len(path)>1:
            if path[1] == self.justVisited:
                cost += 1
        return cost 

    # def locating(self, l):
    #     for ini in self.edge_types:
    #         for edge in self.edge_types[ini]:
    #             if ini[edge] != l:
    #                 del ini[edge]
    #                 if len(ini) == 0:


    def closestNode(self, nd_from, rest):

        d = {nd_from: 0}
        for i in self.nd_dict[nd_from].getSuccessors():
            d[i] = self.dist(nd_from, i)
        for i in self.nd_dict:
            if not i in d:
                d[i] = 1000
        traveled = {nd_from}
        unexplored = {nd_from}
        pre = {}
        path = []

        while True:
            m = [0, 0, 1000]
            for node in unexplored.copy():
                done = True
                for succ in self.nd_dict[node].getSuccessors():
                    if not succ in traveled:
                        done = False
                        d[succ] = min(d[node] + self.dist(node, succ), d[succ])
                        if d[succ] < m[-1]:
                            m = [node, succ, d[succ]]
                if done:
                    unexplored.remove(node)

            if m[1] in rest:
                pre[m[1]] = m[0]
                # print(m[1], 'in rest')
                temp = m[1]
                path.append(m[1])
                while temp != nd_from:
                    # print(path)
                    temp = pre[temp]
                    path.insert(0, temp)
                path_dir = self.pathToString(path)
                return m[1], path, path_dir
            else:
                # print(m[1], 'not in rest')
                traveled.add(m[1])
                unexplored.add(m[1])
                pre[m[1]] = m[0]

    def M_dist_square(self, path):
        x, y = 0, 0
        for i in range(len(path)-1):
            if self.nd_direction[path[i]][path[i+1]] == 1:
                y += self.dist(path[i], path[i+1])
            elif self.nd_direction[path[i]][path[i+1]] == 3:
                x -= self.dist(path[i], path[i+1])
            elif self.nd_direction[path[i]][path[i+1]] == 2:
                y -= self.dist(path[i], path[i+1])
            if self.nd_direction[path[i]][path[i+1]] == 4:
                x += self.dist(path[i], path[i+1])
        d = abs(x) + abs(y)
        return d**2

    def getPathReward(self, path):
        # sum M_dist reward in path
        # look for farest path
        # not optimal
        reward = 0
        tmp_path = []
        for nd_int in path:
            tmp_path.append(nd_int)
            #if nd_int in self.unexplored and nd_int in self.End:
            if nd_int in self.unexplored:
                reward += self.reward[nd_int]
        return reward

    def getEndNodeReward(self, path):
        # find last node reward
        # only end node had reward
        reward = 0
            #if nd_int in self.unexplored and nd_int in self.End:
        if path[-1] in self.unexplored and path[-1] in self.End: # check in end
                reward += self.reward[path[-1]]
        return reward


class Node:
    def __init__(self, index=0):
        self.index = int(index)
        # store successors' indices
        self.Successors = []
        self.Dist = {}

    def getIndex(self):
        return self.index

    def getSuccessors(self):
        return self.Successors

    def setSuccessor(self, successor):
        # check whether 'successor' is valid by comparing with the class member
        for succ in self.Successors:
            if succ == successor:
                return
        # Update the successors in data members
        self.Successors.append(successor)
        return

    def isSuccessor(self, nd):
        # check whether nd is a successor
        for succ in self.Successors:
            if succ[0] == nd: return True
        return False

    def setDist(self, index, dist):
        self.Dist[index] = dist
        return

    def getDist(self, index):
        try:
            return self.Dist[index]
        except KeyError:
            return self.index, index, 'wtf', self.Dist

    def isEnd(self):
        # return len(self.Successors) == 1 and self.index != 1
        return len(self.Successors) == 1

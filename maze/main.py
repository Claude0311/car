import random
from node import *
import maze_withBLT as mz

random.seed(6666)
tests = 50
maze_path = "./maze1.csv"
total_nodes = 38

def main():
	maze = mz.Maze(maze_path)

	for i in range(tests):
		nd_from = random.randrange(total_nodes) + 1
		nd_to = random.randrange(total_nodes) + 1
		print('From %s to %s: %s' % (nd_from, nd_to, maze.shortestPath(nd_from, nd_to)))


if __name__=='__main__':
	main()
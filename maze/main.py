import random
#from node import *
import maze.maze_withBLT as mz
import time

random.seed(6666)
tests = 50
#maze_path = "./medium_maze.csv"
maze_path = "maze/final_map_109.csv"
total_nodes = 38

_sim = True

def fromAtoB(nd_from, nd_to):
    maze = mz.Maze(maze_path)
    path = maze.shortestPath(nd_from, nd_to)
    for i in maze.nd_dict.values():
        print('From %s to %s: %s' % (nd_from, i.getIndex(), path))
        print('Direction sequence: %s' % (maze.pathToString(path)))

def findNextPath(maze,nd_from):
    path_dict = {}
    for i_int in maze.End:
        path = maze.shortestPath(nd_from, i_int) # find all node'path
        path_dict[i_int] = path

    best_cp = 0.0
    best_nodes = []
    best_paths = []

    for i_int, path in path_dict.items():
        #print('From %s to %s: %s' % (nd_from, i_int, path))
        # calculate path reward
        reward = maze.getEndNodeReward(path)
        #calculate path cost
        cost = maze.getPathCost(path)

        #print('Direction sequence: %s' % (maze.pathToString(path)))
        #print('Distance: %s' % (maze.M_dist(path)))
        if len(path)-1 > 0:
            #print('Reward: %s, cost: %s, CP value: %s, %s' % (reward,cost,reward/cost,path))
            if best_cp == (reward/cost):
                best_nodes.append(path[1])
                best_paths.append(path)	

            if best_cp < (reward/cost):
                best_cp = reward/cost
                best_nodes = [path[1]]
                best_paths = [path]			
            
        #print()

    best_path = random.choice(best_paths)
    print('Best path: %s' % (best_path))

    return best_path
    

    
def findNextNode(maze,nd_from):
    path_dict = {}

    for i in maze.nd_dict.values():
        i_int = i.getIndex()
        path = maze.shortestPath(nd_from, i_int) # find all node'path
        path_dict[i_int] = path

    best_cp = 0.0
    best_nodes = []
    best_path = []

    for i_int, path in path_dict.items():
        #print('From %s to %s: %s' % (nd_from, i_int, path))
        # calculate path reward
        reward = maze.getPathReward(path)
        #calculate path cost
        cost = maze.getPathCost(path)

        #print('Direction sequence: %s' % (maze.pathToString(path)))
        #print('Distance: %s' % (maze.M_dist(path)))
        if len(path)-1 > 0:
            #print('Reward: %s, cost: %s, CP value: %s, %s' % (reward,cost,reward/cost,path))
            if best_cp == (reward/cost):
                best_nodes.append(path[1])
                best_path.append(path)	

            if best_cp < (reward/cost):
                best_cp = reward/cost
                best_nodes = [path[1]]
                best_path = [path]			
            
        #print()

    best_node = random.choice(best_nodes)
    #print('Best path: %s' % (best_path))

    return best_node
    

            
def findAllPath():
    '''
    find highest cp_value node
    concept:
        calculate path cost: f -> 2, l,r -> 3

    cp_value = score/cost

    '''
    nd_from = int(input()) # starting node
    maze = mz.Maze(maze_path)

    if _sim:
        bt_reward_sim = random.sample(list(maze.nd_dict.keys()),k=4) # for simulation
        print(bt_reward_sim)
        step_count = 0
        reward = 0
    
    for i in maze.nd_dict.values(): # node
        i_int = i.getIndex()
        path = maze.shortestPath(nd_from, i_int) # find all node'path from Start to i 
        maze.setReward(i_int, maze.M_dist_square(path)) # calculate node reward

    # start search
    nd_nxt = nd_from
    maze.setExplored(nd_nxt)
    path = [nd_nxt]
    while(1):
        nd_prev = nd_nxt
        nd_nxt = findNextNode(maze,nd_nxt)
        step_count += 1
        maze.setJustVisited(nd_prev)
        maze.setExplored(nd_nxt)
        path.append(nd_nxt)

        #print(nd_nxt,end=" ")
        if nd_nxt in bt_reward_sim:
            #print("Reward",maze.reward[nd_nxt])
            reward += maze.reward[nd_nxt]
            bt_reward_sim.remove(nd_nxt)
            if bt_reward_sim == []:
                print("get all reward, count:",step_count,"reward:",reward)
                bt_reward_sim = [-1] 
            else:
                print("get reward, count:",step_count,"reward:",reward)


        if maze.unexplored == []:
            break

    
    pathstr = maze.pathToString(path)
    print(pathstr)
    print(path)
    return pathstr

            
def findPath():
    '''
    find highest cp_value node
    concept:
        calculate path cost: f -> 2, l,r -> 3

    cp_value = score/cost

    '''
    nd_from = int(input("Enter starting node: ")) # starting node
    maze = mz.Maze(maze_path)

    if _sim:
        bt_reward_sim = maze.End # for simulation
        #print(bt_reward_sim)
        step_count = 0
        reward = 0
    
    for i in maze.nd_dict.values(): # node
        i_int = i.getIndex()
        if i_int in maze.End:
            path = maze.shortestPath(nd_from, i_int) # find all node'path from Start to i 
            maze.setReward(i_int, maze.M_dist_square(path)) # calculate node reward

    # start search
    nd_nxt = nd_from
    maze.setExplored(nd_nxt)
    path = [nd_nxt]
    while(1):
        #nd_prev = nd_nxt

        path_nxt = findNextPath(maze,nd_nxt)
        nd_nxt = path_nxt[-1]
        step_count += len(path_nxt)-1
        #maze.setJustVisited(nd_prev)
        for i_int in path_nxt:
            maze.setExplored(i_int)
        path.extend(path_nxt[1:])

        #print(nd_nxt,end=" ")
        if nd_nxt in bt_reward_sim:
            #print("Reward",maze.reward[nd_nxt])
            reward += maze.reward[nd_nxt]
            bt_reward_sim.remove(nd_nxt)
            if bt_reward_sim == []:
                print("get all reward, count:",step_count,"reward:",reward)
                bt_reward_sim = [-1] 
            else:
                print("get reward, count:",step_count,"reward:",reward)


        #print(maze.unexplored)
        if maze.unexplored == []:
            break

    
    #print(path)

    pathstr = maze.pathToString(path)
    print(pathstr)
    return pathstr

            



def RandomTest():
    maze = mz.Maze(maze_path)

    for i in range(tests):
        nd_from = random.randrange(total_nodes) + 1
        nd_to = random.randrange(total_nodes) + 1
        path = maze.shortestPath(nd_from, nd_to)



if __name__=='__main__':
    random.seed(int(time.time()))
    pathstr = findPath()
    #findAllPath()
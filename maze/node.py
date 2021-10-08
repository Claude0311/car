class Node:
    def __init__(self, index=0):
        self.index = index
        # store successors' indices
        self.Successors = []

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

    def isEnd(self):
        return len(self.Successors) == 1 and self.index != 1

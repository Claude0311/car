# This code is maintained by R09945027 呂英弘
# For questions, please reach out to me via email for Facebook.
# Email: R09945027@ntu.edu.tw / louis8683@gmail.com
# Facebook: 呂英弘 / Louis Lu

import requests

from blt.clientsocket import ClientSocket


# ================================================================
# Scoreboard
#   add_UID(UID_str)
#     UID_str : UID string ("0x" excluded)
#   get_current_score()
#     return current score (int)
# ================================================================


class Scoreboard:
    '''
    The Scoreboard class connects to the server socket and enables updating score by sending UID.

    
    '''
    def __init__(self, team_name:str, host:str="http://140.112.31.82:3000"):
        
        # INIT VARIABLES

        self.team = team_name
        self.game = 0
        self.ip = host #'https://creative.ntuee.org'

        print(f"{self.team} wants to play!")
        print(f"connecting to server......{self.ip}")

        # CONNECT WITH SERVER SOCKET

        # Create a socket.io instance and connect to server.
        self.socket = ClientSocket(self.ip)

        # GET INFORMATION FROM SERVER
        
        # Retrieve "current_team" from game status.
        res = requests.get(self.ip + '/game_status')
        if res.status_code == 404:
            raise ConnectionError(f'Response[404] from {self.ip + "/game_status"}')
        playing_team = res.json()['current_team']

        if playing_team is None:
            # No one playing, start game.
            print("Game is starting.....")
            self.socket.start_game({ 
                'gamemode': self.game, 
                'team': self.team })
        elif playing_team != team_name:
            # Disallow multiple players at the same time.
            self.socket.disconnect()
            raise ConnectionError(f"{res.json()['current_team']} is current playing.\nPlease wait {res.json()['time_remain']} seconds for retry.")          


    def add_UID(self, UID_str:str) -> None:
        '''Send [UID_str] to server to update score.'''
        print(f"In add_UID, UID = {UID_str}")

        # Type and length check.
        if(type(UID_str) != str):
            print(f"UID type error! (Your type: {type(UID_str)})")
        if(len(UID_str) != 8):
            print(f"UID length error! (Your length: {len(UID_str)})")
        
        # Send the UID to the server.
        self.socket.add_UID({'uid_str': UID_str})

    
    def get_current_score(self) -> int:
        '''Get the score of the current game. Returns None if failed.'''
        try:
            r = requests.get(self.ip+"/current_score")
            return r.json()['current_score']
        except:
            print("Failed to fetch current score")
            return None


if __name__ == '__main__':
    import time
    myScoreboard = Scoreboard('Test')
    time.sleep(3)
    myScoreboard.add_UID("71A5261C")
    print(type(myScoreboard.get_current_score()))
# This code is maintained by R09945027 呂英弘
# For questions, please reach out to me via email for Facebook.
# Email: R09945027@ntu.edu.tw / louis8683@gmail.com
# Facebook: 呂英弘 / Louis Lu

import socketio
import sys

class ClientSocket(socketio.ClientNamespace):
    '''
    A Socket.io Client to communicate with the scoreboard server.

    Parameters:
    ip : str
        > the IP of the Scoreboard Server.

    Public Methods:
    start_game ( game_info : dict )
        > start the game with the game_info, which is a dict with two elements,
            {'gamemode': 0, 'team': [team_name]}.

    add_UID ( UID_str : str )
        > Send the UID_str to the server, the score on the server will be adjusted accordingly.
    '''
    sio = socketio.Client()

    def __init__(self, ip:str):
        super().__init__('/')
        self.ip = ip
        try:
            self.sio.connect(self.ip)
        except Exception as e:
            print('Failed to connect to server, ', e)
            sys.exit(1)
        self.sio.register_namespace(self)

    # Emitter

    def start_game(self, game_info:dict) -> None:
        '''Start the game with the game_info, {'gamemode': 0, 'team': [team_name]}.'''
        self.emit("start_game", game_info)

    def add_UID(self, UID_str:str) -> None:
        '''Send the UID_str to the server, the score on the server will be adjusted accordingly.'''
        self.emit("add_UID", UID_str)

    # Receiver

    def on_connect(self):
        print("connected")
    
    def on_invalid_mode(self):
        print("Error:invalid gamemode!!")

    def on_game_end(self, data=None):
        print("game_end")
        self.disconnect()

    def on_game_started(self, data):
        print("Game started!")
        print("Playing game as\nTeam: {}\nMode: {}".format(
            data['current_team'], data['gamemode']))
        print("Please checkout {} for more information.".format(self.ip))

    def on_UID_added(self, message):
        print(message)

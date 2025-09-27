import json
import os
from collections import deque
from config import TILE_MAP, BACKUP_LAPTOP_IP, BACKUP_USERNAME, BACKUP_PASSWORD, BACKUP_PATH
import paramiko

STATE_FILE = "/home/pi/snakes_ladders_project/board_state.json"
MAX_STATES = 10

class StateManager:
    def __init__(self):
        self.states = deque(maxlen=MAX_STATES)
        self.load_state()

    def load_state(self):
        if os.path.exists(STATE_FILE):
            try:
                with open(STATE_FILE, "r") as f:
                    data = json.load(f)
                    for state in data:
                        self.states.append(state)
            except Exception as e:
                print("Error loading state:", e)
        else:
            empty_state = {tile: {"color": (0,0,0), "effect": "off"} for tile in TILE_MAP.keys()}
            self.states.append(empty_state)
            self.save_state()

    def save_state(self):
        try:
            with open(STATE_FILE + ".tmp", "w") as f:
                json.dump(list(self.states), f)
            os.replace(STATE_FILE + ".tmp", STATE_FILE)
            self.backup_file(STATE_FILE)
        except Exception as e:
            print("Error saving state:", e)

    def push_state(self, new_state):
        self.states.append(new_state)
        self.save_state()

    def undo(self):
        if len(self.states) > 1:
            self.states.pop()
            self.save_state()
            return self.states[-1]
        return self.states[-1]

    def get_current_state(self):
        return self.states[-1]

    def backup_file(self, local_file):
        try:
            transport = paramiko.Transport((BACKUP_LAPTOP_IP, 22))
            transport.connect(username=BACKUP_USERNAME, password=BACKUP_PASSWORD)
            sftp = paramiko.SFTPClient.from_transport(transport)
            filename = os.path.basename(local_file)
            sftp.put(local_file, BACKUP_PATH + filename)
            sftp.close()
            transport.close()
            print(f"Backed up {local_file} to laptop")
        except Exception as e:
            print("Backup failed:", e)

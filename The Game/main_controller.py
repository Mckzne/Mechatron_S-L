import random
import time
from config import TEAM_COLORS, COLLISION_COLOR, SNAKES, LADDERS, TILE_MAP, TILE_LIGHT_DURATION, ANIMATION_SPEED, CHAOS_ANIMATION_SPEED
from state_manager import StateManager
from mqtt_manager import MQTTManager
from audio_manager import AudioManager
from gui import GameGUI

class MainController:
    def __init__(self):
        self.state_manager = StateManager()
        self.mqtt = MQTTManager()
        self.audio = AudioManager()
        self.gui = GameGUI(self.state_manager, self)
        
        # Board state and player positions
        self.board_state = self.state_manager.get_current_state()
        self.player_positions = {team: 1 for team in TEAM_COLORS.keys()}
        
        # Buzzer status and first responder
        self.buzzer_status = {team: True for team in TEAM_COLORS.keys()}
        self.first_responder_team = None
        self.buzzer_queue = []

        # Subscribe to buzzer MQTT topic
        self.mqtt.register_callback("buzzer/pressed", self.handle_buzzer_press)

    ###############################
    # Dice and Movement Functions
    ###############################
    def roll_dice(self):
        return random.randint(1,6)

    def move_player(self, team, steps):
        path = []
        start = self.player_positions[team]
        for i in range(steps):
            next_tile = start + i + 1
            if next_tile > 100:
                next_tile = 100
            path.append(next_tile)

        # Light up path sequentially
        for tile in path:
            colliding_team = self.check_collision(tile, team)
            if colliding_team:
                self.handle_collision(tile, team, colliding_team)

            self.board_state[tile]["color"] = TEAM_COLORS[team]
            self.board_state[tile]["effect"] = "movement"
            self.send_tile_update(tile)
            self.gui.update_board(self.board_state)
            time.sleep(ANIMATION_SPEED/1000)

        # Update final position
        self.player_positions[team] = path[-1]
        self.state_manager.push_state(self.board_state.copy())

        # Check for snake or ladder
        if path[-1] in SNAKES:
            self.trigger_snake(path[-1], team)
        elif path[-1] in LADDERS:
            self.trigger_ladder(path[-1], team)

    ###############################
    # ESP32 Tile Communication
    ###############################
    def send_tile_update(self, tile):
        esp_id = TILE_MAP[tile]["esp_id"]
        local_index = TILE_MAP[tile]["local_index"]
        color = self.board_state[tile]["color"]
        command = {
            "tiles": [local_index],
            "color": color,
            "animation": self.board_state[tile]["effect"],
            "duration": TILE_LIGHT_DURATION
        }
        self.mqtt.send_command(esp_id, command)

    ###############################
    # Snake and Ladder Animations
    ###############################
    def trigger_snake(self, head_tile, team):
        path = SNAKES[head_tile]
        self.audio.play("snake")
        self.show_chaos()
        for tile in path:
            self.board_state[tile]["color"] = (139,69,19)  # brown
            self.board_state[tile]["effect"] = "snake"
            self.send_tile_update(tile)
            self.gui.update_board(self.board_state)
            time.sleep(ANIMATION_SPEED/1000)
        self.player_positions[team] = path[-1]
        self.state_manager.push_state(self.board_state.copy())

    def trigger_ladder(self, bottom_tile, team):
        path = LADDERS[bottom_tile]
        self.audio.play("ladder")
        self.show_chaos()
        for tile in path:
            self.board_state[tile]["color"] = (255,215,0)  # gold
            self.board_state[tile]["effect"] = "ladder"
            self.send_tile_update(tile)
            self.gui.update_board(self.board_state)
            time.sleep(ANIMATION_SPEED/1000)
        self.player_positions[team] = path[-1]
        self.state_manager.push_state(self.board_state.copy())

    ###############################
    # Chaos Effect
    ###############################
    def show_chaos(self):
        import random
        tiles = list(TILE_MAP.keys())
        for _ in range(CHAOS_ANIMATION_SPEED*10):
            tile = random.choice(tiles)
            color = (random.randint(0,255), random.randint(0,255), random.randint(0,255))
            self.board_state[tile]["color"] = color
            self.send_tile_update(tile)
            self.gui.update_board(self.board_state)
            time.sleep(CHAOS_ANIMATION_SPEED/1000)

    ###############################
    # Collision Handling
    ###############################
    def check_collision(self, tile, current_team):
        for team, pos in self.player_positions.items():
            if team != current_team and pos == tile:
                return team
        return None

    def handle_collision(self, tile, team_a, team_b):
        # Temporarily show tile in dim color
        self.board_state[tile]["color"] = COLLISION_COLOR
        self.gui.update_board(self.board_state)
        # Trigger buzzer logic
        print(f"Collision at tile {tile} between {team_a} and {team_b}. Buzzer question triggered.")

    ###############################
    # Pi-side Buzzer Handling
    ###############################
    def handle_buzzer_press(self, payload):
        team = payload
        if self.buzzer_status.get(team, False) and not self.first_responder_team:
            self.first_responder_team = team
            print(f"Team {team} pressed first!")
            self.audio.play("buzzer")
            self.ask_question(team)

    def ask_question(self, team):
        # Display question on GUI and check answer
        correct = self.get_answer(team)
        if correct:
            print(f"Team {team} answered correctly, they roll dice")
            steps = self.roll_dice()
            self.move_player(team, steps)
            self.reset_buzzers()
        else:
            print(f"Team {team} answered wrong, buzzer disabled")
            self.buzzer_status[team] = False
            self.first_responder_team = None
            if all(not status for status in self.buzzer_status.values()):
                print("All teams failed, new question, reset buzzer privileges")
                self.reset_buzzers()

    def reset_buzzers(self):
        self.buzzer_status = {team: True for team in TEAM_COLORS.keys()}
        self.first_responder_team = None

    def get_answer(self, team):
        """
        Placeholder method. Replace with GUI input logic.
        Return True if team answers correctly, else False.
        """
        # For testing, randomly decide correct/incorrect
        return random.choice([True, False])

    ###############################
    # Run the GUI & Game Loop
    ###############################
    def run(self):
        self.gui.run()

if __name__ == "__main__":
    controller = MainController()
    controller.run()

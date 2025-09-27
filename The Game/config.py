# config.py

# Team colors (RGB)
TEAM_COLORS = {
    "blue": (0, 0, 255),
    "red": (255, 0, 0),
    "green": (0, 255, 0),
    "yellow": (255, 255, 0),
    "purple": (128, 0, 128),
    "orange": (255, 165, 0)
}

# Collision color (dim white)
COLLISION_COLOR = (100, 100, 100)

# Snake and ladder paths (tile numbers)
SNAKES = {
    16: [16, 15, 14, 7],
    49: [49, 38, 23, 12],
    # add remaining snakes
}

LADDERS = {
    3: [3, 12, 21],
    15: [15, 25, 35],
    # add remaining ladders
}

# 25 ESP32s controlling 4 tiles each
TILE_MAP = {
    1: {"esp_id": 1, "local_index": 0},
    2: {"esp_id": 1, "local_index": 1},
    19: {"esp_id": 1, "local_index": 2},
    20: {"esp_id": 1, "local_index": 3},
    # continue up to 100
}

# LED animation settings
TILE_LIGHT_DURATION = 12000  # ms
SNAKE_LADDER_DURATION = 7000 # ms
ANIMATION_SPEED = 300         # ms per tile during movement
CHAOS_ANIMATION_SPEED = 100   # ms per random tile during chaos

# MQTT broker config
MQTT_BROKER_IP = "192.168.4.1"
MQTT_PORT = 1883

# Backup laptop details
BACKUP_LAPTOP_IP = "192.168.4.100"
BACKUP_USERNAME = "laptop_user"
BACKUP_PASSWORD = "password"
BACKUP_PATH = "/home/laptop_user/snakes_backup/"

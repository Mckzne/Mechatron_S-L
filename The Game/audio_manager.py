import pygame
import os

AUDIO_FOLDER = "/home/pi/snakes_ladders_project/audio"

class AudioManager:
    def __init__(self):
        pygame.mixer.init()
        self.sounds = {
            "dice": pygame.mixer.Sound(os.path.join(AUDIO_FOLDER, "dice_roll.wav")),
            "ladder": pygame.mixer.Sound(os.path.join(AUDIO_FOLDER, "ladder.wav")),
            "snake": pygame.mixer.Sound(os.path.join(AUDIO_FOLDER, "snake.wav")),
            "buzzer": pygame.mixer.Sound(os.path.join(AUDIO_FOLDER, "buzzer.wav"))
        }

    def play(self, event_name):
        if event_name in self.sounds:
            self.sounds[event_name].play()

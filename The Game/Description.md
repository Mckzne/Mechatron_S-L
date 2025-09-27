The folder contains preliminary codes subject to change and have not been tested on the infrastructure yet. The final codes with the required changes assuming the scehmatic of 25 esp32s across a 10x10 matrix of tiles, with each esp32 connected to a 2x2 matrix, will be committed once testing is over with required outputs, along with a video of the tiles. Note: code for gui.py requires successful Rpi and electronics connections and will be pushed after.
# 🐍🎲 Interactive Snakes and Ladders – ESP32 + Raspberry Pi 5  

## 📖 Game Description  

This project is a **giant interactive Snakes & Ladders board** with **LED tiles, wireless ESP32 control, buzzers, and a Raspberry Pi 5 as the master controller**.  

- The board is a **10×10 grid = 100 tiles**.  
- Each tile is a physical square lined with LED strips.  
- 25 ESP32 boards are used, each controlling a **2×2 block (4 tiles)**.  
- The Raspberry Pi 5 coordinates all ESP32s, handles player turns, game logic, and GUI controls.  

### 🎮 Gameplay Rules  
1. **Teams**  
   - 6 teams, each with a unique LED color.  
   - Each team has a buzzer.  

2. **Turns**  
   - A quiz master asks a question before each dice roll.  
   - Whoever presses the buzzer first gets to answer.  
   - If correct → that team rolls the dice.  
   - If wrong → their buzzer is disabled until the next round, question reopens to other teams.  
   - If all fail → question is nullified, new question asked, all buzzer privileges reset.  

3. **Movement**  
   - A dice roll generates a random number (1–6).  
   - The player’s LED path lights up tile by tile in their color.  
   - The player’s new position stays lit for 12 seconds.  
   - Only current positions of players remain lit on the board when idle.  

4. **Snakes & Ladders**  
   - Landing on a ladder → LEDs animate in **gold** upwards quickly, chaos colors flash on other tiles for 7 seconds, then the player jumps to the top.  
   - Landing on a snake → LEDs animate in **brown** downwards, chaos colors flash, then the player drops to the tail.  

5. **Collisions (Two players on the same tile)**  
   - Tile is shown in **lower brightness mix color**.  
   - Quiz master asks a question → players buzz to decide who rolls.  

6. **Game End**  
   - First team to reach Tile 100 wins.  

---

## ⚙️ How the Code Runs  

The game logic is implemented in **Python (on Raspberry Pi)** and **Arduino (on ESP32s)**.  

### 1️⃣ Initialization  
- Raspberry Pi starts `main_controller.py`.  
- Sets up board state (all tiles empty).  
- Connects to ESP32s via MQTT.  
- Starts GUI for quiz master.  
- All buzzers enabled.  

**Board response**: All tiles off, GUI shows empty board.  

---

### 2️⃣ Dice Roll  
- Quiz master selects a team.  
- Dice roll triggered (`roll_dice()` = 1–6).  

**Board response**: Dice result shown in GUI.  

---

### 3️⃣ Player Movement  
- Code lights up the player’s path step by step in their team color.  
- Saves the new state to history (for undo/power recovery).  

**Board response**: Tiles glow in team color as the player moves.  

---

### 4️⃣ Snakes & Ladders  
- If landing on a snake/ladder:  
  - Chaos animation plays (random colors everywhere).  
  - Snake path lights **brown downwards** or ladder path **gold upwards**.  
  - Player teleports to new position.  
- State saved.  

**Board response**: Animated sequence + new tile position.  

---

### 5️⃣ Collisions  
- If two players land on the same tile:  
  - Tile glows in **lower brightness**.  
  - Buzzer round starts.  
  - First responder’s answer decides who gets the dice roll.  

**Board response**: Dim color + buzzer logic.  

---

### 6️⃣ Buzzer Questions  
- First buzzer press handled.  
- If answer correct → dice roll + move.  
- If wrong → team disabled for that round.  
- If all wrong → reset question, all buzzers re-enabled.  

---

### 7️⃣ Undo Function  
- If quiz master makes a mistake → GUI **Undo button** restores last saved state.  

**Board response**: Instantly reverts to last valid state.  

---

### 8️⃣ Persistence & Recovery  
- Every move logged to Pi microSD + mirrored log on laptop.  
- On restart, Pi loads last state → resumes game exactly where it left off.  

**Board response**: After power loss, tiles restore to correct positions.  

---

## 🛠️ Tech Stack  
- **Hardware**: Raspberry Pi 5, ESP32s (×25), LED strips, buzzers.  
- **Networking**: Raspberry Pi hotspot + MQTT.  
- **Software**:  
  - Python (GUI, main controller, MQTT, audio).  
  - Arduino (ESP32 LED tile control).  
- **Data**: Logs + last N states stored for undo/power recovery.  


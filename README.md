# Enies Lobby Battle Simulation

A C++ object-oriented battle simulator inspired by the Enies Lobby arc. The project models a turn-based fight between the Straw Hat crew and CP9 agents while also tracking mission objectives such as destroying gates, rescuing Robin, escaping through the bridge, and surviving the Buster Call.

## Overview

The simulation is driven by an input scenario file. Each scenario defines the initial battle context, participating Straw Hat characters, CP9 enemies, and important buildings. The `EniesLobbyBattle` class loads this data, builds the turn order, processes each turn, applies building effects, checks win/loss conditions, and returns the final battle result.

## Features

- Object-oriented character hierarchy using inheritance and polymorphism
- Separate combat logic for Straw Hat members and CP9 agents
- Special skills, energy costs, HP/energy clamping, morale changes, and status effects
- Building system with objective-based effects
- Linked-list turn order through `TurnNode`
- File-based scenario loading
- Deterministic result output for testing

## Project Structure

```text
.
├── main.cpp          # Program entry point
├── main.h            # Standard library includes and global namespace setup
├── eniesLobby.h      # Class declarations for characters, buildings, context, and battle system
├── eniesLobby.cpp    # Full implementation of battle logic
├── input.txt         # Sample battle scenario
└── README.md
```

## Requirements

- C++ compiler with C++11 support or later
- No external libraries are required

## Build

Compile the project with `g++`:

```bash
g++ -std=c++11 main.cpp eniesLobby.cpp -o enies_lobby
```

If your files have versioned names such as `main(1).cpp` or `eniesLobby(14).cpp`, rename them first so the include statements match:

```text
main(1).cpp        -> main.cpp
main(1).h          -> main.h
eniesLobby(14).cpp -> eniesLobby.cpp
eniesLobby(13).h   -> eniesLobby.h
```

## Run

The uploaded `main.cpp` currently contains only an empty entry point. To run the simulator, update `main.cpp` like this:

```cpp
#include "eniesLobby.h"

using namespace std;

int main() {
    EniesLobbyBattle battle("input.txt");
    battle.runBattle();
    cout << battle.getResult() << endl;
    return 0;
}
```

Then compile and run:

```bash
g++ -std=c++11 main.cpp eniesLobby.cpp -o enies_lobby
./enies_lobby
```

On Windows PowerShell:

```powershell
g++ -std=c++11 main.cpp eniesLobby.cpp -o enies_lobby.exe
.\enies_lobby.exe
```

## Input Format

The simulator reads a plain-text scenario file. Each line begins with a record type.

### Battle Context

```text
CONTEXT morale alarmLevel rescueProgress escapeProgress busterCallTimer maxTurns
```

Example:

```text
CONTEXT 93 77 61 45 69 5
```

Field meanings:

| Field | Description |
|---|---|
| `morale` | Straw Hat morale level |
| `alarmLevel` | Enies Lobby alarm level |
| `rescueProgress` | Progress toward rescuing Robin |
| `escapeProgress` | Progress toward escaping |
| `busterCallTimer` | Countdown related to the Buster Call |
| `maxTurns` | Maximum number of turns before timeout |

### Straw Hat Character

```text
STRAW_HAT name hp atk def speed energy bounty
```

Supported names:

```text
Luffy, Zoro, Sanji, Nami, Chopper, Usopp, Franky
```

Example:

```text
STRAW_HAT Franky 149 29 9 49 79 1699
```

### CP9 Agent

```text
CP9 name hp atk def speed energy doriki
```

Supported names:

```text
Lucci, Kaku, Jabra, Blueno, Kalifa, Kumadori, Fukurou
```

Example:

```text
CP9 Fukurou 149 59 34 49 79 799
```

### Building

```text
BUILDING name hp
```

Supported buildings:

```text
MainGate, Courthouse, TowerOfJustice, BridgeOfHesitation, BusterCallShip
```

Example:

```text
BUILDING MainGate 61
```

## Sample Scenario

```text
CONTEXT 93 77 61 45 69 5
STRAW_HAT Franky 149 29 9 49 79 1699
CP9 Fukurou 149 59 34 49 79 799
CP9 Lucci 150 10 0 50 80 800
CP9 Kaku 151 11 1 51 81 801
BUILDING BusterCallShip 60
BUILDING MainGate 61
BUILDING Courthouse 62
BUILDING TowerOfJustice 63
BUILDING BridgeOfHesitation 64
```

## Output Format

`getResult()` returns a single-line summary:

```text
resultCode turnCount morale alarmLevel rescueProgress escapeProgress busterCallTimer
```

Possible result codes include:

| Result Code | Meaning |
|---|---|
| `STRAW_HAT_WIN` | Robin is rescued and escape progress reaches the win condition |
| `STRAW_HAT_WIN_BY_DEFEAT_CP9` | All CP9 agents are defeated |
| `CP9_WIN` | All Straw Hat characters are defeated |
| `BUSTER_CALL` | The Buster Call condition is triggered |
| `TIME_OUT` | The battle reaches the maximum number of turns |

## Core Classes

| Class | Purpose |
|---|---|
| `BattleContext` | Stores global battle state such as turn count, morale, alarm level, rescue progress, escape progress, and result code |
| `Character` | Abstract base class for all playable and enemy characters |
| `StrawHat` | Base class for Straw Hat crew members, storing bounty information |
| `CP9Agent` | Base class for CP9 enemies, storing doriki information |
| `Building` | Abstract base class for mission-related structures |
| `EniesLobbyBattle` | Main controller that loads scenarios, manages turns, processes actions, applies building effects, and checks ending conditions |

## Battle Flow

1. Load scenario data from the input file.
2. Add Straw Hats, CP9 agents, and buildings to the battle.
3. Build a linked-list turn order.
4. Repeatedly process turns until the battle ends or reaches `maxTurns`.
5. Apply building effects after each turn.
6. Check whether the Straw Hats win, CP9 wins, the Buster Call triggers, or time runs out.
7. Return the final result string through `getResult()`.

## Notes

- The battle system is deterministic, so the same input file should produce the same result.
- All major state values such as morale, alarm level, rescue progress, escape progress, HP, and energy are clamped to valid ranges during the simulation.
- The current code is suitable for coursework, unit testing, and extension into a larger turn-based game simulation.

## Future Improvements

- Add a complete CLI interface for selecting input files.
- Print detailed turn-by-turn battle logs.
- Add unit tests for each character skill and building effect.
- Separate utility macros into safer helper functions.
- Add richer scenario validation and error messages.

## License

This project is provided for educational purposes. Add a license file if you plan to publish or distribute it publicly.

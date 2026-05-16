# tictactoe

Console Tic-Tac-Toe with an **unbeatable AI** powered by Minimax and alpha-beta pruning. The AI never loses — the best a human can achieve is a draw.

## Features

- **Alpha-beta pruning** — eliminates branches that can't affect the result, making the AI near-instant
- **Depth scoring** — the AI prefers faster wins and slower losses, so it plays naturally
- Play-again loop — no need to relaunch between games
- Safe input handling — rejects non-integers, out-of-range positions, and already-taken squares; handles EOF cleanly
- Position legend shown at game start

## How to Play

```
Board positions:
 1 | 2 | 3
-----------
 4 | 5 | 6
-----------
 7 | 8 | 9
```

You are **X**, the AI is **O**. Enter the number of the square you want to play. The AI responds instantly.

## AI Design

The AI uses **Minimax with alpha-beta pruning**:

| Outcome | Score |
|---|---|
| AI wins in `d` moves | `10 - d` |
| Human wins in `d` moves | `d - 10` |
| Draw | `0` |

Subtracting depth from the win score ensures the AI prefers routes that win in fewer moves. Alpha-beta pruning skips branches that cannot improve the current best, keeping the response time imperceptible.

## Build & Run

```bash
make
./tictactoe
```

Requires C++17 or later.

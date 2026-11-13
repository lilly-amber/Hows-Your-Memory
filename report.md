# Project Report: How's Your Memory?

* **GitHub Repository:** [https://github.com/lilly-amber/Hows-Your-Memory.git](https://github.com/lilly-amber/Hows-Your-Memory.git)
* **Video Walkthrough:** [Add Unlisted YouTube Link Here]

---

## 1. Game Design Proposal

* **Title:** How’s Your Memory?
* **Genre:** Memory/pair matching game
* **Synopsis:** A memory game where the player must match pairs of face-down cards.

**Core Gameplay Loop:**  
All cards are briefly previewed before being flipped face down, player interacts with hidden card, card is revealed, second hidden card is selected, second card is revealed, if the images on the cards match cards are cleared, if cards don’t match, they are flipped back. Player should memorise previously flipped cards in order to guess accurately. A timer will be set at the start to track how long the player takes. An error counter will keep track of incorrect inputs and reshuffle the deck after every 3 errors (one penalty).

**Intended Mechanics:**  
Layout will consist of a fixed 6x6 grid containing 36 cards. Deck contains 18 matching pairs. Upon loading the system will randomly assign one card to each pair. Each card must track and display three states: face-down (hidden), face-up (revealed), cleared (matched). The player can only have a maximum of two unmatched cards flipped face-up at any time, while the two selected cards are being evaluated the player will be temporarily locked out of further input to prevent breaking the sequence. If the two cards selected are a pair, their state will be set to cleared. If the cards are different, the cards will go back to face-down. A timer will start upon match launch. An error counter will track failed inputs and after every 3 wrong inputs the system will identify remaining hidden cards and shuffles their positions.

**Intended Technical Features:**  
The game will be built using a 6x6 two-dimensional array matrix to manage the data coordinates of the 36 card slots. Each card is an object tracking its unique identifier, its matching pair ID, the card image, and its active state. An input lockout system will prevent player input during card evaluation. When two cards are selected, the evaluation loop will compare their IDs, matching cards will set their state to “cleared” while mismatched cards just get flipped back down. A running clock will track gameplay time in seconds. An error counter will monitor failed attempts and every 3 errors the player receives a penalty strike which will trigger the shuffle function to scan the grid and identify all the remaining face-down cards then randomly shuffle their positions in the grid.

**UML Sketch:**
@startuml

class GameManager {
    - grid : Card[6][6]
    - activeSelections : List<Card>
    - isInputLocked : boolean
    - elapsedTime : float
    - totalErrors : int
    + InitializeGrid() : void
    + OnCardClicked(row: int, col: int) : void
    + UpdateTimer() : void
    - EvaluateMatch() : void
    - ShuffleRemainingCards() : voi d
}

class Card {
    + cardID : int
    + pairID : int
    + cardImage : Image
    + currentState : CardState
    + SetState(newState: CardState) : void
}

class UserInterface {
    - gridLayout : Object
    - timerDisplay : Object
    - strikeDisplay : Object
    + RefreshCardView(card: Card) : void
    + ShowCardFace(card: Card) : void
    + HideCardFace(card: Card) : void
    + UpdateUI(time: float, errors: int) : void
}

enum CardState {
    FaceDown
    FaceUp
    Cleared
}

GameManager "1" *--> "36" Card : manages 
GameManager ..> UserInterface : updates 
Card ..> CardState : uses 

@enduml

## Testing, Playtesting Evidence & Reflection

### Technical & Build Testing
* **Standalone Executable Verification:** After compiling the release build, the `.exe`, `SDL3.dll`, and `assets/` folder were moved into a completely separate directory outside of Visual Studio to verify that the game runs independently without relying on IDE paths or environment variables.
* **Input & Edge-Case Testing:** Tested rapid mouse clicking during the starting preview, during match animations, and between card flips to ensure the input lockout flags (`waitingToFlip`, `previewing`, `matchPending`) prevented players from flipping more than two cards at once or breaking the turn sequence. Also verified that clicking in the gaps between cards or outside the 6x6 grid boundaries was safely ignored.
### Playtesting Reflection
* **Who Tested It:** Playtested iteratively during development.
* **What Worked Well:** Testing found that the 10-second initial preview and the `"Shuffle in: X"` countdown at the bottom of the screen added more to the game. it created a more reasonable sense of urgency and let the player plan their guesses strategically before a shuffle penalty triggered.
* **What Changed From Feedback:** Early testing with the original 3-error shuffle limit showed that the player could easily exploit the mechanic by spamming random guesses just to trigger the post-shuffle reveal as a free hint. Raising the threshold to 10 errors and keeping the post-shuffle preview brief forced players to actually rely on their memory rather than spamming the board.

### Limitations & Future Improvements
* **Visual Transitions:** Currently, card flips swap textures instantaneously. Adding a smooth scale or flip animation would make selecting cards feel much more responsive and polished.
* **Typography & UI Polish:** The HUD and menu screens currently use SDL3's scaled debug text (`SDL_RenderDebugText`). Integrating `SDL_ttf` for custom fonts, along with sound effects for card flips, matches, and shuffles, would improve the overall presentation.
* **Difficulty Modes:** Adding selectable grid sizes (e.g. 4x4 Easy, 6x6 Normal, 8x8 Hard) or various times to initial memorise (e.g. 5 seconds, 10 sec, 15 sec on the start screen would give players more replay value.


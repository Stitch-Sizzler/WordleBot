# Full-Stack Wordle Solver & Entropy Engine

A full-stack web application designed to solve the puzzle game Wordle by The New York Times. This project uses a two-tiered backend composed of a high-performance C engine that utilizes bitwise operations and Shannon Entropy, bridged to a responsive web app via a Node.js API.

## Tech Stack
* **Core Engine:** C (Bitwise operations, optimized memory management)
* **Backend (Server):** Node.js (REST API)
* **Frontend:** HTML5, CSS, JavaScript
* **Data Visualization:** Chart.js (Dynamic search-space graphing)
* **DevOps:** GitHub Actions (CI/CD), Makefile (Automated build)

## Shannon Entropy
Instead of guessing words based on simple letter frequency, this C engine utilizes **Shannon Entropy** to calculate the exact Expected Information Gain ($E$) of every possible word in the English dictionary. 

It evaluates a guess against all remaining valid answers, sorting them into 243 possible color-pattern buckets (Green/Yellow/Gray configurations). The engine then calculates the probability ($p_i$) of each bucket occurring:

$$E = -\sum_{i=1}^{243} p_i \log_2(p_i)$$

By selecting the word with the highest entropy (measured in "bits"), the bot guarantees that it halves the remaining search space as many times as mathematically possible per turn, reducing a 2,315-word dictionary to a single answer in an average of 3.4 guesses.

## Features
* **Dual-Dictionary Architecture:** Segregates potential answers (`wordle-La.txt`) from valid tactical guesses (`wordle-Ta.txt`) for optimal state space searching.
* **Dynamic Search Space Chart:** Integrates Chart.js to visually graph the exponential decay of remaining possible answers turn-by-turn.
* **C-to-Node Interoperability:** Uses Node.js to spin up stateless instances of the compiled C binary, parsing its stdout into JSON for the frontend.

## How to Run

**Prerequisites:** Ensure you have Node.js (`npm`) and a C compiler (`gcc`) installed on your system.

1. Clone or download this repository.
2. Open your terminal in the project root directory.
3. Run the automated build command:
   ```bash
   make
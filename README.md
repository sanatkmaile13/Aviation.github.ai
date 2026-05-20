# ✈️ AERO-QUEUE

### A Priority Queue-Based Runway Scheduling System for Single-Runway Air Traffic Coordination

[![Language](https://img.shields.io/badge/Language-C%20(C99)-blue?style=flat-square&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Data Structure](https://img.shields.io/badge/Data%20Structure-Priority%20Queue-orange?style=flat-square)](https://en.wikipedia.org/wiki/Priority_queue)
[![Build](https://img.shields.io/badge/Build-GCC%20%2F%20Clang-green?style=flat-square)]()
[![Memory](https://img.shields.io/badge/Memory%20Safe-Valgrind%20Clean-brightgreen?style=flat-square)]()
[![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)](LICENSE)

---

## Overview

**AERO-QUEUE** is an interactive Air Traffic Coordination simulator written in C that replaces naïve First-In-First-Out (FIFO) runway scheduling with a **sorted Priority Queue** backed by a singly-linked list. Every aircraft requesting the runway is assigned a priority coefficient derived from real-world aviation urgency (emergency, international, domestic, private) and is automatically inserted into its correct position — guaranteeing the most critical flight always reaches the runway first.

> **"An aircraft running on 8 % fuel cannot wait behind a routine private jet."**

---

## Table of Contents

- [Problem Statement](#problem-statement)
- [Features](#features)
- [Priority System](#priority-system)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Algorithm](#algorithm)
- [Sample Output](#sample-output)
- [Technical Report](#technical-report)
- [References](#references)
- [License](#license)

---

## Problem Statement

Municipal and regional airports with a single runway face a critical scheduling problem: multiple aircraft simultaneously request runway access. A FIFO approach fails catastrophically when a distressed aircraft — low on fuel or experiencing a medical emergency — is forced to queue behind routine traffic.

**AERO-QUEUE** solves this by:
- Dynamically stratifying urgency using an integer priority coefficient
- Re-ordering the queue on every insertion in **O(n)** time
- Serving the head (highest-priority) element in **O(1)** time
- Offering a clean interactive console UI for ATC operators

---

## Features

| Feature | Description |
|---|---|
| **Dynamic Insertion** | Each new flight is sorted into its correct queue position on enqueue |
| **O(1) Dispatch** | Dequeue always pops the head — no scanning required |
| **Emergency First** | Priority-1 (Emergency) flights jump to Position 1 instantly |
| **Fuel Monitoring** | Fuel percentage stored per-flight and surfaced on dispatch |
| **Search** | Locate any active flight ID in the live queue |
| **Peek** | Inspect the next scheduled flight without dispatching |
| **Memory Safety** | `free_queue()` performs full heap cleanup on exit |
| **Interactive CLI** | Operator-driven menu loop for live ATC simulation |

---

## Priority System

| Level | Category | Real-World Examples |
|:---:|---|---|
| **1** | Emergency | Fuel starvation, onboard medical emergency, engine failure |
| **2** | International | Long-haul commercial flights (Air India, Emirates, etc.) |
| **3** | Domestic | Regional and short-haul commercial flights |
| **4** | Private / Cargo | General aviation, cargo, scheduled charters |

> Lower number = higher urgency. Priority 1 always leads the queue.

---

## Project Structure

```
aero-queue/
├── src/
│   └── aero_queue.c        # Full C implementation (single-file)
├── docs/
│   ├── REPORT.md           # Full technical project report
│   └── ALGORITHM.md        # In-depth algorithm analysis
├── Makefile                # Build targets: build, run, clean, valgrind
├── .gitignore
├── README.md               # You are here
└── LICENSE
```

---

## Getting Started

### Prerequisites

- GCC ≥ 7 or Clang ≥ 6
- GNU Make
- *(Optional)* Valgrind for memory verification

### Build

```bash
git clone https://github.com/<your-username>/aero-queue.git
cd aero-queue
make
```

### Run

```bash
make run
# or directly:
./aero_queue
```

### Clean

```bash
make clean
```

### Memory Check (Valgrind)

```bash
make valgrind
```

---

## Usage

On launch, AERO-QUEUE pre-loads **8 representative flights** (including emergency flight `AI-SOS` at 8 % fuel) and sorts them automatically. The operator is then presented with an interactive menu:

```
╔══════════════════════════════════════════════╗
║        AERO-QUEUE — ATC CONTROL PANEL        ║
╠══════════════════════════════════════════════╣
║  [1] Request Runway Slot   (Enqueue Flight)  ║
║  [2] Grant Runway Access   (Dequeue Flight)  ║
║  [3] Peek Next Flight                        ║
║  [4] Display Full Queue                      ║
║  [5] Search Flight by ID                     ║
║  [6] Exit & Clear Memory                     ║
╚══════════════════════════════════════════════╝
```

---

## Algorithm

### Data Structure

A **sorted singly-linked list** acting as a priority queue:

```
HEAD → [P1: AI-SOS] → [P2: AI-202] → [P3: 6E-410] → [P4: G-PRIV] → NULL
```

### Insertion — O(n)

```c
void enqueue(const char *id, const char *airline,
             const char *type, int priority, int fuel);
```

1. Allocate and initialise a new `Flight` node.
2. If queue is empty **or** new node has higher priority than head → prepend.
3. Otherwise traverse until the correct sorted position is found → insert.

### Dispatch — O(1)

```c
void dequeue(void);
```

1. Pop `head`.
2. Print dispatch details (flight ID, airline, type, priority, fuel level).
3. `free()` the node and increment `total_served`.

See [`docs/ALGORITHM.md`](docs/ALGORITHM.md) for full complexity analysis and edge-case handling.

---

## Sample Output

```
  [BOOT] Loading simulation flights...
  [QUEUED] AI-SOS   | Air India    | Priority: 1 (Emergency)
  [QUEUED] AI-202   | Air India    | Priority: 2 (International)
  [QUEUED] EK-501   | Emirates     | Priority: 2 (International)
  [QUEUED] 6E-410   | IndiGo       | Priority: 3 (Domestic)
  [QUEUED] SG-118   | SpiceJet     | Priority: 3 (Domestic)
  [QUEUED] UK-975   | Vistara      | Priority: 3 (Domestic)
  [QUEUED] G-PRIV   | Private Jet  | Priority: 4 (Private)
  [QUEUED] FX-009   | FedEx Cargo  | Priority: 4 (Private)

  [INFO] Queue sorted. 8 flights awaiting clearance.

  *** RUNWAY CLEARED FOR: AI-SOS ***
  Airline   : Air India
  Operation : LAND
  Priority  : 1 (Emergency)
  Fuel      : 8%
```

---

## Technical Report

The full academic project report — covering problem statement, data structure design, algorithmic complexity, execution workflow, and impact analysis — is available in [`docs/REPORT.md`](docs/REPORT.md).

---

## References

- Gemini (Google AI) — Architectural concept design, algorithmic structuring of priority grading metrics, and structural template design.
- Claude (Anthropic AI) — Conceptual design validation, operational edge-case troubleshooting.
- Cormen, T. H., et al. — *Introduction to Algorithms*, 4th ed. MIT Press.
- ICAO Doc 4444 — *Procedures for Air Navigation Services: Air Traffic Management*.

---

## License

This project is licensed under the [MIT License](LICENSE).

---

<p align="center">
  Built with C · Powered by Pointer Arithmetic · Grounded in Real Aviation Logic
</p>

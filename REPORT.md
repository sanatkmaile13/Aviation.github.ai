# AERO-QUEUE — Full Technical Project Report

> **Project Type:** Data Structures & Algorithms — B.Tech Academic Project  
> **Language:** C (C99 Standard)  
> **Core Concept:** Priority Queue via Sorted Singly-Linked List  

---

## Table of Contents

1. [Project Name](#1-project-name)
2. [Problem Statement](#2-problem-statement)
3. [Proposed Solution](#3-proposed-solution)
4. [System Architecture](#4-system-architecture)
5. [Implementation Details](#5-implementation-details)
   - 5.1 [C Structure Definition](#51-c-structure-definition)
   - 5.2 [Core Algorithmic Functions](#52-core-algorithmic-functions)
   - 5.3 [Execution Workflow](#53-execution-workflow)
6. [Impact of Solution](#6-impact-of-solution)
7. [References](#7-references)

---

## 1. Project Name

**AERO-QUEUE: A Priority Queue-Based Runway Scheduling System for Single-Runway Air Traffic Coordination**

---

## 2. Problem Statement

Modern municipal and regional airports frequently operate under tight resource constraints, often relying on a **single runway** to handle a diverse influx of incoming and outgoing flights. The core operational challenge arises when multiple aircraft simultaneously request access to this single runway.

A traditional **First-In, First-Out (FIFO)** scheduling approach fails catastrophically in aviation environments. For instance, an international commercial flight running critically low on fuel — or experiencing an onboard medical crisis — cannot wait in a linear sequence behind a routine, on-time private jet or cargo transport.

Without a systematic, logic-driven scheduling framework, air traffic controllers face intense cognitive load, increasing the risk of:

- Runway incursions
- Severe flight delays
- Fatal aviation accidents caused by fuel starvation or unresolved emergencies

**The problem** is to design a computationally efficient, deterministic, and reliable scheduling engine that dynamically categorises and re-orders runway access based on structural urgency, fuel metrics, and flight classification.

---

## 3. Proposed Solution

The proposed solution is **Aero-Queue**, an interactive Air Traffic Coordination simulator built entirely in the C programming language. The system replaces standard linear scheduling with a strict **Priority Queue** data structure.

By leveraging a **Linear Singly Linked List maintained in sorted order of priority**, the system guarantees that the runway is always allocated to the aircraft with the highest critical need.

### Core Structural Features

| Feature | Description |
|---|---|
| **Dynamic Urgency Stratification** | Every aircraft is assigned a priority coefficient: `1` = Emergency, `2` = International, `3` = Domestic, `4` = Private |
| **Deterministic Re-ordering** | On every new flight registration, the queue self-sorts, ensuring no manual re-prioritisation is needed |
| **O(1) Runway Dispatch** | The head of the list always holds the highest-priority aircraft; dequeue is constant time |
| **Fuel-Aware Scheduling** | Fuel percentage is stored per node and displayed on dispatch, giving controllers immediate situational awareness |
| **Memory-Safe Cleanup** | Exit triggers `free_queue()`, deallocating all heap nodes to ensure zero memory leakage |

---

## 4. System Architecture

### Priority Tier Table

| Priority Level | Category | Real-World Scenario |
|:---:|---|---|
| `1` | **Emergency** | Fuel starvation, engine failure, onboard medical emergency |
| `2` | **International** | Long-haul commercial traffic (AI, EK, BA, etc.) |
| `3` | **Domestic** | Regional short-haul commercial traffic |
| `4` | **Private / Cargo** | General aviation, cargo, scheduled charters |

### Structural Pipeline

```
[New Flight Request]
        │
        ▼
[create_flight() — malloc() + field init]
        │
        ▼
[enqueue() — Find Sorted Position in O(n)]
        │
        ▼
[Linked List — Sorted by Priority Ascending]

HEAD → [P1: AI-SOS, 8% fuel] → [P2: AI-202] → [P3: 6E-410] → [P4: G-PRIV] → NULL
        │
        ▼
[Operator selects: Grant Runway Access]
        │
        ▼
[dequeue() — Pop Head in O(1) → free()]
```

---

## 5. Implementation Details

### 5.1 C Structure Definition

The exact architectural layout of every node managed by the Aero-Queue engine:

```c
#define MAX_FLIGHT_ID  16
#define MAX_AIRLINE    32
#define MAX_TYPE       16

typedef struct Flight {
    char        flight_id[MAX_FLIGHT_ID];   /* e.g. AI-202            */
    char        airline[MAX_AIRLINE];       /* e.g. Air India          */
    char        type[MAX_TYPE];             /* LAND or TAKEOFF         */
    int         priority;                   /* 1 (highest) – 4 (lowest)*/
    char        priority_label[20];         /* Human-readable label    */
    int         fuel_level;                 /* % remaining (0–100)     */
    struct Flight *next;                    /* Pointer to next node    */
} Flight;
```

Each `Flight` node is a self-referential structure. The `next` pointer links to the subsequent node in the priority-sorted chain. A global `head` pointer always references the highest-priority flight.

---

### 5.2 Core Algorithmic Functions

#### `create_flight()` — Node Allocation

```c
Flight *create_flight(const char *id, const char *airline,
                      const char *type, int priority, int fuel) {
    Flight *f = (Flight *)malloc(sizeof(Flight));
    if (!f) {
        fprintf(stderr, "  [ERROR] Memory allocation failed.\n");
        return NULL;
    }
    strncpy(f->flight_id, id,      sizeof(f->flight_id)  - 1);
    strncpy(f->airline,   airline, sizeof(f->airline)     - 1);
    strncpy(f->type,      type,    sizeof(f->type)        - 1);
    f->priority   = priority;
    f->fuel_level = fuel;
    f->next       = NULL;

    /* Assign human-readable priority label */
    switch (priority) {
        case 1:  strncpy(f->priority_label, "Emergency",     19); break;
        case 2:  strncpy(f->priority_label, "International", 19); break;
        case 3:  strncpy(f->priority_label, "Domestic",      19); break;
        default: strncpy(f->priority_label, "Private",       19); break;
    }
    return f;
}
```

#### `enqueue()` — Sorted Insertion — O(n)

```c
void enqueue(const char *id, const char *airline,
             const char *type, int priority, int fuel) {

    Flight *new_flight = create_flight(id, airline, type, priority, fuel);
    if (!new_flight) return;

    /* Case 1: Queue is empty, or new flight has the highest priority */
    if (head == NULL || new_flight->priority < head->priority) {
        new_flight->next = head;
        head = new_flight;
    } else {
        /* Case 2: Traverse to find the correct sorted position */
        Flight *current = head;
        while (current->next != NULL &&
               current->next->priority <= new_flight->priority) {
            current = current->next;
        }
        new_flight->next = current->next;
        current->next    = new_flight;
    }

    printf("  [QUEUED] Flight %-8s | %-14s | Priority: %d (%s)\n",
           id, airline, priority, new_flight->priority_label);
}
```

**Complexity:** O(n) — a single linear pass to locate the insertion point. This is optimal for a linked list; no re-balancing or swapping is required.

#### `dequeue()` — Head Dispatch — O(1)

```c
void dequeue(void) {
    if (head == NULL) {
        printf("  [INFO]  No flights in queue. Runway is clear.\n");
        return;
    }

    Flight *served = head;
    head = head->next;
    total_served++;

    printf("\n  *** RUNWAY CLEARED FOR: %s ***\n", served->flight_id);
    printf("  Airline   : %s\n",   served->airline);
    printf("  Operation : %s\n",   served->type);
    printf("  Priority  : %d (%s)\n", served->priority, served->priority_label);
    printf("  Fuel      : %d%%\n", served->fuel_level);

    free(served);
}
```

**Complexity:** O(1) — head removal with no traversal.

#### `peek()` — Non-Destructive Head Inspection

```c
void peek(void) {
    if (head == NULL) {
        printf("  [INFO]  Queue is empty.\n");
        return;
    }
    printf("  [NEXT]  Flight %-8s | %-14s | Priority: %d (%s) | Fuel: %d%%\n",
           head->flight_id, head->airline,
           head->priority, head->priority_label,
           head->fuel_level);
}
```

#### `search()` — Linear Flight Lookup

```c
void search(const char *id) {
    Flight *current = head;
    int position = 1;
    while (current != NULL) {
        if (strcmp(current->flight_id, id) == 0) {
            printf("  [FOUND] %s at Position %d | Priority: %d (%s) | Fuel: %d%%\n",
                   id, position, current->priority,
                   current->priority_label, current->fuel_level);
            return;
        }
        current = current->next;
        position++;
    }
    printf("  [NOT FOUND] Flight %s is not in the active queue.\n", id);
}
```

#### `free_queue()` — Full Heap Cleanup

```c
void free_queue(void) {
    Flight *current = head;
    while (current != NULL) {
        Flight *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    printf("  [CLEANUP] All nodes deallocated. Memory cleared.\n");
}
```

---

### 5.3 Execution Workflow

The program follows a clear three-stage lifecycle:

#### Stage 1 — Simulation Boot

`main()` pre-loads **8 representative flights** in deliberately out-of-order priority sequence, demonstrating automatic sorting:

| Flight ID | Airline | Operation | Priority | Fuel |
|---|---|---|:---:|:---:|
| AI-SOS | Air India | LAND | 1 — Emergency | 8% |
| AI-202 | Air India | LAND | 2 — International | 45% |
| EK-501 | Emirates | LAND | 2 — International | 62% |
| 6E-410 | IndiGo | LAND | 3 — Domestic | 74% |
| SG-118 | SpiceJet | TAKEOFF | 3 — Domestic | 88% |
| UK-975 | Vistara | TAKEOFF | 3 — Domestic | 91% |
| G-PRIV | Private Jet | LAND | 4 — Private | 55% |
| FX-009 | FedEx Cargo | TAKEOFF | 4 — Private | 67% |

After insertion, `AI-SOS` (Priority 1) is automatically positioned at the head of the queue despite being added mid-sequence.

#### Stage 2 — Interactive Operator Loop

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

The operator loop runs until option `6` is selected. All operations validate queue state before execution (e.g., dequeue on an empty queue is gracefully handled).

#### Stage 3 — Memory Cleanup

Selecting `Exit` triggers `free_queue()`, which walks the entire linked list and calls `free()` on every node. The global `head` is then set to `NULL`. This guarantees **zero memory leakage**, verified by Valgrind under all tested execution paths.

---

## 6. Impact of Solution

### Aviation Safety
By evaluating structural urgency before physical sequencing, AERO-QUEUE prevents fuel starvation events — guaranteeing distressed aircraft are brought to the runway safely before any routine traffic.

### Reduction of Cognitive Load
Rather than forcing human operators to make frantic mental calculations (who waited longer vs. who is in greater danger), the program offloads all triage decisions to a deterministic, auditable algorithm. Operators interact with outcomes, not raw data.

### Educational Value (B.Tech)
The codebase deliberately avoids advanced multi-way trees or recursive graph structures — directly targeting what is covered in standard Data Structures coursework. Every team member can:

- Fully understand the control flow
- Modify and extend the code independently
- Confidently explain and defend design choices during evaluations

### Complexity Summary

| Operation | Time Complexity | Space Complexity |
|---|:---:|:---:|
| Enqueue (sorted insert) | O(n) | O(1) per node |
| Dequeue (head pop) | O(1) | O(1) |
| Peek | O(1) | O(1) |
| Search | O(n) | O(1) |
| Display queue | O(n) | O(1) |
| Free all nodes | O(n) | O(1) |

---

## 7. References

1. **Gemini (Google AI)** — Architectural concept design, algorithmic structuring of priority grading metrics, and structural template design.
2. **Claude (Anthropic AI)** — Conceptual design validation, operational edge-case troubleshooting for simulation inputs.
3. Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. — *Introduction to Algorithms*, 4th Edition. MIT Press.
4. Kernighan, B. W., & Ritchie, D. M. — *The C Programming Language*, 2nd Edition. Prentice Hall.
5. **ICAO Doc 4444** — *Procedures for Air Navigation Services: Air Traffic Management*. International Civil Aviation Organization.
6. Sedgewick, R., & Wayne, K. — *Algorithms*, 4th Edition. Addison-Wesley.

---

*Report prepared as part of B.Tech Data Structures & Algorithms coursework.*

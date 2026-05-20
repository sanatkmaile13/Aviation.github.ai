# AERO-QUEUE — Algorithm Deep Dive

## Overview

AERO-QUEUE implements a **Priority Queue** using a **sorted singly-linked list**. This document explains every algorithmic decision, including complexity analysis, edge-case handling, and design trade-offs.

---

## Why a Linked List Over a Binary Heap?

| Criterion | Sorted Linked List | Binary Min-Heap |
|---|---|---|
| Enqueue | O(n) — linear scan | O(log n) — sift-up |
| Dequeue | O(1) — head pop | O(log n) — sift-down |
| Peek | O(1) | O(1) |
| Implementation Complexity | Low | Moderate |
| B.Tech Coursework Fit | Direct | Requires advanced knowledge |
| Memory Pattern | Pointer-linked nodes | Array-based (index math) |

For a single-runway simulation with at most a few dozen active flights, the O(n) insertion cost is negligible. The linked list approach is pedagogically transparent: every pointer operation is visible and traceable — ideal for academic evaluation and code defence.

---

## Sorted Insertion — Step-by-Step

Given the queue state:

```
HEAD → [P1: AI-SOS] → [P2: AI-202] → [P3: 6E-410] → NULL
```

Insert a new flight `EK-501` with Priority 2 (International):

### Step 1 — Allocate new node
```
new_flight = malloc(sizeof(Flight))
new_flight->priority = 2
new_flight->next = NULL
```

### Step 2 — Check head condition
```
head != NULL  ✓
new_flight->priority (2) < head->priority (1)?  ✗
→ proceed to traversal
```

### Step 3 — Traverse to insertion point
```
current = head   [P1: AI-SOS]
  current->next->priority (2) <= new_flight->priority (2)?  ✓
  current = current->next  →  [P2: AI-202]
  current->next->priority (3) <= new_flight->priority (2)?  ✗
  → STOP
```

### Step 4 — Insert
```
new_flight->next = current->next    → points to [P3: 6E-410]
current->next    = new_flight       → [P2: AI-202] now points to EK-501
```

### Result
```
HEAD → [P1: AI-SOS] → [P2: AI-202] → [P2: EK-501] → [P3: 6E-410] → NULL
```

Equal-priority flights are served in FIFO order within their tier (stable ordering).

---

## Edge Cases

### Case A — Empty Queue
```
head == NULL
→ new_flight->next = NULL
→ head = new_flight
```

### Case B — New Flight Has Highest Priority
```
new_flight->priority < head->priority
→ new_flight->next = head
→ head = new_flight
```

### Case C — New Flight Has Lowest Priority (Tail Insertion)
The while loop runs to the end (`current->next == NULL`) and appends the node at the tail — no special-casing needed.

### Case D — Dequeue on Empty Queue
```c
if (head == NULL) {
    printf("  [INFO]  No flights in queue. Runway is clear.\n");
    return;
}
```
Graceful guard — no undefined behaviour.

### Case E — Search on Empty Queue or No Match
```c
while (current != NULL) { ... }
printf("  [NOT FOUND] Flight %s is not in the active queue.\n", id);
```
Falls through naturally without crashing.

---

## Complexity Summary

| Operation | Best Case | Worst Case | Average Case |
|---|:---:|:---:|:---:|
| `enqueue` | O(1) — prepend | O(n) — tail append | O(n/2) |
| `dequeue` | O(1) | O(1) | O(1) |
| `peek` | O(1) | O(1) | O(1) |
| `search` | O(1) — head match | O(n) — not found | O(n/2) |
| `display` | O(n) | O(n) | O(n) |
| `free_queue` | O(n) | O(n) | O(n) |

**Space Complexity:** O(n) total — one node per flight in the queue at any time.

---

## Memory Model

Each `Flight` node is heap-allocated via `malloc`:

```
Stack                 Heap
─────────            ──────────────────────────────
head ──────────────► [AI-SOS | Air India | LAND | 1 | 8% | *next]
                                                           │
                                                           ▼
                                                     [AI-202 | Air India | LAND | 2 | 45% | *next]
                                                                                          │
                                                                                          ▼
                                                                                        NULL
```

`free_queue()` walks this chain, freeing each node before advancing the pointer — no dangling references, no double-frees:

```c
void free_queue(void) {
    Flight *current = head;
    while (current != NULL) {
        Flight *temp = current;   /* save current node  */
        current = current->next; /* advance before free */
        free(temp);               /* safe free           */
    }
    head = NULL;
}
```

---

## Stability Property

Within the same priority tier, flights are served in **First-In-First-Out** order. The insertion traversal uses `<=` (not `<`) when scanning:

```c
while (current->next != NULL &&
       current->next->priority <= new_flight->priority) {
    current = current->next;
}
```

This means a new Priority-3 flight inserted after an existing Priority-3 flight is placed **behind** it in the list — preserving arrival order within equal-priority tiers. This is the correct behaviour for ATC: among peers, who arrived first gets the slot.

---

## Invariants Maintained at All Times

1. `head` always points to the lowest `priority` integer in the queue (highest urgency).
2. Every node's `priority` is ≤ the next node's `priority`.
3. No node has a dangling `next` pointer — it always points to a valid `Flight` or `NULL`.
4. `total_served` monotonically increments; it is never decremented.

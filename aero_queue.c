/*
 * ============================================================
 *  AERO-QUEUE: A Priority Queue-Based Runway Scheduling System
 *  for Single-Runway Air Traffic Coordination
 * ============================================================
 *
 *  Author  : [Your Name / Team Name]
 *  Course  : Data Structures & Algorithms — B.Tech
 *  Language: C (C99 Standard)
 *  Build   : gcc -std=c99 -Wall -Wextra -o aero_queue src/aero_queue.c
 *
 *  Description:
 *    Implements a priority queue via a sorted singly-linked list to
 *    deterministically schedule runway access for aircraft based on
 *    urgency level (Emergency > International > Domestic > Private).
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Constants ─────────────────────────────────────────────── */
#define MAX_FLIGHT_ID  16
#define MAX_AIRLINE    32
#define MAX_TYPE       16

/* ── Data Structure ─────────────────────────────────────────── */
typedef struct Flight {
    char        flight_id[MAX_FLIGHT_ID];   /* e.g. AI-202            */
    char        airline[MAX_AIRLINE];       /* e.g. Air India          */
    char        type[MAX_TYPE];             /* LAND or TAKEOFF         */
    int         priority;                   /* 1 (highest) – 4 (lowest)*/
    char        priority_label[20];         /* Human-readable label    */
    int         fuel_level;                 /* % remaining (0–100)     */
    struct Flight *next;
} Flight;

/* ── Globals ────────────────────────────────────────────────── */
static Flight *head         = NULL;
static int     total_served = 0;

/* ── Forward Declarations ───────────────────────────────────── */
Flight *create_flight(const char *id, const char *airline,
                      const char *type, int priority, int fuel);
void    enqueue(const char *id, const char *airline,
                const char *type, int priority, int fuel);
void    dequeue(void);
void    peek(void);
void    display_queue(void);
void    search(const char *id);
void    free_queue(void);
void    print_banner(void);
void    print_menu(void);
void    print_separator(void);

/* ================================================================
 *  NODE CREATION
 * ================================================================ */
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
    f->flight_id[sizeof(f->flight_id) - 1] = '\0';
    f->airline  [sizeof(f->airline)   - 1] = '\0';
    f->type     [sizeof(f->type)      - 1] = '\0';

    f->priority   = priority;
    f->fuel_level = fuel;
    f->next       = NULL;

    switch (priority) {
        case 1:  strncpy(f->priority_label, "Emergency",     19); break;
        case 2:  strncpy(f->priority_label, "International", 19); break;
        case 3:  strncpy(f->priority_label, "Domestic",      19); break;
        default: strncpy(f->priority_label, "Private",       19); break;
    }
    f->priority_label[19] = '\0';

    return f;
}

/* ================================================================
 *  ENQUEUE — Sorted Insertion — O(n)
 * ================================================================ */
void enqueue(const char *id, const char *airline,
             const char *type, int priority, int fuel) {

    Flight *new_flight = create_flight(id, airline, type, priority, fuel);
    if (!new_flight) return;

    /* Case 1: Queue is empty, or new flight has highest priority */
    if (head == NULL || new_flight->priority < head->priority) {
        new_flight->next = head;
        head = new_flight;
    } else {
        /* Case 2: Find correct sorted position */
        Flight *current = head;
        while (current->next != NULL &&
               current->next->priority <= new_flight->priority) {
            current = current->next;
        }
        new_flight->next = current->next;
        current->next    = new_flight;
    }

    printf("  [QUEUED]  Flight %-8s | %-14s | Priority: %d (%s)\n",
           id, airline, priority, new_flight->priority_label);
}

/* ================================================================
 *  DEQUEUE — Head Dispatch — O(1)
 * ================================================================ */
void dequeue(void) {
    if (head == NULL) {
        printf("  [INFO]   No flights in queue. Runway is clear.\n");
        return;
    }

    Flight *served = head;
    head = head->next;
    total_served++;

    printf("\n");
    print_separator();
    printf("  *** RUNWAY CLEARED FOR: %-8s ***\n", served->flight_id);
    print_separator();
    printf("  Airline   : %s\n",   served->airline);
    printf("  Operation : %s\n",   served->type);
    printf("  Priority  : %d (%s)\n", served->priority, served->priority_label);
    printf("  Fuel      : %d%%\n", served->fuel_level);
    printf("  Served #  : %d\n",   total_served);
    print_separator();

    free(served);
}

/* ================================================================
 *  PEEK — Non-Destructive Head Inspection — O(1)
 * ================================================================ */
void peek(void) {
    if (head == NULL) {
        printf("  [INFO]   Queue is empty. No flights pending.\n");
        return;
    }
    printf("  [NEXT]   Flight %-8s | %-14s | Priority: %d (%s) | Fuel: %d%%\n",
           head->flight_id, head->airline,
           head->priority, head->priority_label,
           head->fuel_level);
}

/* ================================================================
 *  DISPLAY — Full Queue Listing — O(n)
 * ================================================================ */
void display_queue(void) {
    if (head == NULL) {
        printf("  [INFO]   Queue is empty. No flights pending.\n");
        return;
    }

    printf("\n");
    print_separator();
    printf("  %-4s  %-10s  %-16s  %-8s  %-15s  %s\n",
           "Pos", "Flight ID", "Airline", "Type", "Priority", "Fuel");
    print_separator();

    Flight *current = head;
    int pos = 1;
    while (current != NULL) {
        printf("  %-4d  %-10s  %-16s  %-8s  %d %-12s  %d%%\n",
               pos,
               current->flight_id,
               current->airline,
               current->type,
               current->priority,
               current->priority_label,
               current->fuel_level);
        current = current->next;
        pos++;
    }
    print_separator();
    printf("  Total in queue: %d\n\n", pos - 1);
}

/* ================================================================
 *  SEARCH — Linear Lookup by Flight ID — O(n)
 * ================================================================ */
void search(const char *id) {
    Flight *current = head;
    int position = 1;
    while (current != NULL) {
        if (strcmp(current->flight_id, id) == 0) {
            printf("  [FOUND]  %s at Position %d | Priority: %d (%s) | Fuel: %d%%\n",
                   id, position, current->priority,
                   current->priority_label, current->fuel_level);
            return;
        }
        current = current->next;
        position++;
    }
    printf("  [NOT FOUND] Flight '%s' is not in the active queue.\n", id);
}

/* ================================================================
 *  FREE QUEUE — Full Heap Cleanup — O(n)
 * ================================================================ */
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

/* ================================================================
 *  UI HELPERS
 * ================================================================ */
void print_separator(void) {
    printf("  %-60s\n", "────────────────────────────────────────────────────────────");
}

void print_banner(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║         AERO-QUEUE  —  ATC RUNWAY SCHEDULING SYSTEM     ║\n");
    printf("  ║      Priority Queue · Single-Runway Air Traffic Control  ║\n");
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_menu(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════════════╗\n");
    printf("  ║        AERO-QUEUE — ATC CONTROL PANEL        ║\n");
    printf("  ╠══════════════════════════════════════════════╣\n");
    printf("  ║  [1] Request Runway Slot   (Enqueue Flight)  ║\n");
    printf("  ║  [2] Grant Runway Access   (Dequeue Flight)  ║\n");
    printf("  ║  [3] Peek Next Flight                        ║\n");
    printf("  ║  [4] Display Full Queue                      ║\n");
    printf("  ║  [5] Search Flight by ID                     ║\n");
    printf("  ║  [6] Exit & Clear Memory                     ║\n");
    printf("  ╚══════════════════════════════════════════════╝\n");
    printf("  Choice: ");
}

/* ================================================================
 *  MAIN
 * ================================================================ */
int main(void) {
    print_banner();

    /* ── Simulation Boot: Pre-load 8 flights out of order ── */
    printf("  [BOOT]   Loading simulation flights...\n\n");

    enqueue("AI-SOS",  "Air India",    "LAND",    1, 8);
    enqueue("AI-202",  "Air India",    "LAND",    2, 45);
    enqueue("EK-501",  "Emirates",     "LAND",    2, 62);
    enqueue("6E-410",  "IndiGo",       "LAND",    3, 74);
    enqueue("SG-118",  "SpiceJet",     "TAKEOFF", 3, 88);
    enqueue("UK-975",  "Vistara",      "TAKEOFF", 3, 91);
    enqueue("G-PRIV",  "Private Jet",  "LAND",    4, 55);
    enqueue("FX-009",  "FedEx Cargo",  "TAKEOFF", 4, 67);

    printf("\n  [INFO]   Queue sorted. %d flights awaiting clearance.\n", 8);

    /* ── Interactive Operator Loop ── */
    int choice;
    char buffer[128];

    do {
        print_menu();
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        choice = atoi(buffer);

        switch (choice) {

            case 1: {
                char id[MAX_FLIGHT_ID], airline[MAX_AIRLINE], type[MAX_TYPE];
                int  priority, fuel;

                printf("\n  Flight ID   : "); fgets(id,      sizeof(id),      stdin); id[strcspn(id, "\n")]           = '\0';
                printf("  Airline     : "); fgets(airline, sizeof(airline), stdin); airline[strcspn(airline, "\n")] = '\0';
                printf("  Type (LAND/TAKEOFF): "); fgets(type, sizeof(type), stdin); type[strcspn(type, "\n")]       = '\0';
                printf("  Priority (1=Emergency, 2=Intl, 3=Domestic, 4=Private): ");
                fgets(buffer, sizeof(buffer), stdin); priority = atoi(buffer);
                printf("  Fuel %%      : ");
                fgets(buffer, sizeof(buffer), stdin); fuel = atoi(buffer);

                if (priority < 1 || priority > 4) {
                    printf("  [WARN]   Invalid priority. Must be 1–4.\n");
                } else {
                    printf("\n");
                    enqueue(id, airline, type, priority, fuel);
                }
                break;
            }

            case 2:
                dequeue();
                break;

            case 3:
                printf("\n");
                peek();
                break;

            case 4:
                display_queue();
                break;

            case 5: {
                char search_id[MAX_FLIGHT_ID];
                printf("\n  Flight ID to search: ");
                fgets(search_id, sizeof(search_id), stdin);
                search_id[strcspn(search_id, "\n")] = '\0';
                printf("\n");
                search(search_id);
                break;
            }

            case 6:
                printf("\n  [EXIT]   Shutting down ATC system...\n");
                printf("  [STATS]  Total flights served this session: %d\n\n", total_served);
                free_queue();
                printf("\n  Safe skies. Goodbye.\n\n");
                break;

            default:
                printf("  [WARN]   Invalid option. Please enter 1–6.\n");
                break;
        }

    } while (choice != 6);

    return EXIT_SUCCESS;
}

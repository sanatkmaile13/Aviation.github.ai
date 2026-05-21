#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────────────────────
   CONSTANTS
───────────────────────────────────────── */
#define MAX_FLIGHT_ID   10
#define MAX_AIRLINE     30
#define MAX_TYPE        20

/* Priority values (lower = higher urgency) */
#define PRIORITY_EMERGENCY      1
#define PRIORITY_INTERNATIONAL  2
#define PRIORITY_DOMESTIC       3
#define PRIORITY_PRIVATE        4

/* ─────────────────────────────────────────
   STRUCTURE: Flight Node (Linked List Node)
───────────────────────────────────────── */
typedef struct Flight {
    char        flight_id[MAX_FLIGHT_ID];   /* e.g. AI-202        */
    char        airline[MAX_AIRLINE];       /* e.g. Air India      */
    char        type[MAX_TYPE];             /* Request: LAND/TAKEOFF */
    int         priority;                   /* 1-4                 */
    char        priority_label[20];         /* Human-readable      */
    int         fuel_level;                 /* % remaining (0-100) */
    struct Flight *next;
} Flight;

/* ─────────────────────────────────────────
   GLOBAL: Head pointer of the queue
───────────────────────────────────────── */
Flight *head = NULL;
int total_served = 0;

/* ─────────────────────────────────────────
   HELPER: Map priority int → label string
───────────────────────────────────────── */
void get_priority_label(int priority, char *label) {
    switch (priority) {
        case PRIORITY_EMERGENCY:     strcpy(label, "EMERGENCY");     break;
        case PRIORITY_INTERNATIONAL: strcpy(label, "INTERNATIONAL"); break;
        case PRIORITY_DOMESTIC:      strcpy(label, "DOMESTIC");      break;
        case PRIORITY_PRIVATE:       strcpy(label, "PRIVATE JET");   break;
        default:                     strcpy(label, "UNKNOWN");        break;
    }
}

/* ─────────────────────────────────────────
   FUNCTION: Create a new Flight node
───────────────────────────────────────── */
Flight* create_flight(const char *id, const char *airline,
                      const char *type, int priority, int fuel) {
    Flight *new_flight = (Flight *)malloc(sizeof(Flight));
    if (!new_flight) {
        printf("  [ERROR] Memory allocation failed!\n");
        return NULL;
    }
    strncpy(new_flight->flight_id, id,      MAX_FLIGHT_ID - 1);
    strncpy(new_flight->airline,   airline, MAX_AIRLINE   - 1);
    strncpy(new_flight->type,      type,    MAX_TYPE      - 1);
    new_flight->priority = priority;
    new_flight->fuel_level = fuel;
    get_priority_label(priority, new_flight->priority_label);
    new_flight->next = NULL;
    return new_flight;
}

/* ─────────────────────────────────────────
   FUNCTION: Enqueue (Insert by Priority)
   Core DSC Operation — sorted insertion
   into a singly linked list
───────────────────────────────────────── */
void enqueue(const char *id, const char *airline,
             const char *type, int priority, int fuel) {

    Flight *new_flight = create_flight(id, airline, type, priority, fuel);
    if (!new_flight) return;

    /* Case 1: Queue is empty, or new flight has highest priority */
    if (head == NULL || new_flight->priority < head->priority) {
        new_flight->next = head;
        head = new_flight;
    } else {
        /* Case 2: Find correct position (sorted by priority ascending) */
        Flight *current = head;
        while (current->next != NULL &&
               current->next->priority <= new_flight->priority) {
            current = current->next;
        }
        new_flight->next = current->next;
        current->next = new_flight;
    }

    printf("  [QUEUED] Flight %-8s | %-12s | Priority: %d (%s)\n",
           id, airline, priority, new_flight->priority_label);
}

/* ─────────────────────────────────────────
   FUNCTION: Dequeue (Serve front flight)
   Removes from head (highest priority)
───────────────────────────────────────── */
void dequeue() {
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
    printf("  Status    : Runway GRANTED. Proceed immediately.\n");

    free(served);
}

/* ─────────────────────────────────────────
   FUNCTION: Peek at front (no removal)
───────────────────────────────────────── */
void peek() {
    if (head == NULL) {
        printf("  [INFO]  Queue is empty.\n");
        return;
    }
    printf("  [NEXT]  Flight %-8s | %-12s | Priority: %d (%s)\n",
           head->flight_id, head->airline,
           head->priority, head->priority_label);
}

/* ─────────────────────────────────────────
   FUNCTION: Display entire queue
───────────────────────────────────────── */
void display_queue() {
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║            CURRENT RUNWAY QUEUE STATUS                      ║\n");
    printf("  ╠══════╦══════════╦══════════════════╦════════╦═══════════════╣\n");
    printf("  ║ Pos  ║ FlightID ║ Airline          ║  Fuel  ║ Priority      ║\n");
    printf("  ╠══════╬══════════╬══════════════════╬════════╬═══════════════╣\n");

    if (head == NULL) {
        printf("  ║                    QUEUE IS EMPTY                           ║\n");
    } else {
        Flight *current = head;
        int pos = 1;
        while (current != NULL) {
            printf("  ║  %-3d ║ %-8s ║ %-16s ║  %3d%%  ║ %-13s ║\n",
                   pos,
                   current->flight_id,
                   current->airline,
                   current->fuel_level,
                   current->priority_label);
            current = current->next;
            pos++;
        }
    }

    printf("  ╚══════╩══════════╩══════════════════╩════════╩═══════════════╝\n");
    printf("  Total flights served today: %d\n\n", total_served);
}

/* ─────────────────────────────────────────
   FUNCTION: Count flights in queue
───────────────────────────────────────── */
int count_flights() {
    int count = 0;
    Flight *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

/* ─────────────────────────────────────────
   FUNCTION: Search flight by ID
───────────────────────────────────────── */
void search_flight(const char *id) {
    Flight *current = head;
    int pos = 1;
    while (current != NULL) {
        if (strcmp(current->flight_id, id) == 0) {
            printf("  [FOUND] Flight %s at position %d in queue.\n", id, pos);
            printf("          Airline: %s | Priority: %s | Fuel: %d%%\n",
                   current->airline, current->priority_label, current->fuel_level);
            return;
        }
        current = current->next;
        pos++;
    }
    printf("  [NOT FOUND] Flight %s is not in the queue.\n", id);
}

/* ─────────────────────────────────────────
   FUNCTION: Free all memory (cleanup)
───────────────────────────────────────── */
void free_queue() {
    Flight *current = head;
    while (current != NULL) {
        Flight *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
}

/* ─────────────────────────────────────────
   FUNCTION: Print separator line
───────────────────────────────────────── */
void print_separator() {
    printf("\n  ──────────────────────────────────────────────────────────────\n\n");
}

/* ─────────────────────────────────────────
   FUNCTION: Print banner
───────────────────────────────────────── */
void print_banner() {
    printf("\n");
    printf("  ████████████████████████████████████████████████████████████\n");
    printf("  █                                                          █\n");
    printf("  █        ✈  AVIATION CONTROL SYSTEM  ✈                    █\n");
    printf("  █            RUNWAY SCHEDULER v1.0                        █\n");
    printf("  █        Priority Queue — Linked List Implementation      █\n");
    printf("  █                                                          █\n");
    printf("  ████████████████████████████████████████████████████████████\n");
    printf("\n");
    printf("  PRIORITY GUIDE:\n");
    printf("    [1] EMERGENCY    — Fuel critical / Mayday signal\n");
    printf("    [2] INTERNATIONAL— Scheduled international flight\n");
    printf("    [3] DOMESTIC     — Scheduled domestic flight\n");
    printf("    [4] PRIVATE JET  — Private / charter aircraft\n\n");
}

/* ─────────────────────────────────────────
   FUNCTION: Interactive Menu
───────────────────────────────────────── */
void interactive_menu() {
    int choice, priority, fuel;
    char id[MAX_FLIGHT_ID], airline[MAX_AIRLINE], type[MAX_TYPE];

    while (1) {
        printf("  ┌─────────────────────────────────┐\n");
        printf("  │        CONTROL TOWER MENU       │\n");
        printf("  ├─────────────────────────────────┤\n");
        printf("  │  1. Request Runway (Add Flight) │\n");
        printf("  │  2. Grant Runway (Serve Next)   │\n");
        printf("  │  3. View Queue Status           │\n");
        printf("  │  4. Peek Next Flight            │\n");
        printf("  │  5. Search Flight               │\n");
        printf("  │  6. Flight Count                │\n");
        printf("  │  7. Exit                        │\n");
        printf("  └─────────────────────────────────┘\n");
        printf("  Enter choice: ");
        scanf("%d", &choice);
        print_separator();

        switch (choice) {

            case 1:
                printf("  Enter Flight ID   (e.g. AI-202)  : ");
                scanf("%s", id);
                printf("  Enter Airline Name               : ");
                scanf(" %[^\n]", airline);
                printf("  Enter Operation (LAND/TAKEOFF)   : ");
                scanf("%s", type);
                printf("  Enter Priority (1-Emergency to 4): ");
                scanf("%d", &priority);
                if (priority < 1 || priority > 4) {
                    printf("  [ERROR] Invalid priority. Use 1-4.\n");
                    break;
                }
                printf("  Enter Fuel Level (%% remaining)  : ");
                scanf("%d", &fuel);
                enqueue(id, airline, type, priority, fuel);
                break;

            case 2:
                dequeue();
                break;

            case 3:
                display_queue();
                break;

            case 4:
                peek();
                break;

            case 5:
                printf("  Enter Flight ID to search: ");
                scanf("%s", id);
                search_flight(id);
                break;

            case 6:
                printf("  [INFO] Flights in queue: %d\n", count_flights());
                break;

            case 7:
                printf("  [SHUTDOWN] Control Tower signing off. All systems clear.\n\n");
                free_queue();
                return;

            default:
                printf("  [ERROR] Invalid choice. Please try again.\n");
        }
        print_separator();
    }
}

/* ─────────────────────────────────────────
   MAIN — Demo Simulation + Interactive Menu
───────────────────────────────────────── */
int main() {
    print_banner();

    /* ── DEMO SIMULATION ─────────────────── */
    printf("  [ SIMULATION MODE — Pre-loading scenario ]\n\n");

    /* Flights arrive in random order — queue sorts by priority automatically */
    printf("  Incoming flight requests:\n");
    enqueue("6E-401",  "IndiGo",        "LAND",    PRIORITY_DOMESTIC,      78);
    enqueue("9W-707",  "Jet Airways",   "TAKEOFF",  PRIORITY_INTERNATIONAL, 62);
    enqueue("VT-PJT",  "Private Jet",   "LAND",    PRIORITY_PRIVATE,       90);
    enqueue("AI-SOS",  "Air India",     "LAND",    PRIORITY_EMERGENCY,      8);  /* Mayday! */
    enqueue("EK-521",  "Emirates",      "LAND",    PRIORITY_INTERNATIONAL, 45);
    enqueue("SG-303",  "SpiceJet",      "TAKEOFF",  PRIORITY_DOMESTIC,      85);
    enqueue("VT-XYZ",  "Charter Co.",   "LAND",    PRIORITY_PRIVATE,       95);
    enqueue("UK-837",  "Vistara",       "LAND",    PRIORITY_INTERNATIONAL, 55);

    print_separator();
    display_queue();

    printf("  [ Serving flights one by one ]\n");
    print_separator();

    /* Serve top 3 — watch emergency go first */
    dequeue();
    print_separator();
    dequeue();
    print_separator();
    dequeue();
    print_separator();

    display_queue();

    /* ── INTERACTIVE MENU ─────────────────── */
    printf("  [ Entering Interactive Control Tower Mode ]\n");
    print_separator();
    interactive_menu();

    return 0;
}

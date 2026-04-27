#include <stdio.h>

#define MAX 20
#define QUEUES 3

// Queue labels and their scheduling algorithms
// Q0 - Highest Priority -> Round Robin
// Q1 - Medium Priority  -> Round Robin
// Q2 - Lowest Priority  -> FCFS (quantum = infinity)

typedef struct {
    int pid;
    int burst;
    int arrival;
    int queue_level;   // 0, 1, or 2
    int remaining;
    int completion;
    int turnaround;
    int waiting;
    int finished;
} Process;

int main() {
    Process p[MAX];
    int n, quantum[QUEUES];
    int time = 0, done = 0;

    // ---------- Input ----------
    printf("=== Multilevel Queue Scheduling ===\n");
    printf("Queues:\n");
    printf("  Q0 - High Priority   (Round Robin)\n");
    printf("  Q1 - Medium Priority (Round Robin)\n");
    printf("  Q2 - Low Priority    (FCFS)\n\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter time quantum for Q0 (High Priority RR): ");
    scanf("%d", &quantum[0]);
    printf("Enter time quantum for Q1 (Medium Priority RR): ");
    scanf("%d", &quantum[1]);
    quantum[2] = 9999; // FCFS = effectively infinite quantum

    printf("\n");
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("--- Process P%d ---\n", p[i].pid);
        printf("  Burst Time   : "); scanf("%d", &p[i].burst);
        printf("  Arrival Time : "); scanf("%d", &p[i].arrival);
        printf("  Queue Level  (0/1/2): "); scanf("%d", &p[i].queue_level);
        p[i].remaining  = p[i].burst;
        p[i].completion = p[i].turnaround = p[i].waiting = p[i].finished = 0;
        printf("\n");
    }

    // ---------- Gantt Chart ----------
    printf("Gantt Chart:\n|");

    while (done < n) {
        int idle = 1;

        // Always serve highest priority queue first (Q0 → Q1 → Q2)
        for (int q = 0; q < QUEUES && idle; q++) {
            for (int i = 0; i < n; i++) {
                if (p[i].queue_level == q &&
                    p[i].remaining > 0 &&
                    p[i].arrival <= time &&
                    !p[i].finished) {

                    idle = 0;

                    // Check if any higher-priority process is waiting
                    // (preemption: if a Q0 process arrives while Q1 is running)
                    // Here we re-scan from top each iteration — natural preemption

                    int run = (p[i].remaining < quantum[q]) ? p[i].remaining : quantum[q];
                    printf(" P%d/Q%d(%d-%d) |", p[i].pid, q, time, time + run);

                    p[i].remaining -= run;
                    time += run;

                    if (p[i].remaining == 0) {
                        p[i].finished    = 1;
                        p[i].completion  = time;
                        p[i].turnaround  = p[i].completion - p[i].arrival;
                        p[i].waiting     = p[i].turnaround - p[i].burst;
                        done++;
                    }

                    // After each time slice, restart from Q0
                    // (so higher queues always get priority)
                    break;
                }
            }
        }

        if (idle) time++;
    }

    // ---------- Output Table ----------
    printf("\n\n");
    printf("%-8s %-8s %-10s %-8s %-12s %-12s %-10s %-10s\n",
           "Process", "Queue", "Burst", "Arrival",
           "Completion", "Turnaround", "Waiting", "Remaining");
    printf("--------------------------------------------------------------------------\n");

    float totalTAT = 0, totalWT = 0;

    for (int q = 0; q < QUEUES; q++) {
        for (int i = 0; i < n; i++) {
            if (p[i].queue_level == q) {
                printf("P%-7d Q%-7d %-10d %-8d %-12d %-12d %-10d %-10d\n",
                       p[i].pid, p[i].queue_level,
                       p[i].burst, p[i].arrival,
                       p[i].completion, p[i].turnaround,
                       p[i].waiting, p[i].remaining);
                totalTAT += p[i].turnaround;
                totalWT  += p[i].waiting;
            }
        }
        // Separator between queues
        if (q < QUEUES - 1) printf("  .....\n");
    }

    printf("--------------------------------------------------------------------------\n");
    printf("\nAverage Turnaround Time : %.2f\n", totalTAT / n);
    printf("Average Waiting Time    : %.2f\n",  totalWT  / n);

    return 0;
}
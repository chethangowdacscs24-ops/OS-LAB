#include <stdio.h>
#include <math.h>

#define MAX 10

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { return (a / gcd(a, b)) * b; }

int main() {
    int n;
    int period[MAX], burst[MAX], remaining[MAX], next_deadline[MAX];
    char name[MAX][10];

    printf("===== Earliest Deadline First (EDF) Scheduling =====\n\n");
    printf("Enter number of tasks: ");
    scanf("%d", &n);

    double U = 0.0;
    for (int i = 0; i < n; i++) {
        sprintf(name[i], "T%d", i + 1);
        printf("\nTask T%d:\n", i + 1);
        printf(" Period (T) : "); scanf("%d", &period[i]);
        printf(" CPU Burst (C): "); scanf("%d", &burst[i]);
        
        U += (double)burst[i] / period[i];
        remaining[i] = burst[i];
        next_deadline[i] = period[i]; // Initial deadline is the first period
    }

    printf("\n────────────────────────────────────────\n");
    printf(" Schedulability Check\n");
    printf("────────────────────────────────────────\n");
    printf(" Total Utilization U = %.4f\n", U);
    printf(" EDF Bound = 1.0000\n");
    printf(" Result: %s\n", (U <= 1.0) ? "SCHEDULABLE" : "NOT SCHEDULABLE (Overloaded)");

    int hyper = period[0];
    for (int i = 1; i < n; i++) hyper = lcm(hyper, period[i]);
    printf(" Hyperperiod = %d\n", hyper);

    char g_name[1000][10];
    int g_start[1000], g_end[1000], segments = 0;
    int prev_id = -1, seg_start = 0, deadline_miss = 0;

    for (int t = 0; t < hyper; t++) {
        // 1. Check for new releases and deadline misses
        for (int i = 0; i < n; i++) {
            if (t > 0 && t % period[i] == 0) {
                if (remaining[i] > 0) {
                    printf(" *** DEADLINE MISS: %s at t=%d ***\n", name[i], t);
                    deadline_miss = 1;
                }
                remaining[i] = burst[i];
                next_deadline[i] = t + period[i]; // Update absolute deadline
            }
        }

        // 2. Pick task with the EARLIEST absolute deadline
        int chosen = -1;
        int min_deadline = 999999;

        for (int i = 0; i < n; i++) {
            if (remaining[i] > 0) {
                if (next_deadline[i] < min_deadline) {
                    min_deadline = next_deadline[i];
                    chosen = i;
                }
            }
        }

        // 3. Track segments for Gantt Chart
        if (chosen != prev_id) {
            if (t > seg_start) {
                sprintf(g_name[segments], "%s", (prev_id == -1) ? "Idle" : name[prev_id]);
                g_start[segments] = seg_start;
                g_end[segments] = t;
                segments++;
            }
            seg_start = t;
            prev_id = chosen;
        }

        if (chosen != -1) remaining[chosen]--;
    }

    // Record the final segment
    sprintf(g_name[segments], "%s", (prev_id == -1) ? "Idle" : name[prev_id]);
    g_start[segments] = seg_start;
    g_end[segments] = hyper;
    segments++;

    // Print Gantt Chart
    printf("\n────────────────────────────────────────\n");
    printf(" Gantt Chart (0 to %d)\n", hyper);
    printf("────────────────────────────────────────\n|");
    for (int i = 0; i < segments; i++) printf(" %s(%d-%d) |", g_name[i], g_start[i], g_end[i]);
    printf("\n\n Deadline misses: %s\n", deadline_miss ? "YES" : "None");

    return 0;
}

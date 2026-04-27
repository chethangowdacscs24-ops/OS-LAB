#include <stdio.h>
#include <math.h>

#define MAX 10

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { return (a / gcd(a, b)) * b; }

int main() {
    int n;
    int period[MAX], burst[MAX];
    char name[MAX][5];

    printf("===== Rate-Monotonic Scheduling =====\n\n");
    printf("Enter number of tasks: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        sprintf(name[i], "T%d", i + 1);
        printf("\nTask T%d:\n", i + 1);
        printf("  Period (T)         : "); scanf("%d", &period[i]);
        printf("  CPU Burst time (C) : "); scanf("%d", &burst[i]);
    }

    // Sort by period ascending (shortest = highest priority)
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (period[i] > period[j]) {
                int tmp;
                tmp = period[i]; period[i] = period[j]; period[j] = tmp;
                tmp = burst[i];  burst[i]  = burst[j];  burst[j]  = tmp;
                char t[5];
                for (int k = 0; k < 5; k++) t[k]       = name[i][k];
                for (int k = 0; k < 5; k++) name[i][k] = name[j][k];
                for (int k = 0; k < 5; k++) name[j][k] = t[k];
            }
        }
    }

    // Utilization & RMS Bound
    double U     = 0.0;
    double bound = n * (pow(2.0, 1.0 / n) - 1.0);

    for (int i = 0; i < n; i++)
        U += (double)burst[i] / period[i];

    printf("\n────────────────────────────────────────\n");
    printf("  Schedulability Check\n");
    printf("────────────────────────────────────────\n");
    printf("  Total Utilization  U = %.4f\n", U);
    printf("  RMS Bound = %d x (2^(1/%d) - 1) = %.4f\n", n, n, bound);
    printf("  Theoretical max (n->inf) = ln(2) = 0.6931\n\n");

    printf("  Result: ");
    if (U <= bound) {
        printf("SCHEDULABLE (U <= RMS bound)\n");
    } else if (U <= 1.0) {
        printf("POSSIBLY SCHEDULABLE\n");
        printf("  (U <= 1.0 but exceeds RMS bound - exact analysis needed)\n");
    } else {
        printf("NOT SCHEDULABLE (U > 1.0 - CPU overloaded)\n");
    }

    // Hyperperiod
    int hyper = period[0];
    for (int i = 1; i < n; i++) hyper = lcm(hyper, period[i]);
    printf("\n  Hyperperiod (LCM) = %d time units\n", hyper);

    // Simulate
    int remaining[MAX];
    int deadline_miss = 0;

    for (int i = 0; i < n; i++) remaining[i] = burst[i];

    char g_name[500][8];
    int  g_start[500], g_end[500];
    int  segments = 0;
    int  prev = -2, seg_start = 0;

    for (int t = 0; t < hyper; t++) {

        // Release new instances + check deadline miss
        for (int i = 0; i < n; i++) {
            if (t > 0 && t % period[i] == 0) {
                if (remaining[i] > 0) {
                    printf("  *** DEADLINE MISS: %s at t=%d (remaining=%d) ***\n",
                           name[i], t, remaining[i]);
                    deadline_miss = 1;
                }
                remaining[i] = burst[i];
            }
        }

        // Pick highest-priority ready task
        int chosen = -1;
        for (int i = 0; i < n; i++) {
            if (remaining[i] > 0) { chosen = i; break; }
        }

        // Record segment changes
        int cur = (chosen == -1) ? -2 : chosen;
        if (cur != prev) {
            if (seg_start < t) {
                int s = segments++;
                sprintf(g_name[s], "%s", (prev == -2) ? "Idle" : name[prev]);
                g_start[s] = seg_start;
                g_end[s]   = t;
            }
            seg_start = t;
            prev = cur;
        }

        if (chosen != -1) remaining[chosen]--;
    }
    // Last segment
    {
        int s = segments++;
        sprintf(g_name[s], "%s", (prev == -2) ? "Idle" : name[prev]);
        g_start[s] = seg_start;
        g_end[s]   = hyper;
    }

    // Print Gantt Chart
    printf("\n────────────────────────────────────────\n");
    printf("  Gantt Chart (0 to %d)\n", hyper);
    printf("────────────────────────────────────────\n");

    printf("|");
    for (int i = 0; i < segments; i++)
        printf(" %s(%d-%d) |", g_name[i], g_start[i], g_end[i]);
    printf("\n");

    // Timeline ruler
    printf("0");
    for (int i = 0; i < segments; i++) {
        int width = (g_end[i] - g_start[i]) * 2;
        for (int j = 0; j < width; j++) printf("-");
        printf("%d", g_end[i]);
    }
    printf("\n");

    printf("\n  Deadline misses: %s\n",
           deadline_miss ? "YES (see above)" : "None");

    return 0;
}
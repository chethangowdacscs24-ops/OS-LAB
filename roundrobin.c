#include <stdio.h>

#define MAX 10

int main() {
    int pid[MAX], burst[MAX], arrival[MAX];
    int remaining[MAX], completion[MAX], turnaround[MAX], waiting[MAX];
    int n, quantum;
    int time = 0, done = 0;

    // ---------- Input ----------
    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    printf("\n%-8s %-12s %-12s\n", "Process", "Burst Time", "Arrival Time");
    printf("--------------------------------\n");
    for (int i = 0; i < n; i++) {
        pid[i] = i + 1;
        printf("P%d - Burst Time: ", pid[i]);
        scanf("%d", &burst[i]);
        printf("P%d - Arrival Time: ", pid[i]);
        scanf("%d", &arrival[i]);
        remaining[i] = burst[i];
        completion[i] = turnaround[i] = waiting[i] = 0;
    }

    // ---------- Gantt Chart ----------
    printf("\nGantt Chart:\n|");

    while (done < n) {
        int idle = 1;

        for (int i = 0; i < n; i++) {
            if (remaining[i] > 0 && arrival[i] <= time) {
                idle = 0;

                int run = (remaining[i] < quantum) ? remaining[i] : quantum;
                printf(" P%d(%d-%d) |", pid[i], time, time + run);

                remaining[i] -= run;
                time += run;

                if (remaining[i] == 0) {
                    completion[i]  = time;
                    turnaround[i]  = completion[i] - arrival[i];
                    waiting[i]     = turnaround[i] - burst[i];
                    done++;
                }
            }
        }

        if (idle) time++;
    }

    printf("\n\n");
    printf("%-8s %-10s %-10s %-12s %-14s %-14s %-14s\n",
           "Process", "Burst", "Arrival", "Completion", "Turnaround",
           "Waiting", "Remaining");
    printf("-------------------------------------------------------------------------\n");

    float totalTAT = 0, totalWT = 0;

    for (int i = 0; i < n; i++) {
        printf("P%-7d %-10d %-10d %-12d %-14d %-14d %-14d\n",
               pid[i], burst[i], arrival[i],
               completion[i], turnaround[i], waiting[i], remaining[i]);
        totalTAT += turnaround[i];
        totalWT  += waiting[i];
    }

    printf("-------------------------------------------------------------------------\n");
    printf("\nAverage Turnaround Time : %.2f\n", totalTAT / n);
    printf("Average Waiting Time    : %.2f\n",  totalWT  / n);

    return 0;
}
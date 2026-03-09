#include <stdio.h>
#include <limits.h>

int main() {
    int n,i,time=0,remain,highest;
    int bt[20],rt[20],pr[20],wt[20],tat[20];

    printf("Enter number of processes: ");
    scanf("%d",&n);

    for(i=0;i<n;i++) {
        printf("Enter Burst Time for P%d: ",i+1);
        scanf("%d",&bt[i]);
        rt[i]=bt[i];

        printf("Enter Priority for P%d: ",i+1);
        scanf("%d",&pr[i]);
    }

    remain=n;

    while(remain!=0) {
        highest=-1;
        int min=INT_MAX;

        for(i=0;i<n;i++) {
            if(rt[i]>0 && pr[i]<min) {
                min=pr[i];
                highest=i;
            }
        }

        rt[highest]--;
        time++;

        if(rt[highest]==0) {
            remain--;
            tat[highest]=time;
            wt[highest]=tat[highest]-bt[highest];
        }
    }

    printf("\nProcess\tBT\tPriority\tWT\tTAT\n");

    for(i=0;i<n;i++)
        printf("P%d\t%d\t%d\t\t%d\t%d\n",i+1,bt[i],pr[i],wt[i],tat[i]);

    return 0;
}
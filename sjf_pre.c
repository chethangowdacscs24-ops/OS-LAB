#include <stdio.h>
#include <limits.h>

int main() {
    int n,i,time=0,remain,smallest;
    int bt[20],rt[20],wt[20],tat[20];

    printf("Enter number of processes: ");
    scanf("%d",&n);

    for(i=0;i<n;i++) {
        printf("Enter Burst Time for P%d: ",i+1);
        scanf("%d",&bt[i]);
        rt[i]=bt[i];
    }

    remain=n;

    while(remain!=0) {
        smallest=-1;
        int min=INT_MAX;

        for(i=0;i<n;i++) {
            if(rt[i]>0 && rt[i]<min) {
                min=rt[i];
                smallest=i;
            }
        }

        rt[smallest]--;
        time++;

        if(rt[smallest]==0) {
            remain--;
            tat[smallest]=time;
            wt[smallest]=tat[smallest]-bt[smallest];
        }
    }

    printf("\nProcess\tBT\tWT\tTAT\n");

    for(i=0;i<n;i++)
        printf("P%d\t%d\t%d\t%d\n",i+1,bt[i],wt[i],tat[i]);

    return 0;
}
#include<stdio.h>
int main(){
printf("Enter the number of processes: ");
int n,twt,att;
float awt,ttat;
scanf("%d",&n);
int bt[n];
int at[n];
int wt[n];
int tat[n];
int ct[n];
int rt[n];
for(int i=0;i<n;i++){
    printf("Enter burst time for process %d: ",i+1);
    scanf("%d",&bt[i]);
    printf("Enter arrival time for process %d: ",i+1);
    scanf("%d",&at[i]);
}
wt[0]=0;
ct[0]=bt[0];
rt[0]=wt[0];
for(int i=1;i<n;i++){
   if(at[i]>ct[i-1]){
    ct[i]=at[i]+bt[i];
    tat[i]=ct[i]-at[i];
    wt[i]= tat[i]-bt[i];
   }
  else{ ct[i]=ct[i-1]+bt[i];
    tat[i]=ct[i]-at[i];
    wt[i]= tat[i]-bt[i];
}}
for (int i = 0; i < n; i++) {
twt += wt[i];
ttat += tat[i];
}
awt = twt / n;
att = ttat / n;
// Output Table
printf("\nPROCESS\tAT\tBT\tCT\tWT\tTAT");
for (int i = 0; i < n; i++) {
printf("\nP%d\t%d\t%d\t%d\t%d\t%d",
i + 1, at[i], bt[i], ct[i], wt[i], tat[i]);
}
printf("\n\nAverage Waiting Time = %.2f", awt);
printf("\nAverage Turnaround Time = %.2f\n", att);
return 0;
}
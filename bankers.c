#include<stdio.h>
#include<stdbool.h>
#include<string.h>

int available[10]={0};
int n_processes, n_resources;

typedef struct process{
    int max[10];
    int allocated[10];
    int need[10];
    bool done;
} pro;

pro process[10];
int sequence[10];
int seq_count = 0;

// Check if a process can finish with current available resources
bool isvalid(pro p, int r){
    if(p.done) return false;
    else{
        for(int i=0; i<r; i++){
            if(p.need[i] > available[i]) return false;
        }
        return true;
    }
}

// ============ SAFETY ALGORITHM ============
bool safetyAlgorithm(int r, int n, bool print_output){
    int work[10];
    bool finish[10];
    
    // Step 1: Initialize
    for(int j=0; j<r; j++){
        work[j] = available[j];
    }
    for(int i=0; i<n; i++){
        finish[i] = process[i].done;
    }
    
    seq_count = 0;
    
    if(print_output){
        printf("\n--- Running Safety Algorithm ---\n");
        printf("Work (initial): ");
        for(int j=0; j<r; j++) printf("%d ", work[j]);
        printf("\n");
    }
    
    // Step 2 & 3: Find processes that can finish
    for(int iterations=0; iterations<n; iterations++){
        bool found = false;
        
        for(int i=0; i<n; i++){
            if(!finish[i]){  // Process not finished yet
                // Check if need[i] <= work
                bool can_finish = true;
                for(int j=0; j<r; j++){
                    if(process[i].need[j] > work[j]){
                        can_finish = false;
                        break;
                    }
                }
                
                if(can_finish){
                    found = true;
                    if(print_output){
                        printf("P%d can finish. ", i);
                        printf("Work: ");
                        for(int j=0; j<r; j++) printf("%d ", work[j]);
                    }
                    
                    // Release resources
                    for(int j=0; j<r; j++){
                        work[j] += process[i].allocated[j];
                    }
                    
                    finish[i] = true;
                    sequence[seq_count++] = i;
                    
                    if(print_output){
                        printf(" -> After release: ");
                        for(int j=0; j<r; j++) printf("%d ", work[j]);
                        printf("\n");
                    }
                    break;
                }
            }
        }
        
        if(!found) break;
    }
    
    // Step 4: Check if all processes finished
    bool all_finished = true;
    for(int i=0; i<n; i++){
        if(!finish[i]){
            all_finished = false;
            break;
        }
    }
    
    return all_finished;
}

// ============ RESOURCE-REQUEST ALGORITHM ============
bool resourceRequest(int process_id, int *request, int r){
    printf("\n--- Resource-Request Algorithm for P%d ---\n", process_id);
    printf("Request: ");
    for(int i=0; i<r; i++) printf("%d ", request[i]);
    printf("\n");
    
    // Step 1: Check if request <= need
    printf("\nStep 1: Check if Request <= Need\n");
    bool valid_request = true;
    for(int i=0; i<r; i++){
        if(request[i] > process[process_id].need[i]){
            valid_request = false;
            break;
        }
    }
    
    if(!valid_request){
        printf("ERROR: Request exceeds maximum claims!\n");
        return false;
    }
    printf("✓ Request is within max claims\n");
    
    // Step 2: Check if request <= available
    printf("\nStep 2: Check if Request <= Available\n");
    bool resources_available = true;
    for(int i=0; i<r; i++){
        if(request[i] > available[i]){
            resources_available = false;
            break;
        }
    }
    
    if(!resources_available){
        printf("✗ Insufficient resources available. P%d must wait.\n", process_id);
        return false;
    }
    printf("✓ Resources are available\n");
    
    // Step 3: Pretend to allocate
    printf("\nStep 3: Pretend to allocate and check safety\n");
    
    // Save current state
    int saved_available[10];
    int saved_allocated[10];
    int saved_need[10];
    
    for(int j=0; j<r; j++){
        saved_available[j] = available[j];
        saved_allocated[j] = process[process_id].allocated[j];
        saved_need[j] = process[process_id].need[j];
    }
    
    // Modify state
    for(int j=0; j<r; j++){
        available[j] -= request[j];
        process[process_id].allocated[j] += request[j];
        process[process_id].need[j] -= request[j];
    }
    
    printf("Allocated (new): ");
    for(int j=0; j<r; j++) printf("%d ", process[process_id].allocated[j]);
    printf("\nNeed (new): ");
    for(int j=0; j<r; j++) printf("%d ", process[process_id].need[j]);
    printf("\nAvailable (new): ");
    for(int j=0; j<r; j++) printf("%d ", available[j]);
    printf("\n");
    
    // Run safety algorithm (no print)
    bool is_safe = safetyAlgorithm(r, n_processes, false);
    
    if(is_safe){
        printf("\n✓ SAFE STATE: Request GRANTED\n");
        printf("Safe sequence: < ");
        for(int i=0; i<seq_count; i++){
            printf("P%d", sequence[i]);
            if(i < seq_count-1) printf(", ");
        }
        printf(" >\n");
        return true;
    }
    else{
        printf("\n✗ UNSAFE STATE: Request DENIED. P%d must wait.\n", process_id);
        // Restore state
        for(int j=0; j<r; j++){
            available[j] = saved_available[j];
            process[process_id].allocated[j] = saved_allocated[j];
            process[process_id].need[j] = saved_need[j];
        }
        return false;
    }
}

// ============ DETECTION ALGORITHM ============
bool detectionAlgorithm(int r, int n){
    int work[10];
    bool finish[10];
    int deadlocked[10];
    int deadlock_count = 0;
    
    printf("\n--- Detection Algorithm ---\n");
    
    // Step 1: Initialize
    for(int j=0; j<r; j++){
        work[j] = available[j];
    }
    
    for(int i=0; i<n; i++){
        // If allocated = 0, mark as finished
        bool has_allocation = false;
        for(int j=0; j<r; j++){
            if(process[i].allocated[j] != 0){
                has_allocation = true;
                break;
            }
        }
        finish[i] = !has_allocation;
    }
    
    printf("Work (initial): ");
    for(int j=0; j<r; j++) printf("%d ", work[j]);
    printf("\n");
    
    // Step 2 & 3: Find processes that can finish
    for(int iterations=0; iterations<n; iterations++){
        bool found = false;
        
        for(int i=0; i<n; i++){
            if(!finish[i]){
                // Check if request[i] <= work (no need, only request)
                bool can_finish = true;
                for(int j=0; j<r; j++){
                    // Using need as request since we don't have request table in detection
                    if(process[i].need[j] > work[j]){
                        can_finish = false;
                        break;
                    }
                }
                
                if(can_finish){
                    found = true;
                    printf("P%d can finish. Work: ", i);
                    
                    // Release resources
                    for(int j=0; j<r; j++){
                        work[j] += process[i].allocated[j];
                    }
                    
                    for(int j=0; j<r; j++) printf("%d ", work[j]);
                    printf("\n");
                    
                    finish[i] = true;
                    break;
                }
            }
        }
        
        if(!found) break;
    }
    
    // Step 4: Find deadlocked processes
    printf("\nResult: ");
    bool has_deadlock = false;
    for(int i=0; i<n; i++){
        if(!finish[i]){
            has_deadlock = true;
            deadlocked[deadlock_count++] = i;
        }
    }
    
    if(has_deadlock){
        printf("DEADLOCK DETECTED\n");
        printf("Deadlocked processes: ");
        for(int i=0; i<deadlock_count; i++){
            printf("P%d ", deadlocked[i]);
        }
        printf("\n");
        return false;
    }
    else{
        printf("NO DEADLOCK\n");
        return true;
    }
}

// ============ DISPLAY STATE ============
void displayState(){
    printf("\n--- Current System State ---\n");
    printf("Available: ");
    for(int j=0; j<n_resources; j++) printf("%d ", available[j]);
    printf("\n\n");
    
    printf("%-6s %-20s %-20s %-20s\n", "Proc", "Allocated", "Max", "Need");
    printf("-------------------------------------------------------------------\n");
    for(int i=0; i<n_processes; i++){
        printf("P%-5d ", i);
        for(int j=0; j<n_resources; j++) printf("%d ", process[i].allocated[j]);
        printf("     ");
        for(int j=0; j<n_resources; j++) printf("%d ", process[i].max[j]);
        printf("     ");
        for(int j=0; j<n_resources; j++) printf("%d ", process[i].need[j]);
        printf("\n");
    }
}

// ============ MAIN ============
int main(){
    int choice;
    
    printf("========================================\n");
    printf("   BANKER'S ALGORITHM IMPLEMENTATION\n");
    printf("========================================\n");
    
    // Input: Number of processes and resources
    printf("\nEnter number of processes: ");
    scanf("%d", &n_processes);
    printf("Enter number of resource types: ");
    scanf("%d", &n_resources);
    
    // Input: Total available resources
    printf("\nEnter total instances available:\n");
    for(int j=0; j<n_resources; j++){
        printf("Resource %d: ", j+1);
        scanf("%d", &available[j]);
    }
    
    // Input: Process details
    printf("\nEnter process details:\n");
    for(int i=0; i<n_processes; i++){
        printf("\n--- Process P%d ---\n", i);
        process[i].done = false;
        
        for(int j=0; j<n_resources; j++){
            printf("Resource %d (Max Allocated Need): ", j+1);
            scanf("%d %d %d", &process[i].max[j], &process[i].allocated[j], &process[i].need[j]);
        }
        
        // Update available (subtract allocated)
        for(int j=0; j<n_resources; j++){
            available[j] -= process[i].allocated[j];
        }
    }
    
    // Main menu
    while(1){
        printf("\n========================================\n");
        printf("1. Check Initial Safety (Safety Algorithm)\n");
        printf("2. Process Request (Resource-Request Algorithm)\n");
        printf("3. Check for Deadlock (Detection Algorithm)\n");
        printf("4. Display Current State\n");
        printf("5. Exit\n");
        printf("========================================\n");
        printf("Choice: ");
        scanf("%d", &choice);
        
        switch(choice){
            case 1:{
                // Reset done flags
                for(int i=0; i<n_processes; i++){
                    process[i].done = false;
                }
                
                bool safe = safetyAlgorithm(n_resources, n_processes, true);
                
                if(safe){
                    printf("\n✓ System is in SAFE state\n");
                    printf("Safe sequence: < ");
                    for(int i=0; i<seq_count; i++){
                        printf("P%d", sequence[i]);
                        if(i < seq_count-1) printf(", ");
                    }
                    printf(" >\n");
                }
                else{
                    printf("\n✗ System is in UNSAFE state - DEADLOCK POSSIBLE\n");
                }
                break;
            }
            
            case 2:{
                int pid;
                printf("\nEnter process ID (0-%d): ", n_processes-1);
                scanf("%d", &pid);
                
                if(pid < 0 || pid >= n_processes){
                    printf("Invalid process ID!\n");
                    break;
                }
                
                int request[10];
                printf("Enter request (");
                for(int j=0; j<n_resources; j++){
                    printf("R%d ", j+1);
                }
                printf("): ");
                
                for(int j=0; j<n_resources; j++){
                    scanf("%d", &request[j]);
                }
                
                resourceRequest(pid, request, n_resources);
                break;
            }
            
            case 3:{
                // Reset done flags
                for(int i=0; i<n_processes; i++){
                    process[i].done = false;
                }
                
                detectionAlgorithm(n_resources, n_processes);
                break;
            }
            
            case 4:{
                displayState();
                break;
            }
            
            case 5:{
                printf("\nExiting...\n");
                return 0;
            }
            
            default:
                printf("Invalid choice!\n");
        }
    }
    
    return 0;
}
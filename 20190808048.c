/*
 * @student_id : 20190808048
 * @author muhammed fatih ozdil
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 50
#define MAX_BURSTS 50
#define IDLE_PROCESS_ID -1

typedef struct {
	int id;
	int num_bursts;
	int bursts[MAX_BURSTS][2];  // [0] is CPU burst, [1] is I/O burst
} Process;

// Reads processes from the given file and stores them in the processes array.
// Returns the number of processes read.
int read_processes(const char* filename, Process* processes) {
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	int num_processes = 0;
	while (fscanf(fp, "%d:", &processes[num_processes].id) == 1) {
		int i = 0;
		while (fscanf(fp, "(%d,%d);", &processes[num_processes].bursts[i][0],
					&processes[num_processes].bursts[i][1]) == 2) {
			i++;
		}
		processes[num_processes].num_bursts = i;
		num_processes++;
	}

	fclose(fp);
	return num_processes;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: fcfs_scheduling jobs_file\n");
		return EXIT_FAILURE;
	}

	Process processes[MAX_PROCESSES];
	int num_processes = read_processes(argv[1], processes);

	int total_turnaround_time = 0;
	int total_waiting_time = 0;
	int num_idle_executions = 0;

	int current_time = 0;
	int current_process = 0;
	while (current_process < num_processes) {
		Process p = processes[current_process];

		// If there is no current burst for the current process, move on to the next process.
		if (p.num_bursts == 0) {
			current_process++;
			continue;
		}

		// If there is an I/O burst, execute it immediately.
		if (p.bursts[0][1] > 0) {
			current_time += p.bursts[0][1];
			total_waiting_time += p.bursts[0][1];
			p.bursts[0][1] = -1;  // mark the I/O burst as completed
			p.bursts[0][0]--;  // decrement the remaining CPU burst time
			if (p.bursts[0][0] == 0) {
				p.num_bursts--;  // remove the completed burst
				memmove(p.bursts, p.bur
						sts[1], p.num_bursts * sizeof(p.bursts[0]));
			}
		} else {
			// If there is no I/O burst, execute the CPU burst or run the IDLE process.
			if (p.bursts[0][0] > 0) {
				current_time += p.bursts[0][0];
				total_turnaround_time += current_time;
				p.bursts[0][0] = 0; // mark the CPU burst as completed
				p.num_bursts--; // remove the completed burst
				memmove(p.bursts, p.bursts[1], p.num_bursts * sizeof(p.bursts[0]));
			} else {
				current_time++;
				num_idle_executions++;
			}
		}
	}
	printf("Average turnaround time: %.2f\n", (float) total_turnaround_time / num_processes);
	printf("Average waiting time: %.2f\n", (float) total_waiting_time / num_processes);
	printf("Number of times IDLE process executed: %d\n", num_idle_executions);
	printf("HALT\n");

	return EXIT_SUCCESS;
}

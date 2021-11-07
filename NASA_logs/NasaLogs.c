#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int size = 400;
long array_of_times[1891714];
int line_counter = 0;

int get_status_code(char str[]) {
    char code[4];
    int code_index = 0;
    unsigned int str_index = strlen(str) - 1;
    int space_counter = 0;
    while (!space_counter) {
        if (str[str_index] == ' ') {
            space_counter++;
        }
        str_index--;
    }
    while (space_counter == 1) {
        if (str[str_index] == ' ') {
            space_counter++;
        } else {
            code[code_index] = str[str_index];
            code_index++;
            str_index--;
        }
    }
    return atoi(code);
}

long local_time_to_time(char *str) {
    char time_parameters[6][6] = {""};
    sscanf(str, "%[^/]/%[^/]/%[^:]:%[^:]:%[^:]:%[^ ]", time_parameters[0], time_parameters[1], time_parameters[2],
           time_parameters[3], time_parameters[4], time_parameters[5]);
    struct tm Time;
    Time.tm_mday = atoi(time_parameters[0]),
    Time.tm_mon = atoi(time_parameters[1]),
    Time.tm_year = atoi(time_parameters[2]) - 1900,
    Time.tm_hour = atoi(time_parameters[3]),
    Time.tm_min = atoi(time_parameters[4]),
    Time.tm_sec = atoi(time_parameters[5]),
    Time.tm_isdst = 0;
    time_t sec_time = mktime(&Time);
    return sec_time;
}

int get_local_time(char str[]) {
    char waste[100];
    char time[30];
    sscanf(str, "%[^[][%[^]]", waste, time);
    return local_time_to_time(time);
}

void get_time_window(int time_of_window) {
    int window_index_low = 0;
    int window_index_high = 0;
    int max_window = 0;
    int max_window_index_low = 0;
    int max_window_index_high = 0;
    while (!(window_index_high == line_counter)) {
        if (array_of_times[window_index_high] - array_of_times[window_index_low] <= time_of_window) {
            window_index_high++;
        } else {
            if (window_index_high - window_index_low > max_window) {
                max_window = window_index_high - window_index_low;
                max_window_index_low = window_index_low + 1;
                max_window_index_high = window_index_high;
            }
            window_index_low++;
        }
    }
    printf("Number of server requests: %d\nFirst log's line: %d\nLast log's line: %d", max_window, max_window_index_low,
           max_window_index_high);
}

void main() {
    FILE *file = fopen("access_log_Jul95", "rb");
    int time_window;
    int error_counter = 0;
    char line[size];
    printf("Enter time window in seconds:");
    scanf("%d", &time_window);
    while (!feof(file)) {
        fgets(line, size, file);
        array_of_times[line_counter++] = get_local_time(line);
        if (get_status_code(line) % 10 == 5) {
            error_counter++;
            printf("%s", line);
            printf("____Successfully____\n");
        }

    }
    printf("<<< 5xx Errors: %d >>>\n", error_counter);
    get_time_window(time_window);
}

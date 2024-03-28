// importing necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
  // getting original matrix as input
  int rows;
  int cols;
  printf("Enter number of rows of original array\n");
  scanf("%d", &rows);
  printf("Enter number of columns of original array\n");
  scanf("%d", &cols);
  int **mat = (int **)malloc(rows * sizeof(int*));
  for(int i = 0; i < rows; i++) mat[i] = (int *)malloc(cols * sizeof(int));
  printf("Enter the matrix elements\n");
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      scanf("%d", &mat[i][j]);
    }
  }

  // getting window as input
  int w_rows;
  int w_cols;
  printf("Enter number of rows of window array\n");
  scanf("%d", &w_rows);
  printf("Enter number of columns of window array\n");
  scanf("%d", &w_cols);
  int **window = (int **)malloc(w_rows * sizeof(int*));
  for(int i = 0; i < w_rows; i++) 
    window[i] = (int *)malloc(w_cols * sizeof(int));
  printf("Enter the window matrix elements\n");
  for (int i = 0; i < w_rows; i++)
  {
    for (int j = 0; j < w_cols; j++)
    {
      scanf("%d", &window[i][j]);
    }
  }

  // calculating the time, taken by the operation.
  clock_t start_t, end_t;
  double total_t;
  start_t = clock();


  // here begins your code

      int num_of_processes = 8; 
    int result_rows = rows - w_rows + 1;
    int result_cols = cols - w_cols + 1;
    int row_per_proc = result_rows / num_of_processes;

    int **result = (int **)malloc(result_rows * sizeof(int*));
    for(int i = 0; i < result_rows; i++) result[i] = (int *)malloc(result_cols * sizeof(int));

    pid_t child_pid;

    for (int proc_id = 0; proc_id < num_of_processes; proc_id++) {

        child_pid = vfork();

        if (child_pid == -1) {
            perror("Error : fork FAILED");
            exit(EXIT_FAILURE);
        }
        if (child_pid == 0) { 
            int start_row = proc_id * row_per_proc;
            int end_row = 0;

            if(proc_id == num_of_processes - 1){
              end_row = result_rows;
            }else{
              end_row = start_row + row_per_proc;
            }

            for (int i = start_row; i < end_row; i++) 
            {
                for (int j = 0; j < result_cols; j++) 
                {
                    int sum = 0;
                    for (int k = 0; k < w_rows; k++) 
                    {
                        for (int l = 0; l < w_cols; l++) 
                        {
                            sum += mat[i+k][j+l] * window[k][l];
                        }
                    }
                    result[i][j] = sum;
                }
            }

            _exit(EXIT_SUCCESS); 
        } 
    }
  for (int i = 0; i < num_of_processes; i++) {
        waitpid(-1, NULL, 0);  
    }


  end_t = clock();
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  printf("Total time taken: %f\n", total_t);
  for (int i = 0; i < rows - w_rows + 1; i++)
  {
    for (int j = 0; j < cols - w_cols + 1; j++)
    {
      printf("%d ", result[i][j]);
    }
    printf("\n");
  }

  for (int i = 0; i < rows; i++)
    free(mat[i]);
  for (int i = 0; i < w_rows; i++)
    free(window[i]);
  for (int i = 0; i < (rows - w_rows + 1); i++)
    free(result[i]);
  free(mat);
  free(window);
  free(result);
}
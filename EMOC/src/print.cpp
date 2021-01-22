#include "print.h"

#include <cstdlib>
#include <cstdarg>
#include <cstdio>

// /* Output the objective values to file */
// void print_variable(char *file_name, SMRT_individual *pop_table)
// {
// 	int i, j;
// 	FILE *fpt;
// 	SMRT_individual *pop = NULL;
// 
// 	fpt = fopen(file_name, "w");
// 
// 	for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
// 	{
// 		pop = pop_table + i;
// 		for (j = 0; j < g_algorithm_entity.algorithm_para.variable_number; j++)
// 			fprintf(fpt, "%lf\t", pop->variable[j]);
// 		fprintf(fpt, "\n");
// 	}
// 	fclose(fpt);
// 
// 	return;
// }
// 
// /* Output the objective values to file */
// void print_objective(char *file_name, SMRT_individual * pop_table)
// {
// 	int i, j;
// 	FILE *fpt;
// 	SMRT_individual *pop = NULL;
// 
// 	fpt = fopen(file_name, "w");
// 
// 	int size = 0;
// 	if (weight_num != 0)
// 		size = weight_num;
// 	else
// 		size = g_algorithm_entity.algorithm_para.pop_size;
// 
// 	for (i = 0; i < size; i++)
// 	{
// 		pop = pop_table + i;
// 		for (j = 0; j < g_algorithm_entity.algorithm_para.objective_number; j++)
// 			fprintf(fpt, "%lf\t", pop->obj[j]);
// 		fprintf(fpt, "\n");
// 	}
// 
// 	fclose(fpt);
// 
// 	return;
// }

/* Output the weight vectors used in the decomposition-based methods to file */
/*
void print_weights (char *file_name)
{
	int i, j;
	FILE *fpt;

	fpt = fopen (file_name, "w");
	for (i = 0; i < number_weight; i++)
	{
		for (j = 0; j < number_objective; j++)
			fprintf (fpt, "%lf\t", lambda[i][j]);
		fprintf (fpt, "\n");
	}
	fclose (fpt);

	return;
}
*/
/* Print the current evolution progress */
// extern void print_progress()
// {
// 	printf("\r|\tThe %d run\t|\t%d%%\t|", g_algorithm_entity.run_index_current, g_algorithm_entity.algorithm_para.current_evaluation * 100 / g_algorithm_entity.algorithm_para.max_evaluation + 1);
// 
// 	fflush(stdout);
// 
// 	return;
// }

void print_error(int condition, int n, ...)
{
	int i;
	char * info = NULL;

	va_list vl;
	va_start(vl, n);
	if (condition)
	{
		printf("Error: ");
		for (i = 0; i < n; i++)
		{
			info = va_arg(vl, char*);
			printf("%s", info);
		}
		printf("\n");
		fflush(stdout);
		exit(-1);
	}
	va_end(vl);

	return;
}
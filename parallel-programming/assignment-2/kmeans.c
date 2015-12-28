#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sys/time.h>
#include <mpi.h>
#include <omp.h>

#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#define EXAMPLES 1000000
#define CLUSTERS 5
#define SINGLE_TASK_RUNS 5
// #define SRAND

// #define VISUALIZE
#define WINDOW_X 1024
#define WINDOW_Y 768

struct example 
{
	long x;
	long y;
	unsigned cluster;
};

struct result
{
	double cost;
	unsigned iterations;
	unsigned clusters[EXAMPLES];
};

struct example examples[EXAMPLES];
struct result *results;

void create_examples()
{
	unsigned i;
	struct example centroids[CLUSTERS];
	
	// pick random centroid points
	// no concurrency, issues with rand()
	for (i = 0; i < CLUSTERS; i++)
	{
		centroids[i].x = rand() % WINDOW_X;
		centroids[i].y = rand() % WINDOW_Y;
	}
	
	// create random points around centroids
	// no concurrency, issues with rand()
	for (i = 0; i < EXAMPLES; i++)
	{
		examples[i].x = centroids[i % CLUSTERS].x + ((float)rand() / RAND_MAX - 0.5) * ((float)WINDOW_X / 5);
		examples[i].y = centroids[i % CLUSTERS].y + ((float)rand() / RAND_MAX - 0.5) * ((float)WINDOW_Y / 5);
		examples[i].cluster = 0;
	}
}

void kmeans(struct result *result)
{
	struct example centroids[CLUSTERS];
	
	unsigned i, j, r, p;
	
	double distance;
	double closest_distance;
	unsigned closest_cluster;
	
	unsigned centroid_xs[CLUSTERS];
	unsigned centroid_ys[CLUSTERS];
	unsigned centroid_counts[CLUSTERS];
	
	double cost, last_cost;
	double square_distance_sum;
	
	// randomly initialize centroids
	r = rand() % EXAMPLES;
	
	#pragma omp parallel for
	for (i = 0; i < CLUSTERS; i++)
	{
		centroids[i].x = examples[(r + i) % EXAMPLES].x;
		centroids[i].y = examples[(r + i) % EXAMPLES].y;
		centroids[i].cluster = i + 1;
	}
	
	// iterate
	last_cost = INFINITY;
	cost = INFINITY;
	for (p = 0; p == 0 || last_cost - cost > 0.000001; p++)
	{
		// assign clusters
		
		#pragma omp parallel for private(closest_cluster, closest_distance, distance, j)
		for (i = 0; i < EXAMPLES; i++)
		{
			closest_cluster = 0;
			closest_distance = INFINITY;
			for (j = 0; j < CLUSTERS; j++)
			{
				if (centroids[j].cluster == 0)
					continue;
		
				distance = powl(centroids[j].x - examples[i].x, 2) + powl(centroids[j].y - examples[i].y, 2);
				if (distance < closest_distance)
				{
					closest_distance = distance;
					closest_cluster = centroids[j].cluster;
				}
			}
			examples[i].cluster = closest_cluster;
		}
	
		// move centroids
		
		memset(centroid_xs, 0, sizeof(centroid_xs));
		memset(centroid_ys, 0, sizeof(centroid_ys));
		memset(centroid_counts, 0, sizeof(centroid_counts));
	
		// no concurrency, += needs to be synchronized, pragma omp critical too heavy
		for (i = 0; i < EXAMPLES; i++)
		{
			centroid_xs[examples[i].cluster - 1] += examples[i].x;
			centroid_ys[examples[i].cluster - 1] += examples[i].y;
			centroid_counts[examples[i].cluster - 1] += 1;
		}
	
		#pragma omp parallel for
		for (i = 0; i < CLUSTERS; i++)
		{
			if (centroids[i].cluster == 0)
				continue;
			
			if (centroid_counts[i] > 0)
			{
				centroids[i].x = centroid_xs[i] / centroid_counts[i];
				centroids[i].y = centroid_ys[i] / centroid_counts[i];
				
				// printf("Iteration %d: Cluster %d moved (%d, %d)\n", p, i, centroids[i].x, centroids[i].y);
			}
			else
			{
				// printf("Iteration %d: Cluster %d removed\n", p, i);
				centroids[i].cluster = 0;
			}
		}
		
		// compute cost
		
		square_distance_sum = 0;
		
		// no concurrency, += needs to be synchronized, pragma omp critical too heavy
		for (i = 0; i < EXAMPLES; i++)
			square_distance_sum += powl(centroids[examples[i].cluster - 1].x - examples[i].x, 2) + powl(centroids[examples[i].cluster - 1].y - examples[i].y, 2);
		
		last_cost = cost;
		cost = square_distance_sum / EXAMPLES;
		// printf("Iteration %d: Cost %lf\n", p, cost);
	}
	
	// store result
	result->cost = last_cost;
	result->iterations = p;
	
	for (i = 0; i < EXAMPLES; i++)
		result->clusters[i] = examples[i].cluster;
}

void display()
{
	unsigned i;
	
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (i = 0; i < EXAMPLES; i++)
	{
		// cluster color
		if (examples[i].cluster == 0)
			glColor3f(1, 1, 1);
		else
			glColor3f(examples[i].cluster & 4, examples[i].cluster & 2, examples[i].cluster & 1);
	
		// draw a point
		glVertex2f((float)examples[i].x, (float)examples[i].y);
	}
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	int err, ntasks, id;
	struct timeval start, now;
	MPI_Status status;
	struct result result;
	MPI_Datatype result_type, examples_type;
	unsigned runs;
	
	unsigned i;
	unsigned best;
	double best_cost;
	
	// init MPI
	err = MPI_Init(&argc, &argv); 
	if (err != MPI_SUCCESS) {
		printf("MPI_init failed!\n");
		exit(1);
	}

	// number of tasks
	err = MPI_Comm_size(MPI_COMM_WORLD, &ntasks);	
	if (err != MPI_SUCCESS) {
		printf("MPI_Comm_size failed!\n");
		exit(1);
	}
	
	runs = (ntasks > 1 ? ntasks : SINGLE_TASK_RUNS);

	// process id 
	err = MPI_Comm_rank(MPI_COMM_WORLD, &id);  
	if (err != MPI_SUCCESS) {
		printf("MPI_Comm_rank failed!\n");
		exit(1);
	}
	
	// result type
	MPI_Type_contiguous(sizeof(struct result), MPI_CHAR, &result_type);
	MPI_Type_commit(&result_type);
	
	// examples type
	MPI_Type_contiguous(sizeof(examples), MPI_CHAR, &examples_type);
	MPI_Type_commit(&examples_type);

	if (id == 0) // master
	{
		results = malloc(runs * sizeof(struct result));
	
		// total time
		gettimeofday(&start, NULL);
	
		// initialize examples
#ifdef SRAND
		srand(start.tv_sec);
#endif
		create_examples();
		
		// broadcast examples unless single task
		if (ntasks > 1)
		{
			err = MPI_Bcast(examples, 1, examples_type, 0, MPI_COMM_WORLD);
			if (err != MPI_SUCCESS) {
				printf("Process %i: Error in MPI_Bcast!\n", id);
				exit(1);
			}
		}
		
		// receive results
		best = 0;
		best_cost = INFINITY;
		for (i = 1; i < runs; i++)
		{
			// receive or run kmeans when single task
			if (ntasks > 1)
			{
				err = MPI_Recv(&results[i], 1, result_type, i, 0, MPI_COMM_WORLD, &status);
				if (err != MPI_SUCCESS) {
					printf("Process %i: Error in MPI_Recv!\n", id);
					exit(1);
				}
			}
			else
				kmeans(&results[i]);
			
			printf("Task %d: cost %lf iterations %d\n", i, results[i].cost, results[i].iterations);	
		
			if (results[i].cost < best_cost)
			{
				best = i;
				best_cost = results[i].cost;
			}
		}
	
		printf("Best run is %d\n", best);
	
		gettimeofday(&now, NULL);
		printf("Total time %.2lf secs, %d MPI task(s), %d OpenMP task(s)\n", (now.tv_sec + (double)now.tv_usec / 1000000) 
			- (start.tv_sec + (double)start.tv_usec / 1000000), ntasks, omp_get_max_threads());
	
#ifdef VISUALIZE
		// render best result
		
		for (i = 0; i < EXAMPLES; i++)
			examples[i].cluster = results[best].clusters[i];
		
		glutInit(&argc, argv);
		glutInitWindowSize(WINDOW_X, WINDOW_Y);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutCreateWindow("K-Means");
	
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, WINDOW_X, WINDOW_Y, 0, 0, 1);
		glMatrixMode(GL_MODELVIEW);
	
		glutDisplayFunc(display);
		glutMainLoop();
#endif
	}
	else
	{
		// receive examples
		err = MPI_Bcast(examples, 1, examples_type, 0, MPI_COMM_WORLD);
		if (err != MPI_SUCCESS) {
			printf("Process %i: Error in MPI_Bcast!\n", id);
			exit(1);
		}
		
		// run k-means
#ifdef SRAND
		srand(start.tv_sec + id * 100000);
#endif
		kmeans(&result);
		
		// send result
		err = MPI_Send(&result, 1, result_type, 0, 0, MPI_COMM_WORLD);
		if (err != MPI_SUCCESS) {
			printf("Process %i: Error in MPI_Send!\n", id);
			exit(1);
		}
	}
	
	err = MPI_Finalize();	         /* Terminate MPI */
	if (err != MPI_SUCCESS) {
		printf("Error in MPI_Finalize!\n");
		exit(1);
	}
	
	return EXIT_SUCCESS;
}

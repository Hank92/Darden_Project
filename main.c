/************************************************
 *												*
 *   PROGRAM: main.c							*
 *												*
 *   PURPOSE: Differentiation Model				*
 *												*
 *   VERSION: June 1st, 2005					*
 *												*
 ************************************************/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#define randfloat() ((float)rand() / (float)RAND_MAX)

				/* GLOBAL VARIABLES */

float pi = 3.141592654;
int firms=20;				/* number of potential firms */
int players=20;				/* number of firms in industry */
int N=20;					/* number of tasks */
int K=20;					/* number of interactions */
float rule_pram;			/* rule parameter = percent firms innovating */
float a[2000000][2000000];		/* vector of marginal cost coefficients */
float alpha=1.0;			/* demand parameter */
float beta=1.0;				/* demand parameter */
int b[21][21];				/* vector of substring members */
float fixed_cost=0.0;		/* fixed cost parameter */
float output;				/* total industry output */
float mutation=0.10;		/* likelihood of mutation */
float enter_rule=5;			/* likelihood of entry parameter */
float exit_rule=0.00;		/* threshold for exit */
int iterate=100;			/* number of system iterations */
int leader=1;				/* designates the market leader */
float total_cost = 0.0;		/* sum of marginal costs across all entrants */
float total_profit = 0.01;	/* sum of earnings across all entrants */
float global=0;				/* lowest marginal cost possible */

typedef struct {
     int task[100];
     float price;
     float quantity;
     float mcost;
     float cost;
     float profit;
     float rule;
     int entrant;
} orgs;

orgs firm[100];



				/* FUNCTIONS */


int poww(int x, int y)							/* returns the value of x^y */
{	int h;
	int temp = 1;
	for (h = 1; h <= y; h++)
		{temp = temp * x;
		};
	return(temp);}


float cost(int task[100])						/* calculate firm costs */
{	int i,j,value;
	float running_cost = 0.0;
	for (i = 1; i <= N; i++)
	   	{value = 1 + task[i];
		 for (j = 1; j <= (K-1); j++)
			{value = value + (pow(2,j) * task[b[i][j]]);
			 };
 	     running_cost = running_cost + (a[i][value]);
	  	};
	running_cost = running_cost/N;
	return(running_cost);}


void profits()													/* calculate firm profits */
{	int i,j;
	float output = 0.0;
	total_cost = 0.0;
	total_profit = 0.0;
  	for(i = 1; i <= firms; i++)
  	  {if (firm[i].entrant==1)
         {firm[i].mcost = cost(firm[i].task);
          firm[i].mcost = (-1*((global/firm[i].mcost)-1))+0.1;  /*standardized to global*/
          total_cost = total_cost + firm[i].mcost;
		  };
		};
  	for(i = 1; i <= firms; i++)
  	  {if (firm[i].entrant==1)
         {firm[i].quantity = ((alpha + total_cost)/(players+1))-firm[i].mcost;
          if (firm[i].quantity<=0.0) firm[i].quantity=0.0;
          firm[i].cost = firm[i].mcost*firm[i].quantity + fixed_cost;
		  output = output + firm[i].quantity;
		  };
		};
    for(j = 1; j <= firms; j++)
  	  {if (firm[j].entrant==1)
       	 {firm[j].price = alpha - (beta*output);
		  firm[j].profit = (firm[j].price * firm[j].quantity) - firm[j].cost;
		  total_profit = total_profit + firm[j].profit;
		  };
		};
	total_profit = total_profit/(players+1);
}


/*
void parameterize()
{	printf("LANDSCAPE COMPETITION MODEL (Marginal Cost Version)\n");printf("\n");
	printf("Enter the number of firms (<100): ");
 	scanf("%d",&firms);
 	printf("Enter the number of tasks (<100): ");
 	scanf("%d",&N);
 	printf("Enter the number of interactions (<=%d): ",N);
 	scanf("%d",&K);
 	if (K>N) K=N;
 	printf("\n");
}
*/

void parameterize()
{	firms = 20; /*(int) (1+(10*randfloat()));*/
	mutation = randfloat();
	N = 20; /*(int) (2+((max-1)*randfloat()));*/
	K = (int) (1+(N*randfloat()));
	rule_pram = randfloat();
 	if (K>N) K=N;
}


void seed_landscape()
{	int i,j,h,temp,stack[100];
	int combos = pow(2,K);

  	for(i = 1; i <= N; i++)
         {for(j = 1; j <= combos; j++)
          	 {a[i][j] = randfloat();
          	 };
	  	  for(j = 1; j < i; j++)
          	 {stack[j] = j;
          	 };
	  	  for(j = (i+1); j <= N; j++)
          	 {stack[(j-1)] = j;
          	 };
          for(j = 1; j <= K; j++)
          	 {temp = (1 + ((int) ((N-j)*randfloat())));
          	  b[i][j] = stack[temp];
		  	  for (h = temp;  h <= (N-j-1); h++)
			  	{stack[h] = stack[h+1];
         	 	};
         	 };
          };
}

/* give each firm a different initial task set*/
void initialize_firms(int runner)
{	int i,j; float tmp;
	for(i = 1; i <= firms; i++)
         {firm[i].entrant = 0;
          firm[i].profit = 0;
          tmp = randfloat();
          if (tmp > rule_pram)
         	 	 firm[i].rule = 1;
              		else
         	  	 firm[i].rule = 0;
          for(j = 1; j <= N; j++)
         	{firm[i].task[j] = (int) (2*randfloat());};
         };
}


/* give each firm identical initial task set
void initialize_firms()
{	int i,j;
	firm[1].rule = randfloat();
        for(j = 1; j <= N; j++)
         {firm[1].task[j] = (int) (2*randfloat());};
	for(i = 2; i <= firms; i++)
         {firm[i].entrant = 0;
          firm[i].profit = 0;
          firm[i].rule = randfloat();
          for(j = 1; j <= N; j++)
         	{firm[i].task[j] = firm[1].task[j];};
         };
}
*/

void find_global()
{	int i,j,bins,stopper;
	int position[100];
	/*bins = poww(2,N);*/
  bins = pow(2, N);
    for(i = 1; i <= N; i++)
       {position[i] = 0;};
    global = cost(position);
	for(i = 1; i <= bins; i++)
		{j = 1; stopper = 0;
       	 while (stopper == 0)
 	     	{if (position[j] == 0)
         	 	{position[j] = 1;
         	  	 stopper = 1;}
              		else
         	  	 position[j] = 0;
 	      	 j++;
 	      	 if (j > N) stopper=1;
   	      	};
   	   	 if (cost(position)<global) global=cost(position);
        };
}


void determine_entrants()					/* determine if an entrant to industry */
{	int i,j;
	int change = 1;
	int converge = 0;
	int worst = 1;
	players = 0;
	if (total_profit==0) total_profit=0.01;
	for(i = 1; i <= firms; i++)
         {if ((firm[i].entrant==0) && (randfloat()<(enter_rule*total_profit))) firm[i].entrant=1;
          if (firm[i].entrant==1) players=players+1;
         };
	while (converge<1)
 	{	profits();
 	 	worst = 0;
 	 	firm[0].mcost = 0;
      	change = 0;
		for (i = 1; i <= firms; i++)
   			{if ((firm[i].mcost>firm[worst].mcost)&&(firm[i].entrant==1)) worst=i;
  			};
        if ((worst!=0) && (firm[worst].profit<=exit_rule))
        	{change = 1;
			 firm[worst].entrant=0;
			 players=players-1;
        	 firm[worst].quantity=0;
        	 firm[worst].profit=0;
        	 firm[worst].price=0;
         	 for(j = 1; j <= N; j++) {firm[worst].task[j] = (int) (2*randfloat());};
         	};
        if (change == 0) converge=1;
    };
    profits();
}


void update()												/* update tasks according to algorithm */
{	int i,j,j2,new_task[100],rule;
	float new_quantity,new_price,new_cost,new_profits;
  	for(i = 1; i <= firms; i++)
  	  {if (firm[i].entrant==1)
         {for(j = 1; j <= N; j++)
         	{new_task[j] = firm[i].task[j];};
          for(j = 1; j <= N; j++)
         	{if (randfloat() < mutation)
         	 	{new_task[j] = abs(new_task[j]-1);
         	 	 new_cost = cost(new_task);
         	 	 new_cost = (-1*((global/new_cost)-1))+0.1;  /*standardized to global*/
         	 	 new_quantity = ((alpha + total_cost + (new_cost-firm[i].mcost))/(players+1))-new_cost;
         	 	 if (new_quantity<0.0) new_quantity=0.0;
         	 	 new_price = firm[i].price - (new_quantity-firm[i].quantity);
          	 	 new_profits = (new_price*new_quantity) - (new_cost*new_quantity) - fixed_cost;

			 if (randfloat() < firm[i].rule) 					/* assign rule */
         	 	 	rule = 1;
         	 	 else
         	 	 	rule = 0;

         	 	 if (rule == 0)									/* rule based on profitability */
        	 	 {if (new_profits > firm[i].profit)
         	 	 	firm[i].task[j] = new_task[j];
         	 	  else
         	 	 	new_task[j] = firm[i].task[j];};

         	 	 if (rule == 1)									/* rule based on best practice */
 			 	 {for(j2 = 1; j2 <= firms; j2++)
  	  				{if ((firm[j2].profit * firm[j2].entrant) > (firm[leader].entrant * firm[leader].profit)) leader=j2;
				    };
 			 	  if (new_task[j] == firm[leader].task[j])
         	 	 	firm[i].task[j] = new_task[j];
         	 	  else
         	 	 	new_task[j] = firm[i].task[j];};

         	 	 if (rule == 2)									/* rule based on productivity */
 				 {if ((new_cost/new_quantity) < (firm[i].mcost/firm[i].quantity))
         	 	 	firm[i].task[j] = new_task[j];
         	 	  else
         	 	 	new_task[j] = firm[i].task[j];};

         	 	 if (rule == 3)									/* rule based on marginal cost */
 			 	 {if (new_cost < firm[i].mcost)
         	 	 	firm[i].task[j] = new_task[j];
         	 	  else
         	 	 	new_task[j] = firm[i].task[j];};


        	 	};
         	};
         };
        };
}


void print_results()
{	int i,j;
    printf("firm	entrant	[tasks]   		price	output	mcosts	profits	rule\n");
	for(i = 1; i <= firms; i++)
		 {printf("%d	%d	[",i,firm[i].entrant);
		  for(j = 1; j <= N; j++)
		  	{printf("%d",firm[i].task[j]);};
		  printf("]	%4.3f	%4.3f	%4.3f	%4.3f	%3.2f\n",firm[i].price,firm[i].quantity,firm[i].mcost,firm[i].profit,firm[i].rule);};}




int main()
{
int timer=1;
int batch=1;
int runs=1;
int i,j;
FILE *stream;

srand(121);

/* Rule = random mix */

stream = fopen("output.txt","w");
fprintf(stream,"run	time	firm	entrant	price	output	mcosts	profits	rule	N	K	firms	theta	global	rule_pram\n");

while (runs <= 3)
{
	parameterize();								/* gather parameter values */
	seed_landscape();							/* initialize landscape */
	find_global();								/* find global optima */
	initialize_firms(runs);				/* assign tasks and rules to firms */
	total_profit = 0.01;
  printf("%d\n", runs);
	timer = 1;
	while (timer <= iterate)
 	   {determine_entrants();					/* determine players and profits */
		for(i = 1; i <= firms; i++)
			 {fprintf(stream,"%d	%d	%d	%d	%4.3f	%4.3f	%4.3f	%4.3f	%3.2f	%d	%d	%d	%3.2f	%4.3f	%4.3f\n",runs,timer,i,firm[i].entrant,firm[i].price,firm[i].quantity,firm[i].mcost,firm[i].profit,firm[i].rule,N,K,firms,mutation,global,rule_pram);};
		update();								/* update players positions */
		timer++;
   	   };
    runs++;
};

fclose(stream);
printf("\n01 Done!\n");

}

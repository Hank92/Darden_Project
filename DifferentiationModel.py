from random import random
from numpy import *

def randNumber():
  r = random.random()
  return r


pi = 3.141592654;
max=20;
firms=20;				                     # number of potential firms
players=20;			                     # number of firms in industry
N=20;					                       # number of tasks
K=20;					                       # number of interactions
rule_pram = randNumber()			         # rule parameter = percent firms innovating
a= zeros((2000000, 2000000), dtype=float)	 # vector of marginal cost coefficients
alpha=1.0;			                     # demand parameter
beta=1.0;				       # demand parameter
b= zeros((21, 21), dtype=int)				     # vector of substring members
fixed_cost=0.0;	       # fixed cost parameter
output =0;				         # total industry output
mutation=0.10;		     # likelihood of mutation
enter_rule=5;			     # likelihood of entry parameter
exit_rule=0.00;		     # threshold for exit
iterate=100;		       # number of system iterations
leader=1;				       # designates the market leader
total_cost = 0.0;		   # sum of marginal costs across all entrants
total_profit = 0.01;	 # sum of earnings across all entrants
lowest_margin=0;				       # lowest marginal cost possible

class orgs:
	task = zeros(100, dtype=int)
	price = 0.0
	quantity = 0.0
	mcost = 0.0
	cost = 0.0
	profit = 0.0
	rule = 0.0
	entrant = 0

firm = [orgs() for x in range(100)]

def cost(task):
	running_cost = 0.0
	for i in range(1,N+1):
		value = 1 + task[i]
		for j in range(1, K):
			value = value + (2**j) * task[b[i][j]]
		running_cost = running_cost + a[i][value]
	running_cost = running_cost/N;
	return running_cost

# calculate firm profits
def profits():
	output = 0.0
	total_cost = 0.0
	total_profit = 0.0
	for i in range(1, firms+1):
		if firm[i].entrant==1:
			firm[i].mcost = cost(firm[i].task)
			firm[i].mcost = (-1*((lowest_margin/firm[i].mcost)-1))+0.1;  #standardized to lowest_margin
			total_cost = total_cost + firm[i].mcost
	for i in range(1, firms+1):
		if firm[i].entrant==1:
			firm[i].quantity = ((alpha + total_cost)/(players+1))-firm[i].mcost
		if firm[i].quantity <= 0.0:
			firm[i].quantity = 0.0
			firm[i].cost = firm[i].mcost*firm[i].quantity + fixed_cost
			output = output + firm[i].quantity
	for i in range(1, firms+1):
		if firm[i].entrant==1:
			 firm[i].price = alpha - (beta*output)
			 firm[i].profit = (firm[i].price * firm[i].quantity) - firm[i].cost
			 total_profit = total_profit + firm[i].profit
	total_profit = total_profit/(players+1)

def parameterize():
	firms = 20; # (int) (1+(10*randNumber()))
	mutation = randNumber();
	N = 20; # (int) (2+((max-1)*randNumber()))
	K = (int) (1+(N*randNumber()));
	rule_pram = randNumber();
	if K > N:
		K = N

def seed_landscape():
	stack =[0 for x in range(100)];
	combos = 2**K;

	for i in range(1, N+1):
		for j in range(1, combos+1):
			a[i][j] = randNumber();
		for j in range(1, i):
			stack[j] = j
		for j in range(i+1, N+1):
			stack[j-1] = j
		for j in range(1, K+1):
			temp = (1 + int((N-j)*randNumber()))
			b[i][j] = stack[temp]
			for h in range(temp, N-j):
				stack[h] = stack[h+1]

# give each firm a different initial task set

'''
Question:
What is runner?
'''

def initialize_firms(runner): 
	for i in range(1, firms):
		firm[i].entrant = 0
		firm[i].profit = 0
		tmp = randNumber()
		if tmp > rule_pram:
			firm[i].rule = 1
		else:
			firm[i].rule = 0;
		for j in range(1, N+1):
			firm[i].task[j] = int(2*randNumber())

def find_margin():
	position = zeros(100, dtype=int)
	bins = 2**N
	lowest_margin = cost(position)
	for i in range(1, bins+1):
		j = 1
		stopper = 0
		while stopper == 0:
			if position[j] == 0:
				position[j] = 1
				stopper = 1
			else: 
				position[j] = 0
			j+=1
			if j > N:
				stopper = 1
		if cost(position) < lowest_margin:
			lowest_margin = cost(position)


def find_lowest_marginal_cost():
	position = [0 for x in range(100)]
	bins = 2**N
	for i in range(1, N+1):
		position[i] = 0

	lowest_margin = cost(position);
	for i in range(1, bins+1):
		j = 1
		stopper = 0
		while stopper == 0:
			if position[j] == 0:
				position[j] = 1
				stopper = 1
			else:
				position[j] = 0
			j += 1
			if (j > N):
				stopper = 1
		if cost(position) < lowest_margin:
			lowest_margin = cost(position)

# determine if an entrant to industry
def determine_entrants():
	change = 1
	converge = 0
	worst = 1
	players = 0
	#if total_profit==0:
	#	total_profit=0.01
	for i in range(1, firms+1):
		if firm[i].entrant == 0 and randNumber() < enter_rule*total_profit:
			firm[i].entrant=1
		if firm[i].entrant == 1:
			players = players+1
	while converge < 1:
		profits()
		worst = 0
		firm[0].mcost = 0
		change = 0
		for i in range(1, firms+1):
			if firm[i].mcost > firm[worst].mcost and firm[i].entrant==1:
				worst = i
		if worst != 0 and firm[worst].profit <= exit_rule:
			change = 1
			firm[worst].entrant=0
			players=players-1
			firm[worst].quantity=0
			firm[worst].profit=0
			firm[worst].price=0
			for j in range(1, N+1):
				firm[worst].task[j] = int(2*randNumber())
		if change == 0:
			converge =1
	profits()

#update tasks according to algorithm
def update():
	new_task = [0 for x in range(100)]
	for i in range(1, firms+1):
		if firm[i].entrant ==1:
			for j in range(1, N):
				new_task[j] = firm[i].task[j]
			for j in range(1, N):
				if randNumber() < mutation:
					new_task[j] = abs(new_task[j]-1)
					new_cost = cost(new_task)
					new_cost = (-1*((lowest_margin/new_cost)-1))+0.1;  #standardized to lowest_margin
					new_quantity = ((alpha + total_cost + (new_cost-firm[i].mcost))/(players+1))-new_cost
					if new_quantity<0.0:
						new_quantity=0.0
					new_price = firm[i].price - (new_quantity-firm[i].quantity)
					new_profits = (new_price*new_quantity) - (new_cost*new_quantity) - fixed_cost

					if randNumber() < firm[i].rule:					# assign rule
					   rule = 1;
					else:
					   rule = 0;

					if rule == 0:								    # rule based on profitability
					   if new_profits > firm[i].profit:
						   firm[i].task[j] = new_task[j]
					   else:
						   new_task[j] = firm[i].task[j]
					if rule == 1:                                   # rule based on best practice
						for j2 in range(1,firms+1):
							if firm[j2].profit * firm[j2].entrant > firm[leader].entrant * firm[leader].profit:
								leader = j2
						if new_task[j] == firm[leader].task[j]:
							firm[i].task[j] = new_task[j]
						else:
							new_task[j] = firm[i].task[j]
					if rule == 2:                                   # rule based on productivity
						if new_cost/new_quantity < firm[i].mcost/firm[i].quantity:
							firm[i].task[j] = new_task[j]
						else:
							new_task[j] = firm[i].task[j]
					if rule == 3:                                   # rule based on marginal cost
						if new_cost < firm[i].mcost:
							firm[i].task[j] = new_task[j]
						else:
							new_task[j] = firm[i].task[j]


runs = 0
timer=1;
batch=1;

stream = open("output.txt","w+")
stream.write("run	time	firm	entrant	price	output	mcosts	profits	rule	N	K	firms	theta	global	rule_pram\r\n")


while runs < 2:
	parameterize()								# gather parameter values
	seed_landscape()							# initialize landscape
	find_lowest_marginal_cost()								# find global optima
	initialize_firms(runs);				        # assign tasks and rules to firms
	print(runs)
	#total_profit = 0.01
	timer = 1
	while timer <= iterate:
		determine_entrants()
		for i in range(1, firms+1):
			temp = str(runs)+ " " +str(timer)+ " " +str(i)+ " " +str(firm[i].entrant)+ " " +str(round(firm[i].price),3)+ " " +str(round(firm[i].quantity),3)+ " " +str(round(firm[i].mcost),3)+ " " +str(round(firm[i].profit),3)+ " " +str(round(firm[i].rule),2)+ " " +str(N)+ " " +str(K)+ " " +str(firms)+ " " +str(round(mutation),2)+ " " +str(round(lowest_margin),3)+ " " +str(round(rule_pram),3)
			stream.write(temp)
		update()
		timer+=1
	runs+=1


stream.close()
print("Done!")

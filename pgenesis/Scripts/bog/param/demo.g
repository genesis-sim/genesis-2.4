int n_nodes = 4, individuals = 1000, population = 10	
float param_a, param_b, bit_mutation_prob = 0.02, min_fitness, max_fitness
int actual_population = 0, least_fit, most_fit, i, free_index = 0, bs_a, bs_b
randseed 12345		

function init_farm
  create neutral /farm; disable /farm
  create neutral /farm/free; addfield /farm/free value
  createmap /farm/free / {n_nodes-1} 1	
  for (i=0; i<{n_nodes-1}; i=i+1); setfield /free[{i}] value {i+1};  end
  free_index = n_nodes - 2;
end

function delegate_task
  while (1)
    if (free_index >= 0)
      async worker@0.{getfield /free[{free_index}] value} {bs_a} {bs_b}
      free_index = free_index - 1;
      return
    else; clearthreads; end
  end
end
	
function finish
  while (free_index < n_nodes - 2); clearthreads; end
end

function evaluate (a, b)
    float a, b, match, fit
    match = {min {{abs {a-1}} + {abs {b-1}}} {{abs {a+1}} + {abs {b+1}}}}
    if (match != 0.0); fit = 1.0/{sqrt {match}} 
    else; fit = 1e+9; end
    return {fit}
end

function recompute_fitness_extremes
  min_fitness = 1e+10; max_fitness = -1e+10
  for (i = 0; i < actual_population; i = i + 1)
    if ({getfield /population[{i}] fitness} < min_fitness)
      least_fit = i; min_fitness = {getfield /population[{i}] fitness}
    end
    if ({getfield /population[{i}] fitness} > max_fitness)
      most_fit = i; max_fitness = {getfield /population[{i}] fitness}
    end
  end
end

function worker (bs_a, bs_b, bs_c, bs_d)
  int bs_a, bs_b
  float a, b
  float fit
  
  a = (bs_a - 32768.0) / 1000.0; b = (bs_b - 32768.0) / 1000.0;
  fit = {evaluate {a} {b}}
  return_result@0.0 {mytotalnode} {bs_a} {bs_b} {bs_c} {bs_d} {fit}
end

function return_result (node, bs_a, bs_b, bs_c, bs_d, fit)
  int node, bs_a, bs_b
  float fit
  if (actual_population < population)
    least_fit = actual_population; min_fitness = -1e+10;
    actual_population = actual_population + 1
  end
  if (fit > min_fitness)
    setfield /population[{least_fit}] fitness {fit}
    setfield /population[{least_fit}] a_value {bs_a}
    setfield /population[{least_fit}] b_value {bs_b}
    if (actual_population == population); recompute_fitness_extremes; end
  end

  echo " " {fit}
  free_index = free_index + 1
  setfield /free[{free_index}] value {node}
end

function mutate (v)
    int v, i, b = 1
    for (i = 0; i < 16; i = i + 1)
	if ({rand 0 1} < bit_mutation_prob); v = v ^ b; end
	b = b + b
    end
    return {v}
end

function init_search
  create neutral /gs; disable /gs
  create neutral /gs/population
  addfield /gs/population a_value; addfield /gs/population b_value
  addfield /gs/population fitness
  createmap /gs/population / {population} 1
end

function init_individual
    bs_a = {rand 0 65536}; bs_b = {rand 0 65536}
end

function mutate_individual (chosen)
    int	chosen
    bs_a = {mutate {getfield /population[{chosen}] a_value}}
    bs_b = {mutate {getfield /population[{chosen}] b_value}}
end

function print_best
    float a, b
    a = ({getfield /population[{most_fit}] a_value} - 32768.0) / 1000.0;
    b = ({getfield /population[{most_fit}] b_value} - 32768.0) / 1000.0;
    echo "Best match with a = " {a} ", b = " {b} ", fitness = " {max_fitness}
end

function search
    int   i
    init_search; init_farm
    for (i = 0; i < individuals; i = i + 1)
	if (i < population); init_individual
	else; mutate_individual {rand 0 actual_population}; end
	delegate_task {i} {bs_a} {bs_b}
    end 
    finish; echo; echo "Finished search at " {getdate}; print_best
end

paron -farm -silent 0 -nodes {n_nodes} -output o.out -executable nxpgenesis
echo I am node {mytotalnode}
barrierall
echo Completed startup at {getdate}
if ({mytotalnode} == 0)
	search 
end
barrierall 7 1000000
paroff
quit


// genesis

// Some utility functions for looking at synapse parameters


function showrates(start, stop)
  int start, stop
  int i
  for(i = start; i < stop; i = i + 1)
    showfield /retina/recplane/rec[{i}]/input rate
  end
end


function showsynfield(path,field)
  str path, field
  str name
  int num
  int i

  foreach name ({getelementlist {path}})
    num = {getfield {name} nsynapses}
    for (i = 0; i < num; i = i + 1)
      showfield {name} synapse[{i}].{field}
    end
  end

end

function getmaxsynfield(path,field)
  str path, field
  str name
  int num
  int i
  float max = -9999.9
  float temp

  foreach name ({getelementlist {path}})
    num = {getfield {name} nsynapses}
    for (i = 0; i < num; i = i + 1)
      temp = {getfield {name} synapse[{i}].{field}}
      if (temp > max)
        max = temp
      end
    end
  end

  return max 
end


function showmaxsynfield(path,field)
  str path, field
  float max

  max = {getmaxsynfield {path} {field}}
  echo Maximum {field} in {path} = {max}
end



function synapse_info(path)
  str path, src
  int i
  float weight, delay
  floatformat %.3g
  for(i = 0; i < {getsyncount {path}}; i = i + 1)
    src    = {getsynsrc {path} {i}}
    weight = {getfield {path} synapse[{i}].weight}
    delay  = {getfield {path} synapse[{i}].delay}
    echo synapse[{i}]: \
        src = {src} weight = {weight} delay = {delay}
  end
end

str name = "new"

function synapse_list
  int i
  for(i = 0; i < 100; i = i + 1)
    echo connections from: /retina/recplane/rec[{i}]/input >> synapse.list.{name}
    showmsg /retina/recplane/rec[{i}]/input | grep SPIKE >> synapse.list.{name}
    echo >> synapse.list.{name}
    echo . -n
  end
  echo DONE!
end


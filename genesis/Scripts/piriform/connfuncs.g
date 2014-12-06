//genesis
function propagation_velocity(path, velocity)
str path
float velocity
     radialdelay {path} {1.0/velocity}
end

function gausspropagation_velocity(path, min, max, mean, sd)
str path
float min, max
float mean, sd
     radialdelay {path} -gaussian {1.0/mean} {1.0/sd} {1.0/max}  \
        {1.0/min}
end

function rangepropagation_velocity(path, min_velocity, max_velocity)
str path
float min_velocity
float max_velocity
     radialdelay {path} -uniform {1.0/max_velocity}  \
        {1.0/min_velocity}
end

function exponential_weight(path, maxweight, lambda, minval)
str path
float maxweight
float lambda
float minval
     expweight {path} {-1.0/lambda} {maxweight} {minval}
end

function aff_weight(path, angle, max, mlambda, clambda, min)
str path
float angle
float maxweight
float mlambda
float clambda
float min
     affweight {path} {angle} {max} {-1.0/mlambda} {-1.0/clambda} \
         {min}
end


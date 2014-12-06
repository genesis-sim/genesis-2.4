//genesis - methods.g - sets integration methods for Cable tutorial

function make_hsolve// re-creates the hsolve element if it is needed

    if (({IntegMethod}) < 10)
	return
    end
    if ({exists /cell/solve})
	delete /cell/solve
    end
    create hsolve /cell/solve
    setfield /cell/solve path "/cell/##[][TYPE=compartment]"
    setfield /cell/solve chanmode 0
    setmethod /cell/hsolve {IntegMethod}
    call /cell/solve SETUP
    reset
    echo "Using hsolve"
end

function set_method(method)// sets integration method for all compartments

    int method
    // save the current method in a global variable
    IntegMethod = {method}
    if ((method) > 9)
        make_hsolve
    else
      if ({exists /cell/solve})      // If previous method was implicit

	 delete /cell/solve
      end
      setmethod /cell/##[TYPE=compartment] {method}
      reset
    end
    echo "Integration method set to method "{method}
end

exec('../examples/common/Scilab/functions.sce', 0);

function [] = plotPacketSinks(packetSinkNames, savePlot)
	if ~exists("savePlot", "local")  then
		savePlot = %T	   
    end
    
	plotPacketSinksThroughput(packetSinkNames, savePlot);
	plotPacketSinksLatency(packetSinkNames, savePlot);
endfunction

function [] = plotPacketSinksThroughput(packetSinkNames, savePlot)
	[rows, cols] = size(packetSinkNames);	
	
	t = [];
    values = [];
    plot_legends = [];
    for i = 1:cols
    	modelName = packetSinkNames(i);
    	
    	[this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s", modelName, "ReceivingApplication", "bits", "sum"));
    	mean_value = mean_value / (8 * M * samplingPeriod) // in MB/s
    	
    	// add this values to arrays. In case some model has more measurements, needs to resize
	    [this_t, t] = array_make_same_size(this_t, t);
	    [this_value, values] = array_make_same_size(this_value, values);
	    t(:, i) = this_t;
	    values(:, i) = this_value;
		
		plot_legends(:, i) = sprintf("%s (avg:%f MB/s)", modelName, mean_value);		
    end
	
	// plot
	scf();
	plot(t, values / (8 * k) , ".-");
	xtitle("Arrived data");
	xlabel('Time (s)'); ylabel("Arrived KBytes/" + string(samplingPeriod) + "s");
	legend(plot_legends);
	
	if (~exists("savePlot", "local") | savePlot==%T) then
	   saveCurrentPlot()	   
	end
endfunction

function [] = plotPacketSinksLatency(packetSinkNames, savePlot)
	[rows, cols] = size(packetSinkNames)
	
	t = [];
    values = [];
    plot_legends = [];
    for i = 1:cols
    	modelName = packetSinkNames(i);
    	
    	[this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s", modelName, "ReceivingApplication", "residence_time", "avg"));
    	mean_value = mean_value * 1000 * 1000 // in us
    	this_value = this_value * 1000 * 1000 // is us
    	
    	// add this values to arrays. In case some model has more measurements, needs to resize
	    [this_t, t] = array_make_same_size(this_t, t);
	    [this_value, values] = array_make_same_size(this_value, values);
	    t(:, i) = this_t;
	    values(:, i) = this_value;
		
		plot_legends(:, i) = sprintf("%s (avg:%f us)", modelName, mean_value);		
    end
	
	// plot
	scf();
	plot(t, values, ".-");
	xtitle("Mean network packet latency");
	xlabel('Time (s)'); ylabel("mean network latency (us)");
	legend(plot_legends);
	
	if (~exists("savePlot", "local") | savePlot==%T) then
	   saveCurrentPlot()	   
	end
endfunction

function [] = plotSwitchQueues(switchName, savePlot)
	model = eval(switchName);
	
	// read all links (always with name 'egressPort')
    t = [];
    values = [];
    plot_legends = [];
    for i = 0:15 // up to 15 links
    	if(	isfield(model, "egressPort" + string(i) ) ) then
		    [this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s.%s.%s", switchName, "egressPort" + string(i), "PriorityQueue", "WRRScheduler", "sum_queue_sizes_bytes", "max"));
			
			// in case some model has more measurements
		    [this_t, t] = array_make_same_size(this_t, t);
		    [this_value, values] = array_make_same_size(this_value, values);
		    t(:, i+1) = this_t;
		    values(:, i+1) = this_value;
		    
			// use the mean to calculate the mean
			[this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s.%s.%s", switchName, "egressPort" + string(i), "PriorityQueue", "WRRScheduler", "sum_queue_sizes_bytes", "timeAvg"));
			plot_legends(:, i+1) = sprintf("%s.%s (timeAvg:%f Bytes/s)", switchName, "egressPort" + string(i), mean_value);		
		end	
    end
	
	// plot
	scf();
	plot(t, values , ".-");	
	xtitle(switchName + " - Switch max queue usage (sum of all priority queues)");
	xlabel('Time (s)'); ylabel("maximum sum of all priority queues (Bytes)");
	legend(plot_legends);
	
	if (~exists("savePlot", "local") | savePlot==%T) then
	   saveCurrentPlot()	   
	end
	
endfunction

function [] = plotHostQueues(hostName, savePlot)
	model = eval(hostName);
	
	// read all links (always with name 'egressPort')
    t = [];
    values = [];
    plot_legends = [];
    for i = 0:15 // up to 15 links
    	if(	isfield(model, "egressPort" + string(i) ) ) then
    	    // plot the max of the queue
		    [this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s.%s", hostName, "egressPort" + string(i), "portQueue", "qsize_bytes", "max"));
			
			// in case some model has more measurements
		    [this_t, t] = array_make_same_size(this_t, t);
		    [this_value, values] = array_make_same_size(this_value, values);
		    t(:, i+1) = this_t;
		    values(:, i+1) = this_value;
			
			// use the mean to calculate the mean
			[this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s.%s", hostName, "egressPort" + string(i), "portQueue", "qsize_bytes", "timeAvg"));
			plot_legends(:, i+1) = sprintf("%s.%s (timeAvg:%f Bytes)", hostName, "egressPort" + string(i), mean_value);		
		end	
    end
	
	// plot
	scf();
	plot(t, values , ".-");	
	xtitle(hostName + " - Host max output queue usage");
	xlabel('Time (s)'); ylabel("maximum queue size (Bytes)");
	legend(plot_legends);
	
	if (~exists("savePlot", "local") | savePlot==%T) then
	   saveCurrentPlot()	   
	end
	
endfunction

function [plotHandle] = plotEgressLinks(modelName, savePlot)
	model = eval(modelName);

	// read all links (always with name 'egressPort')
    t = [];
    values = [];
    plot_legends = [];
	for i = 0:15 // up to 15 links
		if(	isfield(model, "egressPort" + string(i) ) ) then
		   [this_t, this_value, mean_value] =  getTimeSerie(sprintf("%s.%s.%s.%s.%s", modelName, "egressPort" + string(i), "Link", "sent_bits", "sum"));
		   
		   // in case some model has more measurements
		   [this_t, t] = array_make_same_size(this_t, t);
		   [this_value, values] = array_make_same_size(this_value, values);
		   t(:, i+1) = this_t;
		   values(:, i+1) = this_value;
		   
		   plot_legends(:, i+1) = sprintf("%s.%s (avg:%f MBs)", modelName, "egressPort" + string(i), mean_value / (8*M* samplingPeriod));
		end		
	end 
	
	// plot
	values = values / (8*k); // plot in KB
	plotHandle = scf();
	plot(t, values , ".-");
	xtitle( modelName + " - Links Usage");
	xlabel('Time (s)'); ylabel("Link Utilization (KBytes/" + string(samplingPeriod) + "s)");
	legend(plot_legends);
	
	if (~exists("savePlot", "local") | savePlot==%T) then
	   saveCurrentPlot()	   
	end
endfunction

function [array1, array2] = array_make_same_size(array1, array2)
	[x1_rows, x1_cols] = size(array1);
	[x2_rows, x2_cols] = size(array2);
	if(x1_rows > x2_rows) then
		array2(x1_rows, :) = 0		
	end
	
	if(x2_rows > x1_rows) then
		array1(x2_rows, :) = 0		
	end
endfunction

function [time, values, avg] = getTimeSerie(variableName)
	without_variableName = strsubst(variableName, strrchr(variableName, '.'), "")

	time     = eval(sprintf("%s.%s''", without_variableName, "t"));
	values = eval(sprintf("%s''", variableName));
    avg  = eval(sprintf("mean(%s)", variableName));
endfunction
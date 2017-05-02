function [] = saveSimulationResults(saveDirectory, simu_index)
	dt = getdate();
	backupDirectory = sprintf('%s/%s_%i%.2i%.2i/', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
	logFileName = sprintf('%s_%i_%i%.2i%.2i_%2i%2i%2i', SimulationName, simu_index,  dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
	mkdir(backupDirectory);
	
	printf("Making a backup of all variables at " + + backupDirectory + logFileName + ".sod   ...\n")
	save(backupDirectory + logFileName + '.sod'); // saving all variables
	unix_g('cp pdevs*.log ' + backupDirectory ); // backup log
	unix_g('cp *.performance.log ' + backupDirectory ); // backup performance	 
endfunction

function [ParameterValues] = completeParameterSpace(parameter_1, parameter_2)
	ParameterValues = [];
	[col, i_rows] = size(parameter_1);
	[col, j_rows] = size(parameter_2);
	for i = 1:i_rows
		for j = 1:j_rows
	       ParameterValues(1,$+1) = parameter_1(i)
	       ParameterValues(2,$) = parameter_2(j)
		end
	end
endfunction

function [ParameterValues] = complete3ParameterSpace(parameter_1, parameter_2, parameter_3)
	ParameterValues = [];
	[col, i_rows] = size(parameter_1);
	[col, j_rows] = size(parameter_2);
	[col, k_rows] = size(parameter_3);
	for i = 1:i_rows
		for j = 1:j_rows
			for k = 1:k_rows
	       		ParameterValues(1,$+1) = parameter_1(i)
	       		ParameterValues(2,$) = parameter_2(j)
	       		ParameterValues(3,$) = parameter_3(k)
			end
		end
	end
endfunction


function [] = saveResults(fileName, directoryName)
	dt = getdate();
	
	// default parameters
	[out, inp] = argn(0);  	
  	if inp < 2 then, directoryName = sprintf('ScilabLogs/QT_%i%.2i%.2i/%.2d%.2d%.2d/', dt(1),dt(2),dt(6),dt(7), dt(8), dt(9)), end
  	if inp < 1 then, fileName = sprintf('simuResults_%i%.2d%.2d_%.2d%.2d%.2d', dt(1),dt(2),dt(6), dt(7), dt(8), dt(9)), end
	
	mkdir(directoryName);
	
	printf("making a backup of all variables at " + + directoryName + fileName + ".sod   ...\n")
	unix_g('cp pdevs*.log ' + directoryName ); // backup log
	unix_g('cp *.performance.log ' + directoryName ); // backup performance log
	save(directoryName + fileName + '.sod'); // saving all variables 
endfunction

function [] = saveCurrentPlot()
	plotHandle = gcf();
	axes = gca();
	
	dt = getdate();
	saveDirectory = "ScilabLogs";
	backupDirectory = sprintf('%s/%s_%i%.2i%.2i', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
	mkdir(backupDirectory);
	plot_file_name = sprintf('%s/%s_%i%.2i%.2i_%2i%2i%2i', backupDirectory, axes.title.text, dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
	
	// save as SVG 
	xs2svg(plotHandle, plot_file_name);
endfunction

/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**		       Universidad Nacional de Rosario - Argentina.
**  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
**
**  This file is part of PowerDEVS.
**
**  PowerDEVS is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  PowerDEVS is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "model.h"
#include "pdevslib.h"
#include <stdio.h>
#include <signal.h>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

//tmp
#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;
///

void sig_hdlr(int sig)
{
  //exit(sig);
	abort();
}

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
}

void printf_flush(const char *fmt,...) 
{

	va_list ap;
	va_start(ap, fmt); 
	vprintf(fmt, ap); 
	va_end(ap); 
	fflush(stdout);
}

double tf;
const char *msg = 

"PowerDEVS 2.0\n\
Copyright (C) 2009 Facultad de Ciencias Exactas Agrimensura e Ingeniería\n\
                   Universidad Nacional de Rosario - Argentina\n\
Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)\n\
License GPL: GNU GPL <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n";

const char *msgHelp = 
"PowerDEVS model\n\n\
  -i     Run intearctive shell\n\
  -tf t  Set final time to t (double format)\n\
  -ti t  Set initial time to t (double format)\n\
  -rt    Synchronize events with real time\n\
  -d     Debug simulation\n\
  -b n   Stop simulation after n steps in the same time (to detecet illegitimal models)\n";

void runInteractive(double tf, double ti, bool timed, int breakCount);

void parseCmdLineOptions(int ac,char **av, po::variables_map& simulatorParams, std::vector<std::string>& modelParams);

/** Config file arrays are parsed differently by boost. This function leaves them like the commandline parser **/
void addConfigLineOption(po::option o, std::vector<std::string>& modelParams);


int main(int argc,char **argv)
{

#ifdef RTAIOS
	preinitLib();
#endif 

	//printf_flush(" getFinalTime()=%f /n",getFinalTime());

	// Parse parameters
	po::variables_map simulatorParams;
	std::vector<std::string> modelParams;
	parseCmdLineOptions(argc, argv, simulatorParams, modelParams);

	// if help requested there is nothing else to be done
	if (simulatorParams.empty() || simulatorParams.count("help") || simulatorParams.count("version")) {
		return 0;
	}

	debug = simulatorParams.count("d") ;
	setFinalTime(simulatorParams["tf"].as<double>());
	double ti = simulatorParams["ti"].as<double>();
	bool interactive = simulatorParams.count("i") ;
	bool timed = simulatorParams.count("rt") ;
	int breakCount = simulatorParams["b"].as<int>();
	setCmdLineModelParams(modelParams); // set model params

	if(simulatorParams.count("print_model_params") || debug){
		cout << "Parameters sent to the atomic models: \n" << CmdLineModelParams << "\n";
	}

	if(debug){
		cout << "debug: " << debug << "\n";
		cout << "finalTime: " << tf << "\n";
		cout << "initialTime: " << ti << "\n";
		cout << "interactive: " << interactive << "\n";
		cout << "timed: " << timed << "\n";
		cout << "breakCount: " << breakCount << "\n";
	}

	if (interactive) {
		cout<< "running interactive \n";
		runInteractive(tf, ti, timed, breakCount);
	} else {
		cout << "Creating model... \n";
		Model*mod = new Model(ti);
		if (timed)
			mod->setRealTime();
		mod->setInitialTime(ti);
		mod->setBreakCount(breakCount);
		mod->init();
		mod->setFinalTime(tf);
		setFinalTime(tf); // set the global tf
		cout << "Model Created, start running ... \n";
		mod->run();
		cout << "Model completed running. \n";
		int ret=mod->getExitStatus();
		delete mod;
		return ret;
	}
	return 0;
}

void runInteractive(double tf, double ti, bool timed, int breakCount){
		Model *mod=NULL;
		char buff[1024];
		bool running=false;
		int runs=1;
		int runCount = 0;

		printf_flush("%s",msg);
		while (buff[0]!='q') {
			printf_flush("(pdevs)");
			char *res=fgets(buff,1024,stdin);
			if (res==NULL)
				break;
			if (buff[0]=='x') {
				if (!running) {
					runs=atoi(buff+1);
				}
			}
			if (buff[0]=='t') {
				if (!running) {
					tf=atof(buff+1);
					setFinalTime(tf); // set the global tf
				}
			}
			if (buff[0]=='y') {
				if (!running) {
					timed=atoi(buff+1);
				}
			}
			if (buff[0]=='r') {
				if (running) {
					mod->exit(tf);
					delete mod;
				}
				mod = new Model(ti);
				mod->setBreakCount(breakCount);
				mod->setInitialTime(ti);
				mod->init();
				mod->setFinalTime(tf);
				setFinalTime(tf); // set the global tf
				if (timed)
					mod->setRealTime();

				mod->run();
				printf_flush("Simulation ended %d\n",mod->getExitStatus());
				delete mod;
				mod=NULL;
				running=false;
			}
			if (buff[0]=='p') {
				if (running)
					printf_flush("%d\n",(runCount*(100/runs))+mod->percentageDone()/runs);
				else
					printf_flush("Model not running\n");
			}
			if (buff[0]=='b') {
				breakCount=atoi(buff+2);
			}
			if (buff[0]=='o') {
				mod->exit(tf);
				printf_flush("Simulation ended %d\n",mod->getExitStatus());
				delete mod;
				mod=NULL;
				running=false;
				runCount = 0;
			}
			if (buff[0]=='s') {
				int steps=atoi(buff+1);
				if (steps==0) steps=1;
				if (!running) {
					running=true;
					printf_flush("Init\n");
					mod=new Model(ti);
					mod->setInitialTime(ti);
					mod->setBreakCount(breakCount);
					mod->init();
					if (timed) mod->setRealTime();
					mod->setFinalTime(tf);
					printf_flush("Running\n");
				}
				if (mod->step(steps)) { // A Simulation has ended
					runCount ++;
					if (runCount<runs) {
						delete mod;
						mod=NULL;
						mod=new Model(ti);
						mod->setBreakCount(breakCount);
						mod->setInitialTime(ti);
						mod->init();
						if (timed)
							mod->setRealTime();
						mod->setFinalTime(tf);
					} else {
						printf_flush("Simulation ended %d\n",mod->getExitStatus());
						delete mod;
						mod=NULL;
						running=false;
						runCount = 0;
					}
				}
			}
			if (buff[0]=='h') {
				printf_flush("\
									  r: Run model\n\
									  p: Show percentage done\n\
									  y [0|1] sYnch events with real time\n\
									  x n: Run n simulations\n\
									  s [n]: Make n steps\n\
									  o Stop simulation\n\
									  t time: Set final Time\n\
									  b n: Stop simulating after n steps in the same time (illegitimal)\n\
									  q: quit\n");
			}

		}
}

/** Config file arrays are parsed differently by boost. This function leaves them like the commandline parser **/
void addConfigLineOption(po::option o, std::vector<std::string>& modelParams){
	// validate that we can safetly use the first element of value only
	if(o.value.size()!=1){
		std::cout << "options in the config file should have a single value" << o.string_key << "\n";
		throw std::runtime_error ("options in the config file should have a single value\n");
	}

//	std::string optionPrefix = "-" + o.string_key;
	auto keyExists = std::find_if(modelParams.begin(), modelParams.end(), [o](const std::string & modelParam) {
						//cout << "comparing:1)'" << modelParam << "' with  2)'"<<optionPrefix<<"'\n";
						return !modelParam.compare(1, o.string_key.size(), o.string_key);
				});

	if(keyExists != modelParams.end()){
		return; // if the key was already present, do nothing as cmdLine options have more priority than configuration options
	}

	std::string value = o.value[0];
	boost::erase_all(value, " "); // remove spaces

	// if it is not an array add the name=value pair
	if(value.find("{")==std::string::npos){
		modelParams.push_back(std::string("-") + o.string_key + std::string("=") + value); // add it as a cmdline options would appear
	} else 	{ // it is an array (starting with '{'
		// validate the array is closed
		if(value.find("}")==std::string::npos){
			std::cout << "Array "<< o.string_key << " is not closed. Expecting '}' at the end of the line \n" ;
			throw std::runtime_error ("Array " + o.string_key + " is not closed. Expecting '}' at the end of the line \n");
		}

		boost::char_separator<char> sep(",");
		boost::erase_all(value, "{"); // remove {
		boost::erase_all(value, "}"); // remove }

		// tokenize values and add them onto modelParams
		boost::tokenizer<boost::char_separator<char> > tokens(value, sep);
		for(auto token : tokens){
			modelParams.push_back(std::string("-") + o.string_key + std::string("=") + token); // add it as a cmdline options would appear
		}
	}
}


void parseCmdLineOptions(int ac,char **av, po::variables_map& simulatorParams, std::vector<std::string>& modelParams){
	try {
		string config_file;
		 // Declare a group of options that will be allowed only on command line
		po::options_description generic("Simulator Command Line Options");
		generic.add_options()
				("help,h", "Print this help.")
				("version,v", "Print version, contact and license information.")
				("print_model_params", "Print all parameters sent to the atomic models")
				("c", po::value<std::string>(&config_file)->default_value("model.params"), "path for the configuration file.")
					;

		// Declare a group of options that will be allowed both on command line and in config file
		po::options_description config_file_options("Simulator Configuration (and Command Line) options");
		config_file_options.add_options()
			("i",  "Run interactive shell.")
			("b", po::value<int>()->default_value(1000000), "Stop simulation after n steps in the same time (to detect illegitimate models).")
			("d",  "Debug simulation.")
			("tf", po::value<double>()->default_value(30), "Set final time to t (double format).")
			("ti", po::value<double>()->default_value(0), "Set initial time to t (double format).")
			("rt",  "Synchronize events with real time.");

		// accept all options from command line
		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config_file_options);

		// parse the cmdline options
		auto parsed_cmdLine_options = po::command_line_parser(ac,av).options(cmdline_options)
						.style(po::command_line_style::allow_long | po::command_line_style::long_allow_adjacent | po::command_line_style::long_allow_next
									| po::command_line_style::allow_long_disguise) // to enable options to start with a '-' instead of '--'
						.allow_unregistered()    // to allow generic parameters (which can be read by models)
				        .run();
		po::store(parsed_cmdLine_options, simulatorParams);
		notify(simulatorParams);

		// undeclared options are considered parameters for the model
		for (const auto& o : parsed_cmdLine_options.options) {
			if (simulatorParams.find(o.string_key) == simulatorParams.end()) { // an unknown option
				copy(o.original_tokens.begin(), o.original_tokens.end(), back_inserter(modelParams));
			}
		}

		// Parse the config file
		ifstream ifs(config_file.c_str());
		if (!ifs)
		{
			cout << "[warning]: config file not found (or unable to open): " << config_file << "\n";
		}
		else
		{
			auto parsed_config_options = parse_config_file(ifs, config_file_options, true);
			po::store(parsed_config_options, simulatorParams);
			notify(simulatorParams);

			// undeclared options are considered parameters for the model
			for (const auto& o : parsed_config_options.options) {
			    if (simulatorParams.find(o.string_key) == simulatorParams.end()) {
			        // an unknown option
			    	addConfigLineOption(o, modelParams);
			    }
			}
		}

		// Show version if requested
		if (simulatorParams.count("version")) {
			cout << msg;
		}

		// Show help is requested
		if (simulatorParams.empty() || simulatorParams.count("help")) {
			cout << "PowerDEVS model\n\n";
			cout << cmdline_options << "\n";
			cout << "\nParameters for the model:\n"<<
					"\tAn arbitrary amount of parameters can be passed using the Command Line or the Configuration file.\n"<<
					"\tThese parameters can be accessed by the atomic models using getCmdLineParam<T>() function.\n"<<
					"\tThe following syntax is accepted:\n"<<
					"\t\tparamName=paramValue  \t\t\t\t(for int, double, string, etc)\n" <<
					"\t\tparamName={paramValue1, paramValue2, ..., paramValueN}  \t(for array parameters)\n"<<
			        "\tWhen using the Command line appen a '-' to each paramName (unix style). For the Configuration file each parameter should be declared in a new line.\n";
			cout << "\n";
		}
	}
	catch(exception& e)
	{
		cout << e.what() << "\n";
		throw e;
	}
}


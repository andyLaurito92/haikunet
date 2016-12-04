require 'commander'
require 'uri'

class CommandLineArguments
    include Commander::Methods

    attr_reader :file_name, :destiny_name, :uri_initial_topo

    def run
        program :name, 'Haikunet interpreter'
        program :version, '0.0.1'
        program :description, 'The interpreter for Haikunet programming language. Haikunet is a network intents programming language, which is focused in making programmer\'s life easier.'
        program :help, 'Author', 'Andrés Laurito'

        my_command_line_argument = self 

        my_command = command :program do |c|
          c.syntax = 'haikunet program [OPTIONS]'
          c.summary = 'Specify the file name of the haikunet program'
          c.description = 'Specify the file name of the haikunet program'
          c.option '-n', '--name NAME', String, 'Specify the file name'
          c.option '-d', '--destiny NAME', String, 'Specify the current destiny for the intent. Haikunet currently support ONOS, OPENDAYLIGHT and DEBUG modes'
          c.option '-u', '--uriInitialTopo NAME', String, 'Specify the uri from where to get the initial topology. The uri can be either a controller\'s url, for example http://127.0.0.1:8181/onos/v1/, or a network topology file\'s path (the network topologie\'s file has to has the structure specified by the topologyGenerator gem).'
          c.example 'Interpretating a file called hellohaikunet.hk heading the output to ONOS and getting the initial topology from ONOS', 'haikunet progran -n hellohaikunet.hk -d ONOS -u http://127.0.0.1:8181/onos/v1/'
          c.example 'Interpretating the same file hellohaikunet.hk heading the output to DEBUG and getting the initial topology from ONOS', 'haikunet progran -n hellohaikunet.hk -d DEBUG -u http://127.0.0.1:8181/onos/v1/'
          c.example 'Interpretating the file hellohaikunet.hk heading the output to DEBUG and getting the initial topology from a file', 'haikunet progran -n hellohaikunet.hk -d DEBUG -u network_topologies_examples/tree_topology.rb'
          c.action do |args, options|
            @file_name = options.name
            @destiny_name = options.destiny
            @uri_initial_topo = options.uriInitialTopo
            raise ArgumentError, "The file #{@file_name} does not exist. Please check that you provide the correct path." unless File.file?(@file_name)
            raise ArgumentError, "The destiny #{@destiny_name} was not provided. Please select one of the currently supported (haikunet --help to see currently suported destinies)." unless @destiny_name
            raise ArgumentError, "The destiny provided was #{destiny_name} which is not one of the supported by Haikunet. Please select one of the currently supported (haikunet --help to see currently suported destinies)." unless ['OPENDAYLIGHT', 'ONOS', 'DEVS'].include? @destiny_name
            raise ArgumentError, "The uriInitialTopo provided #{@uriInitialTopo} is not a valid one." unless (File.exists? @uri_initial_topo) || @uri_initial_topo =~ /\A#{URI::regexp(['http', 'https'])}\z/
            raise ArgumentError, "The interpreter only accepts files with the extension .hk, file name provided was #{@file_name}" unless ['.hk'].include? File.extname(@file_name)
          end
        end

    	run!
    end
end

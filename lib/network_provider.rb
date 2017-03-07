class NetworkProvider
    def initialize(file_name, uri_initial_topology)
        validate file_name, uri_initial_topology

        @file_name = file_name
        source_name = get_source_from uri_initial_topology
        @topology_generator = Topologygenerator.new({
            "source" => source_name,
            "directory_concrete_builders" => "#{File.dirname(File.realpath(__FILE__))}/haikunet_builders/initial_topology_builders",
            "output_directory" => "#{ENV['HOME']}/.haikunet/initial_topology/#{@file_name}",
            "uri_resource" => "#{uri_initial_topology}"
        })

        @topology_generator.generate

        require "#{ENV['HOME']}/.haikunet/initial_topology/#{@file_name}/initial_network_topology.rb"
        extend NetworkTopology
    end

    def initial_topology
        get_topology
    end

    "TODO: IMPROVE THIS
    In this function we check if either an URI or a PATH is recieved. In the second case, CUSTOM is returned.
    In the first case, it has to be returned either ONOS or OPENDAYLIGHT, since we just have the URI, we 
    check in it if the string onos appears in the URI."
    def get_source_from(uri_initial_topology)
        if uri_initial_topology =~ /\A#{URI::regexp(['http', 'https'])}\z/ 
            return "ONOS" if uri_initial_topology.include? "onos"
            return "OPENDAYLIGHT"
        else
            return "CUSTOM"
        end
    end

    def validate(file_name, uri_initial_topology)
        raise ArgumentError, "A valid file name must be provided, #{file_name} was recieved" if file_name.nil?
        raise ArgumentError, "The uri_initial_topology provided #{uri_initial_topology} is not a valid one." unless (File.exists? uri_initial_topology) || uri_initial_topology =~ /\A#{URI::regexp(['http', 'https'])}\z/
    end

    def source_provider
        @topology_generator.topology_provider
    end
end
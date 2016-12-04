require 'topologygenerator'

class NetworkProvider
    def initialize(file_name, uri_initial_topology)
        @file_name = file_name
        @topology_generator = Topologygenerator.new({
            "source" => "ONOS",
            "directory_concrete_builders" => "#{File.dirname(File.realpath(__FILE__))}/haikunet_topology_generator_builder",
            "output_directory" => "#{ENV['HOME']}/.haikunet/#{@file_name}",
            "uri_resource" => uri_initial_topology 
        })
    end

    def initial_topology
        @topology_generator.generate

        require "#{ENV['HOME']}/.haikunet/#{@file_name}/initial_network_topology.rb"
        extend NetworkTopology

        get_topology
    end

    def source_provider
        @topology_generator.topology_provider
    end
end
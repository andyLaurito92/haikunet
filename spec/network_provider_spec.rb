require 'spec_helper'
require 'network_provider.rb'
require 'topologygenerator'
require 'byebug'

describe NetworkProvider do
  context 'when no parameter is recieved' do
    it "raise an ArgumentError" do
      expect { NetworkProvider.new  }.to raise_error ArgumentError
    end
  end

  context 'invalid parameters are provided' do
    it 'raises an ArgumentError when no file is recieved' do
      expect { NetworkProvider.new nil, "something" }.to raise_error ArgumentError, "A valid file name must be provided,  was recieved"
    end

    it 'raises an ArgumentError when an invalid uri is recieved' do
      expect { NetworkProvider.new "pepe", "something" }.to raise_error ArgumentError, "The uri_initial_topology provided something is not a valid one."
    end
  end

  context 'when a file name and an uri are provided' do
    before :each do
      @file_name = double
      @initial_topo_uri = double
      @my_topology_generator = double
      allow(@my_topology_generator).to receive(:generate)
      allow(@file_name).to receive(:nil?).and_return false
      allow(@initial_topo_uri).to receive(:to_s).and_return '/valid/uri'
      allow(@file_name).to receive(:to_s).and_return 'file_pepito'
      allow(File).to receive(:exists?).and_return true
      allow(Topologygenerator).to receive(:new).and_return @my_topology_generator

      module NetworkTopology 
        def get_topology 
          "my_topo" 
        end
      end

    end

    it "creates a valid NetworkProvider with CUSTOM as source" do
      allow_any_instance_of(NetworkProvider).to receive(:require)
      allow_any_instance_of(NetworkProvider).to receive(:extend).with(NetworkTopology)

      expect(Topologygenerator).to receive(:new).with({
          "source" => "CUSTOM",
          "directory_concrete_builders" => "/home/andylaurito/repos/haikunet/lib/haikunet_builders/initial_topology_builders",
          "output_directory" => "#{ENV['HOME']}/.haikunet/initial_topology/file_pepito",
          "uri_resource" => "/valid/uri"
      })
      NetworkProvider.new @file_name, @initial_topo_uri
    end

    it "retrieves the expected initial topology" do #TODO: Improve the test
      my_topology_generator_mock = double
      allow(my_topology_generator_mock).to receive(:generate)
      allow_any_instance_of(NetworkProvider).to receive(:require)
      allow(Topologygenerator).to receive(:new).and_return my_topology_generator_mock

      expect_any_instance_of(NetworkProvider).to receive(:require).with("#{ENV['HOME']}/.haikunet/initial_topology/#{@file_name}/initial_network_topology.rb").once
      expect(my_topology_generator_mock).to receive(:generate).once

      my_network_provider = NetworkProvider.new @file_name, @initial_topo_uri
      my_initial_topo = my_network_provider.initial_topology

      expect(my_initial_topo).to eq('my_topo')
    end
  end

  context '#get_source_from' do #TODO: IMPROVE THIS METHOD, ONOS IS RETURNED WHEN AN URI CONTAINS ONOS AS STRING. OPENDAYLIGHT IS RETURN OTHERWISE. CUSTOM IS RETURNED IF NO URI IS RECIEVED
    before :each do
      file_name = double
      initial_topo_uri = double
      my_topology_generator = double
      allow(my_topology_generator).to receive(:generate)
      allow_any_instance_of(NetworkProvider).to receive(:require)
      allow(file_name).to receive(:nil?).and_return false
      allow(File).to receive(:exists?).and_return true
      allow(Topologygenerator).to receive(:new).and_return my_topology_generator

      @my_network_provider = NetworkProvider.new file_name, initial_topo_uri
    end
    it 'returns ONOS when an ONOS uri is recieved' do
      expect(@my_network_provider.get_source_from 'http://127.0.0.1:8181/onos/v1/').to eq("ONOS")

      expect(@my_network_provider.get_source_from 'http://onos/').to eq("ONOS")
    end
    it 'returns OPENDAYLIGHT when an OPENDAYLIGHT uri is recieved' do
      expect(@my_network_provider.get_source_from 'http://localhost:8080/restconf/operational/network-topology:network-topology/topology/flow:1/').to eq("OPENDAYLIGHT")

      expect(@my_network_provider.get_source_from 'http://pepito/').to eq("OPENDAYLIGHT")      
    end
    it 'returns CUSTOM when a path is recieved' do
      expect(@my_network_provider.get_source_from '/valid/path/to/topology').to eq("CUSTOM")
    end
  end
end
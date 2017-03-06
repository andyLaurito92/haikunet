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
      allow(@file_name).to receive(:nil?).and_return false
      allow(@initial_topo_uri).to receive(:to_s).and_return '/valid/uri'
      allow(@file_name).to receive(:to_s).and_return 'file_pepito'
      allow(File).to receive(:exists?).and_return true
      allow(Topologygenerator).to receive(:new)
    end

    it "creates a valid NetworkProvider with CUSTOM as source" do
      expect(Topologygenerator).to receive(:new).with({
          "source" => "CUSTOM",
          "directory_concrete_builders" => "/home/andylaurito/repos/haikunet/lib/haikunet_builders/initial_topology_builders",
          "output_directory" => "#{ENV['HOME']}/.haikunet/initial_topology/file_pepito",
          "uri_resource" => "/valid/uri"
      })
      NetworkProvider.new @file_name, @initial_topo_uri
    end

    it "retrieves the expected initial topology" do
      expect(false).to eq(true)
    end
  end

  context '#get_source' do #TODO: IMPROVE THIS METHOD, ONOS IS RETURNED WHEN AN URI CONTAINS ONOS AS STRING. OPENDAYLIGHT IS RETURN OTHERWISE. CUSTOM IS RETURNED IF NO URI IS RECIEVED
    before :each do
      file_name = double
      initial_topo_uri = double
      allow(file_name).to receive(:nil?).and_return false
      allow(File).to receive(:exists?).and_return true
      allow(Topologygenerator).to receive(:new)

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
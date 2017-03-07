require 'spec_helper'
require 'semantic_rules_checker.rb'
require 'byebug'

describe SemanticRulesChecker do
  context '#check' do
    before :each do
      @my_context = double
      @my_topology = double
    end

    it "raise an error if an invalid context is provided" do
      expect { SemanticRulesChecker.new.check @my_context, @my_topology }.to raise_error ArgumentError
    end

    it "raise an error if an invalid topology is provided" do
      expect { SemanticRulesChecker.new.check @my_context, @my_topology }.to raise_error ArgumentError
    end

    it "checks for the expected semantic errors" do
      allow(@my_context).to receive(:class).and_return Hash
      allow(@my_context).to receive(:key?).and_return true
      allow(@my_topology).to receive(:class).and_return Topology

      expect_any_instance_of(SemanticRulesChecker).to receive(:hosts_are_well_defined).once
      expect_any_instance_of(SemanticRulesChecker).to receive(:flows_are_well_defined).once
      expect_any_instance_of(SemanticRulesChecker).to receive(:values_defined_in_flows_are_defined_in_topology).once
      expect_any_instance_of(SemanticRulesChecker).to receive(:paths_for_flows_exists).once

      SemanticRulesChecker.new.check @my_context, @my_topology
    end
  end
end
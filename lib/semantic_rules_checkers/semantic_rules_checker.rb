require "ipaddress"
require_relative '../utils/custom_file_utils.rb'

class SemanticRulesChecker
    def initialize(destiny)
        require_relative "#{destiny.downcase}_semantic_rules_checker.rb"
        self.class.send(:include, Object.const_get("#{destiny.capitalize}SemanticRulesChecker"))
    end

    def check(context, topology)
        @context = context
        @topology = topology

        hosts_are_well_defined

        flows_are_well_defined

        values_defined_in_flows_are_defined_in_topology

        paths_for_flows_exists
    end

    def raise_semantic_error(message)
        raise SemanticalError, "A semantic error was found in one of the flow definitions. The problem is that #{message}\n Please correct this error in order to run the program ;)."
    end
end
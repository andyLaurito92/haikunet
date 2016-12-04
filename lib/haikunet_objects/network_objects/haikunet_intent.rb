class HaikunetIntent
	attr_accessor :name, :select, :condition
	def initialize(name)
		@name = name
	end
end
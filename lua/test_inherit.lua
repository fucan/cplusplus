function createRobot(name,id)
	local obj={name=name,id=id}
	function obj:SetName(name)
		self.name = name
	end
	
	function obj.GetName(self)
		return self.name
	end
	
	function obj.GetId(self)
		return self.id
	end
	
	return obj
end

function createFootballRobot(name,id,position)
	local obj=createRobot(name,id)
	obj.position =position
	
	function obj:SetPosition(p)
		self.position = p
	end
	
	function obj.GetPosition(self)
		return self.position
	end
	
	return obj
end

o1 = createFootballRobot("Sam",001,"left ")

print("o1's name:",o1:GetName(),"o1's id:",o1:GetId(),"o1's position",o1.GetPosition(o1))
module(...,package.seeall)	--�����
ver = "0.1 alpha"

function aFunInMyPack()
	print("Hello!")
end

_G.aFuncFromMyPack = 
aFunInMyPack
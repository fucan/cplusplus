--test_package.lua:
pack =require "mypack" --�����
print(var or "No ver defined!")
print(pack.ver)

print(aFunInMyPack or "No aFunInMyPack defined!")
pack.aFunInMyPack()

print(aFuncFromMyPack or "No aFuncFromMyPack defined!")
aFuncFromMyPack()
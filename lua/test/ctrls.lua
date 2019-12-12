-- export LUA_CPATH="$LUA_CPATH;/home/abhishek/code/clibdocker/build/lib?.so"

docker = require("luaclibdocker")
d = docker.connect_url("http://l3:2376/")

-- for k,v in ipairs(getmetatable(d)) do
--     print (k .. "=" .. v)
-- end

x = d:container_ls()
print(x)
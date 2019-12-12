json = require("json")
docker = require("luaclibdocker")

test_js = docker.json_create('{ "x": 1, "y": 2 }');
test_js_str = test_js:to_string()
print(test_js_str)

d = docker.connect()
-- d = docker.connect_url("http://l3:2376/")

ctr_ls_str = d:container_ls()
ctr_ls = json.decode(ctr_ls_str)

-- print(ctr_ls_str)

for k, v in ipairs(ctr_ls) do
    print("Container #" .. k .. " is " .. v.Names[1])
end
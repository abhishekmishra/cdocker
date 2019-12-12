json = require("json")
docker = require("luaclibdocker")

test_js = docker.json_create('{ "x": 1, "y": 2 }');
test_js_str = test_js:to_string()
print(test_js_str)

test_create_params = {
    Image = "alpine",
    Cmd = { "echo", "hello", "world" }
}

test_create_params_js = docker.json_create(json.encode(test_create_params));
test_create_params_js_str = test_create_params_js:to_string()
print(test_create_params_js_str)

d = docker.connect()
-- d = docker.connect_url("http://l3:2376/")

-- Create a container
created_id = d:container_create(test_create_params_js)
print("Created container id " .. created_id)

-- Start the container
d:container_start(created_id)

-- Inspect container
ctr = d:container_inspect(created_id)
-- print(ctr)
ctr = json.decode(ctr)
print(ctr.Name)

-- Print container logs
logs = d:container_logs_raw(created_id)
print(logs)

-- Remove the container
d:container_remove(created_id)

-- List containers
ctr_ls_str = d:container_ls()
ctr_ls = json.decode(ctr_ls_str)

-- print(ctr_ls_str)

for k, v in ipairs(ctr_ls) do
    print("Container #" .. k .. " is " .. v.Names[1])
end
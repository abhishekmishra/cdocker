local class = require "lib.middleclass"

local Dockerfile = class('Dockerfile')

function Dockerfile:initialize()
    self.content = ''    
end

function Dockerfile:command(cmdName, args)
    self.content = self.content .. '\n' .. cmdName
    if args ~= nil then
        if type(args) == 'table' then
            for _, v in ipairs(args) do
                self.content = self.content .. ' ' .. v
            end
        else
            self.content = self.content .. ' ' .. args
        end
    end
    return self
end

function Dockerfile:from(fromImage)
    return self:command("FROM", fromImage)
end

function Dockerfile:contents()
    return self.content
end

function Dockerfile:run_shell(command)
    return self:command("RUN", command)
end

function Dockerfile:run_exec(command)
    if type(command) == 'table' then
        local command_str = "["
        for _, v in ipairs(command) do
            command_str = command_str .. '"' .. v .. '", '
        end
        command_str = command_str .. "]"
        return self:command("RUN", command_str)
    else
        return self:command("RUN", command)
    end
end

function Dockerfile:cmd_shell(command)
    return self:command("CMD", command)
end

function Dockerfile:cmd_exec(command)
    if type(command) == 'table' then
        local command_str = "["
        for _, v in ipairs(command) do
            command_str = command_str .. '"' .. v .. '", '
        end
        command_str = command_str .. "]"
        return self:command("CMD", command_str)
    else
        return self:command("CMD", command)
    end
end

function Dockerfile:label(labels)
    local label_str = ''
    for k, v in pairs(labels) do
        label_str = label_str .. ' "' .. k .. '"="' .. v .. '"'
    end
    return self:command('LABEL', label_str)
end

function Dockerfile:maintainer(name)
    return self:label({maintainer=name})
end

function Dockerfile:expose(ports)
    return self:command("EXPOSE", ports)
end

function Dockerfile:env(envs)
    local env_str = ''
    for k, v in pairs(envs) do
        env_str = env_str .. ' "' .. k .. '"="' .. v .. '"'
    end
    return self:command('ENV', env_str)
end

local d = Dockerfile:new()
    :from("alpine:latest")
    :run_exec({"ls", "-al"})
    :label({x='y', t='v'})
    :maintainer('abhishekmishra')
    :expose(8080)
    :env({CPATH="blah:bluh"})

print (d:contents())

local class = require "lib.middleclass"

local Dockerfile = class('Dockerfile')

function Dockerfile:initialize()
    self.content = ''    
end

function Dockerfile:command(cmdName, args)
    self.content = self.content .. cmdName
    if args ~= nil then
        if type(args) == 'table' then
            for _, v in ipairs(args) do
                self.content = self.content .. ' ' .. v
            end
        else
            self.content = self.content .. ' ' .. args
        end
    end
    self.content = self.content .. '\n'
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

function Dockerfile:add(src, dest, usr, grp)
    local paths_str = ""
    if type(src) == 'table' then
        for k, v in pairs(src) do
            paths_str = paths_str .. '"' .. v .. '", '
        end
    else
        paths_str = paths_str .. '"' .. src .. '"'
    end
    paths_str = paths_str .. ' "' .. dest .. '"' .. ''

    if usr~=nil then
        if grp~=nil then
            paths_str = "--chown=" .. usr .. ':'.. grp .." " .. paths_str
        else
            paths_str = "--chown=" .. usr .. " " .. paths_str
        end
    end
    return self:command("ADD", paths_str)
end

function Dockerfile:copy(src, dest, usr, grp)
    local paths_str = ""
    if type(src) == 'table' then
        for k, v in pairs(src) do
            paths_str = paths_str .. '"' .. v .. '", '
        end
    else
        paths_str = paths_str .. '"' .. src .. '"'
    end
    paths_str = paths_str .. ' "' .. dest .. '"' .. ''

    if usr~=nil then
        if grp~=nil then
            paths_str = "--chown=" .. usr .. ':'.. grp .." " .. paths_str
        else
            paths_str = "--chown=" .. usr .. " " .. paths_str
        end
    end
    return self:command("COPY", paths_str)
end

function Dockerfile:entrypoint_exec(cmd, params)
    local ep_str = '["' .. cmd .. '"'
    for _, v in ipairs(params) do
        ep_str = ep_str .. ', "' .. v .. '"'
    end
    ep_str = ep_str .. ']'
    return self:command('ENTRYPOINT', ep_str)
end

function Dockerfile:entrypoint_shell(cmd, params)
    local ep_str = '' .. cmd .. ''
    for _, v in ipairs(params) do
        ep_str = ep_str .. ' ' .. v .. ''
    end
    ep_str = ep_str .. ''
    return self:command('ENTRYPOINT', ep_str)
end

function Dockerfile:volume(vols)
    local vol_str = '['
    if type(vols) == 'table' then
        for _, v in ipairs(vols) do
            vol_str = vol_str .. '"' .. v .. '", '
        end
    else
        vol_str = vol_str .. '"' .. vols .. '"'
    end
    vol_str = vol_str .. ']'
    return self:command('VOLUME', vol_str)
end

function Dockerfile:user(uid, gid)
    if gid == nil then
        return self:command('USER', uid)
    else
        return self:command('USER', uid .. ':'.. gid)
    end
end

function Dockerfile:workdir(dir)
    return self:command('WORKDIR', dir)
end

function Dockerfile:arg(name, default)
    if default == nil then
        return self:command('ARG', name)
    else
        return self:command('ARG', name .. '='.. default)
    end
end

function Dockerfile:onbuild()
    self.content = self.content .. 'ONBUILD '
    return self
end

function Dockerfile:stopsignal(signal)
    return self:command('STOPSIGNAL', signal)
end

function Dockerfile:healthcheck(args)
    return self:command('HEALTHCHECK', args)
end

function Dockerfile:shell(args)
    local command_str = "["
    for _, v in ipairs(args) do
        command_str = command_str .. '"' .. v .. '", '
    end
    command_str = command_str .. "]"
    return self:command("SHELL", command_str)
end

function Dockerfile:write(path)
    local dfile = io.open(path, 'w')
    dfile:write(self:contents())
    dfile:close()
end

return Dockerfile

-- local d = Dockerfile:new()
--     :from("alpine:latest")
--     :run_exec({"ls", "-al"})
--     :label({x='y', t='v'})
--     :maintainer('abhishekmishra')
--     :expose(8080)
--     :env({CPATH="blah:bluh"})
--     :add({'some/path', 'other/path'}, 'dest/path', 'root', 'root')
--     :add('another', 'another')
--     :copy({'some/path', 'other/path'}, 'dest/path', 'root', 'root')
--     :entrypoint_exec("start", {"arg1", "arg2"})
--     :entrypoint_shell("start", {"arg1", "arg2"})
--     :volume('/data')
--     :user('root')
--     :workdir('/some/dir')
--     :arg('arg1', 'default_val1')
--     :onbuild()
--     :run_exec({'blah', 'bluh'})
--     :healthcheck({"some", "thing"})
--     :shell({"b", "c", "d"})

-- print (d:contents())
-- print()

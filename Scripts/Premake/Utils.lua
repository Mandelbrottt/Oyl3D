function pairsByKeys(t, f)
    local a = {}
    for n in pairs(t) do table.insert(a, n) end
    table.sort(a, f)
    local i = 0                 -- iterator variable
    local iter = function()     -- iterator function
        i = i + 1
        if a[i] == nil then
            return nil
        else
            return a[i], t[a[i]]
        end
    end
    return iter
end

local function mysplit (inputstr, sep)
    if sep == nil then
       sep = "%s"
    end
    local t={}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
       table.insert(t, str)
    end
    return t
end

-- https://stackoverflow.com/a/23535333
function script_dir(depth)
    -- By default, depth is the calling script
    depth = depth or 2
    
    local str = debug.getinfo(depth, "S").source:sub(2)
    local path = str:match("(.*[/\\])") or "."
    local split = mysplit(path, "/\\")
    local last_dir
    for _, v in ipairs(split) do
        last_dir = v
    end
    return last_dir
end
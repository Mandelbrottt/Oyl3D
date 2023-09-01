RearmModule({
    Group = "Executables",
    Language = premake.CPP,
    Kind = premake.WINDOWEDAPP,
    Dependencies = {
        "Core",
        "Editor",
    },
    Properties = function()

    end
})
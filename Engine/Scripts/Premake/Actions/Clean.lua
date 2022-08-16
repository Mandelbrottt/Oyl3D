if _ACTION == 'clean' then
    os.rmdir('Build')
    os.remove('**.sln')
    os.remove('**.vcxproj')
    os.remove('**.vcxproj.*')
end
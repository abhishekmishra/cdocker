# change to the build directory
Set-Location .\build

# run msbuild to build the library
# see https://github.com/Microsoft/vswhere/wiki/Find-MSBuild
# and https://github.com/microsoft/vswhere/wiki/Installing
# for finding msbuild using vswhere (and installing vswhere if does not exist)
$msbuildExe = vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe"

&$msbuildExe clibdocker.sln

# go back to parent directory
Set-Location ..

# I) Library setup
Here are the few steps to setup a library created via [lib_template](https://github.com/Scaly-Sphere-Studio/lib_template).

## 1) Edit the `CONTROL` file
The [CONTROL](CONTROL) file is used by `vcpkg`. Here is an example of what it should look like :
```
Source: sss-text-rendering
Description: Text rendering using FreeType and HarfBuzz
Build-Depends: freetype, harfbuzz, sss-commons
```
After installing a package via this file, `vcpkg list` will display something like :
```
sss-text-rendering:x64-windows        0.5.1        Text rendering using FreeType and HarfBuzz
sss-text-rendering:x86-windows        0.5.1        Text rendering using FreeType and HarfBuzz
```

## 2) Rename the `.vcxproj` and `.vcxproj.filters` files
Rename [untitled.vcxproj](untitled.vcxproj) and [untitled.vcxproj.filters](untitled.vcxproj.filters) to match your library name.<br/>
For example, if your library is named "Text-Rendering", you can rename those files to `Text-Rendering.vcxproj` and `Text-Rendering.vcxproj.filters`.

## 3) Run the Visual Studio project to edit the target's name
- **en** : Project -> Properties of _[project's name]_ -> Configuration properties -> General -> Target Name
- **fr** : Projet -> Propriétés de _[nom du projet]_ -> Propriétés de la configuration -> Général -> Nom de la cible

The name must match the `Source:` parameter in [CONTROL](CONTROL).<br/>
In the previous example, it should be `sss-text-rendering`.

# II) Install vcpkg scripts
## 1) How to allow `.ps1` scripts to run on Windows 10
Windows 10 doesn't allow powershell scripts to run by default. You can see your current execution policy with the command:
```ps1
Get-ExecutionPolicy
```
You'd want your execution policy to be either `Bypass` or `Unrestricted`. You can set it via:
```ps1
Set-ExecutionPolicy Bypass
```
## 2) How to run `.ps1` scripts directly from Visual Studio by double clicking
- Right click on the powershell file
- Click "Open with..."
- Click "Add..."
- Fill the parameters:
  - Program: `powershell.exe`
  - Arguments: `-noexit -File %1`
- Click "Ok", then "Set as Default"

## 3) Double click on `DownloadVcpkgScripts.ps1`
This will add [vcpkg_scripts](https://github.com/Scaly-Sphere-Studio/vcpkg_scripts) as a git submodule to this repository.<br/>
_NOTE :_ Simply run this script again to update the scripts when needed.

### a) Install.ps1
_NOTE :_ If you've just edited the Visual Studio project or solution, you might need to save them by closing. Else, the scripts won't be able to access the modifications.<br/>
This script exports the library sources then locally installs the package via `vcpkg`.<br/>
You can run it to install, or update a package _(be it installed locally before or not)_.<br />

### b) Remove.ps1
This script removes any local `vcpkg` installation of this library.<br/>
If the library was installed from another source, it will display a warning and skip the removal.

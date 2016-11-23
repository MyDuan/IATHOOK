# IATHOOK
An IAT hook sample to hook the function MessageBoxA in user32.dll

You should add the shlwapi.lib to the linker because I use the function "PathAppend"
Also you should go to "Linker settings -> System". Change the field "Subsystem" from "Windows" to "Console". Done.If you create a win32 project.

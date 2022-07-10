param ($windbgPath='C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe')
(Get-Content $PSScriptRoot\Manifest\config.xml) -Replace [regex]::escape('C:\the\path\to\this\repo'), "$PSScriptRoot" | Set-Content $PSScriptRoot\Manifest\config.xml
$TempFile = New-TemporaryFile
".settings load $PSScriptRoot\Manifest\config.xml`n.settings save`nqq`n" | Out-File -Encoding ASCII $TempFile
& "$windbgPath" "-c" "$<$TempFile" "C:\Windows\system32\cmd.exe"

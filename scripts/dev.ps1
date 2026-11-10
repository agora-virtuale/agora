[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$Run
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$previousPath = $env:PATH
$previousLocation = Get-Location
try {
    # Discover a pre-existing toolchain; never install or change global PATH.
    $compilerCommand = Get-Command g++.exe -ErrorAction SilentlyContinue
    $compilerPath = if ($compilerCommand) { $compilerCommand.Source } else { $null }
    if (-not $compilerPath) {
        foreach ($candidateBin in @("$env:SystemDrive\msys64\mingw64\bin", "$env:SystemDrive\msys64\ucrt64\bin")) {
            $candidateCompiler = Join-Path $candidateBin 'g++.exe'
            if (Test-Path -LiteralPath $candidateCompiler) { $compilerPath = $candidateCompiler; break }
        }
    }
    if (-not $compilerPath) { throw 'No installed MinGW g++ found. Configure a CLion toolchain or use a Visual Studio developer shell; see README.md.' }
    $compilerBin = Split-Path -Parent $compilerPath
    $env:PATH = "$compilerBin;$previousPath"
    $cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
    $ninjaCommand = Get-Command ninja.exe -ErrorAction SilentlyContinue
    if (-not $cmakeCommand -or -not $ninjaCommand) { throw 'CMake and Ninja must be installed and discoverable with the compiler. No software was installed.' }

    Set-Location -LiteralPath $projectRoot
    $localPresetsPath = Join-Path $projectRoot 'CMakeUserPresets.json'
    if (-not (Test-Path -LiteralPath $localPresetsPath)) {
        $configurePresets = @()
        $buildPresets = @()
        $testPresets = @()
        foreach ($mode in @('debug', 'release')) {
            $name = "local-$mode"
            $configurePresets += @{
                name = $name; inherits = $mode; displayName = "Agora Local $mode"
                cacheVariables = @{ CMAKE_CXX_COMPILER = $compilerPath; CMAKE_MAKE_PROGRAM = $ninjaCommand.Source }
                environment = @{ PATH = ($compilerBin + ';$penv{PATH}') }
            }
            $buildPresets += @{ name = $name; configurePreset = $name }
            $testPresets += @{ name = $name; configurePreset = $name; output = @{ outputOnFailure = $true } }
        }
        $presets = @{ version = 6; configurePresets = $configurePresets; buildPresets = $buildPresets; testPresets = $testPresets }
        $json = $presets | ConvertTo-Json -Depth 8
        [IO.File]::WriteAllText($localPresetsPath, $json, [Text.UTF8Encoding]::new($false))
    }

    $selectedPreset = 'local-' + $Configuration.ToLowerInvariant()
    $currentPresets = Get-Content -LiteralPath $localPresetsPath -Raw | ConvertFrom-Json
    if ($selectedPreset -notin @($currentPresets.configurePresets.name)) {
        throw "Existing CMakeUserPresets.json has no $selectedPreset preset. Preserved your file; configure your chosen preset manually."
    }
    & $cmakeCommand.Source --preset $selectedPreset
    if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
    & $cmakeCommand.Source --build --preset $selectedPreset --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
    $ctestPath = Join-Path (Split-Path -Parent $cmakeCommand.Source) 'ctest.exe'
    & $ctestPath --preset $selectedPreset
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
    if ($Run) {
        & (Join-Path $projectRoot "build/$selectedPreset/agora_sim.exe") --help
        if ($LASTEXITCODE -ne 0) { throw 'Starter executable failed.' }
    }
    Write-Host "Ready. Open this repository in CLion and select CMake preset $selectedPreset."
} finally {
    $env:PATH = $previousPath
    Set-Location -LiteralPath $previousLocation
}

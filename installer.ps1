$RepoUrl = "https://github.com/Merutilm/RFF-2.0",

$ExternRepos = @("https://github.com/ocornut/imgui")

$ErrorActionPreference = "Stop"
$WorkDir = (Get-Location).Path   # powershell executed dir

function Write-Step($msg) {
    Write-Host ""
    Write-Host "==== $msg ====" -ForegroundColor Cyan
}

# ---------------------------------------------------------------------------
# 1. MSYS2 installation (winget)
# ---------------------------------------------------------------------------
Write-Step "MSYS2 Installation"

$msys2Root = "C:\msys64"
$msys2Bash = Join-Path $msys2Root "clang64.exe"

if (-not (Test-Path $msys2Bash)) {
    winget install -e --id MSYS2.MSYS2 --accept-package-agreements --accept-source-agreements
}

if (-not (Test-Path $msys2Bash)) {
    throw "MSYS2 installation failed: $msys2Bash"
}

# ---------------------------------------------------------------------------
# 2. Package Installation  (clang toolchain, cmake, ninja, make, git, vulkan, glm, opencv)
# ---------------------------------------------------------------------------
Write-Step "MSYS2 Package Update / Installation"

# core update wtf?
& $msys2Bash -lc "pacman -Syu --noconfirm --needed"
& $msys2Bash -lc "pacman -Syu --noconfirm --needed"

$packages = @(
    "git",
    "mingw-w64-clang-x86_64-clang",
    "mingw-w64-clang-x86_64-make",
    "mingw-w64-clang-x86_64-cmake",
    "mingw-w64-clang-x86_64-ninja",
    "mingw-w64-clang-x86_64-gmp",
    "mingw-w64-clang-x86_64-vulkan",
    "mingw-w64-clang-x86_64-glm",
    "mingw-w64-clang-x86_64-opencv"
) -join " "

& $msys2Bash -lc "pacman -S --noconfirm --needed $packages"

# ---------------------------------------------------------------------------
# 3. clang64 toolchain to PATH
# ---------------------------------------------------------------------------
Write-Step "set PATH"

$clang64Bin = Join-Path $msys2Root "clang64\bin"
$usrBin     = Join-Path $msys2Root "usr\bin"
$env:PATH = "$clang64Bin;$usrBin;$env:PATH"

# ---------------------------------------------------------------------------
# 3-1. Check Version (원격 HEAD SHA vs 마지막 설치 시 저장한 SHA)
# ---------------------------------------------------------------------------
Write-Step "Check Version"

function Get-RemoteHeadSha($url) {
    $line = (git ls-remote $url HEAD) | Select-Object -First 1
    if (-not $line) { throw "Failed to load remote sha: $url" }
    return ($line -split "\s+")[0]
}

$RepoDirName = [System.IO.Path]::GetFileNameWithoutExtension($RepoUrl.TrimEnd('/'))
$VersionFile = Join-Path $WorkDir "version.config"

$upToDate = $false
$newestSha = Get-RemoteHeadSha $RepoUrl

if (Test-Path $VersionFile) {
    $installedSha = Get-Content $VersionFile
    if ($installedSha -eq $newestSha) {
        $upToDate = $true
    }
}

if ($upToDate) {
    Read-Host "Already up to date. Press any key to exit..." -ForegroundColor Green
    Exit
}



# ---------------------------------------------------------------------------
# 4. Clone Repository
# ---------------------------------------------------------------------------
Write-Step "Clone Repository"

$repoDir = Join-Path $WorkDir "RFF-2.0"

if (Test-Path $repoDir) {
    Remove-Item $repoDir -Recurse -Force
}

git clone $RepoUrl


# ---------------------------------------------------------------------------
# 4-1. cloning extern (e.g. imgui)
# ---------------------------------------------------------------------------
Write-Step "Cloning extern sources"

$externDir = Join-Path $repoDir "extern"
if (-not (Test-Path $externDir)) {
    New-Item -ItemType Directory -Path $externDir | Out-Null
}

foreach ($url in $ExternRepos) {
    git clone $url $externDir
}

# ---------------------------------------------------------------------------
# 5. Configuring CMake(clang + Ninja)
# ---------------------------------------------------------------------------
Write-Step "Building RFF2"

$BuildDir = Join-Path $repoDir "build"

cmake -S $repoDir -B $BuildDir -G "Ninja" `
    -DCMAKE_C_COMPILER=clang `
    -DCMAKE_CXX_COMPILER=clang++ `
    -DCMAKE_BUILD_TYPE=Release

cmake --build $BuildDir

# ---------------------------------------------------------------------------
# 6. remove all except bin and shaders
# ---------------------------------------------------------------------------
Write-Step "RFF2 Installation"

$targetDirs = @("res", bin", "shaders")

foreach ($name in $targetDirs) {

    $item = Join-Path $repoDir $name
    $dest = Join-Path $WorkDir $name

    if (Test-Path $dest) {
        Remove-Item -Path $dest -Recurse -Force
    }

    Move-Item -Path $item -Destination $dest -Recurse
}

Remove-Item -Path $repoDir -Recurse -Force
Set-Content -Path $VersionFile -Value $newestSha

Write-Step "Installation Finished"
Write-Host "Location: $WorkDir"



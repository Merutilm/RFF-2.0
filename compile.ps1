[Console]::OutputEncoding = [Text.UTF8Encoding]::new()


$dirIn = "../shdsrc"
$dirOut = "../shaders"
$filesIn = Get-ChildItem -Path $dirIn -File;
$len = $filesIn.Length;

$ext = ".spv"

Write-Output "-- Shader Compilation"
$startTime = [System.Diagnostics.Stopwatch]::StartNew()


# Remove previous .spv file
Get-ChildItem -Path $dirOut -File | ForEach-Object -Process {
    Remove-Item $_.FullName
}

$success = 0;
$fail = 0;

# Compile .spv file

for($count = 0; $count -lt $len; $count++){
    $pathIn = $filesIn[$count]
    $name = $pathIn.Name;
    $target = $pathIn.FullName
    $compiled = "$dirOut/$name$ext";

    Write-Host "# [$($count+1)/$len] Compiling $name..." -ForegroundColor Cyan
    try{
        glslc $target -o $compiled
        if ($LASTEXITCODE -ne 0) {
            throw
        }
        $success++;
    }catch{
        Write-Host "-- Failed to compile $name!" -ForegroundColor Red
        $fail++;
    }
}
$startTime.Stop()
$elapsedSec = [Math]::Round($startTime.Elapsed.TotalSeconds, 2)

Write-Host -NoNewLine "-- Shader Compilation Done ($($elapsedSec)s), "
Write-Host -NoNewLine "Success $success" -ForegroundColor Cyan
Write-Host -NoNewLine " | "
Write-Host -NoNewline "Fail $fail" -ForegroundColor Red
Write-Host -NoNewLine " | "
Write-HOst "Total $len"

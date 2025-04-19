# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# Replace all occurrences of 'device' with 'dev' in vkUnmapMemory calls where it deals with mBufferMemory
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(device, mBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(dev, mBufferMemory)"

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "Fixed all vkUnmapMemory calls using mBufferMemory in RenderWindow.cpp" 
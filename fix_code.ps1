# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# Fix the duplicate VkResult line by replacing it with a single line
$content = $content -replace "VkResult err = mDeviceFunctions->vkMapMemory\(device, mBufferMemory, bufferInfo.offset,\s+VkResult err = mDeviceFunctions->vkMapMemory\(dev, mBufferMemory, bufferInfo.offset,", "VkResult err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, bufferInfo.offset,"

# Replace all dev references to mBufferMemory with device
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(dev, mBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(device, mBufferMemory)"

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "File updated successfully" 
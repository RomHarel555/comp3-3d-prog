# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# Find the drawOutdoorScene function
$startPattern = "void RenderWindow::drawOutdoorScene\(VkDevice dev,"
$endPattern = "void RenderWindow::drawIndoorScene\("

# Extract the function content
$startIndex = $content.IndexOf($startPattern)
$endIndex = $content.IndexOf($endPattern, $startIndex)
$functionContent = $content.Substring($startIndex, $endIndex - $startIndex)

# Replace 'device' with 'dev' within the drawOutdoorScene function
$updatedFunctionContent = $functionContent -replace "mDeviceFunctions->vkUnmapMemory\(device, mBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(dev, mBufferMemory)"

# Replace the function in the file
$updatedContent = $content.Remove($startIndex, $functionContent.Length).Insert($startIndex, $updatedFunctionContent)

# Find the drawIndoorScene function
$startPattern = "void RenderWindow::drawIndoorScene\(VkDevice dev,"
$endPattern = "VkShaderModule RenderWindow::createShader\("

# Extract the function content
$startIndex = $updatedContent.IndexOf($startPattern)
$endIndex = $updatedContent.IndexOf($endPattern, $startIndex)
if ($endIndex -eq -1) {
    $endIndex = $updatedContent.Length
}
$functionContent = $updatedContent.Substring($startIndex, $endIndex - $startIndex)

# Replace 'device' with 'dev' within the drawIndoorScene function
$updatedFunctionContent = $functionContent -replace "mDeviceFunctions->vkUnmapMemory\(device, mBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(dev, mBufferMemory)"

# Replace the function in the file
$updatedContent = $updatedContent.Remove($startIndex, $functionContent.Length).Insert($startIndex, $updatedFunctionContent)

# Save the file
$updatedContent | Set-Content -Path "RenderWindow.cpp"

Write-Host "Updated drawOutdoorScene and drawIndoorScene functions to use 'dev' consistently" 
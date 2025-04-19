# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# 1. Fix the duplicate VkResult line
$content = $content -replace "VkResult err = mDeviceFunctions->vkMapMemory\(device, mBufferMemory, bufferInfo.offset,\s+VkResult err = mDeviceFunctions->vkMapMemory\(dev, mBufferMemory, bufferInfo.offset,", "VkResult err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, bufferInfo.offset,"

# 2. Fix the updateDoorState function to use 'device' consistently
$content = $content -replace "VkDevice dev = mWindow->device\(\);", "VkDevice device = mWindow->device();"
$content = $content -replace "mDeviceFunctions->vkMapMemory\(dev, mHouseDoorBufferMemory", "mDeviceFunctions->vkMapMemory(device, mHouseDoorBufferMemory"
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(dev, mHouseDoorBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(device, mHouseDoorBufferMemory)"

# 3. Fix the mCrateCubeIndexBuffer uses
$content = $content -replace "mDeviceFunctions->vkDestroyBuffer\(dev, mCrateCubeIndexBuffer", "mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeIndexBuffer"
$content = $content -replace "mDeviceFunctions->vkFreeMemory\(dev, mCrateCubeIndexBufferMemory", "mDeviceFunctions->vkFreeMemory(device, mCrateCubeIndexBufferMemory"

# 4. In the drawOutdoorScene and drawIndoorScene functions, keep 'dev' as the parameter name
# First make a function to extract and process a section
function Process-Function-Section {
    param (
        [string]$fullText,
        [string]$startPattern,
        [string]$endPattern
    )

    $startIndex = $fullText.IndexOf($startPattern)
    if ($startIndex -eq -1) {
        Write-Host "Warning: Could not find pattern: $startPattern"
        return $fullText
    }

    $endIndex = $fullText.IndexOf($endPattern, $startIndex)
    if ($endIndex -eq -1) {
        $endIndex = $fullText.Length
    }

    $beforeSection = $fullText.Substring(0, $startIndex)
    $section = $fullText.Substring($startIndex, $endIndex - $startIndex)
    $afterSection = $fullText.Substring($endIndex)

    # Replace device with dev in vkUnmapMemory calls within this function only
    $modifiedSection = $section -replace "mDeviceFunctions->vkUnmapMemory\(device, mBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(dev, mBufferMemory)"

    return $beforeSection + $modifiedSection + $afterSection
}

# Process drawOutdoorScene
$content = Process-Function-Section -fullText $content -startPattern "void RenderWindow::drawOutdoorScene(VkDevice dev," -endPattern "void RenderWindow::drawIndoorScene("

# Process drawIndoorScene
$content = Process-Function-Section -fullText $content -startPattern "void RenderWindow::drawIndoorScene(VkDevice dev," -endPattern "VkShaderModule RenderWindow::createShader("

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "Fixed all issues in RenderWindow.cpp" 
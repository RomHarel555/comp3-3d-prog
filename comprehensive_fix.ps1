# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# 1. First, find createIndoorSceneResources function and add a device declaration at the beginning
$createIndoorPattern = "void RenderWindow::createIndoorSceneResources\(\)"
$createIndoorIndex = $content.IndexOf($createIndoorPattern)
if ($createIndoorIndex -ne -1) {
    # Find the opening brace after the function declaration
    $openBraceIndex = $content.IndexOf("{", $createIndoorIndex)
    if ($openBraceIndex -ne -1) {
        # Insert VkDevice device = mWindow->device(); after the opening brace
        $content = $content.Insert($openBraceIndex + 1, "`n    VkDevice device = mWindow->device();")
    }
}

# 2. Fix the duplicate VkResult line by replacing it with a single line
$content = $content -replace "VkResult err = mDeviceFunctions->vkMapMemory\(device, mBufferMemory, bufferInfo.offset,\s+VkResult err = mDeviceFunctions->vkMapMemory\(dev, mBufferMemory, bufferInfo.offset,", "VkResult err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, bufferInfo.offset,"

# 3. Fix the updateDoorState function to use 'device' consistently
$content = $content -replace "VkDevice dev = mWindow->device\(\);", "VkDevice device = mWindow->device();"
$content = $content -replace "mDeviceFunctions->vkMapMemory\(dev, mHouseDoorBufferMemory", "mDeviceFunctions->vkMapMemory(device, mHouseDoorBufferMemory"
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(dev, mHouseDoorBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(device, mHouseDoorBufferMemory)"

# 4. Fix the mCrateCubeIndexBuffer uses
$content = $content -replace "mDeviceFunctions->vkDestroyBuffer\(dev, mCrateCubeIndexBuffer", "mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeIndexBuffer"
$content = $content -replace "mDeviceFunctions->vkFreeMemory\(dev, mCrateCubeIndexBufferMemory", "mDeviceFunctions->vkFreeMemory(device, mCrateCubeIndexBufferMemory"

# 5. In the createIndoorSceneResources function, replace all 'dev' with 'device'
# Using more specific replacements to avoid affecting other parts of the code
$content = $content -replace "mDeviceFunctions->vkCreateBuffer\(dev, &bufInfo,", "mDeviceFunctions->vkCreateBuffer(device, &bufInfo,"
$content = $content -replace "mDeviceFunctions->vkGetBufferMemoryRequirements\(dev, mIndoorWallsBuffer,", "mDeviceFunctions->vkGetBufferMemoryRequirements(device, mIndoorWallsBuffer,"
$content = $content -replace "mDeviceFunctions->vkAllocateMemory\(dev, &memAllocInfo,", "mDeviceFunctions->vkAllocateMemory(device, &memAllocInfo,"
$content = $content -replace "mDeviceFunctions->vkBindBufferMemory\(dev, mIndoorWallsBuffer,", "mDeviceFunctions->vkBindBufferMemory(device, mIndoorWallsBuffer,"
$content = $content -replace "mDeviceFunctions->vkMapMemory\(dev, mIndoorWallsBufferMemory,", "mDeviceFunctions->vkMapMemory(device, mIndoorWallsBufferMemory,"
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(dev, mIndoorWallsBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(device, mIndoorWallsBufferMemory)"

$content = $content -replace "mDeviceFunctions->vkCreateBuffer\(dev, &bufInfo, nullptr, &mExitDoorBuffer\)", "mDeviceFunctions->vkCreateBuffer(device, &bufInfo, nullptr, &mExitDoorBuffer)"
$content = $content -replace "mDeviceFunctions->vkGetBufferMemoryRequirements\(dev, mExitDoorBuffer,", "mDeviceFunctions->vkGetBufferMemoryRequirements(device, mExitDoorBuffer,"
$content = $content -replace "mDeviceFunctions->vkAllocateMemory\(dev, &memAllocInfo, nullptr, &mExitDoorBufferMemory\)", "mDeviceFunctions->vkAllocateMemory(device, &memAllocInfo, nullptr, &mExitDoorBufferMemory)"
$content = $content -replace "mDeviceFunctions->vkBindBufferMemory\(dev, mExitDoorBuffer,", "mDeviceFunctions->vkBindBufferMemory(device, mExitDoorBuffer,"
$content = $content -replace "mDeviceFunctions->vkMapMemory\(dev, mExitDoorBufferMemory,", "mDeviceFunctions->vkMapMemory(device, mExitDoorBufferMemory,"
$content = $content -replace "mDeviceFunctions->vkUnmapMemory\(dev, mExitDoorBufferMemory\)", "mDeviceFunctions->vkUnmapMemory(device, mExitDoorBufferMemory)"

$content = $content -replace "mDeviceFunctions->vkAllocateDescriptorSets\(dev, &descSetAllocInfo,", "mDeviceFunctions->vkAllocateDescriptorSets(device, &descSetAllocInfo,"
$content = $content -replace "mDeviceFunctions->vkUpdateDescriptorSets\(dev, 1, &descWrite,", "mDeviceFunctions->vkUpdateDescriptorSets(device, 1, &descWrite,"

# 6. For drawOutdoorScene and drawIndoorScene functions, use 'dev' parameter correctly
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
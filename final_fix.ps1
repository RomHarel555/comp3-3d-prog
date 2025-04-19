# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# 1. Find startNextFrame function and locate where it calls drawOutdoorScene/drawIndoorScene
$startNextFramePattern = "void RenderWindow::startNextFrame\(\)"
$startNextFrameIndex = $content.IndexOf($startNextFramePattern)

if ($startNextFrameIndex -ne -1) {
    # Find the section where it calls the draw functions with 'dev'
    $pattern1 = "drawOutdoorScene\(dev, cb,"
    $pattern2 = "drawIndoorScene\(dev, cb,"
    
    # Replace both patterns
    $content = $content -replace $pattern1, "drawOutdoorScene(device, cb,"
    $content = $content -replace $pattern2, "drawIndoorScene(device, cb,"
}

# 2. Fix the releaseResources function
$releaseResourcesPattern = "void RenderWindow::releaseResources\(\)"
$releaseResourcesIndex = $content.IndexOf($releaseResourcesPattern)

if ($releaseResourcesIndex -ne -1) {
    # Replace all instances of 'dev' with 'device' in the function
    # First, find the end of the function (next function declaration or end of file)
    $nextFunctionIndex = $content.IndexOf("void RenderWindow::", $releaseResourcesIndex + 1)
    if ($nextFunctionIndex -eq -1) {
        $nextFunctionIndex = $content.Length
    }
    
    # Extract the function content
    $beforeFunc = $content.Substring(0, $releaseResourcesIndex)
    $funcContent = $content.Substring($releaseResourcesIndex, $nextFunctionIndex - $releaseResourcesIndex)
    $afterFunc = $content.Substring($nextFunctionIndex)
    
    # Replace 'dev' with 'device' in the function
    $fixedFuncContent = $funcContent -replace "mDeviceFunctions->vkDestroyPipeline\(dev,", "mDeviceFunctions->vkDestroyPipeline(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkDestroyPipelineLayout\(dev,", "mDeviceFunctions->vkDestroyPipelineLayout(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkDestroyPipelineCache\(dev,", "mDeviceFunctions->vkDestroyPipelineCache(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkDestroyDescriptorSetLayout\(dev,", "mDeviceFunctions->vkDestroyDescriptorSetLayout(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkDestroyDescriptorPool\(dev,", "mDeviceFunctions->vkDestroyDescriptorPool(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkDestroyBuffer\(dev,", "mDeviceFunctions->vkDestroyBuffer(device,"
    $fixedFuncContent = $fixedFuncContent -replace "mDeviceFunctions->vkFreeMemory\(dev,", "mDeviceFunctions->vkFreeMemory(device,"
    
    # Update the content
    $content = $beforeFunc + $fixedFuncContent + $afterFunc
}

# 3. Make sure we clean up any remaining instances in VkShaderModule RenderWindow::createShader
$createShaderPattern = "VkShaderModule RenderWindow::createShader\("
$createShaderIndex = $content.IndexOf($createShaderPattern)

if ($createShaderIndex -ne -1) {
    # Find the next function declaration
    $nextFunctionIndex = $content.IndexOf("void RenderWindow::", $createShaderIndex + 1)
    if ($nextFunctionIndex -eq -1) {
        $nextFunctionIndex = $content.Length
    }
    
    # Extract the function content
    $beforeFunc = $content.Substring(0, $createShaderIndex)
    $funcContent = $content.Substring($createShaderIndex, $nextFunctionIndex - $createShaderIndex)
    $afterFunc = $content.Substring($nextFunctionIndex)
    
    # Replace any 'dev' with 'device' in the function
    $fixedFuncContent = $funcContent -replace "mDeviceFunctions->vkCreateShaderModule\(dev,", "mDeviceFunctions->vkCreateShaderModule(device,"
    
    # Update the content
    $content = $beforeFunc + $fixedFuncContent + $afterFunc
}

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "Fixed all remaining 'dev' issues in RenderWindow.cpp" 
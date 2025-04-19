# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# 1. Add device declaration to startNextFrame
$startNextFramePattern = "void RenderWindow::startNextFrame\(\)"
$startNextFrameIndex = $content.IndexOf($startNextFramePattern)
if ($startNextFrameIndex -ne -1) {
    $openBraceIndex = $content.IndexOf("{", $startNextFrameIndex)
    if ($openBraceIndex -ne -1) {
        # Insert VkDevice device = mWindow->device(); after the opening brace
        $content = $content.Insert($openBraceIndex + 1, "`n    VkDevice device = mWindow->device();")
    }
}

# 2. Replace dev with device in specific functions where it's not a parameter
$content = $content -replace "drawOutdoorScene\(dev,", "drawOutdoorScene(device,"
$content = $content -replace "drawIndoorScene\(dev,", "drawIndoorScene(device,"

# 3. Split into lines to process line by line
$lines = $content -split "`n"
$newLines = @()

# These are function declarations where 'dev' is a parameter
$skipPatterns = @(
    "void RenderWindow::drawOutdoorScene(VkDevice dev,",
    "void RenderWindow::drawIndoorScene(VkDevice dev,"
)

foreach ($line in $lines) {
    $skip = $false
    foreach ($pattern in $skipPatterns) {
        if ($line.Contains($pattern)) {
            $skip = $true
            break
        }
    }
    
    if (-not $skip) {
        # Replace 'dev' with 'device' in all other lines
        $line = $line -replace "\bdev\b", "device"
    }
    
    $newLines += $line
}

# 4. Join lines back together
$content = $newLines -join "`n"

# 5. Make sure releaseResources has device declaration
$releaseResourcesPattern = "void RenderWindow::releaseResources\(\)"
$releaseResourcesIndex = $content.IndexOf($releaseResourcesPattern)
if ($releaseResourcesIndex -ne -1) {
    $openBraceIndex = $content.IndexOf("{", $releaseResourcesIndex)
    if ($openBraceIndex -ne -1) {
        # Check if VkDevice device is already declared
        $nextLines = $content.Substring($openBraceIndex, 100)
        if (-not $nextLines.Contains("VkDevice device")) {
            # Add device declaration
            $content = $content.Insert($openBraceIndex + 1, "`n    VkDevice device = mWindow->device();")
        }
    }
}

# 6. Add device declaration to createIndoorSceneResources
$createIndoorPattern = "void RenderWindow::createIndoorSceneResources\(\)"
$createIndoorIndex = $content.IndexOf($createIndoorPattern)
if ($createIndoorIndex -ne -1) {
    $openBraceIndex = $content.IndexOf("{", $createIndoorIndex)
    if ($openBraceIndex -ne -1) {
        # Check if VkDevice device is already declared
        $nextLines = $content.Substring($openBraceIndex, 100)
        if (-not $nextLines.Contains("VkDevice device")) {
            # Add device declaration
            $content = $content.Insert($openBraceIndex + 1, "`n    VkDevice device = mWindow->device();")
        }
    }
}

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "Standardized device variable naming in RenderWindow.cpp" 
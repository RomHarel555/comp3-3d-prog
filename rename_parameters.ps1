# Read the file content
$content = Get-Content -Path "RenderWindow.cpp" -Raw

# 1. Change the parameter name in function declarations
$content = $content -replace "void RenderWindow::drawOutdoorScene\(VkDevice dev,", "void RenderWindow::drawOutdoorScene(VkDevice device,"
$content = $content -replace "void RenderWindow::drawIndoorScene\(VkDevice dev,", "void RenderWindow::drawIndoorScene(VkDevice device,"

# Save the file
$content | Set-Content -Path "RenderWindow.cpp"

Write-Host "Renamed function parameters from 'dev' to 'device'" 
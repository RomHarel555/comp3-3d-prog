// This is a modified version of createShader that tries multiple locations
VkShaderModule RenderWindow::createShader(const QString &name)
{
    // Try to open the file using the standard path
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s, trying alternate paths...", qPrintable(name));
        
        // Try without the colon prefix (local file)
        QString localPath = name;
        if (localPath.startsWith(":/")) {
            localPath = localPath.mid(2); // Remove the ":/" prefix
        }
        
        QFile localFile(localPath);
        if (!localFile.open(QIODevice::ReadOnly)) {
            // Try assets/shaders directory
            QString assetsPath = "assets/shaders/" + QFileInfo(localPath).fileName();
            QFile assetsFile(assetsPath);
            
            if (!assetsFile.open(QIODevice::ReadOnly)) {
                qWarning("Failed to read shader from all possible paths: %s", qPrintable(name));
                return VK_NULL_HANDLE;
            }
            
            QByteArray blob = assetsFile.readAll();
            assetsFile.close();
            
            VkShaderModuleCreateInfo shaderInfo;
            memset(&shaderInfo, 0, sizeof(shaderInfo));
            shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderInfo.codeSize = blob.size();
            shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
            VkShaderModule shaderModule;
            VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
            if (err != VK_SUCCESS) {
                qWarning("Failed to create shader module: %d", err);
                return VK_NULL_HANDLE;
            }
            
            qDebug("Successfully loaded shader from assets path: %s", qPrintable(assetsPath));
            return shaderModule;
        }
        
        QByteArray blob = localFile.readAll();
        localFile.close();
        
        VkShaderModuleCreateInfo shaderInfo;
        memset(&shaderInfo, 0, sizeof(shaderInfo));
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.codeSize = blob.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
        VkShaderModule shaderModule;
        VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
        if (err != VK_SUCCESS) {
            qWarning("Failed to create shader module: %d", err);
            return VK_NULL_HANDLE;
        }
        
        qDebug("Successfully loaded shader from local path: %s", qPrintable(localPath));
        return shaderModule;
    }
    
    // Original path worked
    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkShaderModule shaderModule;
    VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }
    
    qDebug("Successfully loaded shader from original path: %s", qPrintable(name));
    return shaderModule;
} 
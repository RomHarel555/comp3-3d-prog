with open('RenderWindow.cpp', 'r') as f:
    lines = f.readlines()

# Skip the duplicate line at 2710
new_lines = lines[:2709] + lines[2710:]

with open('RenderWindow.cpp', 'w') as f:
    f.writelines(new_lines)

# Replace any remaining 'dev' with 'device' where it refers to mBufferMemory
with open('RenderWindow.cpp', 'r') as f:
    content = f.read()

content = content.replace('vkUnmapMemory(dev, mBufferMemory)', 'vkUnmapMemory(device, mBufferMemory)')

with open('RenderWindow.cpp', 'w') as f:
    f.write(content)

print('File updated successfully') 
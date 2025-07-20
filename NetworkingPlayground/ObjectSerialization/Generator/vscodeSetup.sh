#!/bin/bash

# Get the directory where this script is located (root/Generator)
scriptDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Define the root directory as the parent of the script directory (root)
absoluteRootPath="$(dirname "$scriptDir")"

# Get the name of the parent folder
rootFolder="$(basename "$absoluteRootPath")"

# Check if absoluteRootPath/.vscode exists
if [ ! -d "${absoluteRootPath}/.vscode" ]; then
    echo "❌ ${rootFolder}/.vscode folder does not exist. Terminating."
    exit 1
fi

echo "Generating files..."

# Write debugCompiler.sh
cat << EOF > ${absoluteRootPath}/.vscode/debugCompiler.sh
#!/bin/bash
g++-13 -fdiagnostics-color=always -g -std=c++20 \\
    \$(find ./Source/Code -name '*.cpp') \\
    -I./Source/Code \\
    -o ./Solution/Debug${rootFolder}
EOF

chmod +x ${absoluteRootPath}/.vscode/debugCompiler.sh
echo "✅ debugCompiler.sh has been created"

# Write tasks.json
cat << 'EOF' > ${absoluteRootPath}/.vscode/tasks.json
{
    "tasks": [
        {
            "type": "shell",
            "label": "C/C++: g++-13 build project",
            "command": "bash",
            "args": [
                "-c",
                "clear && ${workspaceFolder}/.vscode/debugCompiler.sh"
            ],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Custom task to build project using g++-13"
        }
    ],
    "version": "2.0.0"
}
EOF

echo "✅ tasks.json has been created"

# Write launch.json
cat << EOF > ${absoluteRootPath}/.vscode/launch.json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug project with g++-13",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/Solution/Debug${rootFolder}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}/Source/Code/${rootFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: g++-13 build project"
        }
    ]
}
EOF

echo "✅ launch.json has been created"

echo "Files saved in ${rootFolder}/.vscode."
echo "Process completed!"

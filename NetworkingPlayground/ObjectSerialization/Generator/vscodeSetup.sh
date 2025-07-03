#!/bin/bash

# Get the directory where this script is located (root/Generator)
scriptDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Define the root directory as the parent of the script directory (root)
rootDir="$(dirname "$scriptDir")"

# Check if rootDir/.vscode exists
if [ ! -d "${rootDir}/.vscode" ]; then
    echo "❌ ${rootDir}/.vscode folder does not exist. Terminating."
    exit 1
fi

echo "Generating files..."

# Write debugCompiler.sh
cat << EOF > ${rootDir}/.vscode/debugCompiler.sh
#!/bin/bash
g++-13 -fdiagnostics-color=always -g -std=c++20 \\
    \$(find ./Source/Code -name '*.cpp') \\
    -I./Source/Code \\
    -o ./Source/Code/${rootDir}/main
EOF

chmod +x ${rootDir}/.vscode/debugCompiler.sh
echo "✅ debugCompiler.sh has been created"

# Write tasks.json
cat << 'EOF' > ${rootDir}/.vscode/tasks.json
{
    "tasks": [
        {
            "type": "shell",
            "label": "C/C++: g++-13 build project",
            "command": "${workspaceFolder}/.vscode/debugCompiler.sh",
            "args": [],
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
cat << EOF > ${rootDir}/.vscode/launch.json
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
            "program": "\${workspaceFolder}/Source/Code/${rootDir}/main",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}/Source/Code/${rootDir}",
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

# Get the name of the parent folder
parentFolderName="$(basename "$rootDir")"

echo "Files saved in ${parentFolderName}/.vscode."
echo "Process completed!"

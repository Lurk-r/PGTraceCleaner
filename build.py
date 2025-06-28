import os, shutil, subprocess
SPECFILE = "main.spec"
DISTFOLDER = "dist"
BUILDFOLDER = "build"
print("Building...")
pyinstaller_cmd = [
    "pyinstaller", "--onefile", "--noconfirm", "--clean", "--icon=NONE", "--uac-admin",
    "--exclude-module", "tkinter", "--exclude-module", "test", "--exclude-module", "unittest", "--exclude-module", "email", "--exclude-module", "html", "--exclude-module", "xml",
    "main.py"
]
try:
    subprocess.run(pyinstaller_cmd, check=True)
except subprocess.CalledProcessError as e:
    print(f"[ERROR] PyInstaller build failed: {e}")
    exit(1)
target_folder = os.path.abspath(os.path.join(DISTFOLDER, os.pardir))
source_file = os.path.join(DISTFOLDER, "main.exe")
destination_file = os.path.join(target_folder, "PGTraceCleaner.exe")
try:
    shutil.move(source_file, destination_file)
    print("Moved exe to this folder")
except FileNotFoundError:
    print(f"[ERROR] Built executable not found at {source_file}")
    exit(1)
except Exception as e:
    print(f"[ERROR] Failed to move executable: {e}")
    exit(1)
print("Cleaning up...")
for path in [SPECFILE, DISTFOLDER, BUILDFOLDER]:
    try:
        if os.path.isfile(path):
            os.remove(path)
            print(f"File '{path}' deleted successfully.")
        elif os.path.isdir(path):
            shutil.rmtree(path)
            print(f"Folder '{path}' deleted successfully.")
        else:
            print(f"'{path}' not found.")
    except Exception as e:
        print(f"[WARN] Failed to delete '{path}': {e}")
import os, shutil, re, fnmatch, winreg, colorama, psutil
from pathlib import Path
from colorama import Fore, Style
colorama.init()
def log(msg, level="info"):
    colors = {"info": Fore.WHITE, "warn": Fore.YELLOW, "error": Fore.RED, "success": Fore.GREEN}
    labels = {"info": "INFO", "warn": "WARN", "error": "ERROR", "success": "SUCCESS"}
    print(f"{colors.get(level, Fore.WHITE)}[{labels.get(level, 'INFO')}] {Style.RESET_ALL}{msg}")
def clear_console():
    os.system('cls')
def is_pg_running():
    for p in psutil.process_iter():
        try:
            if fnmatch.fnmatch(p.name().lower(), "pixel*gun*.exe"):
                return True
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
    return False
def find_pg_folder(appdata_path):
    local_low = Path(appdata_path).parent / "LocalLow"
    for team in local_low.glob("pixel*gun*"):
        if team.is_dir():
            for pg in team.glob("pixel*gun*"):
                if pg.is_dir():
                    return pg
    return None
def find_pg_id(root_dir, max_depth=3):
    user_dirs = []
    queue = [(root_dir, 0)]
    while queue:
        current_path, depth = queue.pop(0)
        if depth > max_depth:
            continue
        try:
            for entry in current_path.iterdir():
                if entry.is_dir():
                    if entry.name.startswith("user"):
                        user_dirs.append(entry)
                    else:
                        queue.append((entry, depth + 1))
        except Exception as e:
            log(f"Error accessing {str(current_path)}: {e}", "warn")
    return user_dirs
WILDCARD_PATTERNS = [
    "*ä*", "*acc*", "*afd*", "*anal*", "*auth*", "*clan*", "*cloud*", "*contine*", "*count*", "*crash*", "*data*", "*event*", "*face*", "*key*", "*histor*"
    "*ids*", "*indx*", "*inhouse*", "*item*", "*last*", "*lead*", "*lvi*", "*mail*", "*map*", "*mark*", "*new*", "*notif*", "*offs*", "*old*", "*permi*", "*perso*",
    "*player*", "*sched*", "*send*", "*session*", "*skin*", "*shop*", "*slot*", "*start*", "*stat*", "*steam*", "*terc*", "*time*", "*user*", "*version*"
]
REGEX_PATTERNS = [
    re.compile(r".*[ ._-]+\d+[ ._-]+.*", re.IGNORECASE),
    re.compile(r".*\d+[ ._-]+.*", re.IGNORECASE)
]
def matches_pattern(name, wildcards, regexes):
    nl = name.lower()
    return any(fnmatch.fnmatch(nl, w.lower()) for w in wildcards) or any(r.match(nl) for r in regexes)
def delete_reg_values(base_key, sub_key_path, wildcards, regexes):
    try:
        with winreg.OpenKey(base_key, sub_key_path, 0, winreg.KEY_READ | winreg.KEY_WRITE) as key:
            to_del = []
            i = 0
            while True:
                try:
                    val_name = winreg.EnumValue(key, i)[0]
                    if matches_pattern(val_name, wildcards, regexes):
                        to_del.append(val_name)
                    i += 1
                except OSError:
                    break
            if not to_del:
                log(f"Registry traces not found. Skipping...", "info")
                return 0, 0
            log(f"Found {len(to_del)} registry traces in '{sub_key_path}'. Deleting...", "info")
            cleared = 0
            for val in to_del:
                try:
                    winreg.DeleteValue(key, val)
                    cleared += 1
                except Exception:
                    log(f"Failed to delete registry value '{val}'.", "error")
            log(f"{cleared} registry traces deleted.", "success")
            return len(to_del), cleared
    except FileNotFoundError:
        log(f"Registry key '{sub_key_path}' not found. Skipping...", "info")
        return 0, 0
    except Exception:
        log(f"Failed to open registry key '{sub_key_path}'.", "error")
        return 0, 0
def find_reg_subkeys(base_key, parent_subkey, pattern):
    matched = []
    try:
        with winreg.OpenKey(base_key, parent_subkey) as key:
            i = 0
            while True:
                try:
                    sk = winreg.EnumKey(key, i)
                    if fnmatch.fnmatch(sk.lower(), pattern.lower()):
                        matched.append(sk)
                    i += 1
                except OSError:
                    break
    except FileNotFoundError:
        log(f"Registry key '{parent_subkey}' not found. Skipping...", "info")
    except Exception:
        log(f"Failed to access registry key '{parent_subkey}'.", "error")
    return matched
def clear_pg_temp():
    temp_dir = os.getenv("TEMP")
    if not temp_dir:
        log("Unable to get TEMP path.", "error")
        return 0
    deleted_count = 0
    for folder in Path(temp_dir).glob("pixel*gun*"):
        if folder.is_dir():
            log(f"Found Pixel Gun temp folder '{folder}'. Deleting...", "info")
            try:
                shutil.rmtree(folder)
                log("Pixel Gun temp folder deleted.", "success")
                deleted_count += 1
            except PermissionError:
                log(f"Permission denied deleting '{folder}'.", "error")
            except Exception as e:
                log(f"Error deleting '{folder}': {e}", "error")
    if deleted_count == 0:
        log("Pixel Gun temp folder not found. Skipping...", "info")
    return deleted_count
def clear_pg_logs(pg_dir):
    if pg_dir is None:
        log("Pixel Gun logs folder not found. Skipping...", "info")
        return False
    try:
        shutil.rmtree(pg_dir.parent)
        log("Pixel Gun logs deleted.", "success")
        return True
    except PermissionError:
        log("Permission denied deleting Pixel Gun logs.", "error")
        return False
    except Exception as e:
        log(f"Failed to delete Pixel Gun logs: {e}", "error")
        return False
def main():
    clear_console()
    os.system("title PGTC - Pixel Gun Trace Cleaner")
    print("""> Pixel Gun Trace Cleaner | by .lurk_r <+>
- PGTC reduces ban chances on new account.
- Run this before creating/playing on new account.
- Join discord.gg/GYSdwzQtD3
""")
    if is_pg_running():
        log("Pixel Gun is running. Please close it before running this script.", "error")
        input("Press Enter to exit.")
        return
    appdata = os.getenv("APPDATA")
    if not appdata:
        log("Unable to get APPDATA path.", "error")
        input("Press Enter to exit.")
        return
    pg_dir = find_pg_folder(appdata)
    acc_id_str = "N/A"
    if pg_dir:
        log(f"Found Pixel Gun logs folder '{pg_dir}'. Deleting...", "info")
        user_dirs = find_pg_id(pg_dir, max_depth=3)
    else:
        user_dirs = []
    acc_id = []
    pattern = re.compile(r"user.*?(\d+)")
    for d in user_dirs:
        match = pattern.match(d.name)
        if match:
            try:
                acc_id.append(int(match.group(1)))
            except ValueError:
                continue
    if acc_id:
        acc_id_str = ", ".join(map(str, acc_id))
    else:
        log("Pixel Gun ID not found. Skipping...", "info")
    logs_cleared = 0
    if pg_dir and pg_dir.parent.exists():
        if clear_pg_logs(pg_dir):
            logs_cleared = 1
    else:
        log("Pixel Gun logs folder not found. Skipping...", "info")
    temp_deleted = clear_pg_temp()
    registry_cleared = 0
    base_key = winreg.HKEY_CURRENT_USER
    parent_reg_path = r"Software\Pixel Gun Team"
    subkey_patterns = ["pixel*gun*", "pg*"]
    matching_subkeys = set()
    for pattern in subkey_patterns:
        matching_subkeys.update(find_reg_subkeys(base_key, parent_reg_path, pattern))
    matching_subkeys = list(matching_subkeys)
    if matching_subkeys:
        total_matching = 0
        total_deleted = 0
        for subkey in matching_subkeys:
            matching, deleted = delete_reg_values(base_key, parent_reg_path + "\\" + subkey, WILDCARD_PATTERNS, REGEX_PATTERNS)
            total_matching += matching
            total_deleted += deleted
        if total_matching == 0:
            pass
        elif total_deleted == 0:
            log("No registry traces were deleted.", "warn")
        else:
            registry_cleared = 1
    else:
        log("Registry key not found. Skipping...", "info")
    completed = logs_cleared + (1 if temp_deleted > 0 else 0) + registry_cleared
    print()
    log(f"Found ID: {acc_id_str}", "info")
    input(f"[{completed} / 3] Cleanup completed. Press Enter to exit.")
if __name__ == "__main__":
    main()
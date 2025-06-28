# Pixel Gun Trace Cleaner - PGTC

Pixel Gun 3D/PG3D/2 Trace Cleaner is a script made for reducing ban chances when creating or playing on new PG accounts by cleaning local traces (i.e account traces, logs) left by the game.

---

## Features

- Detects and deletes PG's temp files and logs.
- Cleans registry entries contain account/session data.
- Finds and displays user IDs found in PG local data.

---

## How to Use

1. **Close PG** if it is running before starting PGTC.
2. Run the `PGTC.exe` (or `main.py` if running from source).

---

## Requirements

- Python 3.6+ (if running from source)
- Python packages: `colorama`, `psutil` (not needed with exe)

---

## Notes

- Always **close Pixel Gun** before running this script.
- Need to run as Administrator to fully clean traces.
- This script **only deletes local data**; it CANNOT unban your account.
- False positives may occur with some antivirus software due to registry and file operations.
- Join my Discord for support, guides etc.: https://discord.gg/GYSdwzQtD3


> Credit to YeetDisDude's BanTraceCleaner

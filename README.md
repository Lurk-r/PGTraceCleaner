# Pixel Gun Trace Cleaner - PGTC

PGTC is a simple script made for reducing ban chances when creating or playing on new PG accounts by cleaning local traces (i.e account/ban traces, logs) left by Pixel Gun 3D/PG3D (Pixel Gun 2/PG2?).

**[Download](https://github.com/Lurk-r/PG-Trace-Cleaner/releases/latest)**

---

## Features

- Detects and deletes PG's temp files and logs.
- Cleans registry entries contain account/ban traces.
- Finds and displays user IDs found in PG local data.
- Your game settings won't be reset.

---

## How to Use

1. **Close PG** if it is running before starting PGTC.
2. Run the `PGTC.exe` (or `main.py` if running from source).

---

## Requirements
> *running from source*

- Python 3.6+
- Python packages: `colorama`, `psutil`

---

## Notes

- Join my Discord for support, guides etc.: **https://discord.gg/GYSdwzQtD3**
- Always **close PG** before running this script.
- Need to run as Administrator to fully clean traces.
- This script **only deletes local data**; it CANNOT unban your account.
- False positives may occur with antivirus due to registry and file operations.


> Credit to YeetDisDude's [BanTraceCleaner](https://github.com/YeetDisDude/bantracecleaner)

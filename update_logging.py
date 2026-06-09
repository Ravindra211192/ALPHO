import re
import os

file_path = "PI5-Modbus-PIC-2026-05-05/PI_modbus_master.py"
with open(file_path, "r") as f:
    content = f.read()

# Add logging import and basic config
if "import logging" not in content:
    content = content.replace("import time\n", "import time\nimport logging\n\nlog_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'PI_modbus_master_error.log')\nlogging.basicConfig(level=logging.ERROR, format='%(asctime)s - %(levelname)s - %(message)s', filename=log_file_path)\n")

# Replace print with logging
lines = content.split('\n')
for i, line in enumerate(lines):
    if 'print(' in line:
        if any(keyword in line.lower() for keyword in ['error', 'exception', 'failed', 'cannot read']):
            lines[i] = line.replace('print(', 'logging.error(')
        else:
            lines[i] = line.replace('print(', 'logging.info(')

# Update copy_file_to_usb definition and logic
new_lines = []
in_copy_function = False
for line in lines:
    if line.startswith('def copy_file_to_usb(csv_path, excel_path):'):
        line = line.replace('def copy_file_to_usb(csv_path, excel_path):', 'def copy_file_to_usb(csv_path, excel_path, log_path=log_file_path):')
        in_copy_function = True
    elif in_copy_function and "dest_excel = os.path.join(mount, os.path.basename(excel_path))" in line:
        new_lines.append(line)
        new_lines.append(line.replace('dest_excel', 'dest_log').replace('excel_path', 'log_path'))
        continue
    elif in_copy_function and "logging.info(f\" [USB] copying {os.path.basename(excel_path)} -> {dest_excel}\")" in line:
        new_lines.append(line)
        new_lines.append("        if os.path.isfile(log_path):")
        new_lines.append("            logging.info(f\" [USB] copying {os.path.basename(log_path)} -> {dest_log}\")")
        continue
    elif in_copy_function and "shutil.copy2(excel_path, dest_excel)" in line:
        new_lines.append(line)
        new_lines.append("            if os.path.isfile(log_path):")
        new_lines.append("                shutil.copy2(log_path, dest_log)")
        continue
    elif in_copy_function and "logging.info(f\" [USB] Data synced to USB. It will stay mounted for the next test.\")" in line:
        in_copy_function = False

    new_lines.append(line)

with open(file_path, "w") as f:
    f.write('\n'.join(new_lines))


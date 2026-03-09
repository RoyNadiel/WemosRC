import re
import os

main_file = r"c:\Users\Roy Nadiel\Desktop\Projects\WemosRC\main.ino"
index_file = r"c:\Users\Roy Nadiel\Desktop\Projects\WemosRC\index.html"

with open(main_file, 'r', encoding='utf-8') as f:
    f_content = f.read()

m = re.search(r'R"=====\(\n(.*?)\n\)=====";', f_content, re.DOTALL)
if m:
    html_content = m.group(1)
    with open(index_file, 'w', encoding='utf-8') as f:
        f.write(html_content + "\n")
    print("SUCCESS: index.html has been generated.")
else:
    print("ERROR: Could not find the HTML block inside main.ino")

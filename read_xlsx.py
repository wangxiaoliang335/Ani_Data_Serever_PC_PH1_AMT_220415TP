# -*- coding: utf-8 -*-
import zipfile
import xml.etree.ElementTree as ET
import os

# Get xlsx file path using GBK encoding
base_path = r"E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP"
files = os.listdir(base_path)
xlsx_file = None
for f in files:
    if 'xlsx' in f and '数据库' in f:
        xlsx_file = f
        break

if not xlsx_file:
    print("XLSX file not found")
    exit(1)

xlsx_path = os.path.join(base_path, xlsx_file)
print(f"Reading: {xlsx_file}")

try:
    with zipfile.ZipFile(xlsx_path, 'r') as z:
        # Read shared strings
        shared_strings = []
        if 'xl/sharedStrings.xml' in z.namelist():
            xml_content = z.read('xl/sharedStrings.xml')
            root = ET.fromstring(xml_content)
            for si in root.findall('.//{http://schemas.openxmlformats.org/spreadsheetml/2006/main}si'):
                text = ''.join(t.text for t in si.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}t') if t.text)
                shared_strings.append(text)
        
        # Read sheet1
        xml_content = z.read('xl/worksheets/sheet1.xml')
        root = ET.fromstring(xml_content)
        
        ns = '{http://schemas.openxmlformats.org/spreadsheetml/2006/main}'
        
        output_lines = []
        for row in root.findall(f'.//{ns}row'):
            row_data = []
            for cell in row.findall(f'{ns}c'):
                cell_type = cell.get('t', '')
                value = ''
                v_elem = cell.find(f'{ns}v')
                if v_elem is not None and v_elem.text:
                    if cell_type == 's':
                        idx = int(v_elem.text)
                        value = shared_strings[idx] if idx < len(shared_strings) else ''
                    else:
                        value = v_elem.text
                row_data.append(value)
            output_lines.append('\t'.join(row_data))
        
        content = '\n'.join(output_lines)
        
        # Save to file
        output_path = os.path.join(base_path, "点灯检数据库说明_内容.txt")
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(content)
        
except FileNotFoundError:
    print(f"File not found: {xlsx_path}")
except Exception as e:
    print(f"Error: {e}")

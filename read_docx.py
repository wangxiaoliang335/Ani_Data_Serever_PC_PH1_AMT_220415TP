# -*- coding: utf-8 -*-
import zipfile
import xml.etree.ElementTree as ET
import sys

docx_path = r"E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\点灯检软件通信说明.docx"

try:
    with zipfile.ZipFile(docx_path, 'r') as z:
        xml_content = z.read('word/document.xml')
    
    # Parse XML
    root = ET.fromstring(xml_content)
    
    # Extract text
    text_content = []
    for elem in root.iter():
        if elem.tag.endswith('}t'):
            if elem.text:
                text_content.append(elem.text)
    
    full_text = '\n'.join(text_content)
    
    # Save to file with proper encoding
    output_path = r"E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\点灯检软件通信说明_内容.txt"
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(full_text)
    
    print(f"Content saved to: {output_path}")
    print("\n" + "="*50)
    print(full_text)
    
except FileNotFoundError:
    print(f"File not found: {docx_path}")
except Exception as e:
    print(f"Error: {e}")

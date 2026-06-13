import pandas as pd
import os

# Excel文件路径
excel_path = r"E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\点灯数据库表说明.xlsx"

# 输出文件夹
output_dir = r"E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\_extracted_docs"
os.makedirs(output_dir, exist_ok=True)

# 使用pandas读取Excel文件
xl = pd.ExcelFile(excel_path)

print(f"Sheet names: {xl.sheet_names}")

# 遍历所有sheet
for sheet_name in xl.sheet_names:
    print(f"\n=== Sheet: {sheet_name} ===")
    
    # 读取sheet数据
    df = xl.parse(sheet_name=sheet_name, dtype=str)
    
    # 显示前几行和列信息
    print(f"Columns: {list(df.columns)}")
    print(f"Shape: {df.shape}")
    print("\nFirst 5 rows:")
    print(df.head())
    
    # 保存为CSV
    out_file = os.path.join(output_dir, f"{sheet_name}.csv")
    df.to_csv(out_file, index=False, encoding="utf-8-sig")
    print(f"Saved to: {out_file}")

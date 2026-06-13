# Extract docx content
Add-Type -AssemblyName System.IO.Compression.FileSystem
$docxPath = "E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\点灯检软件通信说明.docx"
$zip = [System.IO.Compression.ZipFile]::OpenRead($docxPath)
$entry = $zip.Entries | Where-Object { $_.Name -eq 'document.xml' }
$stream = $entry.Open()
$reader = New-Object System.IO.StreamReader($stream, [System.Text.Encoding]::UTF8)
$content = $reader.ReadToEnd()
$reader.Close()
$zip.Dispose()
$content | Out-File -FilePath "E:\CSOT_PH1_AFT151_CAMERA\AMT 4# Data Server软件\Ani_Data_Serever_PC_PH1_AMT_220415TP\_extracted_docs\document.xml" -Encoding UTF8
Write-Output "Done"

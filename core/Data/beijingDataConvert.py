# import json

# import xml.etree.ElementTree as ET

# def parse_xml(file):
#     # 解析XML文件
#     tree = ET.parse(file)
#     root = tree.getroot()

#     # 创建一个空字典来存储结果
#     lines = {}

#     i=0
#     # 遍历所有的'l'标签
#     for l in root.iter('l'):
#         print(l)
#         lb = l.get('lb')
        
#         i+=1
#         # 将'i'补零到三位
#         key = str(i).zfill(3)

#         # 将'i'和'lb'添加到字典中
#         lines[key] = lb

#     return lines

# lines = parse_xml('beijing.xml')
# print(lines)
# # Convert the dictionary to JSON string
# json_data = json.dumps(lines)

# # Write the JSON string to a file
# with open('beijing.json', 'w') as f:
#     f.write(json_data)


import json
import xml.etree.ElementTree as ET


# Read the JSON file
with open('BeiJingSubway.json', 'r', encoding='utf-8') as f:
    json_data = f.read()

# Parse the JSON data
json = json.loads(json_data)
# print(json)

# Access the dictionary containing line numbers and line names
lines = json['lines']

# Print the lines dictionary
# print(lines)


# 解析XML文件
tree = ET.parse('beijing.xml')
root = tree.getroot()

# 创建一个空字典来存储结果
sites = {}

def find_key(lines, lineName):
    # 遍历字典
    for key, value in lines.items():
        # 如果value等于lineName，返回key
        if value == lineName:
            return key

    # 如果没有找到，返回None
    return None

# 遍历所有的'l'标签
for l in root.iter('l'):
    # 遍历'l'标签下的所有'p'标签
    lineName = l.get('lb')
    lineIndex=find_key(lines, lineName)
    print(lineIndex)
    for p in l.iter('p'):
        # 获取'p'标签的'n'和'lb'属性
        siteIndex = p.get('n')
        siteName = p.get('lb')

        # 如果'lb'属性不存在或为空，跳过这个'p'标签
        if not siteName:
            continue

        Index=lineIndex+siteIndex.zfill(3)

        sites[Index] = siteName

print(sites)

# Convert the dictionary to JSON string
json_data = json.dumps(sites)

# Write the JSON string to a file
with open('beijingsite.json', 'w') as f:
    f.write(json_data)

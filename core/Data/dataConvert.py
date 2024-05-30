import json

# Open the JSON file
with open('./ChengDuSubway.json', 'r', encoding='utf-8') as file:
    data = json.load(file)

"""
{
    "sites": {
        "001001": "韦家碾",
        "001002": "升仙湖",
        "001003": "火车北站",
        "001004": "人民北路",
        "001005": "文殊院",
        "001006": "骡马市",
        "001007": "天府广场",
        "001008": "锦江宾馆",
        "001009": "华西坝",
        "001010": "省体育馆",
        "001011": "倪家桥",
"""
# 以上是json的格式，需要将其转换为字典格式
# Convert the JSON data to a dictionary
data_dict = {}
for key, value in data["sites"].items():
    data_dict[key] = value

# Print the converted dictionary
# print(data_dict)

table = []
for key, value in data_dict.items():
    # print(key,value)
    dictNew={}
    dictNew["node"]=key
    dictNew["guiCoord"]=[-1,-1]
    
    # 检测 key-1，key+1 是否在data_dict内存在，如果存在，则按照下述格式构造 connectNode[]
    '''
    "connectNode": [
                {
                    "node": "001002",
                    "distance": 1
                }
            ]
    '''
    connectNode = []
    if str(int(key)-1).zfill(6) in data_dict:
        connectNode.append({
            "node": str(int(key)-1).zfill(6),
            "distance": 1
        })
    if str(int(key)+1).zfill(6) in data_dict:
        connectNode.append({
            "node": str(int(key)+1).zfill(6),
            "distance": 1
        })

    # 继续判断 data_dict{}内有没有同value的项，如果有，继续添加到connectNode[]中，此时distance=3
    for k, v in data_dict.items():
        if v == value and k != key:
            connectNode.append({
                "node": k,
                "distance": 3
            })

    dictNew["connectNode"] = connectNode
    # print(dictNew)
    table.append(dictNew)

print(table)
# Save the converted data to a new JSON file
with open('./ChengDuSubwayNew.json', 'w', encoding='utf-8') as file:
    json.dump(table, file, ensure_ascii=False, indent=4)

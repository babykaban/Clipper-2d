import json

def printPoly(file, index):
    with open(file, 'r') as f:
        data = json.load(f)

    polygon_pair = data['polygons'][index]
    points1 = polygon_pair['points']['subject']
    points2 = polygon_pair['points']['clip']
    
    print("Subject: ")
    for point in points1:
        print(f"({point[0]}, {point[1]}), ")
    
    print("Clip: ")
    for point in points2:
        print(f"({point[0]}, {point[1]}), ")

#printPoly('d:/Clipper-2d/output/polygons.json', 131233)
printPoly('c:/Paul/Clipper-2d/output/polygons.json', 131232)
line16 = "B,F,C,F,C,F,B,,5B,F,C,F,C,F,C,F,C,F,C,F,C,F,C,F,B,F,C,F,B,F,C,B,B,GO,GO,GO,GO,GO,GO,GO,B,B,B,B,W,B,GO,BR,BR,BR,B,BR,BR,B,F,F,B,O1,O1,O1,B,B,GO1,GO,B,B,O,B,F,F,F,B,BR,B,W,B,B,B,B,W,W,B,F,C,F,C,F,C,F,C,B,F,C,F,C,F,C,B"

line17 = "B,C,F,C,F,C,F,B,,4B,C,F,C,F,C,F,C,F,C,F,C,F,C,F,C,B,C,F,C,F,B,F,C,B,B,B,B,BR,BR,BR,BR,GO,GO,GO,GO,B,B,GO,BR,BR,BR,B,BR1,BR1,BR1,B,B,O1,B,B,B,GO,GO1,GO,GO1,GO1,B,O1,O,B,F,B,BR,B,BR1,B,G,W,W,W,B,W,B,C,F,C,F,C,F,C,B,F,C,F,C,F,C,B"

line18 = "B,C,F,C,F,C,F,B,,4B,C,F,C,F,C,F,C,F,C,F,C,F,C,F,B,F,C,F,C,B,C,F,C,B,C,F,B,BR,BR1,BR1,BR1,BR1,BR1,BR1,GO,BR,GO,BR,BR,BR,BR,B,BR1,BR1,B,B,B,GO,GO1,B,GO1,GO,GO,GO,GO,B,B,O,B,F,B,BR,B,BR1,B,G,G,B,W,B,B,B,F,C,F,C,F,C,B,F,C,F,C,F,C,B,B"

def main():
    char_to_color = {
        'F': "Firefly",
        'C': "cha",
        'B': "Black",
        'W': "White",
        'G': "Gray",
        'O': "Orange",
        'O1': "Orange1",
        'BR': "Brown",
        'BR1': "Brown1",
        'GO': "Gold",
        'GO1': "Gold1",
        '': "",
    }

    input_string = line18

    characters = input_string.split(',')
    optimized_output = []
    i = 0

    while i < len(characters):
        char = characters[i].strip()  # 当前字符

        if not char:  # 跳过空字符
            print('\n')
            i += 1
            continue

        number_part = ""
        color_part = ""

        # 提取数字和颜色部分
        for j, c in enumerate(char):
            if not c.isdigit():
                number_part = char[:j]
                color_part = char[j:]
                break
        else:
            color_part = char

        if number_part.isdigit():  # 如果存在数字部分，处理数量逻辑
            count = int(number_part)
            color = char_to_color.get(color_part, "Undefined")
            optimized_output.append(f"DrawCube(TransHEXtoVec3(Color_{color}), 1.0f, nowDrawingPoint += {count}.0f * Rightward);")
            i += 1
            continue

        count = 1  # 记录重复次数

        # 检查单色连续重复
        while i + count < len(characters) and characters[i + count].strip() == char:
            count += 1

        if count >= 2:  # 如果单色连续重复>=2次
            color = char_to_color.get(color_part, "Undefined")
            optimized_output.append(f"for (int i = 0; i < {count}; i++) {{ DrawCube(TransHEXtoVec3(Color_{color}), 1.0f, nowDrawingPoint += 1.0f * Rightward); }}")
            i += count
            continue

        # 检查两个颜色的组合重复
        if i + 1 < len(characters):
            next_char = characters[i + 1].strip()
            pair = [char, next_char]
            pair_count = 1

            while i + 2 * pair_count < len(characters) and characters[i + 2 * pair_count: i + 2 * pair_count + 2] == pair:
                pair_count += 1

            if pair_count >= 3:  # 如果两个颜色组合重复>=3次
                color1 = char_to_color.get(pair[0], "Undefined")
                color2 = char_to_color.get(pair[1], "Undefined")
                optimized_output.append(f"for (int i = 0; i < {pair_count}; i++) {{ \nDrawCube(TransHEXtoVec3(Color_{color1}), 1.0f, nowDrawingPoint += 1.0f * Rightward); \nDrawCube(TransHEXtoVec3(Color_{color2}), 1.0f, nowDrawingPoint += 1.0f * Rightward); \n}}")
                i += 2 * pair_count
                continue

        # 默认处理单个字符
        color = char_to_color.get(color_part, "Undefined")
        optimized_output.append(f"DrawCube(TransHEXtoVec3(Color_{color}), 1.0f, nowDrawingPoint += 1.0f * Rightward);")
        i += 1

    # 输出优化后的结果
    for line in optimized_output:
        print(line)

if __name__ == "__main__":
    main()




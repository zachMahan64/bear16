import math
name = "math_sint"
print(f"{name}_start:")
for i in range(0, 1570):
    x = i * 0.0039
    print(f".word {x:.3f}, {math.sin(x):.3f}        ", end="")
    if i % 8 == 7:
        print("")
print(f"\n{name}_end:")
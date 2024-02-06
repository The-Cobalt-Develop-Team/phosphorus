import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv("./result1.csv", encoding="utf-8")

# print(df["x"].tolist())
# print(df["y"].tolist())
x = df["x"].tolist()
y = df["y"].tolist()

plt.figure(figsize=(10, 10))
plt.plot(x, y, label="Movement", color="red", linewidth=2)

plt.xlabel("y")
plt.ylabel("x")

plt.title("Movement Graph")
# plt.ylim(0, 0.4)
# plt.xlim(-0.2, 0.2)
plt.legend()
plt.show()

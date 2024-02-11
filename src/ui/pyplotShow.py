# The main implementation of the Phosphorus Engine
# Copyright (C) 2024  Andy Shen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

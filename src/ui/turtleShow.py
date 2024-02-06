import turtle
import pandas as pd
import time

df = pd.read_csv("./result1.csv", encoding="utf-8")

# print(df["x"].tolist())
# print(df["y"].tolist())
x = df["x"].tolist()
y = df["y"].tolist()

l = len(x)
t = turtle.Pen()
t.penup()
t.goto(-400, 0)
while True:
    t.penup()
    t.goto(-400, 0)
    for i in range(0, l):
        t.pendown()
        t.goto(x[i] * 10000 - 400, y[i] * 10000)
        time.sleep(0.1)
    t.clear()
    time.sleep(1)
